#include <string>
#include <cstring>
#include <queue>
#include <stack>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <gmp.h>
#include <mpfr.h>

#include "logger.h"
#include "calc_error.h"
#include "tokenizer.h"
#include "token.h"
#include "utils.h"
#include "calculator.h"

using namespace std;

#define LIST_SIZE                   128
#define STACK_SIZE                   64

#define CONSTANT_C            299792458U
#define CONSTANT_G                  "0.000000000066743"

static stack<token_t *>             tokenStack;
static queue<token_t *>             tokenQueue;

static operand_t                    memory[10];

static void _convertToRPN(tokenizer_t * tokenizer) {
    while (tzrHasMoreTokens(tokenizer)) {
        token_t * t = tzrNextToken(tokenizer);
        
        /*
        ** If the token is a number, then push it to the output queue.
        */
        if (t->getType() == token_operand) {
            tokenQueue.push(t);
        }
        else if (t->getType() == token_constant) {
            tokenQueue.push(t);
        }
        /*
        ** If the token is a function token, then push it onto the stack.
        */
        else if (t->getType() == token_function) {
            tokenStack.push(t);
        }
        /*
        ** If the token is an operator, operand->value, then:
        **	while there is an operator token o2, at the top of the operator stack
        **	and either
        **	o1 is left-associative and its precedence is less than or equal to that
        **	of o2, or
        **	o1 is right associative, and has precedence less than that of o2,
        **	pop o2 off the operator stack, onto the output queue;
        **	at the end of iteration push o1 onto the operator stack.
        */
        else if (t->getType() == token_operator) {
            while (!tokenStack.empty()) {
                operator_t * op = (operator_t *)t;

                token_t * topToken;
    
                topToken = tokenStack.top();

                if (topToken->getType() != token_operator && topToken->getType() != token_function) {
                    break;
                }
                else {
                    operator_t * topOp = (operator_t *)topToken;

                    if ((op->getAssociativity() == operator_t::left && op->getPrescedence() <= topOp->getPrescedence()) ||
                        (op->getAssociativity() == operator_t::right && op->getPrescedence() < topOp->getPrescedence()))
                    {
                        token_t * op2;

                        op2 = tokenStack.top();
                        tokenStack.pop();
                        tokenQueue.push(op2);
                    }
                    else {
                        break;
                    }
                }
            }

            tokenStack.push(t);
        }
        else if (t->getType() == token_brace) {
            brace_t * brace = (brace_t *)t;

            /*
            ** If the token is a left parenthesis (i.e. "("), then push it onto the stack.
            */
            if (brace->isBraceLeft()) {
                tokenStack.push(t);
            }
            else {
                /*
                If the token is a right parenthesis (i.e. ")"):
                    Until the token at the top of the stack is a left parenthesis, pop
                    operators off the stack onto the output queue.
                    Pop the left parenthesis from the stack, but not onto the output queue.
                    If the token at the top of the stack is a function token, pop it onto
                    the output queue.
                    If the stack runs out without finding a left parenthesis, then there
                    are mismatched parentheses.
                */
                bool foundLeftParenthesis = false;

                while (!tokenStack.empty() && !foundLeftParenthesis) {
                    token_t * stackToken;

                    stackToken = tokenStack.top();
                    tokenStack.pop();

                    if (stackToken->getType() == token_brace) {
                        brace_t * b = (brace_t *)stackToken;

                        if (b->isBraceLeft()) {
                            foundLeftParenthesis = true;
                        }
                        else {
                            tokenQueue.push(t);
                        }
                    }
                    else {
                        tokenQueue.push(t);
                    }
                }

                if (!foundLeftParenthesis) {
                    /*
                    ** If we've got here, we must have unmatched parenthesis...
                    */
                    throw unmatched_parenthesis_error("_convertToRPN()", __FILE__, __LINE__);
                }
            }
        }
    }

    lgLogDebug("Num items in stack %d", (int)tokenStack.size());

    /*
        While there are still operator tokens in the stack:
        If the operator token on the top of the stack is a parenthesis,
        then there are mismatched parentheses.
        Pop the operator onto the output queue.
    */
    while (!tokenStack.empty()) {
        token_t * stackToken;

        stackToken = tokenStack.top();
        tokenStack.pop();

        if (stackToken == NULL) {
            lgLogError("NULL item on stack");
            throw stack_error("NULL item on stack", __FILE__, __LINE__);
        }

        if (stackToken->getType() == token_brace) {
            /*
            ** If we've got here, we must have unmatched parenthesis...
            */
            throw unmatched_parenthesis_error("_convertToRPN()", __FILE__, __LINE__);
        }
        else {
            tokenQueue.push(stackToken);
        }
    }
}

void memoryStore(operand_t * operand, int memoryLocation) {
    if (memoryLocation < 0 || memoryLocation > 9) {
        throw calc_error("Memory index out-of-range", __FILE__, __LINE__);
    }

    //memory[memoryLocation].
    memcpy(&memory[memoryLocation], operand, sizeof(operand_t));
}

operand_t * memoryFetch(int memoryLocation) {
    if (memoryLocation < 0 || memoryLocation > 9) {
        throw calc_error("Memory index out-of-range", __FILE__, __LINE__);
    }

    return &memory[memoryLocation];
}

operand_t * evaluate(const char * pszExpression) {
    tokenizer_t             tokenizer;

    tzrInit(&tokenizer, pszExpression, getBase());

    /*
    ** Convert the calculation in infix notation to the postfix notation
    ** (Reverse Polish Notation) using the 'shunting yard algorithm'...
    */
    _convertToRPN(&tokenizer);

    lgLogDebug("num items in queue = %d", tokenQueue.size());

    while (!tokenQueue.empty()) {
        token_t * t;

        t = tokenQueue.front();
        tokenQueue.pop();

        if (t->getType() == token_operand) {
            lgLogDebug("Got operand: '%s'", t->getTokenStr().c_str());
            tokenStack.push(t);
        }
        else if (t->getType() == token_constant) {
            lgLogDebug("Got constant: '%s'", t->getTokenStr().c_str());

            constant_t * c = (constant_t *)t;

            operand_t * result = c->evaluate();

            tokenStack.push(result);
        }
        /*
        ** Must be Operator or Function...
        */
        else if (t->getType() == token_function) {
            operand_t * o1;

            o1 = (operand_t *)tokenStack.top();
            tokenStack.pop();

            if (o1 == NULL) {
                lgLogError("NULL operand for function '%s'", t->getTokenStr().c_str());
                delete t;
                tzrFinish(&tokenizer);
                throw stack_error("NULL item on stack", __FILE__, __LINE__);
            }

            function_t * f = (function_t *)t;

            operand_t * result = f->evaluate(o1);

            tokenStack.push(result);
        }
        else if (t->getType() == token_operator) {
            operand_t * o1;
            operand_t * o2;

            o2 = (operand_t *)tokenStack.top();
            tokenStack.pop();

            o1 = (operand_t *)tokenStack.top();
            tokenStack.pop();

            if (o1 == NULL || o2 == NULL) {
                lgLogError("NULL operand for operator '%s'", t->getTokenStr().c_str());
                delete t;
                tzrFinish(&tokenizer);
                throw stack_error("NULL item on stack", __FILE__, __LINE__);
            }

            operator_t * op = (operator_t *)t;

            operand_t * result = op->evaluate(o1, o2);

            tokenStack.push(result);
        }

        delete t;
    }

    /*
    ** If there is one and only one item left on the stack,
    ** it is the result of the calculation. Otherwise, we
    ** have too many tokens and therefore an error...
    */
    if (tokenStack.size() == 1) {
        operand_t * result = (operand_t *)tokenStack.top();
        tokenStack.pop();
        return result;
    }
    else {
        lgLogError("evaluate(): Got invalid items on stack!");

        while (!tokenStack.empty()) {
            token_t * tok = tokenStack.top();
            tokenStack.pop();
            lgLogError("Invalid item on stack '%s'\n", tok->getTokenStr().c_str());
        }

        tzrFinish(&tokenizer);
        throw stack_error("Invalid items on stack", __FILE__, __LINE__);
    }

    tzrFinish(&tokenizer);
}

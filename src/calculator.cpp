#include <string>
#include <cstring>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <gmp.h>
#include <mpfr.h>

#include "logger.h"
#include "calc_error.h"
#include "system.h"
#include "tokenizer.h"
#include "questack.h"
#include "operator.h"
#include "function.h"
#include "constant.h"
#include "timeutils.h"
#include "version.h"
#include "test.h"
#include "calculator.h"

using namespace std;

#define LIST_SIZE                   128
#define STACK_SIZE                   64

static vector<string>               stats;

static int getPrescedence(string & token) {
    if (Utils::isOperator(token[0])) {
        return Operator::getPrescedence(token);
    }
    else if (Utils::isFunction(token)) {
        return Function::getPrescedence();
    }

    return 0;
}

static associativity getAssociativity(string & token) {
    if (Utils::isOperator(token[0])) {
        return Operator::getAssociativity(token);
    }
    else if (Utils::isFunction(token)) {
        return Function::getAssociativity();
    }

    return LEFT;
}

static void _convertToRPN(tokenizer_t * tokenizer, Queue & tokenQueue) {
    Stack operatorStack;

    while (tzrHasMoreTokens(tokenizer)) {
        string token = tzrNextToken(tokenizer);
        
        /*
        ** If the token is a number, then push it to the output queue.
        */
        if (Utils::isOperand(token)) {
            tokenQueue.put(token);
        }
        else if (Utils::isConstant(token)) {
            tokenQueue.put(token);
        }
        /*
        ** If the token is a function token, then push it onto the stack.
        */
        else if (Utils::isFunction(token)) {
            operatorStack.push(token);
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
        else if (Utils::isOperator(token[0])) {
            string o1 = token;

            while (!operatorStack.isEmpty()) {
                string topToken = operatorStack.peek();

                if (!Utils::isOperator(topToken[0]) && !Utils::isFunction(topToken)) {
                    break;
                }

                string o2 = topToken;

                if ((getAssociativity(o1) == LEFT && getPrescedence(o1) <= getPrescedence(o2)) ||
                    (getAssociativity(o1) == RIGHT && getPrescedence(o1) < getPrescedence(o2)))
                {
                    tokenQueue.put(operatorStack.pop());
                }
                else {
                    break;
                }
            }

            operatorStack.push(token);
        }
        else if (Utils::isBrace(token[0])) {
            /*
            ** If the token is a left parenthesis (i.e. "("), then push it onto the stack.
            */
            if (Utils::isLeftBrace(token[0])) {
                operatorStack.push(token);
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

                while (!operatorStack.isEmpty()) {
                    string stackToken = operatorStack.pop();

                    if (Utils::isBrace(stackToken[0])) {
                        if (Utils::isLeftBrace(stackToken[0])) {
                            foundLeftParenthesis = true;
                            break;
                        }
                    }
                    else {
                        tokenQueue.put(stackToken);
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
        else {
            throw invalid_token_error(
                        calc_error::buildMsg(
                                    "_convertToRPN(): Invalid token: %s", 
                                    token.c_str()), 
                        __FILE__, 
                        __LINE__);
        }
    }

    lgLogDebug("Num items in stack %d", (int)operatorStack.size());

    /*
        While there are still operator tokens in the stack:
        If the operator token on the top of the stack is a parenthesis,
        then there are mismatched parentheses.
        Pop the operator onto the output queue.
    */
    while (!operatorStack.isEmpty()) {
        string stackToken = operatorStack.pop();

        if (Utils::isBrace(stackToken[0])) {
            /*
            ** If we've got here, we must have unmatched parenthesis...
            */
            throw unmatched_parenthesis_error("_convertToRPN()", __FILE__, __LINE__);
        }
        else {
            tokenQueue.put(stackToken);
        }
    }
}

void evaluate(mpfr_t result, const char * pszExpression, int radix) {
    tokenizer_t             tokenizer;
    Queue                   tokenQueue;

    tzrInit(&tokenizer, pszExpression, radix);

    /*
    ** Convert the calculation in infix notation to the postfix notation
    ** (Reverse Polish Notation) using the 'shunting yard algorithm'...
    */
    _convertToRPN(&tokenizer, tokenQueue);

    lgLogDebug("num items in queue = %d", tokenQueue.size());

    Stack tokenStack;

    while (!tokenQueue.isEmpty()) {
        string t = tokenQueue.get();

        if (Utils::isOperand(t)) {
            lgLogDebug("Got operand: '%s'", t.c_str());
            tokenStack.push(t);
        }
        else if (Utils::isConstant(t)) {
            lgLogDebug("Got constant: '%s'", t.c_str());

            tokenStack.push(Constant::evaluate(t));
        }
        else if (Utils::isFunction(t)) {
            lgLogDebug("Got function: '%s'", t.c_str());

            string o1 = tokenStack.pop();

            tokenStack.push(Function::evaluate(t, radix, o1));
        }
        else if (Utils::isOperator(t[0])) {
            lgLogDebug("Got operator: '%s'", t.c_str());

            string o2 = tokenStack.pop();
            string o1 = tokenStack.pop();

            tokenStack.push(Operator::evaluate(t, radix, o1, o2));
        }
    }

    /*
    ** If there is one and only one item left on the stack,
    ** it is the result of the calculation. Otherwise, we
    ** have too many tokens and therefore an error...
    */
    if (tokenStack.size() == 1) {
        string answer = tokenStack.pop();

        lgLogDebug("Answer = %s", answer.c_str());

        mpfr_set_str(result, answer.c_str(), radix, MPFR_RNDA);
    }
    else {
        lgLogError("evaluate(): Got invalid items on stack!");

        while (!tokenStack.isEmpty()) {
            string tok = tokenStack.pop();
            lgLogError("Invalid item on stack '%s'\n", tok.c_str());
        }

        tzrFinish(&tokenizer);
        throw stack_error("Invalid items on stack", __FILE__, __LINE__);
    }

    tzrFinish(&tokenizer);
}

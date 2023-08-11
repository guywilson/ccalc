#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "que.h"
#include "stack.h"

static void _convertToRPN(char * pszExpression, que_handle_t * q) {
    stack_handle_t          operatorStack;

    stackInit(&operator);
    Stack<Token> operatorStack = new Stack<>();

    CalcTokenizer tok = new CalcTokenizer(expression, this.sys.getBase());

    while (tok.hasMoreTokens()) {
        Token t = tok.nextToken();

        /*
        ** If the token is a number, then push it to the output queue.
        */
        if (t.isOperand()) {
            outputQueue.add(t);
        }
        /*
        ** If the token is a function token, then push it onto the stack.
        */
        else if (t.isFunction()) {
            operatorStack.push(t);
        }
        /*
        ** If the token is an operator, o1, then:
        **	while there is an operator token o2, at the top of the operator stack
        **	and either
        **	o1 is left-associative and its precedence is less than or equal to that
        **	of o2, or
        **	o1 is right associative, and has precedence less than that of o2,
        **	pop o2 off the operator stack, onto the output queue;
        **	at the end of iteration push o1 onto the operator stack.
        */
        else if (t.isOperator()) {
            Operator o1 = (Operator)t;

            while (!operatorStack.empty()) {
                Token topToken = operatorStack.peek();

                if (!topToken.isOperator()) {
                    break;
                }

                Operator o2 = (Operator)topToken;

                if ((o1.getAssociativity() == Associativity.Left && o1.getPrecedence() <= o2.getPrecedence()) ||
                    (o1.getAssociativity() == Associativity.Right && o1.getPrecedence() < o2.getPrecedence()))
                {
                    o2 = (Operator)operatorStack.pop();
                    outputQueue.add(o2);
                }
                else {
                    break;
                }
            }

            operatorStack.push(o1);
        }
        else if (t.isBrace()) {
            Brace br = (Brace)t;

            /*
            ** If the token is a left parenthesis (i.e. "("), then push it onto the stack.
            */
            if (br.getType() == BraceType.Open) {
                operatorStack.push(br);
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
                boolean foundLeftParenthesis = false;

                while (!operatorStack.empty()) {
                    Token stackToken = operatorStack.pop();

                    if (stackToken.isBrace()) {
                        Brace brace = (Brace)stackToken;

                        if (brace.getType() == BraceType.Open) {
                            foundLeftParenthesis = true;
                            break;
                        }
                    }
                    else {
                        outputQueue.add(stackToken);
                    }
                }

                if (!foundLeftParenthesis) {
                    /*
                    ** If we've got here, we must have unmatched parenthesis...
                    */
                    throw new Exception("Failed to find left parenthesis on operator stack");
                }
            }
        }
    }

    /*
    While there are still operator tokens in the stack:
        If the operator token on the top of the stack is a parenthesis,
        then there are mismatched parentheses.
        Pop the operator onto the output queue.
    */
    while (!operatorStack.empty()) {
        Token stackToken = operatorStack.pop();

        if (stackToken.isBrace()) {
            /*
            ** If we've got here, we must have unmatched parenthesis...
            */
            throw new Exception("Found too many parenthesis on operator stack");
        }
        else {
            outputQueue.add(stackToken);
        }
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <gmp.h>
#include <mpfr.h>

#include "tokenizer.h"
#include "list.h"

#define LIST_SIZE                   128

static int _convertToRPN(char * pszExpression, que_handle_t * q, tokenizer_t * tokenizer) {
    stack_handle_t          operatorStack;

    stackInit(&operatorStack, LIST_SIZE);

    while (tzrHasMoreTokens(tokenizer)) {
        token_t * t = tzrNextToken(tokenizer);

        list_item_t tokenItem;

        tokenItem.item = t;
        tokenItem.itemLength = sizeof(token_t);

        /*
        ** If the token is a number, then push it to the output queue.
        */
        if (isOperand(tokenizer, t)) {
            qPutItem(q, tokenItem);
        }
        /*
        ** If the token is a function token, then push it onto the stack.
        */
        else if (isFunction(t)) {
            stackPush(&operatorStack, tokenItem);
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
        else if (isOperator(t)) {
            while (stackGetNumItems(&operatorStack)) {
                token_t topToken;
                
                stackPeek(&operatorStack, &topToken);

                if (!isOperator(&topToken)) {
                    break;
                }
                else {
                    if ((getOperatorAssociativity(t) == associativity_left && getOperatorPrescedense(t) <= getOperatorPrescedense(&topToken)) ||
                        (getOperatorAssociativity(t) == associativity_right && getOperatorPrescedense(t) < getOperatorPrescedense(&topToken)))
                    {
                        list_item_t op2;

                        stackPop(&operatorStack, &op2);
                        qPutItem(q, op2);
                    }
                    else {
                        break;
                    }
                }
            }

            stackPush(&operatorStack, tokenItem);
        }
        else if (isBrace(t)) {
            /*
            ** If the token is a left parenthesis (i.e. "("), then push it onto the stack.
            */
            if (isBraceLeft(t)) {
                stackPush(&operatorStack, tokenItem);
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

                while (stackGetNumItems(&operatorStack)) {
                    list_item_t stackToken;

                    stackPop(&operatorStack, &stackToken);

                    if (isBraceLeft((token_t *)stackToken.item)) {
                        foundLeftParenthesis = true;
                    }
                    else {
                        qPutItem(q, stackToken);
                    }
                }

                if (!foundLeftParenthesis) {
                    /*
                    ** If we've got here, we must have unmatched parenthesis...
                    */
                    stackDestroy(&operatorStack);
                    return -1;
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
    while (stackGetNumItems(&operatorStack)) {
        list_item_t stackToken;

        stackPop(&operatorStack, &stackToken);

        if (isBrace((token_t *)stackToken.item)) {
            /*
            ** If we've got here, we must have unmatched parenthesis...
            */
            stackDestroy(&operatorStack);
            return -1;
        }
        else {
            qPutItem(q, stackToken);
        }
    }

    stackDestroy(&operatorStack);

    return 0;
}

static int evaluateOperation(token_t * operation, token_t * operand1, token_t * operand2) {
    return 0;
}

static int evaluateFunction(token_t * function, token_t * operand) {
    return 0;
}

int evaluate(char * pszExpression, mpfr_t * result, int base) {
    tokenizer_t             tokenizer;
    que_handle_t            outputQueue;
    stack_handle_t          stack;

    qInit(&outputQueue, LIST_SIZE);
    stackInit(&stack, LIST_SIZE);

    tzrInit(&tokenizer, pszExpression, base);

    /*
    ** Convert the calculation in infix notation to the postfix notation
    ** (Reverse Polish Notation) using the 'shunting yard algorithm'...
    */
    _convertToRPN(pszExpression, &outputQueue, &tokenizer);

    while (qGetNumItems(&outputQueue)) {
        list_item_t queueItem;
        token_t *   t = queueItem.item;

        qGetItem(&outputQueue, &queueItem);

        if (isOperand(&tokenizer, t)) {
            stackPush(&stack, queueItem);
        }
        /*
        ** Must be Operator or Function...
        */
        else {
            if (isOperator(t)) {
                list_item_t o1;
                list_item_t o2;

                stackPop(&stack, &o2);
                stackPop(&stack, &o1);

                evaluateOperation(t, (token_t *)o1.item, (token_t *)o2.item);

                // stackPush(&stack, &result);
            }
            else {
                list_item_t o1;

                stackPop(&stack, &o1);

                evaluateFunction(t, (token_t *)o1.item);

                // stackPush(&stack, &result);
            }
        }
    }

    /*
    ** If there is one and only one item left on the stack,
    ** it is the result of the calculation. Otherwise, we
    ** have too many tokens and therefore an error...
    */
    if (stackGetNumItems(&stack) == 1) {
        //stackPop(&stack, &result);
    }
    else {
        tzrFinish(&tokenizer);
        return -1;
    }

    tzrFinish(&tokenizer);
    
    return 0;
}

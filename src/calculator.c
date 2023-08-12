#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "tokenizer.h"
#include "list.h"

static int _convertToRPN(char * pszExpression, que_handle_t * q, int base) {
    stack_handle_t          operatorStack;
    tokenizer_t             tokenizer;

    stackInit(&operatorStack, 80);

    tzrInit(&tokenizer, pszExpression, base);

    while (tzrHasMoreTokens(&tokenizer)) {
        token_t * t = tzrNextToken(&tokenizer);

        list_item_t tokenItem;

        tokenItem.item = t;
        tokenItem.itemLength = sizeof(token_t);

        /*
        ** If the token is a number, then push it to the output queue.
        */
        if (isOperand(&tokenizer, t)) {
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
           return -1;
        }
        else {
            qPutItem(q, stackToken);
        }
    }
}
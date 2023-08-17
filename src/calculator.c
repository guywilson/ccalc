#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <gmp.h>
#include <mpfr.h>

#include "tokenizer.h"
#include "utils.h"
#include "calculator.h"

#define LIST_SIZE                   128
#define STACK_SIZE                   64

static token_t                      tokenStack[STACK_SIZE];
static token_t                      tokenQueue[LIST_SIZE];

static int                          stackPointer = 0;
static int                          queueHead = 0;
static int                          queueTail = 0;
static int                          queueSize = 0;

static token_t                      memory[10];

const mpfr_prec_t                   basePrecision = 128L;

static void stackInit(void) {
    stackPointer = 0;

    memset(tokenStack, 0, STACK_SIZE);
}

static void stackPush(token_t * token) {
    memcpy(&tokenStack[stackPointer], token, sizeof(token_t));
    tokenStack[stackPointer].pszToken = strndup(token->pszToken, token->length);
    stackPointer++;
}

static token_t * stackPop(void) {
    stackPointer--;
    return &tokenStack[stackPointer];
}

static token_t * stackPeek(void) {
    return &tokenStack[stackPointer - 1];
}

static void queueInit(void) {
    queueHead = 0;
    queueTail = 0;
    queueSize = 0;

    memset(tokenQueue, 0, LIST_SIZE);
}

static void queuePut(token_t * token) {
    memcpy(&tokenQueue[queueTail], token, sizeof(token_t));
    tokenQueue[queueTail].pszToken = strndup(token->pszToken, token->length);
    queueTail++;
    queueSize++;
}

static token_t * queueGet(void) {
    queueSize--;
    return &tokenQueue[queueHead++];
}

static int getQueueSize(void) {
    return queueSize;
}

static int _convertToRPN(tokenizer_t * tokenizer) {
    while (tzrHasMoreTokens(tokenizer)) {
        token_t * t = tzrNextToken(tokenizer);
        
        /*
        ** If the token is a number, then push it to the output queue.
        */
        if (isOperand(tokenizer, t)) {
            printf("Got operand '%s'\n", t->pszToken);
            queuePut(t);
        }
        /*
        ** If the token is a function token, then push it onto the stack.
        */
        else if (isFunction(t)) {
            printf("Got function '%s'\n", t->pszToken);
            stackPush(t);
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
            printf("Got operator '%s'\n", t->pszToken);

            while (stackPointer > 0) {
                token_t *       topToken;
    
                topToken = stackPeek();

                if (!isOperator(topToken) && !isFunction(topToken)) {
                    break;
                }
                else {
                    if ((getOperatorAssociativity(t) == associativity_left && getOperatorPrecedence(t) <= getOperatorPrecedence(topToken)) ||
                        (getOperatorAssociativity(t) == associativity_right && getOperatorPrecedence(t) < getOperatorPrecedence(topToken)))
                    {
                        token_t * op2;

                        op2 = stackPop();
                        queuePut(op2);
                    }
                    else {
                        break;
                    }
                }
            }

            stackPush(t);
        }
        else if (isBrace(t)) {
            printf("Got brace '%s'\n", t->pszToken);

            /*
            ** If the token is a left parenthesis (i.e. "("), then push it onto the stack.
            */
            if (isBraceLeft(t)) {
                stackPush(t);
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

                while (stackPointer > 0 && !foundLeftParenthesis) {
                    token_t * stackToken;

                    stackToken = stackPop();

                    printf("Looking for '(', popped '%s' off the stack\n", stackToken->pszToken);

                    if (isBraceLeft(stackToken)) {
                        foundLeftParenthesis = true;
                    }
                    else {
                        queuePut(stackToken);
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

    printf("Num items in stack %d\n", stackPointer);

    /*
        While there are still operator tokens in the stack:
        If the operator token on the top of the stack is a parenthesis,
        then there are mismatched parentheses.
        Pop the operator onto the output queue.
    */
    while (stackPointer > 0) {
        token_t * stackToken;

        stackToken = stackPop();

        printf("Popped item off stack\n");

        if (stackToken != NULL) {
            printf("stackPop '%s'\n", stackToken->pszToken);
        }
        else {
            printf("NULL item on stack\n");
            return -1;
        }

        if (isBrace((token_t *)stackToken)) {
            /*
            ** If we've got here, we must have unmatched parenthesis...
            */
            return -1;
        }
        else {
            printf("qPutItem '%s'\n", stackToken->pszToken);
            queuePut(stackToken);
        }
    }

    return 0;
}

static int evaluateOperation(token_t * result, token_t * operation, token_t * operand1, token_t * operand2) {
    mpfr_t          o1;
    mpfr_t          o2;
    mpfr_t          r;
    char            pszResult[80];
    char            szFormatStr[32];

    printf("Got operand: '%s'\n", operand1->pszToken);
    printf("Got operand: '%s'\n", operand2->pszToken);

    mpfr_init2(o1, basePrecision);
    mpfr_strtofr(o1, operand1->pszToken, NULL, getBase(), MPFR_RNDA);

    mpfr_init2(o2, basePrecision);
    mpfr_strtofr(o2, operand2->pszToken, NULL, getBase(), MPFR_RNDA);

    mpfr_init2(r, basePrecision);

    switch (operation->type) {
        case token_operator_plus:
            mpfr_add(r, o1, o2, MPFR_RNDA);
            break;

        case token_operator_minus:
            mpfr_sub(r, o1, o2, MPFR_RNDA);
            break;

        case token_operator_multiply:
            mpfr_mul(r, o1, o2, MPFR_RNDA);
            break;

        case token_operator_divide:
            mpfr_div(r, o1, o2, MPFR_RNDA);
            break;

        case token_operator_mod:
            mpfr_remainder(r, o1, o2, MPFR_RNDA);
            break;

        default:
            return -1;
    }

    sprintf(szFormatStr, "%%.%ldRf", (long)getPrecision());

    mpfr_sprintf(pszResult, szFormatStr, r);
    result->pszToken = pszResult;
    result->length = strlen(pszResult);
    result->type = token_operand;

    stackPush(result);

    return 0;
}

static int evaluateFunction(token_t * result, token_t * function, token_t * operand) {
    mpfr_t          o1;
    mpfr_t          r;
    char            pszResult[80];
    char            szFormatStr[32];

    printf("Got operand: '%s'\n", operand->pszToken);

    mpfr_init2(o1, basePrecision);
    mpfr_strtofr(o1, operand->pszToken, NULL, getBase(), MPFR_RNDA);

    mpfr_init2(r, basePrecision);

    switch (function->type) {
        case token_function_sin:
            mpfr_sinu(r, o1, 360U, MPFR_RNDA);
            break;

        case token_function_cos:
            mpfr_cosu(r, o1, 360U, MPFR_RNDA);
            break;

        case token_function_tan:
            mpfr_tanu(r, o1, 360U, MPFR_RNDA);
            break;

        case token_function_asin:
            mpfr_asinu(r, o1, 360U, MPFR_RNDA);
            break;

        case token_function_acos:
            mpfr_acosu(r, o1, 360U, MPFR_RNDA);
            break;

        case token_function_atan:
            mpfr_atanu(r, o1, 360U, MPFR_RNDA);
            break;

        case token_function_sqrt:
            mpfr_sqrt(r, o1, MPFR_RNDA);
            break;

        case token_function_log:
            mpfr_log10(r, o1, MPFR_RNDA);
            break;

        case token_function_ln:
            mpfr_log(r, o1, MPFR_RNDA);
            break;

        case token_function_fac:
            mpfr_fac_ui(r, mpfr_get_si(o1, MPFR_RNDA), MPFR_RNDA);
            break;

        case token_function_mem:
            mpfr_strtofr(
                    r, 
                    memoryFetch((int)mpfr_get_si(o1, MPFR_RNDA))->pszToken, 
                    NULL, 
                    getBase(), 
                    MPFR_RNDA);
            break;

        default:
            return -1;
    }

    sprintf(szFormatStr, "%%.%ldRf", (long)getPrecision());

    mpfr_sprintf(pszResult, szFormatStr, r);
    result->pszToken = pszResult;
    result->length = strlen(pszResult);
    result->type = token_operand;

    stackPush(result);

    return 0;
}

int memoryStore(token_t * t, int memoryLocation) {
    if (memoryLocation < 0 || memoryLocation > 9) {
        return -1;
    }

    memory[memoryLocation].pszToken = strndup(t->pszToken, t->length);

    memory[memoryLocation].length = t->length;
    memory[memoryLocation].type = token_operand;

    return 0;
}

token_t * memoryFetch(int memoryLocation) {
    if (memoryLocation < 0 || memoryLocation > 9) {
        return NULL;
    }

    return &memory[memoryLocation];
}

int evaluate(char * pszExpression, token_t * result) {
    tokenizer_t             tokenizer;

    tzrInit(&tokenizer, pszExpression, getBase());

    queueInit();
    stackInit();

    /*
    ** Convert the calculation in infix notation to the postfix notation
    ** (Reverse Polish Notation) using the 'shunting yard algorithm'...
    */
    if (_convertToRPN(&tokenizer)) {
        printf("Error converting to RPN\n");
        return -1;
    }

    stackInit();

    printf("num items in queue = %d\n", getQueueSize());

    while (getQueueSize()) {
        token_t *       t;

        t = queueGet();

        printf("\nGot q item '%s'\n", t->pszToken);

        if (isOperand(&tokenizer, t)) {
            stackPush(t);
        }
        /*
        ** Must be Operator or Function...
        */
        else if (isFunction(t)) {
                token_t *       o1;

                o1 = stackPop();

                token_t result;

                evaluateFunction(&result, t, o1);
        }
        else if (isOperator(t)) {
                token_t *       o1;
                token_t *       o2;

                o2 = stackPop();
                o1 = stackPop();

                token_t result;

                evaluateOperation(&result, t, o1, o2);
        }
    }

    /*
    ** If there is one and only one item left on the stack,
    ** it is the result of the calculation. Otherwise, we
    ** have too many tokens and therefore an error...
    */
    if (stackPointer == 1) {
        token_t * r = stackPop();
        memcpy(result, r, sizeof(token_t));
    }
    else {
        tzrFinish(&tokenizer);
        return -1;
    }

    tzrFinish(&tokenizer);
    
    return 0;
}

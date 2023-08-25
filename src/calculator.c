#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <gmp.h>
#include <mpfr.h>

#include "logger.h"
#include "tokenizer.h"
#include "utils.h"
#include "calculator.h"

#define LIST_SIZE                   128
#define STACK_SIZE                   64

#define CONSTANT_C            299792458U
#define CONSTANT_G                  "0.000000000066743"

static token_t                      tokenStack[STACK_SIZE];
static token_t                      tokenQueue[LIST_SIZE];

static int                          stackPointer = 0;
static int                          queueHead = 0;
static int                          queueTail = 0;
static int                          queueSize = 0;

static token_t                      memory[10];

const mpfr_prec_t                   basePrecision = 1024L;

static void stackInit(void) {
    stackPointer = 0;

    memset(tokenStack, 0, STACK_SIZE * sizeof(token_t));
}

static void stackPush(token_t * token) {
    memcpy(&tokenStack[stackPointer], token, sizeof(token_t));
    tokenStack[stackPointer].pszToken = strndup(token->pszToken, token->length);
    stackPointer++;
}

static token_t * stackPop(void) {
    if (stackPointer > 0) {
        stackPointer--;
        return &tokenStack[stackPointer];
    }
    else {
        return NULL;
    }
}

static token_t * stackPeek(void) {
    return &tokenStack[stackPointer - 1];
}

static void queueInit(void) {
    queueHead = 0;
    queueTail = 0;
    queueSize = 0;

    memset(tokenQueue, 0, LIST_SIZE * sizeof(token_t));
}

static void queuePut(token_t * token) {
    memcpy(&tokenQueue[queueTail], token, sizeof(token_t));
    tokenQueue[queueTail].pszToken = strndup(token->pszToken, token->length);
    queueTail++;
    queueSize++;
}

static token_t * queueGet(void) {
    if (queueSize > 0) {
        queueSize--;
        return &tokenQueue[queueHead++];
    }
    else {
        return NULL;
    }
}

static int getQueueSize(void) {
    return queueSize;
}

static void inline freeToken(token_t * t) {
    free(t->pszToken);
    free(t);
}

static int _convertToRPN(tokenizer_t * tokenizer) {
    while (tzrHasMoreTokens(tokenizer)) {
        token_t * t = tzrNextToken(tokenizer);
        
        /*
        ** If the token is a number, then push it to the output queue.
        */
        if (isOperand(tokenizer, t)) {
            lgLogDebug("Got operand '%s'", t->pszToken);
            queuePut(t);
        }
        else if (isConstant(t)) {
            lgLogDebug("Got constant '%s'", t->pszToken);
            queuePut(t);
        }
        /*
        ** If the token is a function token, then push it onto the stack.
        */
        else if (isFunction(t)) {
            lgLogDebug("Got function '%s'", t->pszToken);
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
            lgLogDebug("Got operator '%s'", t->pszToken);

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

                        free(op2->pszToken);
                    }
                    else {
                        break;
                    }
                }
            }

            stackPush(t);
        }
        else if (isBrace(t)) {
            lgLogDebug("Got brace '%s'", t->pszToken);

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
                    return ERROR_RPN_UNMATCHED_PARENTHESIS;
                }
            }
        }

        freeToken(t);
    }

    lgLogDebug("Num items in stack %d", stackPointer);

    /*
        While there are still operator tokens in the stack:
        If the operator token on the top of the stack is a parenthesis,
        then there are mismatched parentheses.
        Pop the operator onto the output queue.
    */
    while (stackPointer > 0) {
        token_t * stackToken;

        stackToken = stackPop();

        if (stackToken != NULL) {
            lgLogDebug("stackPop '%s'", stackToken->pszToken);
        }
        else {
            lgLogError("NULL item on stack");
            return ERROR_RPN_NULL_STACK_POP;
        }

        if (isBrace(stackToken)) {
            /*
            ** If we've got here, we must have unmatched parenthesis...
            */
            return ERROR_RPN_UNMATCHED_PARENTHESIS_ON_STACK;
        }
        else {
            lgLogDebug("qPutItem '%s'", stackToken->pszToken);
            queuePut(stackToken);
        }
    }

    return EVALUATE_OK;
}

static int evaluateConstant(token_t * result, token_t * constant) {
    mpfr_t          r;
    char            pszResult[80];
    char            szFormatStr[32];

    lgLogInfo("Got constant: '%s'", constant->pszToken);

    mpfr_init2(r, basePrecision);

    switch (constant->type) {
        case token_constant_pi:
            mpfr_const_pi(r, MPFR_RNDA);
            break;

        case token_constant_c:
            mpfr_set_ui(r, CONSTANT_C, MPFR_RNDA);
            break;

        case token_constant_euler:
            mpfr_const_euler(r, MPFR_RNDA);
            break;

        case token_constant_gravity:
            mpfr_strtofr(r, CONSTANT_G, NULL, getBase(), MPFR_RNDA);
            break;

        default:
            return ERROR_EVALUATE_UNRECOGNISED_CONSTANT;
    }

    sprintf(szFormatStr, "%%.%ldRf", (long)getPrecision());

    mpfr_sprintf(pszResult, szFormatStr, r);
    result->pszToken = pszResult;
    result->length = strlen(pszResult);
    result->type = token_operand;

    stackPush(result);

    return EVALUATE_OK;
}

static int evaluateOperation(token_t * result, token_t * operation, token_t * operand1, token_t * operand2) {
    mpfr_t          o1;
    mpfr_t          o2;
    mpfr_t          r;
    mpz_t           integer_r;
    char            pszResult[80];
    char            szFormatStr[32];
    unsigned long   ui;

    lgLogInfo("Got operation: '%s'", operation->pszToken);
    lgLogInfo("Got operand 1: '%s'", operand1->pszToken);
    lgLogInfo("Got operand 2: '%s'", operand2->pszToken);

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

        case token_operator_power:
            mpfr_pow(r, o1, o2, MPFR_RNDA);
            break;

        case token_operator_root:
            mpfr_rootn_ui(r, o1, mpfr_get_ui(o2, MPFR_RNDA), MPFR_RNDA);
            break;

        case token_operator_AND:
            ui = mpfr_get_ui(o1, MPFR_RNDA) & mpfr_get_ui(o2, MPFR_RNDA);
            lgLogStatus("Got UI result %LU for operator '&'", ui);
            mpfr_set_ui(r, ui, MPFR_RNDA);
            break;

        case token_operator_OR:
            ui = mpfr_get_ui(o1, MPFR_RNDA) | mpfr_get_ui(o2, MPFR_RNDA);
            lgLogStatus("Got UI result %LU for operator '|'", ui);
            mpfr_set_ui(r, ui, MPFR_RNDA);
            break;

        case token_operator_XOR:
            ui = mpfr_get_ui(o1, MPFR_RNDA) ^ mpfr_get_ui(o2, MPFR_RNDA);
            lgLogStatus("Got UI result %LU for operator '^'", ui);
            mpfr_set_ui(r, ui, MPFR_RNDA);
            break;

        case token_operator_left_shift:
            ui = mpfr_get_ui(o1, MPFR_RNDA) << mpfr_get_ui(o2, MPFR_RNDA);
            lgLogStatus("Got UI result %LU for operator '<<'", ui);
            mpfr_set_ui(r, ui, MPFR_RNDA);
            break;

        case token_operator_right_shift:
            ui = mpfr_get_ui(o1, MPFR_RNDA) >> mpfr_get_ui(o2, MPFR_RNDA);
            lgLogStatus("Got UI result %LU for operator '>>'", ui);
            mpfr_set_ui(r, ui, MPFR_RNDA);
            break;

        default:
            return ERROR_EVALUATE_UNRECOGNISED_OPERATOR;
    }

    if (getBase() == DECIMAL) {
        sprintf(szFormatStr, "%%.%ldRf", (long)getPrecision());
        mpfr_sprintf(pszResult, szFormatStr, r);
    }
    else {
        mpz_init(integer_r);
        mpfr_get_z(integer_r, r, MPFR_RNDA);
        sprintf(pszResult, "%s", mpz_get_str(NULL, getBase(), integer_r));
    }

    result->pszToken = pszResult;
    result->length = strlen(pszResult);
    result->type = token_operand;

    stackPush(result);
    
    return EVALUATE_OK;
}

static int evaluateFunction(token_t * result, token_t * function, token_t * operand) {
    mpfr_t          o1;
    mpfr_t          r;
    char            pszResult[80];
    char            szFormatStr[32];

    lgLogInfo("Got function: '%s'", function->pszToken);
    lgLogInfo("Got operand: '%s'", operand->pszToken);

    mpfr_init2(o1, basePrecision);
    mpfr_strtofr(o1, operand->pszToken, NULL, getBase(), MPFR_RNDA);

    mpfr_init2(r, basePrecision);

    switch (function->type) {
        case token_function_sin:
            if (getTrigMode() == degrees) {
                mpfr_sinu(r, o1, 360U, MPFR_RNDA);
            }
            else {
                mpfr_sin(r, o1, MPFR_RNDA);
            }
            break;

        case token_function_cos:
            if (getTrigMode() == degrees) {
                mpfr_cosu(r, o1, 360U, MPFR_RNDA);
            }
            else {
                mpfr_cos(r, o1, MPFR_RNDA);
            }
            break;

        case token_function_tan:
            if (getTrigMode() == degrees) {
                mpfr_tanu(r, o1, 360U, MPFR_RNDA);
            }
            else {
                mpfr_tan(r, o1, MPFR_RNDA);
            }
            break;

        case token_function_asin:
            if (getTrigMode() == degrees) {
                mpfr_asinu(r, o1, 360U, MPFR_RNDA);
            }
            else {
                mpfr_asin(r, o1, MPFR_RNDA);
            }
            break;

        case token_function_acos:
            if (getTrigMode() == degrees) {
                mpfr_acosu(r, o1, 360U, MPFR_RNDA);
            }
            else {
                mpfr_acos(r, o1, MPFR_RNDA);
            }
            break;

        case token_function_atan:
            if (getTrigMode() == degrees) {
                mpfr_atanu(r, o1, 360U, MPFR_RNDA);
            }
            else {
                mpfr_atan(r, o1, MPFR_RNDA);
            }
            break;

        case token_function_sinh:
            mpfr_sinh(r, o1, MPFR_RNDA);
            break;

        case token_function_cosh:
            mpfr_cosh(r, o1, MPFR_RNDA);
            break;
            break;

        case token_function_tanh:
            mpfr_tanh(r, o1, MPFR_RNDA);
            break;
            break;

        case token_function_asinh:
            mpfr_asinh(r, o1, MPFR_RNDA);
            break;
            break;

        case token_function_acosh:
            mpfr_acosh(r, o1, MPFR_RNDA);
            break;
            break;

        case token_function_atanh:
            mpfr_atanh(r, o1, MPFR_RNDA);
            break;
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
            return ERROR_EVALUATE_UNRECOGNISED_FUNCTION;
    }

    sprintf(szFormatStr, "%%.%ldRf", (long)getPrecision());

    mpfr_sprintf(pszResult, szFormatStr, r);
    result->pszToken = pszResult;
    result->length = strlen(pszResult);
    result->type = token_operand;

    stackPush(result);

    return EVALUATE_OK;
}

int memoryStore(token_t * t, int memoryLocation) {
    if (memoryLocation < 0 || memoryLocation > 9) {
        return -1;
    }

    memory[memoryLocation].pszToken = strndup(t->pszToken, t->length);

    memory[memoryLocation].length = t->length;
    memory[memoryLocation].type = token_operand;

    return EVALUATE_OK;
}

token_t * memoryFetch(int memoryLocation) {
    if (memoryLocation < 0 || memoryLocation > 9) {
        return NULL;
    }

    return &memory[memoryLocation];
}

int evaluate(const char * pszExpression, token_t * result) {
    tokenizer_t             tokenizer;
    int                     error = 0;

    tzrInit(&tokenizer, pszExpression, getBase());

    queueInit();
    stackInit();

    /*
    ** Convert the calculation in infix notation to the postfix notation
    ** (Reverse Polish Notation) using the 'shunting yard algorithm'...
    */
    error = _convertToRPN(&tokenizer);

    if (error) {
        lgLogError("Error converting to RPN");
        return error;
    }

    stackInit();

    lgLogDebug("num items in queue = %d", getQueueSize());

    while (getQueueSize()) {
        token_t *       t;

        t = queueGet();

        if (isOperand(&tokenizer, t)) {
            lgLogDebug("Got operand: '%s'", t->pszToken);
            stackPush(t);
        }
        else if (isConstant(t)) {
            lgLogDebug("Got constant: '%s'", t->pszToken);

            token_t result;

            error = evaluateConstant(&result, t);

            if (error) {
                lgLogError("evaluateContant returned %d\n", error);
                return error;
            }
        }
        /*
        ** Must be Operator or Function...
        */
        else if (isFunction(t)) {
            token_t *       o1;

            o1 = stackPop();

            if (o1 == NULL) {
                lgLogError("NULL operand for function '%s'", t->pszToken);
                free(t->pszToken);
                tzrFinish(&tokenizer);
                return ERROR_EVALUATE_NULL_STACK_POP;
            }

            token_t result;

            error = evaluateFunction(&result, t, o1);

            if (error) {
                lgLogError("evaluateFunction returned %d\n", error);
                return error;
            }
        }
        else if (isOperator(t)) {
            token_t *       o1;
            token_t *       o2;

            o2 = stackPop();
            o1 = stackPop();

            if (o1 == NULL || o2 == NULL) {
                lgLogError("NULL operand for operator '%s'", t->pszToken);
                free(t->pszToken);
                tzrFinish(&tokenizer);
                return ERROR_EVALUATE_NULL_STACK_POP;
            }

            token_t result;

            evaluateOperation(&result, t, o1, o2);

            if (error) {
                lgLogError("evaluateOperation returned %d\n", error);
                return error;
            }
        }

        free(t->pszToken);
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
        lgLogError("evaluate(): Got invalid items on stack!");

        while (stackPointer > 0) {
            token_t * tok = stackPop();
            lgLogError("Invalid item on stack '%s'\n", tok->pszToken);
        }

        tzrFinish(&tokenizer);
        return ERROR_EVALUATE_UNEXPECTED_TOKENS;
    }

    tzrFinish(&tokenizer);
    
    return EVALUATE_OK;
}

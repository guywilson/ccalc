#include <string>
#include <cstring>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <gmp.h>
#include <mpfr.h>

#include "logger.h"
#include "tokenizer.h"
#include "token.h"
#include "utils.h"
#include "calculator.h"

using namespace std;

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

static operand_t                    memory[10];

static void stackInit(void) {
    stackPointer = 0;
}

static void stackPush(token_t & token) {
    tokenStack[stackPointer++] = token;
}

static token_t & stackPop(void) {
    if (stackPointer > 0) {
        stackPointer--;
        return tokenStack[stackPointer];
    }
    else {
        return NULL;
    }
}

static token_t & stackPeek(void) {
    return tokenStack[stackPointer - 1];
}

static void queueInit(void) {
    queueHead = 0;
    queueTail = 0;
    queueSize = 0;
}

static void queuePut(token_t & token) {
    tokenQueue[queueTail++] = token;
    queueSize++;
}

static token_t & queueGet(void) {
    if (queueSize > 0) {
        queueSize--;
        return tokenQueue[queueHead++];
    }
    else {
        return NULL;
    }
}

static int getQueueSize(void) {
    return queueSize;
}

static void inline freeToken(token_t * t) {
    delete t;
}

static int _convertToRPN(tokenizer_t * tokenizer) {
    while (tzrHasMoreTokens(tokenizer)) {
        token_t * t = tzrNextToken(tokenizer);
        
        /*
        ** If the token is a number, then push it to the output queue.
        */
        if (t->type == token_operand) {
            queuePut(t);
        }
        else if (t->type == token_constant) {
            queuePut(t);
        }
        /*
        ** If the token is a function token, then push it onto the stack.
        */
        else if (t->type == token_function) {
            stackPush(t);
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
        else if (t->type == token_operator) {
            operator_t op = t->item.operator;

            while (stackPointer > 0) {
                token_t * topToken;
    
                topToken = stackPeek();

                if (topToken->type != token_operator && topToken->type != token_function) {
                    break;
                }
                else {
                    if ((op.associativity == associativity_left && op.prescedence <= topToken->item.operator.prescedence) ||
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
        else if (t->type == token_brace) {
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

        if (stackToken == NULL) {
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
            queuePut(stackToken);
        }
    }

    return EVALUATE_OK;
}

static int evaluateConstant(operand_t * result, constant_t constant) {
    mpfr_init2(result->value, getBasePrecision());

    switch (constant.ID) {
        case constant_pi:
            mpfr_const_pi(result->value, MPFR_RNDA);
            break;

        case constant_c:
            mpfr_set_ui(result->value, CONSTANT_C, MPFR_RNDA);
            break;

        case constant_euler:
            mpfr_const_euler(result->value, MPFR_RNDA);
            break;

        case constant_gravity:
            mpfr_strtofr(result->value, CONSTANT_G, NULL, getBase(), MPFR_RNDA);
            break;

        default:
            return ERROR_EVALUATE_UNRECOGNISED_CONSTANT;
    }

    return EVALUATE_OK;
}

static int evaluateOperation(operand_t * result, operator_t operator, operand_t * operand1, operand_t * operand2) {
    mpz_t           integer_r;
    unsigned long   ui;

    mpfr_init2(result->value, getBasePrecision());

    switch (operator.ID) {
        case operator_plus:
            mpfr_add(result->value, operand1->value, operand2->value, MPFR_RNDA);
            break;

        case operator_minus:
            mpfr_sub(result->value, operand1->value, operand2->value, MPFR_RNDA);
            break;

        case operator_multiply:
            mpfr_mul(result->value, operand1->value, operand2->value, MPFR_RNDA);
            break;

        case operator_divide:
            mpfr_div(result->value, operand1->value, operand2->value, MPFR_RNDA);
            break;

        case operator_mod:
            mpfr_remainder(result->value, operand1->value, operand2->value, MPFR_RNDA);
            break;

        case operator_power:
            mpfr_pow(result->value, operand1->value, operand2->value, MPFR_RNDA);
            break;

        case operator_root:
            mpfr_rootn_ui(result->value, operand1->value, mpfr_get_ui(operand2->value, MPFR_RNDA), MPFR_RNDA);
            break;

        case operator_AND:
            ui = mpfr_get_ui(operand1->value, MPFR_RNDA) & mpfr_get_ui(operand2->value, MPFR_RNDA);
            lgLogStatus("Got UI result %LU for operator '&'", ui);
            mpfr_set_ui(result->value, ui, MPFR_RNDA);
            break;

        case operator_OR:
            ui = mpfr_get_ui(operand1->value, MPFR_RNDA) | mpfr_get_ui(operand2->value, MPFR_RNDA);
            lgLogStatus("Got UI result %LU for operator '|'", ui);
            mpfr_set_ui(result->value, ui, MPFR_RNDA);
            break;

        case operator_XOR:
            ui = mpfr_get_ui(operand1->value, MPFR_RNDA) ^ mpfr_get_ui(operand2->value, MPFR_RNDA);
            lgLogStatus("Got UI result %LU for operator '^'", ui);
            mpfr_set_ui(result->value, ui, MPFR_RNDA);
            break;

        case operator_left_shift:
            ui = mpfr_get_ui(operand1->value, MPFR_RNDA) << mpfr_get_ui(operand2->value, MPFR_RNDA);
            lgLogStatus("Got UI result %LU for operator '<<'", ui);
            mpfr_set_ui(result->value, ui, MPFR_RNDA);
            break;

        case operator_right_shift:
            ui = mpfr_get_ui(operand1->value, MPFR_RNDA) >> mpfr_get_ui(operand2->value, MPFR_RNDA);
            lgLogStatus("Got UI result %LU for operator '>>'", ui);
            mpfr_set_ui(result->value, ui, MPFR_RNDA);
            break;

        default:
            return ERROR_EVALUATE_UNRECOGNISED_OPERATOR;
    }
    
    return EVALUATE_OK;
}

static int evaluateFunction(operand_t * result, function_t function, operand_t * operand) {
    mpfr_init2(result->value, getBasePrecision());

    switch (function.ID) {
        case function_sin:
            if (getTrigMode() == degrees) {
                mpfr_sinu(result->value, operand->value, 360U, MPFR_RNDA);
            }
            else {
                mpfr_sin(result->value, operand->value, MPFR_RNDA);
            }
            break;

        case function_cos:
            if (getTrigMode() == degrees) {
                mpfr_cosu(result->value, operand->value, 360U, MPFR_RNDA);
            }
            else {
                mpfr_cos(result->value, operand->value, MPFR_RNDA);
            }
            break;

        case function_tan:
            if (getTrigMode() == degrees) {
                mpfr_tanu(result->value, operand->value, 360U, MPFR_RNDA);
            }
            else {
                mpfr_tan(result->value, operand->value, MPFR_RNDA);
            }
            break;

        case function_asin:
            if (getTrigMode() == degrees) {
                mpfr_asinu(result->value, operand->value, 360U, MPFR_RNDA);
            }
            else {
                mpfr_asin(result->value, operand->value, MPFR_RNDA);
            }
            break;

        case function_acos:
            if (getTrigMode() == degrees) {
                mpfr_acosu(result->value, operand->value, 360U, MPFR_RNDA);
            }
            else {
                mpfr_acos(result->value, operand->value, MPFR_RNDA);
            }
            break;

        case function_atan:
            if (getTrigMode() == degrees) {
                mpfr_atanu(result->value, operand->value, 360U, MPFR_RNDA);
            }
            else {
                mpfr_atan(result->value, operand->value, MPFR_RNDA);
            }
            break;

        case function_sinh:
            mpfr_sinh(result->value, operand->value, MPFR_RNDA);
            break;

        case function_cosh:
            mpfr_cosh(result->value, operand->value, MPFR_RNDA);
            break;
            break;

        case function_tanh:
            mpfr_tanh(result->value, operand->value, MPFR_RNDA);
            break;
            break;

        case function_asinh:
            mpfr_asinh(result->value, operand->value, MPFR_RNDA);
            break;
            break;

        case function_acosh:
            mpfr_acosh(result->value, operand->value, MPFR_RNDA);
            break;
            break;

        case function_atanh:
            mpfr_atanh(result->value, operand->value, MPFR_RNDA);
            break;
            break;

        case function_sqrt:
            mpfr_sqrt(result->value, operand->value, MPFR_RNDA);
            break;

        case function_log:
            mpfr_log10(result->value, operand->value, MPFR_RNDA);
            break;

        case function_ln:
            mpfr_log(result->value, operand->value, MPFR_RNDA);
            break;

        case function_fac:
            mpfr_fac_ui(result->value, mpfr_get_si(operand->value, MPFR_RNDA), MPFR_RNDA);
            break;

        case function_mem:
            mpfr_strtofr(
                    result->value, 
                    memoryFetch((int)mpfr_get_si(operand->value, MPFR_RNDA))->pszToken, 
                    NULL, 
                    getBase(), 
                    MPFR_RNDA);
            break;

        default:
            return ERROR_EVALUATE_UNRECOGNISED_FUNCTION;
    }

    return EVALUATE_OK;
}

int memoryStore(operand_t * operand, int memoryLocation) {
    if (memoryLocation < 0 || memoryLocation > 9) {
        return -1;
    }

    memcpy(&memory[memoryLocation], operand, sizeof(operand_t));

    return EVALUATE_OK;
}

operand_t * memoryFetch(int memoryLocation) {
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

            evaluateOperation(&result, t, operand->value, o2);

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

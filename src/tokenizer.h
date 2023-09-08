#include <gmp.h>
#include <mpfr.h>

#include "token.h"

#ifndef __INCL_TOKENIZER
#define __INCL_TOKENIZER


typedef enum {
    token_operator,
    token_function,
    token_constant,
    token_brace,
    token_operand
}
token_type_t;

typedef enum {
    constant_pi,
    constant_c,
    constant_euler,
    constant_gravity
}
constant_id_t;

typedef enum {
    brace_left,
    brace_right
}
brace_id_t;

typedef enum {
    operator_plus,
    operator_minus,
    operator_multiply,
    operator_divide,
    operator_mod,
    operator_power,
    operator_root,
    operator_AND,
    operator_OR,
    operator_XOR,
    operator_left_shift,
    operator_right_shift
}
operator_id_t;

typedef enum {
    function_sin,
    function_cos,
    function_tan,
    function_asin,
    function_acos,
    function_atan,
    function_sinh,
    function_cosh,
    function_tanh,
    function_asinh,
    function_acosh,
    function_atanh,
    function_sqrt,
    function_log,
    function_ln,
    function_fac,
    function_mem
}
function_id_t;

typedef enum {
    associativity_left,
    associativity_right
}
associativity;

typedef struct {
    int         startIndex;
    int         endIndex;
    int         expressionLen;
    int         base;

    char *      pszExpression;
}
tokenizer_t;

bool            isOperand(tokenizer_t * t, char * pszToken);
bool            isBrace(char * pszToken);
bool            isBraceLeft(char * pszToken);
bool            isBraceRight(char * pszToken);
bool            isOperator(char * pszToken);
bool            isConstant(char * pszToken);
bool            isFunction(char * pszToken);
void            tzrInit(tokenizer_t * t, const char * pszExpression, int base);
void            tzrFinish(tokenizer_t * t);
bool            tzrHasMoreTokens(tokenizer_t * t);
token_t *       tzrNextToken(tokenizer_t * t);

#endif

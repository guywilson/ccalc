#include <gmp.h>
#include <mpfr.h>

#ifndef __INCL_TOKENIZER
#define __INCL_TOKENIZER

#define BASE_10                 10
#define BASE_16                 16
#define BASE_8                   8
#define BASE_2                   2

#define DECIMAL                 BASE_10
#define HEXADECIMAL             BASE_16
#define OCTAL                   BASE_8
#define BINARY                  BASE_2

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

typedef struct {
    operator_id_t           ID;

    int                     prescedence;
    associativity           associativity;
}
operator_t;

typedef struct {
    function_id_t           ID;
}
function_t;

typedef struct {
    mpfr_t                  value;
    int                     base;
}
operand_t;

typedef struct {
    constant_id_t           ID;
}
constant_t;

typedef struct {
    brace_id_t              ID;
}
brace_t;

typedef union {
    operand_t               operand;
    operator_t              operator;
    function_t              function;
    constant_t              constant;
    brace_t                 brace;
}
item_t;

typedef struct {
    token_type_t            type;

    item_t                  item;
}
token_t;

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

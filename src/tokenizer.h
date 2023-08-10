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

typedef struct {
    int         startIndex;
    int         endIndex;
    int         expressionLen;
    int         base;

    char *      pszExpression;
}
tokenizer_t;

typedef enum {
    token_operator_plus,
    token_operator_minus,
    token_operator_multiply,
    token_operator_divide,
    token_operator_mod,
    token_operator_power,
    token_operator_AND,
    token_operator_OR,
    token_operator_XOR,
    token_function,
    token_constant,
    token_brace,
    token_operand
}
token_type_t;

typedef struct {
    int                     length;
    token_type_t            type;

    char *                  pszToken;
}
token_t;

#endif

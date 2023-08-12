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
    int                     length;
    token_type_t            type;

    char *                  pszToken;

    int                     operatorPrescedence;
    associativity           operatorAssociativity;
}
token_t;

bool            isOperand(tokenizer_t * t, token_t * token);
bool            isBrace(token_t * token);
bool            isBraceLeft(token_t * token);
bool            isBraceRight(token_t * token);
bool            isOperator(token_t * token);
bool            isConstant(token_t * token);
bool            isFunction(token_t * token);
associativity   getOperatorAssociativity(token_t * t);
int             getOperatorPrescedense(token_t * t);
void            tzrInit(tokenizer_t * t, char * pszExpression, int base);
void            tzrFinish(tokenizer_t * t);
bool            tzrHasMoreTokens(tokenizer_t * t);
token_t *       tzrNextToken(tokenizer_t * t);

#endif

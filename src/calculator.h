#include "tokenizer.h"

#ifndef __INCL_CALCULATOR
#define __INCL_CALCULATOR

#define EVALUATE_OK                                      0
#define ERROR_RPN_UNMATCHED_PARENTHESIS                 -1
#define ERROR_RPN_NULL_STACK_POP                        -2
#define ERROR_RPN_UNMATCHED_PARENTHESIS_ON_STACK        -3
#define ERROR_EVALUATE_UNRECOGNISED_OPERATOR            -4
#define ERROR_EVALUATE_UNRECOGNISED_FUNCTION            -5
#define ERROR_EVALUATE_UNRECOGNISED_CONSTANT            -6
#define ERROR_EVALUATE_UNEXPECTED_TOKENS                -7
#define ERROR_EVALUATE_NULL_STACK_POP                   -8

int         memoryStore(token_t * t, int memoryLocation);
token_t *   memoryFetch(int memoryLocation);
int         evaluate(const char * pszExpression, token_t * result);

#endif

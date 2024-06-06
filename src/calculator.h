#include <string>
#include <cstring>
#include <queue>
#include <stack>

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

#define DEFAULT_LOG_LEVEL                       (LOG_LEVEL_FATAL | LOG_LEVEL_ERROR)

void        evaluate(mpfr_t result, const char * pszExpression, int radix);

#endif

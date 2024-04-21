#include <string>
#include <gmp.h>
#include <mpfr.h>

using namespace std;

#ifndef __INCL_TOKENIZER
#define __INCL_TOKENIZER

typedef struct {
    int         startIndex;
    int         endIndex;
    int         expressionLen;
    int         base;

    char *      pszExpression;
}
tokenizer_t;

void            tzrInit(tokenizer_t * t, const char * pszExpression, int base);
void            tzrFinish(tokenizer_t * t);
bool            tzrHasMoreTokens(tokenizer_t * t);
string &        tzrNextToken(tokenizer_t * t);

#endif

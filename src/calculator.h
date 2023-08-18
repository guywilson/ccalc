#include "tokenizer.h"

#ifndef __INCL_CALCULATOR
#define __INCL_CALCULATOR

int         memoryStore(token_t * t, int memoryLocation);
token_t *   memoryFetch(int memoryLocation);
int         evaluate(const char * pszExpression, token_t * result);

#endif

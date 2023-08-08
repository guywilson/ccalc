#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int         startIndex;
    int         endIndex;
    int         expressionLen;
    int         base;

    char *      pszExpression;
}
tokenizer_t;

static int _findNextTokenPos() {

}

void tzrInit(tokenizer_t * t, char * pszExpression, int base) {
    t->pszExpression = strdup(pszExpression);
    
    t->startIndex = 0;
    t->endIndex = 0;
    t->expressionLen = strlen(t->pszExpression);
    t->base = base;
}

void tzrFinish(tokenizer_t * t) {
    free(t->pszExpression);
}


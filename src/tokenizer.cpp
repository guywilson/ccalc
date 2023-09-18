#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "tokenizer.h"
#include "logger.h"
#include "system.h"
#include "factory.h"

static bool isdelim(char ch) {
    int                     i;
    static const char *     pszDelimiters = " \t\n\r+-*/^:%&|~()[]{}";

    for (i = 0;i < (int)strlen(pszDelimiters);i++) {
        if (ch == pszDelimiters[i]) {
            return true;
        }
    }

    return false;
}

static char * _removeSpaces(const char * pszExpression) {
    char *          pszCopy;
    int             i;
    int             j = 0;
    char            ch;

    pszCopy = (char *)calloc(strlen(pszExpression) + 1, sizeof(char));

    for (i = 0;i < (int)strlen(pszExpression);i++) {
        ch = pszExpression[i];

        if (!isspace(ch)) {
            pszCopy[j++] = ch;
        }
    }

    return pszCopy;
}

/*
** 1. Ignore white space.
** 2. Copy chars to token until delimiter found
** 3. Return position of last char of token found
*/
static int _findNextTokenPos(tokenizer_t * t) {
    int		i;
    char	ch;
    int		tokenLen = 0;

    for (i = t->startIndex;i < (int)t->expressionLen;i++) {
        ch = t->pszExpression[i];

        if (isdelim(ch)) {
            /*
            ** Do we have a token on it's own, or is it a delimiter...
            */
            if (tokenLen > 0) {
                // The token is the delimiter to another token...
                return i;
            }
            else {
                /*
                ** If this is the '-' character and if the next char is a digit (0-9)
                ** and the previous char is not a ')' or a digit then this must be a -ve number,
                ** not the '-' operator...
                */
                char nextChar = t->pszExpression[i + 1];
                char prevChar = t->pszExpression[i - 1];

                if (ch == '-' && isdigit(nextChar)) {
                    bool isNegativeOperand = false;

                    if (i > 0) {
                        bool isPreviousCharBrace = 
                                    (prevChar == ')' || 
                                        prevChar == ']' || 
                                        prevChar == '}');

                        bool isPreviousCharDigit = isdigit(prevChar);

                        if (!isPreviousCharBrace && !isPreviousCharDigit) {
                            isNegativeOperand = true;
                        }
                        else {
                            isNegativeOperand = false;
                        }
                    }
                    else if (i == 0) {
                        // We're at the beginning of the expression, must be
                        // a -ve operand
                        isNegativeOperand = true;
                    }

                    if (isNegativeOperand) {
                        // Found a -ve number...
                        continue;
                    }
                    else {
                        return i + 1;
                    }
                }
                else {
                    // The token is the token we want to return...
                    return i + 1;
                }
            }
        }

        tokenLen++;

        /*
        ** If we haven't returned yet and we're at the end of
        ** the expression, we must have an operand at the end
        ** of the expression...
        */
        if (i == (t->expressionLen - 1)) {
            return i + 1;
        }
    }

    return -1;
}

void tzrInit(tokenizer_t * t, const char * pszExpression, int base) {
    t->pszExpression = _removeSpaces(pszExpression);

    lgLogDebug("Initialising tokenizer with expression: %s", t->pszExpression);

    t->startIndex = 0;
    t->endIndex = 0;
    t->expressionLen = strlen(t->pszExpression);
    t->base = base;
}

void tzrFinish(tokenizer_t * t) {
    free(t->pszExpression);
}

bool tzrHasMoreTokens(tokenizer_t * t) {
    int pos = _findNextTokenPos(t);

    if (pos > 0) {
        t->endIndex = pos;
        return true;
    }

    return false;
}

token_t * tzrNextToken(tokenizer_t * t) {
    int                 tokenLength;
    int                 i;
    int                 j = 0;
    char *              pszToken;

    tokenLength = (t->endIndex - t->startIndex);

    pszToken = (char *)malloc(tokenLength + 1);

    for (i = t->startIndex;i < t->endIndex;i++) {
        pszToken[j++] = t->pszExpression[i];
    }
    
    pszToken[j] = 0;

    t->startIndex = t->endIndex;

    lgLogDebug("Got token '%s'", pszToken);

    token_t * token = token_factory::createToken(pszToken);

    token->setBase(t->base);

    free(pszToken);

    return token;
}

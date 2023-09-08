#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "tokenizer.h"
#include "utils.h"
#include "factory.h"

static const char * pszBraces = "({[]})";

static char * trim(char * src) {
    int         i;
    int         len = strlen(src);

    for (i = 0;i < len;i++) {
        if (i > 0) {
            if (isspace(src[i])) {
                src[i] = 0;
                break;
            }
        }
    }

    return src;
}

static bool isdelim(char ch) {
    int                     i;
    static const char *     pszDelimiters = " \t\n\r+-*/^:%&|~()[]{}";

    for (i = 0;i < strlen(pszDelimiters);i++) {
        if (ch == pszDelimiters[i]) {
            return true;
        }
    }

    return false;
}

static int _findNextTokenPos(tokenizer_t * t) {
    int		i;
    char	ch;
    bool	lookingForWhiteSpace = true;
    int		tokenLen = 0;

    for (i = t->startIndex;i < (int)t->expressionLen;i++) {
        ch = t->pszExpression[i];

        if (lookingForWhiteSpace) {
            if (isspace(ch)) {
                t->startIndex++;
                continue;
            }
            else {
                lookingForWhiteSpace = false;
            }
        }

        if (isspace(ch)) {
            return i;
        }
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
                if (ch == '-' && isdigit(t->pszExpression[i + 1])) {
                    bool isNegativeOperand = false;

                    if (i > 0) {
                        char previousChar = t->pszExpression[i - 1];

                        bool isPreviousCharBrace = 
                                (previousChar == ')' || previousChar == ']' || previousChar == '}');
                        bool isPreviousCharDigit = isdigit(previousChar);

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
    t->pszExpression = strdup(pszExpression);

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
    token_t *           token;
    int                 tokenLength;
    char *              pszToken;

    tokenLength = (t->endIndex - t->startIndex);

    pszToken = trim(strndup(&t->pszExpression[t->startIndex], tokenLength));
    tokenLength = strlen(pszToken);

    t->startIndex = t->endIndex;

    token = token_factory::createToken(pszToken);

    return token;
}

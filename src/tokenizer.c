#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

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
    token_function,
    token_constant,
    token_operand
}
token_type_t;

typedef struct {
    int                     length;
    token_type_t            type;

    char *                  pszToken;
}
token_t;

static const char * pszBraces = "({[]})";

static bool isdelim(char ch) {
    int                     i;
    static const char *     pszDelimiters = " \t\n\r+-*/^%&|~()[]{}";

    for (i = 0;i < strlen(pszDelimiters);i++) {
        if (ch == pszDelimiters[i]) {
            return true;
        }
    }

    return false;
}

static bool isDecimalDigit(char ch) {
    return (isdigit(ch) || ch == '.' || ch == '-');
}

static bool isHexadecimalDigit(char ch) {
    return (ishexnumber(ch));
}

static bool isOctalDigit(char ch) {
    return (ch >= '0' && ch <= '8');
}

static bool isBinaryDigit(char ch) {
    return (ch == '0' || ch == '1');
}

static bool isUniversalDigit(char ch) {
    return (
        isDecimalDigit(ch) || 
        isHexadecimalDigit(ch) || 
        isOctalDigit(ch) || 
        isBinaryDigit(ch));
}

static bool isOperand(tokenizer_t * t, token_t * token) {
    int		i;
    char    ch;
    bool	ret = true;

    if (token->pszToken[0] == '-' && token->length == 1) {
        // Must be the '-' operator...
        return false;
    }
    else {
        for (i = 0;i < token->length;i++) {
            ch = token->pszToken[i];

            switch (t->base) {
                case DECIMAL:
                    if (!isDecimalDigit(ch)) {
                        return false;
                    }
                    break;
                    
                case HEXADECIMAL:
                    if (!isHexadecimalDigit(ch)) {
                        return false;
                    }
                    break;
                    
                case BINARY:
                    if (!isBinaryDigit(ch)) {
                        return false;
                    }
                    break;
                    
                case OCTAL:
                    if (!isOctalDigit(ch)) {
                        return false;
                    }
                    break;
            }
        }
    }

    return true;
}

static bool isOperatorPlus(token_t * token) {
    return (token->pszToken[0] == '+');
}

static bool isOperatorMinus(token_t * token) {
    if (token->pszToken[0] == '-') {
        if (token->length > 1 && (isUniversalDigit(token->pszToken[1]))) {
            return false;
        }
    }
    else {
        return false;
    }

    return true;
}

static bool isOperatorMultiply(token_t * token) {
    return (token->pszToken[0] == '*');
}

static bool isOperatorDivide(token_t * token) {
    return (token->pszToken[0] == '/');
}

static bool isOperatorMod(token_t * token) {
    return (token->pszToken[0] == '%');
}

static bool isOperatorPower(token_t * token) {
    return (token->pszToken[0] == '^');
}

static bool isOperatorAND(token_t * token) {
    return (token->pszToken[0] == '&');
}

static bool isOperatorOR(token_t * token) {
    return (token->pszToken[0] == '|');
}

static bool isOperatorXOR(token_t * token) {
    return (token->pszToken[0] == '~');
}

static bool isOperator(token_t * token) {
    return 
        (isOperatorPlus(token)      || 
        isOperatorMinus(token)      || 
        isOperatorMultiply(token)   || 
        isOperatorDivide(token)     || 
        isOperatorMod(token)        || 
        isOperatorPower(token)      || 
        isOperatorAND(token)        || 
        isOperatorOR(token)         || 
        isOperatorXOR(token));
}

static bool isBrace(token_t * token) {
    int             i;

    for (i = 0;i < strlen(pszBraces);i++) {
        if (token->pszToken[0] == pszBraces[i]) {
            return true;
        }
    }

    return false;
}

static bool isConstantPi(token_t * token) {
    if (strncmp(token->pszToken, "pi", 2) == 0) {
        return true;
    }

    return false;
}

static bool isConstantC(token_t * token) {
    return (token->pszToken[0] == 'c' || token->pszToken[0] == 'C');
}

static bool isConstant(token_t * token) {
    return (isConstantPi(token) || isConstantC(token));
}

static bool isFunctionSine(token_t * token) {
    return (strncmp(token->pszToken, "sin", 3) == 0);
}

static bool isFunctionCosine(token_t * token) {
    return (strncmp(token->pszToken, "cos", 3) == 0);
}

static bool isFunctionTangent(token_t * token) {
    return (strncmp(token->pszToken, "tan", 3) == 0);
}

static bool isFunctionArcSine(token_t * token) {
    return (strncmp(token->pszToken, "asin", 4) == 0);
}

static bool isFunctionArcCosine(token_t * token) {
    return (strncmp(token->pszToken, "acos", 4) == 0);
}

static bool isFunctionArcTangent(token_t * token) {
    return (strncmp(token->pszToken, "atan", 4) == 0);
}

static bool isFunctionSquareRoot(token_t * token) {
    return (strncmp(token->pszToken, "sqrt", 4) == 0);
}

static bool isFunctionLogarithm(token_t * token) {
    return (strncmp(token->pszToken, "log", 3) == 0);
}

static bool isFunctionNaturalLog(token_t * token) {
    return (strncmp(token->pszToken, "ln", 2) == 0);
}

static bool isFunctionFactorial(token_t * token) {
    return (strncmp(token->pszToken, "fact", 4) == 0);
}

static bool isFunctionMemory(token_t * token) {
    return (strncmp(token->pszToken, "mem", 3) == 0);
}

static bool isFunction(token_t * token) {
    return (
        isFunctionSine(token)       ||
        isFunctionCosine(token)     ||
        isFunctionTangent(token)    ||
        isFunctionArcSine(token)    ||
        isFunctionArcCosine(token)  ||
        isFunctionArcTangent(token) ||
        isFunctionSquareRoot(token) ||
        isFunctionLogarithm(token)  ||
        isFunctionNaturalLog(token) ||
        isFunctionFactorial(token)  ||
        isFunctionMemory(token));
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

    token = (token_t *)malloc(sizeof(token_t));

    if (token == NULL) {
        return NULL;
    }

    token->length = (t->endIndex - t->startIndex) + 1;

    token->pszToken = strndup(&t->pszExpression[t->startIndex], token->length);

    t->startIndex = t->endIndex;

    
    t = TokenFactory.createToken(token, this._base);

    return token;
}

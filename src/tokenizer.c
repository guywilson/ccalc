#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "tokenizer.h"

#define CONSTANT_C                      "299792458"
#define CONSTANT_PI                     "3.1415926535897932384626433832795028841971693993751058209749445923"

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

static bool isConstantPi(token_t * token) {
    if (strncmp(token->pszToken, "pi", 2) == 0) {
        return true;
    }

    return false;
}

static bool isConstantC(token_t * token) {
    return (token->pszToken[0] == 'c' || token->pszToken[0] == 'C');
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


bool isOperand(tokenizer_t * t, token_t * token) {
    int		i;
    char    ch;

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

bool isBrace(token_t * token) {
    int             i;

    for (i = 0;i < strlen(pszBraces);i++) {
        if (token->pszToken[0] == pszBraces[i]) {
            return true;
        }
    }

    return false;
}

bool isBraceLeft(token_t * token) {
    if (token->pszToken[0] == '(' || 
        token->pszToken[0] == '[' || 
        token->pszToken[0] == '{')
    {
        return true;
    }

    return false;
}

bool isBraceRight(token_t * token) {
    if (token->pszToken[0] == ')' || 
        token->pszToken[0] == ']' || 
        token->pszToken[0] == '}')
    {
        return true;
    }

    return false;
}

bool isOperator(token_t * token) {
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

bool isConstant(token_t * token) {
    return (isConstantPi(token) || isConstantC(token));
}

bool isFunction(token_t * token) {
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

associativity getOperatorAssociativity(token_t * t) {
    if (isOperatorPlus(t) || 
        isOperatorMinus(t) || 
        isOperatorMultiply(t) || 
        isOperatorDivide(t) || 
        isOperatorAND(t) || 
        isOperatorOR(t) || 
        isOperatorXOR(t))
    {
        return associativity_left;
    }
    else {
        return associativity_right;
    }
}

int getOperatorPrescedense(token_t * t) {
    int             prescedence;

    switch (t->type) {
        case token_operator_plus:
        case token_operator_minus:
            prescedence = 2;
            break;
        
        case token_operator_multiply:
        case token_operator_divide:
        case token_operator_mod:
            prescedence = 3;
            break;

        case token_operator_power:
        case token_operator_AND:
        case token_operator_OR:
        case token_operator_XOR:
            prescedence = 4;
            break;

        default:
            prescedence = 0;
            break;
    }

    return prescedence;
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

    token->pszToken = trim(strndup(&t->pszExpression[t->startIndex], token->length));
    token->length = strlen(token->pszToken);

    t->startIndex = t->endIndex;

    if (isOperand(t, token)) {
        token->type = token_operand;
    }
    else if (isOperatorPlus(token)) {
        token->type = token_operator_plus;
    }
    else if (isOperatorMinus(token)) {
        token->type = token_operator_minus;
    }
    else if (isOperatorMultiply(token)) {
        token->type = token_operator_multiply;
    }
    else if (isOperatorDivide(token)) {
        token->type = token_operator_divide;
    }
    else if (isOperatorMod(token)) {
        token->type = token_operator_mod;
    }
    else if (isOperatorPower(token)) {
        token->type = token_operator_power;
    }
    else if (isOperatorAND(token)) {
        token->type = token_operator_AND;
    }
    else if (isOperatorOR(token)) {
        token->type = token_operator_OR;
    }
    else if (isOperatorXOR(token)) {
        token->type = token_operator_XOR;
    }
    else if (isConstant(token)) {
        if (isConstantC(token)) {
            free(token->pszToken);

            token->type = token_operand;
            token->pszToken = strdup(CONSTANT_C);
            token->length = strlen(token->pszToken);
        }
        else if (isConstantPi(token)) {
            free(token->pszToken);

            token->type = token_operand;
            token->pszToken = strdup(CONSTANT_PI);
            token->length = strlen(token->pszToken);
        }
    }
    else if (isFunction(token)) {
        token->type = token_function;
    }
    else if (isBrace(token)) {
        token->type = token_brace;
    }

    return token;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "tokenizer.h"
#include "utils.h"

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

static bool isDecimalDigit(char ch) {
    return (isdigit(ch) || ch == '.' || ch == '-');
}

static bool isHexadecimalDigit(char ch) {
#ifdef __APPLE__
    return (ishexnumber(ch));
#else
    return (isxdigit(ch));
#endif
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

static bool isOperatorPlus(char * pszToken) {
    return (pszToken[0] == '+');
}

static bool isOperatorMinus(char * pszToken) {
    if (pszToken[0] == '-') {
        if (token->length > 1 && (isUniversalDigit(pszToken[1]))) {
            return false;
        }
    }
    else {
        return false;
    }

    return true;
}

static bool isOperatorMultiply(char * pszToken) {
    return (pszToken[0] == '*');
}

static bool isOperatorDivide(char * pszToken) {
    return (pszToken[0] == '/');
}

static bool isOperatorMod(char * pszToken) {
    return (pszToken[0] == '%');
}

static bool isOperatorPower(char * pszToken) {
    return (pszToken[0] == '^');
}

static bool isOperatorRoot(char * pszToken) {
    return (pszToken[0] == ':');
}

static bool isOperatorAND(char * pszToken) {
    return (pszToken[0] == '&');
}

static bool isOperatorOR(char * pszToken) {
    return (pszToken[0] == '|');
}

static bool isOperatorXOR(char * pszToken) {
    return (pszToken[0] == '~');
}

static bool isOperatorLeftShift(char * pszToken) {
    if (strncmp(pszToken, "<<", 2) == 0) {
        return true;
    }

    return false;
}

static bool isOperatorRightShift(char * pszToken) {
    if (strncmp(pszToken, ">>", 2) == 0) {
        return true;
    }

    return false;
}

static bool isConstantPi(char * pszToken) {
    if (strncmp(pszToken, "pi", 2) == 0) {
        return true;
    }

    return false;
}
static bool isConstantC(char * pszToken) {
    if (strncmp(pszToken, "c", 2) == 0) {
        return true;
    }

    return false;
}
static bool isConstantEuler(char * pszToken) {
    if (strncmp(pszToken, "eu", 2) == 0) {
        return true;
    }

    return false;
}
static bool isConstantGravity(char * pszToken) {
    if (strncmp(pszToken, "g", 2) == 0) {
        return true;
    }

    return false;
}

static bool isFunctionSine(char * pszToken) {
    return (strncmp(pszToken, "sin", 3) == 0);
}

static bool isFunctionCosine(char * pszToken) {
    return (strncmp(pszToken, "cos", 3) == 0);
}

static bool isFunctionTangent(char * pszToken) {
    return (strncmp(pszToken, "tan", 3) == 0);
}

static bool isFunctionArcSine(char * pszToken) {
    return (strncmp(pszToken, "asin", 4) == 0);
}

static bool isFunctionArcCosine(char * pszToken) {
    return (strncmp(pszToken, "acos", 4) == 0);
}

static bool isFunctionArcTangent(char * pszToken) {
    return (strncmp(pszToken, "atan", 4) == 0);
}

static bool isFunctionHyperbolicSine(char * pszToken) {
    return (strncmp(pszToken, "sinh", 4) == 0);
}

static bool isFunctionHyperbolicCosine(char * pszToken) {
    return (strncmp(pszToken, "cosh", 4) == 0);
}

static bool isFunctionHyperbolicTangent(char * pszToken) {
    return (strncmp(pszToken, "tanh", 4) == 0);
}

static bool isFunctionInvHyperbolicSine(char * pszToken) {
    return (strncmp(pszToken, "asinh", 5) == 0);
}

static bool isFunctionInvHyperbolicCosine(char * pszToken) {
    return (strncmp(pszToken, "acosh", 5) == 0);
}

static bool isFunctionInvHyperbolicTangent(char * pszToken) {
    return (strncmp(pszToken, "atanh", 5) == 0);
}

static bool isFunctionSquareRoot(char * pszToken) {
    return (strncmp(pszToken, "sqrt", 4) == 0);
}

static bool isFunctionLogarithm(char * pszToken) {
    return (strncmp(pszToken, "log", 3) == 0);
}

static bool isFunctionNaturalLog(char * pszToken) {
    return (strncmp(pszToken, "ln", 2) == 0);
}

static bool isFunctionFactorial(char * pszToken) {
    return (strncmp(pszToken, "fact", 4) == 0);
}

static bool isFunctionMemory(char * pszToken) {
    return (strncmp(pszToken, "mem", 3) == 0);
}


bool isOperand(tokenizer_t * t, char * pszToken, int tokenLength) {
    int		i;
    char    ch;

    if (pszToken[0] == '-' && tokenLength == 1) {
        // Must be the '-' operator...
        return false;
    }
    else {
        for (i = 0;i < tokenLength;i++) {
            ch = pszToken[i];

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

bool isBrace(char * pszToken) {
    int             i;

    for (i = 0;i < strlen(pszBraces);i++) {
        if (pszToken[0] == pszBraces[i]) {
            return true;
        }
    }

    return false;
}

bool isBraceLeft(char * pszToken) {
    if (pszToken[0] == '(' || 
        pszToken[0] == '[' || 
        pszToken[0] == '{')
    {
        return true;
    }

    return false;
}

bool isBraceRight(char * pszToken) {
    if (pszToken[0] == ')' || 
        pszToken[0] == ']' || 
        pszToken[0] == '}')
    {
        return true;
    }

    return false;
}

bool isOperator(char * pszToken) {
    return (
        isOperatorPlus(pszToken)       || 
        isOperatorMinus(pszToken)      || 
        isOperatorMultiply(pszToken)   || 
        isOperatorDivide(pszToken)     || 
        isOperatorMod(pszToken)        || 
        isOperatorPower(pszToken)      || 
        isOperatorRoot(pszToken)       ||
        isOperatorAND(pszToken)        || 
        isOperatorOR(pszToken)         || 
        isOperatorXOR(pszToken)        ||
        isOperatorLeftShift(pszToken)  ||
        isOperatorRightShift(pszToken));
}

bool isConstant(char * pszToken) {
    return (
        isConstantPi(pszToken)         || 
        isConstantC(pszToken)          ||
        isConstantEuler(pszToken)      ||
        isConstantGravity(pszToken));
}

bool isFunction(char * pszToken) {
    return (
        isFunctionSine(pszToken)       ||
        isFunctionCosine(pszToken)     ||
        isFunctionTangent(pszToken)    ||
        isFunctionArcSine(pszToken)    ||
        isFunctionArcCosine(pszToken)  ||
        isFunctionArcTangent(pszToken) ||
        isFunctionSquareRoot(pszToken) ||
        isFunctionLogarithm(pszToken)  ||
        isFunctionNaturalLog(pszToken) ||
        isFunctionFactorial(pszToken)  ||
        isFunctionMemory(pszToken));
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

    token = (token_t *)malloc(sizeof(token_t));

    if (token == NULL) {
        return NULL;
    }

    tokenLength = (t->endIndex - t->startIndex);

    pszToken = trim(strndup(&t->pszExpression[t->startIndex], tokenLength));
    tokenLength = strlen(pszToken);

    t->startIndex = t->endIndex;

    if (isOperand(t, token)) {
        token->type = token_operand;

        operand_t op;

        mpfr_init2(op.value, getBasePrecision());
        mpfr_strtofr(op.value, pszToken, NULL, getBase(), MPFR_RNDA);

        op.base = t->base;

        memcpy(&token->item.operand, &op, sizeof(operand_t));
    }
    else if (isOperator(pszToken)) {
        token->type = token_operator;

        operator_t op;

        if (isOperatorPlus(pszToken)) {
            op.ID = operator_plus;

            op.prescedence = 2;
            op.associativity = associativity_left;
        }
        else if (isOperatorMinus(pszToken)) {
            op.ID = operator_minus;

            op.prescedence = 2;
            op.associativity = associativity_left;
        }
        else if (isOperatorMultiply(pszToken)) {
            op.ID = operator_multiply;

            op.prescedence = 3;
            op.associativity = associativity_left;
        }
        else if (isOperatorDivide(pszToken)) {
            op.ID = operator_divide;

            op.prescedence = 3;
            op.associativity = associativity_left;
        }
        else if (isOperatorMod(pszToken)) {
            op.ID = operator_mod;

            op.prescedence = 3;
            op.associativity = associativity_left;
        }
        else if (isOperatorPower(pszToken)) {
            op.ID = operator_power;

            op.prescedence = 4;
            op.associativity = associativity_right;
        }
        else if (isOperatorRoot(pszToken)) {
            op.ID = operator_root;

            op.prescedence = 4;
            op.associativity = associativity_left;
        }
        else if (isOperatorAND(pszToken)) {
            op.ID = operator_AND;

            op.prescedence = 4;
            op.associativity = associativity_left;
        }
        else if (isOperatorOR(pszToken)) {
            op.ID = operator_OR;

            op.prescedence = 4;
            op.associativity = associativity_left;
        }
        else if (isOperatorXOR(pszToken)) {
            op.ID = operator_XOR;

            op.prescedence = 4;
            op.associativity = associativity_left;
        }
        else if (isOperatorLeftShift(pszToken)) {
            op.ID = operator_left_shift;

            op.prescedence = 4;
            op.associativity = associativity_right;
        }
        else if (isOperatorRightShift(pszToken)) {
            op.ID = operator_right_shift;

            op.prescedence = 4;
            op.associativity = associativity_right;
        }

        memcpy(&token->item.operator, &op, sizeof(operator_t));
    }
    else if (isConstant(pszToken)) {
        token->type = token_constant;

        constant_t c;

        if (isConstantPi(pszToken)) {
            c.ID = constant_pi;
        }
        else if (isConstantC(pszToken)) {
            c.ID = constant_c;
        }
        else if (isConstantEuler(pszToken)) {
            c.ID = constant_euler;
        }
        else if (isConstantGravity(pszToken)) {
            c.ID = constant_gravity;
        }

        memcpy(&token->item.constant, &c, sizeof(constant_t));
    }
    else if (isFunction(pszToken)) {
        token->type = token_function;

        function_t f;
        
        if (isFunctionSine(pszToken)) {
            f.ID = function_sin;
        }
        else if (isFunctionCosine(pszToken)) {
            f.ID = function_cos;
        }
        else if (isFunctionTangent(pszToken)) {
            f.ID = function_tan;
        }
        else if (isFunctionArcSine(pszToken)) {
            f.ID = function_asin;
        }
        else if (isFunctionArcCosine(pszToken)) {
            f.ID = function_acos;
        }
        else if (isFunctionArcTangent(pszToken)) {
            f.ID = function_atan;
        }
        else if (isFunctionHyperbolicSine(pszToken)) {
            f.ID = function_sinh;
        }
        else if (isFunctionHyperbolicCosine(pszToken)) {
            f.ID = function_cosh;
        }
        else if (isFunctionHyperbolicTangent(pszToken)) {
            f.ID = function_tanh;
        }
        else if (isFunctionInvHyperbolicSine(pszToken)) {
            f.ID = function_asinh;
        }
        else if (isFunctionInvHyperbolicCosine(pszToken)) {
            f.ID = function_acosh;
        }
        else if (isFunctionInvHyperbolicTangent(pszToken)) {
            f.ID = function_atanh;
        }
        else if (isFunctionSquareRoot(pszToken)) {
            f.ID = function_sqrt;
        }
        else if (isFunctionLogarithm(pszToken)) {
            f.ID = function_log;
        }
        else if (isFunctionNaturalLog(pszToken)) {
            f.ID = function_ln;
        }
        else if (isFunctionFactorial(pszToken)) {
            f.ID = function_fac;
        }
        else if (isFunctionMemory(pszToken)) {
            f.ID = function_mem;
        }

        memcpy(&token->item.function, &f, sizeof(function_t));
    }
    else if (isBrace(pszToken)) {
        token->type = token_brace;

        brace_t b;

        if (isBraceLeft(pszToken)) {
            b.ID = brace_left;
        }
        else if (isBraceRight(pszToken)) {
            b.ID = brace_right;
        }

        memcpy(&token->item.brace, &b, sizeof(brace_t));
    }

    return token;
}

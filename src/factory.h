#include <cstring>
#include <stdlib.h>
#include <ctype.h>

#include <gmp.h>
#include <mpfr.h>

#include "token.h"

using namespace std;

#ifndef __INCL_FACTORY
#define __INCL_FACTORY

class token_factory {
    private:
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

        static bool isOperatorPlus(const char * pszToken) {
            return (pszToken[0] == '+');
        }

        static bool isOperatorMinus(const char * pszToken) {
            if (pszToken[0] == '-') {
                if (strlen(pszToken) > 1 && (isUniversalDigit(pszToken[1]))) {
                    return false;
                }
            }
            else {
                return false;
            }

            return true;
        }

        static bool isOperatorMultiply(const char * pszToken) {
            return (pszToken[0] == '*');
        }

        static bool isOperatorDivide(const char * pszToken) {
            return (pszToken[0] == '/');
        }

        static bool isOperatorMod(const char * pszToken) {
            return (pszToken[0] == '%');
        }

        static bool isOperatorPower(const char * pszToken) {
            return (pszToken[0] == '^');
        }

        static bool isOperatorRoot(const char * pszToken) {
            return (pszToken[0] == ':');
        }

        static bool isOperatorAND(const char * pszToken) {
            return (pszToken[0] == '&');
        }

        static bool isOperatorOR(const char * pszToken) {
            return (pszToken[0] == '|');
        }

        static bool isOperatorXOR(const char * pszToken) {
            return (pszToken[0] == '~');
        }

        static bool isOperatorLeftShift(const char * pszToken) {
            if (strncmp(pszToken, "<<", 2) == 0) {
                return true;
            }

            return false;
        }

        static bool isOperatorRightShift(const char * pszToken) {
            if (strncmp(pszToken, ">>", 2) == 0) {
                return true;
            }

            return false;
        }

        static bool isConstantPi(const char * pszToken) {
            if (strncmp(pszToken, "pi", 2) == 0) {
                return true;
            }

            return false;
        }
        static bool isConstantC(const char * pszToken) {
            if (strncmp(pszToken, "c", 2) == 0) {
                return true;
            }

            return false;
        }
        static bool isConstantEuler(const char * pszToken) {
            if (strncmp(pszToken, "eu", 2) == 0) {
                return true;
            }

            return false;
        }
        static bool isConstantGravity(const char * pszToken) {
            if (strncmp(pszToken, "g", 2) == 0) {
                return true;
            }

            return false;
        }

        static bool isFunctionSine(const char * pszToken) {
            return (strncmp(pszToken, "sin", 3) == 0);
        }

        static bool isFunctionCosine(const char * pszToken) {
            return (strncmp(pszToken, "cos", 3) == 0);
        }

        static bool isFunctionTangent(const char * pszToken) {
            return (strncmp(pszToken, "tan", 3) == 0);
        }

        static bool isFunctionArcSine(const char * pszToken) {
            return (strncmp(pszToken, "asin", 4) == 0);
        }

        static bool isFunctionArcCosine(const char * pszToken) {
            return (strncmp(pszToken, "acos", 4) == 0);
        }

        static bool isFunctionArcTangent(const char * pszToken) {
            return (strncmp(pszToken, "atan", 4) == 0);
        }

        static bool isFunctionHyperbolicSine(const char * pszToken) {
            return (strncmp(pszToken, "sinh", 4) == 0);
        }

        static bool isFunctionHyperbolicCosine(const char * pszToken) {
            return (strncmp(pszToken, "cosh", 4) == 0);
        }

        static bool isFunctionHyperbolicTangent(const char * pszToken) {
            return (strncmp(pszToken, "tanh", 4) == 0);
        }

        static bool isFunctionInvHyperbolicSine(const char * pszToken) {
            return (strncmp(pszToken, "asinh", 5) == 0);
        }

        static bool isFunctionInvHyperbolicCosine(const char * pszToken) {
            return (strncmp(pszToken, "acosh", 5) == 0);
        }

        static bool isFunctionInvHyperbolicTangent(const char * pszToken) {
            return (strncmp(pszToken, "atanh", 5) == 0);
        }

        static bool isFunctionSquareRoot(const char * pszToken) {
            return (strncmp(pszToken, "sqrt", 4) == 0);
        }

        static bool isFunctionLogarithm(const char * pszToken) {
            return (strncmp(pszToken, "log", 3) == 0);
        }

        static bool isFunctionNaturalLog(const char * pszToken) {
            return (strncmp(pszToken, "ln", 2) == 0);
        }

        static bool isFunctionFactorial(const char * pszToken) {
            return (strncmp(pszToken, "fact", 4) == 0);
        }

        static bool isFunctionMemory(const char * pszToken) {
            return (strncmp(pszToken, "mem", 3) == 0);
        }

        static bool isBraceLeft(const char * pszToken) {
            return (pszToken[0] == '(' || pszToken[0] == '[' || pszToken[0] == '{');
        }

        static bool isBraceRight(const char * pszToken) {
            return (pszToken[0] == ')' || pszToken[0] == ']' || pszToken[0] == '}');
        }

    public:
        static bool isOperand(const char * pszToken) {
            int		i;
            int     tokenLength = strlen(pszToken);
            char    ch;

            if (pszToken[0] == '-' && tokenLength == 1) {
                // Must be the '-' operator...
                return false;
            }
            else {
                for (i = 0;i < tokenLength;i++) {
                    ch = pszToken[i];

                    switch (i) {
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

        static bool isBrace(const char * pszToken) {
            return (isBraceLeft(pszToken) ||
                    isBraceRight(pszToken));
        }

        static bool isConstant(const char * pszToken) {
            return (isConstantPi(pszToken) || 
                    isConstantEuler(pszToken) || 
                    isConstantC(pszToken) || 
                    isConstantGravity(pszToken));
        }

        static bool isOperator(const char * pszToken) {
            return (isOperatorPlus(pszToken) ||
                    isOperatorMinus(pszToken) ||
                    isOperatorMultiply(pszToken) ||
                    isOperatorDivide(pszToken) ||
                    isOperatorMod(pszToken) ||
                    isOperatorPower(pszToken) ||
                    isOperatorRoot(pszToken) ||
                    isOperatorAND(pszToken) ||
                    isOperatorOR(pszToken) ||
                    isOperatorXOR(pszToken) ||
                    isOperatorLeftShift(pszToken) ||
                    isOperatorRightShift(pszToken));
        }

        static bool isFunction(const char * pszToken) {
            return (isFunctionSine(pszToken) ||
                    isFunctionCosine(pszToken) ||
                    isFunctionTangent(pszToken) ||
                    isFunctionArcSine(pszToken) ||
                    isFunctionArcCosine(pszToken) ||
                    isFunctionArcTangent(pszToken) ||
                    isFunctionHyperbolicSine(pszToken) ||
                    isFunctionHyperbolicCosine(pszToken) ||
                    isFunctionHyperbolicTangent(pszToken) ||
                    isFunctionInvHyperbolicSine(pszToken) ||
                    isFunctionInvHyperbolicCosine(pszToken) ||
                    isFunctionInvHyperbolicTangent(pszToken) ||
                    isFunctionSquareRoot(pszToken) ||
                    isFunctionLogarithm(pszToken) ||
                    isFunctionNaturalLog(pszToken) ||
                    isFunctionFactorial(pszToken) ||
                    isFunctionMemory(pszToken));
        }

        static token_t * createToken(const char * pszToken) {
            if (isOperand(pszToken)) {
                return new operand_t(pszToken);
            }
            else if (isConstantPi(pszToken)) {
                return new constant_pi_t(pszToken);
            }
            else if (isConstantEuler(pszToken)) {
                return new constant_euler_t(pszToken);
            }
            else if (isConstantC(pszToken)) {
                return new constant_c_t(pszToken);
            }
            else if (isConstantGravity(pszToken)) {
                return new constant_G_t(pszToken);
            }
            else if (isBraceLeft(pszToken)) {
                return new brace_left_t(pszToken);
            }
            else if (isBraceRight(pszToken)) {
                return new brace_right_t(pszToken);
            }
            else if (isOperatorPlus(pszToken)) {
                return new operator_plus_t(pszToken);
            }
            else if (isOperatorMinus(pszToken)) {
                return new operator_minus_t(pszToken);
            }
            else if (isOperatorMultiply(pszToken)) {
                return new operator_multiply_t(pszToken);
            }
            else if (isOperatorDivide(pszToken)) {
                return new operator_divide_t(pszToken);
            }
            else if (isOperatorMod(pszToken)) {
                return new operator_modulus_t(pszToken);
            }
            else if (isOperatorPower(pszToken)) {
                return new operator_power_t(pszToken);
            }
            else if (isOperatorRoot(pszToken)) {
                return new operator_root_t(pszToken);
            }
            else if (isOperatorAND(pszToken)) {
                return new operator_AND_t(pszToken);
            }
            else if (isOperatorOR(pszToken)) {
                return new operator_OR_t(pszToken);
            }
            else if (isOperatorXOR(pszToken)) {
                return new operator_XOR_t(pszToken);
            }
            else if (isOperatorLeftShift(pszToken)) {
                return new operator_LSHIFT_t(pszToken);
            }
            else if (isOperatorRightShift(pszToken)) {
                return new operator_RSHIFT_t(pszToken);
            }
            else if (isFunctionSine(pszToken)) {
                return new function_sin_t(pszToken);
            }
            else if (isFunctionCosine(pszToken)) {
                return new function_cos_t(pszToken);
            }
            else if (isFunctionTangent(pszToken)) {
                return new function_tan_t(pszToken);
            }
            else if (isFunctionArcSine(pszToken)) {
                return new function_asin_t(pszToken);
            }
            else if (isFunctionArcCosine(pszToken)) {
                return new function_acos_t(pszToken);
            }
            else if (isFunctionArcTangent(pszToken)) {
                return new function_atan_t(pszToken);
            }
            else if (isFunctionHyperbolicSine(pszToken)) {
                return new function_sinh_t(pszToken);
            }
            else if (isFunctionHyperbolicCosine(pszToken)) {
                return new function_cosh_t(pszToken);
            }
            else if (isFunctionHyperbolicTangent(pszToken)) {
                return new function_tanh_t(pszToken);
            }
            else if (isFunctionInvHyperbolicSine(pszToken)) {
                return new function_asinh_t(pszToken);
            }
            else if (isFunctionInvHyperbolicCosine(pszToken)) {
                return new function_acosh_t(pszToken);
            }
            else if (isFunctionInvHyperbolicTangent(pszToken)) {
                return new function_atanh_t(pszToken);
            }
            else if (isFunctionFactorial(pszToken)) {
                return new function_factorial_t(pszToken);
            }
            else if (isFunctionSquareRoot(pszToken)) {
                return new function_sqrt_t(pszToken);
            }
            else if (isFunctionLogarithm(pszToken)) {
                return new function_logarithm_t(pszToken);
            }
            else if (isFunctionNaturalLog(pszToken)) {
                return new function_natural_log_t(pszToken);
            }
            else if (isFunctionMemory(pszToken)) {
                return new function_memory_t(pszToken);
            }
            else {
                return NULL;
            }
        }
};

#endif

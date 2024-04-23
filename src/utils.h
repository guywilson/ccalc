#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include <gmp.h>
#include <mpfr.h>

using namespace std;

#ifndef __INCL_UTILS
#define __INCL_UTILS

#define BASE2_OUTPUT_LEN                        (sizeof(uint32_t) * 8)

static const char * pszDigits = ".-0123456789abcdefABCDEF";
static const char * pszOperators = "+-*/%^:&|~<>";
static const char * pszLeftBraces = "({[";
static const char * pszRightBraces = "}])";
static const char * pszBraces = "({[]})";

class Utils {
    public:
        static string & lowercase(string & s ) {
            for (uint32_t i = 0;i < (uint32_t)s.length();i++) {
                s[i] = tolower(s[i]);
            }

            return s;
        }

        static bool isOperator(char token) {
            if (strchr(pszOperators, (int)token) != NULL) {
                return true;
            }

            return false;
        }

        static bool isBrace(char token) {
            if (strchr(pszBraces, (int)token) != NULL) {
                return true;
            }

            return false;
        }

        static bool isLeftBrace(char token) {
            if (strchr(pszLeftBraces, (int)token) != NULL) {
                return true;
            }

            return false;
        }

        static bool isRightBrace(char token) {
            if (strchr(pszRightBraces, (int)token) != NULL) {
                return true;
            }

            return false;
        }

        static bool isDigit(char token) {
            if (strchr(pszDigits, (int)token) != NULL) {
                return true;
            }

            return false;
        }

        static bool isOperand(string token) {
            if (token[0] == '-' && token.length() == 1) {
                return false;
            }
            else {
                for (uint32_t i = 0;i < (uint32_t)token.length();i++) {
                    if (!Utils::isDigit(token[i])) {
                        return false;
                    }
                }
            }

            return true;
        }

        static bool isConstant(string token) {
            Utils::lowercase(token);

            if (token.compare("pi") == 0) {
                return true;
            }
            else if (token.compare("eu") == 0) {
                return true;
            }
            else if (token.compare("g") == 0) {
                return true;
            }
            else if (token.compare("c") == 0) {
                return true;
            }

            return false;
        }

        static bool isFunction(string token) {
            Utils::lowercase(token);
            
            if (token.compare("sin") == 0) {
                return true;
            }
            else if (token.compare("cos") == 0) {
                return true;
            }
            else if (token.compare("tan") == 0) {
                return true;
            }
            else if (token.compare("asin") == 0) {
                return true;
            }
            else if (token.compare("acos") == 0) {
                return true;
            }
            else if (token.compare("atan") == 0) {
                return true;
            }
            else if (token.compare("sinh") == 0) {
                return true;
            }
            else if (token.compare("cosh") == 0) {
                return true;
            }
            else if (token.compare("tanh") == 0) {
                return true;
            }
            else if (token.compare("asinh") == 0) {
                return true;
            }
            else if (token.compare("acosh") == 0) {
                return true;
            }
            else if (token.compare("atanh") == 0) {
                return true;
            }
            else if (token.compare("sqrt") == 0) {
                return true;
            }
            else if (token.compare("log") == 0) {
                return true;
            }
            else if (token.compare("ln") == 0) {
                return true;
            }
            else if (token.compare("fact") == 0) {
                return true;
            }
            else if (token.compare("rad") == 0) {
                return true;
            }
            else if (token.compare("deg") == 0) {
                return true;
            }
            else if (token.compare("mem") == 0) {
                return true;
            }

            return false;
        }

        static char * getBase2String(uint32_t value) {
            char        szBinaryString[BASE2_OUTPUT_LEN + 1];
            char        szOutputString[BASE2_OUTPUT_LEN + 1];
            int         i = BASE2_OUTPUT_LEN - 1;
            int         bit = 0;

            szBinaryString[BASE2_OUTPUT_LEN] = 0;

            while (value > 0) {
                bit = value % 2;

                value = value >> 1;

                szBinaryString[i--] = ((char)bit + '0');
            }

            strncpy(szOutputString, &szBinaryString[i + 1], BASE2_OUTPUT_LEN);

            return strndup(szOutputString, BASE2_OUTPUT_LEN);
        }

        static void hexDump(void * buffer, uint32_t bufferLen)
        {
            uint32_t                i;
            int                     j = 0;
            uint8_t *               buf;
            static char             szASCIIBuf[17];
            static uint32_t         offset = 0;

            buf = (uint8_t *)buffer;

            for (i = 0;i < bufferLen;i++) {
                if ((i % 16) == 0) {
                    if (i != 0) {
                        szASCIIBuf[j] = 0;
                        j = 0;

                        printf("  |%s|\n", szASCIIBuf);
                    }
                        
                    printf("%08X\t", offset);
                    offset += 16;
                }

                if ((i % 2) == 0 && (i % 16) > 0) {
                    printf(" ");
                }

                printf("%02X", buf[i]);
                szASCIIBuf[j++] = isprint(buf[i]) ? buf[i] : '.';
            }

            /*
            ** Print final ASCII block...
            */
            szASCIIBuf[j] = 0;
            printf("  |%s|\n", szASCIIBuf);
        }

};

#endif

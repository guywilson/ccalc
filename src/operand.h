#include <string>
#include <cstring>

#include <gmp.h>
#include <mpfr.h>

#include "system.h"
#include "token.h"
#include "utils.h"
#include "logger.h"

using namespace std;

#ifndef __INCL_OPERAND
#define __INCL_OPERAND

#define FORMAT_STRING_LENGTH             32
#define OUTPUT_MAX_STRING_LENGTH        256

#define BASE_10                          10
#define BASE_16                          16
#define BASE_8                            8
#define BASE_2                            2

#define DECIMAL                         BASE_10
#define HEXADECIMAL                     BASE_16
#define OCTAL                           BASE_8
#define BINARY                          BASE_2

class operand_t : public token_t {
    private:
        mpfr_t      value;
        string      outputStr;
        string      formattedString;
        int         base;

        system_t &  sys = system_t::getInstance();

        void setBase(int b) {
            base = b;
        }

    protected:
        void setValue(const char * s) {
            mpfr_init2(value, getBasePrecision());
            mpfr_strtofr(value, s, NULL, sys.getBase(), MPFR_RNDA);

            setBase(sys.getBase());
        }

        void setValue(mpfr_ptr m) {
            mpfr_init2(value, getBasePrecision());
            mpfr_set(value, m, MPFR_RNDA);

            setBase(sys.getBase());

            setTokenStr(toString(sys.getBase()));
        }

        void setValue(operand_t * o) {
            setValue(o->getValue());
            setBase(o->getBase());
        }

        int getBase() {
            return base;
        }

    public:
        mpfr_ptr getValue() {
            return value;
        }

        operand_t & operator=(operand_t * o) {
            if (this == o) {
                return *this;
            }

            setValue(o);

            return *this;
        }

        operand_t(const char * s) : token_t(s) {
            setValue(s);
            setType(token_operand);
        }

        operand_t(string & s) : token_t(s) {
            setValue(s.c_str());
            setType(token_operand);
        }

        operand_t(mpfr_ptr v) {
            setValue(v);
            setType(token_operand);
        }

        operand_t(operand_t * o) {
            setValue(o);
            setType(token_operand);
        }

        operand_t() {
            mpfr_init2(value, getBasePrecision());
            setType(token_operand);
        }

        ~operand_t() {
            mpfr_clear(value);
        }

        string & toString(int outputBase) {
            char    szOutputString[OUTPUT_MAX_STRING_LENGTH];
            char    szFormatString[FORMAT_STRING_LENGTH];

            switch (outputBase) {
                case DECIMAL:
                    snprintf(szFormatString, FORMAT_STRING_LENGTH, "%%.%ldRf", (long)sys.getPrecision());
                    mpfr_snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, szFormatString, getValue());
                    break;

                case HEXADECIMAL:
                    snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, "%08lX", mpfr_get_ui(getValue(), MPFR_RNDA));
                    break;

                case OCTAL:
                    snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, "%lo", mpfr_get_ui(getValue(), MPFR_RNDA));
                    break;

                case BINARY:
                    char * binaryStr = getBase2String(mpfr_get_ui(getValue(), MPFR_RNDA));
                    snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, "%s", binaryStr);
                    free(binaryStr);
                    break;
            }

            outputStr.assign(szOutputString, OUTPUT_MAX_STRING_LENGTH);

            return outputStr;
        }

        string & toFormattedString(int outputBase) {
            const char *    pszIn;
            char *          pszOut;
            int             delim_char;
            int             delim_char_count = 0;
            int             startIndex = 0;
            int             tgtIndex = 0;
            int             srcIndex = 0;
            int             digitCount = 0;
            size_t          oldLength;
            size_t          newLength;
            bool            hasDelimiters = false;

            pszIn = toString(outputBase).c_str();
            oldLength = strlen(pszIn);
            newLength = oldLength;

            switch (outputBase) {
                case DECIMAL:
                    delim_char = ',';
                    delim_char_count = 3;
                    startIndex = outputStr.find_first_of('.') - 1;
                    break;

                case BINARY:
                case HEXADECIMAL:
                case OCTAL:
                    delim_char = ' ';
                    delim_char_count = 4;
                    startIndex = oldLength - 1;
                    break;

                default:
                    delim_char = ' ';
                    delim_char_count = 4;
                    startIndex = oldLength - 1;
            }

            if ((startIndex + 1) > delim_char_count) {
                hasDelimiters = true;
                newLength += (startIndex + 1) / delim_char_count;

                if ((startIndex + 1) % delim_char_count == 0) {
                    newLength--;
                }
            }

            lgLogDebug("toFormattedString(): oldLen:%u, newLen:%u, start:%d", oldLength, newLength, startIndex);

            pszOut = (char *)malloc(newLength);

            if (hasDelimiters) {
                tgtIndex = startIndex + (newLength - oldLength);
                srcIndex = startIndex;

                strncpy(&pszOut[newLength - oldLength], pszIn, oldLength);

                while (srcIndex >= 0) {
                    lgLogDebug("srcIndex:%d, tgtIndex:%d, digitCount:%d", srcIndex, tgtIndex, digitCount);
                    
                    if (digitCount == delim_char_count) {
                        pszOut[tgtIndex--] = delim_char;
                        digitCount = 0;
                    }
                    
                    pszOut[tgtIndex--] = pszIn[srcIndex--];
                    digitCount++;
                }
            }
            else {
                strncpy(pszOut, pszIn, oldLength);
            }

            formattedString.assign(pszOut);

            return formattedString;
        }
};

#endif

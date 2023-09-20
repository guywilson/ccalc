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
            char    szFormatStr[FORMAT_STRING_LENGTH];
            char    s[OUTPUT_MAX_STRING_LENGTH];

            mpfr_init2(value, getBasePrecision());
            mpfr_set(value, m, MPFR_RNDA);

            snprintf(szFormatStr, FORMAT_STRING_LENGTH, "%%.%ldRf", (long)sys.getPrecision());
            mpfr_sprintf(s, szFormatStr, m);
            setTokenStr(s);
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
                    snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, "%016lo", mpfr_get_ui(getValue(), MPFR_RNDA));
                    break;

                case BINARY:
                    snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, "%s", getBase2String(mpfr_get_ui(getValue(), MPFR_RNDA)));
                    break;
            }

            outputStr.assign(szOutputString, OUTPUT_MAX_STRING_LENGTH);

            return outputStr;
        }

        string & toFormattedString(int outputBase) {
            size_t              allocateLen = 0;
            char *              pszFormattedResult;
            int                 i;
            int                 j;
            char                delimiter = 0;
            int                 delimPos = 0;
            int                 digitCount = 0;
            int                 delimRangeLimit = 0;

            formattedString.assign(toString(outputBase));

            allocateLen = formattedString.length();

            switch (outputBase) {
                case DECIMAL:
                    if (allocateLen > 3) {
                        i = formattedString.find_first_of('.');

                        if (i < (int)formattedString.length()) {
                            allocateLen += (i - 1) / 3;
                            delimRangeLimit = i - 1;
                        }
                    }

                    delimiter = ',';
                    delimPos = 3;
                    break;

                case BINARY:
                    if (allocateLen > 4) {
                        allocateLen += ((allocateLen / 4) - 1);
                    }

                    delimiter = ' ';
                    delimPos = 4;
                    break;

                case OCTAL:
                    delimiter = 0;
                    break;

                case HEXADECIMAL:
                    if (allocateLen > 4) {
                        allocateLen += ((allocateLen / 4));
                    }

                    delimiter = ' ';
                    delimPos = 4;
                    break;
            }

            pszFormattedResult = (char *)malloc(allocateLen + 1);
            memset(pszFormattedResult, delimiter, allocateLen);

            pszFormattedResult[allocateLen] = 0;

            if (allocateLen > getLength() && outputBase != OCTAL) {
                if (outputBase != DECIMAL) {
                    delimRangeLimit = getLength() - 1;
                }
                
                i = allocateLen - 1;
                j = getLength() - 1;

                digitCount = delimPos;

                while (j >= 0) {
                    pszFormattedResult[i] = formattedString[j];

                    if (j <= delimRangeLimit) {
                        digitCount--;

                        if (digitCount == 0) {
                            i--;
                            digitCount = delimPos;
                        }
                    }

                    i--;
                    j--;
                }
            }
            else {
                strncpy(pszFormattedResult, getTokenStr().c_str(), getLength());
            }

            formattedString.assign(pszFormattedResult, allocateLen);

            return formattedString;
        }
};

#endif

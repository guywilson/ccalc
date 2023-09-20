#include <string>
#include <cstring>

#include <gmp.h>
#include <mpfr.h>

#include "system.h"
#include "token.h"
#include "logger.h"

using namespace std;

#ifndef __INCL_OPERAND
#define __INCL_OPERAND

#define BASE_10                 10
#define BASE_16                 16
#define BASE_8                   8
#define BASE_2                   2

#define DECIMAL                 BASE_10
#define HEXADECIMAL             BASE_16
#define OCTAL                   BASE_8
#define BINARY                  BASE_2

class operand_t : public token_t {
    private:
        mpfr_t      value;
        int         base;

    protected:
        void setValue(const char * s) {
            mpfr_init2(value, getBasePrecision());
            mpfr_strtofr(value, s, NULL, 10, MPFR_RNDA);
        }

        void setValue(mpfr_ptr m) {
            char    szFormatStr[32];
            char    s[128];

            mpfr_init2(value, getBasePrecision());
            mpfr_set(value, m, MPFR_RNDA);

            system_t & sys = system_t::getInstance();
            snprintf(szFormatStr, 32, "%%.%ldRf", (long)sys.getPrecision());
            mpfr_sprintf(s, szFormatStr, m);
            setTokenStr(s);
        }

        void setValue(operand_t * o) {
            setValue(o->getValue());
        }

        int getBase() {
            return base;
        }

        void setBase(int b) {
            base = b;
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

        char * formattedString(int outputBase) {
            size_t              allocateLen = 0;
            char *              pszFormattedResult;
            int                 i;
            int                 j;
            char                delimiter = 0;
            int                 delimPos = 0;
            int                 digitCount = 0;
            int                 delimRangeLimit = 0;

            allocateLen = getLength();

            switch (outputBase) {
                case DECIMAL:
                    if (allocateLen > 3) {
                        i = getTokenStr().find_first_of('.');

                        if (i < (int)getLength()) {
                            allocateLen += (i - 1) / 3;
                            delimRangeLimit = i - 1;
                        }
                    }

                    delimiter = ',';
                    delimPos = 3;
                    break;

                case BINARY:
                    if (allocateLen > 8) {
                        allocateLen += ((allocateLen / 8) - 1);
                    }

                    delimiter = ' ';
                    delimPos = 8;
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
                    pszFormattedResult[i] = getTokenStr()[j];

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

            return pszFormattedResult;
        }
};

#endif

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
};

#endif

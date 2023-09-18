#include <string>

#include <gmp.h>
#include <mpfr.h>

#include "system.h"
#include "token.h"
#include "operand.h"
#include "logger.h"

using namespace std;

#ifndef __INCL_CONSTANT
#define __INCL_CONSTANT

#define CONSTANT_C                          299792458U
#define CONSTANT_G                          "0.000000000066743"

class constant_t : public operand_t {
    public:
        constant_t(string & s) : operand_t(s) {
            setType(token_constant);
        }
        constant_t(const char * s) : operand_t(s) {
            setType(token_constant);
        }

        virtual operand_t * evaluate(void) {
            return NULL;
        }
};

class constant_pi_t : public constant_t {
    public:
        constant_pi_t(string & s) : constant_t(s) {}
        constant_pi_t(const char * s) : constant_t(s) {}

        operand_t * evaluate(void) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_const_pi(r, MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
            return result;
        }
};

class constant_euler_t : public constant_t {
    public:
        constant_euler_t(string & s) : constant_t(s) {}
        constant_euler_t(const char * s) : constant_t(s) {}

        operand_t * evaluate(void) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_const_euler(r, MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
            return result;
        }
};

class constant_c_t : public constant_t {
    public:
        constant_c_t(string & s) : constant_t(s) {}
        constant_c_t(const char * s) : constant_t(s) {}

        operand_t * evaluate(void) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_set_ui(r, CONSTANT_C, MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
            return result;
        }
};

class constant_G_t : public constant_t {
    public:
        constant_G_t(string & s) : constant_t(s) {}
        constant_G_t(const char * s) : constant_t(s) {}

        operand_t * evaluate(void) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_set_str(r, CONSTANT_G, getBase(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
            return result;
        }
};

#endif

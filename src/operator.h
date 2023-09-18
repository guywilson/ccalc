#include <string>

#include <gmp.h>
#include <mpfr.h>

#include "system.h"
#include "token.h"
#include "operand.h"
#include "logger.h"

using namespace std;

#ifndef __INCL_OPERATOR
#define __INCL_OPERATOR

class operator_t : public token_t {
    public:
        enum associativity {
            left,
            right
        };

        operator_t(string & s) : token_t(s) {
            setType(token_operator);
        }
        operator_t(const char * s) : token_t(s) {
            setType(token_operator);
        }

        virtual associativity getAssociativity() {
            return left;
        }
        virtual int getPrescedence() {
            return 0;
        }

        virtual operand_t * evaluate(operand_t * o1, operand_t * o2) {
            return NULL;
        }
};

class operator_plus_t : public operator_t {
    public:
        operator_plus_t(string & s) : operator_t(s) {}
        operator_plus_t(const char * s) : operator_t(s) {}

        operand_t * evaluate(operand_t * o1, operand_t * o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_add(r, o1->getValue(), o2->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);

            return result;
        }

        associativity getAssociativity() {
            return left;
        }

        int getPrescedence() {
            return 2;
        }
};

class operator_minus_t : public operator_t {
    public:
        operator_minus_t(string & s) : operator_t(s) {}
        operator_minus_t(const char * s) : operator_t(s) {}

        operand_t * evaluate(operand_t * o1, operand_t * o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_sub(r, o1->getValue(), o2->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
            return result;
        }

        associativity getAssociativity() {
            return left;
        }

        int getPrescedence() {
            return 2;
        }
};

class operator_multiply_t : public operator_t {
    public:
        operator_multiply_t(string & s) : operator_t(s) {}
        operator_multiply_t(const char * s) : operator_t(s) {}

        operand_t * evaluate(operand_t * o1, operand_t * o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_mul(r, o1->getValue(), o2->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
            return result;
        }

        associativity getAssociativity() {
            return left;
        }

        int getPrescedence() {
            return 3;
        }
};

class operator_divide_t : public operator_t {
    public:
        operator_divide_t(string & s) : operator_t(s) {}
        operator_divide_t(const char * s) : operator_t(s) {}

        operand_t * evaluate(operand_t * o1, operand_t * o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_div(r, o1->getValue(), o2->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
            return result;
        }

        associativity getAssociativity() {
            return left;
        }

        int getPrescedence() {
            return 3;
        }
};

class operator_modulus_t : public operator_t {
    public:
        operator_modulus_t(string & s) : operator_t(s) {}
        operator_modulus_t(const char * s) : operator_t(s) {}

        operand_t * evaluate(operand_t * o1, operand_t * o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_remainder(r, o1->getValue(), o2->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
            return result;
        }

        associativity getAssociativity() {
            return left;
        }

        int getPrescedence() {
            return 3;
        }
};

class operator_power_t : public operator_t {
    public:
        operator_power_t(string & s) : operator_t(s) {}
        operator_power_t(const char * s) : operator_t(s) {}

        operand_t * evaluate(operand_t * o1, operand_t * o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_pow(r, o1->getValue(), o2->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
            return result;
        }

        associativity getAssociativity() {
            return right;
        }

        int getPrescedence() {
            return 4;
        }
};

class operator_root_t : public operator_t {
    public:
        operator_root_t(string & s) : operator_t(s) {}
        operator_root_t(const char * s) : operator_t(s) {}

        operand_t * evaluate(operand_t * o1, operand_t * o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_rootn_ui(r, o1->getValue(), mpfr_get_ui(o2->getValue(), MPFR_RNDA), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
            return result;
        }

        associativity getAssociativity() {
            return left;
        }

        int getPrescedence() {
            return 4;
        }
};

class operator_AND_t : public operator_t {
    public:
        operator_AND_t(string & s) : operator_t(s) {}
        operator_AND_t(const char * s) : operator_t(s) {}

        operand_t * evaluate(operand_t * o1, operand_t * o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_set_ui(
                    r, 
                    (mpfr_get_ui(o1->getValue(), MPFR_RNDA) & 
                    mpfr_get_ui(o2->getValue(), MPFR_RNDA)), 
                    MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
            return result;
        }

        associativity getAssociativity() {
            return left;
        }

        int getPrescedence() {
            return 4;
        }
};

class operator_OR_t : public operator_t {
    public:
        operator_OR_t(string & s) : operator_t(s) {}
        operator_OR_t(const char * s) : operator_t(s) {}

        operand_t * evaluate(operand_t * o1, operand_t * o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_set_ui(
                    r, 
                    (mpfr_get_ui(o1->getValue(), MPFR_RNDA) | 
                    mpfr_get_ui(o2->getValue(), MPFR_RNDA)), 
                    MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
            return result;
        }

        associativity getAssociativity() {
            return left;
        }

        int getPrescedence() {
            return 4;
        }
};

class operator_XOR_t : public operator_t {
    public:
        operator_XOR_t(string & s) : operator_t(s) {}
        operator_XOR_t(const char * s) : operator_t(s) {}

        operand_t * evaluate(operand_t * o1, operand_t * o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_set_ui(
                    r, 
                    (mpfr_get_ui(o1->getValue(), MPFR_RNDA) ^ 
                    mpfr_get_ui(o2->getValue(), MPFR_RNDA)), 
                    MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
            return result;
        }

        associativity getAssociativity() {
            return left;
        }

        int getPrescedence() {
            return 4;
        }
};

class operator_LSHIFT_t : public operator_t {
    public:
        operator_LSHIFT_t(string & s) : operator_t(s) {}
        operator_LSHIFT_t(const char * s) : operator_t(s) {}

        operand_t * evaluate(operand_t * o1, operand_t * o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_set_ui(
                    r, 
                    (mpfr_get_ui(o1->getValue(), MPFR_RNDA) << 
                    mpfr_get_ui(o2->getValue(), MPFR_RNDA)), 
                    MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
            return result;
        }

        associativity getAssociativity() {
            return right;
        }

        int getPrescedence() {
            return 4;
        }
};

class operator_RSHIFT_t : public operator_t {
    public:
        operator_RSHIFT_t(string & s) : operator_t(s) {}
        operator_RSHIFT_t(const char * s) : operator_t(s) {}

        operand_t * evaluate(operand_t * o1, operand_t * o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_set_ui(
                    r, 
                    (mpfr_get_ui(o1->getValue(), MPFR_RNDA) >> 
                    mpfr_get_ui(o2->getValue(), MPFR_RNDA)), 
                    MPFR_RNDA);

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
            return result;
        }

        associativity getAssociativity() {
            return right;
        }

        int getPrescedence() {
            return 4;
        }
};

#endif

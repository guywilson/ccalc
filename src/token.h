#include <string>
#include <cstring>

#include <gmp.h>
#include <mpfr.h>

#include "utils.h"
#include "logger.h"

using namespace std;

#ifndef __INCL_TOKEN
#define __INCL_TOKEN

#define getBasePrecision()                  1024L

#define CONSTANT_C                          299792458U
#define CONSTANT_G                          "0.000000000066743"

#define BASE_10                 10
#define BASE_16                 16
#define BASE_8                   8
#define BASE_2                   2

#define DECIMAL                 BASE_10
#define HEXADECIMAL             BASE_16
#define OCTAL                   BASE_8
#define BINARY                  BASE_2

typedef enum {
    token_operator,
    token_function,
    token_constant,
    token_brace,
    token_operand
}
token_type_t;

/*
** Declare base classes...
*/
class token_t;
class brace_t;
class operator_t;
class function_t;
class operand_t;
class constant_t;

class token_t {
    private:
        token_type_t    type;
        
        string  tokenString;
        int     length;

    protected:
        token_t() {}

        void setType(token_type_t t) {
            type = t;
        }

    public:
        token_type_t getType() {
            return type;
        }

        static const char * getTokenTypeStr(token_t * t) {
            if (t->getType() == token_brace) {
                return "brace";
            }
            else if (t->getType() == token_constant) {
                return "constant";
            }
            else if (t->getType() == token_function) {
                return "function";
            }
            else if (t->getType() == token_operand) {
                return "operand";
            }
            else if (t->getType() == token_operator) {
                return "operator";
            }
            else {
                return "unknown";
            }
        }

        string & getTokenStr() {
            return tokenString;
        }

        void setTokenStr(string & s) {
            tokenString.assign(s);
            length = (int)s.length();
        }

        void setTokenStr(const char * s) {
            tokenString.assign(s);
            length = (int)strlen(s);
        }

        int getLength() {
            return length;
        }

        token_t(string & s) {
            setTokenStr(s);
        }

        token_t(const char * s) {
            setTokenStr(s);
        }

        token_t(token_t & t) {
            setTokenStr(t.getTokenStr());
        }
};

class brace_t : public token_t {
    public:
        brace_t(string & s) : token_t(s) {
            setType(token_brace);
        }
        brace_t(const char * s) : token_t(s) {
            setType(token_brace);
        }

        virtual bool isBraceLeft() = 0;
        virtual bool isBraceRight() = 0;
};

class brace_left_t : public brace_t {
    public:
        brace_left_t(string & s) : brace_t(s) {}
        brace_left_t(const char * s) : brace_t(s) {}

        bool isBraceLeft() {
            return true;
        }

        bool isBraceRight() {
            return false;
        }
};

class brace_right_t : public brace_t {
    public:
        brace_right_t(string & s) : brace_t(s) {}
        brace_right_t(const char * s) : brace_t(s) {}

        bool isBraceLeft() {
            return false;
        }

        bool isBraceRight() {
            return true;
        }
};

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
            mpfr_init2(value, getBasePrecision());
            mpfr_set(value, m, MPFR_RNDA);
        }

        int getBase() {
            return base;
        }

        void setBase(int b) {
            base = b;
        }

    public:
        operand_t(const char * s) : token_t(s) {
            setValue(s);
            setType(token_operand);
        }

        operand_t(string & s) : token_t(s) {
            setValue(s.c_str());
            setType(token_operand);
        }

        operand_t(mpfr_ptr v) {
            char    szFormatStr[32];
            char    s[128];

            setValue(v);
            setType(token_operand);

            snprintf(szFormatStr, 32, "%%.%ldRf", (long)getPrecision());
            mpfr_sprintf(s, szFormatStr, v);
            setTokenStr(s);
        }

        operand_t(operand_t & o) {
            setValue(o.getValue());
            setType(token_operand);
        }

        operand_t() {
            mpfr_init2(value, getBasePrecision());
            setType(token_operand);
        }

        mpfr_ptr getValue() {
            return value;
        }
};

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

            return result;
        }

        associativity getAssociativity() {
            return right;
        }

        int getPrescedence() {
            return 4;
        }
};

class function_t : public operator_t {
    public:
        function_t(string & s) : operator_t(s) {
            setType(token_function);
        }
        function_t(const char * s) : operator_t(s) {
            setType(token_function);
        }

        int getPrescedence() {
            return 5;
        }

        virtual operand_t * evaluate(operand_t * o1) {
            return NULL;
        }

        virtual operand_t * evaluate(operand_t * o1, operand_t * o2) {
            return NULL;
        }
};

class function_sin_t : public function_t {
    public:
        function_sin_t(string & s) : function_t(s) {}
        function_sin_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_sin(r, o1->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_cos_t : public function_t {
    public:
        function_cos_t(string & s) : function_t(s) {}
        function_cos_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_cos(r, o1->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_tan_t : public function_t {
    public:
        function_tan_t(string & s) : function_t(s) {}
        function_tan_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_tan(r, o1->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_asin_t : public function_t {
    public:
        function_asin_t(string & s) : function_t(s) {}
        function_asin_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_asin(r, o1->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_acos_t : public function_t {
    public:
        function_acos_t(string & s) : function_t(s) {}
        function_acos_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_acos(r, o1->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_atan_t : public function_t {
    public:
        function_atan_t(string & s) : function_t(s) {}
        function_atan_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_atan(r, o1->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_sinh_t : public function_t {
    public:
        function_sinh_t(string & s) : function_t(s) {}
        function_sinh_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_sinh(r, o1->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_cosh_t : public function_t {
    public:
        function_cosh_t(string & s) : function_t(s) {}
        function_cosh_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_cosh(r, o1->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_tanh_t : public function_t {
    public:
        function_tanh_t(string & s) : function_t(s) {}
        function_tanh_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_tanh(r, o1->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_asinh_t : public function_t {
    public:
        function_asinh_t(string & s) : function_t(s) {}
        function_asinh_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_asinh(r, o1->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_acosh_t : public function_t {
    public:
        function_acosh_t(string & s) : function_t(s) {}
        function_acosh_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_cos(r, o1->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_atanh_t : public function_t {
    public:
        function_atanh_t(string & s) : function_t(s) {}
        function_atanh_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_tan(r, o1->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_sqrt_t : public function_t {
    public:
        function_sqrt_t(string & s) : function_t(s) {}
        function_sqrt_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_sqrt(r, o1->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_factorial_t : public function_t {
    public:
        function_factorial_t(string & s) : function_t(s) {}
        function_factorial_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_fac_ui(r, mpfr_get_ui(o1->getValue(), MPFR_RNDA), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_logarithm_t : public function_t {
    public:
        function_logarithm_t(string & s) : function_t(s) {}
        function_logarithm_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_log10(r, o1->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_natural_log_t : public function_t {
    public:
        function_natural_log_t(string & s) : function_t(s) {}
        function_natural_log_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_log(r, o1->getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_memory_t : public function_t {
    public:
        function_memory_t(string & s) : function_t(s) {}
        function_memory_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());

            operand_t * result = new operand_t(r);

            return result;
        }
};

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

            return result;
        }
};

#endif

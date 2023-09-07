#include <string>
#include <cstring>

#include <gmp.h>
#include <mpfr.h>

using namespace std;

#ifndef __INCL_TOKEN
#define __INCL_TOKEN

#define getBasePrecision()                  1024L

#define CONSTANT_C                          299792458U
#define CONSTANT_G                          "0.000000000066743"

/*
** Declare base classes...
*/
class token_t;
class brace_t;
class operator_t;
class function_t;
class operand_t;
class constant_t;

class _evaluatable_t {
    public:
        virtual operand_t * evaluate() = 0;
        virtual operand_t * evaluate(operand_t & o1) = 0;
        virtual operand_t * evaluate(operand_t & o1, operand_t & o2) = 0;
};

class token_t {
    private:
        string  tokenString;
        int     length;

        void identifyType(const char * s) {
            if (s[0] == '+' || s[0] == '-') {

            }
        }

    protected:
        token_t();

        string & getTokenStr() {
            return tokenString;
        }

        int getLength() {
            return length;
        }

    public:
        void setTokenStr(string & s) {
            tokenString.assign(s);
            length = (int)s.length();
        }

        void setTokenStr(const char * s) {
            tokenString.assign(s);
            length = (int)strlen(s);
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
        virtual bool isLeftBrace() {
            return false;
        }

        virtual bool isRightBrace() {
            return false;
        }
};

class brace_left_t : public brace_t {
    public:
        bool isBraceLeft() {
            return true;
        }
};

class brace_right_t : public brace_t {
    public:
        bool isBraceRight() {
            return true;
        }
};

class operator_t : public token_t, _evaluatable_t {
    public:
        enum op {
            plus,
            minus,
            multiply,
            divide,
            modulus,
            power,
            root,
            AND,
            OR,
            XOR,
            LSHIFT,
            RSHIFT
        };

        static bool isOperator(const char * s) {
            const char * basicOps = "+-*/%^:&|~";

            for (int i = 0;i < strlen(basicOps);i++) {
                if (s[0] == basicOps[i]) {
                    return true;
                }
            }

            if (strncmp(s, "<<", 2) == 0) {
                return true;
            }
            else if (strncmp(s, "<<", 2) == 0) {
                return true;
            }

            return false;
        }

        op getOperator() {
            op o;
            string s = getTokenStr();

            switch (s[0]) {
                case '+':
                    o = plus;
                    break;

                case '-':
                    o = minus;
                    break;

                case '*':
                    o = multiply;
                    break;

                case '/':
                    o = divide;
                    break;

                case '%':
                    o = modulus;
                    break;

                case '^':
                    o = power;
                    break;

                case ':':
                    o = root;
                    break;

                case '&':
                    o = AND;
                    break;

                case '|':
                    o = OR;
                    break;

                case '~':
                    o = XOR;
                    break;
            }

            if (s.compare("<<") == 0) {
                o = LSHIFT;
            }
            else if (s.compare(">>") == 0) {
                o = RSHIFT;
            }

            return o;
        }
};

class operator_plus_t : public operator_t {
    public:
        operand_t * evaluate(operand_t & o1, operand_t & o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_add(r, o1.getValue(), o2.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class operator_minus_t : public operator_t {
    public:
        operand_t * evaluate(operand_t & o1, operand_t & o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_sub(r, o1.getValue(), o2.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class operator_multiply_t : public operator_t {
    public:
        operand_t * evaluate(operand_t & o1, operand_t & o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_mul(r, o1.getValue(), o2.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class operator_divide_t : public operator_t {
    public:
        operand_t * evaluate(operand_t & o1, operand_t & o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_div(r, o1.getValue(), o2.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class operator_modulus_t : public operator_t {
    public:
        operand_t * evaluate(operand_t & o1, operand_t & o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_remainder(r, o1.getValue(), o2.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class operator_power_t : public operator_t {
    public:
        operand_t * evaluate(operand_t & o1, operand_t & o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_pow(r, o1.getValue(), o2.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class operator_root_t : public operator_t {
    public:
        operand_t * evaluate(operand_t & o1, operand_t & o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_rootn_ui(r, o1.getValue(), mpfr_get_ui(o2.getValue(), MPFR_RNDA), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class operator_AND_t : public operator_t {
    public:
        operand_t * evaluate(operand_t & o1, operand_t & o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_set_ui(
                    r, 
                    (mpfr_get_ui(o1.getValue(), MPFR_RNDA) & 
                    mpfr_get_ui(o2.getValue(), MPFR_RNDA)), 
                    MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class operator_OR_t : public operator_t {
    public:
        operand_t * evaluate(operand_t & o1, operand_t & o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_set_ui(
                    r, 
                    (mpfr_get_ui(o1.getValue(), MPFR_RNDA) | 
                    mpfr_get_ui(o2.getValue(), MPFR_RNDA)), 
                    MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class operator_XOR_t : public operator_t {
    public:
        operand_t * evaluate(operand_t & o1, operand_t & o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_set_ui(
                    r, 
                    (mpfr_get_ui(o1.getValue(), MPFR_RNDA) ^ 
                    mpfr_get_ui(o2.getValue(), MPFR_RNDA)), 
                    MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class operator_LSHIFT_t : public operator_t {
    public:
        operand_t * evaluate(operand_t & o1, operand_t & o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_set_ui(
                    r, 
                    (mpfr_get_ui(o1.getValue(), MPFR_RNDA) << 
                    mpfr_get_ui(o2.getValue(), MPFR_RNDA)), 
                    MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class operator_RSHIFT_t : public operator_t {
    public:
        operand_t * evaluate(operand_t & o1, operand_t & o2) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_set_ui(
                    r, 
                    (mpfr_get_ui(o1.getValue(), MPFR_RNDA) >> 
                    mpfr_get_ui(o2.getValue(), MPFR_RNDA)), 
                    MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_t : public operator_t, _evaluatable_t {
    public:
        enum func {
            sin,
            cos,
            tan,
            asin,
            acos,
            atan,
            sinh,
            cosh,
            tanh,
            asinh,
            acosh,
            atanh,
            sqrt,
            factorial,
            memory
        };

        static bool isFunction(const char * s) {

        }

        func getFunction() {
            func f;
            string s = getTokenStr();

            if (s.compare("sin") == 0) {
                f = sin;
            }
            else if (s.compare("cos") == 0) {
                f = cos;
            }
            else if (s.compare("tan") == 0) {
                f = tan;
            }
            else if (s.compare("asin") == 0) {
                f = asin;
            }
            else if (s.compare("acos") == 0) {
                f = acos;
            }
            else if (s.compare("atan") == 0) {
                f = atan;
            }
            else if (s.compare("sinh") == 0) {
                f = sinh;
            }
            else if (s.compare("cosh") == 0) {
                f = cosh;
            }
            else if (s.compare("tanh") == 0) {
                f = tanh;
            }
            else if (s.compare("asinh") == 0) {
                f = asinh;
            }
            else if (s.compare("acosh") == 0) {
                f = acosh;
            }
            else if (s.compare("atanh") == 0) {
                f = atanh;
            }
            else if (s.compare("sqrt") == 0) {
                f = sqrt;
            }
            else if (s.compare("fac") == 0) {
                f = factorial;
            }
            else if (s.compare("mem") == 0) {
                f = memory;
            }

            return f;
        }
};

class function_sin_t : public function_t {
    public:
        operand_t * evaluate(operand_t & o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_sin(r, o1.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_cos_t : public function_t {
    public:
        operand_t * evaluate(operand_t & o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_cos(r, o1.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_tan_t : public function_t {
    public:
        operand_t * evaluate(operand_t & o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_tan(r, o1.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_asin_t : public function_t {
    public:
        operand_t * evaluate(operand_t & o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_asin(r, o1.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_acos_t : public function_t {
    public:
        operand_t * evaluate(operand_t & o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_acos(r, o1.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_atan_t : public function_t {
    public:
        operand_t * evaluate(operand_t & o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_atan(r, o1.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_sinh_t : public function_t {
    public:
        operand_t * evaluate(operand_t & o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_sinh(r, o1.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_cosh_t : public function_t {
    public:
        operand_t * evaluate(operand_t & o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_cosh(r, o1.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_tanh_t : public function_t {
    public:
        operand_t * evaluate(operand_t & o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_tanh(r, o1.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_asinh_t : public function_t {
    public:
        operand_t * evaluate(operand_t & o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_asinh(r, o1.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_acosh_t : public function_t {
    public:
        operand_t * evaluate(operand_t & o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_cos(r, o1.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_atanh_t : public function_t {
    public:
        operand_t * evaluate(operand_t & o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_tan(r, o1.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_sqrt_t : public function_t {
    public:
        operand_t * evaluate(operand_t & o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_sqrt(r, o1.getValue(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_factorial_t : public function_t {
    public:
        operand_t * evaluate(operand_t & o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_fac_ui(r, mpfr_get_ui(o1.getValue(), MPFR_RNDA), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

class function_memory_t : public function_t {
    public:
        operand_t * evaluate(operand_t & o1) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());

            operand_t * result = new operand_t(r);

            return result;
        }
};

class operand_t : public token_t {
    private:
        mpfr_t      value;
        mpfr_prec_t basePrecision;

    protected:
        void setValue(const char * s) {
            mpfr_init2(value, getBasePrecision());
            mpfr_strtofr(value, s, NULL, 10, MPFR_RNDA);
        }

        void setValue(mpfr_ptr m) {
            mpfr_set(value, m, MPFR_RNDA);
        }

    public:
        operand_t(const char * s) : token_t(s) {
            setValue(s);
        }

        operand_t(string & s) : token_t(s) {
            setValue(s.c_str());
        }

        operand_t(mpfr_ptr v) {
            setValue(v);
        }

        operand_t(operand_t & o) {
            setValue(o.getValue());
        }

        operand_t() {
            mpfr_init2(value, getBasePrecision());
        }

        mpfr_ptr getValue() {
            return value;
        }
};

class constant_t : public operand_t, _evaluatable_t {
    public:
        constant_t(string & s) {
            if (s.compare("pi") == 0) {
                mpfr_t pi;

                mpfr_init2(pi, getBasePrecision());
                mpfr_const_pi(pi, MPFR_RNDA);

                setValue(pi);
            }
            else if (s.compare("e") == 0) {
                mpfr_t e;

                mpfr_init2(e, getBasePrecision());
                mpfr_const_euler(e, MPFR_RNDA);

                setValue(e);
            }
            else if (s.compare("c") == 0) {
                mpfr_t c;

                mpfr_init2(c, getBasePrecision());
                mpfr_set_ui(c, CONSTANT_C, MPFR_RNDA);

                setValue(c);
            }
            else if (s.compare("g") == 0) {
                mpfr_t g;

                mpfr_init2(g, getBasePrecision());
                mpfr_strtofr(g, CONSTANT_G, NULL, 10, MPFR_RNDA);

                setValue(g);
            }
        }
};

class constant_pi_t : public constant_t {
    public:
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
        operand_t * evaluate(void) {
            mpfr_t r;

            mpfr_init2(r, getBasePrecision());
            mpfr_set_str(r, CONSTANT_G, getBase(), MPFR_RNDA);

            operand_t * result = new operand_t(r);

            return result;
        }
};

#endif

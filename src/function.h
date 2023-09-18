#include <string>

#include <gmp.h>
#include <mpfr.h>

#include "system.h"
#include "memory.h"
#include "token.h"
#include "operator.h"
#include "operand.h"
#include "logger.h"

using namespace std;

#ifndef __INCL_FUNCTION
#define __INCL_FUNCTION

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

            system_t & sys = system_t::getInstance();

            if (sys.getTrigMode() == degrees) {
                mpfr_sinu(r, o1->getValue(), 360U, MPFR_RNDA);
            }
            else {
                mpfr_sin(r, o1->getValue(), MPFR_RNDA);
            }

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
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

            system_t & sys = system_t::getInstance();

            if (sys.getTrigMode() == degrees) {
                mpfr_cosu(r, o1->getValue(), 360U, MPFR_RNDA);
            }
            else {
                mpfr_cos(r, o1->getValue(), MPFR_RNDA);
            }

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
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

            system_t & sys = system_t::getInstance();

            if (sys.getTrigMode() == degrees) {
                mpfr_tanu(r, o1->getValue(), 360U, MPFR_RNDA);
            }
            else {
                mpfr_tan(r, o1->getValue(), MPFR_RNDA);
            }

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
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

            system_t & sys = system_t::getInstance();

            if (sys.getTrigMode() == degrees) {
                mpfr_asinu(r, o1->getValue(), 360U, MPFR_RNDA);
            }
            else {
                mpfr_asin(r, o1->getValue(), MPFR_RNDA);
            }

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
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

            system_t & sys = system_t::getInstance();

            if (sys.getTrigMode() == degrees) {
                mpfr_acosu(r, o1->getValue(), 360U, MPFR_RNDA);
            }
            else {
                mpfr_acos(r, o1->getValue(), MPFR_RNDA);
            }

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
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

            system_t & sys = system_t::getInstance();

            if (sys.getTrigMode() == degrees) {
                mpfr_atanu(r, o1->getValue(), 360U, MPFR_RNDA);
            }
            else {
                mpfr_atan(r, o1->getValue(), MPFR_RNDA);
            }

            operand_t * result = new operand_t(r);

            mpfr_clear(r);
            
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

            mpfr_clear(r);
            
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

            mpfr_clear(r);
            
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

            mpfr_clear(r);
            
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

            mpfr_clear(r);
            
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

            mpfr_clear(r);
            
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

            mpfr_clear(r);
            
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

            mpfr_clear(r);
            
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

            mpfr_clear(r);
            
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

            mpfr_clear(r);
            
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

            mpfr_clear(r);
            
            return result;
        }
};

class function_memory_t : public function_t {
    public:
        function_memory_t(string & s) : function_t(s) {}
        function_memory_t(const char * s) : function_t(s) {}

        operand_t * evaluate(operand_t * o1) {
            memory_t & mem = memory_t::getInstance();

            operand_t * result = mem.retrieve((int)mpfr_get_si(o1->getValue(), MPFR_RNDA));

            return result;
        }
};

#endif

#include <string>

#include <gmp.h>
#include <mpfr.h>

#include "memory.h"
#include "operator.h"
#include "logger.h"
#include "system.h"

using namespace std;

#ifndef __INCL_FUNCTION
#define __INCL_FUNCTION

class Function {
    private:
        static void _radians(mpfr_t radians, mpfr_t degrees) {
            mpfr_t  pi;
            mpfr_t  one_eighty;

            mpfr_init2(pi, getBasePrecision());
            mpfr_init2(one_eighty, getBasePrecision());

            mpfr_const_pi(pi, MPFR_RNDA);
            mpfr_set_ui(one_eighty, 180U, MPFR_RNDA);

            mpfr_div(radians, pi, one_eighty, MPFR_RNDA);

            mpfr_mul(radians, radians, degrees, MPFR_RNDA);
        }

        static void _degrees(mpfr_t degrees, mpfr_t radians) {
            mpfr_t  pi;
            mpfr_t  one_eighty;

            mpfr_init2(pi, getBasePrecision());
            mpfr_init2(one_eighty, getBasePrecision());

            mpfr_const_pi(pi, MPFR_RNDA);
            mpfr_set_ui(one_eighty, 180U, MPFR_RNDA);
            
            mpfr_div(degrees, one_eighty, pi, MPFR_RNDA);

            mpfr_mul(degrees, degrees, radians, MPFR_RNDA);
        }

    public:
        static string & evaluate(string & f, int radix, string & operand1) {
            mpfr_t          r;
            mpfr_t          o1;
            char            szOutputString[OUTPUT_MAX_STRING_LENGTH];
            char            szFormatString[FORMAT_STRING_LENGTH];
            static string   result;

            lgLogDebug("Evaluating: %s(%s)", f.c_str(), operand1.c_str());
            
            mpfr_init2(o1, getBasePrecision());
            mpfr_strtofr(o1, operand1.c_str(), NULL, radix, MPFR_RNDA);

            mpfr_init2(r, getBasePrecision());

            if (f.compare("sin") == 0) {
                mpfr_sinu(r, o1, 360U, MPFR_RNDA);
            }
            else if (f.compare("cos") == 0) {
                mpfr_cosu(r, o1, 360U, MPFR_RNDA);
            }
            else if (f.compare("tan") == 0) {
                mpfr_tanu(r, o1, 360U, MPFR_RNDA);
            }
            else if (f.compare("asin") == 0) {
                mpfr_asinu(r, o1, 360U, MPFR_RNDA);
            }
            else if (f.compare("acos") == 0) {
                mpfr_acosu(r, o1, 360U, MPFR_RNDA);
            }
            else if (f.compare("atan") == 0) {
                mpfr_atanu(r, o1, 360U, MPFR_RNDA);
            }
            else if (f.compare("sinh") == 0) {
                mpfr_sinh(r, o1, MPFR_RNDA);
            }
            else if (f.compare("cosh") == 0) {
                mpfr_cosh(r, o1, MPFR_RNDA);
            }
            else if (f.compare("tanh") == 0) {
                mpfr_tanh(r, o1, MPFR_RNDA);
            }
            else if (f.compare("asinh") == 0) {
                mpfr_asinh(r, o1, MPFR_RNDA);
            }
            else if (f.compare("acosh") == 0) {
                mpfr_acosh(r, o1, MPFR_RNDA);
            }
            else if (f.compare("atanh") == 0) {
                mpfr_atanh(r, o1, MPFR_RNDA);
            }
            else if (f.compare("sqrt") == 0) {
                mpfr_sqrt(r, o1, MPFR_RNDA);
            }
            else if (f.compare("log") == 0) {
                mpfr_log10(r, o1, MPFR_RNDA);
            }
            else if (f.compare("ln") == 0) {
                mpfr_log(r, o1, MPFR_RNDA);
            }
            else if (f.compare("fac") == 0) {
                mpfr_fac_ui(r, mpfr_get_ui(o1, MPFR_RNDA), MPFR_RNDA);
            }
            else if (f.compare("rad") == 0) {
                _radians(r, o1);
            }
            else if (f.compare("deg") == 0) {
                _degrees(r, o1);
            }
            else if (f.compare("mem") == 0) {
                mpfr_cosu(r, o1, 360U, MPFR_RNDA);
            }

            snprintf(szFormatString, FORMAT_STRING_LENGTH, "%%.%ldRf", (long)getPrecision());
            mpfr_snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, szFormatString, r);

            mpfr_clear(r);
            mpfr_clear(o1);

            result.assign(szOutputString);

            return result;
        }

        static int getPrescedence() {
            return 5;
        }

        static associativity getAssociativity() {
            return LEFT;
        }
};

#endif

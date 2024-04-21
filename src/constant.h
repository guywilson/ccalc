#include <string>

#include <gmp.h>
#include <mpfr.h>

#include "logger.h"
#include "system.h"

using namespace std;

#ifndef __INCL_CONSTANT
#define __INCL_CONSTANT

#define CONSTANT_C                          299792458U
#define CONSTANT_G                          "0.000000000066743"

class Constant {
    public:
        static string & evaluate(string & token) {
            mpfr_t          r;
            char            szOutputString[OUTPUT_MAX_STRING_LENGTH];
            char            szFormatString[FORMAT_STRING_LENGTH];
            static string   result;

            mpfr_init2(r, getBasePrecision());

            if (token.compare("pi") == 0) {
                mpfr_const_pi(r, MPFR_RNDA);
            }
            else if (token.compare("eu") == 0) {
                mpfr_const_euler(r, MPFR_RNDA);
            }
            else if (token.compare("g") == 0) {
                mpfr_set_str(r ,CONSTANT_G, 10, MPFR_RNDA);
            }
            else if (token.compare("c") == 0) {
                mpfr_set_ui(r, CONSTANT_C, MPFR_RNDA);
            }

            snprintf(szFormatString, FORMAT_STRING_LENGTH, "%%.%ldRf", (long)getPrecision());
            mpfr_snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, szFormatString, r);

            result.assign(szOutputString);

            mpfr_clear(r);

            return result;
        }
};

#endif

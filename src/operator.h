#include <string>

#include <gmp.h>
#include <mpfr.h>

#include "logger.h"
#include "utils.h"
#include "system.h"

using namespace std;

#ifndef __INCL_OPERATOR
#define __INCL_OPERATOR

typedef enum {
    LEFT,
    RIGHT
}
associativity;

class Operator {
    public:
        static string evaluate(string & op, int radix, string & operand1, string & operand2) {
            mpfr_t          r;
            mpfr_t          o1;
            mpfr_t          o2;
            string          result;

            lgLogDebug("Evaluating: %s %s %s", operand1.c_str(), op.c_str(), operand2.c_str());

            mpfr_init2(o1, getBasePrecision());
            mpfr_strtofr(o1, operand1.c_str(), NULL, radix, MPFR_RNDA);

            mpfr_init2(o2, getBasePrecision());
            mpfr_strtofr(o2, operand2.c_str(), NULL, radix, MPFR_RNDA);

            mpfr_init2(r, getBasePrecision());

            switch (op[0]) {
                case '+':
                    mpfr_add(r, o1, o2, MPFR_RNDA);
                    break;

                case '-':
                    mpfr_sub(r, o1, o2, MPFR_RNDA);
                    break;

                case '*':
                    mpfr_mul(r, o1, o2, MPFR_RNDA);
                    break;

                case '/':
                    mpfr_div(r, o1, o2, MPFR_RNDA);
                    break;

                case '%':
                    mpfr_remainder(r, o1, o2, MPFR_RNDA);
                    break;

                case '^':
                    mpfr_pow(r, o1, o2, MPFR_RNDA);
                    break;

                case ':':
                    mpfr_rootn_ui(r, o1, mpfr_get_ui(o2, MPFR_RNDA), MPFR_RNDA);
                    break;

                case '&':
                    mpfr_set_ui(
                            r, 
                            (mpfr_get_ui(o1, MPFR_RNDA) & 
                            mpfr_get_ui(o2, MPFR_RNDA)), 
                            MPFR_RNDA);
                    break;

                case '|':
                    mpfr_set_ui(
                            r, 
                            (mpfr_get_ui(o1, MPFR_RNDA) | 
                            mpfr_get_ui(o2, MPFR_RNDA)), 
                            MPFR_RNDA);
                    break;

                case '~':
                    mpfr_set_ui(
                            r, 
                            (mpfr_get_ui(o1, MPFR_RNDA) ^ 
                            mpfr_get_ui(o2, MPFR_RNDA)), 
                            MPFR_RNDA);
                    break;

                case '<':
                    mpfr_set_ui(
                            r, 
                            (mpfr_get_ui(o1, MPFR_RNDA) << 
                            mpfr_get_ui(o2, MPFR_RNDA)), 
                            MPFR_RNDA);
                    break;

                case '>':
                    mpfr_set_ui(
                            r, 
                            (mpfr_get_ui(o1, MPFR_RNDA) >> 
                            mpfr_get_ui(o2, MPFR_RNDA)), 
                            MPFR_RNDA);
                    break;
            }

            result.assign(toString(r, radix, getBasePrecision()));

            lgLogDebug("Result = %s", result.c_str());

            mpfr_clear(r);
            mpfr_clear(o2);
            mpfr_clear(o1);

            return result;
        }

        static int getPrescedence(string & op) {
            int p = 0;

            switch (op[0]) {
                case '+':
                    p = 2;
                    break;

                case '-':
                    p = 2;
                    break;

                case '*':
                    p = 3;
                    break;

                case '/':
                    p = 3;
                    break;

                case '%':
                    p = 3;
                    break;

                case '^':
                    p = 4;
                    break;

                case ':':
                    p = 4;
                    break;

                case '&':
                    p = 4;
                    break;

                case '|':
                    p = 4;
                    break;

                case '~':
                    p = 4;
                    break;

                case '<':
                    p = 4;
                    break;

                case '>':
                    p = 4;
                    break;
            }

            return p;
        }

        static associativity getAssociativity(string & op) {
            associativity a = LEFT;

            switch (op[0]) {
                case '+':
                    a = LEFT;
                    break;

                case '-':
                    a = LEFT;
                    break;

                case '*':
                    a = LEFT;
                    break;

                case '/':
                    a = LEFT;
                    break;

                case '%':
                    a = LEFT;
                    break;

                case '^':
                    a = RIGHT;
                    break;

                case ':':
                    a = LEFT;
                    break;

                case '&':
                    a = LEFT;
                    break;

                case '|':
                    a = LEFT;
                    break;

                case '~':
                    a = LEFT;
                    break;

                case '<':
                    a = RIGHT;
                    break;

                case '>':
                    a = RIGHT;
                    break;
            }

            return a;
        }
};

#endif

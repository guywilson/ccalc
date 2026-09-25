#pragma once

#include <string>

#include <gmp.h>
#include <mpfr.h>

#include "operand.h"

#define CONSTANT_C                          299792458U

static inline bool isTokenPi(const std::string & token) {
    return (token.compare("pi") == 0);
}

static inline bool isTokenEu(const std::string & token) {
    return (token.compare("eu") == 0);
}

static inline bool isTokenC(const std::string & token) {
    return (token.compare("c") == 0);
}

class Constant : public Operand {
    public:
        Constant(const std::string & token) : Operand() {
            if (isTokenPi(token)) {
                mpfr_const_pi(value, MPFR_RNDA);
            }
            else if (isTokenEu(token)) {
                mpfr_const_euler(value, MPFR_RNDA);
            }
            else if (isTokenC(token)) {
                mpfr_set_ui(value, CONSTANT_C, MPFR_RNDA);
            }

            setToken(toString(INTERMEDIATE_PRECISION));
        }

        static bool isConstant(const std::string & token) {
            return (isTokenPi(token) || isTokenEu(token) || isTokenC(token));
        }
};

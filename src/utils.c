#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <gmp.h>
#include <mpfr.h>

#include "utils.h"

static mpfr_prec_t  _precision;
static int          _base;

void setBase(int b) {
    _base = b;
}

int getBase(void) {
    return _base;
}

void setPrecision(mpfr_prec_t p) {
    _precision = p;
}

mpfr_prec_t getPrecision(void) {
    return _precision;
}

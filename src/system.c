#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include <gmp.h>
#include <mpfr.h>

#include "system.h"

#define NUM_MEMORY_LOCATIONS         10

static mpfr_prec_t  precision;
static mpfr_t       memory[NUM_MEMORY_LOCATIONS];

void setPrecision(mpfr_prec_t p) {
    precision = p;
}

mpfr_prec_t getPrecision(void) {
    return precision;
}

void memInit(void) {
    for (int i = 0;i < NUM_MEMORY_LOCATIONS;i++) {
        mpfr_init2(memory[i], getBasePrecision());
    }
}

void memRetrieve(mpfr_t m, int location) {
    mpfr_set(m, memory[location], MPFR_RNDA);
}

void memStore(mpfr_t m, int location) {
    mpfr_set(memory[location], m, MPFR_RNDA);
}

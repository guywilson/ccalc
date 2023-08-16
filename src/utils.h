#include <stdint.h>
#include <stdbool.h>

#include <mpfr.h>

#include "tokenizer.h"

#ifndef __INCL_UTILS
#define __INCL_UTILS

void        setBase(int b);
int         getBase(void);
void        setPrecision(mpfr_prec_t p);
mpfr_prec_t getPrecision(void);

#endif

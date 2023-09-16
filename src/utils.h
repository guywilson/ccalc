#include <stdint.h>
#include <stdbool.h>

#include <mpfr.h>

//#include "tokenizer.h"

#ifndef __INCL_UTILS
#define __INCL_UTILS

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    radians,
    degrees
}
trig_mode_t;

void        setBase(int b);
int         getBase(void);
void        setPrecision(mpfr_prec_t p);
mpfr_prec_t getPrecision(void);
void        setTrigMode(trig_mode_t m);
trig_mode_t getTrigMode(void);

#ifdef __cplusplus
}
#endif

#endif

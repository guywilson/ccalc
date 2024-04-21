#include <gmp.h>
#include <mpfr.h>

#ifndef __INCL_SYSTEM
#define __INCL_SYSTEM

#define getBasePrecision()                      1024L

#define DEFAULT_PRECISION                        2
#define MAX_PRECISION                           80

#ifdef __cplusplus
extern "C" {
#endif

void        setPrecision(mpfr_prec_t p);
mpfr_prec_t getPrecision(void);
void        memInit(void);
void        memRetrieve(mpfr_t m, int location);
void        memStore(mpfr_t m, int location);

#ifdef __cplusplus
}
#endif

#endif

#include <string>
#include <gmp.h>
#include <mpfr.h>

using namespace std;

#ifndef __INCL_SYSTEM
#define __INCL_SYSTEM

#define getBasePrecision()                      1024L

#define DEFAULT_PRECISION                        2
#define MAX_PRECISION                           80

#define FORMAT_STRING_LENGTH             32
#define OUTPUT_MAX_STRING_LENGTH        256

#define NUM_MEMORY_LOCATIONS             10

#define MAX_BASE                         62

#define BASE_10                          10
#define BASE_16                          16
#define BASE_8                            8
#define BASE_2                            2

#define DECIMAL                         BASE_10
#define HEXADECIMAL                     BASE_16
#define OCTAL                           BASE_8
#define BINARY                          BASE_2
#define STATISTIC                       1

void        setPrecision(mpfr_prec_t p);
mpfr_prec_t getPrecision(void);
void        memInit(void);
string      memRetrieve(int location);
void        memStore(string r, int location);
void        memClear(int location);
string      toString(mpfr_t value, int radix);
string      toFormattedString(mpfr_t value, int radix);

#endif

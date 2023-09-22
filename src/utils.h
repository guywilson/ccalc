#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef __INCL_UTILS
#define __INCL_UTILS

#ifdef __cplusplus
extern "C" {
#endif

char *          getBase2String(uint32_t value);
void            hexDump(void * buffer, uint32_t bufferLen);

#ifdef __cplusplus
}
#endif

#endif

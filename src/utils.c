#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "utils.h"

#define BASE2_OUTPUT_LEN                        ((sizeof(uint32_t) * 8) + 1)

const char * getBase2String(uint32_t value) {
    char        szOutputString[BASE2_OUTPUT_LEN];
    int         i = BASE2_OUTPUT_LEN - 1;
    int         bit = 0;

    while (value > 0) {
        bit = value % 2;

        value = value >> 1;

        szOutputString[i--] = (char)bit;
    }

    return strndup(szOutputString, BASE2_OUTPUT_LEN);
}

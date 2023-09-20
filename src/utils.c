#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "utils.h"

#define BASE2_OUTPUT_LEN                        (sizeof(uint32_t) * 8)

const char * getBase2String(uint32_t value) {
    char        szOutputString[BASE2_OUTPUT_LEN + 1];
    int         i = BASE2_OUTPUT_LEN - 1;
    int         bit = 0;

    szOutputString[BASE2_OUTPUT_LEN] = 0;

    while (value > 0) {
        bit = value % 2;

        value = value >> 1;

        szOutputString[i--] = ((char)bit + '0');
    }

    strncpy(szOutputString, &szOutputString[i + 1], BASE2_OUTPUT_LEN);

    return strndup(szOutputString, BASE2_OUTPUT_LEN);
}

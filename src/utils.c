#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"

#define BASE2_OUTPUT_LEN                        (sizeof(uint32_t) * 8)

char * getBase2String(uint32_t value) {
    char        szBinaryString[BASE2_OUTPUT_LEN + 1];
    char        szOutputString[BASE2_OUTPUT_LEN + 1];
    int         i = BASE2_OUTPUT_LEN - 1;
    int         bit = 0;

    szBinaryString[BASE2_OUTPUT_LEN] = 0;

    while (value > 0) {
        bit = value % 2;

        value = value >> 1;

        szBinaryString[i--] = ((char)bit + '0');
    }

    strncpy(szOutputString, &szBinaryString[i + 1], BASE2_OUTPUT_LEN);

    return strndup(szOutputString, BASE2_OUTPUT_LEN);
}

void hexDump(void * buffer, uint32_t bufferLen)
{
    int                     i;
    int                     j = 0;
    uint8_t *               buf;
    static char             szASCIIBuf[17];
    static uint32_t         offset = 0;

    buf = (uint8_t *)buffer;

    for (i = 0;i < bufferLen;i++) {
        if ((i % 16) == 0) {
            if (i != 0) {
                szASCIIBuf[j] = 0;
                j = 0;

                printf("  |%s|\n", szASCIIBuf);
            }
                
            printf("%08X\t", offset);
            offset += 16;
        }

        if ((i % 2) == 0 && (i % 16) > 0) {
            printf(" ");
        }

        printf("%02X", buf[i]);
        szASCIIBuf[j++] = isprint(buf[i]) ? buf[i] : '.';
    }

    /*
    ** Print final ASCII block...
    */
    szASCIIBuf[j] = 0;
    printf("  |%s|\n", szASCIIBuf);
}

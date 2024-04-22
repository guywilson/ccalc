#include <string>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include <gmp.h>
#include <mpfr.h>

#include "logger.h"
#include "utils.h"
#include "system.h"

using namespace std;

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

string toString(mpfr_t value, int outputBase) {
    char            szOutputString[OUTPUT_MAX_STRING_LENGTH];
    char            szFormatString[FORMAT_STRING_LENGTH];
    string          outputStr;

    switch (outputBase) {
        case DECIMAL:
            snprintf(szFormatString, FORMAT_STRING_LENGTH, "%%.%ldRf", (long)getPrecision());
            mpfr_snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, szFormatString, value);
            break;

        case HEXADECIMAL:
            snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, "%08lX", mpfr_get_ui(value, MPFR_RNDA));
            break;

        case OCTAL:
            snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, "%lo", mpfr_get_ui(value, MPFR_RNDA));
            break;

        case BINARY:
            char * binaryStr = Utils::getBase2String(mpfr_get_ui(value, MPFR_RNDA));
            snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, "%s", binaryStr);
            free(binaryStr);
            break;
    }

    outputStr.assign(szOutputString, OUTPUT_MAX_STRING_LENGTH);

    lgLogDebug(
        "Output string = '%s', radix = %d, precision = %ld", 
        outputStr.c_str(), 
        outputBase, 
        getPrecision());

    return outputStr;
}

string toFormattedString(mpfr_t value, int outputBase) {
    const char *    pszIn;
    char *          pszOut;
    int             delim_char;
    int             delim_char_count = 0;
    int             startIndex = 0;
    int             tgtIndex = 0;
    int             srcIndex = 0;
    int             digitCount = 0;
    size_t          oldLength;
    size_t          newLength;
    bool            hasDelimiters = false;
    static string   outputStr;
    string          formattedString;

    pszIn = toString(value, outputBase).c_str();
    oldLength = strlen(pszIn);
    newLength = oldLength;

    switch (outputBase) {
        case DECIMAL:
            delim_char = ',';
            delim_char_count = 3;
            startIndex = outputStr.find_first_of('.') - 1;
            break;

        case BINARY:
        case HEXADECIMAL:
        case OCTAL:
            delim_char = ' ';
            delim_char_count = 4;
            startIndex = oldLength - 1;
            break;

        default:
            delim_char = ' ';
            delim_char_count = 4;
            startIndex = oldLength - 1;
    }

    if ((startIndex + 1) > delim_char_count) {
        hasDelimiters = true;
        newLength += (startIndex + 1) / delim_char_count;

        if ((startIndex + 1) % delim_char_count == 0) {
            newLength--;
        }
    }

    lgLogDebug("toFormattedString(): oldLen:%u, newLen:%u, start:%d", oldLength, newLength, startIndex);

    pszOut = (char *)malloc(newLength + 1);

    /*
    ** Make sure the string is null terminated...
    */
    pszOut[newLength] = 0;

    if (hasDelimiters) {
        tgtIndex = startIndex + (newLength - oldLength);
        srcIndex = startIndex;

        strncpy(&pszOut[newLength - oldLength], pszIn, oldLength);

        while (srcIndex >= 0) {
            lgLogDebug("srcIndex:%d, tgtIndex:%d, digitCount:%d", srcIndex, tgtIndex, digitCount);
            
            if (digitCount == delim_char_count) {
                pszOut[tgtIndex--] = delim_char;
                digitCount = 0;
            }
            
            pszOut[tgtIndex--] = pszIn[srcIndex--];
            digitCount++;
        }
    }
    else {
        strncpy(pszOut, pszIn, oldLength);
    }

    lgLogDebug("Built formatted string '%s'", pszOut);

    if (lgCheckLogLevel(LOG_LEVEL_DEBUG)) {
        Utils::hexDump(pszOut, newLength + 1);
    }

    formattedString.assign(pszOut);

    return formattedString;
}

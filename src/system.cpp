#include <string>
#include <sstream>
#include <iomanip>
#include <locale>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include <gmp.h>
#include <mpfr.h>

#include "calc_error.h"
#include "logger.h"
#include "utils.h"
#include "system.h"

using namespace std;

static mpfr_prec_t  precision;
static string       memory[NUM_MEMORY_LOCATIONS];

void setPrecision(mpfr_prec_t p) {
    precision = p;
}

mpfr_prec_t getPrecision(void) {
    return precision;
}

void memInit(void) {
    for (int m = 0;m < NUM_MEMORY_LOCATIONS;m++) {
        memory[m].assign("0.00");
    }
}

string memRetrieve(int location) {
    if (location < 0 || location > NUM_MEMORY_LOCATIONS - 1) {
        throw calc_error("Memory location out of range. Must be between 0 and 9");
    }

    return memory[location];
}

void memStore(string r, int location) {
    if (location < 0 || location > NUM_MEMORY_LOCATIONS - 1) {
        throw calc_error("Memory location out of range. Must be between 0 and 9");
    }
    
    memory[location].assign(r);
}

void memClear(int location) {
    if (location < 0 || location > NUM_MEMORY_LOCATIONS - 1) {
        throw calc_error("Memory location out of range. Must be between 0 and 9");
    }
    
    memory[location].assign("0.00");
}

string toString(mpfr_t value, int radix) {
    char            szOutputString[OUTPUT_MAX_STRING_LENGTH];
    char            szFormatString[FORMAT_STRING_LENGTH];
    string          outputStr;

    switch (radix) {
        case DECIMAL:
            snprintf(szFormatString, FORMAT_STRING_LENGTH, "%%.%ldRf", (long)getPrecision());
            mpfr_snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, szFormatString, value);
            break;

        case HEXADECIMAL:
            snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, "%016lX", mpfr_get_ui(value, MPFR_RNDA));
            break;

        case OCTAL:
            snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, "%lo", mpfr_get_ui(value, MPFR_RNDA));
            break;

        case BINARY:
            char * binaryStr = Utils::getBase2String(mpfr_get_ui(value, MPFR_RNDA));
            snprintf(szOutputString, strlen(binaryStr), "%s", binaryStr);
            free(binaryStr);
            break;
    }

    outputStr.assign(szOutputString);

    lgLogDebug(
        "Output string = '%s', radix = %d, precision = %ld", 
        outputStr.c_str(), 
        radix, 
        getPrecision());

    return outputStr;
}

string toFormattedString(mpfr_t value, int radix) {
    int     i;
    int     j;
    int     k;
    int     numDigits = 0;
    char    seperator = ' ';
    string  s = toString(value, radix);
    string  out(s.length() * 3, '0');

    i = s.length() - 1;
    j = out.length() - 1;

    lgLogDebug("toFormattedString: inputLen: %d, outputLength: %d", i, j);

    switch (radix) {
        case DECIMAL:
            numDigits = 3;
            seperator = ',';

            if (s.find_last_of('.') != string::npos) {
                while (s[i] != '.') {
                    out[j--] = s[i--];
                }
                out[j--] = s[i--];          // Copy the decimal point...
            }
            break;

        case HEXADECIMAL:
        case OCTAL:
        case BINARY:
            numDigits = 4;
            seperator = ' ';
            break;
    }

    k = 0;

    while (i >= 0) {
        out[j--] = s[i--];
        k++;

        lgLogDebug("toFormattedString: i:%d, j:%d, k:%d", i, j, k);

        /*
        ** Add the seperator char...
        */
        if (k == numDigits && i >= 0) {
            out[j--] = seperator;
            k = 0;
        }
    }

    /*
    ** Trim the output string...
    */
    out = out.substr(j + 1, out.length());

    return out;
}

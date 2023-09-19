/******************************************************************************
Command line calculator
Copyright (C) 2023  Guy Wilson

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <gmp.h>
#include <mpfr.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "logger.h"
#include "operand.h"
#include "calc_error.h"
#include "tokenizer.h"
#include "calculator.h"
#include "timeutils.h"
#include "system.h"
#include "memory.h"
#include "test.h"
#include "version.h"

#define DEFAULT_LOG_LEVEL                       (LOG_LEVEL_FATAL | LOG_LEVEL_ERROR)

#define DEFAULT_PRECISION                       2

const char * pszWarranty = 
    "This program comes with ABSOLUTELY NO WARRANTY.\n" \
    "This is free software, and you are welcome to redistribute it\n" \
    "under certain conditions.\n\n";

static void printBanner(void) {
    int         year;

    tmUpdate();
    year = tmGetYear();

	printf("\n*** Welcome to CCALC v%s ***\n\n", getVersion());
    printf("A cmd line scientific calculator. Copyright © Guy Wilson %d\n", year);
	printf("Type a calculation or command at the prompt, type 'help' for info.\n\n");
}

static void printVersion(void) {
    printf("CCALC version '%s' - built [%s]\n\n", getVersion(), getBuildDate());
}

static void printUsage(void) {
    printBanner();
    printf("%s", pszWarranty);

    printf("Operators supported:\n");
    printf("\t+, -, *, /, %% (Modulo)\n");
    printf("\t& (AND), | (OR), ~ (XOR)\n");
    printf("\t<< (left shift), >> (right shift)\n");
    printf("\t^ (power, e.g. x to the power of y)\n");
    printf("\t: (root, e.g. x : y - the yth root of x)\n\n");
    printf("\tNesting is achieved with braces ()\n\n");
    printf("Functions supported:\n");
    printf("\tsin(x)\treturn the sine of the angle x degrees\n");
    printf("\tcos(x)\treturn the cosine of the angle x degrees\n");
    printf("\ttan(x)\treturn the tangent of the angle x degrees\n");
    printf("\tasin(x)\treturn the angle in degrees of arcsine(x)\n");
    printf("\tacos(x)\treturn the angle in degrees of arccosine(x)\n");
    printf("\tatan(x)\treturn the angle in degrees of arctangent(x)\n");
    printf("\tsinh(x)\treturn the hyperbolic sine of the angle x radians\n");
    printf("\tcosh(x)\treturn the hyperbolic cosine of the angle x radians\n");
    printf("\ttanh(x)\treturn the hyperbolic tangent of the angle x radians\n");
    printf("\tasinh(x) return the inverse hyperbolic sine of angle x in radians\n");
    printf("\tacosh(x) return the inverse hyperbolic cosine of angle x in radians\n");
    printf("\tatanh(x) return the inverse hyperbolic tangent of angle x in radians\n");
    printf("\tsqrt(x)\treturn the square root of x\n");
    printf("\tlog(x)\treturn the log of x\n");
    printf("\tln(x)\treturn the natural log of x\n");
    printf("\tfact(x)\treturn the factorial of x\n");
    printf("\tmem(n)\tthe value in memory location n, where n is 0 - 9\n\n");
    printf("Constants supported:\n");
    printf("\tpi\tthe ratio pi\n");
    printf("\teu\tEulers constant\n");
    printf("\tg\tThe gravitational constant G\n");
    printf("\tc\tthe speed of light in a vacuum\n\n");
    printf("Commands supported:\n");
    printf("\tmemstn\tStore the last result in memory location n (0 - 9)\n");
    printf("\tdec\tSwitch to decimal mode\n");
    printf("\thex\tSwitch to hexadecimal mode\n");
    printf("\tbin\tSwitch to binary mode\n");
    printf("\toct\tSwitch to octal mode\n");
    printf("\tdeg\tSwitch to degrees mode for trigometric functions\n");
    printf("\trad\tSwitch to radians mode for trigometric functions\n");
    printf("\tsetpn\tSet the precision to n\n");
    printf("\tfmton\tTurn on output formatting (on by default)\n");
    printf("\tfmtoff\tTurn off output formatting\n");
    printf("\thelp\tThis help text\n");
    printf("\ttest\tRun a self test of the calculator\n");
    printf("\tversion\tPrint the calculator version\n");
    printf("\texit\tExit the calculator\n\n");
}

static const char * getBaseString(void) {
    system_t & sys = system_t::getInstance();

    switch (sys.getBase()) {
        case DECIMAL:
            return "DEC";

        case HEXADECIMAL:
            return "HEX";

        case OCTAL:
            return "OCT";

        case BINARY:
            return "BIN";

        default:
            return "DEC";
    }
}

static const char * getTrigModeString(void) {
    system_t & sys = system_t::getInstance();

    if (sys.getBase() == DECIMAL) {
        switch (sys.getTrigMode()) {
            case radians:
                return "RN";

            case degrees:
                return "DG";
        }
    }
    else {
        return "";
    }

    return "";
}

static char * formatResult(operand_t * result) {
    size_t              allocateLen = 0;
    char *              pszFormattedResult;
    int                 i;
    int                 j;
    char                delimiter = 0;
    int                 delimPos = 0;
    int                 digitCount = 0;
    int                 delimRangeLimit = 0;

    allocateLen = result->getLength();

    system_t & sys = system_t::getInstance();

    switch (sys.getBase()) {
        case DECIMAL:
            if (allocateLen > 3) {
                i = result->getTokenStr().find_first_of('.');

                if (i < result->getLength()) {
                    allocateLen += (i - 1) / 3;
                    delimRangeLimit = i - 1;
                }
            }

            delimiter = ',';
            delimPos = 3;
            break;

        case BINARY:
            if (allocateLen > 8) {
                allocateLen += ((allocateLen / 8) - 1);
            }

            delimiter = ' ';
            delimPos = 8;
            break;

        case OCTAL:
            delimiter = 0;
            break;

        case HEXADECIMAL:
            if (allocateLen > 4) {
                allocateLen += ((allocateLen / 4));
            }

            delimiter = ' ';
            delimPos = 4;
            break;
    }

    pszFormattedResult = (char *)malloc(allocateLen + 1);
    memset(pszFormattedResult, delimiter, allocateLen);

    pszFormattedResult[allocateLen] = 0;

    if (allocateLen > result->getLength() && sys.getBase() != OCTAL) {
        if (sys.getBase() != DECIMAL) {
            delimRangeLimit = result->getLength() - 1;
        }
        
        i = allocateLen - 1;
        j = result->getLength() - 1;

        digitCount = delimPos;

        while (j >= 0) {
            pszFormattedResult[i] = result->getTokenStr()[j];

            if (j <= delimRangeLimit) {
                digitCount--;

                if (digitCount == 0) {
                    i--;
                    digitCount = delimPos;
                }
            }

            i--;
            j--;
        }
    }
    else {
        strncpy(pszFormattedResult, result->getTokenStr().c_str(), result->getLength());
    }

    return pszFormattedResult;
}

int main(int argc, char ** argv) {
    char *              pszCalculation;
    char *              pszFormattedResult;
    char                szPrompt[32];
    bool                loop = true;
    bool                doFormat = true;
    operand_t *         result = NULL;

    rl_bind_key('\t', rl_complete);

    using_history();

    system_t & sys = system_t::getInstance();
    memory_t & mem = memory_t::getInstance();

    sys.setPrecision(DEFAULT_PRECISION);
    sys.setBase(DECIMAL);
    sys.setTrigMode(degrees);

    lgOpenStdout("LOG_LEVEL_ALL");
    lgSetLogLevel(DEFAULT_LOG_LEVEL);

    printBanner();

    while (loop) {
        snprintf(szPrompt, 32, "calc [%s][%s]> ", getBaseString(), getTrigModeString());

        pszCalculation = readline(szPrompt);

        add_history(pszCalculation);

        if (strlen(pszCalculation) > 0) {
            if (strncmp(pszCalculation, "exit", 4) == 0 || strncmp(pszCalculation, "quit", 4) == 0 || pszCalculation[0] == 'q') {
                loop = false;
            }
            else if (strncmp(pszCalculation, "help", 4) == 0) {
                printUsage();
            }
            else if (strncmp(pszCalculation, "version", 7) == 0) {
                printVersion();
            }
            else if (strncmp(pszCalculation, "test", 4) == 0) {
                return test();
            }
            else if (strncmp(pszCalculation, "setp", 4) == 0) {
                sys.setPrecision(strtol(&pszCalculation[4], NULL, BASE_10));
            }
            else if (strncmp(pszCalculation, "dbgon", 5) == 0) {
                lgSetLogLevel(LOG_LEVEL_ALL);
            }
            else if (strncmp(pszCalculation, "dbgoff", 6) == 0) {
                lgSetLogLevel(DEFAULT_LOG_LEVEL);
            }
            else if (strncmp(pszCalculation, "staon", 5) == 0) {
                lgSetLogLevel(DEFAULT_LOG_LEVEL | LOG_LEVEL_STATUS);
            }
            else if (strncmp(pszCalculation, "staoff", 6) == 0) {
                lgSetLogLevel(DEFAULT_LOG_LEVEL);
            }
            else if (strncmp(pszCalculation, "fmton", 5) == 0) {
                doFormat = true;
            }
            else if (strncmp(pszCalculation, "fmtoff", 6) == 0) {
                doFormat = false;
            }
            else if (strncmp(pszCalculation, "memst", 5) == 0) {
                int m = atoi(&pszCalculation[5]);

                try {
                    mem.store(m, result);
                }
                catch (calc_error & e) {
                    fprintf(stderr, "Failed to store result in memory %d: %s\n", m, e.what());
                }
            }
            else if (strncmp(pszCalculation, "dec", 3) == 0) {
                sys.setBase(DECIMAL);
                printf("= %s\n", result->getTokenStr().c_str());
            }
            else if (strncmp(pszCalculation, "hex", 3) == 0) {
                sys.setBase(HEXADECIMAL);
                sys.setTrigMode(degrees);

                printf("= 0x%08X\n", (unsigned int)strtoul(result->getTokenStr().c_str(), NULL, DECIMAL));
            }
            else if (strncmp(pszCalculation, "bin", 3) == 0) {
                sys.setBase(BINARY);
                sys.setTrigMode(degrees);
            }
            else if (strncmp(pszCalculation, "oct", 3) == 0) {
                sys.setBase(OCTAL);
                sys.setTrigMode(degrees);
                printf("= 0o%o\n", (unsigned int)strtoul(result->getTokenStr().c_str(), NULL, DECIMAL));
            }
            else if (strncmp(pszCalculation, "deg", 3) == 0) {
                sys.setTrigMode(degrees);
            }
            else if (strncmp(pszCalculation, "rad", 3) == 0) {
                sys.setTrigMode(radians);
            }
            else {
                try {
                    if (result != NULL) {
                        delete result;
                    }

                    result = evaluate(pszCalculation);

                    if (doFormat) {
                        pszFormattedResult = formatResult(result);

                        printf("%s = %s\n", pszCalculation, pszFormattedResult);

                        free(pszFormattedResult);
                    }
                    else {
                        printf("%s = %s\n", pszCalculation, result->getTokenStr().c_str());
                    }
                }
                catch (calc_error & e) {
                    printf("Calculation failed for %s: %s\n", pszCalculation, e.what());
                }
            }
        }

        free(pszCalculation);
    }

    if (result != NULL) {
        delete result;
    }

    return 0;
}

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

#include "tokenizer.h"
#include "calculator.h"
#include "utils.h"
#include "test.h"

#define DEFAULT_PRECISION                       12

const char * pszWarranty = 
    "CCALC (Command-line CALCulator)\n\n" \
    "Copyright (C) 2023  Guy Wilson\n" \
    "This program comes with ABSOLUTELY NO WARRANTY.\n" \
    "This is free software, and you are welcome to redistribute it\n" \
    "under certain conditions.\n\n";

static void printBanner(void) {
	printf("\nWelcome to CCALC. A cmd line scientific calculator. Copyright © Guy Wilson\n");
	printf("Type a calculation or command at the prompt, type 'help' for info.\n\n");
}

static void printUsage(void) {
    printf("%s", pszWarranty);

    printf("Operators supported:\n");
    printf("\t+, -, *, /, %% (Modulo)\n");
    printf("\t& (AND), | (OR), ~ (XOR)\n");
    printf("\t^ (power, e.g. x to the power of y)\n\n");
    printf("\tNesting is achieved with braces ()\n\n");
    printf("Functions supported:\n");
    printf("\tsin(x)\treturn the sine of the angle x degrees\n");
    printf("\tcos(x)\treturn the cosine of the angle x degrees\n");
    printf("\ttan(x)\treturn the tangent of the angle x degrees\n");
    printf("\tasin(x)\treturn the angle in degrees of arcsine(x)\n");
    printf("\tacos(x)\treturn the angle in degrees of arccosine(x)\n");
    printf("\tatan(x)\treturn the angle in degrees of arctangent(x)\n");
    printf("\tsqrt(x)\treturn the square root of x\n");
    printf("\tlog(x)\treturn the log of x\n");
    printf("\tln(x)\treturn the natural log of x\n");
    printf("\tfact(x)\treturn the factorial of x\n");
    printf("\tmem(n)\tthe value in memory location n, where n is 0 - 9\n\n");
    printf("Constants supported:\n");
    printf("\tpi\tthe ratio pi\n");
    printf("\tc\tthe speed of light in a vacuum\n\n");
    printf("Commands supported:\n");
    printf("\tmemstn\tStore the last result in memory location n (0 - 9)\n");
    printf("\tdec\tSwitch to decimal mode\n");
    printf("\thex\tSwitch to hexadecimal mode\n");
    printf("\tbin\tSwitch to binary mode\n");
    printf("\toct\tSwitch to octal mode\n");
    printf("\tsetpn\tSet the precision to n\n");
    printf("\thelp\tThis help text\n");
    printf("\ttest\tRun a self test of the calculator\n");
    printf("\texit\tExit the calculator\n\n");
}

static const char * getBaseString(void) {
    switch (getBase()) {
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

int main(int argc, char ** argv) {
    char *              pszCalculation;
    char                szPrompt[16];
    bool                loop = true;
    token_t             result;

    rl_bind_key('\t', rl_complete);

    // Enable history
    using_history();

    setPrecision(DEFAULT_PRECISION);
    setBase(DECIMAL);

    result.type = token_operand;
    result.pszToken = "0";
    result.length = 1;

    printBanner();

    while (loop) {
        sprintf(szPrompt, "calc [%s]> ", getBaseString());

        // Display prompt and read input
        pszCalculation = readline(szPrompt);

        // Add input to readline history.
        add_history(pszCalculation);

        if (strlen(pszCalculation) > 0) {
            if (strncmp(pszCalculation, "exit", 4) == 0 || strncmp(pszCalculation, "quit", 4) == 0 || pszCalculation[0] == 'q') {
                loop = false;
            }
            else if (strncmp(pszCalculation, "help", 4) == 0) {
                printUsage();
            }
            else if (strncmp(pszCalculation, "test", 4) == 0) {
                return test();
            }
            else if (strncmp(pszCalculation, "setp", 4) == 0) {
                setPrecision(strtol(&pszCalculation[4], NULL, BASE_10));
            }
            else if (strncmp(pszCalculation, "memst", 5) == 0) {
                int m = atoi(&pszCalculation[5]);

                if (memoryStore(&result, m) < 0) {
                    printf("Invalid memory location '%d' must be between 0 & 9\n", m);
                }
            }
            else if (strncmp(pszCalculation, "dec", 3) == 0) {
                setBase(DECIMAL);
            }
            else if (strncmp(pszCalculation, "hex", 3) == 0) {
                setBase(HEXADECIMAL);
            }
            else if (strncmp(pszCalculation, "bin", 3) == 0) {
                setBase(BINARY);
            }
            else if (strncmp(pszCalculation, "oct", 3) == 0) {
                setBase(OCTAL);
            }
            else {
                evaluate(pszCalculation, &result);
                printf("%s = %s\n", pszCalculation, result.pszToken);
            }
        }

        // Free buffer that was allocated by readline
        free(pszCalculation);
    }
}

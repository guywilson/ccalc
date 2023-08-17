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

static void printUsage(void) {

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

    setPrecision(8);
    setBase(DECIMAL);

    result.type = token_operand;
    result.pszToken = "0";
    result.length = 1;
    
    while (loop) {
        sprintf(szPrompt, "calc [%s]> ", getBaseString());

        // Display prompt and read input
        pszCalculation = readline(szPrompt);

        // Add input to readline history.
        add_history(pszCalculation);

        // Check for EOF.
        if (strncmp(pszCalculation, "exit", 4) == 0 || strncmp(pszCalculation, "quit", 4) == 0 || pszCalculation[0] == 'q') {
            loop = false;
        }
        else if (strncmp(pszCalculation, "setp", 4) == 0) {
            setPrecision(strtol(&pszCalculation[4], NULL, BASE_10));
        }
        else if (strncmp(pszCalculation, "memst", 5) == 0) {
            memoryStore(&result, atoi(&pszCalculation[5]));
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

        // Free buffer that was allocated by readline
        free(pszCalculation);
    }
}

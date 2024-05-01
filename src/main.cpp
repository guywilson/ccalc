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
#include <vector>

#include <gmp.h>
#include <mpfr.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "logger.h"
#include "calc_error.h"
#include "tokenizer.h"
#include "calculator.h"
#include "timeutils.h"
#include "utils.h"
#include "system.h"
#include "test.h"
#include "version.h"

using namespace std;

static const char * getModeString(int mode) {
    switch (mode) {
        case DECIMAL:
            return "DEC";

        case HEXADECIMAL:
            return "HEX";

        case OCTAL:
            return "OCT";

        case BINARY:
            return "BIN";

        case STATISTIC:
            return "STAT";

        default:
            return "CUS";
    }
}

int main(int argc, char ** argv) {
    char *              pszCommand;
    char                szPrompt[32];
    bool                loop = true;
    int                 mode = DECIMAL;
    mpfr_t              result;
    string              answer;
    vector<string>      stats;

    rl_bind_key('\t', rl_complete);

    using_history();

    mpfr_init2(result, getBasePrecision());
    mpfr_set_d(result, 0.0, MPFR_RNDA);

    memInit();
    setPrecision(DEFAULT_PRECISION);

    lgOpenStdout("LOG_LEVEL_ALL");
    lgSetLogLevel(DEFAULT_LOG_LEVEL);

    printBanner();

    while (loop) {
        snprintf(szPrompt, 32, "calc [%s]> ", getModeString(mode));

        pszCommand = readline(szPrompt);

        add_history(pszCommand);

        loop = processCommand(pszCommand, result, &mode);

        free(pszCommand);
    }

    mpfr_clear(result);

    return 0;
}

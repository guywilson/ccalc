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

#define DEFAULT_LOG_LEVEL                       (LOG_LEVEL_FATAL | LOG_LEVEL_ERROR)

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
    printf("\t< (left shift), > (right shift)\n");
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
    printf("\trad(x)\tthe value in radians of x degrees\n");
    printf("\tdeg(x)\tthe value in degrees of x radians\n");
    printf("\tmem(n)\tthe value in memory location n, where n is 0 - 9\n\n");
    printf("Constants supported:\n");
    printf("\tpi\tthe ratio pi\n");
    printf("\teu\tEulers constant\n");
    printf("\tg\tThe gravitational constant G\n");
    printf("\tc\tthe speed of light in a vacuum\n\n");
    printf("Commands supported:\n");
    printf("\tmemstn\tStore the last result in memory location n (0 - 9)\n");
    printf("\tmemclrn\tClear the memory location n (0 - 9)\n");
    printf("\tclrall\tClear all memory locations\n");
    printf("\tlistall\tList all memory locations\n");
    printf("\tdec\tSwitch to decimal mode\n");
    printf("\thex\tSwitch to hexadecimal mode\n");
    printf("\tbin\tSwitch to binary mode\n");
    printf("\toct\tSwitch to octal mode\n");
    printf("\tstat\tSwitch to statistical mode\n");
    printf("\tsum\tStatistical sum function\n");
    printf("\tavg\tStatistical average function\n");
    printf("\tmin\tStatistical minimum function\n");
    printf("\tmax\tStatistical maximum function\n");
    printf("\tclrstat\tClear the statistic buffer of all values\n");
    printf("\tsetpn\tSet the precision to n\n");
    printf("\tfmton\tTurn on output formatting (on by default)\n");
    printf("\tfmtoff\tTurn off output formatting\n");
    printf("\thelp\tThis help text\n");
    printf("\ttest\tRun a self test of the calculator\n");
    printf("\tversion\tPrint the calculator version\n");
    printf("\texit\tExit the calculator\n\n");
}

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
    char *              pszCalculation;
    char                szPrompt[32];
    bool                loop = true;
    bool                doFormat = true;
    int                 mode = DECIMAL;
    long                precision;
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
                int numTestsFailed = test();

                if (numTestsFailed) {
                    fprintf(stderr, "Self-test failed with %d failures\n\n", numTestsFailed);
                }
            }
            else if (strncmp(pszCalculation, "setp", 4) == 0) {
                precision = strtol(&pszCalculation[4], NULL, BASE_10);

                if (precision < 0 || precision > MAX_PRECISION) {
                    fprintf(stderr, "Precision must be between 0 and %d\n", MAX_PRECISION);
                }
                else {
                    setPrecision(precision);
                }
            }
            else if (strncmp(pszCalculation, "setb", 4) == 0) {
                int originalBase = mode;

                mode = atoi(&pszCalculation[4]);

                if (mode < 2 || mode > MAX_BASE) {
                    fprintf(stderr, "Base must be between 1 and %d\n", MAX_BASE);
                    mode = originalBase;
                }
                else {
                    printf("= ");

                    mpfr_out_str(stdout, mode, MAX_PRECISION, result, MPFR_RNDA);
                    
                    printf("\n");
                    fflush(stdout);
                }
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

                memStore(toString(result, mode), m);
            }
            else if (strncmp(pszCalculation, "memclr", 6) == 0) {
                int m = atoi(&pszCalculation[6]);

                memClear(m);
            }
            else if (strncmp(pszCalculation, "clrall", 6) == 0) {
                for (int m = 0;m < NUM_MEMORY_LOCATIONS;m++) {
                    memClear(m);
                }
            }
            else if (strncmp(pszCalculation, "listall", 7) == 0) {
                for (int m = 0;m < NUM_MEMORY_LOCATIONS;m++) {
                    printf("\tmem %d -> %s\n", m, memRetrieve(m).c_str());
                }
            }
            else if (strncmp(pszCalculation, "dec", 3) == 0) {
                mode = DECIMAL;
                
                if (doFormat) {
                    answer.assign(toFormattedString(result, mode));
                }
                else {
                    answer.assign(toString(result, mode));
                }

                printf("= %s\n", answer.c_str());
            }
            else if (strncmp(pszCalculation, "hex", 3) == 0) {
                mode = HEXADECIMAL;

                if (doFormat) {
                    answer.assign(toFormattedString(result, mode));
                }
                else {
                    answer.assign(toString(result, mode));
                }

                printf("= %s\n", answer.c_str());
            }
            else if (strncmp(pszCalculation, "bin", 3) == 0) {
                mode = BINARY;

                if (doFormat) {
                    answer.assign(toFormattedString(result, mode));
                }
                else {
                    answer.assign(toString(result, mode));
                }

                printf("= %s\n", answer.c_str());
            }
            else if (strncmp(pszCalculation, "oct", 3) == 0) {
                mode = OCTAL;

                if (doFormat) {
                    answer.assign(toFormattedString(result, mode));
                }
                else {
                    answer.assign(toString(result, mode));
                }

                printf("= %s\n", answer.c_str());
            }
            else if (strncmp(pszCalculation, "stat", 4) == 0) {
                mode = STATISTIC;
            }
            else if (strncmp(pszCalculation, "sum", 3) == 0) {
                if (mode == STATISTIC) {
                    if (stats.size() > 0) {
                        string addition(stats[0] + " + ");

                        for (int s = 1;s < stats.size();s++) {
                            addition.append(stats[s]);

                            if (s < stats.size() - 1) {
                                addition.append(" + ");
                            }
                        }

                        evaluate(result, addition.c_str(), DECIMAL);

                        if (doFormat) {
                            answer.assign(toFormattedString(result, DECIMAL));
                        }
                        else {
                            answer.assign(toString(result, DECIMAL));
                        }

                        printf("SUM = %s\n", answer.c_str());
                    }
                    else {
                        fprintf(stderr, "No data stored for statistic function\n");
                    }
                }
                else {
                    fprintf(stderr, "Must be in STAT mode to use the SUM command\n");
                }
            }
            else if (strncmp(pszCalculation, "avg", 3) == 0) {
                if (mode == STATISTIC) {
                    if (stats.size() > 0) {
                        char    szCount[40];
                        string  average("(" + stats[0] + " + ");

                        for (int s = 1;s < stats.size();s++) {
                            average.append(stats[s]);

                            if (s < stats.size() - 1) {
                                average.append(" + ");
                            }
                        }

                        snprintf(szCount, 40, "%lu", stats.size());
                        average.append(") / ");
                        average.append(szCount);

                        evaluate(result, average.c_str(), DECIMAL);

                        if (doFormat) {
                            answer.assign(toFormattedString(result, DECIMAL));
                        }
                        else {
                            answer.assign(toString(result, DECIMAL));
                        }

                        printf("AVG = %s\n", answer.c_str());
                    }
                    else {
                        fprintf(stderr, "No data stored for statistic function\n");
                    }
                }
                else {
                    fprintf(stderr, "Must be in STAT mode to use the AVG command\n");
                }
            }
            else if (strncmp(pszCalculation, "min", 3) == 0) {
                if (mode == STATISTIC) {
                    if (stats.size() > 0) {
                        double  min = strtod(stats[0].c_str(), NULL);
                        int     minIndex = 0;
                        mpfr_t  minValue;

                        for (int s = 1;s < stats.size();s++) {
                            double v = strtod(stats[s].c_str(), NULL);

                            if (v < min) {
                                min = v;
                                minIndex = s;
                            }
                        }

                        mpfr_init2(minValue, getBasePrecision());
                        mpfr_set_str(minValue, stats[minIndex].c_str(), DECIMAL, MPFR_RNDA);

                        if (doFormat) {
                            answer.assign(toFormattedString(minValue, DECIMAL));
                        }
                        else {
                            answer.assign(toString(minValue, DECIMAL));
                        }

                        mpfr_clear(minValue);

                        printf("MIN = %s\n", answer.c_str());
                    }
                    else {
                        fprintf(stderr, "No data stored for statistic function\n");
                    }
                }
                else {
                    fprintf(stderr, "Must be in STAT mode to use the MIN command\n");
                }
            }
            else if (strncmp(pszCalculation, "max", 3) == 0) {
                if (mode == STATISTIC) {
                    if (stats.size() > 0) {
                        double  max = strtod(stats[0].c_str(), NULL);
                        int     maxIndex = 0;
                        mpfr_t  maxValue;

                        for (int s = 1;s < stats.size();s++) {
                            double v = strtod(stats[s].c_str(), NULL);

                            if (v > max) {
                                max = v;
                                maxIndex = s;
                            }
                        }

                        mpfr_init2(maxValue, getBasePrecision());
                        mpfr_set_str(maxValue, stats[maxIndex].c_str(), DECIMAL, MPFR_RNDA);

                        if (doFormat) {
                            answer.assign(toFormattedString(maxValue, DECIMAL));
                        }
                        else {
                            answer.assign(toString(maxValue, DECIMAL));
                        }

                        mpfr_clear(maxValue);

                        printf("MIN = %s\n", answer.c_str());
                    }
                    else {
                        fprintf(stderr, "No data stored for statistic function\n");
                    }
                }
                else {
                    fprintf(stderr, "Must be in STAT mode to use the MAX command\n");
                }
            }
            else if (strncmp(pszCalculation, "clrstat", 7) == 0) {
                for (int s = 0;s < stats.size();s++) {
                    stats[s].clear();
                    stats[s].resize(0);
                }

                stats.clear();
            }
            else {
                try {
                    if (mode == STATISTIC) {
                        if (Utils::isOperand(pszCalculation)) {
                            stats.push_back(string(pszCalculation));
                        }
                    }
                    else {
                        evaluate(result, pszCalculation, mode);

                        if (mode != BASE_16 && mode != BASE_10 && mode != BASE_8 && mode != BASE_2) {
                            printf("%s = ", pszCalculation);

                            mpfr_out_str(stdout, mode, MAX_PRECISION, result, MPFR_RNDA);

                            printf("\n");
                            fflush(stdout);
                        }
                        else {
                            if (doFormat) {
                                answer.assign(toFormattedString(result, mode));
                            }
                            else {
                                answer.assign(toString(result, mode));
                            }

                            printf("%s = %s\n", pszCalculation, answer.c_str());
                        }
                    }
                }
                catch (calc_error & e) {
                    printf("Calculation failed for %s: %s\n", pszCalculation, e.what());
                }
            }
        }

        free(pszCalculation);
    }

    mpfr_clear(result);

    return 0;
}

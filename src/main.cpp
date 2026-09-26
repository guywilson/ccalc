#include <iostream>
#include <sstream>
#include <iomanip>
#include <locale>
#include <string>
#include <stdint.h>
#include <vector>
#include <time.h>
#include <sys/time.h>

#include <gmp.h>
#include <mpfr.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "calc_error.h"
#include "system.h"
#include "tokenizer.h"
#include "expression.h"
#include "prompt.h"
#include "cmdarg.h"
#include "version.h"
#include "test.h"

#define DEFAULT_PRECISION                   2

void test();

const char * pszWarranty = 
    "This program comes with ABSOLUTELY NO WARRANTY.\n" \
    "This is free software, and you are welcome to redistribute it\n" \
    "under certain conditions.\n\n";

static void printVersion(void) {
    std::cout <<"CCALC version '" << getVersion() << "' - built [" << getBuildDate() << "]" << std::endl << std::endl;
}

static void printBanner(void) {
    struct timeval tv;

    gettimeofday(&tv, NULL);
    time_t t = tv.tv_sec;
    struct tm * localTime = localtime(&t);

    int year = localTime->tm_year + 1900;

	std::cout << std::endl << "*** Welcome to CCALC v" << getVersion() << " ***" << std::endl << std::endl;
    std::cout << "*************************************************************" << std::endl;
    std::cout << "*                                                           *" << std::endl;
    std::cout << "*                      FREE PALESTINE!                      *" << std::endl;
    std::cout << "*                                                           *" << std::endl;
    std::cout << "*************************************************************" << std::endl << std::endl;
    std::cout << "A cmd line scientific calculator. Copyright © Guy Wilson " << year << std::endl;
	std::cout << "Type a calculation or command, or type 'help' for info." << std::endl << std::endl;
}

static void printUsage(void) {
    printBanner();
    std::cout << pszWarranty;

    std::cout << "Operators supported:" << std::endl;
    std::cout << "\t+, -, *, /, %% (Modulo)" << std::endl;
    std::cout << "\t& (AND), | (OR), ~ (XOR)" << std::endl;
    std::cout << "\t< (left shift), > (right shift)" << std::endl;
    std::cout << "\t^ (power, e.g. x to the power of y)" << std::endl;
    std::cout << "\t: (root, e.g. x : y - the yth root of x)" << std::endl << std::endl;
    std::cout << "\tNesting is achieved with braces ()" << std::endl << std::endl;
    std::cout << "Functions supported:" << std::endl;
    std::cout << "\tsin(x)\treturn the sine of the angle x degrees" << std::endl;
    std::cout << "\tcos(x)\treturn the cosine of the angle x degrees" << std::endl;
    std::cout << "\ttan(x)\treturn the tangent of the angle x degrees" << std::endl;
    std::cout << "\tasin(x)\treturn the angle in degrees of arcsine(x)" << std::endl;
    std::cout << "\tacos(x)\treturn the angle in degrees of arccosine(x)" << std::endl;
    std::cout << "\tatan(x)\treturn the angle in degrees of arctangent(x)" << std::endl;
    std::cout << "\tsinh(x)\treturn the hyperbolic sine of the angle x radians" << std::endl;
    std::cout << "\tcosh(x)\treturn the hyperbolic cosine of the angle x radians" << std::endl;
    std::cout << "\ttanh(x)\treturn the hyperbolic tangent of the angle x radians" << std::endl;
    std::cout << "\tasinh(x) return the inverse hyperbolic sine of angle x in radians" << std::endl;
    std::cout << "\tacosh(x) return the inverse hyperbolic cosine of angle x in radians" << std::endl;
    std::cout << "\tatanh(x) return the inverse hyperbolic tangent of angle x in radians" << std::endl;
    std::cout << "\tsqrt(x)\treturn the square root of x" << std::endl;
    std::cout << "\tlog(x)\treturn the log of x" << std::endl;
    std::cout << "\tln(x)\treturn the natural log of x" << std::endl;
    std::cout << "\tfact(x)\treturn the factorial of x" << std::endl;
    std::cout << "\trad(x)\tthe value in radians of x degrees" << std::endl;
    std::cout << "\tdeg(x)\tthe value in degrees of x radians" << std::endl;
    std::cout << "Constants supported:" << std::endl;
    std::cout << "\tpi\tthe ratio pi" << std::endl;
    std::cout << "\teu\tEulers constant" << std::endl;
    std::cout << "\tc\tthe speed of light in a vacuum" << std::endl << std::endl;
    std::cout << "Commands supported:" << std::endl;
    std::cout << "\tdec\tSwitch to decimal mode" << std::endl;
    std::cout << "\thex\tSwitch to hexadecimal mode" << std::endl;
    std::cout << "\tbin\tSwitch to binary mode" << std::endl;
    std::cout << "\toct\tSwitch to octal mode" << std::endl;
    std::cout << "\tsetpn\tSet the precision to n" << std::endl;
    std::cout << "\tfmton\tSwitch on output formatting (on by default)" << std::endl;
    std::cout << "\tfmtoff\tSwitch off output formatting" << std::endl;
    std::cout << "\thelp\tThis help text" << std::endl;
    std::cout << "\ttest\tRun a self test of the calculator" << std::endl;
    std::cout << "\tversion\tPrint the calculator version" << std::endl;
    std::cout << "\texit\tExit the calculator" << std::endl << std::endl;
}

std::string getRadixCode(int radix) {
    std::string code;

    switch (radix) {
        case DECIMAL:
            code.assign("DEC");
            break;

        case HEXADECIMAL:
            code.assign("HEX");
            break;

        case OCTAL:
            code.assign("OCT");
            break;

        case BINARY:
            code.assign("BIN");
            break;
    }

    return code;
}

std::string getPromptString(int radix) {
    std::stringstream s;

    s << "calc [" << getRadixCode(radix) << "]> ";

    return s.str();
}

/*
** Inserts thousands separators into the integer part of a decimal string.
** - Keeps an optional leading + / -
** - Preserves fractional part and any scientific exponent (e.g. "e+42")
** - Works for arbitrarily long strings (>> 2^64)
*/
static std::string addThousandsSeparators(const std::string & input) {
    char separator = std::use_facet< std::numpunct<char> >(std::cout.getloc()).thousands_sep();
    char decimalPoint = std::use_facet< std::numpunct<char> >(std::cout.getloc()).decimal_point();

    /*
    ** 1) Split off sign.
    */
    std::string sign;
    std::size_t pos = 0;
    if (input[pos] == '+' || input[pos] == '-') {
        sign.assign(1, input[pos]);
        ++pos;
    }

    /*
    ** 2) Split mantissa and exponent (e/E …). We keep the exponent untouched.
    */
    std::size_t exp_pos = input.find_first_of("eE", pos);
    std::string mantissa =
            (exp_pos == std::string::npos) ?
                input.substr(pos) : 
                input.substr(pos, exp_pos - pos);

    std::string exponent =
            (exp_pos == std::string::npos) ?
                std::string() :
                input.substr(exp_pos); // includes the 'e' or 'E' and whatever follows

    /*
    ** 3) Split integer and fractional parts of the mantissa.
    */
    std::size_t dot_pos = mantissa.find(decimalPoint);
    std::string int_part =
            (dot_pos == std::string::npos) ?
                mantissa : 
                mantissa.substr(0, dot_pos);

    std::string frac_part =
            (dot_pos == std::string::npos) ? 
            std::string() :
            mantissa.substr(dot_pos); // keep decimal point + fraction as-is

    /*
    ** 4) Insert separators into the integer part, grouping from the right.
    */
    const std::size_t n = int_part.size();
    if (n <= 3) {
        // No grouping needed.
        return sign + int_part + frac_part + exponent;
    }

    std::string grouped;
    grouped.reserve(n + n / 3);

    std::size_t first_group = n % 3;
    if (first_group == 0) {
        first_group = 3;
    }

    grouped.append(int_part.data(), first_group);
    
    for (std::size_t i = first_group; i < n; i += 3) {
        grouped.push_back(separator);
        grouped.append(int_part, i, 3);
    }

    /*
    ** 5) Reassemble and return.
    */
    return sign + grouped + frac_part + exponent;
}

int main(int argc, char ** argv) {
    CmdArg args(argc, argv);

    while (args.hasMoreArgs()) {
        std::string arg = args.nextArg();

        if (arg == "--test") {
            test();
            return 0;
        }
        else if (arg == "--help") {
            printUsage();
        }
        else if (arg == "--version") {
            printVersion();
            return 0;
        }
        else {
            throw calc_error(calc_error::buildMsg("Sorry, I do not understand program argumant '%s'", arg.c_str()));
        }
    }

    long precision = DEFAULT_PRECISION;

    printBanner();

    System & system = System::getInstance();
    system.setRadix(DECIMAL);

    Prompt prompt;
    std::string answer = "0.00";

    bool isFormattingOn = true;
    bool loop = true;

    while (loop) {
        prompt.setPrompt(getPromptString(system.getRadix()));
        std::string response = prompt.read();

        if (response.compare("quit") == 0 || response.compare("q") == 0 || response.compare("exit") == 0) {
            loop = false;
        }
        else if (response.compare("version") == 0) {
            printVersion();
        }
        else if (response.compare("help") == 0 || response.compare("?") == 0) {
            printUsage();
        }
        else if (response.find("setp", 0) == 0) {
            std::string p = response.substr(4);
            precision = strtol(p.c_str(), NULL, 10);

            if (precision < 0L || precision > 80) {
                std::cout << "Invalid precision, must be between 0 and 80" << std::endl;
                precision = DEFAULT_PRECISION;
            }
        }
        else if (response.compare("fmton") == 0) {
            isFormattingOn = true;
        }
        else if (response.compare("fmtoff") == 0) {
            isFormattingOn = false;
        }
        else if (response.compare("test") == 0) {
            test();
        }
        else if (response.compare("dec") == 0) {
            int oldRadix = system.getRadix();

            if (oldRadix != DECIMAL) {
                system.setRadix(DECIMAL);

                Operand o(answer, oldRadix);
                std::cout << "Last answer = " << o.toString(precision) << std::endl << std::endl;
            }
        }
        else if (response.compare("hex") == 0) {
            int oldRadix = system.getRadix();

            if (oldRadix != HEXADECIMAL) {
                system.setRadix(HEXADECIMAL);

                Operand o(answer, oldRadix);
                std::cout << "Last answer = " << o.toString(precision) << std::endl << std::endl;
            }
         }
        else if (response.compare("oct") == 0) {
            int oldRadix = system.getRadix();

            if (oldRadix != OCTAL) {
                system.setRadix(OCTAL);

                Operand o(answer, oldRadix);
                std::cout << "Last answer = " << o.toString(precision) << std::endl << std::endl;
            }
        }
        else if (response.compare("bin") == 0) {
            int oldRadix = system.getRadix();

            if (oldRadix != BINARY) {
                system.setRadix(BINARY);

                Operand o(answer, oldRadix);
                std::cout << "Last answer = " << o.toString(precision) << std::endl << std::endl;
            }
        }
        else {
            if (response.length() > 0) {
                try {
                    Expression e(precision);
                    answer = e.evaluate(response);

                    if (isFormattingOn && system.getRadix() == DECIMAL) {
                        std::cout << "\t" << response << " = " << addThousandsSeparators(answer) << std::endl << std::endl;
                    }
                    else {
                        std::cout << "\t" << response << " = " << answer << std::endl << std::endl;
                    }
                }
                catch (calc_error & e) {
                    std::cout << "Error: " << e.what() << std::endl << std::endl;
                }
            }
        }
    }

    return 0;
}

void test() {
    const int testPrecision = 6;

    TestFramework t;

    t.addTest("2 + (3 * 4) ^ 2 - 13",                           "133.000000",   DECIMAL);
    t.addTest("2+(3*4)^2-13",                                   "133.000000",   DECIMAL);
    t.addTest("12 - ((2 * 3) - (8 / 2) / 0.5) / 12.653",        "12.158065",    DECIMAL);
    t.addTest("2 ^ 16 - 1",                                     "65535.000000", DECIMAL);
    t.addTest("(((((((1 + 2 * 3)-2)*4)/2)-12)+261)/12) - 5.25", "16.333333",    DECIMAL);
    t.addTest("pi + sin(45 + 45)",                              "4.141593",     DECIMAL);
    t.addTest("pi * (2 ^ 2)",                                   "12.566371",    DECIMAL);
    t.addTest("84 * -15 + sin(47)",                             "-1259.268646", DECIMAL);
    t.addTest("16 / (3 - 5 + 8) * (3 + 5 - 4)",                 "10.666667",    DECIMAL);
    t.addTest("sin(90) * cos(45) * tan(180) + asin(1) + acos(0) + atan(25)",    "267.709390",    DECIMAL);
    t.addTest("asin(sin(90)) + acos(cos(90))",                  "180.000000",   DECIMAL);
    t.addTest("fact(12) + 13",                                  "479001613.000000", DECIMAL);
    t.addTest("sin(45 + 45) + pi",                              "4.141593",     DECIMAL);
    t.addTest("-232 * 647.19",                                  "-150148.080000", DECIMAL);
    t.addTest("2 ^ 32 - 1",                                     "4294967295.000000",DECIMAL);
    t.addTest("sin(90)-1",                                      "0.000000",     DECIMAL);
    t.addTest("(F100 < 3) + (AA > 1)",                          "0000000000078855", HEXADECIMAL);
    t.addTest("10101010 | 1010101",                             "11111111",     BINARY);
    t.addTest("(1 < 111) | (1 < 110)",                          "11000000",     BINARY);

    int numTests = t.numTests();
    int numPassed = 0;
    int numFailed = 0;

    System & system = System::getInstance();

    int originalRadix = system.getRadix();
    
    Expression expression(testPrecision);

    for (int i = 0;i < numTests;i++) {
        std::string testExpression = t.getTestExpression(i);

        system.setRadix(t.getRadix(i));

        std::cout << "Testing: '" << testExpression << "'..." << std::endl;

        std::string actualResult = expression.evaluate(testExpression);

        if (t.assertTest(i, actualResult)) {
            std::cout << "    Test passed :) - expected: [" << t.getExpectedResult(i) << "], actual: [" << actualResult << "]" << std::endl << std::endl;
            numPassed++;
        }
        else {
            std::cout << "    Test failed :( - expected: [" << t.getExpectedResult(i) << "], actual: [" << actualResult << "]" << std::endl << std::endl;
            numFailed++;
        }
    }

    system.setRadix(originalRadix);

    std::cout << "Total tests ran: " << numTests << ", passed: " << numPassed << ", failed: " << numFailed << std::endl << std::endl;
}

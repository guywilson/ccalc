#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <gmp.h>
#include <mpfr.h>

#include "tokenizer.h"
#include "calc_error.h"
#include "calculator.h"
#include "utils.h"
#include "system.h"

using namespace std;

static bool testEvaluate(const char * pszCalculation, int radix, const char * pszExpectedResult) {
    mpfr_t          r;
    bool            success;
    string          result;

    mpfr_init2(r, getBasePrecision());

    try {
        evaluate(r, pszCalculation, radix);
    }
    catch (calc_error & e) {
        printf("**** Failed :( - Evaluate failed for [%s] with error: %s\n", pszCalculation, e.what());
        return false;
    }

    result = toString(r, radix, (long)getPrecision());

    if (strncmp(result.c_str(), pszExpectedResult, strlen(pszExpectedResult)) == 0) {
        printf("**** Success :) - [%s] Expected '%s', got '%s'\n", pszCalculation, pszExpectedResult, result.c_str());
        success = true;
    }
    else {
        printf("**** Failed :( - [%s] Expected '%s', got '%s'\n", pszCalculation, pszExpectedResult, result.c_str());
        success = false;
    }

    mpfr_clear(r);

    return success;
}

int test(void) {
    int             numTestsFailed = 0;
    int             numTestsPassed = 0;
    int             totalTests = 0;
    int             mode;

    setPrecision(2U);
    mode = DECIMAL;
    testEvaluate("2 + (3 * 4) ^ 2 - 13", mode, "133.00") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    mode = DECIMAL;
    testEvaluate("12 - ((2 * 3) - (8 / 2) / 0.5) / 12.653", mode, "12.16") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    mode = DECIMAL;
    testEvaluate("2 ^ 16 - 1", mode, "65535.00") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    mode = DECIMAL;
    testEvaluate("(((((((1 + 2 * 3)-2)*4)/2)-12)+261)/12) - 5.25", mode, "16.33") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    mode = DECIMAL;
    testEvaluate("pi + sin(45 + 45)", mode, "4.14") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    mode = DECIMAL;
    testEvaluate("pi * (2 ^ 2)", mode, "12.56") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    mode = DECIMAL;
    testEvaluate("3 + 4/(2 * 3 * 4) - 4/(4 * 5 * 6) + 4/(6 * 7 * 8) - 4/(8 * 9 * 10) + 4/(10 * 11 * 12)", mode, "3.14") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    mode = DECIMAL;
    testEvaluate("84 * -15 + sin(47)", mode, "-1259.27") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(8U);
    mode = DECIMAL;
    testEvaluate("16 / (3 - 5 + 8) * (3 + 5 - 4)", mode, "10.66") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    mode = DECIMAL;
    testEvaluate("sin(90) * cos(45) * tan(180) + asin(1) + acos(0) + atan(25)", mode, "267.71") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    mode = DECIMAL;
    testEvaluate("fact(12) + 13", mode, "479001613.0") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(0U);
    mode = HEXADECIMAL;
    testEvaluate("(F100 < 3) + (AA > 1)", mode, "0000000000078855") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(0U);
    mode = BINARY;
    testEvaluate("10101010 | 1010101", mode, "11111111") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(0U);
    mode = BINARY;
    testEvaluate("(1 < 111) | (1 < 110) | (1 < 101) | (1 < 100) | (1 < 11) | (1 < 10) | (1 < 1) | 1", mode, "11111111") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    mode = DECIMAL;
    testEvaluate("asin(sin(90)) + acos(cos(90))", mode, "180.00") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    printf("\nTest: %d tests failed, %d tests passed out of %d total\n\n", numTestsFailed, numTestsPassed, totalTests);

    return numTestsFailed;
}

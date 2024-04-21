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
    const char *    pszResult;

    mpfr_init2(r, getBasePrecision());

    try {
        evaluate(r, pszCalculation, radix);
    }
    catch (calc_error & e) {
        printf("**** Failed :( - Evaluate failed for [%s] with error: %s\n", pszCalculation, e.what());
        return false;
    }

    pszResult = toString(r, radix).c_str();

    if (strncmp(pszResult, pszExpectedResult, strlen(pszExpectedResult)) == 0) {
        printf("**** Success :) - [%s] Expected '%s', got '%s'\n", pszCalculation, pszExpectedResult, pszResult);
        success = true;
    }
    else {
        printf("**** Failed :( - [%s] Expected '%s', got '%s'\n", pszCalculation, pszExpectedResult, pszResult);
        success = false;
    }

    mpfr_clear(r);
    
    return success;
}

int test(void) {
    int             numTestsFailed = 0;
    int             numTestsPassed = 0;
    int             totalTests = 0;
    int             base;

    setPrecision(2U);
    base = DECIMAL;
    testEvaluate("2 + (3 * 4) ^ 2 - 13", base, "133.00") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    base = DECIMAL;
    testEvaluate("12 - ((2 * 3) - (8 / 2) / 0.5) / 12.653", base, "12.16") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    base = DECIMAL;
    testEvaluate("2 ^ 16 - 1", base, "65535.00") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    base = DECIMAL;
    testEvaluate("(((((((1 + 2 * 3)-2)*4)/2)-12)+261)/12) - 5.25", base, "16.33") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    base = DECIMAL;
    testEvaluate("pi + sin(45 + 45)", base, "4.14") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    base = DECIMAL;
    testEvaluate("pi * (2 ^ 2)", base, "12.57") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    base = DECIMAL;
    testEvaluate("3 + 4/(2 * 3 * 4) - 4/(4 * 5 * 6) + 4/(6 * 7 * 8) - 4/(8 * 9 * 10) + 4/(10 * 11 * 12)", base, "3.14") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    base = DECIMAL;
    testEvaluate("84 * -15 + sin(47)", base, "-1259.27") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(8U);
    base = DECIMAL;
    testEvaluate("16 / (3 - 5 + 8) * (3 + 5 - 4)", base, "10.66") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    base = DECIMAL;
    testEvaluate("sin(90) * cos(45) * tan(180) + asin(1) + acos(0) + atan(25)", base, "267.71") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    base = DECIMAL;
    testEvaluate("fact(12) + 13", base, "479001613.0") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(0U);
    base = HEXADECIMAL;
    testEvaluate("(F100 < 3) + (AA > 1)", base, "00078855") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(0U);
    base = BINARY;
    testEvaluate("10101010 | 1010101", base, "11111111") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(0U);
    base = BINARY;
    testEvaluate("(1 < 111) | (1 < 110) | (1 < 101) | (1 < 100) | (1 < 11) | (1 < 10) | (1 < 1) | 1", base, "11111111") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    base = DECIMAL;
    testEvaluate("asin(sin(90)) + acos(cos(90))", base, "180.00") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    printf("\nTest: %d tests failed, %d tests passed out of %d total\n\n", numTestsFailed, numTestsPassed, totalTests);

    return numTestsFailed;
}

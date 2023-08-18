#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <gmp.h>
#include <mpfr.h>

#include "tokenizer.h"
#include "calculator.h"
#include "utils.h"

static bool testEvaluate(const char * pszCalculation, const char * pszExpectedResult) {
    token_t         r;
    bool            success;

    evaluate(pszCalculation, &r);

    r.pszToken[strlen(pszExpectedResult)] = 0;

    if (strncmp(r.pszToken, pszExpectedResult, strlen(pszExpectedResult)) == 0) {
        printf("**** Success :) - [%s] Expected '%s', got '%s'\n", pszCalculation, pszExpectedResult, r.pszToken);
        success = true;
    }
    else {
        printf("**** Failed :( - [%s] Expected '%s', got '%s'\n", pszCalculation, pszExpectedResult, r.pszToken);
        success = false;
    }

    return success;
}

int test(void) {
    int             numTestsFailed = 0;
    int             numTestsPassed = 0;
    int             totalTests = 0;

    setPrecision(2U);
    setBase(DECIMAL);
    testEvaluate("2 + (3 * 4) ^ 2 - 13", "133.00") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    setBase(DECIMAL);
    testEvaluate("12 - ((2 * 3) - (8 / 2) / 0.5) / 12.653", "12.16") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    setBase(DECIMAL);
    testEvaluate("2 ^ 16 - 1", "65535.00") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    setBase(DECIMAL);
    testEvaluate("(((((((1 + 2 * 3)-2)*4)/2)-12)+261)/12) - 5.25", "16.33") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    setBase(DECIMAL);
    setTrigMode(degrees);
    testEvaluate("pi + sin(45 + 45)", "4.14") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    setBase(DECIMAL);
    testEvaluate("pi * (2 ^ 2)", "12.57") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    setBase(DECIMAL);
    testEvaluate("3 + 4/(2 * 3 * 4) - 4/(4 * 5 * 6) + 4/(6 * 7 * 8) - 4/(8 * 9 * 10) + 4/(10 * 11 * 12)", "3.14") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    setBase(DECIMAL);
    setTrigMode(degrees);
    testEvaluate("84 * -15 + sin(47)", "-1259.27") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(8U);
    setBase(DECIMAL);
    testEvaluate("16 / (3 - 5 + 8) * (3 + 5 - 4)", "10.66") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    setBase(DECIMAL);
    setTrigMode(degrees);
    testEvaluate("sin(90) * cos(45) * tan(180) + asin(1) + acos(0) + atan(25)", "267.71") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    setPrecision(2U);
    setBase(DECIMAL);
    testEvaluate("fact(12) + 13", "479001613.0") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    printf("\nTest: %d tests failed, %d tests passed out of %d total\n\n", numTestsFailed, numTestsPassed, totalTests);

    return numTestsFailed;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <gmp.h>
#include <mpfr.h>

#include "tokenizer.h"
#include "operand.h"
#include "calc_error.h"
#include "calculator.h"
#include "system.h"

static bool testEvaluate(const char * pszCalculation, const char * pszExpectedResult) {
    operand_t *     r;
    bool            success;
    const char *    pszResult;

    system_t & sys = system_t::getInstance();

    try {
        r = evaluate(pszCalculation);
    }
    catch (calc_error & e) {
        printf("**** Failed :( - Evaluate failed for [%s] with error: %s\n", pszCalculation, e.what());
        return false;
    }

    pszResult = r->toString(sys.getBase()).c_str();

    if (strncmp(pszResult, pszExpectedResult, strlen(pszExpectedResult)) == 0) {
        printf("**** Success :) - [%s] Expected '%s', got '%s'\n", pszCalculation, pszExpectedResult, pszResult);
        success = true;
    }
    else {
        printf("**** Failed :( - [%s] Expected '%s', got '%s'\n", pszCalculation, pszExpectedResult, pszResult);
        success = false;
    }

    return success;
}

int test(void) {
    int             numTestsFailed = 0;
    int             numTestsPassed = 0;
    int             totalTests = 0;

    system_t & sys = system_t::getInstance();

    sys.setPrecision(2U);
    sys.setBase(DECIMAL);
    testEvaluate("2 + (3 * 4) ^ 2 - 13", "133.00") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    sys.setPrecision(2U);
    sys.setBase(DECIMAL);
    testEvaluate("12 - ((2 * 3) - (8 / 2) / 0.5) / 12.653", "12.16") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    sys.setPrecision(2U);
    sys.setBase(DECIMAL);
    testEvaluate("2 ^ 16 - 1", "65535.00") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    sys.setPrecision(2U);
    sys.setBase(DECIMAL);
    testEvaluate("(((((((1 + 2 * 3)-2)*4)/2)-12)+261)/12) - 5.25", "16.33") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    sys.setPrecision(2U);
    sys.setBase(DECIMAL);
    sys.setTrigMode(degrees);
    testEvaluate("pi + sin(45 + 45)", "4.14") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    sys.setPrecision(2U);
    sys.setBase(DECIMAL);
    testEvaluate("pi * (2 ^ 2)", "12.57") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    sys.setPrecision(2U);
    sys.setBase(DECIMAL);
    testEvaluate("3 + 4/(2 * 3 * 4) - 4/(4 * 5 * 6) + 4/(6 * 7 * 8) - 4/(8 * 9 * 10) + 4/(10 * 11 * 12)", "3.14") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    sys.setPrecision(2U);
    sys.setBase(DECIMAL);
    sys.setTrigMode(degrees);
    testEvaluate("84 * -15 + sin(47)", "-1259.27") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    sys.setPrecision(8U);
    sys.setBase(DECIMAL);
    testEvaluate("16 / (3 - 5 + 8) * (3 + 5 - 4)", "10.66") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    sys.setPrecision(2U);
    sys.setBase(DECIMAL);
    sys.setTrigMode(degrees);
    testEvaluate("sin(90) * cos(45) * tan(180) + asin(1) + acos(0) + atan(25)", "267.71") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    sys.setPrecision(2U);
    sys.setBase(DECIMAL);
    testEvaluate("fact(12) + 13", "479001613.0") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    sys.setPrecision(0U);
    sys.setBase(HEXADECIMAL);
    testEvaluate("(F100 < 3) + (AA > 1)", "00078855") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    sys.setPrecision(0U);
    sys.setBase(BINARY);
    testEvaluate("10101010 | 1010101", "11111111") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    sys.setPrecision(0U);
    sys.setBase(BINARY);
    testEvaluate("(1 < 111) | (1 < 110) | (1 < 101) | (1 < 100) | (1 < 11) | (1 < 10) | (1 < 1) | 1", "11111111") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    sys.setPrecision(2U);
    sys.setBase(DECIMAL);
    testEvaluate("asin(sin(90)) + acos(cos(90))", "180.00") ? numTestsPassed++ : numTestsFailed++;
    totalTests++;

    printf("\nTest: %d tests failed, %d tests passed out of %d total\n\n", numTestsFailed, numTestsPassed, totalTests);

    return numTestsFailed;
}

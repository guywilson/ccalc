#pragma once

#include <string>
#include <vector>

typedef struct {
    std::string expression;
    std::string expectedResult;

    int radix;
}
calc_test_t;

class TestFramework {
    private:
        std::vector<calc_test_t *> tests;

    public:
        ~TestFramework() {
            for (calc_test_t * t : tests) {
                delete t;
            }
        }

        void addTest(const std::string & expression, const std::string & expectedResult, int radix) {
            calc_test_t * test = new calc_test_t;

            test->expression = expression;
            test->expectedResult = expectedResult;
            test->radix = radix;

            tests.push_back(test);
        }

        int numTests() {
            return tests.size();
        }

        std::string getTestExpression(int i) {
            return tests[i]->expression;
        }

        std::string getExpectedResult(int i) {
            return tests[i]->expectedResult;
        }

        int getRadix(int i) {
            return tests[i]->radix;
        } 

        bool assertTest(int i, std::string & actualResult) {
            if (actualResult.compare(tests[i]->expectedResult) == 0) {
                return true;
            }

            return false;
        }
};

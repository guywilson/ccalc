#pragma once

#include <string>

#include <ctype.h>
#include <stdint.h>

#include <gmp.h>
#include <mpfr.h>

#include "system.h"
#include "token.h"

#define MPFR_BASE_PRECISION           1024L

#define INTERMEDIATE_PRECISION          256

#define FORMAT_STRING_LENGTH             32
#define OUTPUT_MAX_STRING_LENGTH       4096

#define BASE2_OUTPUT_LEN               (sizeof(uint32_t) * 8)

static inline bool isDigit(char ch, int radix) {
    bool is_digit = false;

    switch (radix) {
        case DECIMAL:
            is_digit = isdigit(ch);
            break;

        case HEXADECIMAL:
            is_digit = isxdigit(ch);
            break;

        case OCTAL:
            is_digit = (ch >= '0' && ch < '8');
            break;

        case BINARY:
            is_digit = (ch == '0' || ch == '1');
            break;
    }

    return is_digit;
}

static inline bool isOperandChar(char ch, int radix) {
    return (isDigit(ch, radix) || ch == '-' || ch == '.');
}

class Operand : public Token {
    private:
        void initialiseValue() {
            mpfr_init2(value, MPFR_BASE_PRECISION);
        }

        std::string toBase2() {
            char szBinaryString[BASE2_OUTPUT_LEN + 1];
            char szOutputString[BASE2_OUTPUT_LEN + 1];
            int i = BASE2_OUTPUT_LEN - 1;
            int bit = 0;
            uint32_t uiValue = mpfr_get_ui(value, MPFR_RNDA);

            szBinaryString[BASE2_OUTPUT_LEN] = 0;

            while (uiValue > 0) {
                bit = uiValue % 2;

                uiValue = uiValue >> 1;

                szBinaryString[i--] = ((char)bit + '0');
            }

            strncpy(szOutputString, &szBinaryString[i + 1], BASE2_OUTPUT_LEN);

            std::string output = szOutputString;

            return output;
        }

    public:
        mpfr_t value;

        Operand() {
            initialiseValue();
        }

        Operand(const std::string & token, int radix = DECIMAL) : Token(token) {
            initialiseValue();
            mpfr_strtofr(value, token.c_str(), NULL, radix, MPFR_RNDA);
        }

        Operand(mpfr_t src) {
            initialiseValue();
            mpfr_set(value, src, MPFR_RNDA);
        }

        void clear() {
            mpfr_clear(value);
        }

        static bool isOperand(const std::string & token) {
            System & system = System::getInstance();

            for (int i = 0;i < token.length();i++) {
                char ch = token[i];

                if (!isOperandChar(ch, system.getRadix())) {
                    return false;
                }
            }

            return true;
        }

        std::string toString(long precision) {
            char szOutputString[OUTPUT_MAX_STRING_LENGTH];
            char szFormatString[FORMAT_STRING_LENGTH];
            std::string output;

            System & system = System::getInstance();

            switch (system.getRadix()) {
                case DECIMAL:
                    snprintf(szFormatString, FORMAT_STRING_LENGTH, "%%.%ldRf", precision);
                    mpfr_snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, szFormatString, value);
                    output.assign(szOutputString);
                    break;

                case HEXADECIMAL:
                    snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, "%016lX", mpfr_get_ui(value, MPFR_RNDA));
                    output.assign(szOutputString);
                    break;

                case OCTAL:
                    snprintf(szOutputString, OUTPUT_MAX_STRING_LENGTH, "%lo", mpfr_get_ui(value, MPFR_RNDA));
                    output.assign(szOutputString);
                    break;

                case BINARY:
                    output.assign(toBase2());
                    break;
            }

            return output;
        }

        virtual std::string evaluate() override {
            return toString(INTERMEDIATE_PRECISION);
        }

        static const std::string CLASS_NAME() {
            return "Operand";
        }

        virtual const std::string className() override {
            return Operand::CLASS_NAME();
        }

        const Operand sin() {
            Operand result;
            mpfr_sinu(result.value, this->value, 360U, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand cos() {
            Operand result;
            mpfr_cosu(result.value, this->value, 360U, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand tan() {
            Operand result;
            mpfr_tanu(result.value, this->value, 360U, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand asin() {
            Operand result;
            mpfr_asinu(result.value, this->value, 360U, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand acos() {
            Operand result;
            mpfr_acosu(result.value, this->value, 360U, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand atan() {
            Operand result;
            mpfr_atanu(result.value, this->value, 360U, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand sinh() {
            Operand result;
            mpfr_sinh(result.value, this->value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand cosh() {
            Operand result;
            mpfr_cosh(result.value, this->value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand tanh() {
            Operand result;
            mpfr_tanh(result.value, this->value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand asinh() {
            Operand result;
            mpfr_asinh(result.value, this->value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand acosh() {
            Operand result;
            mpfr_acosh(result.value, this->value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand atanh() {
            Operand result;
            mpfr_atanh(result.value, this->value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand sqrt() {
            Operand result;
            mpfr_sqrt(result.value, this->value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand log() {
            Operand result;
            mpfr_log10(result.value, this->value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand ln() {
            Operand result;
            mpfr_log(result.value, this->value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand factorial() {
            Operand result;
            mpfr_fac_ui(result.value, mpfr_get_ui(this->value, MPFR_RNDA), MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand bitwise_not() {
            Operand result;
            mpfr_set_ui(result.value, (~mpfr_get_ui(this->value, MPFR_RNDA)), MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand bitwise_xor(const Operand & rhs) {
            Operand result;
            mpfr_set_ui(result.value, (mpfr_get_ui(this->value, MPFR_RNDA) ^ mpfr_get_ui(rhs.value, MPFR_RNDA)), MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand root(const Operand & rhs) {
            Operand result;
            mpfr_rootn_ui(result.value, this->value, mpfr_get_ui(rhs.value, MPFR_RNDA), MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand deg() {
            Operand result;

            mpfr_t  pi;
            mpfr_t  one_eighty;

            mpfr_init2(pi, MPFR_BASE_PRECISION);
            mpfr_init2(one_eighty, MPFR_BASE_PRECISION);

            mpfr_const_pi(pi, MPFR_RNDA);
            mpfr_set_ui(one_eighty, 180U, MPFR_RNDA);
            
            mpfr_div(result.value, one_eighty, pi, MPFR_RNDA);
            mpfr_mul(result.value, result.value, this->value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand rad() {
            Operand result;

            mpfr_t  pi;
            mpfr_t  one_eighty;

            mpfr_init2(pi, MPFR_BASE_PRECISION);
            mpfr_init2(one_eighty, MPFR_BASE_PRECISION);

            mpfr_const_pi(pi, MPFR_RNDA);
            mpfr_set_ui(one_eighty, 180U, MPFR_RNDA);
            
            mpfr_div(result.value, pi, one_eighty, MPFR_RNDA);
            mpfr_mul(result.value, result.value, this->value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand operator+(const Operand & rhs) {
            Operand result;
            mpfr_add(result.value, this->value, rhs.value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand operator-(const Operand & rhs) {
            Operand result;
            mpfr_sub(result.value, this->value, rhs.value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand operator*(const Operand & rhs) {
            Operand result;
            mpfr_mul(result.value, this->value, rhs.value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand operator/(const Operand & rhs) {
            Operand result;
            mpfr_div(result.value, this->value, rhs.value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand operator%(const Operand & rhs) {
            Operand result;
            mpfr_remainder(result.value, this->value, rhs.value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand operator^(const Operand & rhs) {
            Operand result;
            mpfr_pow(result.value, this->value, rhs.value, MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand operator&(const Operand & rhs) {
            Operand result;
            mpfr_set_ui(result.value, (mpfr_get_ui(this->value, MPFR_RNDA) & mpfr_get_ui(rhs.value, MPFR_RNDA)), MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand operator|(const Operand & rhs) {
            Operand result;
            mpfr_set_ui(result.value, (mpfr_get_ui(this->value, MPFR_RNDA) | mpfr_get_ui(rhs.value, MPFR_RNDA)), MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand operator<<(const Operand & rhs) {
            Operand result;
            mpfr_set_ui(result.value, (mpfr_get_ui(this->value, MPFR_RNDA) << mpfr_get_ui(rhs.value, MPFR_RNDA)), MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }

        const Operand operator>>(const Operand & rhs) {
            Operand result;
            mpfr_set_ui(result.value, (mpfr_get_ui(this->value, MPFR_RNDA) >> mpfr_get_ui(rhs.value, MPFR_RNDA)), MPFR_RNDA);
            result.setToken(result.toString(INTERMEDIATE_PRECISION));
            return result;
        }
};

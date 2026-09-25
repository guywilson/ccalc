#pragma once

#include <string>

#define BASE_10                          10
#define BASE_16                          16
#define BASE_8                            8
#define BASE_2                            2

#define DECIMAL                         BASE_10
#define HEXADECIMAL                     BASE_16
#define OCTAL                           BASE_8
#define BINARY                          BASE_2

class System {
    private:
        int radix;

    public:
        static System & getInstance() {
            static System instance;
            return instance;
        }

    private:
        System() {
            radix = DECIMAL;
        }

    public:
        ~System() {}

        int getRadix() {
            return radix;
        }
        void setRadix(int r) {
            radix = r;
        }
};

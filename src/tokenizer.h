#pragma once

#include <string>
#include <vector>
#include <string.h>
#include <stdint.h>

#include "container.h"
#include "token.h"

class Tokenizer {
    private:
        uint32_t startIndex;
        uint32_t endIndex;

        std::string expression;
        
        int findNextTokenPos();
        int getTokenLength();

    public:
        Tokenizer(const std::string & expression);

        TokenArray tokenize();
};

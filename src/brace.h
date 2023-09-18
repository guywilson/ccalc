#include <string>

#include "system.h"
#include "token.h"
#include "logger.h"

using namespace std;

#ifndef __INCL_BRACE
#define __INCL_BRACE

class brace_t : public token_t {
    public:
        brace_t(string & s) : token_t(s) {
            setType(token_brace);
        }
        brace_t(const char * s) : token_t(s) {
            setType(token_brace);
        }

        virtual bool isBraceLeft() = 0;
        virtual bool isBraceRight() = 0;
};

class brace_left_t : public brace_t {
    public:
        brace_left_t(string & s) : brace_t(s) {}
        brace_left_t(const char * s) : brace_t(s) {}

        bool isBraceLeft() {
            return true;
        }

        bool isBraceRight() {
            return false;
        }
};

class brace_right_t : public brace_t {
    public:
        brace_right_t(string & s) : brace_t(s) {}
        brace_right_t(const char * s) : brace_t(s) {}

        bool isBraceLeft() {
            return false;
        }

        bool isBraceRight() {
            return true;
        }
};

#endif

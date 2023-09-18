#include <string>
#include <cstring>

#include "system.h"
#include "logger.h"

using namespace std;

#ifndef __INCL_TOKEN
#define __INCL_TOKEN

#define getBasePrecision()                  1024L

#define BASE_10                 10
#define BASE_16                 16
#define BASE_8                   8
#define BASE_2                   2

#define DECIMAL                 BASE_10
#define HEXADECIMAL             BASE_16
#define OCTAL                   BASE_8
#define BINARY                  BASE_2

typedef enum {
    token_operator,
    token_function,
    token_constant,
    token_brace,
    token_operand
}
token_type_t;

class token_t {
    private:
        token_type_t    type;
        
        string  tokenString;
        size_t  length;
        int     base;

    protected:
        token_t() {}

        void setType(token_type_t t) {
            type = t;
        }

    public:
        token_type_t getType() {
            return type;
        }

        static const char * getTokenTypeStr(token_t * t) {
            if (t->getType() == token_brace) {
                return "brace";
            }
            else if (t->getType() == token_constant) {
                return "constant";
            }
            else if (t->getType() == token_function) {
                return "function";
            }
            else if (t->getType() == token_operand) {
                return "operand";
            }
            else if (t->getType() == token_operator) {
                return "operator";
            }
            else {
                return "unknown";
            }
        }

        string & getTokenStr() {
            return tokenString;
        }

        void setTokenStr(string & s) {
            tokenString.assign(s);
            length = s.length();
        }

        void setTokenStr(const char * s) {
            tokenString.assign(s);
            length = strlen(s);
        }

        void setBase(int b) {
            base = b;
        }

        int getBase() {
            return base;
        }

        size_t getLength() {
            return length;
        }

        token_t(string & s) {
            setTokenStr(s);
        }

        token_t(const char * s) {
            setTokenStr(s);
        }

        token_t(token_t & t) {
            setTokenStr(t.getTokenStr());
        }
};

#endif

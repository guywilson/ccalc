#include <exception>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

using namespace std;

#ifndef _CALC_ERROR
#define _CALC_ERROR

class calc_error : public exception
{
    private:
        string          message;

    public:
        const char * getTitle() {
            return "Calc error: ";
        }

        calc_error() {
            this->message.assign(getTitle());
        }

        calc_error(const char * msg) : calc_error() {
            this->message.append(msg);
        }

        calc_error(const char * msg, const char * file, int line) : calc_error() {
            char lineNumBuf[8];

            snprintf(lineNumBuf, 8, ":%d", line);

            this->message.append(msg);
            this->message.append(" at ");
            this->message.append(file);
            this->message.append(lineNumBuf);
        }

        virtual const char * what() const noexcept {
            return this->message.c_str();
        }

        static const char * buildMsg(const char * fmt, ...) {
            va_list     args;
            char *      buffer;
            size_t      length;

            va_start(args, fmt);
            
            length = strlen(fmt) + 80;

            buffer = (char *)malloc(length);
            
            vsnprintf(buffer, length, fmt, args);
            
            va_end(args);

            return buffer;
        }
};

class unmatched_parenthesis_error : public calc_error {
    public:
        const char * getTitle() {
            return "Unmatched Parenthesis error: ";
        }

        unmatched_parenthesis_error() : calc_error() {}
        unmatched_parenthesis_error(const char * msg) : calc_error(msg) {}
        unmatched_parenthesis_error(const char * msg, const char * file, int line) : calc_error(msg, file, line) {}
};

class stack_error : public calc_error {
    public:
        const char * getTitle() {
            return "Stack error: ";
        }

        stack_error() : calc_error() {}
        stack_error(const char * msg) : calc_error(msg) {}
        stack_error(const char * msg, const char * file, int line) : calc_error(msg, file, line) {}
};

#endif

#include <stdlib.h>

#include <gmp.h>
#include <mpfr.h>

#ifndef __INCL_SYSTEM
#define __INCL_SYSTEM

typedef enum {
    radians,
    degrees
}
trig_mode_t;

class system_t {
    public:
        static system_t & getInstance() {
            static system_t sys;
            return sys;
        }

    private:
        system_t() {}

        int             base;
        mpfr_prec_t     precision;
        trig_mode_t     mode;

    public:
        void setBase(int b) {
            base = b;
        }

        int getBase() {
            return base;
        }

        void setPrecision(mpfr_prec_t p) {
            precision = p;
        }

        mpfr_prec_t getPrecision() {
            return precision;
        }

        void setTrigMode(trig_mode_t m) {
            mode = m;
        }

        trig_mode_t getTrigMode() {
            return mode;
        }
};

#endif

#include <stdlib.h>

#include <gmp.h>
#include <mpfr.h>

#include "operand.h"
#include "calc_error.h"

#ifndef __INCL_MEMORY
#define __INCL_MEMORY

#define NUM_MEMORY_LOCATIONS            10

class memory_t {
    public:
        static memory_t & getInstance() {
            static memory_t mem;
            return mem;
        }

    private:
        memory_t() {}

        operand_t *     mem[NUM_MEMORY_LOCATIONS];

    public:
        void store(int location, operand_t * value) {
            if (location < 0 || location > 9) {
                throw calc_error("Memory index out-of-range", __FILE__, __LINE__);
            }

            mem[location] = value;
        }

        operand_t * retrieve(int location) {
            if (location < 0 || location > 9) {
                throw calc_error("Memory index out-of-range", __FILE__, __LINE__);
            }

            return mem[location];
        }
};

#endif

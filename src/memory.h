#include <stdlib.h>
#include <map>

#include <gmp.h>
#include <mpfr.h>

#include "operand.h"
#include "calc_error.h"

using namespace std;

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
        map<int, mpfr_ptr>       memMap;

        memory_t() {
            int             i;
            static mpfr_t   mem[NUM_MEMORY_LOCATIONS];

            for (i = 0;i < NUM_MEMORY_LOCATIONS;i++) {
                mpfr_init2(mem[i], getBasePrecision());

                mpfr_set_d(mem[0], 0.0, MPFR_RNDA);

                memMap.insert(std::pair<int, mpfr_ptr>(i, mem[i]));
            }
        }

    public:
        void store(int location, operand_t * value) {
            if (location < 0 || location > 9) {
                throw calc_error("Memory index out-of-range", __FILE__, __LINE__);
            }

            mpfr_ptr v = memMap[location];

            mpfr_set(v, value->getValue(), MPFR_RNDA);

            memMap.insert_or_assign(location, v);
        }

        operand_t * retrieve(int location) {
            if (location < 0 || location > 9) {
                throw calc_error("Memory index out-of-range", __FILE__, __LINE__);
            }

            operand_t * o = new operand_t(memMap[location]);

            return o;
        }
};

#endif

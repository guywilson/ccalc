#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <gmp.h>
#include <mpfr.h>
#include <readline/readline.h>
#include <readline/history.h>

void printUsage(void) {

}

int main(int argc, char ** argv) {
    char *              pszCalculation;
    bool                loop = true;

    rl_bind_key('\t', rl_complete);

    // Enable history
    using_history();

    while (loop) {
        // Display prompt and read input
        pszCalculation = readline("calc> ");

        // Add input to readline history.
        add_history(pszCalculation);

        // Check for EOF.
        if (strncmp(pszCalculation, "exit", 4) == 0 || strncmp(pszCalculation, "quit", 4) == 0 || pszCalculation[0] == 'q') {
            loop = false;
        }
        else {
            
        // Do stuff...
        }

        // Free buffer that was allocated by readline
        free(pszCalculation);
    }
}

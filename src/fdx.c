#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "machine/machine.h"
#include "machine/loop.h"
#include "machine/instructions.h"

static char usage[] = "Usage: %s PROGRAM_FILE\n";

static uint8_t program[256];
static FILE *program_file;
static size_t program_size;

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, usage, argv[0]);
        return EXIT_FAILURE;
    }

    program_file = fopen(argv[1], "r");
    if (!program_file) {
        fprintf(stderr,
                "%s: %s: %s\n",
                argv[0],
                argv[1],
                strerror(errno));
        return EXIT_FAILURE;
    }

    program_size = fread((void *) program, 1, 256, program_file);
    if (ferror(program_file)) {
        fprintf(stderr,
                "%s: %s: %s\n",
                argv[0],
                argv[1],
                strerror(ferror(program_file)));
        return EXIT_FAILURE;
    }

    if (!feof(program_file)) {
        fprintf(stderr, 
                "%s: %s: %s\n",
                argv[0],
                argv[1],
                "Program is too big\n");
        return EXIT_FAILURE;
    }

    init_fdx_instruction_table();
    fdx_load_program(program, program_size);
    fdx_execution_loop();

    return EXIT_SUCCESS;
}

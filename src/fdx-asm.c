#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "assembler/assembler.h"

static char usage[] = "Usage: %s ASM_FILE [OUTPUT_PROGRAM]\n";

static uint8_t output_prog[256];
static char *argv_0;

static FILE *open_file(char *filename, char *mode)
{
    FILE *file;
    file = fopen(filename, mode);
    if (!file) {
        fprintf(stderr,
                "%s: %s: %s\n",
                argv_0,
                filename,
                strerror(errno));
        exit(EXIT_FAILURE);
    }
    return file;
}

int main(int argc, char* argv[])
{
    FILE *input_file, *output_file;
    argv_0 = argv[0];
    int prog_size;

    if (argc  < 2 || argc > 3) {
        fprintf(stderr, usage, argv[0]);
        return EXIT_FAILURE;
    }

    // Abre o arquivo com as instruções
    input_file = open_file(argv[1], "r");

    // Abre o arquivo de saida
    output_file = (argc < 3) ?
        open_file("a.out", "w") : open_file(argv[2], "w");

    // Compila as instruções e gera um programa de tamanho prog_size
    fdx_assemble(input_file, output_prog, &prog_size);

    // Escreve o output_prog no arquivo de saida
    fwrite(output_prog, sizeof(uint8_t), prog_size, output_file);
    if (ferror(output_file)) {
        fprintf(stderr, "%s\n", strerror(ferror(output_file)));
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

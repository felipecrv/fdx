#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "machine.h"
#include "instructions.h"
#include "loop.h"

/**
 * Carrega o programa na memória.
 */
void fdx_load_program(uint8_t *prog, int prog_len)
{
    memcpy(fdx_memory, prog, prog_len);
}

static void fdx_fetch_instruction()
{
    fdx_IR = fdx_memory[fdx_PC];
    // Carregamos o próximo byte (que a depender da instrução pode ser um
    // operando).
    fdx_operand = fdx_memory[fdx_PC + 1];
}

static void fdx_execute_instruction()
{

    if (fdx_instruction_length() == 2) {
        (*((two_byte_instruction_fn_t) 
           fdx_instruction_table[fdx_IR >> 3].fn))(fdx_operand);
    } else {
        (*((one_byte_instruction_fn_t) 
           fdx_instruction_table[fdx_IR >> 3].fn))();
    }
}

static int fdx_instruction_length()
{
    return fdx_instruction_table[fdx_IR >> 3].length;
}

void fdx_execution_loop()
{
    int j, i = 20;

    // A próxima instrução está na posição 0
    fdx_PC = 0;
    for (;;) {
        // Carrega a instrução no registrador de instrução.
        fdx_fetch_instruction();

        // A próxima instrução será executada na próxima iteração, a menos que
        // alguma instrução de jump seja executada.
        fdx_PC += fdx_instruction_length();

        // Executa a instrução
        fdx_execute_instruction();
    }
}

#include <stdlib.h>
#include <stdint.h>
#include "machine.h"
#include "instructions.h"

struct fdx_instruction_table_entry
    fdx_instruction_table[FDX_INSTRUCTION_SET_SIZE];

#define OP(OPCODE, FN, LEN) \
    fdx_instruction_table[(OPCODE)].fn = &(FN); \
    fdx_instruction_table[(OPCODE)].length = (LEN);

void init_fdx_instruction_table()
{
    // opcode = 5 bits
    OP(0x00, fdx_nop, 1)
    OP(0x01, fdx_store, 2)
    OP(0x02, fdx_load, 2)
    OP(0x03, fdx_add, 2)
    OP(0x04, fdx_or, 2)
    OP(0x05, fdx_and, 2)
    OP(0x06, fdx_not, 1)

    OP(0x07, fdx_subtraction, 2) // Extensão

    OP(0x08, fdx_jump, 2)
    OP(0x09, fdx_jump_on_negative, 2)
    OP(0x0A, fdx_jump_on_zero, 2)

    // Extensões
    //OP(0x0B, ..

    OP(0x1F, fdx_halt, 1)
}

/**
 * NOP - Nenhuma operação
 */
inline void fdx_nop()
{
    // Faz nada!
}

/**
 * STA addr - Store
 *
 * Armazena conteúdo de AC no endereço addr
 */
void fdx_store(uint8_t addr)
{
    fdx_memory[addr] = fdx_AC;
}

/**
 * LDA addr - Load
 *
 * Carrega a palavra do endereço addr no acumulador AC
 */
void fdx_load(uint8_t addr)
{
    fdx_AC = fdx_memory[addr];
}

/**
 * ADD addr - Soma (signed)
 *
 * Soma AC com a palavra em addr e coloca o resultado em AC
 */
void fdx_add(uint8_t addr)
{
    static int8_t signed_AC;
    signed_AC = (int8_t) fdx_AC;

    signed_AC += (int8_t) fdx_memory[addr];
    fdx_AC = (uint8_t) signed_AC;
}

// TODO: documentar estas instruções

/**
 * OR addr - Ou lógico
 */
void fdx_or(uint8_t addr)
{
    fdx_AC |= fdx_memory[addr];
}

void fdx_and(uint8_t addr)
{
    fdx_AC &= fdx_memory[addr];
}

void fdx_not()
{
    fdx_AC = ~fdx_AC;
}

void fdx_jump(uint8_t addr)
{
    fdx_PC = addr;
}

void fdx_jump_on_negative(uint8_t addr)
{
    fdx_state = (((int8_t) fdx_AC) < 0) ? fdx_N : 0;
    if (fdx_state == fdx_N) fdx_PC = addr;
}

void fdx_jump_on_zero(uint8_t addr)
{
    fdx_state = (fdx_AC == 0) ? fdx_Z : 0;
    if (fdx_state == fdx_Z) fdx_PC = addr;
}

void fdx_halt()
{
    exit(0);
}

// Extensões

/**
 * SUB - Subtração (signed)
 */
void fdx_subtraction(uint8_t addr)
{
    static int8_t signed_AC;
    signed_AC = (int8_t) fdx_AC;

    signed_AC -= (int8_t) fdx_memory[addr];
    fdx_AC = (uint8_t) signed_AC;
}

/**
 * ADDU - Soma (unsigned)
 */
void fdx_add_unsigned(uint8_t addr)
{
    fdx_AC += fdx_memory[addr];
}

/**
 * SUBU - Subtração (unsigned)
 */
void fdx_subtraction_unsigned(uint8_t addr)
{
    fdx_AC -= fdx_memory[addr];
}

/**
 * LDI - Load Immediate
 *
 * Carrega o valor do operando no acumulador AC
 */
void fdx_load_immediate(uint8_t value)
{
    fdx_AC = value;
}

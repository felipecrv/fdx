#include <stdlib.h>
#include <stdint.h>
#include "machine.h"
#include "instructions.h"

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

void fdx_subtraction(uint8_t addr)
{
    static int8_t signed_AC;
    signed_AC = (int8_t) fdx_AC;

    signed_AC -= (int8_t) fdx_memory[addr];
    fdx_AC = (uint8_t) signed_AC;
}

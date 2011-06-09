#ifndef MACHINE_H_
#define MACHINE_H_

/**
 * Aqui são definidos todos os elementos da máquina:
 *
 *  - Memória Principal
 *  - Acumuladores
 */

// A arquitetura Neander define incríveis 256 bytes de memória RAM
extern uint8_t fdx_memory[256];

// Os acumuladores de 8bits
//
// O *único* acumulador "usável" do FD-X
extern uint8_t fdx_AC;
// O apontador de programa
extern uint8_t fdx_PC;
// Um registrador de estado com dois códigos de condição: negativo (N) e zero(Z)
extern uint8_t fdx_state;

// Os dois possíveis estados de fdx_state
#define fdx_N 2
#define fdx_Z 1

// Instruction register
extern uint8_t fdx_IR;

// Operando (usado em instruções de 2 bytes)
extern uint8_t fdx_operand;

#endif /*MACHINE_H_*/

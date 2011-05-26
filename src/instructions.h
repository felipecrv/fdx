#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

void fdx_nop();
void fdx_store(uint8_t);
void fdx_load(uint8_t);
void fdx_add(uint8_t);
void fdx_or(uint8_t);
void fdx_and(uint8_t);
void fdx_not();
void fdx_jump(uint8_t);
void fdx_jump_on_negative(uint8_t);
void fdx_jump_on_zero(uint8_t);
void fdx_halt();

// Extensões
void fdx_subtraction(uint8_t);

#endif /*INSTRUCTIONS_H_*/

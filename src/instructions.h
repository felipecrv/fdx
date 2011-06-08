#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#define FDX_INSTRUCTION_SET_SIZE 16

void init_fdx_instruction_table();

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
void fdx_add_unsigned(uint8_t);
void fdx_subtraction_unsigned(uint8_t);
void fdx_load_immediate(uint8_t);

// Definindo tipos de ponteiro de funções para as instruções
typedef void (*two_byte_instruction_fn_t)(uint8_t);
typedef void (*one_byte_instruction_fn_t)(void);

struct fdx_instruction_table_entry {
    // Ponteiro para a função correspondente à instrução. Um cast para
    // two_byte_instruction_fn_t ou one_byte_instruction_fn_t é necessário antes
    // da chamada e depende do tamanho da instrução.
    void* fn;
    // Tamanho da instrução em bytes (1, ou 2)
    uint8_t length;
};

extern struct fdx_instruction_table_entry
    fdx_instruction_table[FDX_INSTRUCTION_SET_SIZE];

#endif /*INSTRUCTIONS_H_*/

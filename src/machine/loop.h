#ifndef LOOP_H_
#define LOOP_H_

extern void fdx_load_program(uint8_t *prog, int prog_len);
static void fdx_fetch_instruction();
static void fdx_execute_instruction();
static int fdx_instruction_length();
extern void fdx_execution_loop();

#endif /*LOOP_H_*/

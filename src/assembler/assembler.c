#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "assembler.h"
#include "hashmap.h"
#include "../machine/instructions.h"
#include "../machine/loop.h"

#define MNMNC(M, OPCODE) opcode_pool[opcode_pool_push_pos] = (OPCODE); \
    hashmap_set(&mnemonics, (M), \
        &opcode_pool[opcode_pool_push_pos++])

#define FATAL(MSG) assembler_error((MSG)); \
    exit(1);

#define EMIT_BYTE(B) if (ins_addr >= 255) {\
        FATAL("too many instructions")\
    }\
    output_prog[ins_addr++] = (B);

#define MAX_LABELS 512

static struct hashmap mnemonics;
static struct hashmap labels;
static uint8_t label_pool[MAX_LABELS], opcode_pool[2<<5];
static size_t label_pool_push_pos, opcode_pool_push_pos;

static char branches[MAX_LABELS][64];
static uint8_t branch_to_label_on_addr[256], branch_idx;

static FILE* assembler_input_file;
static char ch;
static int assembler_lineno, assembler_last_errno = 0;

static void assembler_error(const char *fmt, ...);
static void assembler_init(FILE* input_file);
static void assembler_cleanup();

static char asm_next(char);
static void asm_white();
static char* asm_mnemonic();
static char* asm_label();
static void asm_comment();
static uint8_t asm_address();


static void assembler_error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "error:%d: ", assembler_lineno);
    vfprintf(stderr, fmt, ap);
}

static void assembler_init(FILE* input_file)
{
    static int first_time = 1;

    label_pool_push_pos = 0;
    opcode_pool_push_pos = 0;
    branch_idx = 0;

    if (first_time) {
        init_fdx_instruction_table();

        hashmap_init(&mnemonics, 128);
        MNMNC("NOP", 0x00);
        MNMNC("STA", 0x01);
        MNMNC("LDA", 0x02);
        MNMNC("ADD", 0x03);
        MNMNC("OR",  0x04);
        MNMNC("AND", 0x05);
        MNMNC("NOT", 0x06);

        MNMNC("SUB", 0x07);

        MNMNC("JMP", 0x08);
        MNMNC("JN",  0x09);
        MNMNC("JZ",  0x0A);

        MNMNC("LDI", 0x0B);
        MNMNC("SHL", 0x0C);
        MNMNC("SHR", 0x0D);
        MNMNC("ADDU", 0x0E);
        MNMNC("SUBU", 0x0F);
        MNMNC("IN",  0x10);
        MNMNC("OUT", 0x11);
        MNMNC("MDMP", 0x12);


        MNMNC("HLT", 0x1F);

        first_time = 0;
    }

    hashmap_init(&labels, MAX_LABELS);

    // inits the parser input file and state
    assembler_input_file = input_file;
    ch = ' '; // initial parser char
    assembler_lineno = 1;
}

static void assembler_cleanup()
{
    hashmap_destroy(&labels);
}

int fdx_assemble(FILE *input_file, uint8_t *output_prog, int *prog_size)
{
    uint8_t *opcode, addr_arg, *branch_dest_addr, i;
    size_t ins_addr = 0;
    int code_has_error = 0;
    char *lbl;

    assembler_init(input_file);

    while (ch) {
        asm_white();
        if (ch && ch == ';') {
            asm_comment();
            if (ch) {
                asm_next('\n');
                if (assembler_last_errno) {
                    code_has_error = 1;
                }
                continue;
            }
        }

        if (isupper(ch)) {
            char *mnemonic = asm_mnemonic();
            if (!mnemonic) {
                code_has_error = 1;
                continue;
            }
            if ((opcode = (uint8_t *) hashmap_lookup(&mnemonics, mnemonic))) {
                // Adiciona a instrução ao binário
                EMIT_BYTE((*opcode) << 3)

                // Se a instrução tem apenas um byte o resto da linha tem
                // que estar vazio. Se for de 2 bytes continuamos lendo.
                if (fdx_instruction_table[*opcode].length == 2) {
                    int is_branch_inst = fdx_is_branch_instruction(*opcode);

                    asm_white();
                    // Se for branch aceite um label como argumento
                    if (is_branch_inst && islower(ch)) {
                        lbl = asm_label();
                        if (!lbl) {
                            code_has_error = 1;
                            continue;
                        }
                        strcpy(branches[branch_idx], lbl);
                        branch_to_label_on_addr[branch_idx] = ins_addr;
                        branch_idx++;
                        // Esse 0 será substituído pelo endereço
                        // correspondente ao label no final da compilação
                        EMIT_BYTE(0)
                    } else if (isdigit(ch)) {
                        addr_arg = asm_address();
                        EMIT_BYTE(addr_arg)
                    } else {
                        assembler_error("'%s' requires an argument", mnemonic);
                        code_has_error = 1;
                    }
                }
            } else {
                assembler_error("invalid instruction '%s'", mnemonic);
                code_has_error = 1;
            }
        } else if (islower(ch) || ch == '_') {
            lbl = asm_label();
            if (!lbl) {
                code_has_error = 1;
                continue;
            }
            asm_white();
            asm_next(':');
            if (assembler_last_errno) {
                code_has_error = 1;
                continue;
            } else {
                label_pool[label_pool_push_pos] = ins_addr;
                hashmap_set(
                        &labels,
                        lbl,
                        (void *) &label_pool[label_pool_push_pos]);
                ++label_pool_push_pos;
            }
        }

        asm_white();
        if (ch && ch == ';') {
            asm_comment();
        }
        if (ch) {
            asm_next('\n');
            if (assembler_last_errno) {
                code_has_error = 1;
            }
        }
    }

    // Resolvendo todos os branches
    if (!code_has_error) {
        for (i = 0; i < branch_idx; i++) {
            branch_dest_addr = (uint8_t *) hashmap_lookup(&labels, branches[i]);

            if (!branch_dest_addr) {
                assembler_error("reference to label '%s' not found", branches[i]); 
                code_has_error = 1;
            }
            output_prog[branch_to_label_on_addr[i]] = *branch_dest_addr;
        }

        *prog_size = ins_addr;
    }

    assembler_cleanup();
    return !code_has_error;
}

/**
 * Reads the next character and if c in no-zero expects the character to be c
 * code_has_error = 1;
 */
static char asm_next(char c)
{
    if (c && ch != c) {
        assembler_error("expected '%c', but found '%c'", c, ch);
        // recover by skiping the current line
        while (ch && ch != '\n') {
            asm_next(0);
        }
        if (!ch) {
            assembler_last_errno = 1;
            return ch;
        }
        char ret = asm_next(0);
        assembler_last_errno = 1;
        return ret;
    }
    assembler_last_errno = 0;
    ch = fgetc(assembler_input_file);
    if (ch == EOF) {
        return ch = '\0';
    }
    ch == '\n' && assembler_lineno++;
    return ch;
}

static void asm_white()
{
    while (ch && isblank(ch)) {
        asm_next(0);
    }
}

static char* asm_mnemonic()
{
    char *str = (char *) malloc(16 * sizeof(char));
    int len = 0;
    while (ch && isupper(ch)) {
        if (len < 15) {
            str[len++] = ch;
        }
        asm_next(0);
    }
    if (!len) {
        assembler_last_errno = 1;
        assembler_error("expected a mnemonic");
        return NULL;
    }
    str[len] = '\0';
#ifdef DEBUG
    printf("--> mnem: %s\n", str);
#endif
    return str;
}

static char* asm_label()
{
    int len = 0, is_first = 1;
    char *lbl = (char *) malloc(64 * sizeof(char));

    while (ch && (islower(ch) || ch == '_' || (isdigit(ch) && !is_first))) {
        if (len < 63) {
            lbl[len++] = ch;
        }
        asm_next(0);
        is_first = 0;
    }
    if (!len) {
        assembler_last_errno = 2;
        assembler_error("expected a label");
        return NULL;
    }
    lbl[len] = '\0';
#ifdef DEBUG
    printf("--> label: %s\n", lbl);
#endif
    return lbl;
}

static void asm_comment()
{
    asm_next(';');
    while (ch && ch != '\n') {
        asm_next(0);
    }
}

static uint8_t asm_address() {
    uint8_t addr = 0, dig;
    while (ch && isdigit(ch)) {
        dig = ch - '0';
        addr = addr * 10 + dig;
        asm_next(0);
    }
#ifdef DEBUG
    printf("--> addr: %hu\n", addr);
#endif
    return addr;
}

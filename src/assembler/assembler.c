#include <stdlib.h>
#include <stdio.h>
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

#define FATAL(MSG) fprintf(stderr, "error: %zu: %s\n", line_number, (MSG)); \
    exit(1);

#define ADD_BYTE(B) if (ins_addr > 254) {\
        FATAL("Too many instructions")\
    }\
    output_prog[ins_addr++] = (B);

#define MAXLINESIZE 2048
#define MAX_LABELS 512

static char line[MAXLINESIZE];
static struct hashmap mnemonics, labels;

static uint8_t label_pool[MAX_LABELS], opcode_pool[2<<5];
size_t label_pool_push_pos, opcode_pool_push_pos;

char branches[MAX_LABELS][64];
uint8_t branch_to_label_on_addr[256], branch_idx;

static void remove_comments(char *line, size_t *len)
{
    size_t i;
    for (i = 0; i < *len; i++) {
        if (line[i] == ';') {
            *len = i;
            line[i] = '\0';
            break;
        }
    }
}

static void trim(char *s, size_t *l)
{
    char *p = s;

    while (isspace(p[*l - 1])) p[--(*l)] = 0;
    while (*p && isspace(*p)) ++p, --(*l);
    memmove(s, p, *l + 1);
}

static void assembler_init(void)
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
}

static void assembler_cleanup()
{
    hashmap_destroy(&labels);
}

static void assembler_parse_argument(
        const char *text,
        char *label,
        uint8_t *addr_arg,
        char **error_message)
{
    static char expected_addr[] = "Address expected";
    static char expected_addr_or_label[] = "Label or address expected";

    char *err = NULL;

    if (addr_arg) {
        if (sscanf(text, "%hhu", addr_arg) < 1) {
            err = expected_addr;
        }
    }

    if (err && label) {
        if (sscanf(text, "%64s", label) < 1) {
            label[0] = '\0';
            err = expected_addr_or_label;
        } else {
            err = NULL;
        }
    }

    *error_message = err;
}


void fdx_assemble(FILE *input_file, uint8_t *output_prog, int *prog_size)
{
    // Para cada linha line:
    //    Remova os comentários de line
    //
    //    Lê o primeiro token
    //    Se for token for um label:
    //       registre-o
    //    Senão:
    //       Se é uma instrução válida:
    //            Adicione o opcode ao programa
    //            Incremente o endereço de instrução
    //
    //            Se é uma instrução de um byte:
    //                Se há algo na linha dispare um erro fatal
    //
    //            Senão:
    //                Leia o argumento (endereço ou label para instruções de
    //                branch)
    //                Adicione o argumento ao programa
    //                Incremente o endereço de instrução
    //
    //            Se a instrução for um branch espere por uma label ou endereço
    //
    // Resolve todas as intruções de branch
    // Retona o programa

    char *read_status, token[64], label[64], *label_ptr;
    char *error_message;
    size_t len, line_number = 1, ins_addr = 0;
    uint8_t *opcode, arg_addr, *branch_dest_addr, i;

    assembler_init();

    for (;; ++line_number) {
        // Lê a linha
        read_status = fgets(line, MAXLINESIZE, input_file);
        if (!read_status)
            break;

        len = strlen(line);

        // Remove os comentários
        remove_comments(line, &len);

        // Remove os espaços
        trim((char *) line, &len);

        if (len) {
            sscanf(line, "%64s", token);
            // Eh um label?
            if (token[strlen(token) - 1] == ':') {
                // Registra o label como apontador para o endereço atual
                token[strlen(token) - 1] = '\0';
                label_pool[label_pool_push_pos] = ins_addr;
                hashmap_set(
                        &labels,
                        token,
                        (void *) &label_pool[label_pool_push_pos]);
                ++label_pool_push_pos;
            } else {
                // Eh uma uma instrução válida?
                if ((opcode = (uint8_t *) hashmap_lookup(&mnemonics, token))) {
                    // Adiciona a instrução ao binário
                    ADD_BYTE((*opcode) << 3)

                    if (fdx_instruction_table[*opcode].length == 1) {
                        // Se a instrução tem apenas um byte o resto da linha tem
                        // que estar vazio
                        if (line[strlen(token)] != '\0') {
                            FATAL("End of line expected")
                        }
                    } else {
                        // A instrução é de dois bytes

                        // Se for branch aceite um label como argumento
                        label_ptr = fdx_is_branch_instruction(*opcode) ? label : NULL;
                        // Leia o argumento
                        assembler_parse_argument(
                                &line[strlen(token)],
                                label_ptr,
                                &arg_addr,
                                &error_message);

                        if (error_message) {
                            FATAL(error_message)
                        }

                        // Leu um label?
                        if (label_ptr && label_ptr[0]) {
                            strcpy(branches[branch_idx], label_ptr);
                            branch_to_label_on_addr[branch_idx] = ins_addr;
                            branch_idx++;
                            // Esse 0 será substituído pelo endereço
                            // correspondente ao label no final da compilação
                            ADD_BYTE(0)
                        } else {
                            ADD_BYTE(arg_addr)
                        }
                    }
                } else {
                    FATAL("Invalid instruction")
                }
            }
        }
    }

    // Resolvendo todos os branches
    for (i = 0; i < branch_idx; i++) {
        branch_dest_addr = (uint8_t *) hashmap_lookup(&labels, branches[i]);

        if (!branch_dest_addr) {
            fprintf(stderr, "Label not found: %s\n", branches[i]);
            exit(1);
        }
        output_prog[branch_to_label_on_addr[i]] = *branch_dest_addr;
    }

    *prog_size = ins_addr;

    assembler_cleanup();
}

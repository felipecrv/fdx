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

#define FATAL(MSG) printf("error: %zu: %s\n", line_number, (MSG)); \
    exit(1);

#define MAXLINESIZE 2048
#define MAX_LABELS 512

static char line[MAXLINESIZE];
static struct hashmap mnemonics, labels;

static uint8_t label_pool[MAX_LABELS], opcode_pool[2<<5];
size_t label_pool_push_pos, opcode_pool_push_pos;

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
        MNMNC("JN", 0x09);
        MNMNC("JZ",  0x0A);

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

    if (addr_arg) {
        if (sscanf(text, "%hhu", addr_arg) < 1) {
            *error_message = expected_addr;
            return;
        }
    }

    if (label) {
        if (sscanf(text, "%64s", label) <= 0) {
            *error_message = expected_addr_or_label;
            return;
        }
    }

    *error_message = NULL;
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
    uint8_t *opcode, arg_addr;

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
                    output_prog[ins_addr++] = (*opcode) << 3;

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

                        // TODO: tratar labels


                        // Adiciona o argumento ao programa
                        output_prog[ins_addr++] = arg_addr;
                    }
                } else {
                    FATAL("Invalid instruction")
                }
            }
        }
    }

    *prog_size = ins_addr;

    assembler_cleanup();
}

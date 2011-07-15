FILES=src/machine/machine.c src/machine/loop.c src/machine/instructions.c \
src/assembler/assembler.c \
src/fdx.c src/fdx-asm.c \
src/assembler/hashmap.c \
src/tests/tests.c

OBJS=machine.o loop.o instructions.o fdx.o

FDX_ASM_OBJS=machine.o loop.o instructions.o assembler.o fdx-asm.o hashmap.o

TEST_OBJS=tests.o hashmap.o

CC_ARGS=-g -Wall -std=c99 -pedantic

all:
	gcc -c $(CC_ARGS) $(FILES)
	gcc $(CC_ARGS) $(OBJS) -o fdx
	gcc $(CC_ARGS) $(FDX_ASM_OBJS) -o fdx-asm

clean:
	rm $(OBJS) $(FDX_ASM_OBJS)
	rm fdx
	rm fdx-asm
	rm tests

test:
	gcc -c $(CC_ARGS) -Wall $(FILES)
	gcc $(TEST_OBJS) -o tests && ./tests

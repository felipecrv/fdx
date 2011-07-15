FILES=src/machine/machine.c src/machine/loop.c src/machine/instructions.c \
src/assembler/assembler.c \
src/fdx.c src/fdx-asm.c \
src/assembler/hashmap.c \
src/tests/tests.c

OBJS=machine.o loop.o instructions.o fdx.o

FDX_ASM_OBJS=machine.o loop.o instructions.o assembler.o fdx-asm.o hashmap.o

TEST_OBJS=tests.o hashmap.o

all:
	gcc -c -g $(FILES)
	gcc $(OBJS) -o fdx
	gcc -g $(FDX_ASM_OBJS) -o fdx-asm

clean:
	rm $(OBJS)
	rm $(FDX_ASM_OBJS) 
	rm fdx
	rm fdx-asm
	rm tests

test:
	gcc -c -g $(FILES)
	gcc $(TEST_OBJS) -o tests && ./tests

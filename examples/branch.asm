; Esse exemplo é baseado no simple.asm, mas aqui o programa apenas carrega o
; 16 no registrador, pois um jump ocorre logo após o carregamento.

LDA 0  ; Coloca 16 ("opcode(LDA) << 3" que está na pos. 0) no AC.
       ; LDI 16 poderia ser usado para carregar o valor 16 no AC
JMP 8  ; Vai direto para o OUT. Note que 8 é o endereço em que a
       ; Instrução OUT está. Veja o labels.asm para não precisar contar bytes
       ; ao usar instruções de branch.
STA 15
ADD 15
OUT
HLT

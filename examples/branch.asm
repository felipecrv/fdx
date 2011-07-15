; Esse exemplo é baseado no simple.asm, mas aqui o programa apenas carrega o 16
; no registrador.

LDA 0  ; Coloca 16 ("opcode(LDA) << 3" que está na pos. 0) no AC
JMP 8  ; Vai direto para o HLT
STA 15 ; Coloca 16 na posição 15
ADD 15 ; AC = AC + mem(15) (ou seja, AC será 16 + 16)
HLT

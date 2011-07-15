LDA 0  ; Coloca 16 ("opcode(LDA) << 3" que está na pos. 0) no AC
STA 15 ; Coloca 16 na posição 15
ADD 15 ; AC = AC + mem(15) (ou seja, AC será 16 + 16)
HLT

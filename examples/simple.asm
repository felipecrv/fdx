LDA 0  ; Coloca 16 ("opcode(LDA) << 3" que está na pos. 0) no AC.
       ; LDI 16 poderia ser usado para carregar o valor 16 no AC
OUT    ; 16

STA 15 ; Coloca 16 na posição 15
OUT    ; 16

ADD 15 ; AC = AC + mem[15] (ou seja, AC será 16 + 16)
OUT    ; 32
HLT

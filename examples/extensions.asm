; Testando as extensões

; Grava o 2 no 128
LDI 2
STA 128

LDI 1
SHL 128 ; AC = 1 << 2 = 4
OUT

STA 129 ; Grava 4 no 129
LDI 1
STA 254
LDA 129
SHR 254 ; AC = 4 >> 1 = 2
OUT

ADDU 129 ; AC = 4 + 2
SUBU 254 ; AC = AC - 1 = 5
OUT
STA 130 ; 130 recebe 3

; 131 recebe entrada do usuário
IN
STA 131
LDA 131
OUT

MDMP
HLT

; Saída esperada:
; OUT: 4
; OUT: 2
; OUT: 5
; IN: 8
; OUT: 8
; ...


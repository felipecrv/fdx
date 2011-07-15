; Programa recebe 2 numeros como entrada e diz se o primeiro
; número é divisível pelo segundo

IN
STA 128
IN
STA 129

MDMP

; $128 divide por $129?
loop:
 ; $128 = $128 - $129
 LDA 128
 SUB 129
 STA 128
 ; Se o resultado da subtração é negativo é por que não divide
 JN nao_divide
 ; Se o resultado da subtração é zero é por que divide
 JZ divide
 ; Continua o loop até uma das condições serem satisfeitas
 JMP loop

nao_divide:
 LDI 0
 JMP end

divide:
 LDI 1

; Imprime resultado e termina
end:
 OUT
 MDMP
 HLT

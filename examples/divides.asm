; Programa recebe 2 números como entrada e diz se o primeiro
; número é divisível pelo segundo.

; Carrega o primeiro número em mem[128]
IN
STA 128
IN
STA 129

; Memory Dump
MDMP

; mem[128] divide por mem[129]?
loop:
 ; mem[128] = mem[128] - mem[129]
 LDA 128
 SUB 129
 STA 128
 ; Se o resultado da subtração é negativo é por que não divide
 JN nao_divide
 ; Se o resultado da subtração é zero é por que divide
 JZ divide
 ; Continua o loop até uma das condições acima serem satisfeitas
 JMP loop

 nao_divide:
  LDI 0
  JMP end

 divide:
  LDI 1
  ; Note que o "JMP end" não é necessário aqui, pois o label end vem logo após

; Imprime resultado e termina
end:
 OUT
 MDMP
 HLT

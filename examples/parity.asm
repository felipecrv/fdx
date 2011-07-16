; Retorna 1 se o número de bits ligados na representação\
; binária do número for par ou 0 caso contrário.

; Posição das variáveis na memória
; 128:   n: o número que tem a paridade sendo calculada
; 129:   1: constante 1 (usada como máscara)
; 130: bit: variável temporária -- armazena o bit extraído
; 131: qtd: quantidade de bits em n

; Lê o número n
IN
STA 128

; Guarda a constante 1 na posição 129
LDI 1
STA 129

; Inicializa qtd com 0
LDI 0
STA 131

loop:
 ; Extrai o bit menos significativo
 LDA 128 ; Carrega n
 AND 129 ; Carrega o bit menos significativo no acumulador AC: AC = AC & 1
 STA 130 ; Poe o bit em mem[130]

 ; Soma o bit(0 ou 1) à quantidade de bits
 LDA 131
 ADDU 130
 STA 131

 ; Remove o bit menos significativo de n fazendo um Shift Right
 LDA 128
 SHR 129
 STA 128 ; Grava o novo valor de n

 JZ end   ; Vai para o final se n == 0
 JMP loop ; Continua o loop até a condição anterior ser favorecida

end:
 ; A paridade é o inverso do bit menos significativo de qtd
 LDA 131 ; Carrega qtd
 NOT     ; Nega todos os bits de qtd
 AND 129 ; Obtém o bit menos significativo
 OUT
 HLT


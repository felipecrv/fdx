; Paridade

; Posição das variáveis na memória
; 128: o número que tem a paridade sendo calculada -- n
; 129: constante 1 (usada como máscara)
; 130: variável temporária -- armazena o bit extraído
; 131: quantidade de bits em n

; Lê o número n
IN
STA 128

; Guarda a constante 1 na posição 129
LDI 1
STA 129

; Inicializa 131 com 0
LDI 0
STA 131

loop:
 ; Extrai o bit menos significativo
 LDA 128 ; Traz n
 AND 129 ; Carrega o bit menos significativo no acumulador AC: AC = AC & 1
 STA 130 ; Poe o bit em 130

 ; Soma o bit à quantidade de bits
 LDA 131
 ADDU 130
 STA 131

 ; Remove o bit menos significativo fazendo um Shift Right
 LDA 128
 SHR 129
 STA 128

 JZ end ; Vai para o final se 128 for 0
 JMP loop ; Continua o loop até a condição anterior ser favorecida

end:
 ; A paridade é o bit menos significativo da posição 131 negado
 LDA 131 ; Carrega 131
 NOT     ; Nega todos os bits em 131
 AND 129 ; Pega o bit menos significativo
 OUT
 HLT


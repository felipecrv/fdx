; Esse exemplo tem vários branches: a execução faz ium zig-zag

LDA 1 ; Coloca 1 en AC (note que 1 é o endereço, mas como 1 está na posição 1...)

JMP lbl_z

; 2nd
lbl_a:
ADD 1 ; Incrementa em 1
JMP lbl_x

; 4th
lbl_b:
ADD 1
JMP end ; termina

; 3rd
lbl_x:
ADD 1
JMP lbl_b

; 1st
lbl_z:
ADD 1
JMP lbl_a

end:
; Dobra AC (que é 5, pois houveram 4 incrementos a partir de 1)
STA 128 ; Salva AC na pos 128
ADD 128
HLT

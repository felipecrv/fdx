; Esse exemplo tem vários branches: a execução faz um "zig-zag".

LDI 1 ; Coloca 1 em AC
OUT

; Primeiro branch
JMP lbl_z

; 2nd
lbl_a:
 ADD 1 ; Incrementa em 1
 OUT
 JMP lbl_x

; 4th
lbl_b:
 ADD 1
 OUT
 JMP end ; Termina

; 3rd
lbl_x:
 ADD 1
 OUT
 JMP lbl_b

; 1st
lbl_z:
 ADD 1
 OUT
 JMP lbl_a

end:
 STA 128 ; Salva AC na pos 128
 ADD 128 ; Dobra AC (que é 5, pois houveram 4 incrementos a partir de 1)
 OUT
 MDMP ; Memory dump
 HLT

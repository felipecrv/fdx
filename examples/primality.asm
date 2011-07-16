
; Variáveis:
;
; 128:        n: entrada (somente leitura)
; 129:  n_local: usado no divides
; 130:        i: iterador (usado como pâmetro p/ divides)
; 131:
; 132:      tmp: = n - i ; Usado para testar igualdade de i e n
; 133:        1: constante 1
; 134: eh_primo: se n é primo ou não

; Lê n
IN
STA 128

; mem[133] = 1 (constante)
LDI 1
STA 133
; Se dividir por algum número durante o loop é porque não é primo.
STA 134

; Para i = 2 até n - 1 teste se n divide por i
; i = 1
LDI 1
STA 130
main_loop:
 ; i++
 LDA 130
 ADDU 133
 STA 130

 ; Se i == n termine e imprima o resultado
 LDA 128  ; Carrega n
 SUBU 130 ; AC = n - i
 JZ end   ; Termina se zero, ou seja n - i == 0 => n == i

 ; Testa se n divide por i chamando divides
 JMP divides
 after_divides:
  ; O resultado de divides está em AC
  ; Se n divide por i(AC == 1), então n não é primo
  SUB 133 ; AC = AC - 1
  JZ end_main_loop_nao_eh_primo
  ; Caso contrário continua o loop
  JMP main_loop

  end_main_loop_nao_eh_primo:
  ; Poe 0 em resultado
  LDI 0
  STA 134
  JMP end
; Fim do main_loop

; AC = (n divide por i)?
divides:
 ; Copia n para n_local, pois n é alterado durante o loop
 LDA 128
 STA 129
 divides_loop:
  ; mem[129] = mem[129] - mem[130]
  LDA 129
  SUB 130
  STA 129
  ; Se o resultado da subtração é negativo é por que não divide
  JN nao_divide
  ; Se o resultado da subtração é zero é por que divide
  JZ divide
  ; Continua o loop até uma das condições serem satisfeitas
  JMP divides_loop

 nao_divide:
  LDI 0
  JMP after_divides

 divide:
  LDI 1
  JMP after_divides
; Fim de divides

end:
 ; Carrega resultado e imprime
 LDA 134
 OUT
 HLT

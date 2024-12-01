; Autor reseni: Dmitrii Ivanushkin xivanu00

; Projekt 2 - INP 2024
; Vigenerova sifra na architekture MIPS64

; DATA SEGMENT
                .data
msg:            .asciiz "dmitriiivanushkin"
msg_index:      .word   0     ; index symbolu v zprave
shift_i:        .word   9     ; i = 9
shift_v:        .word   22    ; v = 22
shift_a:        .word   1     ; a = 1
rem_0:          .word   0     ; zbytek deleni = 0
rem_1:          .word   1     ; zbytek deleni = 0
rem_2:          .word   2     ; zbytek deleni = 0
modulo_key:     .word   3     ; delka klice
modulo_par:     .word   2     ; %2
cipher:         .space  31    ; max delka cipher
lower_a:        .word   97    ; ASCII lower a
alphabet_size:  .word   26    ; delka abecedy
params_sys5:    .space  8     ; misto pro ulozeni adresy pocatku
                              ; retezce pro vypis pomoci syscall 5
                              ; (viz nize "funkce" print_string)

; CODE SEGMENT
                .text
main:
                daddi   r1, r0, msg           ; adresa msg do r1
                daddi   r2, r0, msg_index     ; adresa msg_index do r2
                daddi   r3, r0, shift_i       ; adresa shift_i do r3
                daddi   r4, r0, shift_v       ; adresa shift_v do r4
                daddi   r5, r0, shift_a       ; adresa shift_a do r5
                daddi   r6, r0, rem_0         ; adresa rem_0 do r6
                daddi   r7, r0, rem_1         ; adresa rem_1 do r7
                daddi   r8, r0, rem_2         ; adresa rem_2 do r8
                daddi   r9, r0, modulo_key    ; adresa modulo_key do r9
                daddi   r10, r0, modulo_par   ; adresa modulo_par do r10
                daddi   r11, r0, cipher       ; adresa cipher do r11
                daddi   r12, r0, lower_a      ; adresa lower_a do r12
                daddi   r14, r0, alphabet_size; adresa alphabet_size do r14
                sw      r0, 0(r2)             ; msg_index = 0

loop:
                lw      r15, 0(r2)            ; index do r15
                add     r16, r1, r15          ; msg[msg_index]
                lb      r17, 0(r16)           ; obsah msg[msg_index]
                beqz    r17, end              ; \0
                ddivu   r15, r9               ; msg[msg_index] / 3
                mfhi    r18                   ; zbytek deleni z hi do r18
                lw      r19, 0(r6)
                beq     r18, r19, use_shift_i ; i pro 0
                lw      r19, 0(r7)
                beq     r18, r19, use_shift_v ; v pro 1
                lw      r19, 0(r8)
                beq     r18, r19, use_shift_a ; a pro 2

use_shift_i:
                lw      r30, 0(r3)            ; uloz posuv do r30
                b       check_parity

use_shift_v:
                lw      r30, 0(r4)            ; uloz posuv do r30
                b       check_parity

use_shift_a:
                lw      r30, 0(r5)            ; uloz posuv do r30
                b       check_parity

check_parity:
                lw      r20, 0(r10)           ; modulo_par do r20
                ddivu   r15, r20              ; msg_index / modulo_par
                mfhi    r21                   ; zbytek deleni z hi do r21
                beqz    r21, add_shift        ; %2 = 0 -> +
                b       sub_shift             ; %2 = 1 -> -

add_shift:
                lw      r22, 0(r12)           ; ASCII(a) do r22
                sub     r17, r17, r22         ; ASCII -> abeceda
                add     r17, r17, r30         ; +
                b       finalize_shift

sub_shift:
                lw      r22, 0(r12)           ; ASCII(a) do r22
                sub     r17, r17, r22         ; ASCII -> abeceda
                sub     r17, r17, r30         ; -
                b       finalize_shift

finalize_shift:
                lw      r24, 0(r14)           ; alphabet_size do r24
                divu    r17, r24              ; msg[msg_index] / alphabet_size
                mfhi    r17                   ; zbytek deleni z hi do r17
                add     r17, r17, r22         ; abeceda -> ASCII
                sb      r17, 0(r11)           ; uloz vysledek do r11 (cipher)
                addi    r11, r11, 1           ; inkrementuj index cipher
                b       increment_index

increment_index:
                addi    r15, r15, 1           ; msg_index += 1
                sw      r15, 0(r2)            ; uloz zpet do r2
                b       loop
end:
                sb      r0, 0(r11)            ; \0
                daddi   r4, r0, cipher        ; cipher do r4 pro volani print_string
                jal     print_string          ; vypis pomoci print_string - viz nize


; NASLEDUJICI KOD NEMODIFIKUJTE!

                syscall 0   ; halt

print_string:   ; adresa retezce se ocekava v r4
                sw      r4, params_sys5(r0)
                daddi   r14, r0, params_sys5    ; adr pro syscall 5 musi do r14
                syscall 5   ; systemova procedura - vypis retezce na terminal
                jr      r31 ; return - r31 je urcen na return address


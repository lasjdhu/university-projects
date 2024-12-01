; Autor reseni: Dmitrii Ivanushkin xivanu00

; Projekt 2 - INP 2024
; Vigenerova sifra na architekture MIPS64

; DATA SEGMENT
                .data
msg:            .asciiz "dmitriiivanushkin"
msg_index:      .word   0
shift_i:        .word   9
shift_v:        .word   22
shift_a:        .word   1
rem_0:          .word   0
rem_1:          .word   1
rem_2:          .word   2
modulo_key:     .word   3
modulo_par:     .word   2
cipher:         .space  31
lower_a:        .word   97
lower_z:        .word   122
alphabet_size:  .word   26
params_sys5:    .space  8

; CODE SEGMENT
                .text
main:
                daddi   r4, r0, msg
                daddi   r5, r0, msg_index
                daddi   r6, r0, shift_i
                daddi   r7, r0, shift_v
                daddi   r8, r0, shift_a
                daddi   r9, r0, rem_0
                daddi   r10, r0, rem_1
                daddi   r11, r0, rem_2
                daddi   r12, r0, modulo_key
                daddi   r13, r0, modulo_par
                daddi   r14, r0, cipher
                daddi   r15, r0, lower_a
                daddi   r16, r0, lower_z
                daddi   r17, r0, alphabet_size
                sw      r0, 0(r5)

loop:
                lw      r18, 0(r5)
                add     r19, r4, r18
                lb      r20, 0(r19)
                beqz    r20, end
                ddivu   r18, r12
                mfhi    r21

                lw      r22, 0(r9)
                beq     r21, r22, use_shift_i

                lw      r22, 0(r10)
                beq     r21, r22, use_shift_v

                lw      r22, 0(r11)
                beq     r21, r22, use_shift_a

use_shift_i:
                lw      r22, 0(r6)
                j       apply_shift

use_shift_v:
                lw      r22, 0(r7)
                j       apply_shift

use_shift_a:
                lw      r22, 0(r8)
                j       apply_shift

apply_shift:
                lw      r26, 0(r13)
                ddivu   r18, r26
                mfhi    r27
                beqz    r27, add_shift
                sub     r22, r0, r22

add_shift:
                lw      r23, 0(r15)
                lw      r24, 0(r16)
                sub     r20, r20, r23
                add     r20, r20, r22
                lw      r28, 0(r17)
                divu    r20, r28
                mfhi    r20
                add     r20, r20, r23
                sb      r20, 0(r14)
                addi    r14, r14, 1


increment_index:
                lw      r18, 0(r5)
                addi    r18, r18, 1
                sw      r18, 0(r5)
                j       loop

end:
                sb      r0, 0(r14)
                daddi   r4, r0, cipher
                jal     print_string
                syscall 0

print_string:   ; adresa retezce se ocekava v r4
                sw      r4, params_sys5(r0)
                daddi   r14, r0, params_sys5    ; adr pro syscall 5 musi do r14
                syscall 5   ; systemova procedura - vypis retezce na terminal
                jr      r31 ; return - r31 je urcen na return address

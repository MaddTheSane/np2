
; r4 - tmp
; r7 - clock
; r8 - IP/flag
; r9 - i286core/mem
; r10 - iflag
; r11 - ret


	INCLUDE	i286a.inc
	IMPORT	i286a_memoryread
	IMPORT	i286a_memoryread_w

	EXPORT	i286a_ea
	EXPORT	i286a_lea
	EXPORT	i286a_a

	AREA	.text, CODE, READONLY

; ---- calc_ea_dst

i286a_ea		and		r1, r0, #(&18 << 3)
				and		r2, r0, #7
				add		r3, pc, r1 lsr #1
				add		pc, r3, r2 lsl #2

				b		ea_bx_si
				b		ea_bx_di
				b		ea_bp_si
				b		ea_bp_di
				b		ea_si
				b		ea_di
				b		ea_d16
				b		ea_bx

				b		ea_bx_si_d8
				b		ea_bx_di_d8
				b		ea_bp_si_d8
				b		ea_bp_di_d8
				b		ea_si_d8
				b		ea_di_d8
				b		ea_bp_d8
				b		ea_bx_d8

				b		ea_bx_si_d16
				b		ea_bx_di_d16
				b		ea_bp_si_d16
				b		ea_bp_di_d16
				b		ea_si_d16
				b		ea_di_d16
				b		ea_bp_d16
				b		ea_bx_d16

ea_bx_si		ldrh	r1, [r9, #CPU_BX]
				ldrh	r2, [r9, #CPU_SI]
				ldr		r0, [r9, #CPU_DS_FIX]
				add		r3, r1, r2
				bic		r12, r3, #(1 << 16)
				add		r0, r12, r0
				mov		pc, lr

ea_bx_si_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BX]
				ldrh	r2, [r9, #CPU_SI]
				ldr		r3, [r9, #CPU_DS_FIX]
				add		r1, r1, r2
				mov		r12, r0 lsl #24
				add		r8, r8, #(1 << 16)
				add		r2, r1, r12 asr #24
				mov		r1, r2 lsl #16
				add		r0, r3, r1 lsr #16
				mov		pc, r4

ea_bx_si_d16	mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BX]
				ldrh	r2, [r9, #CPU_SI]
				ldr		r3, [r9, #CPU_DS_FIX]
				add		r12, r1, r2
				add		r8, r8, #(2 << 16)
				add		r2, r12, r0
				bic		r1, r2, #(3 << 16)
				add		r0, r1, r3
				mov		pc, r4

ea_bx_di		ldrh	r1, [r9, #CPU_BX]
				ldrh	r2, [r9, #CPU_DI]
				ldr		r0, [r9, #CPU_DS_FIX]
				add		r3, r1, r2
				bic		r12, r3, #(1 << 16)
				add		r0, r12, r0
				mov		pc, lr

ea_bx_di_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BX]
				ldrh	r2, [r9, #CPU_DI]
				ldr		r3, [r9, #CPU_DS_FIX]
				add		r1, r1, r2
				mov		r12, r0 lsl #24
				add		r8, r8, #(1 << 16)
				add		r2, r1, r12 asr #24
				mov		r1, r2 lsl #16
				add		r0, r3, r1 lsr #16
				mov		pc, r4

ea_bx_di_d16	mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BX]
				ldrh	r2, [r9, #CPU_DI]
				ldr		r3, [r9, #CPU_DS_FIX]
				add		r12, r1, r2
				add		r8, r8, #(2 << 16)
				add		r2, r12, r0
				bic		r1, r2, #(3 << 16)
				add		r0, r1, r3
				mov		pc, r4

ea_bp_si		ldrh	r1, [r9, #CPU_BP]
				ldrh	r2, [r9, #CPU_SI]
				ldr		r0, [r9, #CPU_SS_FIX]
				add		r3, r1, r2
				bic		r12, r3, #(1 << 16)
				add		r0, r12, r0
				mov		pc, lr

ea_bp_si_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BP]
				ldrh	r2, [r9, #CPU_SI]
				ldr		r3, [r9, #CPU_SS_FIX]
				add		r1, r1, r2
				mov		r12, r0 lsl #24
				add		r8, r8, #(1 << 16)
				add		r2, r1, r12 asr #24
				mov		r1, r2 lsl #16
				add		r0, r3, r1 lsr #16
				mov		pc, r4

ea_bp_si_d16	mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BP]
				ldrh	r2, [r9, #CPU_SI]
				ldr		r3, [r9, #CPU_SS_FIX]
				add		r12, r1, r2
				add		r8, r8, #(2 << 16)
				add		r2, r12, r0
				bic		r1, r2, #(3 << 16)
				add		r0, r1, r3
				mov		pc, r4

ea_bp_di		ldrh	r1, [r9, #CPU_BP]
				ldrh	r2, [r9, #CPU_DI]
				ldr		r0, [r9, #CPU_SS_FIX]
				add		r3, r1, r2
				bic		r12, r3, #(1 << 16)
				add		r0, r12, r0
				mov		pc, lr

ea_bp_di_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BP]
				ldrh	r2, [r9, #CPU_DI]
				ldr		r3, [r9, #CPU_SS_FIX]
				add		r1, r1, r2
				mov		r12, r0 lsl #24
				add		r8, r8, #(1 << 16)
				add		r2, r1, r12 asr #24
				mov		r1, r2 lsl #16
				add		r0, r3, r1 lsr #16
				mov		pc, r4

ea_bp_di_d16	mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BP]
				ldrh	r2, [r9, #CPU_DI]
				ldr		r3, [r9, #CPU_SS_FIX]
				add		r12, r1, r2
				add		r8, r8, #(2 << 16)
				add		r2, r12, r0
				bic		r1, r2, #(3 << 16)
				add		r0, r1, r3
				mov		pc, r4

ea_si			ldrh	r1, [r9, #CPU_SI]
				ldr		r2, [r9, #CPU_DS_FIX]
				add		r0, r1, r2
				mov		pc, lr

ea_si_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_SI]
				ldr		r2, [r9, #CPU_DS_FIX]
				mov		r3, r0 lsl #24
				mov		r12, r1 lsl #16
				add		r8, r8, #(1 << 16)
				add		r1, r12, r3, asr #8
				add		r0, r2, r1 lsr #16
				mov		pc, r4

ea_si_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_SI]
				ldr		r2, [r9, #CPU_DS_FIX]
				add		r8, r8, #(2 << 16)
				add		r3, r0, r1
				bic		r1, r3, #(1 << 16)
				add		r0, r1, r2
				mov		pc, r4

ea_di			ldrh	r1, [r9, #CPU_DI]
				ldr		r2, [r9, #CPU_DS_FIX]
				add		r0, r1, r2
				mov		pc, lr

ea_di_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_DI]
				ldr		r2, [r9, #CPU_DS_FIX]
				mov		r3, r0 lsl #24
				mov		r12, r1 lsl #16
				add		r8, r8, #(1 << 16)
				add		r1, r12, r3, asr #8
				add		r0, r2, r1 lsr #16
				mov		pc, r4

ea_di_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_DI]
				ldr		r2, [r9, #CPU_DS_FIX]
				add		r8, r8, #(2 << 16)
				add		r3, r0, r1
				bic		r1, r3, #(1 << 16)
				add		r0, r1, r2
				mov		pc, r4

ea_bx			ldrh	r1, [r9, #CPU_BX]
				ldr		r2, [r9, #CPU_DS_FIX]
				add		r0, r1, r2
				mov		pc, lr

ea_bx_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BX]
				ldr		r2, [r9, #CPU_DS_FIX]
				mov		r3, r0 lsl #24
				mov		r12, r1 lsl #16
				add		r8, r8, #(1 << 16)
				add		r1, r12, r3, asr #8
				add		r0, r2, r1 lsr #16
				mov		pc, r4

ea_bx_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BX]
				ldr		r2, [r9, #CPU_DS_FIX]
				add		r8, r8, #(2 << 16)
				add		r3, r0, r1
				bic		r1, r3, #(1 << 16)
				add		r0, r1, r2
				mov		pc, r4

ea_d16			mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldr		r1, [r9, #CPU_DS_FIX]
				add		r8, r8, #(2 << 16)
				add		r0, r0, r1
				mov		pc, r4

ea_bp_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BP]
				ldr		r2, [r9, #CPU_SS_FIX]
				mov		r3, r0 lsl #24
				mov		r12, r1 lsl #16
				add		r8, r8, #(1 << 16)
				add		r1, r12, r3, asr #8
				add		r0, r2, r1 lsr #16
				mov		pc, r4

ea_bp_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BP]
				ldr		r2, [r9, #CPU_SS_FIX]
				add		r8, r8, #(2 << 16)
				add		r3, r0, r1
				bic		r1, r3, #(1 << 16)
				add		r0, r1, r2
				mov		pc, r4


; ---- calc_lea

i286a_lea		and		r1, r0, #(&18 << 3)
				and		r2, r0, #7
				add		r3, pc, r1 lsr #1
				add		pc, r3, r2 lsl #2

				b		lea_bx_si
				b		lea_bx_di
				b		lea_bp_si
				b		lea_bp_di
				b		lea_si
				b		lea_di
				b		lea_d16
				b		lea_bx

				b		lea_bx_si_d8
				b		lea_bx_di_d8
				b		lea_bp_si_d8
				b		lea_bp_di_d8
				b		lea_si_d8
				b		lea_di_d8
				b		lea_bp_d8
				b		lea_bx_d8

				b		lea_bx_si_d16
				b		lea_bx_di_d16
				b		lea_bp_si_d16
				b		lea_bp_di_d16
				b		lea_si_d16
				b		lea_di_d16
				b		lea_bp_d16
				b		lea_bx_d16

lea_bx_si		ldrh	r0, [r9, #CPU_BX]
				ldrh	r1, [r9, #CPU_SI]
				add		r2, r0, r1
				bic		r0, r2, #&10000
				mov		pc, lr

lea_bx_si_d8	mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BX]
				ldrh	r2, [r9, #CPU_SI]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r1, r2, r1
				add		r8, r8, #(1 << 16)
				add		r2, r0, r1
				bic		r0, r2, #(3 << 16)
				mov		pc, r4

lea_bx_si_d16	mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BX]
				ldrh	r2, [r9, #CPU_SI]
				add		r3, r0, r1
				add		r1, r3, r2
				add		r8, r8, #(2 << 16)
				bic		r0, r1, #(3 << 16)
				mov		pc, r4

lea_bx_di		ldrh	r0, [r9, #CPU_BX]
				ldrh	r1, [r9, #CPU_DI]
				add		r2, r1, r0
				bic		r0, r2, #(1 << 16)
				mov		pc, lr

lea_bx_di_d8	mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BX]
				ldrh	r2, [r9, #CPU_DI]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r1, r2, r1
				add		r8, r8, #(1 << 16)
				add		r2, r0, r1
				bic		r0, r2, #(3 << 16)
				mov		pc, r4

lea_bx_di_d16	mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BX]
				ldrh	r2, [r9, #CPU_DI]
				add		r3, r0, r1
				add		r1, r3, r2
				add		r8, r8, #(2 << 16)
				bic		r0, r1, #(3 << 16)
				mov		pc, r4

lea_bp_si		ldrh	r0, [r9, #CPU_BP]
				ldrh	r1, [r9, #CPU_SI]
				add		r2, r0, r1
				bic		r0, r2, #&10000
				mov		pc, lr

lea_bp_si_d8	mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BP]
				ldrh	r2, [r9, #CPU_SI]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r1, r2, r1
				add		r8, r8, #(1 << 16)
				add		r2, r0, r1
				bic		r0, r2, #(3 << 16)
				mov		pc, r4

lea_bp_si_d16	mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BP]
				ldrh	r2, [r9, #CPU_SI]
				add		r3, r0, r1
				add		r1, r3, r2
				add		r8, r8, #(2 << 16)
				bic		r0, r1, #(3 << 16)
				mov		pc, r4

lea_bp_di		ldrh	r0, [r9, #CPU_BP]
				ldrh	r1, [r9, #CPU_DI]
				add		r2, r1, r0
				bic		r0, r2, #(1 << 16)
				mov		pc, lr

lea_bp_di_d8	mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BP]
				ldrh	r2, [r9, #CPU_DI]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r1, r2, r1
				add		r8, r8, #(1 << 16)
				add		r2, r0, r1
				bic		r0, r2, #(3 << 16)
				mov		pc, r4

lea_bp_di_d16	mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BP]
				ldrh	r2, [r9, #CPU_DI]
				add		r3, r0, r1
				add		r1, r3, r2
				add		r8, r8, #(2 << 16)
				bic		r0, r1, #(3 << 16)
				mov		pc, r4

lea_si			ldrh	r0, [r9, #CPU_SI]
				mov		pc, lr

lea_si_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_SI]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r2, r0, r1
				add		r8, r8, #(1 << 16)
				bic		r0, r2, #(1 << 16)
				mov		pc, r4

lea_si_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_SI]
				add		r8, r8, #(2 << 16)
				add		r2, r1, r0
				bic		r0, r2, #(1 << 16)
				mov		pc, r4

lea_di			ldrh	r0, [r9, #CPU_DI]
				mov		pc, lr

lea_di_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_DI]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r2, r0, r1
				add		r8, r8, #(1 << 16)
				bic		r0, r2, #(1 << 16)
				mov		pc, r4

lea_di_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_DI]
				add		r8, r8, #(2 << 16)
				add		r2, r1, r0
				bic		r0, r2, #(1 << 16)
				mov		pc, r4

lea_bx			ldrh	r0, [r9, #CPU_BX]
				mov		pc, lr

lea_bx_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BX]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r2, r0, r1
				add		r8, r8, #(1 << 16)
				bic		r0, r2, #(1 << 16)
				mov		pc, r4

lea_bx_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BX]
				add		r8, r8, #(2 << 16)
				add		r2, r1, r0
				bic		r0, r2, #(1 << 16)
				mov		pc, r4

lea_d16			mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				add		r8, r8, #(2 << 16)
				mov		pc, r4

lea_bp_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BP]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r2, r0, r1
				add		r8, r8, #(1 << 16)
				bic		r0, r2, #(1 << 16)
				mov		pc, r4

lea_bp_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BP]
				add		r8, r8, #(2 << 16)
				add		r2, r1, r0
				bic		r0, r2, #(1 << 16)
				mov		pc, r4


; ---- calc_a

i286a_a			and		r1, r0, #(&18 << 3)
				and		r2, r0, #7
				add		r3, pc, r1 lsr #1
				add		pc, r3, r2 lsl #2

				b		a_bx_si
				b		a_bx_di
				b		a_bp_si
				b		a_bp_di
				b		a_si
				b		a_di
				b		a_d16
				b		a_bx

				b		a_bx_si_d8
				b		a_bx_di_d8
				b		a_bp_si_d8
				b		a_bp_di_d8
				b		a_si_d8
				b		a_di_d8
				b		a_bp_d8
				b		a_bx_d8

				b		a_bx_si_d16
				b		a_bx_di_d16
				b		a_bp_si_d16
				b		a_bp_di_d16
				b		a_si_d16
				b		a_di_d16
				b		a_bp_d16
				b		a_bx_d16

a_bx_si			ldrh	r0, [r9, #CPU_BX]
				ldrh	r1, [r9, #CPU_SI]
				ldr		r6, [r9, #CPU_DS_FIX]
				add		r2, r0, r1
				bic		r0, r2, #&10000
				mov		pc, lr

a_bx_si_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BX]
				ldrh	r2, [r9, #CPU_SI]
				ldr		r6, [r9, #CPU_DS_FIX]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r1, r2, r1
				add		r8, r8, #(1 << 16)
				add		r2, r0, r1
				bic		r0, r2, #(3 << 16)
				mov		pc, r4

a_bx_si_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BX]
				ldrh	r2, [r9, #CPU_SI]
				ldr		r6, [r9, #CPU_DS_FIX]
				add		r3, r0, r1
				add		r1, r3, r2
				add		r8, r8, #(2 << 16)
				bic		r0, r1, #(3 << 16)
				mov		pc, r4

a_bx_di			ldrh	r0, [r9, #CPU_BX]
				ldrh	r1, [r9, #CPU_DI]
				ldr		r6, [r9, #CPU_DS_FIX]
				add		r2, r1, r0
				bic		r0, r2, #(1 << 16)
				mov		pc, lr

a_bx_di_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BX]
				ldrh	r2, [r9, #CPU_DI]
				ldr		r6, [r9, #CPU_DS_FIX]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r1, r2, r1
				add		r8, r8, #(1 << 16)
				add		r2, r0, r1
				bic		r0, r2, #(3 << 16)
				mov		pc, r4

a_bx_di_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BX]
				ldrh	r2, [r9, #CPU_DI]
				ldr		r6, [r9, #CPU_DS_FIX]
				add		r3, r0, r1
				add		r1, r3, r2
				add		r8, r8, #(2 << 16)
				bic		r0, r1, #(3 << 16)
				mov		pc, r4

a_bp_si			ldrh	r0, [r9, #CPU_BP]
				ldrh	r1, [r9, #CPU_SI]
				ldr		r6, [r9, #CPU_SS_FIX]
				add		r2, r0, r1
				bic		r0, r2, #&10000
				mov		pc, lr

a_bp_si_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BP]
				ldrh	r2, [r9, #CPU_SI]
				ldr		r6, [r9, #CPU_SS_FIX]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r1, r2, r1
				add		r8, r8, #(1 << 16)
				add		r2, r0, r1
				bic		r0, r2, #(3 << 16)
				mov		pc, r4

a_bp_si_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BP]
				ldrh	r2, [r9, #CPU_SI]
				ldr		r6, [r9, #CPU_SS_FIX]
				add		r3, r0, r1
				add		r1, r3, r2
				add		r8, r8, #(2 << 16)
				bic		r0, r1, #(3 << 16)
				mov		pc, r4

a_bp_di			ldrh	r0, [r9, #CPU_BP]
				ldrh	r1, [r9, #CPU_DI]
				ldr		r6, [r9, #CPU_SS_FIX]
				add		r2, r1, r0
				bic		r0, r2, #(1 << 16)
				mov		pc, lr

a_bp_di_d8		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BP]
				ldrh	r2, [r9, #CPU_DI]
				ldr		r6, [r9, #CPU_SS_FIX]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r1, r2, r1
				add		r8, r8, #(1 << 16)
				add		r2, r0, r1
				bic		r0, r2, #(3 << 16)
				mov		pc, r4

a_bp_di_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BP]
				ldrh	r2, [r9, #CPU_DI]
				ldr		r6, [r9, #CPU_SS_FIX]
				add		r3, r0, r1
				add		r1, r3, r2
				add		r8, r8, #(2 << 16)
				bic		r0, r1, #(3 << 16)
				mov		pc, r4

a_si			ldrh	r0, [r9, #CPU_SI]
				ldr		r6, [r9, #CPU_DS_FIX]
				mov		pc, lr

a_si_d8			mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_SI]
				ldr		r6, [r9, #CPU_DS_FIX]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r2, r0, r1
				add		r8, r8, #(1 << 16)
				bic		r0, r2, #(1 << 16)
				mov		pc, r4

a_si_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_SI]
				ldr		r6, [r9, #CPU_DS_FIX]
				add		r8, r8, #(2 << 16)
				add		r2, r1, r0
				bic		r0, r2, #(1 << 16)
				mov		pc, r4

a_di			ldrh	r0, [r9, #CPU_DI]
				ldr		r6, [r9, #CPU_DS_FIX]
				mov		pc, lr

a_di_d8			mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_DI]
				ldr		r6, [r9, #CPU_DS_FIX]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r2, r0, r1
				add		r8, r8, #(1 << 16)
				bic		r0, r2, #(1 << 16)
				mov		pc, r4

a_di_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_DI]
				ldr		r6, [r9, #CPU_DS_FIX]
				add		r8, r8, #(2 << 16)
				add		r2, r1, r0
				bic		r0, r2, #(1 << 16)
				mov		pc, r4

a_bx			ldrh	r0, [r9, #CPU_BX]
				ldr		r6, [r9, #CPU_DS_FIX]
				mov		pc, lr

a_bx_d8			mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BX]
				ldr		r6, [r9, #CPU_DS_FIX]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r2, r0, r1
				add		r8, r8, #(1 << 16)
				bic		r0, r2, #(1 << 16)
				mov		pc, r4

a_bx_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BX]
				ldr		r6, [r9, #CPU_DS_FIX]
				add		r8, r8, #(2 << 16)
				add		r2, r1, r0
				bic		r0, r2, #(1 << 16)
				mov		pc, r4

a_d16			mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldr		r6, [r9, #CPU_DS_FIX]
				add		r8, r8, #(2 << 16)
				mov		pc, r4

a_bp_d8			mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread
				ldrh	r1, [r9, #CPU_BP]
				ldr		r6, [r9, #CPU_SS_FIX]
				tst		r0, #&80
				orrne	r0, r0, #&ff00
				add		r2, r0, r1
				add		r8, r8, #(1 << 16)
				bic		r0, r2, #(1 << 16)
				mov		pc, r4

a_bp_d16		mov		r4, lr
				ldr		r0, [r9, #CPU_CS_BASE]
				ldr		r6, [r9, #CPU_SS_FIX]
				add		r0, r0, r8 lsr #16
				bl		i286a_memoryread_w
				ldrh	r1, [r9, #CPU_BP]
				add		r8, r8, #(2 << 16)
				add		r2, r1, r0
				bic		r0, r2, #(1 << 16)
				mov		pc, r4

	END


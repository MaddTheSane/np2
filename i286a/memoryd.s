
USE_HIMEM			equ		&110000

	INCLUDE	i286a.inc

	IMPORT	memfn
	IMPORT	i286core
	IMPORT	i286_nonram_r
	IMPORT	i286_nonram_rw

	EXPORT	i286_memoryread
	EXPORT	i286_memoryread_w
	EXPORT	i286_memorywrite
	EXPORT	i286_memorywrite_w

	EXPORT	i286a_memoryread
	EXPORT	i286a_memoryread_w
	EXPORT	i286a_memorywrite
	EXPORT	i286a_memorywrite_w

	AREA	.text, CODE, READONLY

i286_memoryread
i286a_memoryread
				ldr		r3, i2mr_cpu
				cmp		r0, #I286_MEMREADMAX
				bcs		i2mr_ext
				ldrb	r0, [r3, r0]
				mov		pc, lr
i2mr_cpu		dcd		i286core - CPU_REG
i2mr_ext		cmp		r0, #USE_HIMEM
				bcs		i2mr_himem
				stmdb	sp!, {r7, r9, lr}
				ldr		r2, i2mr_memfnrd8
				and		r12, r0, #(&1f << 15)
			;	ldr		r7, [r3, #CPU_REMAINCLOCK]
				ldr		r2, [r2, r12 lsr #(15 - 2)]
				mov		r9, r3						; cpu
				mov		lr, pc
				mov		pc, r2
			;	str		r7, [r9, #CPU_REMAINCLOCK]
				ldmia	sp!, {r7, r9, pc}
i2mr_memfnrd8	dcd		memfn
i2mr_himem		ldr		r12, [r3, #CPU_EXTMEMSIZE]
				ldr		r2, [r3, #CPU_EXTMEM]
				sub		r1, r0, #&100000
				cmp		r1, r12
				bcs		i286_nonram_r
				ldrb	r0, [r2, r1]
				mov		pc, lr


i286_memoryread_w
i286a_memoryread_w
				ldr		r3, i2mrw_cpu
				tst		r0, #1
				bne		i2mro_main
				cmp		r0, #I286_MEMREADMAX
				bcs		i2mre_ext
				ldrh	r0, [r3, r0]
				mov		pc, lr
i2mrw_cpu		dcd		i286core - CPU_REG
i2mre_ext		cmp		r0, #USE_HIMEM
				bcs		i2mre_himem
i2mrw_ext		stmdb	sp!, {r7, r9, lr}
				ldr		r2, i2mre_memfnrd16
				and		r12, r0, #(&1f << 15)
			;	ldr		r7, [r3, #CPU_REMAINCLOCK]
				ldr		r2, [r2, r12 lsr #(15 - 2)]
				mov		r9, r3						; cpu
				mov		lr, pc
				mov		pc, r2
			;	str		r7, [r9, #CPU_REMAINCLOCK]
				ldmia	sp!, {r7, r9, pc}
i2mre_memfnrd16	dcd		memfn + (32 * 4) * 2
i2mre_himem		ldr		r12, [r3, #CPU_EXTMEMSIZE]
				ldr		r12, [r3, #CPU_EXTMEM]
				sub		r1, r0, #&100000
				cmp		r1, r12
				bcs		i286_nonram_rw
				ldrh	r0, [r1, r2]
				mov		pc, lr
i2mro_main		add		r1, r0, #1
				cmp		r1, #I286_MEMREADMAX
				bcs		i2mro_ext
				add		r1, r3, r0
				ldrb	r0, [r3, r0]
				ldrb	r1, [r1, #1]
				add		r0, r0, r1 lsl #8
				mov		pc, lr
i2mro_ext		cmp		r1, #USE_HIMEM
				bcs		i2mro_himem
				movs	r1, r1, lsl #(32 - 15)
				bne		i2mrw_ext
				stmdb	sp!, {r4 - r7, r9, lr}		; ここチェックするように…
				ldr		r4, i2mro_memfnrd8
					and		r1, r0, #(&1f << 15)
			;	ldr		r7, [r3, #CPU_REMAINCLOCK]
				mov		r9, r3
					ldr		r2, [r4, r1 lsr #(15 - 2)]
				add		r5, r0, #1
					mov		lr, pc
					mov		pc, r2
					and		r1, r5, #(&1f << 15)
				mov		r6, r0
					ldr		r2, [r4, r1, lsr #(15 - 2)]
				mov		r0, r5
					mov		lr, pc
					mov		pc, r2
				add		r0, r6, r0 lsl #8
			;	str		r7, [r9, #CPU_REMAINCLOCK]
				ldmia	sp!, {r4 - r7, r9, pc}
i2mro_memfnrd8	dcd		memfn
i2mro_himem		ldr		r2, [r3, #CPU_EXTMEM]
				ldr		r12, [r3, #CPU_EXTMEMSIZE]
				sub		r1, r1, #&100000
				add		r2, r1, r2
				beq		i2mro_10ffff			; = 10ffff
				cmp		r1, r12
				bcs		i2mro_himeml			; = over
				ldrb	r0, [r2, #-1]
				ldrb	r1, [r2]
				add		r0, r0, r1 lsl #8
				mov		pc, lr
i2mro_10ffff	ldrb	r0, [r3, r0]
				cmp		r1, r12
				bcs		i2mro_himemh
				ldrb	r1, [r2]
				add		r0, r0, r1 lsl #8
				mov		pc, lr
i2mro_himeml	ldreqb	r0, [r2, #-1]
				movne	r0, #&ff
i2mro_himemh	orr		r0, r0, #&ff00
				mov		pc, lr


i286_memorywrite
i286a_memorywrite
				ldr		r3, i2mw_cpu
				cmp		r0, #I286_MEMWRITEMAX
				bcs		i2mw_ext
				strb	r1, [r3, r0]
				mov		pc, lr
i2mw_cpu		dcd		i286core - CPU_REG						; !!
i2mw_ext		cmp		r0, #USE_HIMEM
				bcs		i2mw_himem
				stmdb	sp!, {r7, r9, lr}
				ldr		r2, i2mw_memfnwr8
				and		r12, r0, #(&1f << 15)
			;	ldr		r7, [r3, #CPU_REMAINCLOCK]
				ldr		r2, [r2, r12, lsr #(15 - 2)]
				mov		r9, r3
				mov		lr, pc
				mov		pc, r2
			;	str		r7, [r9, #CPU_REMAINCLOCK]
				ldmia	sp!, {r7, r9, pc}
i2mw_memfnwr8	dcd		memfn + (32 * 4)
i2mw_himem		ldr		r12, [r3, #CPU_EXTMEMSIZE]
				ldr		r3, [r3, #CPU_EXTMEM]
				sub		r2, r0, #&100000
				cmp		r2, r12
				strccb	r1, [r3, r2]
				mov		pc, lr


i286_memorywrite_w
i286a_memorywrite_w
				ldr		r3, i2mww_cpu
				tst		r0, #1
				bne		i2mwo_main
				cmp		r0, #I286_MEMWRITEMAX
				bcs		i2mwe_ext
				strh	r1, [r3, r0]
				mov		pc, lr
i2mww_cpu		dcd		i286core - CPU_REG						; !!
i2mwe_ext		cmp		r0, #USE_HIMEM
				bcs		i2mwe_himem
i2mww_ext		stmdb	sp!, {r7, r9, lr}
				ldr		r2, i2mwe_memfnwr16
				and		r12, r0, #(&1f << 15)
			;	ldr		r7, [r3, #CPU_REMAINCLOCK]
				ldr		r2, [r2, r12 lsr #(15 - 2)]
				mov		r9, r3						; cpu
				mov		lr, pc
				mov		pc, r2
			;	str		r7, [r9, #CPU_REMAINCLOCK]
				ldmia	sp!, {r7, r9, pc}
i2mwe_memfnwr16	dcd		memfn + (32 * 4) * 3
i2mwe_himem		ldr		r12, [r3, #CPU_EXTMEMSIZE]
				ldr		r3, [r3, #CPU_EXTMEM]
				sub		r2, r0, #&100000
				cmp		r2, r12
				strcch	r1, [r2, r3]
				mov		pc, lr

i2mwo_main		add		r2, r0, #1
				cmp		r2, #I286_MEMWRITEMAX
				bcs		i2mwo_ext
				add		r2, r3, r0
				mov		r12, r1 lsr #8
				strb	r1, [r3, r0]
				strb	r12, [r2, #1]
				mov		pc, lr

i2mwo_ext		cmp		r2, #USE_HIMEM
				bcs		i2mwo_himem
				movs	r2, r2, lsl #(32 - 15)
				bne		i2mww_ext

				stmdb	sp!, {r4 - r7, r9, lr}		; ここチェックするように…
				ldr		r4, i2mwo_memfnwr8
					and		r12, r0, #(&1f << 15)
			;	ldr		r7, [r3, #CPU_REMAINCLOCK]
				mov		r9, r3
					ldr		r2, [r4, r12 lsr #(15 - 2)]
				add		r5, r0, #1
				mov		r6, r1 lsr #8
					mov		lr, pc
					mov		pc, r2
					and		r12, r5, #(&1f << 15)
				mov		r0, r5
					ldr		r2, [r4, r12, lsr #(15 - 2)]
				mov		r1, r6
					mov		lr, pc
					mov		pc, r2
			;	str		r7, [r9, #CPU_REMAINCLOCK]
				ldmia	sp!, {r4 - r7, r9, pc}
i2mwo_memfnwr8	dcd		memfn + (32 * 4)

i2mwo_himem		ldr		r12, [r3, #CPU_EXTMEM]
				ldr		r0, [r3, #CPU_EXTMEMSIZE]
				sub		r2, r2, #&100000
				add		r12, r2, r12
				beq		i2mwo_10ffff			; = 10ffff
				cmp		r2, r0
				mov		r2, r1 lsr #8
				strlsb	r1, [r12, #-1]
				strccb	r2, [r12]
				mov		pc, lr
i2mwo_10ffff	add		r3, r3, #USE_HIMEM
				mov		r2, r1 lsr #8
				strb	r1, [r3]
				cmp		r0, #0
				strneb	r1, [r12]
				mov		pc, lr

	END


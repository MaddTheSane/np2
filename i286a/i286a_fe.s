
	INCLUDE		i286a.inc
	INCLUDE		i286aalu.inc

	IMPORT		_szpcflag8
	IMPORT		i286a_ea
	IMPORT		i286_memoryread
	IMPORT		i286_memoryread_w
	IMPORT		i286_memorywrite
	IMPORT		i286_memorywrite_w

	EXPORT		i286a_0xfe
	EXPORT		i286a_0xff

	AREA	.text, CODE, READONLY


i286a_0xfe		GETPC8
				tst		r0, #(1 << 3)
				bne		decea8

				cmp		r0, #&c0
				bcc		inc8m
				CPUWORK	#2
				R8SRC	r0, r5
				ldrb	r4, [r5, #CPU_REG]
				INC8	r4
				strb	r1, [r5, #CPU_REG]
				mov		pc, r11
inc8m			CPUWORK	#7
				bl		i286a_ea
				cmp		r0, #I286_MEMWRITEMAX
				bcs		inc8e
				ldrb	r4, [r9, r0]
				INC8	r4
				strb	r1, [r9, r0]
				mov		pc, r11
inc8e			mov		r5, r0
				bl		i286_memoryread
				INC8	r0
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite

decea8			cmp		r0, #&c0
				bcc		dec8m
				CPUWORK	#2
				R8SRC	r0, r5
				ldrb	r4, [r5, #CPU_REG]
				DEC8	r4
				strb	r1, [r5, #CPU_REG]
				mov		pc, r11
dec8m			CPUWORK	#7
				bl		i286a_ea
				cmp		r0, #I286_MEMWRITEMAX
				bcs		dec8e
				ldrb	r4, [r9, r0]
				DEC8	r4
				strb	r1, [r9, r0]
				mov		pc, r11
dec8e			mov		r5, r0
				bl		i286_memoryread
				DEC8	r0
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite


; ----

i286a_0xff		GETPC8
				and		r12, r0, #(7 << 3)
				adr		r1, opefftbl
				ldr		pc, [r1, r12 lsr #1]

opefftbl		dcd		incea16
				dcd		decea16

				dcd		pushea16
				dcd		popea16

incea16			cmp		r0, #&c0
				bcc		inc16m
				CPUWORK	#2
				R16SRC	r0, r5
				ldrh	r4, [r5, #CPU_REG]
				INC16	r4
				strh	r1, [r5, #CPU_REG]
				mov		pc, r11
inc16m			CPUWORK	#7
				bl		i286a_ea
				tst		r0, #1
				bne		inc16e
				cmp		r0, #I286_MEMWRITEMAX
				bcs		inc16e
				ldrh	r4, [r9, r0]
				INC16	r4
				strh	r1, [r9, r0]
				mov		pc, r11
inc16e			mov		r5, r0
				bl		i286_memoryread_w
				INC16	r0
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite_w

decea16			cmp		r0, #&c0
				bcc		dec16m
				CPUWORK	#2
				R16SRC	r0, r5
				ldrh	r4, [r5, #CPU_REG]
				DEC16	r4
				strh	r1, [r5, #CPU_REG]
				mov		pc, r11
dec16m			CPUWORK	#7
				bl		i286a_ea
				tst		r0, #1
				bne		dec16e
				cmp		r0, #I286_MEMWRITEMAX
				bcs		dec16e
				ldrh	r4, [r9, r0]
				DEC16	r4
				strh	r1, [r9, r0]
				mov		pc, r11
dec16e			mov		r5, r0
				bl		i286_memoryread_w
				DEC16	r0
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite_w

pushea16		cmp		r0, #&c0
				bcc		push16m
				CPUWORK	#3
				R16SRC	r0, r5
				ldrh	r1, [r5, #CPU_REG]
				b		push16e
push16m			CPUWORK	#5
				bl		i286a_ea
				bl		i286_memoryread_w
				mov		r1, r0
push16e			ldrh	r2, [r9, #CPU_SP]
				ldr		r3, [r9, #CPU_SS_BASE]
				subs	r2, r2, #2
				addcc	r2, r2, #&10000
				strh	r2, [r9, #CPU_SP]
				add		r0, r2, r3
				mov		lr, r11
				b		i286_memorywrite_w

popea16			CPUWORK	#5
				ldrh	r2, [r9, #CPU_SP]
				ldr		r3, [r9, #CPU_SS_BASE]
				mov		r6, r0
				add		r0, r2, r3
				add		r2, r2, #2
				bic		r2, r2, #&10000
				strh	r2, [r9, #CPU_SP]
				bl		i286_memoryread_w
				cmp		r6, #&c0
				bcc		pop16m
				R16SRC	r6, r1
				ldrh	r0, [r1, #CPU_REG]
				mov		pc, r11
pop16m			mov		r5, r0
				mov		r0, r6
				bl		i286a_ea
				mov		r1, r5
				mov		lr, r11
				b		i286_memoryread_w

	END


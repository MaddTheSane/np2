
	INCLUDE		i286a.inc
	INCLUDE		i286aalu.inc

	IMPORT		i286a_ea
	IMPORT		i286_memoryread
	IMPORT		i286_memoryread_w
	IMPORT		i286_memorywrite
	IMPORT		i286_memorywrite_w

	EXPORT		i286aop80
	EXPORT		i286aop81
	EXPORT		i286aop83

	AREA	.text, CODE, READONLY

i286aop80		GETPC8
				and		r6, r0, #(7 << 3)
				cmp		r0, #&c0
				bcc		ope80m
				CPUWORK	#3
				R8SRC	r0, r5
				add		r5, r5, #CPU_REG
				GETPC8
				adr		r1, op8x_reg8
				ldr		pc, [r1, r6 lsr #1]
ope80m			CPUWORK	#7
				bl		i286a_ea
				cmp		r0, #I286_MEMWRITEMAX
				bcs		ope80e
				add		r5, r9, r0
				GETPC8
				adr		r1, op8x_reg8
				ldr		pc, [r1, r6 lsr #1]
ope80e			mov		r4, r0
				GETPC8
				mov		r5, r0
				bl		i286_memoryread
				adr		r1, op8x_ext8
				ldr		pc, [r1, r6 lsr #1]

op8x_reg8		dcd		add_r8_i
				dcd		or_r8_i
				dcd		adc_r8_i
				dcd		sbb_r8_i
				dcd		and_r8_i
				dcd		sub_r8_i
				dcd		xor_r8_i
				dcd		cmp_r8_i

op8x_ext8		dcd		add_r8_e
				dcd		or_r8_e
				dcd		adc_r8_e
				dcd		sbb_r8_e
				dcd		and_r8_e
				dcd		sub_r8_e
				dcd		xor_r8_e
				dcd		cmp_r8_e

add_r8_i		ldrb	r6, [r5]
				ADD8	r6, r0
				strb	r1, [r5]
				mov		pc, r11
or_r8_i			ldrb	r6, [r5]
				OR8		r6, r0
				strb	r1, [r5]
				mov		pc, r11
adc_r8_i		ldrb	r6, [r5]
				ADC8	r6, r0
				strb	r1, [r5]
				mov		pc, r11
sbb_r8_i		ldrb	r6, [r5]
				SBB8	r6, r0
				strb	r1, [r5]
				mov		pc, r11
and_r8_i		ldrb	r6, [r5]
				AND8	r6, r0
				strb	r1, [r5]
				mov		pc, r11
sub_r8_i		ldrb	r6, [r5]
				SUB8	r6, r0
				strb	r1, [r5]
				mov		pc, r11
xor_r8_i		ldrb	r6, [r5]
				XOR8	r6, r0
				strb	r1, [r5]
				mov		pc, r11
cmp_r8_i		ldrb	r6, [r5]
				SUB8	r6, r0
				mov		pc, r11

add_r8_e		ADD8	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite
or_r8_e			OR8		r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite
adc_r8_e		ADC8	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite
sbb_r8_e		SBB8	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite
and_r8_e		AND8	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite
sub_r8_e		SUB8	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite
xor_r8_e		XOR8	r0, r4
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite
cmp_r8_e		SUB8	r0, r4
				mov		pc, r11


; ----

i286aop81		GETPC8
				and		r6, r0, #(7 << 3)
				cmp		r0, #&c0
				bcc		ope81m
				CPUWORK	#3
				R16SRC	r0, r5
				add		r5, r5, #CPU_REG
				GETPC16
				adr		r1, op8x_reg16
				ldr		pc, [r1, r6 lsr #1]
ope81m			CPUWORK	#7
				bl		i286a_ea
				tst		r0, #1
				bne		ope81e
				cmp		r0, #I286_MEMWRITEMAX
				bcs		ope81e
				add		r5, r9, r0
				GETPC16
				adr		r1, op8x_reg16
				ldr		pc, [r1, r6 lsr #1]
ope81e			mov		r5, r0
				bl		i286_memoryread_w
				mov		r4, r0
				GETPC16
				adr		r1, op8x_ext16
				ldr		pc, [r1, r6 lsr #1]

i286aop83		GETPC8
				and		r6, r0, #(7 << 3)
				cmp		r0, #&c0
				bcc		ope83m
				CPUWORK	#3
				R16SRC	r0, r5
				add		r5, r5, #CPU_REG
				GETPC8
				tst		r0, #(1 << 7)
				orrne	r0, r0, #(&ff << 8)
				adr		r1, op8x_reg16
				ldr		pc, [r1, r6 lsr #1]
ope83m			CPUWORK	#7
				bl		i286a_ea
				tst		r0, #1
				bne		ope83e
				cmp		r0, #I286_MEMWRITEMAX
				bcs		ope83e
				add		r5, r9, r0
				GETPC8
				tst		r0, #(1 << 7)
				orrne	r0, r0, #(&ff << 8)
				adr		r1, op8x_reg16
				ldr		pc, [r1, r6 lsr #1]
ope83e			mov		r5, r0
				bl		i286_memoryread_w
				mov		r4, r0
				GETPC8
				tst		r0, #(1 << 7)
				orrne	r0, r0, #(&ff << 8)
				adr		r1, op8x_ext16
				ldr		pc, [r1, r6 lsr #1]

op8x_reg16		dcd		add_r16_i
				dcd		or_r16_i
				dcd		adc_r16_i
				dcd		sbb_r16_i
				dcd		and_r16_i
				dcd		sub_r16_i
				dcd		xor_r16_i
				dcd		cmp_r16_i

op8x_ext16		dcd		add_r16_e
				dcd		or_r16_e
				dcd		adc_r16_e
				dcd		sbb_r16_e
				dcd		and_r16_e
				dcd		sub_r16_e
				dcd		xor_r16_e
				dcd		cmp_r16_e

add_r16_i		ldrh	r6, [r5]
				ADD16	r6, r0
				strh	r1, [r5]
				mov		pc, r11
or_r16_i		ldrh	r6, [r5]
				OR16	r6, r0
				strh	r1, [r5]
				mov		pc, r11
adc_r16_i		ldrh	r6, [r5]
				ADC16	r6, r0
				strh	r1, [r5]
				mov		pc, r11
sbb_r16_i		ldrh	r6, [r5]
				SBB16	r6, r0
				strh	r1, [r5]
				mov		pc, r11
and_r16_i		ldrh	r6, [r5]
				AND16	r6, r0
				strh	r1, [r5]
				mov		pc, r11
sub_r16_i		ldrh	r6, [r5]
				SUB16	r6, r0
				strh	r1, [r5]
				mov		pc, r11
xor_r16_i		ldrh	r6, [r5]
				XOR16	r6, r0
				strh	r1, [r5]
				mov		pc, r11
cmp_r16_i		ldrh	r6, [r5]
				SUB16	r6, r0
				mov		pc, r11

add_r16_e		ADD16	r4, r0
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite_w
or_r16_e		OR16	r4, r0
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite_w
adc_r16_e		ADC16	r4, r0
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite_w
sbb_r16_e		SBB16	r4, r0
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite_w
and_r16_e		AND16	r4, r0
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite_w
sub_r16_e		SUB16	r4, r0
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite_w
xor_r16_e		XOR16	r4, r0
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite_w
cmp_r16_e		SUB16	r4, r0
				mov		pc, r11

	END


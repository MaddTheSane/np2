
	INCLUDE		i286a.inc
	INCLUDE		i286aea.inc
	INCLUDE		i286aalu.inc

	IMPORT		i286a_ea
	IMPORT		i286a_a
	IMPORT		i286a_memoryread
	IMPORT		i286a_memoryread_w
	IMPORT		i286a_memorywrite
	IMPORT		i286a_memorywrite_w

	IMPORT		i286a_localint

	EXPORT		i286a_cts

	AREA	.text, CODE, READONLY


i286a_cts		mov		r6, r8
				GETPC8
				cmp		r0, #1
				bne		cts_ldall
				GETPC8
				and		r12, r0, #(7 << 3)
				add		pc, pc, r12 lsr #1
				nop
				b		sgdt
				b		sidt
				b		lgdt
				b		lidt
				b		smsw
				b		smsw
				b		lmsw
				b		lmsw

sgdt			cmp		r0, #&c0
				bcs		cts_intr
				CPUWORK	#11
				bl		i286a_a
				add		r4, r0, #2
				ldrh	r1, [r9, #(CPU_GDTR + 0)]
				add		r0, r0, r6
				bic		r4, r4, #(1 << 16)
				bl		i286a_memorywrite_w
				add		r0, r4, r6
				ldrh	r1, [r9, #(CPU_GDTR + 2)]
				add		r4, r4, #2
				bl		i286a_memorywrite_w
				ldrb	r1, [r9, #(CPU_GDTR + 4)]
				bic		r4, r4, #(1 << 16)
				orr		r1, r1, #&ff00
				add		r0, r4, r6
				mov		lr, r11
				b		i286a_memorywrite_w

sidt			cmp		r0, #&c0
				bcs		cts_intr
				CPUWORK	#12
				bl		i286a_a
				add		r4, r0, #2
				ldrh	r1, [r9, #(CPU_IDTR + 0)]
				add		r0, r0, r6
				bic		r4, r4, #(1 << 16)
				bl		i286a_memorywrite_w
				add		r0, r4, r6
				ldrh	r1, [r9, #(CPU_IDTR + 2)]
				add		r4, r4, #2
				bl		i286a_memorywrite_w
				ldrb	r1, [r9, #(CPU_IDTR + 4)]
				bic		r4, r4, #(1 << 16)
				orr		r1, r1, #&ff00
				add		r0, r4, r6
				mov		lr, r11
				b		i286a_memorywrite_w

lgdt			cmp		r0, #&c0
				bcs		cts_intr
				CPUWORK	#11
				bl		i286a_a
				add		r4, r0, #2
				add		r0, r0, r6
				bic		r4, r4, #(1 << 16)
				bl		i286a_memoryread_w
				strh	r0, [r9, #(CPU_GDTR + 0)]
				add		r0, r4, r6
				add		r4, r4, #2
				bl		i286a_memoryread_w
				bic		r4, r4, #(1 << 16)
				strh	r0, [r9, #(CPU_GDTR + 2)]
				add		r0, r4, r6
				bl		i286a_memoryread_w
				strh	r0, [r9, #(CPU_GDTR + 4)]
				mov		pc, r11

lidt			cmp		r0, #&c0
				bcs		cts_intr
				CPUWORK	#12
				bl		i286a_a
				add		r4, r0, #2
				add		r0, r0, r6
				bic		r4, r4, #(1 << 16)
				bl		i286a_memoryread_w
				strh	r0, [r9, #(CPU_IDTR + 0)]
				add		r0, r4, r6
				add		r4, r4, #2
				bl		i286a_memoryread_w
				bic		r4, r4, #(1 << 16)
				strh	r0, [r9, #(CPU_IDTR + 2)]
				add		r0, r4, r6
				bl		i286a_memoryread_w
				strh	r0, [r9, #(CPU_IDTR + 4)]
				mov		pc, r11

smsw			cmp		r0, #&c0
				bcc		smswm
				CPUWORK	#3
				R16SRC	r0, r5
				ldrh	r1, [r9, #CPU_MSW]
				strh	r1, [r5, #CPU_REG]
				mov		pc, r11
smswm			CPUWORK	#6
				bl		i286a_ea
				strh	r1, [r9, #CPU_MSW]
				mov		lr, r11
				b		i286a_memorywrite_w

lmsw			cmp		r0, #&c0
				bcc		lmswm
				CPUWORK	#3
				R16SRC	r0, r5
				ldrh	r0, [r5, #CPU_REG]
				strh	r0, [r9, #CPU_MSW]
				mov		pc, r11
lmswm			CPUWORK	#6
				bl		i286a_ea
				bl		i286a_memoryread_w
				strh	r0, [r9, #CPU_MSW]
				mov		pc, r11

cts_ldall		cmp		r0, #5
				bne		cts_intr
				add		r6, r9, #&800
				CPUWORK	#195
				ldrh	r0, [r6, #&04]			; MSW
				ldr		r8, [r6, #&18]			; IP:flag
				mov		r2, #3
				strh	r0, [r9, #CPU_MSW]
				and		r2, r2, r8 lsr #8
				bic		r8, r8, #&f000
				and		r2, r2, r2 lsr #1
				strb	r2, [r9, #CPU_TRAP]
				ldrh	r0, [r6, #&1e]			; DS
				ldr		r1, [r6, #&20]			; CS:SS
				ldr		r2, [r6, #&24]			; DI:ES
				strh	r0, [r9, #CPU_DS]
				mov		r0, r1 lsr #16
				strh	r1, [r9, #CPU_SS]
				strh	r0, [r9, #CPU_CS]
				mov		r0, r2 lsr #16
				strh	r2, [r9, #CPU_ES]
				strh	r0, [r9, #CPU_DI]

				ldr		r0, [r6, #&28]			; BP:SI
				ldr		r1, [r6, #&2c]			; BX:SP
				ldr		r2, [r6, #&30]			; CX:DX
				mov		r12, r0 lsr #16
				strh	r0, [r9, #CPU_SI]
				strh	r12, [r9, #CPU_BP]
				mov		r12, r1 lsr #16
				strh	r1, [r9, #CPU_SP]
				strh	r12, [r9, #CPU_BX]
				mov		r12, r2 lsr #16
				strh	r2, [r9, #CPU_DX]
				strh	r12, [r9, #CPU_CX]

				ldrh	r0, [r6, #&34]			; AX
				ldrh	r1, [r6, #&36]			; ES
				ldrb	r2, [r6, #&38]			; ES
				ldr		r3, [r6, #&3c]			; CS
				ldrh	r4, [r6, #&42]			; SS
				ldrb	r5, [r6, #&44]			; SS
				ldr		r6, [r6, #&48]			; DS
				strh	r0, [r9, #CPU_AX]
				orr		r1, r1, r2 lsl #16
				bic		r3, r3, #(&ff << 24)
				orr		r4, r4, r5 lsl #16
				bic		r6, r6, #(&ff << 24)
				str		r1, [r9, #CPU_ES_BASE]
				str		r3, [r9, #CPU_CS_BASE]
				str		r4, [r9, #CPU_SS_BASE]
				str		r6, [r9, #CPU_DS_BASE]
				str		r4, [r9, #CPU_SS_FIX]
				str		r6, [r9, #CPU_DS_FIX]
				I286IRQCHECKTERM

cts_intr		sub		r8, r6, #(1 << 16)
				mov		r6, #6
				b		i286a_localint

	END


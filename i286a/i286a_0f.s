
	INCLUDE		i286a.inc
	INCLUDE		i286aalu.inc

	IMPORT		i286a_ea
	IMPORT		i286a_a
	IMPORT		i286_memoryread
	IMPORT		i286_memoryread_w
	IMPORT		i286_memorywrite
	IMPORT		i286_memorywrite_w

	IMPORT		i286a_localint

	EXPORT		i286a_cts

	AREA	.text, CODE, READONLY


i286a_cts		mov		r6, r8
				GETPC8
				cmp		r0, #1
				bne		cts_ldall
				GETPC8
				and		r12, r0, #(7 << 3)
				adr		r1, cts1tbl
				ldr		pc, [r1, r12 lsr #1]
cts1tbl			dcd		sgdt
				dcd		sidt
				dcd		lgdt
				dcd		lidt
				dcd		smsw
				dcd		smsw
				dcd		lmsw
				dcd		lmsw

cts_ldall		cmp		r0, #5
				bne		cts_intr

cts_intr		sub		r8, r6, #(1 << 16)
				mov		r6, #6
				b		i286a_localint

sgdt			cmp		r0, #&c0
				bcs		cts_intr
				CPUWORK	#11
				bl		i286a_a
				add		r4, r0, #2
				ldrh	r1, [r9, #(CPU_GDTR + 0)]
				add		r0, r0, r6
				bic		r4, r4, #(1 << 16)
				bl		i286_memorywrite_w
				add		r0, r4, r6
				ldrh	r1, [r9, #(CPU_GDTR + 2)]
				add		r4, r4, #2
				bl		i286_memorywrite_w
				ldrb	r1, [r9, #(CPU_GDTR + 4)]
				bic		r4, r4, #(1 << 16)
				orr		r1, r1, #&ff00
				add		r0, r4, r6
				mov		lr, r11
				b		i286_memorywrite_w

sidt			cmp		r0, #&c0
				bcs		cts_intr
				CPUWORK	#12
				bl		i286a_a
				add		r4, r0, #2
				ldrh	r1, [r9, #(CPU_IDTR + 0)]
				add		r0, r0, r6
				bic		r4, r4, #(1 << 16)
				bl		i286_memorywrite_w
				add		r0, r4, r6
				ldrh	r1, [r9, #(CPU_IDTR + 2)]
				add		r4, r4, #2
				bl		i286_memorywrite_w
				ldrb	r1, [r9, #(CPU_IDTR + 4)]
				bic		r4, r4, #(1 << 16)
				orr		r1, r1, #&ff00
				add		r0, r4, r6
				mov		lr, r11
				b		i286_memorywrite_w

lgdt			cmp		r0, #&c0
				bcs		cts_intr
				CPUWORK	#11
				bl		i286a_a
				add		r4, r0, #2
				add		r0, r0, r6
				bic		r4, r4, #(1 << 16)
				bl		i286_memoryread_w
				strh	r0, [r9, #(CPU_GDTR + 0)]
				add		r0, r4, r6
				add		r4, r4, #2
				bl		i286_memoryread_w
				bic		r4, r4, #(1 << 16)
				strh	r0, [r9, #(CPU_GDTR + 2)]
				add		r0, r4, r6
				bl		i286_memoryread_w
				strh	r0, [r9, #(CPU_GDTR + 4)]
				mov		pc, r11

lidt			cmp		r0, #&c0
				bcs		cts_intr
				CPUWORK	#12
				bl		i286a_a
				add		r4, r0, #2
				add		r0, r0, r6
				bic		r4, r4, #(1 << 16)
				bl		i286_memoryread_w
				strh	r0, [r9, #(CPU_IDTR + 0)]
				add		r0, r4, r6
				add		r4, r4, #2
				bl		i286_memoryread_w
				bic		r4, r4, #(1 << 16)
				strh	r0, [r9, #(CPU_IDTR + 2)]
				add		r0, r4, r6
				bl		i286_memoryread_w
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
				b		i286_memorywrite_w

lmsw			cmp		r0, #&c0
				bcc		lmswm
				CPUWORK	#3
				R16SRC	r0, r5
				ldrh	r0, [r5, #CPU_REG]
				strh	r0, [r9, #CPU_MSW]
				mov		pc, r11
lmswm			CPUWORK	#6
				bl		i286a_ea
				bl		i286_memoryread_w
				strh	r0, [r9, #CPU_MSW]
				mov		pc, r11

	END


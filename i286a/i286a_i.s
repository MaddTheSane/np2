
	INCLUDE		i286a.inc

	IMPORT		i286core
	IMPORT		_szpcflag8
	IMPORT		i286_memorywrite_w
	EXPORT		i286a_localint
	EXPORT		i286a_trapint
	IMPORT		i286a_trapintr
	EXPORT		i286a_interrupt

	AREA	.text, CODE, READONLY

				; r6 - num / r8 - IP / r11 - ret
i286a_localint	ldrh	r4, [r9, #CPU_SP]
				ldr		r5, [r9, #CPU_SS_BASE]
				CPUWORK	#20
				mov		r4, r4 lsl #16
				sub		r4, r4, #(2 << 16)
				mov		r1, r8
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				sub		r4, r4, #(2 << 16)
				ldrh	r1, [r9, #CPU_CS]
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				sub		r4, r4, #(2 << 16)
				mov		r1, r8 lsr #16
				mov		r4, r4 lsr #16
				sub		r8, r8, r1 lsl #16
				add		r0, r4, r5
				strh	r4, [r9, #CPU_SP]
				bl		i286_memorywrite_w
				mov		r0, #0
				ldr		r1, [r9, r6 lsl #2]
				strb	r0, [r9, #CPU_TRAP]
				bic		r8, r8, #(T_FLAG + I_FLAG)
				mov		r0, r1 lsr #16
				orr		r8, r8, r1 lsl #16
				mov		r2, r0 lsl #4
				strh	r0, [r9, #CPU_CS]
				str		r2, [r9, #CPU_CS_BASE]
				mov		pc, r11

i286a_trapint	ldrh	r4, [r9, #CPU_SP]
				ldr		r5, [r9, #CPU_SS_BASE]
				CPUWORK	#20
				mov		r4, r4 lsl #16
				sub		r4, r4, #(2 << 16)
				mov		r1, r8
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				sub		r4, r4, #(2 << 16)
				ldrh	r1, [r9, #CPU_CS]
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				sub		r4, r4, #(2 << 16)
				mov		r1, r8 lsr #16
				mov		r4, r4 lsr #16
				sub		r8, r8, r1 lsl #16
				add		r0, r4, r5
				strh	r4, [r9, #CPU_SP]
				bl		i286_memorywrite_w
				mov		r0, #0
				ldr		r1, [r9, #4]
				strb	r0, [r9, #CPU_TRAP]
				bic		r8, r8, #(T_FLAG + I_FLAG)
				mov		r0, r1 lsr #16
				orr		r8, r8, r1 lsl #16
				mov		r2, r0 lsl #4
				strh	r0, [r9, #CPU_CS]
				str		r2, [r9, #CPU_CS_BASE]
				b		i286a_trapintr



i286a_interrupt	stmdb	sp!, {r4 - r10, lr}
				ldr		r9, iai_r9
				ldr		r10, iai_r10			; ˆê‰ž
				mov		r6, r0
				CPULD
				ldrh	r4, [r9, #CPU_SP]
				ldr		r5, [r9, #CPU_SS_BASE]
				CPUWORK	#20
				mov		r4, r4 lsl #16
				sub		r4, r4, #(2 << 16)
				mov		r1, r8
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				sub		r4, r4, #(2 << 16)
				ldrh	r1, [r9, #CPU_CS]
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				sub		r4, r4, #(2 << 16)
				mov		r1, r8 lsr #16
				mov		r4, r4 lsr #16
				sub		r8, r8, r1 lsl #16
				add		r0, r4, r5
				strh	r4, [r9, #CPU_SP]
				bl		i286_memorywrite_w
				mov		r0, #0
				ldr		r1, [r9, r6 lsl #2]
				strb	r0, [r9, #CPU_TRAP]
				bic		r8, r8, #(T_FLAG + I_FLAG)
				mov		r0, r1 lsr #16
				orr		r8, r8, r1 lsl #16
				mov		r2, r0 lsl #4
				strh	r0, [r9, #CPU_CS]
				str		r2, [r9, #CPU_CS_BASE]
				CPUSVC
				ldmia	sp!, {r4 - r11, pc}
iai_r9			dcd		i286core - CPU_REG
iai_r10			dcd		_szpcflag8

	END




	MACRO
$label	OP_EA_R8	$alu, $regclk, $memclk
$label		EAREG8	r12
			ldrb	r6, [r12, #CPU_REG]
			cmp		r0, #&c0
			bcc		$label.1
			CPUWORK	$regclk
			R8SRC	r0, r5
			ldrb	r4, [r5, #CPU_REG]
			$alu	r4, r6
			strb	r1, [r5, #CPU_REG]
			mov		pc, r11
$label.1	CPUWORK	$memclk
			bl		i286a_ea
			cmp		r0, #I286_MEMWRITEMAX
			bcs		$label.2
			ldrb	r4, [r9, r0]
			$alu	r4, r6
			strb	r1, [r9, r0]
			mov		pc, r11
$label.2	mov		r5, r0
			bl		i286_memoryread
			$alu	r0, r6
			mov		r0, r5
			mov		lr, r11
			b		i286_memorywrite
	MEND

	MACRO
$label	OP_EA_R16	$alu, $regclk, $memclk
$label		EAREG16	r12
			ldrh	r6, [r12, #CPU_REG]
			cmp		r0, #&c0
			bcc		$label.1
			CPUWORK	$regclk
			R8SRC	r0, r5
			ldrh	r4, [r5, #CPU_REG]
			$alu	r4, r6
			strh	r1, [r5, #CPU_REG]
			mov		pc, r11
$label.1	CPUWORK	$memclk
			bl		i286a_ea
			tst		r0, #1
			bne		$label.2
			cmp		r0, #I286_MEMWRITEMAX
			bcs		$label.2
			ldrh	r4, [r9, r0]
			$alu	r4, r6
			strh	r1, [r9, r0]
			mov		pc, r11
$label.2	mov		r5, r0
			bl		i286_memoryread_w
			$alu	r0, r6
			mov		r0, r5
			mov		lr, r11
			b		i286_memorywrite_w
	MEND

	MACRO
$label	OP_R8_EA	$alu, $regclk, $memclk
$label		REG8EA	r5, $regclk, $memclk
			ldrb	r2, [r5, #CPU_REG]
			$alu	r2, r0
			strb	r1, [r5, #CPU_REG]
			mov		pc, r11
	MEND

	MACRO
$label	OP_R16_EA	$alu, $regclk, $memclk
$label		REG16EA	r5, $regclk, $memclk
			ldrh	r2, [r5, #CPU_REG]
			$alu	r2, r0
			strh	r1, [r5, #CPU_REG]
			mov		pc, r11
	MEND

	MACRO
$label	OP_AL_D8	$alu, $clk
$label		CPUWORK	$clk
			GETPC8
			ldrb	r2, [r9, #CPU_AL]
			$alu	r2, r0
			strb	r1, [r9, #CPU_AL]
			mov		pc, r11
	MEND

	MACRO
$label	OP_AX_D16	$alu, $clk
$label		CPUWORK	$clk
			GETPC16
			ldrh	r2, [r9, #CPU_AX]
			$alu	r2, r0
			strh	r1, [r9, #CPU_AX]
			mov		pc, r11
	MEND


	MACRO
$label	S_EA_R8		$alu, $regclk, $memclk
$label		EAREG8	r12
			ldrb	r6, [r12, #CPU_REG]
			cmp		r0, #&c0
			bcc		$label.1
			CPUWORK	$regclk
			R8SRC	r0, r5
			ldrb	r4, [r5, #CPU_REG]
			$alu	r4, r6
			mov		pc, r11
$label.1	CPUWORK	$memclk
			bl		i286a_ea
			cmp		r0, #I286_MEMWRITEMAX
			bcs		$label.2
			ldrb	r4, [r9, r0]
			$alu	r4, r6
			mov		pc, r11
$label.2	bl		i286_memoryread
			$alu	r0, r6
			mov		pc, r11
	MEND

	MACRO
$label	S_EA_R16	$alu, $regclk, $memclk
$label		EAREG16	r12
			ldrh	r6, [r12, #CPU_REG]
			cmp		r0, #&c0
			bcc		$label.1
			CPUWORK	$regclk
			R8SRC	r0, r5
			ldrh	r4, [r5, #CPU_REG]
			$alu	r4, r6
			mov		pc, r11
$label.1	CPUWORK	$memclk
			bl		i286a_ea
			tst		r0, #1
			bne		$label.2
			cmp		r0, #I286_MEMWRITEMAX
			bcs		$label.2
			ldrh	r4, [r9, r0]
			$alu	r4, r6
			mov		pc, r11
$label.2	bl		i286_memoryread_w
			$alu	r0, r6
			mov		pc, r11
	MEND

	MACRO
$label	S_R8_EA		$alu, $regclk, $memclk
$label		REG8EA	r5, $regclk, $memclk
			ldrb	r2, [r5, #CPU_REG]
			$alu	r2, r0
			mov		pc, r11
	MEND

	MACRO
$label	S_R16_EA	$alu, $regclk, $memclk
$label		REG16EA	r5, $regclk, $memclk
			ldrh	r2, [r5, #CPU_REG]
			$alu	r2, r0
			mov		pc, r11
	MEND

	MACRO
$label	S_AL_D8		$alu, $clk
$label		CPUWORK	$clk
			GETPC8
			ldrb	r2, [r9, #CPU_AL]
			$alu	r2, r0
			mov		pc, r11
	MEND

	MACRO
$label	S_AX_D16	$alu, $clk
$label		CPUWORK	$clk
			GETPC16
			ldrh	r2, [r9, #CPU_AX]
			$alu	r2, r0
			mov		pc, r11
	MEND


	MACRO
$label	OP_INC16	$r, $clk
$label		CPUWORK	$clk
			ldrh	r2, [r9, $r]
			INC16	r2
			strh	r1, [r9, $r]
			mov		pc, r11
	MEND

	MACRO
$label	OP_DEC16	$r, $clk
$label		CPUWORK	$clk
			ldrh	r2, [r9, $r]
			DEC16	r2
			strh	r1, [r9, $r]
			mov		pc, r11
	MEND

	MACRO
$label	XCHG_AX		$r, $clk
$label		CPUWORK	$clk
			ldrh	r0, [r9, #CPU_AX]
			ldrh	r1, [r9, $r]
			strh	r0, [r9, $r]
			strh	r1, [r9, #CPU_AX]
			mov		pc, r11
	MEND


	MACRO
$label	MOVIMM8		$r, $clk
$label		CPUWORK	$clk
			GETPC8
			strb	r0, [r9, $r]
			mov		pc, r11
	MEND

	MACRO
$label	MOVIMM16	$r, $clk
$label		CPUWORK	$clk
			GETPC16
			strh	r0, [r9, $r]
			mov		pc, r11
	MEND


	MACRO
$label	SP_PUSH		$clk
$label		CPUWORK	$clk
			ldrh	r1, [r9, #CPU_SP]
			ldr		r2, [r9, #CPU_SS_BASE]
			sub		r0, r1, #2
			mov		r3, r0 lsl #16
			strh	r0, [r9, #CPU_SP]
			add		r0, r2, r3 lsr #16
			mov		lr, r11
			b		i286_memorywrite_w
	MEND

	MACRO
$label	SP_POP		$clk
$label		CPUWORK	$clk
			ldrh	r0, [r9, #CPU_SP]
			ldr		r1, [r9, #CPU_SS_BASE]
			add		r0, r1, r0
			bl		i286_memoryread_w
			strh	r0, [r9, #CPU_SP]
			mov		pc, r11
	MEND

	MACRO
$label	SEGPOP		$s, $b, $clk
$label		POP		$clk
			mov		r1, r0 lsl #4
			strh	r0, [r9, $s]
			str		r1, [r9, $b]
			mov		pc, r11
	MEND

	MACRO
$label	SEGPOPFIX	$s, $b, $f, $clk
$label		POP		$clk
			mov		r1, r0 lsl #4
			strh	r0, [r9, $s]
			str		r1, [r9, $b]
			str		r1, [r9, $f]
			mov		pc, r11
	MEND



	END

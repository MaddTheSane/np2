
I286_MEMREADMAX		equ		&a4000
I286_MEMWRITEMAX	equ		&a0000

C_FLAG				equ		&0001
P_FLAG				equ		&0004
A_FLAG				equ		&0010
Z_FLAG				equ		&0040
S_FLAG				equ		&0080
T_FLAG				equ		&0100
I_FLAG				equ		&0200
D_FLAG				equ		&0400
O_FLAG				equ		&0800

CPU_REG				equ		0 - 112
CPU_REMAINCLOCK		equ		28 - 112
; cpu_baseclock		equ		32 - 112
; cpu_clock			equ		36 - 112
; cpu_adrsmask		equ		40 - 112
CPU_ES_BASE			equ		44 - 112
CPU_CS_BASE			equ		48 - 112
CPU_SS_BASE			equ		52 - 112
CPU_DS_BASE			equ		56 - 112
CPU_SS_FIX			equ		60 - 112
CPU_DS_FIX			equ		64 - 112
; cpu_prefix		equ		68 - 112
CPU_TRAP			equ		70 - 112
; cpu_type			equ		71 - 112
; cpu_pf_semaphore	equ		72 - 112
; cpu_repbak		equ		76 - 112
; cpu_inport		equ		80 - 112
; cpu_ovflag		equ		84 - 112
CPU_GDTR			equ		88 - 112
CPU_IDTR			equ		94 - 112
; cpu_MSW			equ		100 - 112
; cpu_resetreq		equ		102 - 112
; cpu_itfbank		equ		103 - 112
; cpu_extmem		equ		104 - 112
; cpu_extmemsize	equ		108 - 112
; cpu_mainmem		equ		112 - 112

CPU_AL				equ		0 - 112
CPU_AH				equ		1 - 112
CPU_CL				equ		2 - 112
CPU_CH				equ		3 - 112
CPU_DL				equ		4 - 112
CPU_DH				equ		5 - 112
CPU_BL				equ		6 - 112
CPU_BH				equ		7 - 112

CPU_AX				equ		0 - 112
CPU_CX				equ		2 - 112
CPU_DX				equ		4 - 112
CPU_BX				equ		6 - 112
CPU_SP				equ		8 - 112
CPU_BP				equ		10 - 112
CPU_SI				equ		12 - 112
CPU_DI				equ		14 - 112
CPU_ES				equ		16 - 112
CPU_CS				equ		18 - 112
CPU_SS				equ		20 - 112
CPU_DS				equ		22 - 112
CPU_FLAG			equ		24 - 112
CPU_IP				equ		26 - 112


	MACRO
$label	CPUWORK	$clock
$label		ldr		r7, [r9, #CPU_REMAINCLOCK]
			sub		r7, r7, $clock
			str		r7, [r9, #CPU_REMAINCLOCK]
	MEND

	MACRO
$label	CREMSET	$clock
$label		mov		r7, $clock
			str		r7, [r9, #CPU_REMAINCLOCK]
	MEND

	MACRO
$label	CPUSV
$label
	MEND

	MACRO
$label	CPULD
$label
	MEND


	MACRO
$label	GETPC8
$label		ldr		r0, [r9, #CPU_CS_BASE]
			add		r0, r0, r8 lsr #16
			bl		i286_memoryread
			add		r8, r8, #(1 << 16)
	MEND

	MACRO
$label	GETPC16
$label		ldr		r0, [r9, #CPU_CS_BASE]
			add		r0, r0, r8 lsr #16
			bl		i286_memoryread_w
			add		r8, r8, #(2 << 16)
	MEND


	MACRO
$label	R8SRC	$op, $out
$label		and		$out, $op, #3
			add		$out, r9, $out lsl #1
			tst		$op, #(1 << 2)
			addne	$out, $out, #1
	MEND

	MACRO
$label	R8DST	$op, $out
$label		and		$out, $op, #(6 << 2)
			add		$out, r9, $out lsr #2
			tst		$op, #(1 << 5)
			addne	$out, $out, #1
	MEND

	MACRO
$label	EAREG8	$src
$label		ldr		r0, [r9, #CPU_CS_BASE]
			add		r0, r0, r8 lsr #16
			bl		i286_memoryread
			add		r8, r8, #(1 << 16)
			and		$src, r0, #(6 << 2)
			add		$src, r9, $src lsr #2
			tst		r0, #(1 << 5)
			addne	$src, $src, #1
	MEND

	MACRO
$label	REG8EA	$dst, $regclk, $memclk
$label		ldr		r0, [r9, #CPU_CS_BASE]
			add		r0, r0, r8 lsr #16
			bl		i286_memoryread
			add		r8, r8, #(1 << 16)
			R8DST	r0, $dst
			cmp		r0, #&c0
			bcc		$label.1
			CPUWORK	$regclk
			R8SRC	r0, r12
			ldrb	r0, [r12, #CPU_REG]
			b		$label.2
$label.1	CPUWORK	$memclk
			bl		i286a_ea
			bl		i286_memoryread
$label.2
	MEND


	MACRO
$label	R16SRC	$op, $out
$label		and		$out, $op, #7
			add		$out, r9, $out lsl #1
	MEND

	MACRO
$label	R16DST	$op, $out
$label		and		$out, $op, #(7 << 3)
			add		$out, r9, $out lsr #2
	MEND

	MACRO
$label	EAREG16	$src
$label		ldr		r0, [r9, #CPU_CS_BASE]
			add		r0, r0, r8 lsr #16
			bl		i286_memoryread
			add		r8, r8, #(1 << 16)
			and		$src, r0, #(7 << 3)
			add		$src, r9, $src lsr #2
	MEND

	MACRO
$label	REG16EA	$dst, $regclk, $memclk
$label		ldr		r0, [r9, #CPU_CS_BASE]
			add		r0, r0, r8 lsr #16
			bl		i286_memoryread
			add		r8, r8, #(1 << 16)
			R16DST	r0, $dst
			cmp		r0, #&c0
			bcc		$label.1
			CPUWORK	$regclk
			R16SRC	r0, r12
			ldrh	r0, [r12, #CPU_REG]
			b		$label.2
$label.1	CPUWORK	$memclk
			bl		i286a_ea
			bl		i286_memoryread_w
$label.2
	MEND


	END


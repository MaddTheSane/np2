
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
CPU_SEG				equ		16 - 112
CPU_REMAINCLOCK		equ		28 - 112
CPU_BASECLOCK		equ		32 - 112
; cpu_clock			equ		36 - 112
; cpu_adrsmask		equ		40 - 112
CPU_ES_BASE			equ		44 - 112
CPU_CS_BASE			equ		48 - 112
CPU_SS_BASE			equ		52 - 112
CPU_DS_BASE			equ		56 - 112
CPU_SS_FIX			equ		60 - 112
CPU_DS_FIX			equ		64 - 112
CPU_PREFIX			equ		68 - 112
CPU_TRAP			equ		70 - 112
; cpu_type			equ		71 - 112
; cpu_pf_semaphore	equ		72 - 112
; cpu_repbak		equ		76 - 112
CPU_INPUT			equ		80 - 112
; cpu_ovflag		equ		84 - 112
CPU_GDTR			equ		88 - 112
CPU_IDTR			equ		94 - 112
CPU_MSW				equ		100 - 112
; cpu_resetreq		equ		102 - 112
; cpu_itfbank		equ		103 - 112
CPU_EXTMEM			equ		104 - 112
CPU_EXTMEMSIZE		equ		108 - 112
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

MAX_PREFIX			equ		8


	MACRO
$label	CPUSVC
$label
	MEND

	MACRO
$label	CPULDC
$label
	MEND

	MACRO
$label	CPUSVF
$label		str		r8, [r9, #CPU_FLAG]
	MEND

	MACRO
$label	CPULDF
$label		ldr		r8, [r9, #CPU_FLAG]
	MEND

	MACRO
$label	CPUSV
$label	CPUSVF
		CPUSVC
	MEND

	MACRO
$label	CPULD
$label	CPULDF
		CPULDC
	MEND


	MACRO
$label	CPUWORK	$clock
$label		ldr		r7, [r9, #CPU_REMAINCLOCK]
			sub		r7, r7, $clock
			str		r7, [r9, #CPU_REMAINCLOCK]
	MEND

	MACRO
$label	CPUWKS	$clock
$label		ldr		r7, [r9, #CPU_REMAINCLOCK]
			subs	r7, r7, $clock
			str		r7, [r9, #CPU_REMAINCLOCK]
	MEND

	MACRO
$label	CREMSET	$clock
$label		mov		r7, $clock
			str		r7, [r9, #CPU_REMAINCLOCK]
	MEND

	MACRO
$label	NEXT_OPCODE
$label		ldr		r7, [r9, #CPU_REMAINCLOCK]
			cmp		r7, #1
			movge	pc, r11
			ldr		r0, [r9, #CPU_BASECLOCK]
			rsb		r1, r7, #1
			mov		r7, #1
			add		r0, r0, r1
			str		r0, [r9, #CPU_BASECLOCK]
			str		r7, [r9, #CPU_REMAINCLOCK]
			mov		pc, r11
	MEND

	MACRO
$label	REMAIN_ADJUST	$clk
$label		ldr		r7, [r9, #CPU_REMAINCLOCK]
			cmp		r7, $clk
			moveq	pc, r11
			ldr		r0, [r9, #CPU_BASECLOCK]
			rsb		r1, r7, $clk
			mov		r7, $clk
			add		r0, r0, r1
			str		r0, [r9, #CPU_BASECLOCK]
			str		r7, [r9, #CPU_REMAINCLOCK]
			mov		pc, r11
	MEND

	MACRO
$label	I286IRQCHECKTERM
$label		ldr		r7, [r9, #CPU_REMAINCLOCK]
			cmp		r7, #0
			movgt	pc, r11
			ldr		r0, [r9, #CPU_BASECLOCK]
			mov		r1, r7
			mov		r7, #0
			sub		r0, r0, r1
			str		r7, [r9, #CPU_REMAINCLOCK]
			str		r0, [r9, #CPU_BASECLOCK]
			mov		pc, r11
	MEND

	END



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

CPU_SIZE			equ		116
CPU_STAT			equ		(0 - CPU_SIZE)
CPU_EXT				equ		(104 - CPU_SIZE)

CPU_REG				equ		CPU_STAT + 0
CPU_SEG				equ		CPU_STAT + 16
CPU_REMAINCLOCK		equ		CPU_STAT + 28
CPU_BASECLOCK		equ		CPU_STAT + 32
; cpu_clock			equ		CPU_STAT + 36
CPU_ADRSMASK		equ		CPU_STAT + 40
CPU_ES_BASE			equ		CPU_STAT + 44
CPU_CS_BASE			equ		CPU_STAT + 48
CPU_SS_BASE			equ		CPU_STAT + 52
CPU_DS_BASE			equ		CPU_STAT + 56
CPU_SS_FIX			equ		CPU_STAT + 60
CPU_DS_FIX			equ		CPU_STAT + 64
CPU_PREFIX			equ		CPU_STAT + 68
CPU_TRAP			equ		CPU_STAT + 70
; cpu_type			equ		CPU_STAT + 71
; cpu_pf_semaphore	equ		CPU_STAT + 72
; cpu_repbak		equ		CPU_STAT + 76
CPU_INPUT			equ		CPU_STAT + 80
; cpu_ovflag		equ		CPU_STAT + 84
CPU_GDTR			equ		CPU_STAT + 88
CPU_IDTR			equ		CPU_STAT + 94
CPU_MSW				equ		CPU_STAT + 100
; cpu_resetreq		equ		CPU_STAT + 102
CPU_ITFBANK			equ		CPU_STAT + 103
; cpu_stat_size		equ		104

CPU_AL				equ		CPU_STAT + 0
CPU_AH				equ		CPU_STAT + 1
CPU_CL				equ		CPU_STAT + 2
CPU_CH				equ		CPU_STAT + 3
CPU_DL				equ		CPU_STAT + 4
CPU_DH				equ		CPU_STAT + 5
CPU_BL				equ		CPU_STAT + 6
CPU_BH				equ		CPU_STAT + 7

CPU_AX				equ		CPU_STAT + 0
CPU_CX				equ		CPU_STAT + 2
CPU_DX				equ		CPU_STAT + 4
CPU_BX				equ		CPU_STAT + 6
CPU_SP				equ		CPU_STAT + 8
CPU_BP				equ		CPU_STAT + 10
CPU_SI				equ		CPU_STAT + 12
CPU_DI				equ		CPU_STAT + 14
CPU_ES				equ		CPU_STAT + 16
CPU_CS				equ		CPU_STAT + 18
CPU_SS				equ		CPU_STAT + 20
CPU_DS				equ		CPU_STAT + 22
CPU_FLAG			equ		CPU_STAT + 24
CPU_IP				equ		CPU_STAT + 26


CPU_EXTMEM			equ		CPU_EXT + 0
CPU_EXTMEMSIZE		equ		CPU_EXT + 4
MEMWAIT_TRAM		equ		CPU_EXT + 8
MEMWAIT_VRAM		equ		CPU_EXT + 9
MEMWAIT_GRCG		equ		CPU_EXT + 10
; cpu_ext_size		equ		12

MAX_PREFIX			equ		8



	MACRO
$label	CPUDBGS
$label	;;	str		r7, [r9, #CPU_REMAINCLOCK]
	MEND

	MACRO
$label	CPUDBGL
$label	;;	ldr		r7, [r9, #CPU_REMAINCLOCK]
	MEND


	MACRO
$label	CPUSVC
$label		str		r7, [r9, #CPU_REMAINCLOCK]
	MEND

	MACRO
$label	CPULDC
$label		ldr		r7, [r9, #CPU_REMAINCLOCK]
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
$label		CPUDBGL
			sub		r7, r7, $clock
			CPUDBGS
	MEND

	MACRO
$label	CPUWKS	$clock
$label		CPUDBGL
			subs	r7, r7, $clock
			CPUDBGS
	MEND

	MACRO
$label	CREMSET	$clock
$label		mov		r7, $clock
			CPUDBGS
	MEND

	MACRO
$label	NEXT_OPCODE
$label		CPUDBGL
			cmp		r7, #1
			movge	pc, r11
			ldr		r0, [r9, #CPU_BASECLOCK]
			rsb		r1, r7, #1
			mov		r7, #1
			add		r0, r0, r1
			str		r0, [r9, #CPU_BASECLOCK]
			CPUDBGS
			mov		pc, r11
	MEND

	MACRO
$label	REMAIN_ADJUST	$clk
$label		CPUDBGL
			cmp		r7, $clk
			moveq	pc, r11
			ldr		r0, [r9, #CPU_BASECLOCK]
			rsb		r1, r7, $clk
			mov		r7, $clk
			add		r0, r0, r1
			str		r0, [r9, #CPU_BASECLOCK]
			CPUDBGS
			mov		pc, r11
	MEND

	MACRO
$label	I286IRQCHECKTERM
$label		CPUDBGL
			cmp		r7, #0
			movle	pc, r11
			ldr		r0, [r9, #CPU_BASECLOCK]
			mov		r1, r7
			mov		r7, #0
			sub		r0, r0, r1
			str		r0, [r9, #CPU_BASECLOCK]
			CPUDBGS
			mov		pc, r11
	MEND


	MACRO
$label	WORDACC	$r, $l
$label		tst		$r, #1
			bne		$l
			cmp		$r, #I286_MEMWRITEMAX
			bcs		$l
	MEND

	END



	INCLUDE		i286a.inc
	INCLUDE		i286aalu.inc
	INCLUDE		i286aop.inc

	IMPORT		i286core
	IMPORT		_szpcflag8
	IMPORT		i286a_ea
	IMPORT		i286a_lea
	IMPORT		i286_memoryread
	IMPORT		i286_memoryread_w
	IMPORT		i286_memorywrite
	IMPORT		i286_memorywrite_w
	IMPORT		iocore_inp8
	IMPORT		iocore_inp16
	IMPORT		iocore_out8
	IMPORT		iocore_out16
	IMPORT		i286a_localint

	EXPORT		i286a_step
	EXPORT		optbl1

	AREA	.text, CODE, READONLY

add_ea_r8		OP_EA_R8	ADD8, #2, #7
add_ea_r16		OP_EA_R16	ADD16, #2, #7
add_r8_ea		OP_R8_EA	ADD8, #2, #7
add_r16_ea		OP_R16_EA	ADD16, #2, #7
add_al_d8		OP_AL_D8	ADD8, #3
add_ax_d16		OP_AX_D16	ADD16, #3
push_es			REGPUSH		#CPU_ES, #3
pop_es			SEGPOP		#CPU_ES, #CPU_ES_BASE, #5

or_ea_r8		OP_EA_R8	OR8, #2, #7
or_ea_r16		OP_EA_R16	OR16, #2, #7
or_r8_ea		OP_R8_EA	OR8, #2, #7
or_r16_ea		OP_R16_EA	OR16, #2, #7
or_al_d8		OP_AL_D8	OR8, #3
or_ax_d16		OP_AX_D16	OR16, #3
push_cs			REGPUSH		#CPU_CS, #3
; ope0f

adc_ea_r8		OP_EA_R8	ADC8, #2, #7
adc_ea_r16		OP_EA_R16	ADC16, #2, #7
adc_r8_ea		OP_R8_EA	ADC8, #2, #7
adc_r16_ea		OP_R16_EA	ADC16, #2, #7
adc_al_d8		OP_AL_D8	ADC8, #3
adc_ax_d16		OP_AX_D16	ADC16, #3
push_ss			REGPUSH		#CPU_SS, #3
pop_ss			SEGPOPFIX	#CPU_SS, #CPU_SS_BASE, #CPU_SS_FIX, #5

sbb_ea_r8		OP_EA_R8	SBB8, #2, #7
sbb_ea_r16		OP_EA_R16	SBB16, #2, #7
sbb_r8_ea		OP_R8_EA	SBB8, #2, #7
sbb_r16_ea		OP_R16_EA	SBB16, #2, #7
sbb_al_d8		OP_AL_D8	SBB8, #3
sbb_ax_d16		OP_AX_D16	SBB16, #3
push_ds			REGPUSH		#CPU_DS, #3
pop_ds			SEGPOPFIX	#CPU_DS, #CPU_DS_BASE, #CPU_DS_FIX, #5

and_ea_r8		OP_EA_R8	AND8, #2, #7
and_ea_r16		OP_EA_R16	AND16, #2, #7
and_r8_ea		OP_R8_EA	AND8, #2, #7
and_r16_ea		OP_R16_EA	AND16, #2, #7
and_al_d8		OP_AL_D8	AND8, #3
and_ax_d16		OP_AX_D16	AND16, #3
; segprefix_es		!
; daa			*

sub_ea_r8		OP_EA_R8	SUB8, #2, #7
sub_ea_r16		OP_EA_R16	SUB16, #2, #7
sub_r8_ea		OP_R8_EA	SUB8, #2, #7
sub_r16_ea		OP_R16_EA	SUB16, #2, #7
sub_al_d8		OP_AL_D8	SUB8, #3
sub_ax_d16		OP_AX_D16	SUB16, #3
; segprefix_cs		!
; das			*

xor_ea_r8		OP_EA_R8	XOR8, #2, #7
xor_ea_r16		OP_EA_R16	XOR16, #2, #7
xor_r8_ea		OP_R8_EA	XOR8, #2, #7
xor_r16_ea		OP_R16_EA	XOR16, #2, #7
xor_al_d8		OP_AL_D8	XOR8, #3
xor_ax_d16		OP_AX_D16	XOR16, #3
; segprefix_ss		!
; aaa			*

cmp_ea_r8		S_EA_R8		SUB8, #2, #7
cmp_ea_r16		S_EA_R16	SUB16, #2, #7
cmp_r8_ea		S_R8_EA		SUB8, #2, #6
cmp_r16_ea		S_R16_EA	SUB16, #2, #6
cmp_al_d8		S_AL_D8		SUB8, #3
cmp_ax_d16		S_AX_D16	SUB16, #3
; segprefix_ds		!
; aas			*

inc_ax			OP_INC16	#CPU_AX, #2
inc_cx			OP_INC16	#CPU_CX, #2
inc_dx			OP_INC16	#CPU_DX, #2
inc_bx			OP_INC16	#CPU_BX, #2
inc_sp			OP_INC16	#CPU_SP, #2
inc_bp			OP_INC16	#CPU_BP, #2
inc_si			OP_INC16	#CPU_SI, #2
inc_di			OP_INC16	#CPU_DI, #2
dec_ax			OP_DEC16	#CPU_AX, #2
dec_cx			OP_DEC16	#CPU_CX, #2
dec_dx			OP_DEC16	#CPU_DX, #2
dec_bx			OP_DEC16	#CPU_BX, #2
dec_sp			OP_DEC16	#CPU_SP, #2
dec_bp			OP_DEC16	#CPU_BP, #2
dec_si			OP_DEC16	#CPU_SI, #2
dec_di			OP_DEC16	#CPU_DI, #2

push_ax			REGPUSH		#CPU_AX, #3
push_cx			REGPUSH		#CPU_CX, #3
push_dx			REGPUSH		#CPU_DX, #3
push_bx			REGPUSH		#CPU_BX, #3
push_sp			SP_PUSH		#3
push_bp			REGPUSH		#CPU_BP, #3
push_si			REGPUSH		#CPU_SI, #3
push_di			REGPUSH		#CPU_DI, #3
pop_ax			REGPOP		#CPU_AX, #5
pop_cx			REGPOP		#CPU_CX, #5
pop_dx			REGPOP		#CPU_DX, #5
pop_bx			REGPOP		#CPU_BX, #5
pop_sp			SP_POP		#5
pop_bp			REGPOP		#CPU_BP, #5
pop_si			REGPOP		#CPU_SI, #5
pop_di			REGPOP		#CPU_DI, #5

; pusha			*
; popa			*
; bound
; arpl
; push_d16		*
; imul_r_ea_d16	+
; push_d8		*
; imul_r_ea_d8	+
; insb			*
; insw			*
; outsb			*
; outsw			*

jo_short		JMPNE		#O_FLAG, #2, #7
jno_short		JMPEQ		#O_FLAG, #2, #7
jc_short		JMPNE		#C_FLAG, #2, #7
jnc_short		JMPEQ		#C_FLAG, #2, #7
jz_short		JMPNE		#Z_FLAG, #2, #7
jnz_short		JMPEQ		#Z_FLAG, #2, #7
jna_short		JMPNE		#(Z_FLAG + C_FLAG), #2, #7
ja_short		JMPEQ		#(Z_FLAG + C_FLAG), #2, #7
js_short		JMPNE		#S_FLAG, #2, #7
jns_short		JMPEQ		#S_FLAG, #2, #7
jp_short		JMPNE		#P_FLAG, #2, #7
jnp_short		JMPEQ		#P_FLAG, #2, #7
; jl_short		+
; jnl_short		+
; jle_short		+
; jnle_short	+

; calc_ea8_i8	+
; calc_ea16_i16	+
; calc_ea16_i8	+
test_ea_r8		S_EA_R8		AND8, #2, #6
test_ea_r16		S_EA_R16	AND16, #2, #6
; xchg_ea_r8	*
; xchg_ea_r16	*
; mov_ea_r8		*
; mov_ea_r16	*
; mov_r8_ea		*
; mov_r16_ea	*
; mov_ea_seg	+
; lea_r16_ea	+
; mov_seg_ea		!
; pop_ea		*

; nop
xchg_ax_cx		XCHG_AX		#CPU_CX, #3
xchg_ax_dx		XCHG_AX		#CPU_DX, #3
xchg_ax_bx		XCHG_AX		#CPU_BX, #3
xchg_ax_sp		XCHG_AX		#CPU_SP, #3
xchg_ax_bp		XCHG_AX		#CPU_BP, #3
xchg_ax_si		XCHG_AX		#CPU_SI, #3
xchg_ax_di		XCHG_AX		#CPU_DI, #3
; cbw			*
; cwd			*
; call_far		*
; wait			*
; pushf			*
; popf				!
; sahf			*
; lahf			*

; mov_al_m8		*
; mov_ax_m16	*
; mov_m8_al		*
; mov_m16_ax	*
; movsb			*
; movsw			*
; cmpsb			*
; cmpsw			*
test_al_d8		S_AL_D8		AND8, #3
test_ax_d16		S_AX_D16	AND16, #3
; stosb			*
; stosw			*
; lodsb			*
; lodsw			*
; scasb			*
; scasw			*

mov_al_imm		MOVIMM8		#CPU_AL, #2
mov_cl_imm		MOVIMM8		#CPU_CL, #2
mov_dl_imm		MOVIMM8		#CPU_DL, #2
mov_bl_imm		MOVIMM8		#CPU_BL, #2
mov_ah_imm		MOVIMM8		#CPU_AH, #2
mov_ch_imm		MOVIMM8		#CPU_CH, #2
mov_dh_imm		MOVIMM8		#CPU_DH, #2
mov_bh_imm		MOVIMM8		#CPU_BH, #2
mov_ax_imm		MOVIMM16	#CPU_AX, #2
mov_cx_imm		MOVIMM16	#CPU_CX, #2
mov_dx_imm		MOVIMM16	#CPU_DX, #2
mov_bx_imm		MOVIMM16	#CPU_BX, #2
mov_sp_imm		MOVIMM16	#CPU_SP, #2
mov_bp_imm		MOVIMM16	#CPU_BP, #2
mov_si_imm		MOVIMM16	#CPU_SI, #2
mov_di_imm		MOVIMM16	#CPU_DI, #2

; shift_ea8_d8
; shift_ea16_d8
; ret_near_d16	+
; ret_near		+
; les_r16_ea
; lds_r16_ea
; mov_ea8_d8	*
; mov_ea16_d16	*
; enter
; leave			+
; ret_far_d16	+
; ret_far		+
; int_03		+
; int_d8		+
; into			+
; iret				!

; shift_ea8_1
; shift_ea16_1
; shift_ea8_cl
; shift_ea16_cl
; aam			+
; aad			*
; setalc		*
; xlat			*
; esc			*

; loopnz		*
; loopz			*
; loop			*
; jcxz			*
; in_al_d8		*
; in_ax_d8		*
; out_d8_al		*
; out_d8_ax		*
; call_near		*
; jmp_near		*
; jmp_far		*
jmp_short		JMPS	#7
; in_al_dx		*
; in_ax_dx		*
; out_dx_al		*
; out_dx_ax		*

; lock			*
; repne				!
; repe				!
; hlt			+
; cmc			*
; ope0xf6
; ope0xf7
; clc			*
; stc			*
; cli			*
; sti				!
; cld			*
; std			*
; ope0xfe
; ope0xff


; ----

reserved		mov		r6, #6
				sub		r8, r8, #(1 << 16)
				b		i286a_localint

daa				CPUWORK	#3
				ldrb	r0, [r9, #CPU_AL]
				bic		r8, r8, #O_FLAG
				eor		r2, r0, #&80
				tst		r8, #A_FLAG
				bne		daalo2
				and		r1, r0, #&0f
				cmp		r1, #10
				bcc		daahi
				orr		r8, r8, #A_FLAG
daalo2			add		r0, r0, #6
				orr		r8, r8, r0 lsr #8
				and		r0, r0, #&ff
daahi			tst		r8, #C_FLAG
				bne		daahi2
				cmp		r0, #&a0
				bcc		daaflg
				orr		r8, r8, #C_FLAG
daahi2			add		r0, r0, #&60
				and		r0, r0, #&ff
daaflg			strb	r0, [r9, #CPU_AL]
				ldrb	r1, [r10, r0]
				bic		r8, r8, #(&ff - A_FLAG - C_FLAG)
				and		r2, r0, r2
				orr		r8, r1, r8
				tst		r2, #&80
				addne	r8, r8, #O_FLAG
				mov		pc, r11

das				CPUWORK	#3
				ldrb	r0, [r9, #CPU_AL]
				tst		r8, #C_FLAG
				bne		dashi2
				cmp		r0, #&9a
				bcc		daslo
				orr		r8, r8, #C_FLAG
dashi2			sub		r0, r0, #&60
				and		r0, r0, #&ff
daslo			tst		r8, #A_FLAG
				bne		daslo2
				and		r1, r0, #&0f
				cmp		r1, #10
				bcc		dasflg
				orr		r8, r8, #A_FLAG
daslo2			sub		r0, r0, #6
				orr		r8, r8, r0 lsr #31
				and		r0, r0, #&ff
dasflg			strb	r0, [r9, #CPU_AL]
				ldrb	r1, [r10, r0]
				bic		r8, r8, #(&ff - A_FLAG - C_FLAG)
				orr		r8, r1, r8
				mov		pc, r11

aaa				CPUWORK	#3
				ldrh	r0, [r9, #CPU_AX]
				tst		r8, #A_FLAG
				bic		r8, r8, #(A_FLAG + C_FLAG)
				bne		aaa1
				and		r1, r0, #&f
				cmp		r1, #10
				bcc		aaa2
aaa1			orr		r8, r8, #(A_FLAG + C_FLAG)
				add		r0, r0, #6
				add		r0, r0, #&100
aaa2			bic		r0, r0, #&f0
				strh	r0, [r9, #CPU_AX]
				mov		pc, r11

aas				CPUWORK	#3
				ldrh	r0, [r9, #CPU_AX]
				tst		r8, #A_FLAG
				bic		r8, r8, #(A_FLAG + C_FLAG)
				bne		aas1
				and		r1, r0, #&f
				cmp		r1, #10
				movcc	pc, r11
aas1			orr		r8, r8, #(A_FLAG + C_FLAG)
				sub		r0, r0, #6
				sub		r0, r0, #&100
				strh	r0, [r9, #CPU_AX]
				mov		pc, r11


pusha			ldrh	r4, [r9, #CPU_SP]
				ldr		r5, [r9, #CPU_SS_BASE]
				mov		r6, r4
				mov		r4, r4 lsl #16
				ldrh	r1, [r9, #CPU_AX]
				sub		r4, r4, #(2 << 16)
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				ldrh	r1, [r9, #CPU_CX]
				sub		r4, r4, #(2 << 16)
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				ldrh	r1, [r9, #CPU_DX]
				sub		r4, r4, #(2 << 16)
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				ldrh	r1, [r9, #CPU_BX]
				sub		r4, r4, #(2 << 16)
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				mov		r1, r6
				sub		r4, r4, #(2 << 16)
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				ldrh	r1, [r9, #CPU_BP]
				sub		r4, r4, #(2 << 16)
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				ldrh	r1, [r9, #CPU_SI]
				sub		r4, r4, #(2 << 16)
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				ldrh	r1, [r9, #CPU_DI]
				sub		r4, r4, #(2 << 16)
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				mov		r0, r4 lsr #16
				strh	r0, [r9, #CPU_SP]
				mov		pc, r11

popa			ldrh	r4, [r9, #CPU_SP]
				ldr		r5, [r9, #CPU_SS_BASE]
				add		r0, r5, r4
				mov		r4, r4 lsl #16
				bl		i286_memoryread_w
				strh	r0, [r9, #CPU_DI]
				add		r4, r4, #(2 << 16)
				add		r0, r5, r4 lsr #16
				bl		i286_memoryread_w
				strh	r0, [r9, #CPU_SI]
				add		r4, r4, #(2 << 16)
				add		r0, r5, r4 lsr #16
				bl		i286_memoryread_w
				strh	r0, [r9, #CPU_BP]
				add		r4, r4, #(4 << 16)
				add		r0, r5, r4 lsr #16
				bl		i286_memoryread_w
				strh	r0, [r9, #CPU_BX]
				add		r4, r4, #(2 << 16)
				add		r0, r5, r4 lsr #16
				bl		i286_memoryread_w
				strh	r0, [r9, #CPU_DX]
				add		r4, r4, #(2 << 16)
				add		r0, r5, r4 lsr #16
				bl		i286_memoryread_w
				strh	r0, [r9, #CPU_CX]
				add		r4, r4, #(2 << 16)
				add		r0, r5, r4 lsr #16
				bl		i286_memoryread_w
				strh	r0, [r9, #CPU_AX]
				add		r4, r4, #(2 << 16)
				mov		r0, r4 lsr #16
				strh	r0, [r9, #CPU_SP]
				mov		pc, r11

push_d16		CPUWORK	#3
				GETPC16
				ldrh	r2, [r9, #CPU_SP]
				ldr		r3, [r9, #CPU_SS_BASE]
				subs	r2, r2, #2
				addcc	r2, r2, #&10000
				mov		r1, r0
				strh	r2, [r9, #CPU_SP]
				add		r0, r2, r3
				mov		lr, r11
				b		i286_memorywrite_w

imul_r_ea_d16	REG16EA	r5, #21, #24
				mov		r4, r0, lsl #16
				GETPC16
				mov		r0, r0, lsl #16
				mov		r4, r4, asr #16
				mov		r0, r0, asr #16
				mul		r1, r0, r4
				add		r12, r1, #&8000
				strh	r1, [r5, #CPU_REG]
				movs	r12, r12 lsr #16
				biceq	r8, r8, #O_FLAG
				biceq	r8, r8, #C_FLAG
				orrne	r8, r8, #O_FLAG
				orrne	r8, r8, #C_FLAG
				mov		pc, r11

push_d8			CPUWORK	#3
				GETPC8
				ldrh	r2, [r9, #CPU_SP]
				ldr		r3, [r9, #CPU_SS_BASE]
				subs	r2, r2, #2
				addcc	r2, r2, #&10000
				mov		r0, r0 lsl #24
				mov		r1, r0 asr #24
				strh	r2, [r9, #CPU_SP]
				add		r0, r2, r3
				mov		lr, r11
				b		i286_memorywrite_w

imul_r_ea_d8	REG16EA	r5, #21, #24
				mov		r4, r0, lsl #16
				GETPC8
				mov		r0, r0, lsl #24
				mov		r4, r4, asr #16
				mov		r0, r0, asr #24
				mul		r1, r0, r4
				add		r12, r1, #&8000
				strh	r1, [r5, #CPU_REG]
				movs	r12, r12 lsr #16
				biceq	r8, r8, #O_FLAG
				biceq	r8, r8, #C_FLAG
				orrne	r8, r8, #O_FLAG
				orrne	r8, r8, #C_FLAG
				mov		pc, r11

insb			ldrh	r0, [r9, #CPU_DX]
				bl		iocore_inp8
				ldrh	r2, [r9, #CPU_DI]
				ldr		r3, [r9, #CPU_ES_BASE]
				mov		r1, r0
				add		r0, r2, r3
				tst		r8, #D_FLAG
				addeq	r2, r2, #1
				subne	r2, r2, #1
				mov		lr, r11
				strh	r2, [r9, #CPU_DI]
				b		i286_memorywrite

insw			ldrh	r0, [r9, #CPU_DX]
				bl		iocore_inp16
				ldrh	r2, [r9, #CPU_DI]
				ldr		r3, [r9, #CPU_ES_BASE]
				mov		r1, r0
				add		r0, r2, r3
				tst		r8, #D_FLAG
				addeq	r2, r2, #2
				subne	r2, r2, #2
				mov		lr, r11
				strh	r2, [r9, #CPU_DI]
				b		i286_memorywrite_w

outsb			ldrh	r1, [r9, #CPU_SI]
				ldr		r2, [r9, #CPU_DS_FIX]
				add		r0, r1, r2
				tst		r8, #D_FLAG
				addeq	r1, r1, #1
				subne	r1, r1, #1
				strh	r1, [r9, #CPU_SI]
				bl		i286_memoryread
				mov		r1, r0
				ldr		r0, [r9, #CPU_DX]
				mov		lr, r11
				b		iocore_out8

outsw			ldrh	r1, [r9, #CPU_SI]
				ldr		r2, [r9, #CPU_DS_FIX]
				add		r0, r1, r2
				tst		r8, #D_FLAG
				addeq	r1, r1, #2
				subne	r1, r1, #2
				strh	r1, [r9, #CPU_SI]
				bl		i286_memoryread_w
				mov		r1, r0
				ldr		r0, [r9, #CPU_DX]
				mov		lr, r11
				b		iocore_out16

jle_short		tst		r8, #Z_FLAG
				bne		jmps
jl_short		eor		r0, r8, r8 lsr #4
				tst		r0, #S_FLAG
				bne		jmps
nojmps			CPUWORK	#2
				add		r8, r8, #(1 << 16)
				mov		pc, r11

jnle_short		tst		r8, #Z_FLAG
				bne		jmps
jnl_short		eor		r0, r8, r8 lsr #4
				tst		r0, #S_FLAG
				bne		nojmps
jmps			JMPS	#7


xchg_ea_r8		EAREG8	r6
				cmp		r0, #&c0
				bcc		xchgear8_1
				CPUWORK	#3
				R8SRC	r0, r5
				ldrb	r0, [r6, #CPU_REG]
				ldrb	r1, [r5, #CPU_REG]
				strb	r0, [r5, #CPU_REG]
				strb	r1, [r6, #CPU_REG]
				mov		pc, r11
xchgear8_1		CPUWORK	#5
				bl		i286a_ea
				cmp		r0, #I286_MEMWRITEMAX
				bcs		xchgear8_2
				ldrb	r1, [r6, #CPU_REG]
				ldrb	r4, [r9, r0]
				strb	r1, [r9, r0]
				strb	r4, [r6, #CPU_REG]
				mov		pc, r11
xchgear8_2		mov		r5, r0
				bl		i286_memoryread
				ldrb	r1, [r6, #CPU_REG]
				strb	r0, [r6, #CPU_REG]
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite

xchg_ea_r16		EAREG16	r6
				cmp		r0, #&c0
				bcc		xchgear16_1
				CPUWORK	#3
				R16SRC	r0, r5
				ldrh	r0, [r6, #CPU_REG]
				ldrh	r1, [r5, #CPU_REG]
				strh	r0, [r5, #CPU_REG]
				strh	r1, [r6, #CPU_REG]
				mov		pc, r11
xchgear16_1		CPUWORK	#5
				bl		i286a_ea
				tst		r0, #1
				bne		xchgear16_2
				cmp		r0, #I286_MEMWRITEMAX
				bcs		xchgear16_2
				ldrh	r1, [r6, #CPU_REG]
				ldrh	r4, [r9, r0]
				strh	r1, [r9, r0]
				strh	r4, [r6, #CPU_REG]
				mov		pc, r11
xchgear16_2		mov		r5, r0
				bl		i286_memoryread_w
				ldrh	r1, [r6, #CPU_REG]
				strh	r0, [r6, #CPU_REG]
				mov		r0, r5
				mov		lr, r11
				b		i286_memorywrite_w

mov_ea_r8		EAREG8	r6
				cmp		r0, #&c0
				bcc		movear8_1
				CPUWORK	#3
				R8SRC	r0, r5
				ldrb	r0, [r6, #CPU_REG]
				strb	r0, [r5, #CPU_REG]
				mov		pc, r11
movear8_1		CPUWORK	#5
				bl		i286a_ea
				ldrb	r1, [r6, #CPU_REG]
				mov		lr, r11
				b		i286_memorywrite

mov_ea_r16		EAREG16	r6
				cmp		r0, #&c0
				bcc		movear16_1
				CPUWORK	#3
				R16SRC	r0, r5
				ldrh	r0, [r6, #CPU_REG]
				strh	r0, [r5, #CPU_REG]
				mov		pc, r11
movear16_1		CPUWORK	#5
				bl		i286a_ea
				ldrh	r1, [r6, #CPU_REG]
				mov		lr, r11
				b		i286_memorywrite_w

mov_r8_ea		REG8EA	r5, #2, #5
				ldrb	r0, [r5, #CPU_REG]
				mov		pc, r11

mov_r16_ea		REG16EA	r5, #2, #5
				ldrh	r0, [r5, #CPU_REG]
				mov		pc, r11

mov_ea_seg		GETPC8
				and		r1, r0, #(3 << 3)
				add		r1, r9, r1 lsr #2
				ldrh	r5, [r1, #CPU_REG]
				cmp		r0, #&c0
				bcc		measegm
				CPUWORK	#2
				R16DST	r0, r4
				strh	r5, [r4, #CPU_REG]
				mov		pc, r11
measegm			CPUWORK	#3
				bl		i286a_ea
				mov		r1, r5
				mov		lr, r11
				b		i286_memorywrite_w

lea_r16_ea		CPUWORK	#3
				GETPC8
				cmp		r0, #&c0
				bcs		leareg
				R8DST	r0, r5
				bl		i286a_lea
				strh	r0, [r5, #CPU_REG]
				mov		pc, r11
leareg			mov		r6, #6
				sub		r8, r8, #(2 << 16)
				b		i286a_localint


pop_ea			POP		#5
				mov		r4, r0
				GETPC8
				cmp		r0, #&c0
				bcs		popreg
				bl		i286a_ea
				mov		r1, r4
				mov		lr, r11
				b		i286_memorywrite_w
popreg			R16DST	r0, r1
				ldrh	r4, [r1, #CPU_REG]
				mov		pc, r11


cbw				CPUWORK	#2
				ldrb	r0, [r9, #CPU_AL]
				mov		r1, r0 lsl #24
				mov		r0, r1 asr #31
				strb	r0, [r9, #CPU_AH]
				mov		pc, r11

cwd				CPUWORK	#2
				ldrb	r0, [r9, #CPU_AH]
				mov		r1, r0 lsl #24
				mov		r0, r1 asr #31
				strh	r0, [r9, #CPU_DX]
				mov		pc, r11

call_far		CPUWORK	#13
				ldrh	r1, [r9, #CPU_CS]
				ldrh	r4, [r9, #CPU_SP]
				ldr		r5, [r9, #CPU_SS_BASE]
				mov		r4, r4 lsl #16
				sub		r4, r4, #(2 << 16)
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				add		r12, r8, #(4 << 16)
				sub		r4, r4, #(2 << 16)
				mov		r1, r12 lsr #16
				add		r0, r5, r4 lsr #16
				bl		i286_memorywrite_w
				ldr		r5, [r9, #CPU_CS_BASE]
				strh	r4, [r9, #CPU_SP]
				add		r0, r5, r8 lsr #16
				bl		i286_memoryread_w
				add		r8, r8, #(2 << 16)
				mov		r4, r0 lsl #16
				add		r0, r5, r8 lsr #16
				bl		i286_memoryread_w
				mov		r1, r0 lsl #4
				strh	r0, [r9, #CPU_CS]
				str		r1, [r9, #CPU_CS_BASE]
				mov		r0, r8 lsl #16
				orr		r8, r4, r0 lsr #16
				mov		pc, r11

wait			CPUWORK	#2
				mov		pc, r11

pushf			CPUWORK	#3
				ldrh	r1, [r9, #CPU_SP]
				ldr		r2, [r9, #CPU_SS_BASE]
				sub		r0, r1, #2
				mov		r3, r0 lsl #16
				strh	r0, [r9, #CPU_SP]
				add		r0, r2, r3 lsr #16
				mov		r1, r8
				mov		lr, r11
				b		i286_memorywrite_w

sahf			CPUWORK	#2
				ldrb	r0, [r9, #CPU_AH]
				bic		r8, r8, #&ff
				orr		r8, r0, r8
				mov		pc, r11

lahf			CPUWORK	#2
				strb	r8, [r9, #CPU_AH]
				mov		pc, r11


mov_al_m8		CPUWORK	#5
				ldr		r5, [r9, #CPU_DS_FIX]
				GETPC16
				add		r0, r5, r0
				bl		i286_memoryread
				strb	r0, [r9, #CPU_AL]
				mov		pc, r11

mov_ax_m16		CPUWORK	#5
				ldr		r5, [r9, #CPU_DS_FIX]
				GETPC16
				add		r0, r5, r0
				bl		i286_memoryread_w
				strh	r0, [r9, #CPU_AX]
				mov		pc, r11

mov_m8_al		CPUWORK	#5
				ldr		r5, [r9, #CPU_DS_FIX]
				GETPC16
				ldrb	r1, [r9, #CPU_AL]
				add		r0, r5, r0
				mov		lr, r11
				b		i286_memorywrite

mov_m16_ax		CPUWORK	#5
				ldr		r5, [r9, #CPU_DS_FIX]
				GETPC16
				ldrh	r1, [r9, #CPU_AX]
				add		r0, r5, r0
				mov		lr, r11
				b		i286_memorywrite_w

movsb			CPUWORK	#5
				ldr		r0, [r9, #CPU_DS_FIX]
				ldrh	r5, [r9, #CPU_SI]
				add		r0, r5, r0
				bl		i286_memoryread
				ldr		r2, [r9, #CPU_ES_BASE]
				ldrh	r3, [r9, #CPU_DI]
				and		r12, r8, #D_FLAG
				mov		r1, r0
				mov		r12, r12 lsr #(10 - 1)
				add		r0, r2, r3
				add		r2, r12, #-1
				mov		lr, r11
				add		r5, r2, r5
				add		r3, r2, r3
				strh	r5, [r9, #CPU_SI]
				strh	r3, [r9, #CPU_DI]
				b		i286_memorywrite

movsw			CPUWORK	#5
				ldr		r0, [r9, #CPU_DS_FIX]
				ldrh	r5, [r9, #CPU_SI]
				add		r0, r5, r0
				bl		i286_memoryread_w
				ldr		r2, [r9, #CPU_ES_BASE]
				ldrh	r3, [r9, #CPU_DI]
				and		r12, r8, #D_FLAG
				mov		r1, r0
				mov		r12, r12 lsr #(10 - 2)
				add		r0, r2, r3
				add		r2, r12, #-2
				mov		lr, r11
				add		r5, r2, r5
				add		r3, r2, r3
				strh	r5, [r9, #CPU_SI]
				strh	r3, [r9, #CPU_DI]
				b		i286_memorywrite_w

cmpsb			CPUWORK	#8
				ldr		r0, [r9, #CPU_DS_FIX]
				ldrh	r5, [r9, #CPU_SI]
				add		r0, r5, r0
				bl		i286_memoryread
				ldr		r2, [r9, #CPU_ES_BASE]
				ldrh	r3, [r9, #CPU_DI]
				mov		r6, r0
				and		r12, r8, #D_FLAG
				mov		r12, r12 lsr #(10 - 1)
				add		r0, r2, r3
				add		r2, r12, #-1
				add		r5, r2, r5
				add		r3, r2, r3
				strh	r5, [r9, #CPU_SI]
				strh	r3, [r9, #CPU_DI]
				bl		i286_memoryread
				SUB8	r6, r0
				mov		pc, r11

cmpsw			CPUWORK	#8
				ldr		r0, [r9, #CPU_DS_FIX]
				ldrh	r5, [r9, #CPU_SI]
				add		r0, r5, r0
				bl		i286_memoryread_w
				ldr		r2, [r9, #CPU_ES_BASE]
				ldrh	r3, [r9, #CPU_DI]
				mov		r6, r0
				and		r12, r8, #D_FLAG
				mov		r12, r12 lsr #(10 - 2)
				add		r0, r2, r3
				add		r2, r12, #-2
				add		r5, r2, r5
				add		r3, r2, r3
				strh	r5, [r9, #CPU_SI]
				strh	r3, [r9, #CPU_DI]
				bl		i286_memoryread_w
				SUB16	r6, r0
				mov		pc, r11

stosb			CPUWORK	#3
				ldr		r0, [r9, #CPU_ES_BASE]
				ldrb	r1, [r9, #CPU_AL]
				ldrh	r2, [r9, #CPU_DI]
				add		r0, r2, r0
				tst		r8, #D_FLAG
				addeq	r2, r2, #1
				subne	r2, r2, #1
				strh	r2, [r9, #CPU_DI]
				mov		lr, r11
				b		i286_memorywrite

stosw			CPUWORK	#3
				ldr		r0, [r9, #CPU_ES_BASE]
				ldrh	r1, [r9, #CPU_AX]
				ldrh	r2, [r9, #CPU_DI]
				add		r0, r2, r0
				tst		r8, #D_FLAG
				addeq	r2, r2, #2
				subne	r2, r2, #2
				strh	r2, [r9, #CPU_DI]
				mov		lr, r11
				b		i286_memorywrite_w

lodsb			CPUWORK	#5
				ldr		r0, [r9, #CPU_DS_FIX]
				ldrh	r5, [r9, #CPU_SI]
				add		r0, r5, r0
				bl		i286_memoryread
				tst		r8, #D_FLAG
				addeq	r5, r5, #1
				subne	r5, r5, #1
				strb	r0, [r9, #CPU_AL]
				strh	r5, [r9, #CPU_SI]
				mov		pc, r11

lodsw			CPUWORK	#5
				ldr		r0, [r9, #CPU_DS_FIX]
				ldrh	r5, [r9, #CPU_SI]
				add		r0, r5, r0
				bl		i286_memoryread_w
				tst		r8, #D_FLAG
				addeq	r5, r5, #2
				subne	r5, r5, #2
				strh	r0, [r9, #CPU_AX]
				strh	r5, [r9, #CPU_SI]
				mov		pc, r11

scasb			CPUWORK	#7
				ldr		r0, [r9, #CPU_DS_FIX]
				ldrh	r5, [r9, #CPU_SI]
				add		r0, r5, r0
				bl		i286_memoryread
				tst		r8, #D_FLAG
				addeq	r5, r5, #1
				subne	r5, r5, #1
				strh	r5, [r9, #CPU_SI]
				ldrb	r5, [r9, #CPU_AL]
				SUB8	r5, r0
				mov		pc, r11

scasw			CPUWORK	#7
				ldr		r0, [r9, #CPU_DS_FIX]
				ldrh	r5, [r9, #CPU_SI]
				add		r0, r5, r0
				bl		i286_memoryread_w
				tst		r8, #D_FLAG
				addeq	r5, r5, #2
				subne	r5, r5, #2
				strh	r5, [r9, #CPU_SI]
				ldrh	r5, [r9, #CPU_AX]
				SUB16	r5, r0
				mov		pc, r11


ret_near_d16	CPUWORK	#11
				GETPC16
				ldrh	r1, [r9, #CPU_SP]
				ldr		r2, [r9, #CPU_SS_BASE]
				add		r3, r0, r1
				add		r0, r1, r2
				add		r3, r3, #2
				strh	r3, [r9, #CPU_SP]
				bl		i286_memoryread_w
				mov		r8, r8 lsl #16
				mov		r8, r8 lsr #16
				orr		r8, r8, r0 lsl #16
				mov		pc, r11

ret_near		CPUWORK	#11
				ldrh	r4, [r9, #CPU_SP]
				ldr		r0, [r9, #CPU_SS_BASE]
				add		r0, r4, r0
				add		r4, r4, #2
				strh	r4, [r9, #CPU_SP]
				bl		i286_memoryread_w
				mov		r8, r8 lsl #16
				mov		r8, r8 lsr #16
				orr		r8, r8, r0 lsl #16
				mov		pc, r11

mov_ea8_d8		GETPC16
				and		r1, r0, #&ff
				mov		r4, r0, lsr #8
				cmp		r1, #&c0
				bcs		med8_r
				bl		i286a_ea
				mov		r1, r4
				mov		lr, r11
				b		i286_memorywrite
med8_r			R8DST	r0, r1
				ldrb	r4, [r1, #CPU_REG]
				mov		pc, r11

mov_ea16_d16	GETPC8
				cmp		r0, #&c0
				bcs		med16_r
				bl		i286a_ea
				mov		r4, r0
				GETPC16
				mov		r1, r0
				mov		r0, r4
				mov		lr, r11
				b		i286_memorywrite_w
med16_r			R16DST	r0, r4
				GETPC16
				ldrh	r0, [r4, #CPU_REG]
				mov		pc, r11

leave			CPUWORK	#5
				ldrh	r4, [r9, #CPU_BP]
				ldr		r0, [r9, #CPU_SS_BASE]
				add		r0, r4, r0
				bl		i286_memoryread_w
				add		r4, r4, #2
				strh	r0, [r9, #CPU_BP]
				strh	r4, [r9, #CPU_SP]
				mov		pc, r11

ret_far_d16		CPUWORK	#15
				GETPC16
				mov		r6, r0
				ldrh	r4, [r9, #CPU_SP]
				ldr		r5, [r9, #CPU_SS_BASE]
				add		r0, r4, r5
				add		r4, r4, #2
				bl		i286_memoryread_w
				mov		r8, r8 lsl #16
				mov		r8, r8 lsr #16
				orr		r8, r8, r0 lsl #16
				bic		r4, r4, #(1 << 16)
				add		r0, r4, r5
				add		r4, r4, #2
				bl		i286_memoryread_w
				add		r4, r6, r4
				mov		r1, r0 lsl #4
				strh	r4, [r9, #CPU_SP]
				strh	r0, [r9, #CPU_CS]
				str		r1, [r9, #CPU_CS_BASE]
				mov		pc, r11

ret_far			CPUWORK	#15
				ldrh	r4, [r9, #CPU_SP]
				ldr		r5, [r9, #CPU_SS_BASE]
				add		r0, r4, r5
				add		r4, r4, #2
				bl		i286_memoryread_w
				mov		r8, r8 lsl #16
				mov		r8, r8 lsr #16
				orr		r8, r8, r0 lsl #16
				bic		r4, r4, #(1 << 16)
				add		r0, r4, r5
				add		r4, r4, #2
				bl		i286_memoryread_w
				mov		r1, r0 lsl #4
				strh	r4, [r9, #CPU_SP]
				strh	r0, [r9, #CPU_CS]
				str		r1, [r9, #CPU_CS_BASE]
				mov		pc, r11

int_03			CPUWORK	#3
				mov		r6, #3
				b		i286a_localint

int_d8			CPUWORK	#3
				GETPC8
				mov		r6, r0
				b		i286a_localint

into			CPUWORK	#4
				tst		r8, #O_FLAG
				moveq	pc, r11
				mov		r6, #4
				b		i286a_localint


aam				CPUWORK	#16
				GETPC8
				movs	r0, r0, lsl #7
				beq		aamzero
				ldrb	r1, [r9, #CPU_AL]
				mov		r2, #&80
				mov		r3, #0
aamlp			cmp		r1, r0
				subcs	r1, r1, r0
				orrcs	r3, r2, r3
				movs	r2, r2 lsr #1
				bne		aamlp
				bic		r8, r8, #(S_FLAG + Z_FLAG + P_FLAG)
				ldrb	r2, [r10, r1]
				and		r2, r2, #P_FLAG
				orr		r8, r2, r8
				orr		r1, r1, r3 lsl #8
				movs	r2, r1 lsl #16
				orreq	r8, r8, #Z_FLAG
				orrmi	r8, r8, #S_FLAG
				strh	r1, [r9, #CPU_AX]
				mov		pc, r11
aamzero			sub		r8, r8, #(2 << 16)
				mov		r6, #0
				b		i286a_localint

aad				CPUWORK	#14
				GETPC8
				ldrh	r1, [r9, #CPU_AX]
				bic		r8, r8, #(S_FLAG + Z_FLAG + P_FLAG)
				mov		r2, r1 lsr #8
				mla		r3, r2, r0, r1
				and		r1, r3, #&ff
				ldrb	r2, [r10, r1]
				strh	r1, [r9, #CPU_AX]
				orr		r8, r2, r8
				mov		pc, r11

setalc			CPUWORK	#2
				mov		r0, r8 lsr #31
				mov		r0, r0 asr #31
				strb	r0, [r9, #CPU_AL]
				mov		pc, r11

xlat			CPUWORK	#5
				ldrb	r0, [r9, #CPU_AL]
				ldrh	r1, [r9, #CPU_BX]
				ldr		r2, [r9, #CPU_DS_FIX]
				add		r0, r1, r0
				bic		r0, r0, #(1 << 16)
				add		r0, r2, r0
				bl		i286_memoryread
				strb	r0, [r9, #CPU_AL]
				mov		pc, r11

esc				CPUWORK	#2
				GETPC8
				cmp		r0, #&c0
				movcs	pc, r11
				mov		lr, r11
				b		i286a_ea


loopnz			ldrh	r0, [r9, #CPU_CX]
				subs	r0, r0, #1
				strh	r0, [r9, #CPU_CX]
				beq		lpnznoj
				tst		r8, #Z_FLAG
				bne		lpnznoj
				JMPS	#8
lpnznoj			CPUWORK	#4
				add		r8, r8, #(1 << 16)
				mov		pc, r11

loopz			ldrh	r0, [r9, #CPU_CX]
				subs	r0, r0, #1
				strh	r0, [r9, #CPU_CX]
				beq		lpznoj
				tst		r8, #Z_FLAG
				beq		lpnznoj
				JMPS	#8
lpznoj			CPUWORK	#4
				add		r8, r8, #(1 << 16)
				mov		pc, r11

loop			ldrh	r0, [r9, #CPU_CX]
				subs	r0, r0, #1
				strh	r0, [r9, #CPU_CX]
				beq		lpnoj
				JMPS	#8
lpnoj			CPUWORK	#4
				add		r8, r8, #(1 << 16)
				mov		pc, r11

jcxz			ldrh	r0, [r9, #CPU_CX]
				cmp		r0, #0
				beq		jcxzj
				CPUWORK	#4
				add		r8, r8, #(1 << 16)
				mov		pc, r11
jcxzj			JMPS	#8

in_al_d8		CPUWORK	#5
				GETPC8
				bl		iocore_inp8
				strb	r0, [r9, #CPU_AL]
				mov		pc, r11

in_ax_d8		CPUWORK	#5
				GETPC8
				bl		iocore_inp16
				strh	r0, [r9, #CPU_AX]
				mov		pc, r11

out_d8_al		CPUWORK	#3
				GETPC8
				ldrb	r1, [r9, #CPU_AL]
				mov		lr, r11
				b		iocore_out8

out_d8_ax		CPUWORK	#3
				GETPC8
				ldrh	r1, [r9, #CPU_AX]
				mov		lr, r11
				b		iocore_out16

call_near		CPUWORK	#7
				GETPC16
				ldrh	r2, [r9, #CPU_SP]
				ldr		r3, [r9, #CPU_SS_BASE]
				sub		r1, r2, #2
				mov		r2, r1 lsl #16
				strh	r1, [r9, #CPU_SP]
				mov		r1, r8 lsr #16
				add		r8, r8, r0 lsl #16
				add		r0, r3, r2 lsr #16
				mov		lr, r11
				b		i286_memorywrite_w

jmp_near		CPUWORK	#7
				GETPC16
				add		r8, r8, r0 lsl #16
				mov		pc, r11

jmp_far			CPUWORK	#11
				ldr		r4, [r9, #CPU_CS_BASE]
				add		r0, r4, r8 lsr #16
				bl		i286_memoryread_w
				add		r8, r8, #(2 << 16)
				mov		r1, r0, lsl #16
				add		r0, r4, r8 lsr #16
				mov		r8, r8, lsl #16
				orr		r8, r0, r8 lsr #16
				bl		i286_memoryread_w
				mov		r1, r0 lsl #4
				strh	r0, [r9, #CPU_CS]
				str		r1, [r9, #CPU_CS_BASE]
				mov		pc, r11

in_al_dx		CPUWORK	#5
				ldrh	r0, [r9, #CPU_DX]
				bl		iocore_inp8
				strb	r0, [r9, #CPU_AL]
				mov		pc, r11

in_ax_dx		CPUWORK	#5
				ldrh	r0, [r9, #CPU_DX]
				bl		iocore_inp16
				strh	r0, [r9, #CPU_AX]
				mov		pc, r11

out_dx_al		CPUWORK	#3
				ldrb	r1, [r9, #CPU_AL]
				ldrh	r0, [r9, #CPU_DX]
				mov		lr, r11
				b		iocore_out8

out_dx_ax		CPUWORK	#3
				ldrh	r1, [r9, #CPU_AX]
				ldrh	r0, [r9, #CPU_DX]
				mov		lr, r11
				b		iocore_out16


lock			CPUWORK	#2
				mov		pc, r11

hlt				CREMSET	#-1
				sub		r8, r8, #(1 << 16)
				mov		pc, r11

cmc				CPUWORK	#2
				eor		r8, r8, #C_FLAG
				mov		pc, r11

clc				CPUWORK	#2
				bic		r8, r8, #C_FLAG
				mov		pc, r11

stc				CPUWORK	#2
				orr		r8, r8, #C_FLAG
				mov		pc, r11

cli				CPUWORK	#3
				mov		r0, #0
				bic		r8, r8, #I_FLAG
				strb	r0, [r9, #CPU_TRAP]
				mov		pc, r11

cld				CPUWORK	#2
				bic		r8, r8, #D_FLAG
				mov		pc, r11

std				CPUWORK	#2
				orr		r8, r8, #D_FLAG
				mov		pc, r11



i286a_step		stmdb	sp!, {r4 - r11, lr}
				ldr		r9, ias_r9
				ldr		r10, ias_r10
		;		ldr		r7, [r9, #CPU_REMAINCLOCK]
				ldr		r8, [r9, #CPU_FLAG]

				ldr		r0, [r9, #CPU_CS_BASE]
				add		r0, r0, r8 lsr #16
				bl		i286_memoryread
				add		r8, r8, #(1 << 16)

				adr		r1, optbl1
				mov		r11, pc
				ldr		pc, [r1, r0 lsl #2]

				str		r8, [r9, #CPU_FLAG]

				ldmia	sp!, {r4 - r11, pc}

ias_r9			dcd		i286core - CPU_REG
ias_r10			dcd		_szpcflag8

optbl1			dcd		add_ea_r8
				dcd		add_ea_r16
				dcd		add_r8_ea
				dcd		add_r16_ea
				dcd		add_al_d8
				dcd		add_ax_d16
				dcd		push_es
				dcd		pop_es
				dcd		or_ea_r8
				dcd		or_ea_r16
				dcd		or_r8_ea
				dcd		or_r16_ea
				dcd		or_al_d8
				dcd		or_ax_d16
				dcd		push_cs
				dcd		0					; op_0f/pop_cs

				dcd		adc_ea_r8
				dcd		adc_ea_r16
				dcd		adc_r8_ea
				dcd		adc_r16_ea
				dcd		adc_al_d8
				dcd		adc_ax_d16
				dcd		push_ss
				dcd		pop_ss
				dcd		sbb_ea_r8
				dcd		sbb_ea_r16
				dcd		sbb_r8_ea
				dcd		sbb_r16_ea
				dcd		sbb_al_d8
				dcd		sbb_ax_d16
				dcd		push_ds
				dcd		pop_ds

				dcd		and_ea_r8
				dcd		and_ea_r16
				dcd		and_r8_ea
				dcd		and_r16_ea
				dcd		and_al_d8
				dcd		and_ax_d16
				dcd		0				; segprefix_es
				dcd		0				; daa
				dcd		sub_ea_r8
				dcd		sub_ea_r16
				dcd		sub_r8_ea
				dcd		sub_r16_ea
				dcd		sub_al_d8
				dcd		sub_ax_d16
				dcd		0				; segprefix_cs
				dcd		0				; das

				dcd		xor_ea_r8
				dcd		xor_ea_r16
				dcd		xor_r8_ea
				dcd		xor_r16_ea
				dcd		xor_al_d8
				dcd		xor_ax_d16
				dcd		0				; segprefix_ss
				dcd		0				; aaa
				dcd		cmp_ea_r8
				dcd		cmp_ea_r16
				dcd		cmp_r8_ea
				dcd		cmp_r16_ea
				dcd		cmp_al_d8
				dcd		cmp_ax_d16
				dcd		0				; segprefix_ds
				dcd		0				; aas

				dcd		inc_ax
				dcd		inc_cx
				dcd		inc_dx
				dcd		inc_bx
				dcd		inc_sp
				dcd		inc_bp
				dcd		inc_si
				dcd		inc_di
				dcd		dec_ax
				dcd		dec_cx
				dcd		dec_dx
				dcd		dec_bx
				dcd		dec_sp
				dcd		dec_bp
				dcd		dec_si
				dcd		dec_di

				dcd		push_ax
				dcd		push_cx
				dcd		push_dx
				dcd		push_bx
				dcd		push_sp
				dcd		push_bp
				dcd		push_si
				dcd		push_di
				dcd		pop_ax
				dcd		pop_cx
				dcd		pop_dx
				dcd		pop_bx
				dcd		pop_sp
				dcd		pop_bp
				dcd		pop_si
				dcd		pop_di

				dcd		0				; pusha
				dcd		0				; popa
				dcd		0				; bound
				dcd		0				; arpl
				dcd		0				; reserved
				dcd		0				; reserved
				dcd		0				; reserved
				dcd		0				; reserved
				dcd		0				; push_d16
				dcd		0				; imul_reg_ea_d16
				dcd		0				; push_d8
				dcd		0				; imul_reg_ea_d8
				dcd		0				; insb
				dcd		0				; insw
				dcd		0				; outsb
				dcd		0				; outsw

				dcd		jo_short
				dcd		jno_short
				dcd		jc_short
				dcd		jnc_short
				dcd		jz_short
				dcd		jnz_short
				dcd		jna_short
				dcd		ja_short
				dcd		js_short
				dcd		jns_short
				dcd		jp_short
				dcd		jnp_short
				dcd		0				; jl_short
				dcd		0				; jnl_short
				dcd		0				; jle_short
				dcd		0				; jnle_short

				dcd		0				; calc_ea8_i8
				dcd		0				; calc_ea16_i16
				dcd		0				; calc_ea8_i8
				dcd		0				; calc_ea16_i8
				dcd		test_ea_r8
				dcd		test_ea_r16
				dcd		xchg_ea_r8
				dcd		xchg_ea_r16
				dcd		mov_ea_r8
				dcd		mov_ea_r16
				dcd		mov_r8_ea
				dcd		mov_r16_ea
				dcd		0				; mov_ea_seg
				dcd		0				; lea_r16_ea
				dcd		0				; mov_seg_ea
				dcd		0				; pop_ea

				dcd		0				; nop
				dcd		xchg_ax_cx
				dcd		xchg_ax_dx
				dcd		xchg_ax_bx
				dcd		xchg_ax_sp
				dcd		xchg_ax_bp
				dcd		xchg_ax_si
				dcd		xchg_ax_di
				dcd		cbw
				dcd		cwd
				dcd		0				; call_far
				dcd		wait
				dcd		0				; pushf
				dcd		0				; popf
				dcd		sahf
				dcd		lahf

				dcd		mov_al_m8
				dcd		mov_ax_m16
				dcd		mov_m8_al
				dcd		mov_m16_ax
				dcd		0				; movsb
				dcd		0				; movsw
				dcd		0				; cmpsb
				dcd		0				; cmpsw
				dcd		test_al_d8
				dcd		test_ax_d16
				dcd		0				; stosb
				dcd		0				; stosw
				dcd		0				; lodsb
				dcd		0				; lodsw
				dcd		0				; scasb
				dcd		0				; scasw

				dcd		mov_al_imm
				dcd		mov_cl_imm
				dcd		mov_dl_imm
				dcd		mov_bl_imm
				dcd		mov_ah_imm
				dcd		mov_ch_imm
				dcd		mov_dh_imm
				dcd		mov_bh_imm
				dcd		mov_ax_imm
				dcd		mov_cx_imm
				dcd		mov_dx_imm
				dcd		mov_bx_imm
				dcd		mov_sp_imm
				dcd		mov_bp_imm
				dcd		mov_si_imm
				dcd		mov_di_imm

				dcd		0				; shift_ea8_d8
				dcd		0				; shift_ea16_d8
				dcd		0				; ret_near_d16
				dcd		0				; ret_near
				dcd		0				; les_r16_ea
				dcd		0				; lds_r16_ea
				dcd		0				; mov_ea8_d8
				dcd		0				; mov_ea16_d16
				dcd		0				; enter
				dcd		0				; leave
				dcd		0				; ret_far_d16
				dcd		0				; ret_far
				dcd		0				; int_03
				dcd		0				; int_d8
				dcd		0				; into
				dcd		0				; iret

				dcd		0				; shift_ea8_1
				dcd		0				; shift_ea16_1
				dcd		0				; shift_ea8_cl
				dcd		0				; shift_ea16_cl
				dcd		0				; aam
				dcd		0				; aad
				dcd		0				; setalc
				dcd		0				; xlat
				dcd		0				; esc0
				dcd		0				; esc1
				dcd		0				; esc2
				dcd		0				; esc3
				dcd		0				; esc4
				dcd		0				; esc5
				dcd		0				; esc6
				dcd		0				; esc7

				dcd		0				; loopnz
				dcd		0				; loopz
				dcd		0				; loop
				dcd		0				; jcxz
				dcd		0				; in_al_d8
				dcd		0				; in_ax_d8
				dcd		0				; out_d8_al
				dcd		0				; out_d8_ax
				dcd		0				; call_near
				dcd		0				; jmp_near
				dcd		0				; jmp_far
				dcd		jmp_short
				dcd		0				; in_al_dx
				dcd		0				; in_ax_dx
				dcd		0				; out_dx_al
				dcd		0				; out_dx_ax

				dcd		0				; lock
				dcd		0				; lock
				dcd		0				; repne
				dcd		0				; repe
				dcd		0				; hlt
				dcd		0				; cmc
				dcd		0				; ope0xf6
				dcd		0				; ope0xf7
				dcd		0				; clc
				dcd		0				; stc
				dcd		0				; cli
				dcd		0				; sti
				dcd		0				; cld
				dcd		0				; std
				dcd		0				; ope0xfe
				dcd		0				; ope0xff


	END


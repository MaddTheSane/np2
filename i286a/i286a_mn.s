
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

	EXPORT		i286a_step

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
; segprefix_es
; daa

sub_ea_r8		OP_EA_R8	SUB8, #2, #7
sub_ea_r16		OP_EA_R16	SUB16, #2, #7
sub_r8_ea		OP_R8_EA	SUB8, #2, #7
sub_r16_ea		OP_R16_EA	SUB16, #2, #7
sub_al_d8		OP_AL_D8	SUB8, #3
sub_ax_d16		OP_AX_D16	SUB16, #3
; segprefix_cs
; das

xor_ea_r8		OP_EA_R8	XOR8, #2, #7
xor_ea_r16		OP_EA_R16	XOR16, #2, #7
xor_r8_ea		OP_R8_EA	XOR8, #2, #7
xor_r16_ea		OP_R16_EA	XOR16, #2, #7
xor_al_d8		OP_AL_D8	XOR8, #3
xor_ax_d16		OP_AX_D16	XOR16, #3
; segprefix_ss
; aaa

cmp_ea_r8		S_EA_R8		SUB8, #2, #7
cmp_ea_r16		S_EA_R16	SUB16, #2, #7
cmp_r8_ea		S_R8_EA		SUB8, #2, #6
cmp_r16_ea		S_R16_EA	SUB16, #2, #6
cmp_al_d8		S_AL_D8		SUB8, #3
cmp_ax_d16		S_AX_D16	SUB16, #3
; segprefix_ds
; aas

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

; pusha
; popa
; bound
; arpl
; push_d16
; imul_reg_ea_d16
; push_d8
; imul_reg_ea_d8
; insb
; insw
; outsb
; outsw

; jo_short
; jno_short
; jc_short
; jnc_short
; jz_short
; jnz_short
; jna_short
; ja_short
; js_short
; jns_short
; jp_short
; jnp_short
; jl_short
; jnl_short
; jle_short
; jnle_short

; calc_ea8_i8
; calc_ea16_i16
; calc_ea16_i8
test_ea_r8		S_EA_R8		AND8, #2, #6
test_ea_r16		S_EA_R16	AND16, #2, #6
; xchg_ea_r8	*
; xchg_ea_r16	*
; mov_ea_r8		*
; mov_ea_r16	*
; mov_r8_ea		*
; mov_r16_ea	*
; mov_ea_seg
; lea_r16_ea
; mov_seg_ea
; pop_ea

; nop
xchg_ax_cx		XCHG_AX		#CPU_CX, #3
xchg_ax_dx		XCHG_AX		#CPU_DX, #3
xchg_ax_bx		XCHG_AX		#CPU_BX, #3
xchg_ax_sp		XCHG_AX		#CPU_SP, #3
xchg_ax_bp		XCHG_AX		#CPU_BP, #3
xchg_ax_si		XCHG_AX		#CPU_SI, #3
xchg_ax_di		XCHG_AX		#CPU_DI, #3
; cbw
; cwd
; call_far
; wait
; pushf
; popf
; sahf
; lahf

; mov_al_m8
; mov_ax_m16
; mov_m8_al
; mov_m16_ax
; movsb
; movsw
; cmpsb
; cmpsw
test_al_d8		S_AL_D8		AND8, #3
test_ax_d16		S_AX_D16	AND16, #3
; stosb
; stosw
; lodsb
; lodsw
; scasb
; scasw

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
; ret_near_d16
; ret_near
; les_r16_ea
; lds_r16_ea
; mov_ea8_d8
; mov_ea16_d16
; enter
; leave
; ret_far_d16
; ret_far
; int_03
; int_d8
; into
; iret

; shift_ea8_1
; shift_ea16_1
; shift_ea8_cl
; shift_ea16_cl
; aam
; aad
; setalc
; xlat
; esc

; loopnz
; loopz
; loop
; jcxz
; in_al_d8
; in_ax_d8
; out_d8_al
; out_d8_ax
; call_near
; jmp_near
; jmp_far
; jmp_short
; in_al_dx
; in_ax_dx
; out_dx_al
; out_dx_ax

; lock
; repne
; repe
; hlt
; cmc
; ope0xf6
; ope0xf7
; clc
; stc
; cli
; sti
; cld
; std
; ope0xfe
; ope0xff


; ----

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
				dcd		0				; push_d16
				dcd		0				; imul_reg_ea_d16
				dcd		0				; push_d8
				dcd		0				; imul_reg_ea_d8
				dcd		0				; insb
				dcd		0				; insw
				dcd		0				; outsb
				dcd		0				; outsw

				dcd		0				; jo_short
				dcd		0				; jno_short
				dcd		0				; jc_short
				dcd		0				; jnc_short
				dcd		0				; jz_short
				dcd		0				; jnz_short
				dcd		0				; jna_short
				dcd		0				; ja_short
				dcd		0				; js_short
				dcd		0				; jns_short
				dcd		0				; jp_short
				dcd		0				; jnp_short
				dcd		0				; jl_short
				dcd		0				; jnl_short
				dcd		0				; jle_short
				dcd		0				; jnle_short

				dcd		0				; calc_ea8_i8
				dcd		0				; calc_ea16_i16
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
				dcd		0				; cbw
				dcd		0				; cwd
				dcd		0				; call_far
				dcd		0				; wait
				dcd		0				; pushf
				dcd		0				; popf
				dcd		0				; sahf
				dcd		0				; lahf

				dcd		0				; mov_al_m8
				dcd		0				; mov_ax_m16
				dcd		0				; mov_m8_al
				dcd		0				; mov_m16_ax
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
				dcd		0				; jmp_short
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


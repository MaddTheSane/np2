
VRAM_STEP			equ		&100000
VRAM_B				equ		&0a8000
VRAM_R				equ		&0b0000
VRAM_G				equ		&0b8000
VRAM_E				equ		&0e0000

FONT_ADRS			equ		&110000
ITF_ADRS			equ		&1f8000

; gdcs_access		equ		0
; gdcs_disp			equ		1
GDCS_TEXTDISP		equ		2
GDCS_MSWACC			equ		3
GDCS_GRPHDISP		equ		4
; gdcs_palchange	equ		5
; gdcs_mode2		equ		6

; grcg_counter		equ		0
; grcg_mode			equ		4
GRCG_MODEREG		equ		6
GRCG_TILE			equ		8
; grcg_gdcwithgrcg	equ		16
; grcg_chip			equ		20

; vramop_operate	equ		0
VRAMOP_TRAMWAIT		equ		4
VRAMOP_VRAMWAIT		equ		8
VRAMOP_GRCGWAIT		equ		12

GW_LOW				equ		0
GW_HIGH				equ		4
GW_WRITABLE			equ		8


	INCLUDE	i286a.inc

	IMPORT	memfn
	IMPORT	vramupdate
	IMPORT	gdcs
	IMPORT	vramop
	IMPORT	grcg
	IMPORT	tramupdate
	IMPORT	cgwindow

	EXPORT	i286_wt
	EXPORT	i286_wtex
	EXPORT	tram_wt
	EXPORT	vram_w0
	EXPORT	vram_w1
	EXPORT	grcg_rmw0
	EXPORT	grcg_rmw1
	EXPORT	grcg_tdw0
	EXPORT	grcg_tdw1

	EXPORT	i286_rd
	EXPORT	i286_rdex
;	EXPORT	tram_rd
;	EXPORT	vram_r0
;	EXPORT	vram_r1

	EXPORT	i286w_wt
	EXPORT	i286w_wtex
	EXPORT	vramw_w0
	EXPORT	vramw_w1
	EXPORT	grcgw_rmw0
	EXPORT	grcgw_rmw1
	EXPORT	grcgw_tdw0
	EXPORT	grcgw_tdw1

	EXPORT	i286w_rd
	EXPORT	i286w_rdex
;	EXPORT	tramw_rd
;	EXPORT	vramw_r0
;	EXPORT	vramw_r1

	EXPORT	i286_nonram_r
	EXPORT	i286_nonram_rw


	AREA	.text, CODE, READONLY


; ---- write byte

i286_wt			strb	r1, [r9, r0]
				mov		pc, lr

i286_wtex		ldr		r2, [r9, #CPU_ADRSMASK]
				and		r0, r2, r0
				strb	r1, [r9, r0]
				mov		pc, lr


tram_wt			ldr		r3, twt_vramop
				ldr		r3, [r3, #VRAMOP_TRAMWAIT]
				CPUWORK	r3

				mov		r12, r0, lsl #(31 - 12)
				cmp		r0, #&a2000
				bcc		twt_write

				cmp		r0, #&a4000
				bcs		twt_nontram
				tst		r0, #1
				movne	pc, lr

				cmp		r12, #(&1fe0 << (31 - 12))
				bcc		twt_write
				tst		r0, #2
				beq		twt_write

				ldr		r2, twt_gdcs
				ldrb	r2, [r2, #GDCS_MSWACC]
				cmp		r2, #0
				moveq	pc, lr

twt_write		strb	r1, [r9, r0]
				ldr		r2, twt_tramupd
				mov		r3, #1
				strb	r3, [r2, r12, lsr #(32 - 12)]
				ldr		r2, twt_gdcs
				ldrb	r12, [r2, #GDCS_TEXTDISP]
				orr		r12, r12, r3
				strb	r12, [r2, #GDCS_TEXTDISP]
				mov		pc, lr

twt_nontram		cmp		r0, #&a5000
				movcs	pc, lr
				tst		r0, #1
				moveq	pc, lr
				ldr		r2, twt_cgwnd
				ldrb	r12, [r2, #GW_WRITABLE]
				tst		r12, #1
				moveq	pc, lr
				orr		r12, r12, #&80
				strb	r12, [r2, #GW_WRITABLE]
				and		r0, r0, #(&f << 1)
				ldr		r3, [r2, #GW_HIGH]
				add		r12, r9, #FONT_ADRS
				add		r12, r12, r0 lsr #1
				strb	r1, [r3, r12]
				mov		pc, lr
twt_vramop		dcd		vramop
twt_tramupd		dcd		tramupdate
twt_gdcs		dcd		gdcs
twt_cgwnd		dcd		cgwindow


vram_w0			ldr		r3, vw0_vramop
				ldr		r2, vw0_gdcs
				strb	r1, [r0, r9]
				ldr		r1, [r3, #VRAMOP_VRAMWAIT]
				ldr		r3, vw0_vramupd
				mov		r0, r0, lsl #(32 - 15)
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				CPUWORK	r1
				ldrb	r1, [r3, r0, lsr #(32 - 15)]
				orr		r12, r12, #1
				strb	r12, [r2, #GDCS_GRPHDISP]
				orr		r1, r1, #1
				strb	r1, [r3, r0, lsr #(32 - 15)]
				mov		pc, lr
vw0_vramop		dcd		vramop
vw0_gdcs		dcd		gdcs
vw0_vramupd		dcd		vramupdate

vram_w1			add		r0, r0, #VRAM_STEP
				ldr		r3, vw1_vramop
				ldr		r2, vw1_gdcs
				strb	r1, [r0, r9]
				ldr		r1, [r3, #VRAMOP_VRAMWAIT]
				ldr		r3, vw1_vramupd
				mov		r0, r0, lsl #(32 - 15)
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				CPUWORK	r1
				ldrb	r1, [r3, r0, lsr #(32 - 15)]
				orr		r12, r12, #2
				strb	r12, [r2, #GDCS_GRPHDISP]
				orr		r1, r1, #2
				strb	r1, [r3, r0, lsr #(32 - 15)]
				mov		pc, lr
vw1_vramop		dcd		vramop
vw1_gdcs		dcd		gdcs
vw1_vramupd		dcd		vramupdate


grcg_tdw0		mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grw_vramupd
				ldrb	r12, [r2, r0]
				orr		r12, r12, #1
				strb	r12, [r2, r0]
				ldr		r2, grw_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #1
				strb	r12, [r2, #GDCS_GRPHDISP]
				b		grcg_tdw

grcg_tdw1		mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grw_vramupd
				ldrb	r12, [r2, r0]
				orr		r12, r12, #2
				strb	r12, [r2, r0]
				ldr		r2, grw_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #2
				strb	r12, [r2, #GDCS_GRPHDISP]
				add		r0, r0, #VRAM_STEP

grcg_tdw		add		r0, r0, #VRAM_B
				ldr		r3, grw_grcg
				add		r0, r0, r9
				ldrb	r2, [r3, #GRCG_MODEREG]
				orr		r1, r1, r2 lsl #16

				tst		r1, #(1 << 16)
				ldreqb	r2, [r3, #(GRCG_TILE + 0)]
				streqb	r2, [r0]
				tst		r1, #(2 << 16)
				add		r0, r0, #(VRAM_R - VRAM_B)
				ldreqb	r2, [r3, #(GRCG_TILE + 2)]
				streqb	r2, [r0]
				tst		r1, #(4 << 16)
				add		r0, r0, #(VRAM_G - VRAM_R)
				ldreqb	r2, [r3, #(GRCG_TILE + 4)]
				streqb	r2, [r0]
				tst		r1, #(8 << 16)
				add		r0, r0, #(VRAM_E - VRAM_G)
				ldreqb	r2, [r3, #(GRCG_TILE + 6)]
				streqb	r2, [r0]

				ldr		r3, grw_vramop
				ldr		r3, [r3, #VRAMOP_GRCGWAIT]
				CPUWORK	r3
				mov		pc, lr

grcg_rmw0		cmp		r1, #&ff
				beq		grcg_tdw0
				cmp		r1, #0
				beq		grcg_clock
				mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grw_vramupd
				ldrb	r12, [r2, r0]
				orr		r12, r12, #1
				strb	r12, [r2, r0]
				ldr		r2, grw_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #1
				strb	r12, [r2, #GDCS_GRPHDISP]
				b		grcg_rmw

grcg_rmw1		cmp		r1, #&ff
				beq		grcg_tdw1
				cmp		r1, #0
				beq		grcg_clock
				mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grw_vramupd
				ldrb	r12, [r2, r0]
				orr		r12, r12, #2
				strb	r12, [r2, r0]
				ldr		r2, grw_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #2
				strb	r12, [r2, #GDCS_GRPHDISP]
				add		r0, r0, #VRAM_STEP

grcg_rmw		add		r0, r0, #VRAM_B
				ldr		r3, grw_grcg
				add		r0, r0, r9
				ldrb	r2, [r3, #GRCG_MODEREG]
				orr		r1, r1, r2 lsl #16
				tst		r1, #(1 << 16)
				bne		grmw_bed
					ldrb	r12, [r0]
					ldrb	r2, [r3, #(GRCG_TILE + 0)]
					and		r2, r2, r1
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]
grmw_bed		tst		r1, #(2 << 16)
				add		r0, r0, #(VRAM_R - VRAM_B)
				bne		grmw_red
					ldrb	r12, [r0]
					ldrb	r2, [r3, #(GRCG_TILE + 2)]
					and		r2, r2, r1
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]
grmw_red		tst		r1, #(4 << 16)
				add		r0, r0, #(VRAM_G - VRAM_R)
				bne		grmw_ged
					ldrb	r12, [r0]
					ldrb	r2, [r3, #(GRCG_TILE + 4)]
					and		r2, r2, r1
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]
grmw_ged		tst		r1, #(8 << 16)
				bne		grcg_clock
				add		r0, r0, #(VRAM_E - VRAM_G)
					ldrb	r12, [r0]
					ldrb	r2, [r3, #(GRCG_TILE + 6)]
					and		r2, r2, r1
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]

grcg_clock		ldr		r3, grw_vramop
				ldr		r3, [r3, #VRAMOP_GRCGWAIT]
				CPUWORK	r3
				mov		pc, lr

grw_vramupd		dcd		vramupdate
grw_gdcs		dcd		gdcs
grw_grcg		dcd		grcg
grw_vramop		dcd		vramop


; ---- read word

i286_rd			ldrb	r0, [r9, r0]
				mov		pc, lr

i286_rdex		ldr		r12, [r9, #CPU_ADRSMASK]
				and		r0, r12, r0
				ldrb	r0, [r9, r0]
				mov		pc, lr


tram_rd			ldr		r3, trd_vramop
				cmp		r0, #&a4000
				bcs		trd_nontram
				ldr		r12, [r3, #VRAMOP_TRAMWAIT]
				ldrb	r0, [r9, r0]
trd_wait		CPUWORK	r12
				mov		pc, lr
trd_nontram		ldr		r12, [r3, #VRAMOP_TRAMWAIT]
				cmp		r0, #&a5000
				bcs		trd_wait
				ldr		r2, trd_cgwnd
				add		r1, r9, #FONT_ADRS
				tst		r0, #1
				ldreq	r3, [r2, #GW_LOW]
				ldrne	r3, [r2, #GW_HIGH]
				and		r0, r0, #(&f << 1)
				add		r1, r0, r1
				CPUWORK	r12
				ldrb	r0, [r1, r0]
				mov		pc, lr
trd_vramop		dcd		vramop
trd_cgwnd		dcd		cgwindow


vram_r0			ldr		r3, trd_vramop
				ldrb	r0, [r9, r0]
				ldr		r3, [r3, #VRAMOP_VRAMWAIT]
				CPUWORK	r3
				mov		pc, lr

vram_r1			ldr		r3, trd_vramop
				add		r0, r0, #VRAM_STEP
				ldrb	r0, [r0, r9]
				ldr		r3, [r3, #VRAMOP_VRAMWAIT]
				CPUWORK	r3
				mov		pc, lr


; ---- write word

i286w_wt		add		r2, r9, #1
				mov		r3, r1 lsr #8
				strb	r1, [r9, r0]
				strb	r3, [r2, r0]
				mov		pc, lr

i286w_wtex		ldr		r12, [r9, #CPU_ADRSMASK]
				add		r2, r9, #1
				mov		r3, r1 lsr #8
				and		r0, r12, r0
				strb	r1, [r9, r0]
				strb	r3, [r2, r0]
				mov		pc, lr


vramw_w0		ldr		r3, vww0_vramop
				ldr		r2, vww0_gdcs
				tst		r0, #1
				bne		vww0_odd
				strh	r1, [r0, r9]
				ldr		r1, [r3, #VRAMOP_VRAMWAIT]
				ldr		r3, vww0_vramupd
				mov		r0, r0 lsl #(32 - 15)
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				add		r3, r3, r0 lsr #(32 - 15)
				CPUWORK	r1
				ldrh	r1, [r3]
				orr		r12, r12, #1
				strb	r12, [r2, #GDCS_GRPHDISP]
				orr		r1, r1, #1
				orr		r1, r1, #(1 << 8)
				strh	r1, [r3]
				mov		pc, lr
vww0_vramop		dcd		vramop
vww0_gdcs		dcd		gdcs
vww0_vramupd	dcd		vramupdate
vww0_odd		add		r12, r0, r9
				strb	r1, [r0, r9]
				mov		r1, r1 lsr #8
				strb	r1, [r12, #1]
				ldr		r1, [r3, #VRAMOP_VRAMWAIT]
				ldr		r3, vww0_vramupd
				mov		r0, r0, lsl #(32 - 15)
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				add		r3, r3, r0 lsr #(32 - 15)
				CPUWORK	r1
				ldrb	r0, [r3]
				ldrb	r1, [r3, #1]
				orr		r12, r12, #1
				strb	r12, [r2, #GDCS_GRPHDISP]
				orr		r0, r0, #1
				orr		r1, r1, #1
				strb	r0, [r3]
				strb	r1, [r3, #1]
				mov		pc, lr

vramw_w1		add		r0, r0, #VRAM_STEP
				ldr		r3, vww1_vramop
				ldr		r2, vww1_gdcs
				tst		r0, #1
				bne		vww1_odd
				strh	r1, [r0, r9]
				ldr		r1, [r3, #VRAMOP_VRAMWAIT]
				ldr		r3, vww1_vramupd
				mov		r0, r0 lsl #(32 - 15)
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				add		r3, r3, r0 lsr #(32 - 15)
				CPUWORK	r1
				ldrh	r1, [r3]
				orr		r12, r12, #2
				strb	r12, [r2, #GDCS_GRPHDISP]
				orr		r1, r1, #2
				orr		r1, r1, #(2 << 8)
				strh	r1, [r3]
				mov		pc, lr
vww1_vramop		dcd		vramop
vww1_gdcs		dcd		gdcs
vww1_vramupd	dcd		vramupdate
vww1_odd		add		r12, r0, r9
				strb	r1, [r0, r9]
				mov		r1, r1 lsr #8
				strb	r1, [r12, #1]
				ldr		r1, [r3, #VRAMOP_VRAMWAIT]
				ldr		r3, vww1_vramupd
				mov		r0, r0, lsl #(32 - 15)
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				add		r3, r3, r0 lsr #(32 - 15)
				CPUWORK	r1
				ldrb	r0, [r3]
				ldrb	r1, [r3, #1]
				orr		r12, r12, #2
				strb	r12, [r2, #GDCS_GRPHDISP]
				orr		r0, r0, #2
				orr		r1, r1, #2
				strb	r0, [r3]
				strb	r1, [r3, #1]
				mov		pc, lr


grcgw_tdw0		ldr		r2, grww_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #1
				strb	r12, [r2, #GDCS_GRPHDISP]
				mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grw_vramupd
				add		r2, r2, r0
				ldrb	r12, [r2]
				orr		r12, r12, #1
				strb	r12, [r2]
				ldrb	r12, [r2, #1]
				orr		r12, r12, #1
				strb	r12, [r2, #1]
				b		grcgw_tdw
grcgw_tdw1		ldr		r2, grww_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #2
				strb	r12, [r2, #GDCS_GRPHDISP]
				mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grww_vramupd
				add		r2, r2, r0
				ldrb	r12, [r2]
				orr		r12, r12, #2
				strb	r12, [r2]
				ldrb	r12, [r2, #1]
				orr		r12, r12, #2
				strb	r12, [r2, #1]
				add		r0, r0, #VRAM_STEP
grcgw_tdw		add		r2, r9, #VRAM_B
				ldr		r3, grww_grcg
				add		r0, r0, r2
				ldrb	r2, [r3, #GRCG_MODEREG]
				orr		r1, r1, r2 lsl #16
				tst		r1, #(1 << 16)
				ldreqb	r2, [r3, #(GRCG_TILE + 0)]
				streqb	r2, [r0]
				streqb	r2, [r0, #1]
				tst		r1, #(2 << 16)
				add		r0, r0, #(VRAM_R - VRAM_B)
				ldreqb	r2, [r3, #(GRCG_TILE + 2)]
				streqb	r2, [r0]
				streqb	r2, [r0, #1]
				tst		r1, #(4 << 16)
				add		r0, r0, #(VRAM_G - VRAM_R)
				ldreqb	r2, [r3, #(GRCG_TILE + 4)]
				streqb	r2, [r0]
				streqb	r2, [r0, #1]
				tst		r1, #(8 << 16)
				add		r0, r0, #(VRAM_E - VRAM_G)
				ldreqb	r2, [r3, #(GRCG_TILE + 6)]
				streqb	r2, [r0]
				streqb	r2, [r0, #1]
				ldr		r3, grww_vramop
				ldr		r3, [r3, #VRAMOP_GRCGWAIT]
				CPUWORK	r3
				mov		pc, lr

grcgw_rmw0		add		r2, r1, #1
				cmp		r2, #&10000
				beq		grcgw_tdw0
				cmp		r1, #0
				beq		grcgw_clock
				ldr		r2, grww_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #1
				strb	r12, [r2, #GDCS_GRPHDISP]
				mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grww_vramupd
				tst		r0, #1
				bne		grcgo_rmw0
				ldrh	r12, [r2, r0]
				orr		r12, r12, #1
				orr		r12, r12, #(1 << 8)
				strh	r12, [r2, r0]
				b		grcge_rmw

grcgw_rmw1		add		r2, r1, #1
				cmp		r2, #&10000
				beq		grcgw_tdw1
				cmp		r1, #0
				beq		grcgw_clock
				ldr		r2, grww_gdcs
				ldrb	r12, [r2, #GDCS_GRPHDISP]
				orr		r12, r12, #2
				strb	r12, [r2, #GDCS_GRPHDISP]
				mov		r0, r0, lsl #(32 - 15)
				mov		r0, r0, lsr #(32 - 15)
				ldr		r2, grww_vramupd
				tst		r0, #1
				bne		grcgo_rmw1

				ldrh	r12, [r2, r0]
				orr		r12, r12, #2
				orr		r12, r12, #(2 << 8)
				strh	r12, [r2, r0]
				add		r0, r0, #VRAM_STEP

grcge_rmw		add		r2, r9, #VRAM_B
				ldr		r3, grww_grcg
				add		r0, r0, r2
				ldrb	r2, [r3, #GRCG_MODEREG]
				orr		r1, r1, r2 lsl #16
				tst		r1, #(1 << 16)
				bne		grmwo_bed
					ldrh	r2, [r3, #(GRCG_TILE + 0)]
					and		r2, r2, r1
					ldrh	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strh	r12, [r0]
grmwe_bed		tst		r1, #(2 << 16)
				add		r0, r0, #(VRAM_R - VRAM_B)
				bne		grmwo_red
					ldrh	r2, [r3, #(GRCG_TILE + 2)]
					and		r2, r2, r1
					ldrh	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strh	r12, [r0]
grmwe_red		tst		r1, #(4 << 16)
				add		r0, r0, #(VRAM_G - VRAM_R)
				bne		grmwo_ged
					ldrh	r2, [r3, #(GRCG_TILE + 4)]
					and		r2, r2, r1
					ldrh	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strh	r12, [r0]
grmwe_ged		tst		r1, #(8 << 16)
				bne		grmwe_eed
				add		r0, r0, #(VRAM_E - VRAM_G)
					ldrh	r2, [r3, #(GRCG_TILE + 6)]
					and		r2, r2, r1
					ldrh	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strh	r12, [r0]
grmwe_eed		ldr		r3, grww_vramop
				ldr		r3, [r3, #VRAMOP_GRCGWAIT]
				CPUWORK	r3
				mov		pc, lr

grww_gdcs		dcd		gdcs
grww_vramupd	dcd		vramupdate
grww_grcg		dcd		grcg
grww_vramop		dcd		vramop

grcgo_rmw0		add		r2, r2, r0
				ldrb	r12, [r2]
				orr		r12, r12, #1
				strb	r12, [r2]
				ldrb	r12, [r2, #1]
				orr		r12, r12, #1
				strb	r12, [r2, #1]
				b		grcgo_rmw

grcgo_rmw1		add		r2, r2, r0
				ldrb	r12, [r2]
				orr		r12, r12, #2
				strb	r12, [r2]
				ldrb	r12, [r2, #1]
				orr		r12, r12, #2
				strb	r12, [r2, #1]
				add		r0, r0, #VRAM_STEP

grcgo_rmw		add		r2, r9, #VRAM_B
				ldr		r3, grww_grcg
				add		r0, r0, r2
				ldrb	r2, [r3, #GRCG_MODEREG]
				orr		r1, r1, r2 lsl #16
				tst		r1, #(1 << 16)
				bne		grmwo_bed
					ldrh	r2, [r3, #(GRCG_TILE + 0)]
					and		r2, r2, r1

					ldrb	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]
					ldrb	r12, [r0, #1]
					bic		r12, r12, r1 lsr #8
					orr		r12, r12, r2 lsr #8
					strb	r12, [r0, #1]

grmwo_bed		tst		r1, #(2 << 16)
				add		r0, r0, #(VRAM_R - VRAM_B)
				bne		grmwo_red
					ldrh	r2, [r3, #(GRCG_TILE + 2)]
					and		r2, r2, r1

					ldrb	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]
					ldrb	r12, [r0, #1]
					bic		r12, r12, r1 lsr #8
					orr		r12, r12, r2 lsr #8
					strb	r12, [r0, #1]

grmwo_red		tst		r1, #(4 << 16)
				add		r0, r0, #(VRAM_G - VRAM_R)
				bne		grmwo_ged
					ldrh	r2, [r3, #(GRCG_TILE + 4)]
					and		r2, r2, r1

					ldrb	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]
					ldrb	r12, [r0, #1]
					bic		r12, r12, r1 lsr #8
					orr		r12, r12, r2 lsr #8
					strb	r12, [r0, #1]

grmwo_ged		tst		r1, #(8 << 16)
				bne		grcgw_clock
				add		r0, r0, #(VRAM_E - VRAM_G)
					ldrh	r2, [r3, #(GRCG_TILE + 6)]
					and		r2, r2, r1

					ldrb	r12, [r0]
					bic		r12, r12, r1
					orr		r12, r12, r2
					strb	r12, [r0]
					ldrb	r12, [r0, #1]
					bic		r12, r12, r1 lsr #8
					orr		r12, r12, r2 lsr #8
					strb	r12, [r0, #1]

grcgw_clock		ldr		r3, grww_vramop
				ldr		r3, [r3, #VRAMOP_GRCGWAIT]
				CPUWORK	r3
				mov		pc, lr


; ---- read word

i286w_rd		add		r2, r9, #1
				ldrb	r1, [r9, r0]
				ldrb	r3, [r2, r0]
				orr		r0, r1, r3 lsl #8
				mov		pc, lr

i286w_rdex		ldr		r12, [r9, #CPU_ADRSMASK]
				add		r2, r9, #1
				and		r0, r12, r0
				ldrb	r1, [r9, r0]
				ldrb	r3, [r2, r0]
				orr		r0, r1, r3 lsl #8
				mov		pc, lr

tramw_rd
twrd_vramop		dcd		vramop

vramw_r1		add		r0, r0, #VRAM_STEP
vramw_r0		ldr		r3, twrd_vramop
				add		r2, r9, r0
				ldrb	r0, [r9, r0]
				ldr		r3, [r3, #VRAMOP_VRAMWAIT]
				ldrb	r1, [r2, #1]
				CPUWORK	r3
				orr		r0, r0, r1 lsl #8
				mov		pc, lr


; ---- other

i286_nonram_r	mov		r0, #&ff
				mov		pc, lr

i286_nonram_rw	mov		r0, #&ff
				orr		r0, r0, #&ff00
				mov		pc, lr


	END


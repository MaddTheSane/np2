
VRAM_STEP			equ		&100000
VRAM_B				equ		&0a8000
VRAM_R				equ		&0b0000
VRAM_G				equ		&0b8000
VRAM_E				equ		&0e0000

SURFACE_WIDTH		equ		640
SURFACE_HEIGHT		equ		480
SURFACE_SIZE		equ		(SURFACE_WIDTH * SURFACE_HEIGHT)

NC_UPD72020			equ		&00
; NC_DISPSYNC		equ		&01
; NC_RASTER			equ		&02
; NC_realpal		equ		&03
; NC_LCD_MODE		equ		&04
; NC_skipline		equ		&05
; NC_skiplight		equ		&06
				; and more...

; DS_text_vbp		equ		&00
; DS_textymax		equ		&04
DS_GRPH_VBP			equ		&08
DS_GRPHYMAX			equ		&0c
; DS_scrnxpos		equ		&10
; DS_scrnxmax		equ		&14
; DS_scrnxextend	equ		&18
; DS_scrnymax		equ		&1c
; DS_textvad		equ		&20
DS_GRPHVAD			equ		&24

GDCCMD_MAX			equ		32

; GDC_SYNC			equ		0
; GDC_ZOOM			equ		8
GDC_CSRFORM			equ		9
GDC_SCROLL			equ		12
; GDC_TEXTW			equ		20
GDC_PITCH			equ		28
; GDC_LPEN			equ		29
; GDC_VECTW			equ		32
; GDC_CSRW			equ		43
; GDC_MASK			equ		46
; GDC_CSRR			equ		48
; GDC_WRITE			equ		53
; GDC_CODE			equ		54
; GDC_TERMDATA		equ		56

GD_PARA				equ		&000
; GD_fifo			equ		&100
; GD_cnt			equ		(&100 + (GDCCMD_MAX * 2))
; GD_ptr			equ		(&102 + (GDCCMD_MAX * 2))
; GD_rcv			equ		(&103 + (GDCCMD_MAX * 2))
; GD_snd			equ		(&104 + (GDCCMD_MAX * 2))
; GD_cmd			equ		(&105 + (GDCCMD_MAX * 2))
; GD_paracb			equ		(&106 + (GDCCMD_MAX * 2))
; GD_reserved		equ		(&107 + (GDCCMD_MAX * 2))
GD_SIZE				equ		(&108 + (GDCCMD_MAX * 2))

G_MASTER			equ		(GD_SIZE * -1)
G_SLAVE				equ		0
G_MODE1				equ		(GD_SIZE + &00)
; G_mode2			equ		(GD_SIZE + &01)
; G_vsync			equ		(GD_SIZE + &02)
; G_vsyncint		equ		(GD_SIZE + &03)
; G_analog			equ		(GD_SIZE + &04)
; G_palnum			equ		(GD_SIZE + &08)
; G_degpal			equ		(GD_SIZE + &0c)
; G_anapal			equ		(GD_SIZE + &10)
G_CLOCK				equ		(GD_SIZE + &50)
; G_display			equ		(GD_SIZE + &51)
; G_bitac			equ		(GD_SIZE + &52)
; G_m_drawing		equ		(GD_SIZE + &53)
; G_s_drawing		equ		(GD_SIZE + &54)


	IMPORT	np2cfg
	IMPORT	mem
	IMPORT	np2_vram
	IMPORT	dsync
	IMPORT	vramupdate
	IMPORT	renewal_line
	IMPORT	gdc

	EXPORT	grph_table0
	EXPORT	makegrph_init
	EXPORT	makegrph

	AREA	MAKEGRPHBSS, DATA, READONLY

grph_table0		dcd		&00000000
				dcd		&01000000
				dcd		&00010000
				dcd		&01010000
				dcd		&00000100
				dcd		&01000100
				dcd		&00010100
				dcd		&01010100
				dcd		&00000001
				dcd		&01000001
				dcd		&00010001
				dcd		&01010001
				dcd		&00000101
				dcd		&01000101
				dcd		&00010101
				dcd		&01010101


	AREA	MAKEGRPH, CODE, READONLY

makegrph_init	mov		pc, lr


	; r8 = mem
	; r9 = vc
	; r10 = out
	; r11 = grph_table0
	; tmp r2, r3, r4, r5, r12
	MACRO
$label	PIXADD		$shift
$label			and		r12, r3, #&0f
				and		r3, r3, #&f0
				ldr		r3, [r11, r3, lsr #2]
				ldr		r12, [r11, r12, lsl #2]
				orr		r4, r4, r3, lsl #$shift
				orr		r5, r5, r12, lsl #$shift
	MEND

		MACRO
$label	GRPHDATASET
$label			add		r2, r8, r9, lsr #17
				ldrb	r3, [r2]
				add		r2, r2, #(VRAM_R - VRAM_B)
				and		r12, r3, #&0f
				and		r3, r3, #&f0
				ldr		r4, [r11, r3, lsr #2]
				ldr		r5, [r11, r12, lsl #2]
				ldrb	r3, [r2]
				add		r2, r2, #(VRAM_G - VRAM_R)
				PIXADD	1
				ldrb	r3, [r2]
				add		r2, r2, #(VRAM_E - VRAM_G)
				PIXADD	2
				ldrb	r3, [r2]
				PIXADD	3
				str		r4, [r10]
				str		r5, [r10, #4]
	MEND


; ----

	; r0 = remain:0 (10:22)
	; r1 = mg.vm
	; r6 = liney:gdc.mode1:mul:mulorg:dsync.grphymax:0 (9:1:5:5:9:3)
	; r7 = vramupdate
	; r8 = mem
	; r9 = vc:0:flag:bit (15:1:8:8)
	; r10 = out
	; r11 = grph_table0
	; tmp r2, r3 / r4, r5, r12 (GRPHDATASET)
	; input - r2 = pos r3 = gdc

gp_all			ldr		r12, [r2, r3]
				orr		r9, r9, r12, lsl #(1 + 17)	; (vad << 17)
				mov		r12, r12, lsr #(4 + 16)		; remain
				orr		r0, r0, r12, lsl #22		; (remain << 22)
gpa_lineylp1	and		r12, r6, #(&1f << 12)		; mul !
				orr		r6, r6, r12, lsl #5
gpa_lineylp2	add		r1, r1, #640
				tst		r6, #(1 << 23)
				tstne	r6, #(1 << 22)
				bne		gpa_lineyed
				sub		r10, r1, #640
gpa_pixlp		GRPHDATASET
				add		r9, r9, #(1 << 17)
				add		r10, r10, #8
				cmp		r10, r1
				bcc		gpa_pixlp
				sub		r9, r9, #(80 << 17)
				ldr		r2, gp_renewline
				ldrb	r3, [r2, r6, lsr #23]
				orr		r3, r3, r9
				strb	r3, [r2, r6, lsr #23]
				bic		r9, r9, #(3 << 8)
gpa_lineyed		add		r6, r6, #(1 << 23)
				cmp		r6, r6, lsl #20
				bcs		makegrph_ed
				sub		r0, r0, #(1 << 22)
				movs	r12, r0, lsr #22
				beq		gpa_break
				tst		r6, #(&1f << 17)
				subne	r6, r6, #(1 << 17)
				bne		gpa_lineylp2
				add		r9, r9, r0, lsl #17
				b		gpa_lineylp1
gpa_break		ldr		r3, gp_gdc
				and		r9, r9, #255
				bic		r6, r6, #(&1f << 17)
				mov		pc, lr

gp_indirty		ldr		r12, [r2, r3]
				orr		r9, r9, r12, lsl #(1 + 17)	; (vad << 17)
				mov		r12, r12, lsr #(4 + 16)		; remain
				orr		r0, r0, r12, lsl #22		; (remain << 22)
gpi_lineylp1	and		r12, r6, #(&1f << 12)		; mul !
				orr		r6, r6, r12, lsl #5
gpi_lineylp2	add		r1, r1, #640
				tst		r6, #(1 << 23)
				tstne	r6, #(1 << 22)
				bne		gpi_lineyed
				sub		r10, r1, #640
gpi_pixlp		ldrb	r2, [r7, r9, lsr #17]
				ands	r2, r2, r9
				beq		gpi_pixnt
				orr		r9, r9, r2, lsl #8
				GRPHDATASET
gpi_pixnt		add		r9, r9, #(1 << 17)
				add		r10, r10, #8
				cmp		r10, r1
				bcc		gpi_pixlp
				sub		r9, r9, #(80 << 17)
				tst		r9, #(3 << 8)
				beq		gpi_lineyed
				ldr		r2, gp_renewline
				ldrb	r3, [r2, r6, lsr #23]
				orr		r3, r3, r9, lsr #8
				strb	r3, [r2, r6, lsr #23]
				bic		r9, r9, #(3 << 8)
gpi_lineyed		add		r6, r6, #(1 << 23)
				cmp		r6, r6, lsl #20
				bcs		makegrph_ed
				sub		r0, r0, #(1 << 22)
				movs	r12, r0, lsr #22
				beq		gpi_break
				tst		r6, #(&1f << 17)
				subne	r6, r6, #(1 << 17)
				bne		gpi_lineylp2
				add		r9, r9, r0, lsl #17
				b		gpi_lineylp1
gpi_break		ldr		r3, gp_gdc
				and		r9, r9, #255
				bic		r6, r6, #(&1f << 17)
				mov		pc, lr


makegrph		stmdb	sp!, {r4 - r11, lr}
				ldr		r7, gp_vramupdate
				ldr		r8, gp_vmem
				ldr		r11, gp_gtable0
				ldr		r3, gp_gdc
				ldr		r4, gp_dsync
				ldr		r2, [r4, #DS_GRPHVAD]
				ands	r0, r0, #1
				addne	r2, r2, #SURFACE_SIZE
				addne	r8, r8, #VRAM_STEP
				mov		r9, #1
				mov		r9, r9, lsl r0
				ldrb	r12, [r3, #G_CLOCK]
				ldrb	r0, [r3, #(G_SLAVE + GD_PARA + GDC_PITCH)]
				tst		r12, #&80
				moveq	r0, r0, lsl #1
				and		r0, r0, #&fe				; mg.pitch
				ldr		r6, [r4, #DS_GRPH_VBP]
				ldrb	r12, [r3, #G_MODE1]
				mov		r6, r6, lsl #23				; mg.liney << 23
				and		r12, r12, #&10
				orr		r6, r6, r12, lsl #(22 - 4)	; gdc.mode1:bit4 << 22
				ldrb	r12, [r3, #(G_SLAVE + GD_PARA + GDC_CSRFORM)]
				and		r12, r12, #&1f
				orr		r6, r6, r12, lsl #12		; mg.lr << 12
				ldr		r12, [r4, #DS_GRPHYMAX]
				orr		r6, r6, r12, lsl #3			; dsync.grphymax << 3

				cmp		r1, #0
				ldr		r1, gp_np2vram
				add		r1, r1, r2
				bne		mg_alp
mg_ilp			mov		r2, #(G_SLAVE + GD_PARA + GDC_SCROLL + 0)
				bl		gp_indirty
				mov		r2, #(G_SLAVE + GD_PARA + GDC_SCROLL + 4)
				bl		gp_indirty
				ldr		r12, gp_np2cfg
				ldrb	r12, [r12, #NC_UPD72020]
				cmp		r12, #0
				bne		mg_ilp
				mov		r2, #(G_SLAVE + GD_PARA + GDC_SCROLL + 8)
				bl		gp_indirty
				mov		r2, #(G_SLAVE + GD_PARA + GDC_SCROLL + 12)
				bl		gp_indirty
				b		mg_ilp
mg_alp			mov		r2, #(G_SLAVE + GD_PARA + GDC_SCROLL + 0)
				bl		gp_all
				mov		r2, #(G_SLAVE + GD_PARA + GDC_SCROLL + 4)
				bl		gp_all
				ldr		r12, gp_np2cfg
				ldrb	r12, [r12, #NC_UPD72020]
				cmp		r12, #0
				bne		mg_alp
				mov		r2, #(G_SLAVE + GD_PARA + GDC_SCROLL + 8)
				bl		gp_all
				mov		r2, #(G_SLAVE + GD_PARA + GDC_SCROLL + 12)
				bl		gp_all
				b		mg_alp

makegrph_ed		mov		r3, r9
				and		r3, r3, #255
				orr		r3, r3, r3, lsl #8
				orr		r3, r3, r3, lsl #16
				mov		r4, #0
mg_updclear		ldr		r12, [r7, r4]
				bic		r12, r12, r3
				str		r12, [r7, r4]
				add		r4, r4, #4
				cmp		r4, #&8000
				bcc		mg_updclear
				ldmia	sp!, {r4 - r11, pc}

gp_vramupdate	dcd		vramupdate
gp_vmem			dcd		mem + VRAM_B
gp_gtable0		dcd		grph_table0
gp_gdc			dcd		gdc - G_MASTER
gp_np2vram		dcd		np2_vram
gp_dsync		dcd		dsync
gp_renewline	dcd		renewal_line
gp_np2cfg		dcd		np2cfg

	END


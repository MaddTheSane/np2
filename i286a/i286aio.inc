
; PI_levels			equ		0
; PI_level			equ		4
; PI_levelsbak		equ		12
; PI_levelbak		equ		16
; PI_pry			equ		24
; PI_icw			equ		32
PI_IMR				equ		36
; PI_ocw3			equ		37
; PI_irr			equ		38
; PI_ext			equ		39
; PI_isr			equ		40
; PI_isrbak			equ		41
; PI_writeicw		equ		42
; PI_padding		equ		43
PI_SIZE				equ		44

PIC_MASTER			equ		0
PIC_SLAVE			equ		PI_SIZE
PIC_EXTIRQ			equ		(PI_SIZE * 2)


; DMACH_adrs		equ		0
; DMACH_leng		equ		4
; DMACH_adrsorg		equ		6
; DMACH_lengorg		equ		8
; DMACH_action		equ		10
; DMACH_outproc		equ		12
; DMACH_inproc		equ		16
; DMACH_extproc		equ		20
; DMACH_mode		equ		24
; DMACH_sreq		equ		25
; DMACH_ready		equ		26
; DMACH_mask		equ		27
DMACH_SIZE			equ		28

; DMA_dmach			equ		0
; DMA_lh			equ		(DMACH_SIZE * 4) + 0
; DMA_work			equ		(DMACH_SIZE * 4) + 4
DMAC_WORKING		equ		(DMACH_SIZE * 4) + 8
; DMA_mask			equ		(DMACH_SIZE * 4) + 12
; DMA_stat			equ		(DMACH_SIZE * 4) + 13


; #define PICEXISTINTR	((!pic.ext_irq) &&							\
;						((pic.pi[0].irr & (~pic.pi[0].imr)) ||		\
;						(pic.pi[1].irr & (~pic.pi[1].imr))))


	MACRO
$label	NOINTREXIT
				tst		r8, #I_FLAG
				moveq	pc, r11
				ldrb	r1, [r0, #PIC_EXTIRQ]
				ldr		r2, [r0, #(PIC_MASTER + PI_IMR)]
				mov		r12, #(&ff << 24)
				cmp		r1, #0
				movne	pc, r11
				ldr		r3, [r0, #(PIC_SLAVE + PI_IMR)]
				eor		r0, r12, r2 lsl #24
				tst		r0, r2 lsl #8
				eoreq	r0, r12, r3 lsl #24
				tsteq	r0, r3 lsl #8
				moveq	pc, r11
	MEND

	MACRO
$label	PICEXISTINTR	$noirq
				ldrb	r1, [r0, #PIC_EXTIRQ]
				ldr		r2, [r0, #(PIC_MASTER + PI_IMR)]
				mov		r12, #(&ff << 24)
				cmp		r1, #0
				bne		$noirq
				ldr		r3, [r0, #(PIC_SLAVE + PI_IMR)]
				eor		r0, r12, r2 lsl #24
				tst		r0, r2 lsl #8
				eoreq	r0, r12, r3 lsl #24
				tsteq	r0, r3 lsl #8
	MEND

	END



; PI_levels		equ		0
; PI_level		equ		4
; PI_levelsbak	equ		12
; PI_levelbak	equ		16
; PI_pry		equ		24
; PI_icw		equ		32
PI_IMR			equ		36
; PI_ocw3		equ		37
; PI_irr		equ		38
; PI_ext		equ		39
; PI_isr		equ		40
; PI_isrbak		equ		41
; PI_writeicw	equ		42
; PI_padding	equ		43
PI_SIZE			equ		44

PIC_MASTER		equ		0
PIC_SLAVE		equ		PI_SIZE
PIC_EXTIRQ		equ		(PI_SIZE * 2)


; #define PICEXISTINTR	((!pic.ext_irq) &&							\
;						((pic.pi[0].irr & (~pic.pi[0].imr)) ||		\
;						(pic.pi[1].irr & (~pic.pi[1].imr))))

	MACRO
$label	PICEXISTINTR
				mov		r12, #(&ff << 24)
				ldrb	r1, [r0, #PIC_EXTIRQ]
				ldr		r2, [r0, #(PIC_MASTER + PI_IMR)]
				ldr		r3, [r0, #(PIC_SLAVE + PI_IMR)]
				cmp		r1, #0
				eoreq	r0, r12, r2 lsl #24
				tsteq	r0, r2 lsl #8
				eoreq	r0, r12, r3 lsl #24
				tsteq	r0, r3 lsl #8
	MEND

	END


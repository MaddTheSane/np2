

; ---- dmac.h

				struc	dmach_t
.adrs			resd	1		; 00
.leng			resw	1		; 04
.adrsorg		resw	1		; 06
.lengorg		resw	1		; 08
.action			resw	1		; 0a
.outproc		resd	1		; 0c
.inproc			resd	1		; 10
.extproc		resd	1		; 14
.mode			resb	1		; 18
.sreq			resb	1		; 19
.ready			resb	1		; 1a
.mask			resb	1		; 1b
.size							; 1c
				endstruc

				struc	dmac_t
.dmach			resb	(dmach_t.size * 4)
.lh				resd	1
.work			resb	1
.working		resb	1
.mask			resb	1
.stat			resb	1
				endstruc

DMAEXT_START		equ		0
DMAEXT_END			equ		1
DMAEXT_BREAK		equ		2
DMA_INITSIGNALONLY	equ		1

	extern	_dmac


; ---- pic.h

				struc	picitem_t
.icw			resb	4
.imr			resb	1
.isr			resb	1
.irr			resb	1
.ocw3			resb	1
.pry			resb	1
.writeicw		resb	1
				resb	2
				endstruc

				struc	pic_t
.master			resb	picitem_t_size
.slave			resb	picitem_t_size
				endstruc

	extern	_pic



USE_EGCASM		equ		1
USE_HIMEM		equ		0110000h

EARG1			equ		4
EARG2			equ		(EARG1+4)
EARG3			equ		(EARG2+4)
EARG4			equ		(EARG3+4)

FC_ARG3			equ		4
FC_ARG4			equ		(FC_ARG3+4)
FC_ARG5			equ		(FC_ARG4+4)
FC_ARG6			equ		(FC_ARG5+4)


				struc	np2cfg_t
.uPD72020		resb	1
.DISPSYNC		resb	1
.RASTER			resb	1
.realpal		resb	1
				endstruc


VRAM_STEP		equ		100000h
VRAM_B			equ		0a8000h
VRAM_R			equ		0b0000h
VRAM_G			equ		0b8000h
VRAM_E			equ		0e0000h

VRAM0_B			equ		VRAM_B
VRAM0_R			equ		VRAM_R
VRAM0_G			equ		VRAM_G
VRAM0_E			equ		VRAM_E
VRAM1_B			equ		VRAM_B + VRAM_STEP
VRAM1_R			equ		VRAM_R + VRAM_STEP
VRAM1_G			equ		VRAM_G + VRAM_STEP
VRAM1_E			equ		VRAM_E + VRAM_STEP

%macro VRAMPOS	1
				and		%1, VRAM_STEP | 7fffh
%endmacro

FONT_ADRS		equ		110000h
ITF_ADRS		equ		1f8000h

SCRN_BUFSIZE	equ		04b000h

				struc	i286core_t
.reg			resw	14
.es_base		resd	1
.cs_base		resd	1
.ss_base		resd	1
.ds_base		resd	1
.ss_fix			resd	1
.ds_fix			resd	1
.adrsmask		resd	1
.prefix			resw	1
.trap			resb	1
.resetreq		resb	1
.prefetchque	resd	1
.GDTR			resb	6
.MSW			resw	1
.IDTR			resb	6
.LDTR			resw	1
.LDTRC			resb	6
.TR				resw	1
.TRC			resb	6
				resb	2

.cpu_type		resb	1
.itfbank		resb	1
.ram_d0			resw	1
.remainclock	resd	1
.baseclock		resd	1
.clock			resd	1

.ext			resd	1
.extsize		resd	1
.ems			resd	4
.repbak			resd	1
.inport			resd	1
				endstruc

				struc	vramop_t
.operate		resd	1
.tramwait		resd	1
.vramwait		resd	1
.grcgwait		resd	1
				endstruc

				struc	dsync_t
.text_vbp		resd	1
.textymax		resd	1
.grph_vbp		resd	1
.grphymax		resd	1

.scrnxpos		resd	1
.scrnxmax		resd	1
.scrnxextend	resd	1
.scrnymax		resd	1
.textvad		resd	1
.grphvad		resd	1
				endstruc



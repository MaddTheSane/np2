
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

GDCCMD_MAX		equ		32

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

				struc	gdcd_t
.para			resb	256
.fifo			resw	GDCCMD_MAX
.cnt			resw	1
.ptr			resb	1
.rcv			resb	1
.snd			resb	1
.cmd			resb	1
.paracb			resb	1
.reserved		resb	1
				endstruc

				struc	gdc_t
.m				resb	gdcd_t_size
.s				resb	gdcd_t_size
.mode1			resb	1
.mode2			resb	1
.clock			resb	1
.crt15khz		resb	1
.m_drawing		resb	1
.s_drawing		resb	1
.vsync			resb	1
.vsyncint		resb	1
.display		resb	1
.bitac			resb	1
.reserved		resb	2
.analog			resd	1
.palnum			resd	1
.degpal			resb	4
.anapal			resd	16
.dispclock		resd	1
.vsyncclock		resd	1
.rasterclock	resd	1
.hsyncclock		resd	1
				endstruc

				struc	gdcs_t
.access			resb	1
.disp			resb	1
.textdisp		resb	1
.msw_accessable	resb	1
.grphdisp		resb	1
.palchange		resb	1
.mode2			resb	1
				endstruc

				struc	grcg_t
.counter		resd	1
.mode			resw	1
.modereg		resb	2
.tile_b			resw	1
.tile_r			resw	1
.tile_g			resw	1
.tile_e			resw	1
				endstruc

				struc	cgwin_t
.low			resd	1
.high			resd	1
.writable		resb	1
				endstruc

				struc	vramop_t
.operate		resd	1
.tramwait		resd	1
.vramwait		resd	1
.grcgwait		resd	1
				endstruc

				struc	egc_t
.access			resw	1
.fgbg			resw	1
.ope			resw	1
.fg				resw	1
.mask			resw	1
.bg				resw	1
.sft			resw	1
.leng			resw	1
.lastvram		resd	2
.patreg			resd	2
.fgc			resd	2
.bgc			resd	2

.func			resd	1
.remain			resd	1
.stack			resd	1
.inptr			resd	1
.outptr			resd	1
.mask2			resw	1
.srcmask		resw	1
.srcbit			resb	1
.dstbit			resb	1
.sft8bitl		resb	1
.sft8bitr		resb	1

				resd	4
.buf			resb	(4096/8 + 4*4)
				resd	4
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



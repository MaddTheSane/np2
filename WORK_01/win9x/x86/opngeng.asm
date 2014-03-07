;=======================================================================
;	OPN(A)	Process
;
;	this file written for Microsoft Macro Assembler version 9.00
;			and Microsoft Visual C++.net 2008
;
;		re-maked by S.W.
;			2009.11.21	ちょっとだけpipe-lineの最適化
;			2010. 1.14	ハードウェアＬＦＯのエミュレートに対応
;
;=======================================================================
	.586p
	.model	flat,c

;---------------
;定数定義
FMDIV_BITS		equ		8
FMDIV_ENT		equ		(1 shl FMDIV_BITS)
FMVOL_SFTBIT		equ		4

SIN_BITS		equ		11
EVC_BITS		equ		10
ENV_BITS		equ		16
KF_BITS			equ		6
FREQ_BITS		equ		20
ENVTBL_BIT		equ		14
SINTBL_BIT		equ		14

TL_BITS			equ		(FREQ_BITS+2)
OPM_OUTSB		equ		(TL_BITS + 2 - 16)

SIN_ENT			equ		(1 shl SIN_BITS)
EVC_ENT			equ		(1 shl EVC_BITS)

EC_ATTACK		equ		0
EC_DECAY		equ		(EVC_ENT shl ENV_BITS)
EC_OFF			equ		((2 * EVC_ENT) shl ENV_BITS)

EM_ATTACK		equ		4
EM_DECAY1		equ		3
EM_DECAY2		equ		2
EM_RELEASE		equ		1
EM_OFF			equ		0


;---------------
;構造体定義
slot_t			struc	
detune1			dd	?		; 00
totallevel		dd	?		; 04
decaylevel		dd	?		; 08
attack			dd	?		; 0c
decay1			dd	?		; 10
decay2			dd	?		; 14
release			dd	?		; 18
freq_cnt		dd	?		; 1c
freq_inc		dd	?		; 20
multiple		dd	?		; 24
keyscale		db	?		; 28
env_mode		db	?		; 29
envraito		db	?		; 2a
ssgeg1			db	?		; 2b
env_cnt			dd	?		; 2c
env_end			dd	?		; 30
env_inc			dd	?		; 34
env_inc_attack		dd	?		; 38
env_inc_decay1		dd	?		; 3c
env_inc_decay2		dd	?		; 40
env_inc_rel		dd	?		; 44
amon			dd	?		; 48
slot_t			ends

ch_t			struc	
slot			db	(sizeof(slot_t) * 4)	dup(?)
algorithm		db	?
feedback		db	?
playing			db	?
outslot			db	?
op1fb			dd	?
connect1		dd	?
connect3		dd	?
connect2		dd	?
connect4		dd	?
keynote			dd	4	dup(?)
keyfunc			db	4	dup(?)
kcode			db	4	dup(?)
pan			db	?
extop			db	?
stereo			db	?
padding2		db	?
pms			dd	?
ams			dd	?
ch_t			ends
ch_t_off		equ	sizeof(slot_t) * 4

opngen_t		struc 
playchannels		dd	?
playing			dd	?
feedback2		dd	?
feedback3		dd	?
feedback4		dd	?
outdl			dd	?
outdc			dd	?
outdr			dd	?
calcremain		dd	?
lfo_freq_cnt		dd	?	;frequency count
lfo_freq_inc		dd	?	;frequency step
lfo_enable		dd	?	;LFO Enable / Disable
keyreg			db	12	dup(?)
opngen_t		ends

opncfg_t		struc 
calc1024		dd	?
fmvol			dd	?
ratebit			dd	?
vr_en			dd	?
vr_l			dd	?
vr_r			dd	?
envcurve		dd	(EVC_ENT*2 + 1)	dup(?)
sintable		dd	SIN_ENT		dup(?)
envtable		dd	EVC_ENT		dup(?)
sinshift		db	SIN_ENT		dup(?)
envshift		db	EVC_ENT		dup(?)
opncfg_t		ends

;---------------
;外部宣言

	extern	C	opngen	:opngen_t
	extern	C	opnch	:ch_t			;FM音源の構造体
	extern	C	opncfg	:opncfg_t		

;	extern	C	sinshift:byte
;	extern	C	envshift:byte

;ENVCURVE	equ	(opncfg + opncfg_t.envcurve)
;SINTABLE	equ	(opncfg + opncfg_t.sintable)
;ENVTABLE	equ	(opncfg + opncfg_t.envtable)

;---------------
;プロトタイプ宣言

opngen_getpcm	proto	near	stdcall,	hdl:ptr byte, buf:ptr DWORD, OPN_LENG:DWORD
opngen_getpcmvr	proto	near	stdcall,	hdl:ptr byte, buf:ptr DWORD, OPN_LENG:DWORD

.code

;===============================================================
;		オペレータ
;---------------------------------------------------------------
;◆引数
;	eax	スロットへの入力
;	edx	音量（TL込み）
;	edi	スロットの構造体
;◆返り値
;	eax	スロットの出力
;◆レジスタ
;	esi	ch
;	edi	ch->slot
;===============================================================
op_out	macro

	assume	edi:ptr slot_t

	add	eax, [edi].freq_cnt			;(u)	2
	mov	cl, opncfg.envshift[edx]		;(v)	1
	shr	eax, (FREQ_BITS - SIN_BITS)		;(u)		shrはu-pipe限定
	mov	edx, opncfg.envtable[edx*4]		;(v)	1
	and	eax, (SIN_ENT - 1)			;(u)	1
	add	cl, opncfg.sinshift[eax]		;(v)	2
	mov	eax, opncfg.sintable[eax*4]		;(u)	1
	imul	eax, edx				;(np)	1	ペアリング不可
	sar	eax, cl					;(np)		ペアリング不可
	endm
;===============================================================
;		エンベロープ計算
;---------------------------------------------------------------
;◆引数
;	edi	スロットの構造体
;◆返り値
;	edx	音量（TL込み）
;◆レジスタ
;	esi	ch
;	edi	ch->slot
;===============================================================
calcenv	macro	p3
							;()内は、CPU(586以降)のpipe。
	assume	edi:ptr slot_t

	;Freqency & Envlop
	mov	eax, [edi].env_cnt		;(u)	1
	mov	edx, [edi].freq_inc		;(v)	1
	add	eax, [edi].env_inc		;(u)	2
	add	[edi].freq_cnt, edx		;(v)	3

	;フェーズ チェンジ
	.if	( eax >= dword ptr [edi].env_end )
	   mov	dl, [edi].env_mode
	   .if		(dl == EM_ATTACK)		;AR(4)
		mov	[edi].env_mode, EM_DECAY1	;(u)	2
		mov	eax, [edi].decaylevel		;(v)	1
		mov	edx, [edi].env_inc_decay1	;(u)	1
		mov	[edi].env_end, eax		;(v)	2
		mov	[edi].env_inc, edx		;(u)	2
		mov	eax, EC_DECAY				;(v)	1
	   .elseif	(dl == EM_DECAY1)		;DR(3)
		mov	[edi].env_mode, EM_DECAY2	;(u)	2
		mov	edx, [edi].env_inc_decay2	;(v)	1
		mov	[edi].env_end, EC_OFF		;(u)	2
		mov	[edi].env_inc, edx		;(v)	2
		mov	eax, [edi].decaylevel		;(u)	1
	   .else					;SR(2) & RR(1) % OFF(0)
		.if	(dl == EM_RELEASE)
		  mov	[edi].env_mode, EM_OFF			;(u)	2
		.endif
		and	(ch_t ptr [esi]).playing, (not p3)	;(v)	3
		mov	eax, EC_OFF +1				;(u)	1	EC_ATTACK
		xor	edx, edx				;(v)	1
		mov	[edi].env_end, eax			;(u)	2
		mov	[edi].env_inc, edx			;(v)	2
		dec	eax					;(u)
	   .endif
	.endif
	;音量計算
	mov	[edi].env_cnt, eax			;(u)	2
	mov	edx, [edi].totallevel			;(v)	1
	shr	eax, ENV_BITS				;(u)		eax >> ENV_BITS
	sub	edx, opncfg.envcurve[eax*4]		;(u)	2	(直ぐにeaxを使う為、ペアリング不可)
	endm						;		でも、次は、jl命令(v-pipe)が来ている。
;===============================================================
;		
;---------------------------------------------------------------
;◆引数
;	ecx	void	*hdl		
;	edx	SINT32	*buf		
;	[ebp-4]	UINT	OPN_LENG	count
;◆返り値
;
;◆レジスタ
;	esi	chの構造体
;	edi	slotの構造体
;===============================================================
	align	16
opngen_getpcm	proc	near	stdcall	public	uses ebx esi edi,
		hdl		:ptr BYTE,
		buf		:ptr DWORD,
		OPN_LENG	:DWORD
	;Local変数
	local	OPN_SAMPL	:DWORD
	local	OPN_SAMPR	:DWORD
	local	LFO_Level	:DWORD
	local	LFO_LevelSft	:BYTE

	xor	ecx, ecx			;ecx = 0

   .if	(dword ptr opngen.playing != ecx)

	mov	ebx, opngen.calcremain

	align	16

	.while	(OPN_LENG > ecx)
	   mov	eax, opngen.outdl			;(u)	1
	   imul	eax, ebx				;(np)	1
	   mov	edx, opngen.outdr			;(u)	1
	   mov	OPN_SAMPL, eax				;(v)	2	OPN_SAMPL = opngen.outdl * opngen.calcremain
	   imul	edx, ebx				;(np)	1
	   mov	eax, ebx				;(u)	1
	   mov	OPN_SAMPR, edx				;(u)	2	OPN_SAMPL = opngen.outdr * opngen.calcremain
	   mov	ebx, FMDIV_ENT				;(v)	1		(256 = 2^8)
	   sub	ebx, eax				;(v)	1	ebx = FMDIV_ENT - opngen.calcremain

	   .repeat
		mov	opngen.playing, ecx		;(v)	2
		mov	opngen.outdl, ecx		;(u)	2
		mov	opngen.outdc, ecx		;(v)	2
		mov	opngen.outdr, ecx		;(u)	2
		lea	esi, [opnch + ch_t_off]		;(u)	1
		mov	opngen.calcremain, ebx		;(v)	2
		mov	ecx, opngen.playchannels	;(u)	1

		;ハードLFO-------------
		;変位の計算
		.if	(opngen.lfo_enable & 01h)
			mov	eax, opngen.lfo_freq_cnt
			add	eax, opngen.lfo_freq_inc
			mov	opngen.lfo_freq_cnt, eax
			shr	eax, (FREQ_BITS - SIN_BITS)
			and	eax, (SIN_ENT - 1)			;(u)	1
			mov	dl, opncfg.sinshift[eax]		;(v)	2
			mov	eax, opncfg.sintable[eax*4]		;(u)	1
		.else
			xor	eax, eax
			xor	edx, edx
		.endif
		mov	LFO_Level, eax
		mov	LFO_LevelSft, dl
		;-----------------------

		assume	edi:ptr slot_t

		;ch数だけ繰り返す。
		.while	(ecx != 0)
		   mov	al, (ch_t ptr [esi - ch_t_off]).outslot		;出力するslotはKeyOnされているか？
		   .if	(al & (byte ptr (ch_t ptr [esi - ch_t_off]).playing))
			push	ecx				;(u)	4
			lea	edi, [esi - ch_t_off]		;(v)	1
			xor	eax,eax				;(u)	1
			xor	ecx,ecx				;(v)	1	パーシャルレジスタ防止も兼ねる
			mov	opngen.feedback2, eax		;(u)	2
			mov	opngen.feedback3, eax		;(v)	2
			mov	opngen.feedback4, eax		;(u)	2
			mov	ch,(not 01h)			;(v)	1

			align	16

			;slot数だけ繰り返す。
			.repeat					;	jmp命令になる。
				;Freqency
				push	ecx
				mov	eax, [edi].freq_inc		;(v)	1
				mov	ebx, (ch_t ptr [esi - ch_t_off]).pms
				mov	ecx, eax
				imul	ebx, LFO_Level
				imul	ebx
				mov	eax, ecx
				mov	cl, LFO_LevelSft
				sar	edx, cl
				add	eax, edx
				add	[edi].freq_cnt, eax		;(v)	3
				pop	ecx

				;Envlop
				mov	eax, [edi].env_cnt		;(u)	1
				add	eax, [edi].env_inc		;(u)	2
				cmp	eax, dword ptr [edi].env_end	;2
				jnc	PhaseChange			;1
PhaseChangeEndPoint:
				mov	[edi].env_cnt, eax			;(u)	2
				mov	edx, [edi].totallevel			;(v)	1
				shr	eax, ENV_BITS				;(u)		eax >> ENV_BITS
				.if	(dword ptr [edi].amon & 01h)
					push	ecx
					mov	ebx, (ch_t ptr [esi - ch_t_off]).ams
					mov	cl, LFO_LevelSft
					imul	ebx, LFO_Level
					sar	ebx, cl
					add	edx, ebx
					pop	ecx
				.endif
				sub	edx, opncfg.envcurve[eax*4]		;(u)	2	(直ぐにeaxを使う為、ペアリング不可)
				jl	og_calcslot3				;(v)	1	条件ジャンプは(v)限定。
				push	ecx							;(u)	4
				.if	(cl == 0)						;(u→v)
					mov	ebx,(ch_t ptr [esi - ch_t_off]).op1fb		;(u)	1	with feedback
					mov	cl, (ch_t ptr [esi - ch_t_off]).feedback	;(v)	1	4PI=1 | 2PI=2 | PI=3 | PI/2=4 | PI/4=5 | PI/8=6 | PI/16=7 | Off=0
					.if	(cl == 0)					;(u→v)
						xor	eax,eax					;(u)	1
					.else							;(v)
						mov	eax,ebx					;(u)	1
						shr	eax,cl					;(u)	
					.endif							;(v)
					op_out
					mov	(ch_t ptr [esi - ch_t_off]).op1fb, eax		;(u)	2
					add	eax,ebx						;(v)	1
					sar	eax,1						;(np)
					.if	(byte ptr (ch_t ptr [esi - ch_t_off]).algorithm != 5)
						mov	ebx,(ch_t ptr [esi - ch_t_off]).connect1	; case ALG != 5
						add	[ebx], eax
					.else
						mov	opngen.feedback2, eax			;(u)	2	case ALG == 5
						mov	opngen.feedback3, eax			;(v)	2
						mov	opngen.feedback4, eax			;(u)	2	i586はこの方が早い
					.endif
				.else
					.if	(cl==1)
						mov	eax, opngen.feedback2				;(u)	1
						mov	ebx,(ch_t ptr [esi - ch_t_off]).connect2	; 08h	1
					.elseif	(cl==2)
						mov	eax, opngen.feedback3				;(u)	1
						mov	ebx,(ch_t ptr [esi - ch_t_off]).connect3	; 04h	1
					.else
						mov	eax, opngen.feedback4				;(u)	1
						mov	ebx,(ch_t ptr [esi - ch_t_off]).connect4	; 0Ch	1
					.endif
					op_out
					add	[ebx], eax	;	1
				.endif
				pop	ecx			;(u)	4
og_calcslot3:
				rol	ch,1			;(u)	
				add	edi, sizeof(slot_t)	;	1
				inc	cl			;	1
			.until	(cl >= 4)
			inc	opngen.playing			;	3
			pop	ecx				;	4
		   .endif
		   add	esi, sizeof(ch_t)			;(u)	1
		   dec	ecx					;(v)	1
		.endw
		mov	eax, opngen.outdc		;(u)	1
		mov	esi, opngen.outdl		;(v)	1
		mov	edi, opngen.outdr		;(u)	1
		add	esi, eax			;(v)	1
		sar	esi, FMVOL_SFTBIT		;(np)	-
		add	edi, eax			;(u)	1
		mov	ebx, opngen.calcremain		;(v)	1
		sar	edi, FMVOL_SFTBIT		;(np)	-
		mov	edx, opncfg.calc1024		;(u)	1
		mov	eax, ebx			;(v)	1
		mov	opngen.outdl,esi		;(u)	2
		sub	ebx, edx			;(v)	1	opngen.calcremain - opncfg.calc1024
		mov	opngen.outdr,edi		;(u)	2
		jbe	og_nextsamp			;(v)	1
		mov	eax, edx			;(u)	1
		mov	opngen.calcremain, ebx		;(v)	2
		imul	eax, esi			;(np)	1
		add	OPN_SAMPL, eax			;	2
		xor	ecx,ecx				;	1
		imul	edx, edi			;(np)	1
		add	OPN_SAMPR, edx			;(u)	2
	  .until	0				;(v)	
og_nextsamp:
	   mov	ecx, eax				;(u)	1	opngen.calcremain
	   imul	eax, esi				;(np)	1
	   add	eax, OPN_SAMPL				;(u)	2	eax = opngen.calcremain * opngen.outdl + OPN_SAMPL
	   mov	esi, buf				;(v)	1
	   imul	ecx, edi				;(np)	1
	   add	ecx, OPN_SAMPR				;(u)	2	ecx = opngen.calcremain * opngen.outdr + OPN_SAMPR
	   mov	edi, opncfg.fmvol			;(v)	1
	   imul	edi					;(np)	3	edx:eax = eax * opncfg.fmvol
	   add	[esi], edx				;(u)	3
	   mov	eax, ecx				;(v)	1
	   imul	edi					;(np)	3	edx:eax = ecx * opncfg.fmvol
	   add	[esi+4], edx				;(u)	3
	   neg	ebx					;(v)	1
	   xor	ecx, ecx				;(u)	1
	   add	buf, 8					;(v)	3
	   mov	opngen.calcremain, ebx			;(u)	2
	   dec	OPN_LENG				;(v)	2
	.endw

   .endif

og_noupdate:
	ret

;-------------------------------
	align	16

PhaseChange:
	;フェーズ チェンジ
	mov	dl, [edi].env_mode
	.if		(dl == EM_ATTACK)		;AR(4)
		mov	[edi].env_mode, EM_DECAY1		;(u)	2
		mov	eax, [edi].decaylevel			;(v)	1
		mov	edx, [edi].env_inc_decay1		;(u)	1
		mov	[edi].env_end, eax			;(v)	2
		mov	[edi].env_inc, edx			;(u)	2
		mov	eax, EC_DECAY				;(v)	1
		jmp	PhaseChangeEndPoint			;(v)	1
	.elseif		(dl == EM_DECAY1)		;DR(3)
		mov	[edi].env_mode, EM_DECAY2		;(u)	2
		mov	edx, [edi].env_inc_decay2		;(v)	1
		mov	[edi].env_end, EC_OFF			;(u)	2
		mov	[edi].env_inc, edx			;(v)	2
		mov	eax, [edi].decaylevel			;(u)	1
		jmp	PhaseChangeEndPoint			;(v)	1
	.else						;SR(2) & RR(1) % OFF(0)
		.if	(dl == EM_RELEASE)
		  mov	[edi].env_mode, EM_OFF			;(u)	2
		.endif
		and	(ch_t ptr [esi - ch_t_off]).playing, ch	;(v)	3
		mov	eax,EC_OFF + 1				;(u)	1
		xor	edx,edx					;(v)	1	EC_ATTACK
		mov	[edi].env_end, eax			;(u)	2
		mov	[edi].env_inc, edx			;(v)	2
		dec	eax					;(u)	1
		jmp	PhaseChangeEndPoint			;(v)	1
	.endif

opngen_getpcm	endp

;===============================================================
;		
;---------------------------------------------------------------
;◆引数
;	ecx	void	*hdl		
;	edx	SINT32	*buf		
;	[ebp-4]	UINT	OPN_LENG	count
;◆返り値
;
;◆レジスタ
;	esi	chの構造体
;	edi	slotの構造体
;===============================================================
	align	16
opngen_getpcmvr	proc	near	stdcall	public	uses	ebx esi edi,
		hdl		:ptr BYTE,
		buf		:ptr DWORD,
		OPNV_LENG	:DWORD
	;Local変数
	local	OPNV_SAMPL	:DWORD
	local	OPNV_SAMPR	:DWORD

	.if	(opncfg.vr_en == 0)
		invoke	opngen_getpcm	,hdl,buf,OPNV_LENG
		jmp	ogv_noupdate
	.endif

	cmp	OPNV_LENG,0
	je	ogv_noupdate

		mov	esi, buf
		mov	ebx, opngen.calcremain

ogv_fmout_st:	mov	eax, ebx
		imul	ebx, opngen.outdl
		mov	OPNV_SAMPL, ebx
		mov	ebx, FMDIV_ENT
		sub	ebx, eax
		imul	eax, opngen.outdr
		mov	OPNV_SAMPR, eax
		push	esi

ogv_fmout_lp:	xor	eax, eax
		mov	opngen.calcremain, ebx
		mov	opngen.outdl, eax
		mov	opngen.outdc, eax
		mov	opngen.outdr, eax
		mov	ch, byte ptr opngen.playchannels
		lea	edi, [opnch]
ogv_calcch_lp:	xor	eax, eax
		mov	opngen.feedback2, eax
		mov	opngen.feedback3, eax
		mov	opngen.feedback4, eax
		lea	esi, ch_t ptr [edi]
		calcenv	1		; slot1 calculate
		jl	ogv_calcslot3
		mov	cl, (ch_t ptr [esi]).feedback
		test	cl, cl
		je	ogv_nofeed
		mov	eax, (ch_t ptr [esi]).op1fb	; with feedback
		mov	ebx, eax
		shr	eax, cl
		op_out
		mov	(ch_t ptr [esi]).op1fb, eax
		add	eax, ebx
		sar	eax, 1
		jmp	ogv_algchk
ogv_nofeed:	xor	eax, eax			; without feedback
		op_out
ogv_algchk:	cmp	(ch_t ptr [esi]).algorithm, 5
		jne	ogv_calcalg5
		mov	opngen.feedback2, eax	; case ALG == 5
		mov	opngen.feedback3, eax
		mov	opngen.feedback4, eax
		jmp	ogv_calcslot3
ogv_calcalg5:	mov	ebx, (ch_t ptr [esi]).connect1	; case ALG != 5
		add	[ebx], eax
ogv_calcslot3:	add	edi, sizeof(slot_t)		; slot3 calculate
		calcenv	2
		jl	ogv_calcslot2
		mov	eax, opngen.feedback2
		op_out
		mov	ebx, (ch_t ptr [esi]).connect2
		add	[ebx], eax
ogv_calcslot2:	add	edi, sizeof(slot_t)		; slot2 calculate
		calcenv	4
		jl	ogv_calcslot4
		mov	eax, opngen.feedback3
		op_out
		mov	ebx, (ch_t ptr [esi]).connect3
		add	[ebx], eax
ogv_calcslot4:	add	edi, sizeof(slot_t)		; slot4 calculate
		calcenv	8
		jl	ogv_calcsloted
		mov	eax, opngen.feedback4
		op_out
		mov	ebx, (ch_t ptr [esi]).connect4
		add	[ebx], eax
ogv_calcsloted:	add	edi, (sizeof(ch_t) - (sizeof(slot_t) * 3))
		dec	ch
		jne	ogv_calcch_lp

		mov	eax, opngen.outdl
		mov	edx, opngen.outdc
		imul	eax, opncfg.vr_l
		mov	ebx, edx
		sar	eax, 5
		add	ebx, eax
		sar	eax, 2
		add	edx, eax
		mov	eax, opngen.outdr
		imul	eax, opncfg.vr_r
		sar	eax, 5
		add	edx, eax
		sar	eax, 2
		add	ebx, eax
		add	opngen.outdl, edx
		add	opngen.outdr, ebx
		sar	opngen.outdl, FMVOL_SFTBIT
		sar	opngen.outdr, FMVOL_SFTBIT
		mov	edx, opncfg.calc1024
		mov	ebx, opngen.calcremain
		mov	eax, ebx
		sub	ebx, edx
		jbe	ogv_nextsamp
		mov	opngen.calcremain, ebx
		mov	eax, edx
		imul	eax, opngen.outdl
		add	OPNV_SAMPL, eax
		imul	edx, opngen.outdr
		add	OPNV_SAMPR, edx
		jmp	ogv_fmout_lp
ogv_nextsamp:	
		pop	esi
		neg	ebx
		mov	opngen.calcremain, ebx
		mov	ecx, eax
		imul	eax, opngen.outdl
		add	eax, OPNV_SAMPL
		imul	opncfg.fmvol

		add	[esi], edx
		mov	eax, opngen.outdr
		imul	ecx
		add	eax, OPNV_SAMPR
		imul	opncfg.fmvol
		add	[esi+4], edx
		add	esi, 8
		dec	OPNV_LENG
		jne	ogv_fmout_st

ogv_noupdate:	ret

opngen_getpcmvr	endp
	end


PSGFREQPADBIT	equ		12
PSGADDEDBIT		equ		3
PSGADDEDCNT		equ		(1 << PSGADDEDBIT)

PSGENV_INC		equ		15
PSGENV_ONESHOT	equ		16
PSGENV_LASTON	equ		32
PSGENV_ONECYCLE	equ		64

T_FREQ			equ		0
T_COUNT			equ		4
T_PVOL			equ		8
T_PUCHI			equ		12
T_PAN			equ		14
T_SIZE			equ		16

P_TONE			equ		0
P_NOISE			equ		48
PN_FREQ			equ		48
PN_COUNT		equ		52
PN_BASE			equ		56
P_REG			equ		60
P_ENVCNT		equ		76
P_ENVMAX		equ		78

P_MIXER			equ		80
P_ENVMODE		equ		81
P_ENVVOL		equ		82
P_ENVVOLCNT		equ		83

P_PUCHICOUNT	equ		84

C_VOLUME		equ		0
C_VOLTBL		equ		64
C_RATE			equ		128
C_BASE			equ		132
C_PUCHIDEC		equ		136

CD_BIT31		equ		&80000000

; r0	psggen
; r1	Offset
; r2	Counter
; r3	Temporary Register
; r4	Temporary Register
; r5	Temporary Register
; r6	Temporary Register
; r7	L
; r8	R
; r9	noise
; r10	mixer
; r11	psgcfg Fix
; r12	Temporary Register
; lr	envcnt?

	IMPORT	__randseed
	IMPORT	psggencfg

	EXPORT	psggen_getpcm

	AREA	PSGGEN, CODE, READONLY


psggen_getpcm	ldrb	r12, [r0, #P_MIXER]
				tst		r12, #&3f
				bne		countcheck
				ldr		r3, [r0, #P_PUCHICOUNT]
				cmp		r2, r3
				movcs	r2, r3
				sub		r3, r3, r2
				str		r3, [r0, #P_PUCHICOUNT]
countcheck		cmp		r2, #0
				moveq	pc, lr

				stmdb	sp!, {r4 - r11, lr}
				ldr		r11, psgvoltbl
				ldrh	lr, [r0, #P_ENVCNT]

psgmake_lp		ldr		r10, [r0, #P_MIXER]
				cmp		lr, #0
				beq		makenoise
				subs	lr, lr, #1
				bne		makenoise
				bic		r10, r10, #(255 << 16)
				subs	r10, r10, #(1 << 24)
				bcs		calcenvnext
				tst		r10, #(PSGENV_ONESHOT << 8)
				beq		calcenvcyc
				tst		r10, #(PSGENV_LASTON << 8)
				orrne	r10, r10, #(15 << 16)
				b		calcenvstr
calcenvcyc		bic		r10, r10, #(240 << 24)
				tst		r10, #(PSGENV_ONECYCLE << 8)
				eoreq	r10, r10, #(PSGENV_INC << 8)
calcenvnext		ldrh	lr, [r0, #P_ENVMAX]
				eor		r3, r10, r10, lsr #16
				and		r3, r3, #(15 << 8)
				orr		r10, r10, r3 lsl #8
calcenvstr		str		r10, [r0, #P_MIXER]

makenoise		tst		r10, #&38
				beq		makesamp
				ldr		r6, [r0, #PN_FREQ]
				ldr		r7, [r0, #PN_COUNT]
				ldr		r8, [r0, #PN_BASE]
				mov		r9, #0
				mov		r3, #PSGADDEDCNT
mknoise_lp		subs	r7, r7, r6
				bcc		updatenoise
updatenoiseret	add		r9, r8, r9, lsl #1
				subs	r3, r3, #1
				bne		mknoise_lp
				str		r7, [r0, #PN_COUNT]

makesamp		mov		r7, #0
				mov		r8, #0
				and		r10, r10, #&3f
				add		r10, r10, #(8 << 8)
makesamp_lp		ldr		r12, [r0, #T_PVOL]
				ldrb	r12, [r12]
				ands	r12, r12, #15
				beq		makesamp_nt
				ldr		r12, [r11, r12, lsl #2]
				mov		r3, #0
				tst		r10, #8
				bne		calcwithnoise
				tst		r10, #1
				beq		calcpuchionly
calctone		ldr		r4, [r0, #T_COUNT]
				ldr		r5, [r0, #T_FREQ]
				mov		r6, #PSGADDEDCNT
calctone_lp		add		r4, r4, r5
				cmp		r4, #0
				addge	r3, r3, r12
				sublt	r3, r3, r12
				subs	r6, r6, #1
				bne		calctone_lp
				str		r4, [r0, #T_COUNT]
				b		calcpanpot
calcpuchionly	ldrb	r4, [r0, #T_PUCHI]
				subs	r4, r4, #1
				strcsb	r4, [r0, #T_PUCHI]
				addcs	r3, r3, r12, lsl #PSGADDEDBIT
				b		calcpanpot
calcwithnoise	tst		r10, #1
				bne		calcboth
calcnoise		mov		r4, #(1 << (32 - PSGADDEDCNT))
calcnoise_lp	tst		r9, r4
				addeq	r3, r3, r12
				subne	r3, r3, r12
				movs	r4, r4, lsl #1
				bne		calcnoise_lp
				b		calcpanpot
calcboth		ldr		r4, [r0, #T_COUNT]
				ldr		r5, [r0, #T_FREQ]
				add		r6, r9, #1
calcboth_lp		add		r4, r4, r5
				tst		r4, r6
				addpl	r3, r3, r12
				submi	r3, r3, r12
				mov		r6, r6, lsl #1
				tst		r6, #(1 << PSGADDEDCNT)
				beq		calcboth_lp
				str		r4, [r0, #T_COUNT]
calcpanpot		ldrb	r4, [r0, #T_PAN]
				tst		r4, #1
				addeq	r7, r7, r3
				tst		r4, #2
				addeq	r8, r8, r3
makesamp_nt		mov		r10, r10, lsr #1
				add		r0, r0, #T_SIZE
				tst		r10, #(1 << 8)
				beq		makesamp_lp
				sub		r0, r0, #(T_SIZE * 3)

				ldr		r4, [r1]
				add		r4, r4, r7
				str		r4, [r1], #4
				ldr		r4, [r1]
				add		r4, r4, r8
				str		r4, [r1], #4
				subs	r2, r2, #1
				bne		psgmake_lp

				strh	lr, [r0, #P_ENVCNT]
				ldmia	sp!, {r4 - r11, pc}

psgvoltbl		dcd		psggencfg + C_VOLUME

updatenoise		ldr		r4, randdcd
				ldr		r8, [r4]
				ldr		r12, randdcd1
				mul		r12, r8, r12
				ldr		r8, randdcd2
				add		r8, r8, r12
				str		r8, [r4]
				and		r8, r8, #(1 << (32 - PSGADDEDCNT))
				str		r8, [r0, #PN_BASE]
				b		updatenoiseret
randdcd			dcd		__randseed
randdcd1		dcd		&343fd
randdcd2		dcd		&269ec3

	END


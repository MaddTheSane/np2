
; ÇªÇÍà»ëOÇ…ÉÅÉÇÉäÇ™íxÇ¢Ç©ÇÁ Cî≈Ç∆Ç†ÇÒÇ‹ÇËïœÇÁÇ»Ç¢ÇÊÇ§Ç»Åc


NP2PALS_TXT		equ		10
NP2PALS_GRPH	equ		16

NP2PAL_TEXT		equ		0
NP2PAL_SKIP		equ		NP2PALS_TXT
NP2PAL_GRPH		equ		(NP2PAL_SKIP + NP2PALS_GRPH)
NP2PAL_TEXT2	equ		(NP2PAL_GRPH + NP2PALS_GRPH)

SURFACE_WIDTH	equ		640

S_SRC			equ		0
S_SRC2			equ		4
S_DST			equ		8
S_WIDTH			equ		12
S_XBYTES		equ		16
S_Y				equ		20
S_XALIGN		equ		24
S_YALIGN		equ		28
S_DIRTY			equ		32

	IMPORT	np2_pal16

	EXPORT	sdraw_getproctbl

	AREA	SDRAWQ16, CODE, READONLY

qvga16p_0		stmdb	sp!, {r4 - r8, lr}
				ldr		r6, [r0, #S_SRC]
				ldr		r7, [r0, #S_DST]
				ldr		r8, [r0, #S_WIDTH]
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				ldr		lr, pal16_0
				ldr		lr, [lr]
				add		lr, lr, lr, lsr #16
				mov		lr, lr, lsr #16
				add		lr, lr, lr, lsl #16
				add		r0, r0, #S_DIRTY
putylp_0		ldrh	r12, [r0, r4]
				cmp		r12, #0
				beq		putyed_0
				mov		r12, r7
				mov		r2, #0
putxlp_0		strh	lr, [r12], r3
				strh	lr, [r12], r3
				add		r2, r2, #4
				cmp		r2, r8
				bcc		putxlp_0
putyed_0		add		r4, r4, #2
				add		r6, r6, #(SURFACE_WIDTH * 2)
				add		r7, r7, r5
				cmp		r4, r1
				bcc		putylp_0
				str		r6, [r0, #(S_SRC - S_DIRTY)]
				str		r7, [r0, #(S_DST - S_DIRTY)]
				str		r4, [r0, #(S_Y - S_DIRTY)]
				ldmia	sp!, {r4 - r8, pc}
pal16_0			dcd		(np2_pal16 + (NP2PAL_TEXT2 * 4))

qvga16p_1		stmdb	sp!, {r4 - r11, lr}
				ldr		r11, pal16_1
				ldr		lr, pmask_1
				ldr		r7, [r0, #S_SRC]
				ldr		r8, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				add		r0, r0, #S_DIRTY
putylp_1		ldrh	r12, [r0, r4]
				cmp		r12, #0
				beq		putyed_1
				str		r4, [r0, #(S_Y - S_DIRTY)]
				mov		r9, r8
				mov		r2, #0
putxlp_1		ldr		r4, [r7, r2]
				and		r12, r4, #255
				ldr		r5, [r11, r12, lsl #2]
				mov		r12, r4, lsr #8
				and		r12, r12, #255
				ldr		r12, [r11, r12, lsl #2]
				add		r5, r5, r12
				mov		r12, r4, lsr #16
				and		r12, r12, #255
				ldr		r6, [r11, r12, lsl #2]
				mov		r12, r4, lsr #24
				ldr		r12, [r11, r12, lsl #2]
				add		r6, r6, r12
				add		r12, r2, #SURFACE_WIDTH
				ldr		r4, [r7, r12]
				and		r12, r4, #255
				ldr		r12, [r11, r12, lsl #2]
				add		r5, r5, r12
				mov		r12, r4, lsr #8
				and		r12, r12, #255
				ldr		r12, [r11, r12, lsl #2]
				add		r5, r5, r12
				mov		r12, r4, lsr #16
				and		r12, r12, #255
				ldr		r12, [r11, r12, lsl #2]
				add		r6, r6, r12
				mov		r12, r4, lsr #24
				ldr		r12, [r11, r12, lsl #2]
				add		r6, r6, r12
				and		r5, r5, lr
				mov		r12, r5, lsr #2
				add		r12, r12, r5, lsr #18
				strh	r12, [r9], r3
				and		r6, r6, lr
				mov		r12, r6, lsr #2
				add		r12, r12, r6, lsr #18
				strh	r12, [r9], r3
				add		r2, r2, #4
				cmp		r2, r10
				bcc		putxlp_1
				ldr		r4, [r0, #(S_Y - S_DIRTY)]
				ldr		r5, [r0, #(S_YALIGN - S_DIRTY)]
putyed_1		add		r4, r4, #2
				add		r7, r7, #(SURFACE_WIDTH * 2)
				add		r8, r8, r5
				cmp		r4, r1
				bcc		putylp_1
				str		r7, [r0, #(S_SRC - S_DIRTY)]
				str		r8, [r0, #(S_DST - S_DIRTY)]
				str		r4, [r0, #(S_Y - S_DIRTY)]
				ldmia	sp!, {r4 - r11, pc}
pal16_1			dcd		(np2_pal16 + (NP2PAL_GRPH * 4))
pmask_1			dcd		(&07e0f81f << 2)

qvga16p_2		stmdb	sp!, {r4 - r11, lr}
				ldr		r11, pal16_2
				ldr		lr, pmask_2
				ldr		r7, [r0, #S_SRC]
				ldr		r8, [r0, #S_SRC2]
				ldr		r9, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				add		r0, r0, #S_DIRTY
putylp_2		ldrh	r12, [r0, r4]
				cmp		r12, #0
				beq		putyed_2
				str		r9, [r0, #(S_DST - S_DIRTY)]
				str		r4, [r0, #(S_Y - S_DIRTY)]
				mov		r2, #0
putxlp_2		ldr		r4, [r7, r2]
				ldr		r12, [r8, r2]
				add		r4, r4, r12
				and		r12, r4, #255
				ldr		r5, [r11, r12, lsl #2]
				mov		r12, r4, lsr #8
				and		r12, r12, #255
				ldr		r12, [r11, r12, lsl #2]
				add		r5, r5, r12
				mov		r12, r4, lsr #16
				and		r12, r12, #255
				ldr		r6, [r11, r12, lsl #2]
				mov		r12, r4, lsr #24
				ldr		r12, [r11, r12, lsl #2]
				add		r6, r6, r12
				add		r12, r2, #SURFACE_WIDTH
				ldr		r4, [r7, r12]
				ldr		r12, [r8, r12]
				add		r4, r4, r12
				and		r12, r4, #255
				ldr		r12, [r11, r12, lsl #2]
				add		r5, r5, r12
				mov		r12, r4, lsr #8
				and		r12, r12, #255
				ldr		r12, [r11, r12, lsl #2]
				add		r5, r5, r12
				mov		r12, r4, lsr #16
				and		r12, r12, #255
				ldr		r12, [r11, r12, lsl #2]
				add		r6, r6, r12
				mov		r12, r4, lsr #24
				ldr		r12, [r11, r12, lsl #2]
				add		r6, r6, r12
				and		r5, r5, lr
				mov		r12, r5, lsr #2
				add		r12, r12, r5, lsr #18
				strh	r12, [r9], r3
				and		r6, r6, lr
				mov		r12, r6, lsr #2
				add		r12, r12, r6, lsr #18
				strh	r12, [r9], r3
				add		r2, r2, #4
				cmp		r2, r10
				bcc		putxlp_2
				ldr		r9, [r0, #(S_DST - S_DIRTY)]
				ldr		r4, [r0, #(S_Y - S_DIRTY)]
				ldr		r5, [r0, #(S_YALIGN - S_DIRTY)]
putyed_2		add		r4, r4, #2
				add		r7, r7, #(SURFACE_WIDTH * 2)
				add		r8, r8, #(SURFACE_WIDTH * 2)
				add		r9, r9, r5
				cmp		r4, r1
				bcc		putylp_2
				str		r7, [r0, #(S_SRC - S_DIRTY)]
				str		r8, [r0, #(S_SRC2 - S_DIRTY)]
				str		r9, [r0, #(S_DST - S_DIRTY)]
				str		r4, [r0, #(S_Y - S_DIRTY)]
				ldmia	sp!, {r4 - r11, pc}
pal16_2			dcd		(np2_pal16 + (NP2PAL_GRPH * 4))
pmask_2			dcd		(&07e0f81f << 2)

qvga16p_gi		stmdb	sp!, {r4 - r11, lr}
				ldr		r11, pal16_gi
				ldr		lr, pmask_gi
				ldr		r7, [r0, #S_SRC]
				ldr		r8, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				add		r0, r0, #S_DIRTY
putylp_gi		ldrh	r12, [r0, r4]
				cmp		r12, #0
				beq		putyed_gi
				str		r4, [r0, #(S_Y - S_DIRTY)]
				mov		r9, r8
				mov		r2, #0
putxlp_gi		ldr		r4, [r7, r2]
				and		r12, r4, #255
				ldr		r5, [r11, r12, lsl #2]
				mov		r12, r4, lsr #8
				and		r12, r12, #255
				ldr		r12, [r11, r12, lsl #2]
				add		r5, r5, r12
				mov		r12, r4, lsr #16
				and		r12, r12, #255
				ldr		r6, [r11, r12, lsl #2]
				mov		r12, r4, lsr #24
				ldr		r12, [r11, r12, lsl #2]
				add		r6, r6, r12
				and		r5, r5, lr
				mov		r12, r5, lsr #1
				add		r12, r12, r5, lsr #17
				strh	r12, [r9], r3
				and		r6, r6, lr
				mov		r12, r6, lsr #1
				add		r12, r12, r6, lsr #17
				strh	r12, [r9], r3
				add		r2, r2, #4
				cmp		r2, r10
				bcc		putxlp_gi
				ldr		r4, [r0, #(S_Y - S_DIRTY)]
				ldr		r5, [r0, #(S_YALIGN - S_DIRTY)]
putyed_gi		add		r4, r4, #2
				add		r7, r7, #(SURFACE_WIDTH * 2)
				add		r8, r8, r5
				cmp		r4, r1
				bcc		putylp_gi
				str		r7, [r0, #(S_SRC - S_DIRTY)]
				str		r8, [r0, #(S_DST - S_DIRTY)]
				str		r4, [r0, #(S_Y - S_DIRTY)]
				ldmia	sp!, {r4 - r11, pc}
pal16_gi		dcd		(np2_pal16 + (NP2PAL_GRPH * 4))
pmask_gi		dcd		(&07e0f81f << 1)

qvga16p_2i		stmdb	sp!, {r4 - r11, lr}
				ldr		r11, pal16_2i
				ldr		lr, pmask_2i
				ldr		r7, [r0, #S_SRC]
				ldr		r8, [r0, #S_SRC2]
				ldr		r9, [r0, #S_DST]
				ldr		r10, [r0, #S_WIDTH]
				ldr		r3, [r0, #S_XALIGN]
				ldr		r4, [r0, #S_Y]
				ldr		r5, [r0, #S_YALIGN]
				add		r0, r0, #S_DIRTY
putylp_2i		ldrh	r12, [r0, r4]
				cmp		r12, #0
				beq		putyed_2i
				str		r9, [r0, #(S_DST - S_DIRTY)]
				str		r4, [r0, #(S_Y - S_DIRTY)]
				mov		r2, #0
putxlp_2i		add		r6, r2, #SURFACE_WIDTH
				ldr		r4, [r7, r2]
				ldr		r6, [r8, r6]
				ands	r12, r6, #(&f0 << 0)
					movne	r12, r12, lsr #4
					addne	r12, r12, #(NP2PAL_TEXT - NP2PAL_GRPH)
					andeq	r12, r4, #255
				ldr		r5, [r11, r12, lsl #2]
				ands	r12, r6, #(&f0 << 8)
					movne	r12, r12, lsr #12
					addne	r12, r12, #(NP2PAL_TEXT - NP2PAL_GRPH)
					moveq	r12, r4, lsr #8
					andeq	r12, r12, #255
				ldr		r12, [r11, r12, lsl #2]
				add		r5, r5, r12
				ands	r12, r6, #(&f0 << 16)
					movne	r12, r12, lsr #20
					addne	r12, r12, #(NP2PAL_TEXT - NP2PAL_GRPH)
					moveq	r12, r4, lsr #16
					andeq	r12, r12, #255
				ldr		r12, [r11, r12, lsl #2]
				movs	r6, r6, lsr #28
					addne	r6, r6, #(NP2PAL_TEXT - NP2PAL_GRPH)
					moveq	r6, r4, lsr #24
				ldr		r6, [r11, r6, lsl #2]
				add		r6, r6, r12
		;;		ldr		r4, [r7, r2]
				ldr		r12, [r8, r2]
				add		r4, r4, r12
				and		r12, r4, #255
				ldr		r12, [r11, r12, lsl #2]
				add		r5, r5, r12
				mov		r12, r4, lsr #8
				and		r12, r12, #255
				ldr		r12, [r11, r12, lsl #2]
				add		r5, r5, r12
				mov		r12, r4, lsr #16
				and		r12, r12, #255
				ldr		r12, [r11, r12, lsl #2]
				add		r6, r6, r12
				mov		r12, r4, lsr #24
				ldr		r12, [r11, r12, lsl #2]
				add		r6, r6, r12
				and		r5, r5, lr
				mov		r12, r5, lsr #2
				add		r12, r12, r5, lsr #18
				strh	r12, [r9], r3
				and		r6, r6, lr
				mov		r12, r6, lsr #2
				add		r12, r12, r6, lsr #18
				strh	r12, [r9], r3
				add		r2, r2, #4
				cmp		r2, r10
				bcc		putxlp_2i
				ldr		r9, [r0, #(S_DST - S_DIRTY)]
				ldr		r4, [r0, #(S_Y - S_DIRTY)]
				ldr		r5, [r0, #(S_YALIGN - S_DIRTY)]
putyed_2i		add		r4, r4, #2
				add		r7, r7, #(SURFACE_WIDTH * 2)
				add		r8, r8, #(SURFACE_WIDTH * 2)
				add		r9, r9, r5
				cmp		r4, r1
				bcc		putylp_2i
				str		r7, [r0, #(S_SRC - S_DIRTY)]
				str		r8, [r0, #(S_SRC2 - S_DIRTY)]
				str		r9, [r0, #(S_DST - S_DIRTY)]
				str		r4, [r0, #(S_Y - S_DIRTY)]
				ldmia	sp!, {r4 - r11, pc}
pal16_2i		dcd		(np2_pal16 + (NP2PAL_GRPH * 4))
pmask_2i		dcd		(&07e0f81f << 2)


sdraw_getproctbl
				mov		r0, pc
				mov		pc, lr
				dcd		qvga16p_0
				dcd		qvga16p_1
				dcd		qvga16p_1
				dcd		qvga16p_2
				dcd		qvga16p_0
				dcd		qvga16p_1
				dcd		qvga16p_gi
				dcd		qvga16p_2i
				dcd		qvga16p_0
				dcd		qvga16p_1
				dcd		qvga16p_gi
				dcd		qvga16p_2i

	END



I286_MEMREADMAX		equ		&a4000
I286_MEMWRITEMAX	equ		&a0000

USE_HIMEM			equ		&110000

; cpu_reg			equ		0
; cpu_remainclock	equ		28
; cpu_baseclock		equ		32
; cpu_clock			equ		36
; cpu_adrsmask		equ		40
; cpu_es_base		equ		44
; cpu_cs_base		equ		48
; cpu_ss_base		equ		52
; cpu_ds_base		equ		56
; cpu_ss_fix		equ		60
; cpu_ds_fix		equ		64
; cpu_prefix		equ		68
; cpu_trap			equ		70
; cpu_type			equ		71
; cpu_pf_semaphore	equ		72
; cpu_repbak		equ		76
; cpu_inport		equ		80
; cpu_ovflag		equ		84
; cpu_GDTR			equ		88
; cpu_IDTR			equ		94
; cpu_MSW			equ		100
; cpu_resetreq		equ		102
; cpu_itfbank		equ		103
CPU_EXTMEM			equ		104
CPU_EXTMEMSIZE		equ		108
CPU_MAINMEM			equ		112


;	IMPORT	mem
	IMPORT	memfn
	IMPORT	i286core
	IMPORT	i286_nonram_r
	IMPORT	i286_nonram_rw

	EXPORT	i286_memoryread
	EXPORT	i286_memoryread_w
	EXPORT	i286_memorywrite
	EXPORT	i286_memorywrite_w

	MACRO
$label	MEMADR	$offset
$label	dcd		(i286core + CPU_MAINMEM) $offset
	MEND


	AREA	.text, CODE, READONLY


i286_memoryread
				cmp		r0, #I286_MEMREADMAX
				bcs		i2mr_ext
				ldr		r1, i2mr_mem
				ldrb	r0, [r0, r1]
				mov		pc, lr
i2mr_mem		MEMADR											; !!
i2mr_ext		cmp		r0, #USE_HIMEM
				bcs		i2mr_himem
				ldr		r3, i2mr_memfnrd8
				and		r1, r0, #(&1f << 15)
				ldr		pc, [r3, r1, lsr #(15 - 2)]
i2mr_memfnrd8	dcd		memfn
i2mr_himem		sub		r1, r0, #&100000
				ldr		r3, i2mr_cpu
				ldr		r12, [r3, #CPU_EXTMEMSIZE]
				cmp		r1, r12
				bcs		i286_nonram_r
				ldr		r12, [r3, #CPU_EXTMEM]
				ldrb	r0, [r1, r12]
				mov		pc, lr
i2mr_cpu		dcd		i286core								; !!



i286_memoryread_w
				tst		r0, #1
				bne		i2mro_main
				cmp		r0, #I286_MEMREADMAX
				bcs		i2mre_ext
				ldr		r1, i2mrw_mem
				ldrh	r0, [r0, r1]
				mov		pc, lr
i2mrw_mem		MEMADR
i2mre_ext		cmp		r0, #USE_HIMEM
				bcs		i2mre_himem
i2mrw_ext		ldr		r3, i2mre_memfnrd16
				and		r1, r0, #(&1f << 15)
				ldr		pc, [r3, r1, lsr #(15 - 2)]
i2mre_memfnrd16	dcd		memfn + (32 * 4) * 2
i2mre_himem		sub		r1, r0, #&100000
				ldr		r3, i2mre_cpu
				ldr		r12, [r3, #CPU_EXTMEMSIZE]
				cmp		r1, r12
				bcs		i286_nonram_rw
				ldr		r12, [r3, #CPU_EXTMEM]
				ldrh	r0, [r1, r12]
				mov		pc, lr
i2mre_cpu		dcd		i286core								; !!
i2mro_main		add		r1, r0, #1
				cmp		r1, #I286_MEMREADMAX
				bcs		i2mro_ext
				ldr		r1, i2mrw_mem
				add		r1, r1, r0
				ldrb	r0, [r1]
				ldrb	r1, [r1, #1]
				add		r0, r0, r1 lsl #8
				mov		pc, lr
i2mro_ext		cmp		r1, #USE_HIMEM
				bcs		i2mro_himem
				movs	r1, r1, lsl #(32 - 15)
				bne		i2mrw_ext
				stmdb	sp!, {r4 - r6, lr}			; ここチェックするように…
				ldr		r4, i2mro_memfnrd8
				add		r5, r0, #1
					and		r1, r0, #(&1f << 15)
					mov		lr, pc
					ldr		pc, [r4, r1, lsr #(15 - 2)]
				mov		r6, r0
				mov		r0, r5
					and		r1, r0, #(&1f << 15)
					mov		lr, pc
					ldr		pc, [r4, r1, lsr #(15 - 2)]
				add		r0, r6, r0 lsl #8
				ldmia	sp!, {r4 - r6, pc}
i2mro_memfnrd8	dcd		memfn
i2mro_himem		sub		r1, r1, #&100000
				ldr		r3, i2mro_cpu
				ldr		r2, [r3, #CPU_EXTMEM]
				ldr		r12, [r3, #CPU_EXTMEMSIZE]
				add		r2, r2, r1
				beq		i2mro_10ffff			; = 10ffff
				cmp		r1, r12
				bcs		i2mro_himeml			; = over
				ldrb	r0, [r2, #-1]
				ldrb	r1, [r2]
				add		r0, r0, r1 lsl #8
				mov		pc, lr
i2mro_10ffff	ldr		r0, i2mro_mem10ffff
				ldrb	r0, [r0]
				cmp		r1, r12
				bcs		i2mro_himemh
				ldrb	r1, [r2]
				add		r0, r0, r1 lsl #8
				mov		pc, lr
i2mro_himeml	ldreqb	r0, [r2, #-1]
				movne	r0, #&ff
i2mro_himemh	add		r0, r0, #&ff00
				mov		pc, lr
i2mro_cpu		dcd		i286core								; !!
i2mro_mem10ffff	MEMADR	+ USE_HIMEM - 1



i286_memorywrite
				cmp		r0, #I286_MEMWRITEMAX
				bcs		i2mw_ext
				ldr		r2, i2mw_mem
				strb	r1, [r0, r2]
				mov		pc, lr
i2mw_mem		MEMADR											; !!
i2mw_ext		cmp		r0, #USE_HIMEM
				bcs		i2mw_himem
				ldr		r3, i2mw_memfnwr8
				and		r2, r0, #(&1f << 15)
				ldr		pc, [r3, r2, lsr #(15 - 2)]
i2mw_memfnwr8	dcd		memfn + (32 * 4)
i2mw_himem		sub		r2, r0, #&100000
				ldr		r3, i2mw_cpu
				ldr		r12, [r3, #CPU_EXTMEMSIZE]
				ldr		r3, [r3, #CPU_EXTMEM]
				cmp		r2, r12
				strccb	r1, [r2, r3]
				mov		pc, lr
i2mw_cpu		dcd		i286core								; !!



	if 1
i286_memorywrite_w
				tst		r0, #1
				bne		i2mwo_main
				cmp		r0, #I286_MEMWRITEMAX
				bcs		i2mwe_ext
				ldr		r2, i2mww_mem
				strh	r1, [r0, r2]
				mov		pc, lr
i2mww_mem		MEMADR											; !!
i2mwe_ext		cmp		r0, #USE_HIMEM
				bcs		i2mwe_himem
i2mww_ext		ldr		r3, i2mwe_memfnwr16
				and		r2, r0, #(&1f << 15)
				ldr		pc, [r3, r2, lsr #(15 - 2)]
i2mwe_memfnwr16	dcd		memfn + (32 * 4) * 3
i2mwe_himem		sub		r2, r0, #&100000
				ldr		r3, i2mwe_cpu
				ldr		r12, [r3, #CPU_EXTMEMSIZE]
				ldr		r3, [r3, #CPU_EXTMEM]
				cmp		r2, r12
				strcch	r1, [r2, r3]
				mov		pc, lr
i2mwe_cpu		dcd		i286core								; !!

i2mwo_main		add		r2, r0, #1
				cmp		r2, #I286_MEMWRITEMAX
				bcs		i2mwo_ext
				ldr		r2, i2mww_mem
				add		r2, r2, r0
				strb	r1, [r2]
				mov		r1, r1, lsr #8
				strb	r1, [r2, #1]
				mov		pc, lr

i2mwo_ext		cmp		r2, #USE_HIMEM
				bcs		i2mwo_himem
				movs	r2, r2, lsl #(32 - 15)
				bne		i2mww_ext

				ldr		r2, i2mwo_memfnwr8
				stmdb	sp!, {r0 - r1, lr}			; ここチェックするように…
					and		r3, r0, #(&1f << 15)
					mov		lr, pc
					ldr		pc, [r2, r3, lsr #(15 - 2)]
				ldmia	sp!, {r0 - r1, lr}
				ldr		r2, i2mwo_memfnwr8
				add		r0, r0, #1
				mov		r1, r1, lsr #8
					and		r3, r0, #(&1f << 15)
					ldr		pc, [r2, r3, lsr #(15 - 2)]
i2mwo_memfnwr8	dcd		memfn + (32 * 4)

i2mwo_himem		sub		r2, r2, #&100000
				ldr		r3, i2mwo_cpu
				ldr		r12, [r3, #CPU_EXTMEM]
				ldr		r3, [r3, #CPU_EXTMEMSIZE]
				add		r12, r12, r2
				beq		i2mwo_10ffff			; = 10ffff
				cmp		r2, r3
				strlsb	r1, [r12, #-1]
				mov		r1, r1, lsr #8
				strccb	r1, [r12]
				mov		pc, lr
i2mwo_10ffff	ldr		r2, i2mwo_mem10ffff
				strb	r1, [r12]
				cmp		r3, #0
				mov		r1, r1, lsr #8
				strneb	r1, [r12]
				mov		pc, lr
i2mwo_cpu		dcd		i286core								; !!
i2mwo_mem10ffff	MEMADR	+ USE_HIMEM - 1
	endif


	END


.186

CODE	SEGMENT
		ASSUME CS:CODE,DS:nothing,ES:nothing,SS:nothing
		ORG 0000H

FIXCS		equ		2eh

START:			retf						; 00
				nop
				nop
				retf						; 03
				nop
				nop
				retf						; 06
				nop
				nop
				db	055h, 0aah, 002h		; 09
				jmp		short sasi_init1	; 0c
				nop
				jmp		short sasi_init2	; 0f
				nop
				retf						; 12
				nop
				nop
				jmp		short sasi_boot		; 15
				nop
				jmp		short sasi_bios		; 18
				nop
				retf						; 1b
				nop
				nop
				retf						; 1e
				nop
				nop
				retf						; 21
				nop
				nop
				retf						; 24
				nop
				nop
				retf						; 27
				nop
				nop
				retf						; 2a
				nop
				nop
				retf						; 2d
				nop
				nop

sasi_init1:		mov		byte ptr [bx], 0d9h
				retf

sasi_init2:		mov		ax, cs
				mov		ds:[04b0h], ah
				mov		ds:[04b8h], ah
				mov		ax, 0300h
				int		1bh
				retf

sasi_bios:		cld
				mov		dx, cs
				mov		ds, dx
				mov		cx, 8
				mov		si, offset sasibiosstr
				mov		dx, 07efh
				cli
				rep outsb
				sti
				pop		ax
				pop		bx
				pop		cx
				pop		dx
				pop		bp
				pop		es
				pop		di
				pop		si
				pop		ds
				iret

sasi_boot:		cmp		al, 0ah
				je		short sasi_boot2
				cmp		al, 0bh
				je		short sasi_boot2
				retf

sasi_boot2:		sub		al, 9
				mov		dl, 0ffh
				test	ds:[055dh], al			; sasi
				je		short boot_exit
				dec		al
				mov		ah, 06h
				mov		cx, 1fc0h
				mov		es, cx
				mov		bp, bx
				mov		cx, bx
				mov		dx, bx
				mov		bh, 4
				int		1bh
				jc		short boot_exit
				or		al, 80h
				mov		byte ptr ds:[0584h], al
				db		9ah					; call far
				dw		0
				dw		1fc0h
boot_exit:		retf

sasibiosstr		db		"scsibios"

CODE	ENDS
	END START


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
				jmp		short sxsi_init		; 0c
				nop
				retf						; 0f
				nop
				nop
				retf						; 12
				nop
				nop
				jmp		short sxsi_boot		; 15
				nop
				jmp		short sxsi_bios		; 18
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

sxsi_init:		push	ds
				push	bx
				xor		bx, bx
				mov		ds, bx
				mov		bx, cs
				mov		ds:[04b0h], bh
				mov		ds:[04b2h], bh
				mov		ds:[04b8h], bh
				mov		ds:[04bah], bh
				mov		bl, bh
				mov		bh, 0
				and		bl, 0fh
				mov		byte ptr ds:[bx+04d0h],45h
				pop		bx
				push	ax
				mov		ax, 0300h
				int		1bh
				mov		ax, 0320h
				int		1bh
				pop		ax
				pop		ds
				retf

sxsi_bios:		cld
				mov		dx, cs
				mov		ds, dx
				mov		cx, 8
				mov		si, offset sxsibiosstr
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

sxsi_boot:		cmp		al, 0ah
				je		short sasi_boot
				cmp		al, 0bh
				je		short sasi_boot
				cmp		al, 0ch
				je		short scsi_boot
				retf

sasi_boot:		push	ds
				pusha
				xor		bx, bx
				mov		ds, bx
				sub		al, 9
				mov		dl, 0ffh
				test	ds:[055dh], al			; sasi
				je		short boot_exit
				dec		al
				mov		ah, 06h
				jmp		short boot_main

scsi_boot:		push	ds
				pusha
				xor		bx, bx
				mov		ds, bx
				mov		ax, 4620h

boot_main:		mov		cx, 1fc0h
				mov		es, cx
				mov		bp, bx
				mov		cx, bx
				mov		dx, bx
				mov		bh, 4
				int		1bh
				jc		boot_exit
				or		al, 80h
				mov		byte ptr ds:[0584h], al
				db		9ah					; call far
				dw		0
				dw		1fc0h
boot_exit:		popa
				pop		ds
				retf

sxsibiosstr		db		"sxsibios", 0

CODE	ENDS
	END START


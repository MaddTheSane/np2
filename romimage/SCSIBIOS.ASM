
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
				jmp		short scsi_init1	; 0c
				nop
				jmp		short scsi_init2	; 0f
				nop
				retf						; 12
				nop
				nop
				jmp		short scsi_boot		; 15
				nop
				jmp		short scsi_bios		; 18
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

scsi_init1:		mov		word ptr [bx], 082c2h
				retf

scsi_init2:		mov		ax, cs
				mov		ds:[04b2h], ah
				mov		ds:[04bah], ah
				mov		ds:[04bch], ah
				mov		ax, 0320h
				int		1bh
				retf

scsi_bios:		cld
				mov		dx, cs
				mov		ds, dx
				mov		cx, 8
				mov		si, offset scsibiosstr
				test	byte ptr [bp], 40h
				je		short scsisend
				lea		si, [si + 8]
scsisend:		mov		dx, 07efh
				cli
	if 1
@@:				lodsb
				out		dx, al
				loop	short @b
	else
				rep outsb
	endif
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

scsi_boot:		cmp		al, 0ch
				jne		short boot_exit
				mov		ax, 4620h
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

scsibiosstr		db		"scsibios"
				db		"scsi_dev"

CODE	ENDS
	END START


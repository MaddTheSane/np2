.186

CODE	SEGMENT
		ASSUME CS:CODE,DS:nothing,ES:nothing,SS:nothing
		ORG 0000H

FIXCS		equ		2eh

START:			xor		ax, ax
				mov		ds, ax
				mov		al, 30h
				mov		ss, ax
				mov		sp, 00feh

				sti
				mov		word ptr ds:[04ach], 0ch
				call	diskbios_set
diskbiosinit1:	mov		es, ds:[04aeh]
				cmp		word ptr es:[0009h], 0aa55h
				jne		short nextparag1
				mov		al, [bx]
				or		al, al
				jne		short nextparag1
				call	calldiskbios
nextparag1:		inc		bx
				inc		byte ptr ds:[04afh]
				loop	short diskbiosinit1

				mov		word ptr ds:[04ach], 0fh
				call	diskbiosinit2
				mov		word ptr ds:[04ach], 12h
				call	diskbiosinit2

				db		9ah					; call far
				dw		027e8h
				dw		0fd80h

				in		al, 31h
				test	al, 1
				jne		short callbasic
				int		1fh
callbasic:		int		1eh

diskbios_set:	mov		bx, 04d0h
				mov		cx, 0010h
				mov		word ptr ds:[04aeh], 0d000h
				ret

diskbiosinit2:	call	diskbios_set
dbi2_lp:		test	byte ptr [bx], 40h
				je		short dbi2_next
				call	calldiskbios
dbi2_next:		inc		bx
				inc		byte ptr ds:[04afh]
				loop	short dbi2_lp
				ret

calldiskbios:	push	ax
				push	bx
				push	cx
				call	dword ptr ds:[04ach]
				pop		cx
				pop		bx
				pop		ax
				ret

CODE	ENDS
	END START


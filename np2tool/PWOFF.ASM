
CODE	SEGMENT
		ASSUME CS:CODE,DS:nothing,ES:nothing,SS:nothing
		ORG 0100H

include	np2tool.inc

START:			cld
				push	cs
				pop		ds
				cli
				call	np2_check
				jne		short err_nonnp2
				call	sendnp2port
				mov		ax, 4c00h
				int		21h

err_nonnp2:		mov		ah, 9
				mov		dx, offset illegal
				int		21h
				mov		ah, 9
				mov		dx, bx
				int		21h
				mov		ah, 9
				mov		dx, offset crlf
				int		21h
				mov		ax, 4c01h
				int		21h


include	np2tool.x86
			db	8, 'poweroff'


illegal		db	'Illegal hardware - $'
crlf		db	0dh, 0ah, '$'

CODE	ENDS
	END START



	ORG		100h
	USE16
	CPU		8086

%include	'np2tool.inc'

START:			jmp		main

str_biosrom		db	"BIOS.ROM", 0
str_bios21rom	db	"BIOS9821.ROM", 0
str_soundrom	db	"SOUND.ROM", 0

str_errnot98	db	"Illegal hardware (work only PC-98x1)", 0

str_sepa		db	" : ", 0
str_openerr		db	"file open error", 0
str_writeerr	db	"file write error", 0
str_done		db	"done!", 0
str_crlf		db	13, 10, 0
sndbios_tbl		db	1, 0, 0, 0, 0d2h, 0


dosdisp:		push	si
				mov		si, dx
.loop:			lodsb
				cmp		al, 0
				jne		short .loop
				dec		si
				mov		byte [si], '$'
				mov		ah, 9
				int		21h
				mov		byte [si], 0
				pop		si
				ret


; ---- file

filecreate:		push	dx
				call	dosdisp
				mov		dx, str_sepa
				call	dosdisp
				pop		dx
				mov		ax, 3c00h
				xor		cx, cx
				int		21h
				jnc		short .ed
				mov		dx, str_openerr
				call	dosdisp
				stc
.ed:			ret

filewrite:		push	ds
				push	cx
				push	bx
				mov		ds, dx
				xor		dx, dx
				mov		ah, 40h
				int		21h
				pop		bx
				pop		cx
				pop		ds
				jc		short .err
				cmp		ax, cx
				je		short .ed
.err:			mov		dx, str_writeerr
				call	dosdisp
				stc
.ed:			ret

fileclose:		mov		ah, 3eh
				int		21h
				ret


main:			push	cs
				pop		ds
				cld
				mov		ah, 0fh
				int		10h
				cmp		ah, 0fh
				je		short makebiosrom
				mov		dx, str_errnot98
				call	dosdisp
				mov		ax, 4c00h
				int		21h

makebiosrom:	mov		dx, str_biosrom
				call	filecreate
				jc		short .ed
				mov		bx, ax
				mov		cx, 08000h
				mov		dx, 0e800h
				call	filewrite
				jc		short .fclose
				mov		dx, 0f000h
				call	filewrite
				jc		short .fclose
				mov		dx, 0f800h
				call	filewrite
				jc		short .fclose
				mov		dx, str_done
				call	dosdisp
.fclose:		call	fileclose

.ed:			mov		dx, str_crlf
				call	dosdisp


makepc9821rom:	xor		ax, ax
				mov		es, ax
				test	byte [es:0x045c], 0x40
				je		short .skip

				mov		dx, str_bios21rom
				call	filecreate
				jc		short .ed
				mov		bx, ax
				mov		cx, 02000h
				mov		dx, 0d800h
				call	filewrite
				jc		short .fclose
				mov		dx, str_done
				call	dosdisp
.fclose:		call	fileclose

.ed:			mov		dx, str_crlf
				call	dosdisp
.skip:


makesoundrom:	mov		ax, 0c800h
				mov		es, ax

.sealp:			mov		si, sndbios_tbl
				mov		di, 2e00h
				mov		cx, 3
				repz cmpsw
				je		short .dump
				add		ah, 4
				cmp		ah, 0d8h
				jc		short .sealp
				jmp		short .skip

.dump:			push	ax
				mov		dx, str_soundrom
				call	filecreate
				jc		short .ed
				mov		bx, ax
				pop		dx
				mov		cx, 04000h
				call	filewrite
				jc		short .fclose
				mov		dx, str_done
				call	dosdisp
.fclose:		call	fileclose

.ed:			mov		dx, str_crlf
				call	dosdisp
.skip:


returndos:		mov		ax, 4c00h
				int		21h

	ends


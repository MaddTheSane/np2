.186

CODE	SEGMENT
		ASSUME CS:CODE,DS:nothing,ES:nothing,SS:nothing
		ORG 0000H

FIXCS		equ		2eh

START:			mov		cx, 1000h
				mov		bx, 8000h
				mov		ss, cx
				mov		sp, bx
				add		bh, 20h
				mov		es, bx

				xor		di, di
				cld
				lea		ax, [di + 20h]
				rep stosw
				mov		al, 0e1h
				mov		cx, 1fe0h
atrclr:			stosb
				inc		di
				loop	atrclr

				mov		si, offset nosysmsg
				xor		di, di
				call	putmsg
				mov		di, 160
				call	putmsg
mine:			cli
				hlt
				jmp		mine

putmsg:			db		FIXCS
				lodsw
				test	ax, ax
				je		short putmsgend
				stosw
				inc		di
				inc		di
				jmp		short putmsg
putmsgend:		ret


				; このハードディスクイメージはフォーマットされていません．
nosysmsg	dw	3304h,4e04h,4f05h,3c01h,4905h,4705h,2305h,3905h
			dw	2f05h,2405h,6105h,3c01h,3805h,4f04h,5505h,2905h
			dw	3c01h,5e05h,4305h,4805h,3504h,6c04h,4604h,2404h
			dw	5e04h,3b04h,7304h,2501h, 0

				; ディスクイメージを挿入後，リセットして下さい．
			dw	4705h,2305h,3905h,2f05h,2405h,6105h,3c01h,3805h
			dw	7204h,5e21h,7e26h,6518h,2401h,6a05h,3b05h,4305h
			dw	4805h,3704h,4604h,3c12h,3504h,2404h,2501h,0

CODE	ENDS
	END START


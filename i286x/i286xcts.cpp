#include	"compiler.h"
#include	"cpucore.h"
#include	"i286x.h"
#include	"i286xadr.h"
#include	"i286xcts.h"
#include	"i286x.mcr"
#include	"i286xea.mcr"


I286 _sgdt(void) {

		__asm {
				cmp		al, 0c0h
				jnc		register_eareg16
				I286CLOCK(11)
				call	p_get_ea[eax*4]
				lea		ecx, [edi + ebp]
				mov		dx, word ptr i286core.s.GDTR
				call	i286_memorywrite_w
				add		bp, 2
				lea		ecx, [edi + ebp]
				mov		dx, word ptr (i286core.s.GDTR+2)
				call	i286_memorywrite_w
				add		bp, 2
				lea		ecx, [edi + ebp]
				mov		dl, byte ptr (i286core.s.GDTR+4)			// ver0.29
				mov		dh, -1
				jmp		i286_memorywrite_w
				align	4
		register_eareg16:
				INT_NUM(6)
		}
}


I286 _sidt(void) {

		__asm {
				cmp		al, 0c0h
				jnc		register_eareg16
				I286CLOCK(12)
				call	p_get_ea[eax*4]
				lea		ecx, [edi + ebp]
				mov		dx, word ptr i286core.s.IDTR
				call	i286_memorywrite_w
				add		bp, 2
				lea		ecx, [edi + ebp]
				mov		dx, word ptr (i286core.s.IDTR+2)
				call	i286_memorywrite_w
				add		bp, 2
				lea		ecx, [edi + ebp]
				mov		dl, byte ptr (i286core.s.IDTR+4)			// ver0.29
				mov		dh, -1
				jmp		i286_memorywrite_w
				align	4
		register_eareg16:
				INT_NUM(6)
		}
}


I286 _lgdt(void) {

		__asm {
				cmp		al, 0c0h
				jnc		register_eareg16
				I286CLOCK(11)
				call	p_get_ea[eax*4]
				lea		ecx, [edi + ebp]
				call	i286_memoryread_w
				mov		word ptr i286core.s.GDTR, ax
				add		bp, 2
				lea		ecx, [edi + ebp]
				call	i286_memoryread_w
				mov		word ptr (i286core.s.GDTR+2), ax
				add		bp, 2
				lea		ecx, [edi + ebp]
				call	i286_memoryread_w
				mov		word ptr (i286core.s.GDTR+4), ax
				ret
				align	4
		register_eareg16:
				INT_NUM(6)
		}
}


I286 _lidt(void) {

		__asm {
				cmp		al, 0c0h
				jnc		register_eareg16
				I286CLOCK(12)
				call	p_get_ea[eax*4]
				lea		ecx, [edi + ebp]
				call	i286_memoryread_w
				mov		word ptr i286core.s.IDTR, ax
				add		bp, 2
				lea		ecx, [edi + ebp]
				call	i286_memoryread_w
				mov		word ptr (i286core.s.IDTR+2), ax
				add		bp, 2
				lea		ecx, [edi + ebp]
				call	i286_memoryread_w
				mov		word ptr (i286core.s.IDTR+4), ax
				ret
				align	4
		register_eareg16:
				INT_NUM(6)
		}
}


I286 _smsw(void) {

		__asm {
				PREPART_EA16(3)
					mov		dx, i286core.s.MSW
					mov		word ptr I286_REG[eax*2], dx
					GET_NEXTPRE2
					ret
				MEMORY_EA16(6)
					mov		ax, i286core.s.MSW
					mov		word ptr I286_MEM[ecx], ax
					ret
				extmem_eareg16:
					mov		dx, i286core.s.MSW
					jmp		i286_memorywrite_w
		}
}


I286 _lmsw(void) {

		__asm {
				PREPART_EA16(2)
					mov		ax, word ptr I286_REG[eax*2]
					and		ax, 0ch
					mov		i286core.s.MSW, ax
					GET_NEXTPRE2
					ret
				MEMORY_EA16(3)
					mov		ax, word ptr I286_MEM[ecx]
					and		ax, 0ch
					mov		i286core.s.MSW, ax
					ret
				EXTMEM_EA16
					and		ax, 0ch
					mov		i286core.s.MSW, ax
					ret
		}
}


static void (*cts1_xtable[])(void) = {
			_sgdt,				_sidt,
			_lgdt,				_lidt,
			_smsw,				_smsw,
			_lmsw,				_lmsw};


I286EXT _xcts(void) {

		__asm {
				mov		edi, esi
				GET_NEXTPRE1
				test	bl, bl
				je		realmodeint6rm
				dec		bl
				je		i286_cts1
				sub		bl, 2
				jnc		realmodeint6

realmodeint6rm:	movzx	eax, bh
				cmp		al, 0c0h
				jnc		realmodeint6
				call	p_get_ea[eax*4]

				align	4
realmodeint6:	cmp		bl, (5 - 3)
				je		loadall286
				mov		si, di					// ver0.27 このタイプ・・・
				I286CLOCK(20)					// 全部修正しなきゃ(汗
				INT_NUM(6)						// i286とi386で挙動が違うから
												// いやらしいね…
				align	4
i286_cts1:		movzx	eax, bh
				mov		edi, eax
				shr		edi, 3-2
				and		edi, 7*4
				jmp		cts1_xtable[edi]

				align	4
loadall286:		I286CLOCK(195)
				mov		ax, word ptr mem[0x0804]		// MSW
				mov		i286core.s.MSW, ax
				mov		ax, word ptr mem[0x0818]		// flag
				mov		I286_FLAG, ax
				and		ah, 3
				cmp		ah, 3
				sete	I286_TRAP
				mov		si, word ptr mem[0x081a]		// ip
				mov		ax, word ptr mem[0x081e]		// ds
				mov		I286_DS, ax
				mov		ax, word ptr mem[0x0820]		// ss
				mov		I286_SS, ax
				mov		ax, word ptr mem[0x0822]		// cs
				mov		I286_CS, ax
				mov		ax, word ptr mem[0x0824]		// es
				mov		I286_ES, ax
				mov		ax, word ptr mem[0x0826]		// di
				mov		I286_DI, ax
				mov		ax, word ptr mem[0x0828]		// si
				mov		I286_SI, ax
				mov		ax, word ptr mem[0x082a]		// bp
				mov		I286_BP, ax
				mov		ax, word ptr mem[0x082c]		// sp
				mov		I286_SP, ax
				mov		ax, word ptr mem[0x082e]		// bx
				mov		I286_BX, ax
				mov		ax, word ptr mem[0x0830]		// dx
				mov		I286_DX, ax
				mov		ax, word ptr mem[0x0832]		// cx
				mov		I286_CX, ax
				mov		ax, word ptr mem[0x0834]		// ax
				mov		I286_AX, ax
				mov		eax, dword ptr mem[0x0836]		// es_desc
				and		eax, 00ffffffh
				mov		ES_BASE, eax
				mov		eax, dword ptr mem[0x083c]		// cs_desc
				and		eax, 00ffffffh
				mov		CS_BASE, eax
				mov		eax, dword ptr mem[0x0842]		// ss_desc
				and		eax, 00ffffffh
				mov		SS_BASE, eax
				mov		SS_FIX, eax
				mov		eax, dword ptr mem[0x0848]		// ds_desc
				and		eax, 00ffffffh
				mov		DS_BASE, eax
				mov		DS_FIX, eax

				RESET_XPREFETCH
				I286IRQCHECKTERM
		}
}


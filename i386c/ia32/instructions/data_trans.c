/*	$Id: data_trans.c,v 1.7 2004/02/19 03:04:02 yui Exp $	*/

/*
 * Copyright (c) 2003 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "compiler.h"
#include "cpu.h"
#include "ia32.mcr"

#include "data_trans.h"

/*
 * MOV
 */
void
MOV_EbGb(void)
{
	DWORD op, src, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = (BYTE)src;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)src);
	}
}

void
MOV_EwGw(void)
{
	DWORD op, src, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg16_b20[op]) = (WORD)src;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)src);
	}
}

void
MOV_EdGd(void)
{
	DWORD op, src, madr;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg32_b20[op]) = src;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, src);
	}
}

void
MOV_GbEb(void)
{
	BYTE *out;
	DWORD op, src;

	PREPART_REG8_EA(op, src, out, 2, 5);
	*out = (BYTE)src;
}

void
MOV_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	*out = (WORD)src;
}

void
MOV_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	*out = src;
}

void
MOV_EwSw(void)
{
	DWORD op, src, madr;
	BYTE idx;

	GET_PCBYTE(op);
	idx = (BYTE)((op >> 3) & 7);
	if (idx < CPU_SEGREG_NUM) {
		src = CPU_REGS_SREG(idx);
		if (op >= 0xc0) {
			CPU_WORKCLOCK(2);
			*(reg16_b20[op]) = (WORD)src;
		} else {
			CPU_WORKCLOCK(3);
			madr = calc_ea_dst(op);
			cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)src);
		}
		return;
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

void
MOV_SwEw(void)
{
	DWORD op, src, madr;
	BYTE idx;

	GET_PCBYTE(op);
	idx = ((BYTE)(op >> 3) & 7);
	if (idx != CPU_CS_INDEX && idx < CPU_SEGREG_NUM) {
		if (op >= 0xc0) {
			CPU_WORKCLOCK(2);
			src = *(reg16_b20[op]);
		} else {
			CPU_WORKCLOCK(5);
			madr = calc_ea_dst(op);
			src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		}
		CPU_SET_SEGREG(idx, (WORD)src);
		if (idx == CPU_SS_INDEX) {
			exec_1step();
		}
		return;
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

void
MOV_ALOb(void)
{
	DWORD madr;

	CPU_WORKCLOCK(5);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	if (!CPU_INST_AS32) {
		GET_PCWORD(madr);
		CPU_AL = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	} else {
		GET_PCDWORD(madr);
		CPU_AL = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	}
}

void
MOV_AXOw(void)
{
	DWORD madr;

	CPU_WORKCLOCK(5);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	if (!CPU_INST_AS32) {
		GET_PCWORD(madr);
		CPU_AX = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	} else {
		GET_PCDWORD(madr);
		CPU_AX = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	}
}

void
MOV_EAXOd(void)
{
	DWORD madr;

	CPU_WORKCLOCK(5);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	if (!CPU_INST_AS32) {
		GET_PCWORD(madr);
		CPU_EAX = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	} else {
		GET_PCDWORD(madr);
		CPU_EAX = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	}
}

void
MOV_ObAL(void)
{
	DWORD madr;

	CPU_WORKCLOCK(3);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	if (!CPU_INST_AS32) {
		GET_PCWORD(madr);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, CPU_AL);
	} else {
		GET_PCDWORD(madr);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, CPU_AL);
	}
}

void
MOV_OwAX(void)
{
	DWORD madr;

	CPU_WORKCLOCK(3);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	if (!CPU_INST_AS32) {
		GET_PCWORD(madr);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, CPU_AX);
	} else {
		GET_PCDWORD(madr);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, CPU_AX);
	}
}

void
MOV_OdEAX(void)
{
	DWORD madr;

	CPU_WORKCLOCK(3);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	if (!CPU_INST_AS32) {
		GET_PCWORD(madr);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, CPU_EAX);
	} else {
		GET_PCDWORD(madr);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, CPU_EAX);
	}
}

void
MOV_EbIb(void)
{
	DWORD op, src, res, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		GET_PCBYTE(res);
		*(reg8_b20[op]) = (BYTE)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		GET_PCBYTE(res);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)res);
	}
}

void
MOV_EwIw(void)
{
	DWORD op, src, res, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		GET_PCWORD(res);
		*(reg16_b20[op]) = (WORD)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		GET_PCWORD(res);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)res);
	}
}

void
MOV_EdId(void)
{
	DWORD op, src, res, madr;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		GET_PCDWORD(res);
		*(reg32_b20[op]) = res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		GET_PCDWORD(res);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, res);
	}
}

void MOV_ALIb(void) { CPU_WORKCLOCK(2); GET_PCBYTE(CPU_AL); }
void MOV_CLIb(void) { CPU_WORKCLOCK(2); GET_PCBYTE(CPU_CL); }
void MOV_DLIb(void) { CPU_WORKCLOCK(2); GET_PCBYTE(CPU_DL); }
void MOV_BLIb(void) { CPU_WORKCLOCK(2); GET_PCBYTE(CPU_BL); }
void MOV_AHIb(void) { CPU_WORKCLOCK(2); GET_PCBYTE(CPU_AH); }
void MOV_CHIb(void) { CPU_WORKCLOCK(2); GET_PCBYTE(CPU_CH); }
void MOV_DHIb(void) { CPU_WORKCLOCK(2); GET_PCBYTE(CPU_DH); }
void MOV_BHIb(void) { CPU_WORKCLOCK(2); GET_PCBYTE(CPU_BH); }

void MOV_AXIw(void) { CPU_WORKCLOCK(2); GET_PCWORD(CPU_AX); }
void MOV_CXIw(void) { CPU_WORKCLOCK(2); GET_PCWORD(CPU_CX); }
void MOV_DXIw(void) { CPU_WORKCLOCK(2); GET_PCWORD(CPU_DX); }
void MOV_BXIw(void) { CPU_WORKCLOCK(2); GET_PCWORD(CPU_BX); }
void MOV_SPIw(void) { CPU_WORKCLOCK(2); GET_PCWORD(CPU_SP); }
void MOV_BPIw(void) { CPU_WORKCLOCK(2); GET_PCWORD(CPU_BP); }
void MOV_SIIw(void) { CPU_WORKCLOCK(2); GET_PCWORD(CPU_SI); }
void MOV_DIIw(void) { CPU_WORKCLOCK(2); GET_PCWORD(CPU_DI); }

void MOV_EAXId(void) { CPU_WORKCLOCK(2); GET_PCDWORD(CPU_EAX); }
void MOV_ECXId(void) { CPU_WORKCLOCK(2); GET_PCDWORD(CPU_ECX); }
void MOV_EDXId(void) { CPU_WORKCLOCK(2); GET_PCDWORD(CPU_EDX); }
void MOV_EBXId(void) { CPU_WORKCLOCK(2); GET_PCDWORD(CPU_EBX); }
void MOV_ESPId(void) { CPU_WORKCLOCK(2); GET_PCDWORD(CPU_ESP); }
void MOV_EBPId(void) { CPU_WORKCLOCK(2); GET_PCDWORD(CPU_EBP); }
void MOV_ESIId(void) { CPU_WORKCLOCK(2); GET_PCDWORD(CPU_ESI); }
void MOV_EDIId(void) { CPU_WORKCLOCK(2); GET_PCDWORD(CPU_EDI); }

/*
 * CMOVcc
 */
void
CMOVO_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if (CPU_OV) {
		*out = (WORD)src;
	}
}

void
CMOVO_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if (CPU_OV) {
		*out = src;
	}
}

void
CMOVNO_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if (!CPU_OV) {
		*out = (WORD)src;
	}
}

void
CMOVNO_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if (!CPU_OV) {
		*out = src;
	}
}

void
CMOVC_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if (CPU_FLAGL & C_FLAG) {
		*out = (WORD)src;
	}
}

void
CMOVC_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if (CPU_FLAGL & C_FLAG) {
		*out = src;
	}
}

void
CMOVNC_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if (!(CPU_FLAGL & C_FLAG)) {
		*out = (WORD)src;
	}
}

void
CMOVNC_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if (!(CPU_FLAGL & C_FLAG)) {
		*out = src;
	}
}

void
CMOVZ_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if (CPU_FLAGL & Z_FLAG) {
		*out = (WORD)src;
	}
}

void
CMOVZ_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if (CPU_FLAGL & Z_FLAG) {
		*out = src;
	}
}

void
CMOVNZ_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if (!(CPU_FLAGL & Z_FLAG)) {
		*out = (WORD)src;
	}
}

void
CMOVNZ_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if (!(CPU_FLAGL & Z_FLAG)) {
		*out = src;
	}
}

void
CMOVA_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if (!(CPU_FLAGL & (Z_FLAG | C_FLAG))) {
		*out = (WORD)src;
	}
}

void
CMOVA_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if (!(CPU_FLAGL & (Z_FLAG | C_FLAG))) {
		*out = src;
	}
}

void
CMOVNA_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if (CPU_FLAGL & (Z_FLAG | C_FLAG)) {
		*out = (WORD)src;
	}
}

void
CMOVNA_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if (CPU_FLAGL & (Z_FLAG | C_FLAG)) {
		*out = src;
	}
}

void
CMOVS_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if (CPU_FLAGL & S_FLAG) {
		*out = (WORD)src;
	}
}

void
CMOVS_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if (CPU_FLAGL & S_FLAG) {
		*out = src;
	}
}

void
CMOVNS_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if (!(CPU_FLAGL & S_FLAG)) {
		*out = (WORD)src;
	}
}

void
CMOVNS_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if (!(CPU_FLAGL & S_FLAG)) {
		*out = src;
	}
}

void
CMOVP_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if (CPU_FLAGL & P_FLAG) {
		*out = (WORD)src;
	}
}

void
CMOVP_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if (CPU_FLAGL & P_FLAG) {
		*out = src;
	}
}

void
CMOVNP_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if (!(CPU_FLAGL & P_FLAG)) {
		*out = (WORD)src;
	}
}

void
CMOVNP_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if (!(CPU_FLAGL & P_FLAG)) {
		*out = src;
	}
}

void
CMOVL_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if ((!CPU_OV) != (!(CPU_FLAGL & S_FLAG))) {
		*out = (WORD)src;
	}
}

void
CMOVL_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if ((!CPU_OV) != (!(CPU_FLAGL & S_FLAG))) {
		*out = src;
	}
}

void
CMOVNL_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if ((!CPU_OV) == (!(CPU_FLAGL & S_FLAG))) {
		*out = (WORD)src;
	}
}

void
CMOVNL_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if ((!CPU_OV) == (!(CPU_FLAGL & S_FLAG))) {
		*out = src;
	}
}

void
CMOVLE_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if ((CPU_FLAGL & Z_FLAG) || ((!(CPU_FLAGL & S_FLAG)) != (!CPU_OV))) {
		*out = (WORD)src;
	}
}

void
CMOVLE_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if ((CPU_FLAGL & Z_FLAG) || ((!(CPU_FLAGL & S_FLAG)) != (!CPU_OV))) {
		*out = src;
	}
}

void
CMOVNLE_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	if ((!(CPU_FLAGL & Z_FLAG)) && ((!(CPU_FLAGL & S_FLAG)) == (!CPU_OV))) {
		*out = (WORD)src;
	}
}

void
CMOVNLE_GdEd(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA(op, src, out, 2, 5);
	if ((!(CPU_FLAGL & Z_FLAG)) && ((!(CPU_FLAGL & S_FLAG)) == (!CPU_OV))) {
		*out = src;
	}
}

/*
 * XCHG
 */
void
XCHG_EbGb(void)
{
	BYTE *out, *src;
	DWORD op, madr;
	BYTE tmp;

	PREPART_EA_REG8P(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(3);
		out = reg8_b20[op];
		SWAPBYTE(*out, *src);
	} else {
		CPU_WORKCLOCK(5);
		madr = calc_ea_dst(op);
		tmp = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, *src);
		*src = tmp;
	}
}

void
XCHG_EwGw(void)
{
	WORD *out, *src;
	DWORD op, madr;
	WORD tmp;

	PREPART_EA_REG16P(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(3);
		out = reg16_b20[op];
		SWAPWORD(*out, *src);
	} else {
		CPU_WORKCLOCK(5);
		madr = calc_ea_dst(op);
		tmp = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, *src);
		*src = tmp;
	}
}

void
XCHG_EdGd(void)
{
	DWORD *out, *src;
	DWORD op, madr;
	DWORD tmp;

	PREPART_EA_REG32P(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(3);
		out = reg32_b20[op];
		SWAPDWORD(*out, *src);
	} else {
		CPU_WORKCLOCK(5);
		madr = calc_ea_dst(op);
		tmp = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, *src);
		*src = tmp;
	}
}

/* void XCHG_AXAX(void) { } */
void XCHG_CXAX(void) { CPU_WORKCLOCK(3); SWAPWORD(CPU_CX, CPU_AX); }
void XCHG_DXAX(void) { CPU_WORKCLOCK(3); SWAPWORD(CPU_DX, CPU_AX); }
void XCHG_BXAX(void) { CPU_WORKCLOCK(3); SWAPWORD(CPU_BX, CPU_AX); }
void XCHG_SPAX(void) { CPU_WORKCLOCK(3); SWAPWORD(CPU_SP, CPU_AX); }
void XCHG_BPAX(void) { CPU_WORKCLOCK(3); SWAPWORD(CPU_BP, CPU_AX); }
void XCHG_SIAX(void) { CPU_WORKCLOCK(3); SWAPWORD(CPU_SI, CPU_AX); }
void XCHG_DIAX(void) { CPU_WORKCLOCK(3); SWAPWORD(CPU_DI, CPU_AX); }

/* void XCHG_EAXEAX(void) { } */
void XCHG_ECXEAX(void) { CPU_WORKCLOCK(3); SWAPDWORD(CPU_ECX, CPU_EAX); }
void XCHG_EDXEAX(void) { CPU_WORKCLOCK(3); SWAPDWORD(CPU_EDX, CPU_EAX); }
void XCHG_EBXEAX(void) { CPU_WORKCLOCK(3); SWAPDWORD(CPU_EBX, CPU_EAX); }
void XCHG_ESPEAX(void) { CPU_WORKCLOCK(3); SWAPDWORD(CPU_ESP, CPU_EAX); }
void XCHG_EBPEAX(void) { CPU_WORKCLOCK(3); SWAPDWORD(CPU_EBP, CPU_EAX); }
void XCHG_ESIEAX(void) { CPU_WORKCLOCK(3); SWAPDWORD(CPU_ESI, CPU_EAX); }
void XCHG_EDIEAX(void) { CPU_WORKCLOCK(3); SWAPDWORD(CPU_EDI, CPU_EAX); }

/*
 * BSWAP
 */
#if defined(bswap32) && !defined(IA32_USE_ASM_BSWAP)
#define	BSWAP_DWORD(v)	bswap32(v)
#else	/* !bswap32 || IA32_USE_ASM_BSWAP */
INLINE static DWORD
BSWAP_DWORD(DWORD val)
{
#if defined(__GNUC__) && (defined(i386) || defined(__i386__))
	__asm__ __volatile__ (
#if defined(IA32_USE_ASM_BSWAP)
		"bswap %0"
#else	/* !IA32_USE_ASM_BSWAP */
		"rorw $8, %w1\n\t"
		"rorl $16, %1\n\t"
		"rorw $8, %w1\n\t"
#endif	/* IA32_USE_ASM_BSWAP */
		: "=r" (val) : "0" (val));
	return val;
#else	/* !(__GNUC__ && (i386 || __i386__)) */
	DWORD v;
	v  = (val & 0x000000ff) << 24;
	v |= (val & 0x0000ff00) << 8;
	v |= (val & 0x00ff0000) >> 8;
	v |= (val & 0xff000000) >> 24;
	return v;
#endif	/* __GNUC__ && (i386 || __i386__) */
}
#endif	/* bswap32 && !IA32_USE_ASM_BSWAP */

void BSWAP_EAX(void) { CPU_WORKCLOCK(2); CPU_EAX = BSWAP_DWORD(CPU_EAX); }
void BSWAP_ECX(void) { CPU_WORKCLOCK(2); CPU_ECX = BSWAP_DWORD(CPU_ECX); }
void BSWAP_EDX(void) { CPU_WORKCLOCK(2); CPU_EDX = BSWAP_DWORD(CPU_EDX); }
void BSWAP_EBX(void) { CPU_WORKCLOCK(2); CPU_EBX = BSWAP_DWORD(CPU_EBX); }
void BSWAP_ESP(void) { CPU_WORKCLOCK(2); CPU_ESP = BSWAP_DWORD(CPU_ESP); }
void BSWAP_EBP(void) { CPU_WORKCLOCK(2); CPU_EBP = BSWAP_DWORD(CPU_EBP); }
void BSWAP_ESI(void) { CPU_WORKCLOCK(2); CPU_ESI = BSWAP_DWORD(CPU_ESI); }
void BSWAP_EDI(void) { CPU_WORKCLOCK(2); CPU_EDI = BSWAP_DWORD(CPU_EDI); }

/*
 * XADD
 */
void
XADD_EbGb(void)
{
	BYTE *out, *src;
	DWORD op, dst, res, madr;

	PREPART_EA_REG8P(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
		ADDBYTE(res, dst, *src);
		*src = (BYTE)dst;
		*out = (BYTE)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		ADDBYTE(res, dst, *src);
		*src = (BYTE)dst;
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)res);
	}
}

void
XADD_EwGw(void)
{
	WORD *out, *src;
	DWORD op, dst, res, madr;

	PREPART_EA_REG16P(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
		ADDWORD(res, dst, *src);
		*src = (WORD)dst;
		*out = (WORD)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		ADDWORD(res, dst, *src);
		*src = (WORD)dst;
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)res);
	}
}

void
XADD_EdGd(void)
{
	DWORD *out, *src;
	DWORD op, dst, res, madr;

	PREPART_EA_REG32P(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		dst = *out;
		ADDDWORD(res, dst, *src);
		*src = dst;
		*out = res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		ADDDWORD(res, dst, *src);
		*src = dst;
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, res);
	}
}

/*
 * CMPXCHG
 */
void
CMPXCHG_EbGb(void)
{
	BYTE *out;
	DWORD op, src, dst, madr, tmp;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		out = reg8_b20[op];
		dst = *out;
		BYTE_SUB(tmp, CPU_AL, dst);
		if (CPU_FLAGL & Z_FLAG) {
			*out = (BYTE)src;
		} else {
			CPU_AL = (BYTE)dst;
		}
	} else {
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		BYTE_SUB(tmp, CPU_AL, dst);
		if (CPU_FLAGL & Z_FLAG) {
			cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)src);
		} else {
			CPU_AL = (BYTE)dst;
		}
	}
}

void
CMPXCHG_EwGw(void)
{
	WORD *out;
	DWORD op, src, dst, madr, tmp;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		out = reg16_b20[op];
		dst = *out;
		WORD_SUB(tmp, CPU_AX, dst);
		if (CPU_FLAGL & Z_FLAG) {
			*out = (WORD)src;
		} else {
			CPU_AX = (WORD)dst;
		}
	} else {
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_SUB(tmp, CPU_AX, dst);
		if (CPU_FLAGL & Z_FLAG) {
			cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)src);
		} else {
			CPU_AX = (WORD)dst;
		}
	}
}

void
CMPXCHG_EdGd(void)
{
	DWORD *out;
	DWORD op, src, dst, madr, tmp;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		out = reg32_b20[op];
		dst = *out;
		WORD_SUB(tmp, CPU_EAX, dst);
		if (CPU_FLAGL & Z_FLAG) {
			*out = src;
		} else {
			CPU_EAX = dst;
		}
	} else {
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		DWORD_SUB(tmp, CPU_AX, dst);
		if (CPU_FLAGL & Z_FLAG) {
			cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, src);
		} else {
			CPU_EAX = dst;
		}
	}
}

void
CMPXCHG8B(DWORD op)
{
	DWORD madr, dst_l, dst_h;

	if (op < 0xc0) {
		CPU_WORKCLOCK(2);
		madr = calc_ea_dst(op);
		dst_l = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		dst_h = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr + 4);
		if ((CPU_EDX == dst_h) && (CPU_EAX == dst_l)) {
			cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, CPU_EBX);
			cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr + 4, CPU_ECX);
			CPU_FLAGL |= Z_FLAG;
		} else {
			CPU_EDX = dst_h;
			CPU_EAX = dst_l;
			CPU_FLAGL &= ~Z_FLAG;
		}
		return;
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

/*
 * PUSH
 */
void PUSH_AX(void) { CPU_WORKCLOCK(3); PUSH0_16(CPU_AX); }
void PUSH_CX(void) { CPU_WORKCLOCK(3); PUSH0_16(CPU_CX); }
void PUSH_DX(void) { CPU_WORKCLOCK(3); PUSH0_16(CPU_DX); }
void PUSH_BX(void) { CPU_WORKCLOCK(3); PUSH0_16(CPU_BX); }
void PUSH_SP(void) { CPU_WORKCLOCK(3); SP_PUSH_16(CPU_SP); }
void PUSH_BP(void) { CPU_WORKCLOCK(3); PUSH0_16(CPU_BP); }
void PUSH_SI(void) { CPU_WORKCLOCK(3); PUSH0_16(CPU_SI); }
void PUSH_DI(void) { CPU_WORKCLOCK(3); PUSH0_16(CPU_DI); }

void PUSH_EAX(void) { CPU_WORKCLOCK(3); PUSH0_32(CPU_EAX); }
void PUSH_ECX(void) { CPU_WORKCLOCK(3); PUSH0_32(CPU_ECX); }
void PUSH_EDX(void) { CPU_WORKCLOCK(3); PUSH0_32(CPU_EDX); }
void PUSH_EBX(void) { CPU_WORKCLOCK(3); PUSH0_32(CPU_EBX); }
void PUSH_ESP(void) { CPU_WORKCLOCK(3); ESP_PUSH_32(CPU_ESP); }
void PUSH_EBP(void) { CPU_WORKCLOCK(3); PUSH0_32(CPU_EBP); }
void PUSH_ESI(void) { CPU_WORKCLOCK(3); PUSH0_32(CPU_ESI); }
void PUSH_EDI(void) { CPU_WORKCLOCK(3); PUSH0_32(CPU_EDI); }

void
PUSH_Ew(DWORD op)
{
	DWORD dst, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		dst = *(reg16_b20[op]);
		PUSH0_16(dst);
	} else {
		CPU_WORKCLOCK(5);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		PUSH0_16(dst);
	}
}

void
PUSH_Ed(DWORD op)
{
	DWORD dst, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		dst = *(reg32_b20[op]);
		PUSH0_32(dst);
	} else {
		CPU_WORKCLOCK(5);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		PUSH0_32(dst);
	}
}

void
PUSH_Ib(void)
{
	SDWORD val;

	CPU_WORKCLOCK(3);
	GET_PCBYTESD(val);
	XPUSH0(val);
}

void
PUSH_Iw(void)
{
	WORD val;

	CPU_WORKCLOCK(3);
	GET_PCWORD(val);
	PUSH0_16(val);
}

void
PUSH_Id(void)
{
	DWORD val;

	CPU_WORKCLOCK(3);
	GET_PCDWORD(val);
	PUSH0_32(val);
}

void PUSH16_ES(void) { CPU_WORKCLOCK(3); PUSH0_16(CPU_ES); }
void PUSH16_CS(void) { CPU_WORKCLOCK(3); PUSH0_16(CPU_CS); }
void PUSH16_SS(void) { CPU_WORKCLOCK(3); PUSH0_16(CPU_SS); }
void PUSH16_DS(void) { CPU_WORKCLOCK(3); PUSH0_16(CPU_DS); }
void PUSH16_FS(void) { CPU_WORKCLOCK(3); PUSH0_16(CPU_FS); }
void PUSH16_GS(void) { CPU_WORKCLOCK(3); PUSH0_16(CPU_GS); }

void PUSH32_ES(void) { CPU_WORKCLOCK(3); PUSH0_32(CPU_ES); }
void PUSH32_CS(void) { CPU_WORKCLOCK(3); PUSH0_32(CPU_CS); }
void PUSH32_SS(void) { CPU_WORKCLOCK(3); PUSH0_32(CPU_SS); }
void PUSH32_DS(void) { CPU_WORKCLOCK(3); PUSH0_32(CPU_DS); }
void PUSH32_FS(void) { CPU_WORKCLOCK(3); PUSH0_32(CPU_FS); }
void PUSH32_GS(void) { CPU_WORKCLOCK(3); PUSH0_32(CPU_GS); }

/*
 * POP
 */
void POP_AX(void) { CPU_WORKCLOCK(5); POP0_16(CPU_AX); }
void POP_CX(void) { CPU_WORKCLOCK(5); POP0_16(CPU_CX); }
void POP_DX(void) { CPU_WORKCLOCK(5); POP0_16(CPU_DX); }
void POP_BX(void) { CPU_WORKCLOCK(5); POP0_16(CPU_BX); }
void POP_SP(void) { CPU_WORKCLOCK(5); SP_POP_16(CPU_SP); }
void POP_BP(void) { CPU_WORKCLOCK(5); POP0_16(CPU_BP); }
void POP_SI(void) { CPU_WORKCLOCK(5); POP0_16(CPU_SI); }
void POP_DI(void) { CPU_WORKCLOCK(5); POP0_16(CPU_DI); }

void POP_EAX(void) { CPU_WORKCLOCK(5); POP0_32(CPU_EAX); }
void POP_ECX(void) { CPU_WORKCLOCK(5); POP0_32(CPU_ECX); }
void POP_EDX(void) { CPU_WORKCLOCK(5); POP0_32(CPU_EDX); }
void POP_EBX(void) { CPU_WORKCLOCK(5); POP0_32(CPU_EBX); }
void POP_ESP(void) { CPU_WORKCLOCK(5); ESP_POP_32(CPU_ESP); }
void POP_EBP(void) { CPU_WORKCLOCK(5); POP0_32(CPU_EBP); }
void POP_ESI(void) { CPU_WORKCLOCK(5); POP0_32(CPU_ESI); }
void POP_EDI(void) { CPU_WORKCLOCK(5); POP0_32(CPU_EDI); }

void
POP_Ew(void)
{
	DWORD op, madr;
	WORD src;

	CPU_WORKCLOCK(5);

	POP0_16(src);
	GET_PCBYTE(op);
	if (op >= 0xc0) {
		*(reg16_b20[op]) = src;
	} else {
		madr = calc_ea_dst(op);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, src);
	}
}

void
POP_Ew_G5(DWORD op)
{
	DWORD madr;
	WORD src;

	CPU_WORKCLOCK(5);

	POP0_16(src);
	if (op >= 0xc0) {
		*(reg16_b20[op]) = src;
	} else {
		madr = calc_ea_dst(op);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, src);
	}
}

void
POP_Ed(void)
{
	DWORD op, madr;
	DWORD src;

	CPU_WORKCLOCK(5);

	POP0_32(src);
	GET_PCBYTE(op);
	if (op >= 0xc0) {
		*(reg32_b20[op]) = src;
	} else {
		madr = calc_ea_dst(op);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, src);
	}
}

void
POP_Ed_G5(DWORD op)
{
	DWORD src, madr;

	CPU_WORKCLOCK(5);

	POP0_32(src);
	if (op >= 0xc0) {
		*(reg32_b20[op]) = src;
	} else {
		madr = calc_ea_dst(op);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, src);
	}
}

void
POP16_ES(void)
{
	WORD src;

	CPU_WORKCLOCK(5);

	POP0_16(src);
	CPU_SET_SEGREG(CPU_ES_INDEX, src);
}

void
POP32_ES(void)
{
	DWORD src;

	CPU_WORKCLOCK(5);

	POP0_32(src);
	CPU_SET_SEGREG(CPU_ES_INDEX, (WORD)src);
}

void
POP16_SS(void)
{
	WORD src;

	CPU_WORKCLOCK(5);

	POP0_16(src);
	CPU_SET_SEGREG(CPU_SS_INDEX, src);
	exec_1step();
}

void
POP32_SS(void)
{
	DWORD src;

	CPU_WORKCLOCK(5);

	POP0_32(src);
	CPU_SET_SEGREG(CPU_SS_INDEX, (WORD)src);
	exec_1step();
}

void
POP16_DS(void)
{
	WORD src;

	CPU_WORKCLOCK(5);

	POP0_16(src);
	CPU_SET_SEGREG(CPU_DS_INDEX, src);
}

void
POP32_DS(void)
{
	DWORD src;

	CPU_WORKCLOCK(5);

	POP0_32(src);
	CPU_SET_SEGREG(CPU_DS_INDEX, (WORD)src);
}

void
POP16_FS(void)
{
	WORD src;

	CPU_WORKCLOCK(5);

	POP0_16(src);
	CPU_SET_SEGREG(CPU_FS_INDEX, src);
}

void
POP32_FS(void)
{
	DWORD src;

	CPU_WORKCLOCK(5);

	POP0_32(src);
	CPU_SET_SEGREG(CPU_FS_INDEX, (WORD)src);
}

void
POP16_GS(void)
{
	WORD src;

	CPU_WORKCLOCK(5);

	POP0_16(src);
	CPU_SET_SEGREG(CPU_GS_INDEX, src);
}

void
POP32_GS(void)
{
	DWORD src;

	CPU_WORKCLOCK(5);

	POP0_32(src);
	CPU_SET_SEGREG(CPU_GS_INDEX, (WORD)src);
}

/*
 * PUSHA/POPA
 */
void
PUSHA(void)
{
	WORD sp = CPU_SP;

	CPU_WORKCLOCK(17);
	if (!CPU_STAT_SS32) {
		if (CPU_STAT_PM) {
			CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_SP, 16);
		}
		REGPUSH0(CPU_AX);
		REGPUSH0(CPU_CX);
		REGPUSH0(CPU_DX);
		REGPUSH0(CPU_BX);
		REGPUSH0(sp);
		REGPUSH0(CPU_BP);
		REGPUSH0(CPU_SI);
		REGPUSH0(CPU_DI);
	} else {
		if (CPU_STAT_PM) {
			CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 16);
		}
		REGPUSH0_16_32(CPU_AX);
		REGPUSH0_16_32(CPU_CX);
		REGPUSH0_16_32(CPU_DX);
		REGPUSH0_16_32(CPU_BX);
		REGPUSH0_16_32(sp);
		REGPUSH0_16_32(CPU_BP);
		REGPUSH0_16_32(CPU_SI);
		REGPUSH0_16_32(CPU_DI);
	}
}

void
PUSHAD(void)
{
	DWORD esp = CPU_ESP;

	CPU_WORKCLOCK(17);
	if (!CPU_STAT_SS32) {
		if (CPU_STAT_PM) {
			CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_SP, 32);
		}
		REGPUSH0_32_16(CPU_EAX);
		REGPUSH0_32_16(CPU_ECX);
		REGPUSH0_32_16(CPU_EDX);
		REGPUSH0_32_16(CPU_EBX);
		REGPUSH0_32_16(esp);
		REGPUSH0_32_16(CPU_EBP);
		REGPUSH0_32_16(CPU_ESI);
		REGPUSH0_32_16(CPU_EDI);
	} else {
		if (CPU_STAT_PM) {
			CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 32);
		}
		REGPUSH0_32(CPU_EAX);
		REGPUSH0_32(CPU_ECX);
		REGPUSH0_32(CPU_EDX);
		REGPUSH0_32(CPU_EBX);
		REGPUSH0_32(esp);
		REGPUSH0_32(CPU_EBP);
		REGPUSH0_32(CPU_ESI);
		REGPUSH0_32(CPU_EDI);
	}
}

void
POPA(void)
{

	CPU_WORKCLOCK(19);
	if (!CPU_STAT_SS32) {
		if (CPU_STAT_PM) {
			CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_SP, 16);
		}
		REGPOP0(CPU_DI);
		REGPOP0(CPU_SI);
		REGPOP0(CPU_BP);
		CPU_SP += 2;
		REGPOP0(CPU_BX);
		REGPOP0(CPU_DX);
		REGPOP0(CPU_CX);
		REGPOP0(CPU_AX);
	} else {
		if (CPU_STAT_PM) {
			CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 16);
		}
		REGPOP0_16_32(CPU_DI);
		REGPOP0_16_32(CPU_SI);
		REGPOP0_16_32(CPU_BP);
		CPU_ESP += 2;
		REGPOP0_16_32(CPU_BX);
		REGPOP0_16_32(CPU_DX);
		REGPOP0_16_32(CPU_CX);
		REGPOP0_16_32(CPU_AX);
	}
}

void
POPAD(void)
{

	CPU_WORKCLOCK(19);
	if (!CPU_STAT_SS32) {
		if (CPU_STAT_PM) {
			CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_SP, 32);
		}
		REGPOP0_32_16(CPU_EDI);
		REGPOP0_32_16(CPU_ESI);
		REGPOP0_32_16(CPU_EBP);
		CPU_SP += 4;
		REGPOP0_32_16(CPU_EBX);
		REGPOP0_32_16(CPU_EDX);
		REGPOP0_32_16(CPU_ECX);
		REGPOP0_32_16(CPU_EAX);
	} else {
		if (CPU_STAT_PM) {
			CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 32);
		}
		REGPOP0_32(CPU_EDI);
		REGPOP0_32(CPU_ESI);
		REGPOP0_32(CPU_EBP);
		CPU_ESP += 4;
		REGPOP0_32(CPU_EBX);
		REGPOP0_32(CPU_EDX);
		REGPOP0_32(CPU_ECX);
		REGPOP0_32(CPU_EAX);
	}
}

/*
 * in port
 */
void
IN_ALDX(void)
{

	CPU_WORKCLOCK(5);
	CPU_AL = cpu_in(CPU_DX);
}

void
IN_AXDX(void)
{

	CPU_WORKCLOCK(5);
	CPU_AX = cpu_in_w(CPU_DX);
}

void
IN_EAXDX(void)
{

	CPU_WORKCLOCK(5);
	CPU_EAX = cpu_in_d(CPU_DX);
}

void
IN_ALIb(void)
{
	WORD port;

	CPU_WORKCLOCK(5);
	GET_PCBYTE(port);
	CPU_AL = cpu_in(port);
}

void
IN_AXIb(void)
{
	WORD port;

	CPU_WORKCLOCK(5);
	GET_PCBYTE(port);
	CPU_AX = cpu_in_w(port);
}

void
IN_EAXIb(void)
{
	WORD port;

	CPU_WORKCLOCK(5);
	GET_PCBYTE(port);
	CPU_EAX = cpu_in_d(port);
}

/*
 * out port
 */
void
OUT_DXAL(void)
{

	CPU_WORKCLOCK(3);
	cpu_out(CPU_DX, CPU_AL);
}

void
OUT_DXAX(void)
{

	CPU_WORKCLOCK(3);
	cpu_out_w(CPU_DX, CPU_AX);
}

void
OUT_DXEAX(void)
{

	CPU_WORKCLOCK(3);
	cpu_out_d(CPU_DX, CPU_EAX);
}

void
OUT_IbAL(void)
{
	WORD port;

	CPU_WORKCLOCK(3);
	GET_PCBYTE(port);
	cpu_out(port, CPU_AL);
}

void
OUT_IbAX(void)
{
	WORD port;

	CPU_WORKCLOCK(3);
	GET_PCBYTE(port);
	cpu_out_w(port, CPU_AX);
}

void
OUT_IbEAX(void)
{
	WORD port;

	CPU_WORKCLOCK(3);
	GET_PCBYTE(port);
	cpu_out_d(port, CPU_EAX);
}

/*
 * convert
 */
void
CWD(void)
{

	CPU_WORKCLOCK(2);
	if (CPU_AX & 0x8000) {
		CPU_DX = 0xffff;
	} else {
		CPU_DX = 0;
	}
}

void
CDQ(void)
{

	CPU_WORKCLOCK(2);
	if (CPU_EAX & 0x80000000) {
		CPU_EDX = 0xffffffff;
	} else {
		CPU_EDX = 0;
	}
}

void
CBW(void)
{
	WORD tmp;

	CPU_WORKCLOCK(2);
	tmp = __CBW(CPU_AL);
	CPU_AX = tmp;
}

void
CWDE(void)
{
	DWORD tmp;

	CPU_WORKCLOCK(2);
	tmp = __CWDE(CPU_AX);
	CPU_EAX = tmp;
}

/*
 * MOVSx
 */
void
MOVSX_GwEb(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA8(op, src, out, 2, 5);
	*out = __CBW(src);
}

void
MOVSX_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	*out = (WORD)src;
}

void
MOVSX_GdEb(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA8(op, src, out, 2, 5);
	*out = __CBD(src);
}

void
MOVSX_GdEw(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA16(op, src, out, 2, 5);
	*out = __CWDE(src);
}

/*
 * MOVZx
 */
void
MOVZX_GwEb(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA8(op, src, out, 2, 5);
	*out = (BYTE)src;
}

void
MOVZX_GwEw(void)
{
	WORD *out;
	DWORD op, src;

	PREPART_REG16_EA(op, src, out, 2, 5);
	*out = (WORD)src;
}

void
MOVZX_GdEb(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA8(op, src, out, 2, 5);
	*out = (BYTE)src;
}

void
MOVZX_GdEw(void)
{
	DWORD *out;
	DWORD op, src;

	PREPART_REG32_EA16(op, src, out, 2, 5);
	*out = (WORD)src;
}

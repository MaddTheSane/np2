/*	$Id: bin_arith.c,v 1.3 2004/01/07 14:49:42 monaka Exp $	*/

/*
 * Copyright (c) 2002-2003 NONAKA Kimihiro
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

#include "bin_arith.h"


/*
 * ADD
 */
void
ADD_EbGb(void)
{
	BYTE *out;
	DWORD op, src, dst, res, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
		ADDBYTE(res, dst, src);
		*out = (BYTE)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		ADDBYTE(res, dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)res);
	}
}

void
ADD_EwGw(void)
{
	WORD *out;
	DWORD op, src, dst, res, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
		ADDWORD(res, dst, src);
		*out = (WORD)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		ADDWORD(res, dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)res);
	}
}

void
ADD_EdGd(void)
{
	DWORD *out;
	DWORD op, src, dst, res, madr;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		dst = *out;
		ADDDWORD(res, dst, src);
		*out = res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		ADDDWORD(res, dst, src);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, res);
	}
}

void
ADD_GbEb(void)
{
	BYTE *out;
	DWORD op, src, dst, res;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	ADDBYTE(res, dst, src);
	*out = (BYTE)res;
}

void
ADD_GwEw(void)
{
	WORD *out;
	DWORD op, src, dst, res;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	ADDWORD(res, dst, src);
	*out = (WORD)res;
}

void
ADD_GdEd(void)
{
	DWORD *out;
	DWORD op, src, dst, res;

	PREPART_REG32_EA(op, src, out, 2, 7);
	dst = *out;
	ADDDWORD(res, dst, src);
	*out = (DWORD)res;
}

void
ADD_ALIb(void)
{
	DWORD src, dst, res;

	CPU_WORKCLOCK(3);
	GET_PCBYTE(src);
	dst = CPU_AL;
	ADDBYTE(res, dst, src);
	CPU_AL = (BYTE)res;
}

void
ADD_AXIw(void)
{
	DWORD src, dst, res;

	CPU_WORKCLOCK(3);
	GET_PCWORD(src);
	dst = CPU_AX;
	ADDWORD(res, dst, src);
	CPU_AX = (WORD)res;
}

void
ADD_EAXId(void)
{
	DWORD src, dst, res;

	CPU_WORKCLOCK(3);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	ADDDWORD(res, dst, src);
	CPU_EAX = res;
}

void
ADD_EbIb(BYTE *regp, DWORD src)
{
	DWORD dst, res;

	dst = *regp;
	ADDBYTE(res, dst, src);
	*regp = (BYTE)res;
}

void
ADD_EbIb_ext(DWORD madr, DWORD src)
{
	DWORD dst, res;

	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	ADDBYTE(res, dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)res);
}

void
ADD_EwIx(WORD *regp, DWORD src)
{
	DWORD dst, res;

	dst = *regp;
	ADDWORD(res, dst, src);
	*regp = (WORD)res;
}

void
ADD_EwIx_ext(DWORD madr, DWORD src)
{
	DWORD dst, res;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	ADDWORD(res, dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)res);
}

void
ADD_EdIx(DWORD *regp, DWORD src)
{
	DWORD dst, res;

	dst = *regp;
	ADDDWORD(res, dst, src);
	*regp = res;
}

void
ADD_EdIx_ext(DWORD madr, DWORD src)
{
	DWORD dst, res;

	dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	ADDDWORD(res, dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, res);
}


/*
 * ADC
 */
void
ADC_EbGb(void)
{
	BYTE *out;
	DWORD op, src, dst, res, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
		ADCBYTE(res, dst, src);
		*out = (BYTE)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		ADCBYTE(res, dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)res);
	}
}

void
ADC_EwGw(void)
{
	WORD *out;
	DWORD op, src, dst, res, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
		ADCWORD(res, dst, src);
		*out = (WORD)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		ADCWORD(res, dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)res);
	}
}

void
ADC_EdGd(void)
{
	DWORD *out;
	DWORD op, src, dst, res, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		dst = *out;
		ADCDWORD(res, dst, src);
		*out = res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		ADCDWORD(res, dst, src);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, (WORD)res);
	}
}

void
ADC_GbEb(void)
{
	BYTE *out;
	DWORD op, src, dst, res;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	ADCBYTE(res, dst, src);
	*out = (BYTE)res;
}

void
ADC_GwEw(void)
{
	WORD *out;
	DWORD op, src, dst, res;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	ADCWORD(res, dst, src);
	*out = (WORD)res;
}

void
ADC_GdEd(void)
{
	DWORD *out;
	DWORD op, src, dst, res;

	PREPART_REG32_EA(op, src, out, 2, 7);
	dst = *out;
	ADCDWORD(res, dst, src);
	*out = res;
}

void
ADC_ALIb(void)
{
	DWORD src, dst, res;

	CPU_WORKCLOCK(3);
	GET_PCBYTE(src);
	dst = CPU_AL;
	ADCBYTE(res, dst, src);
	CPU_AL = (BYTE)res;
}

void
ADC_AXIw(void)
{
	DWORD src, dst, res;

	CPU_WORKCLOCK(3);
	GET_PCWORD(src);
	dst = CPU_AX;
	ADCWORD(res, dst, src);
	CPU_AX = (WORD)res;
}

void
ADC_EAXId(void)
{
	DWORD src, dst, res;

	CPU_WORKCLOCK(3);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	ADCDWORD(res, dst, src);
	CPU_EAX = res;
}

void
ADC_EbIb(BYTE *regp, DWORD src)
{
	DWORD dst, res;

	dst = *regp;
	ADCBYTE(res, dst, src);
	*regp = (BYTE)res;
}

void
ADC_EbIb_ext(DWORD madr, DWORD src)
{
	DWORD dst, res;

	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	ADCBYTE(res, dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)res);
}

void
ADC_EwIx(WORD *regp, DWORD src)
{
	DWORD dst, res;

	dst = *regp;
	ADCWORD(res, dst, src);
	*regp = (WORD)res;
}

void
ADC_EwIx_ext(DWORD madr, DWORD src)
{
	DWORD dst, res;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	ADCWORD(res, dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)res);
}

void
ADC_EdIx(DWORD *regp, DWORD src)
{
	DWORD dst, res;

	dst = *regp;
	ADCDWORD(res, dst, src);
	*regp = res;
}

void
ADC_EdIx_ext(DWORD madr, DWORD src)
{
	DWORD dst, res;

	dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	ADCDWORD(res, dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, res);
}


/*
 * SUB
 */
void
SUB_EbGb(void)
{
	BYTE *out;
	DWORD op, src, dst, res, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
		BYTE_SUB(res, dst, src);
		*out = (BYTE)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		BYTE_SUB(res, dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)res);
	}
}

void
SUB_EwGw(void)
{
	WORD *out;
	DWORD op, src, dst, res, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
		WORD_SUB(res, dst, src);
		*out = (WORD)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_SUB(res, dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)res);
	}
}

void
SUB_EdGd(void)
{
	DWORD *out;
	DWORD op, src, dst, res, madr;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		dst = *out;
		DWORD_SUB(res, dst, src);
		*out = res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		DWORD_SUB(res, dst, src);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, res);
	}
}

void
SUB_GbEb(void)
{
	BYTE *out;
	DWORD op, src, dst, res;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	BYTE_SUB(res, dst, src);
	*out = (BYTE)res;
}

void
SUB_GwEw(void)
{
	WORD *out;
	DWORD op, src, dst, res;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	WORD_SUB(res, dst, src);
	*out = (WORD)res;
}

void
SUB_GdEd(void)
{
	DWORD *out;
	DWORD op, src, dst, res;

	PREPART_REG32_EA(op, src, out, 2, 7);
	dst = *out;
	DWORD_SUB(res, dst, src);
	*out = res;
}

void
SUB_ALIb(void)
{
	DWORD src, dst, res;

	CPU_WORKCLOCK(3);
	GET_PCBYTE(src);
	dst = CPU_AL;
	BYTE_SUB(res, dst, src);
	CPU_AL = (BYTE)res;
}

void
SUB_AXIw(void)
{
	DWORD src, dst, res;

	CPU_WORKCLOCK(3);
	GET_PCWORD(src);
	dst = CPU_AX;
	WORD_SUB(res, dst, src);
	CPU_AX = (WORD)res;
}

void
SUB_EAXId(void)
{
	DWORD src, dst, res;

	CPU_WORKCLOCK(3);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	DWORD_SUB(res, dst, src);
	CPU_EAX = res;
}

void
SUB_EbIb(BYTE *regp, DWORD src)
{
	DWORD dst, res;

	dst = *regp;
	BYTE_SUB(res, dst, src);
	*regp = (BYTE)res;
}

void
SUB_EbIb_ext(DWORD madr, DWORD src)
{
	DWORD dst, res;

	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_SUB(res, dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)res);
}

void
SUB_EwIx(WORD *regp, DWORD src)
{
	DWORD dst, res;

	dst = *regp;
	WORD_SUB(res, dst, src);
	*regp = (WORD)res;
}

void
SUB_EwIx_ext(DWORD madr, DWORD src)
{
	DWORD dst, res;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_SUB(res, dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)res);
}

void
SUB_EdIx(DWORD *regp, DWORD src)
{
	DWORD dst, res;

	dst = *regp;
	DWORD_SUB(res, dst, src);
	*regp = res;
}

void
SUB_EdIx_ext(DWORD madr, DWORD src)
{
	DWORD dst, res;

	dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_SUB(res, dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, res);
}


/*
 * SBB
 */
void
SBB_EbGb(void)
{
	BYTE *out;
	DWORD op, src, dst, res, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
		BYTE_SBB(res, dst, src);
		*out = (BYTE)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		BYTE_SBB(res, dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)res);
	}
}

void
SBB_EwGw(void)
{
	WORD *out;
	DWORD op, src, dst, res, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
		WORD_SBB(res, dst, src);
		*out = (WORD)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_SBB(res, dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)res);
	}
}

void
SBB_EdGd(void)
{
	DWORD *out;
	DWORD op, src, dst, res, madr;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		dst = *out;
		DWORD_SBB(res, dst, src);
		*out = res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		DWORD_SBB(res, dst, src);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, res);
	}
}

void
SBB_GbEb(void)
{
	BYTE *out;
	DWORD op, src, dst, res;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	BYTE_SBB(res, dst, src);
	*out = (BYTE)res;
}

void
SBB_GwEw(void)
{
	WORD *out;
	DWORD op, src, dst, res;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	WORD_SBB(res, dst, src);
	*out = (WORD)res;
}

void
SBB_GdEd(void)
{
	DWORD *out;
	DWORD op, src, dst, res;

	PREPART_REG32_EA(op, src, out, 2, 7);
	dst = *out;
	DWORD_SBB(res, dst, src);
	*out = res;
}

void
SBB_ALIb(void)
{
	DWORD src, dst, res;

	CPU_WORKCLOCK(3);
	GET_PCBYTE(src);
	dst = CPU_AL;
	BYTE_SBB(res, dst, src);
	CPU_AL = (BYTE)res;
}

void
SBB_AXIw(void)
{
	DWORD src, dst, res;

	CPU_WORKCLOCK(3);
	GET_PCWORD(src);
	dst = CPU_AX;
	WORD_SBB(res, dst, src);
	CPU_AX = (WORD)res;
}

void
SBB_EAXId(void)
{
	DWORD src, dst, res;

	CPU_WORKCLOCK(3);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	DWORD_SBB(res, dst, src);
	CPU_EAX = res;
}

void
SBB_EbIb(BYTE *regp, DWORD src)
{
	DWORD dst, res;

	dst = *regp;
	BYTE_SBB(res, dst, src);
	*regp = (BYTE)res;
}

void
SBB_EbIb_ext(DWORD madr, DWORD src)
{
	DWORD dst, res;

	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_SBB(res, dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)res);
}

void
SBB_EwIx(WORD *regp, DWORD src)
{
	DWORD dst, res;

	dst = *regp;
	WORD_SBB(res, dst, src);
	*regp = (WORD)res;
}

void
SBB_EwIx_ext(DWORD madr, DWORD src)
{
	DWORD dst, res;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_SBB(res, dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)res);
}

void
SBB_EdIx(DWORD *regp, DWORD src)
{
	DWORD dst, res;

	dst = *regp;
	DWORD_SBB(res, dst, src);
	*regp = res;
}

void
SBB_EdIx_ext(DWORD madr, DWORD src)
{
	DWORD dst, res;

	dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_SBB(res, dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, res);
}


/*
 * IMUL
 */
void
IMUL_ALEb(DWORD op)
{
	DWORD madr;
	SDWORD res;
	SBYTE src, dst;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(13);
		src = *(reg8_b20[op]);
	} else {
		CPU_WORKCLOCK(16);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	}
	dst = CPU_AL;
	BYTE_IMUL(res, dst, src);
	CPU_AX = (WORD)res;
}

void
IMUL_AXEw(DWORD op)
{
	DWORD madr;
	SDWORD res;
	SWORD src, dst;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(21);
		src = *(reg16_b20[op]);
	} else {
		CPU_WORKCLOCK(24);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	}
	dst = CPU_AX;
	WORD_IMUL(res, dst, src);
	CPU_AX = (WORD)(res & 0xffff);
	CPU_DX = (WORD)(res >> 16);
}

void
IMUL_EAXEd(DWORD op)
{
	DWORD madr;
	SQWORD res;
	SDWORD src, dst;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(21);
		src = *(reg32_b20[op]);
	} else {
		CPU_WORKCLOCK(24);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	}
	dst = CPU_EAX;
	DWORD_IMUL(res, dst, src);
	CPU_EAX = (DWORD)res;
	CPU_EDX = (DWORD)(res >> 32);
}

void
IMUL_GwEw(void)
{
	WORD *out;
	DWORD op;
	SDWORD res;
	SWORD src, dst;

	PREPART_REG16_EA(op, src, out, 21, 27);
	dst = *out;
	WORD_IMUL(res, dst, src);
	*out = (WORD)res;
}

void
IMUL_GdEd(void)
{
	DWORD *out;
	DWORD op;
	SQWORD res;
	SDWORD src, dst;

	PREPART_REG32_EA(op, src, out, 21, 27);
	dst = *out;
	DWORD_IMUL(res, dst, src);
	*out = (DWORD)res;
}

void
IMUL_GwEwIb(void)
{
	WORD *out;
	DWORD op;
	SDWORD res;
	SWORD src, dst;

	PREPART_REG16_EA(op, src, out, 21, 24);
	GET_PCBYTES(dst);
	WORD_IMUL(res, dst, src);
	*out = (WORD)res;
}

void
IMUL_GdEdIb(void)
{
	DWORD *out;
	DWORD op;
	SQWORD res;
	SDWORD src, dst;

	PREPART_REG32_EA(op, src, out, 21, 24);
	GET_PCBYTESD(dst);
	DWORD_IMUL(res, dst, src);
	*out = (DWORD)res;
}

void
IMUL_GwEwIw(void)
{
	WORD *out;
	DWORD op;
	SDWORD res;
	SWORD src, dst;

	PREPART_REG16_EA(op, src, out, 21, 24);
	GET_PCWORD(dst);
	WORD_IMUL(res, dst, src);
	*out = (WORD)res;
}

void
IMUL_GdEdId(void)
{
	DWORD *out;
	DWORD op;
	SQWORD res;
	SDWORD src, dst;

	PREPART_REG32_EA(op, src, out, 21, 24);
	GET_PCDWORD(dst);
	DWORD_IMUL(res, dst, src);
	*out = res;
}


/*
 * MUL
 */
void
MUL_ALEb(DWORD op)
{
	DWORD res, madr;
	BYTE src, dst;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(13);
		src = *(reg8_b20[op]);
	} else {
		CPU_WORKCLOCK(16);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	}
	dst = CPU_AL;
	BYTE_MUL(res, dst, src);
	CPU_AX = (WORD)res;
}

void
MUL_AXEw(DWORD op)
{
	DWORD res, madr;
	WORD src, dst;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(21);
		src = *(reg16_b20[op]);
	} else {
		CPU_WORKCLOCK(24);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	}
	dst = CPU_AX;
	WORD_MUL(res, dst, src);
	CPU_AX = (WORD)res;
	CPU_DX = (WORD)(res >> 16);
}

void
MUL_EAXEd(DWORD op)
{
	DWORD res, madr;
	DWORD src, dst;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(21);
		src = *(reg32_b20[op]);
	} else {
		CPU_WORKCLOCK(24);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	}
	dst = CPU_EAX;
	DWORD_MUL(res, dst, src);
	CPU_EAX = res;
	CPU_EDX = CPU_OV;
}


/*
 * IDIV
 */
void
IDIV_ALEb(DWORD op)
{
	DWORD madr;
	SWORD tmp, r;
	SBYTE src;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(17);
		src = *(reg8_b20[op]);
	} else {
		CPU_WORKCLOCK(25);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	}
	tmp = (SWORD)CPU_AX;
	if (src != 0) {
		r = tmp / src;
		if (((r + 0x80) & 0xff00) == 0) {
			CPU_AL = (SBYTE)r;
			CPU_AH = tmp % src;
			return;
		}
	}
	EXCEPTION(DE_EXCEPTION, 0);
}

void
IDIV_AXEw(DWORD op)
{
	SDWORD tmp, r;
	DWORD madr;
	SWORD src;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(17);
		src = *(reg16_b20[op]);
	} else {
		CPU_WORKCLOCK(25);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	}
	tmp = (SDWORD)(((DWORD)CPU_DX << 16) + CPU_AX);
	if (src != 0) {
		r = tmp / src;
		if (((r + 0x8000) & 0xffff0000) == 0) {
			CPU_AX = (SWORD)r;
			CPU_DX = tmp % src;
			return;
		}
	}
	EXCEPTION(DE_EXCEPTION, 0);
}

void
IDIV_EAXEd(DWORD op)
{
	SQWORD tmp, r;
	SDWORD src;
	DWORD madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(17);
		src = *(reg32_b20[op]);
	} else {
		CPU_WORKCLOCK(25);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	}
	tmp = (SQWORD)(((QWORD)CPU_EDX << 32) + CPU_EAX);
	if (src != 0) {
		r = tmp / src;
		if (((r + 0x80000000) & 0xffffffff00000000ULL) == 0) {
			CPU_EAX = (SDWORD)r;
			CPU_EDX = tmp % src;
			return;
		}
	}
	EXCEPTION(DE_EXCEPTION, 0);
}


/*
 * DIV
 */
void
DIV_ALEb(DWORD op)
{
	DWORD madr;
	WORD tmp;
	BYTE src;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(17);
		src = *(reg8_b20[op]);
	} else {
		CPU_WORKCLOCK(25);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	}
	tmp = CPU_AX;
	if (src != 0) {
		if (tmp < ((WORD)src << 8)) {
			CPU_AL = tmp / src;
			CPU_AH = tmp % src;
			return;
		}
	}
	EXCEPTION(DE_EXCEPTION, 0);
}

void
DIV_AXEw(DWORD op)
{
	DWORD madr;
	DWORD tmp;
	WORD src;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(17);
		src = *(reg16_b20[op]);
	} else {
		CPU_WORKCLOCK(25);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	}
	tmp = ((DWORD)CPU_DX << 16) + CPU_AX;
	if (src != 0) {
		if (tmp < ((DWORD)src << 16)) {
			CPU_AX = (WORD)(tmp / src);
			CPU_DX = (WORD)(tmp % src);
			return;
		}
	}
	EXCEPTION(DE_EXCEPTION, 0);
}

void
DIV_EAXEd(DWORD op)
{
	DWORD madr;
	QWORD tmp;
	DWORD src;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(17);
		src = *(reg32_b20[op]);
	} else {
		CPU_WORKCLOCK(25);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	}
	tmp = ((QWORD)CPU_EDX << 32) + CPU_EAX;
	if (src != 0) {
		if (tmp < ((QWORD)src << 32)) {
			CPU_EAX = (DWORD)(tmp / src);
			CPU_EDX = (DWORD)(tmp % src);
			return;
		}
	}
	EXCEPTION(DE_EXCEPTION, 0);
}


/*
 * INC
 */
void
INC_Eb(DWORD op)
{
	BYTE *out;
	DWORD madr;
	BYTE value;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		value = *out;
		BYTE_INC(value);
		*out = value;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		value = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		BYTE_INC(value);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, value);
	}
}

void
INC_Ew(DWORD op)
{
	WORD *out;
	DWORD madr;
	WORD value;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		value = *out;
		WORD_INC(value);
		*out = value;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		value = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_INC(value);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, value);
	}
}

void
INC_Ed(DWORD op)
{
	DWORD *out;
	DWORD madr;
	DWORD value;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		value = *out;
		DWORD_INC(value);
		*out = value;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		value = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		DWORD_INC(value);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, value);
	}
}

void INC_AX(void) { WORD_INC(CPU_AX); CPU_WORKCLOCK(2); }
void INC_CX(void) { WORD_INC(CPU_CX); CPU_WORKCLOCK(2); }
void INC_DX(void) { WORD_INC(CPU_DX); CPU_WORKCLOCK(2); }
void INC_BX(void) { WORD_INC(CPU_BX); CPU_WORKCLOCK(2); }
void INC_SP(void) { WORD_INC(CPU_SP); CPU_WORKCLOCK(2); }
void INC_BP(void) { WORD_INC(CPU_BP); CPU_WORKCLOCK(2); }
void INC_SI(void) { WORD_INC(CPU_SI); CPU_WORKCLOCK(2); }
void INC_DI(void) { WORD_INC(CPU_DI); CPU_WORKCLOCK(2); }

void INC_EAX(void) { DWORD_INC(CPU_EAX); CPU_WORKCLOCK(2); }
void INC_ECX(void) { DWORD_INC(CPU_ECX); CPU_WORKCLOCK(2); }
void INC_EDX(void) { DWORD_INC(CPU_EDX); CPU_WORKCLOCK(2); }
void INC_EBX(void) { DWORD_INC(CPU_EBX); CPU_WORKCLOCK(2); }
void INC_ESP(void) { DWORD_INC(CPU_ESP); CPU_WORKCLOCK(2); }
void INC_EBP(void) { DWORD_INC(CPU_EBP); CPU_WORKCLOCK(2); }
void INC_ESI(void) { DWORD_INC(CPU_ESI); CPU_WORKCLOCK(2); }
void INC_EDI(void) { DWORD_INC(CPU_EDI); CPU_WORKCLOCK(2); }



/*
 * DEC
 */
void
DEC_Eb(DWORD op)
{
	BYTE *out;
	DWORD madr;
	BYTE value;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		value = *out;
		BYTE_DEC(value);
		*out = value;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		value = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		BYTE_DEC(value);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, value);
	}
}

void
DEC_Ew(DWORD op)
{
	WORD *out;
	DWORD madr;
	WORD value;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		value = *out;
		WORD_DEC(value);
		*out = value;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		value = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_DEC(value);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, value);
	}
}

void
DEC_Ed(DWORD op)
{
	DWORD *out;
	DWORD madr;
	DWORD value;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		value = *out;
		DWORD_DEC(value);
		*out = value;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		value = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		DWORD_DEC(value);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, value);
	}
}

void DEC_AX(void) { WORD_DEC(CPU_AX); CPU_WORKCLOCK(2); }
void DEC_CX(void) { WORD_DEC(CPU_CX); CPU_WORKCLOCK(2); }
void DEC_DX(void) { WORD_DEC(CPU_DX); CPU_WORKCLOCK(2); }
void DEC_BX(void) { WORD_DEC(CPU_BX); CPU_WORKCLOCK(2); }
void DEC_SP(void) { WORD_DEC(CPU_SP); CPU_WORKCLOCK(2); }
void DEC_BP(void) { WORD_DEC(CPU_BP); CPU_WORKCLOCK(2); }
void DEC_SI(void) { WORD_DEC(CPU_SI); CPU_WORKCLOCK(2); }
void DEC_DI(void) { WORD_DEC(CPU_DI); CPU_WORKCLOCK(2); }

void DEC_EAX(void) { DWORD_DEC(CPU_EAX); CPU_WORKCLOCK(2); }
void DEC_ECX(void) { DWORD_DEC(CPU_ECX); CPU_WORKCLOCK(2); }
void DEC_EDX(void) { DWORD_DEC(CPU_EDX); CPU_WORKCLOCK(2); }
void DEC_EBX(void) { DWORD_DEC(CPU_EBX); CPU_WORKCLOCK(2); }
void DEC_ESP(void) { DWORD_DEC(CPU_ESP); CPU_WORKCLOCK(2); }
void DEC_EBP(void) { DWORD_DEC(CPU_EBP); CPU_WORKCLOCK(2); }
void DEC_ESI(void) { DWORD_DEC(CPU_ESI); CPU_WORKCLOCK(2); }
void DEC_EDI(void) { DWORD_DEC(CPU_EDI); CPU_WORKCLOCK(2); }


/*
 * NEG
 */
void
NEG_Eb(DWORD op)
{
	BYTE *out;
	DWORD src, dst, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		src = *out;
		BYTE_NEG(dst, src);
		*out = (BYTE)dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		BYTE_NEG(dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)dst);
	}
}

void
NEG_Ew(DWORD op)
{
	WORD *out;
	DWORD src, dst, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		src = *out;
		WORD_NEG(dst, src);
		*out = (WORD)dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_NEG(dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)dst);
	}
}

void
NEG_Ed(DWORD op)
{
	DWORD *out;
	DWORD src, dst, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		src = *out;
		DWORD_NEG(dst, src);
		*out = dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		DWORD_NEG(dst, src);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
	}
}


/*
 * CMP
 */
void
CMP_EbGb(void)
{
	BYTE *out;
	DWORD op, src, dst, res, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	}
	BYTE_SUB(res, dst, src);
}

void
CMP_EwGw(void)
{
	WORD *out;
	DWORD op, src, dst, res, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	}
	WORD_SUB(res, dst, src);
}

void
CMP_EdGd(void)
{
	DWORD *out;
	DWORD op, src, dst, res, madr;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		dst = *out;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	}
	DWORD_SUB(res, dst, src);
}

void
CMP_GbEb(void)
{
	BYTE *out;
	DWORD op, src, dst, res;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	BYTE_SUB(res, dst, src);
}

void
CMP_GwEw(void)
{
	WORD *out;
	DWORD op, src, dst, res;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	WORD_SUB(res, dst, src);
}

void
CMP_GdEd(void)
{
	DWORD *out;
	DWORD op, src, dst, res;

	PREPART_REG32_EA(op, src, out, 2, 7);
	dst = *out;
	DWORD_SUB(res, dst, src);
}

void
CMP_ALIb(void)
{
	DWORD src, dst, res;

	CPU_WORKCLOCK(3);
	GET_PCBYTE(src);
	dst = CPU_AL;
	BYTE_SUB(res, dst, src);
}

void
CMP_AXIw(void)
{
	DWORD src, dst, res;

	CPU_WORKCLOCK(3);
	GET_PCWORD(src);
	dst = CPU_AX;
	WORD_SUB(res, dst, src);
}

void
CMP_EAXId(void)
{
	DWORD src, dst, res;

	CPU_WORKCLOCK(3);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	DWORD_SUB(res, dst, src);
}

void
CMP_EbIb(BYTE *regp, DWORD src)
{
	DWORD dst, res;

	dst = *regp;
	BYTE_SUB(res, dst, src);
}

void
CMP_EbIb_ext(DWORD madr, DWORD src)
{
	DWORD dst, res;

	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_SUB(res, dst, src);
}

void
CMP_EwIx(WORD *regp, DWORD src)
{
	DWORD dst, res;

	dst = *regp;
	WORD_SUB(res, dst, src);
}

void
CMP_EwIx_ext(DWORD madr, DWORD src)
{
	DWORD dst, res;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_SUB(res, dst, src);
}

void
CMP_EdIx(DWORD *regp, DWORD src)
{
	DWORD dst, res;

	dst = *regp;
	DWORD_SUB(res, dst, src);
}

void
CMP_EdIx_ext(DWORD madr, DWORD src)
{
	DWORD dst, res;

	dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_SUB(res, dst, src);
}

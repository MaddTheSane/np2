/*	$Id: bin_arith.c,v 1.9 2004/02/20 16:09:05 monaka Exp $	*/

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
	UINT8 *out;
	UINT32 op, src, dst, res, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
		ADD_BYTE(res, dst, src);
		*out = (UINT8)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		ADD_BYTE(res, dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)res);
	}
}

void
ADD_EwGw(void)
{
	UINT16 *out;
	UINT32 op, src, dst, res, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
		ADD_WORD(res, dst, src);
		*out = (UINT16)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		ADD_WORD(res, dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)res);
	}
}

void
ADD_EdGd(void)
{
	UINT32 *out;
	UINT32 op, src, dst, res, madr;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		dst = *out;
		ADD_DWORD(res, dst, src);
		*out = res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		ADD_DWORD(res, dst, src);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, res);
	}
}

void
ADD_GbEb(void)
{
	UINT8 *out;
	UINT32 op, src, dst, res;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	ADD_BYTE(res, dst, src);
	*out = (UINT8)res;
}

void
ADD_GwEw(void)
{
	UINT16 *out;
	UINT32 op, src, dst, res;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	ADD_WORD(res, dst, src);
	*out = (UINT16)res;
}

void
ADD_GdEd(void)
{
	UINT32 *out;
	UINT32 op, src, dst, res;

	PREPART_REG32_EA(op, src, out, 2, 7);
	dst = *out;
	ADD_DWORD(res, dst, src);
	*out = res;
}

void
ADD_ALIb(void)
{
	UINT32 src, dst, res;

	CPU_WORKCLOCK(2);
	GET_PCBYTE(src);
	dst = CPU_AL;
	ADD_BYTE(res, dst, src);
	CPU_AL = (UINT8)res;
}

void
ADD_AXIw(void)
{
	UINT32 src, dst, res;

	CPU_WORKCLOCK(2);
	GET_PCWORD(src);
	dst = CPU_AX;
	ADD_WORD(res, dst, src);
	CPU_AX = (UINT16)res;
}

void
ADD_EAXId(void)
{
	UINT32 src, dst, res;

	CPU_WORKCLOCK(2);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	ADD_DWORD(res, dst, src);
	CPU_EAX = res;
}

void
ADD_EbIb(UINT8 *regp, UINT32 src)
{
	UINT32 dst, res;

	dst = *regp;
	ADD_BYTE(res, dst, src);
	*regp = (UINT8)res;
}

void
ADD_EbIb_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst, res;

	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	ADD_BYTE(res, dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)res);
}

void
ADD_EwIx(UINT16 *regp, UINT32 src)
{
	UINT32 dst, res;

	dst = *regp;
	ADD_WORD(res, dst, src);
	*regp = (UINT16)res;
}

void
ADD_EwIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst, res;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	ADD_WORD(res, dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)res);
}

void
ADD_EdIx(UINT32 *regp, UINT32 src)
{
	UINT32 dst, res;

	dst = *regp;
	ADD_DWORD(res, dst, src);
	*regp = res;
}

void
ADD_EdIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst, res;

	dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	ADD_DWORD(res, dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, res);
}


/*
 * ADC
 */
void
ADC_EbGb(void)
{
	UINT8 *out;
	UINT32 op, src, dst, res, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
		ADC_BYTE(res, dst, src);
		*out = (UINT8)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		ADC_BYTE(res, dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)res);
	}
}

void
ADC_EwGw(void)
{
	UINT16 *out;
	UINT32 op, src, dst, res, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
		ADC_WORD(res, dst, src);
		*out = (UINT16)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		ADC_WORD(res, dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)res);
	}
}

void
ADC_EdGd(void)
{
	UINT32 *out;
	UINT32 op, src, dst, res, madr;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		dst = *out;
		ADC_DWORD(res, dst, src);
		*out = res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		ADC_DWORD(res, dst, src);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, res);
	}
}

void
ADC_GbEb(void)
{
	UINT8 *out;
	UINT32 op, src, dst, res;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	ADC_BYTE(res, dst, src);
	*out = (UINT8)res;
}

void
ADC_GwEw(void)
{
	UINT16 *out;
	UINT32 op, src, dst, res;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	ADC_WORD(res, dst, src);
	*out = (UINT16)res;
}

void
ADC_GdEd(void)
{
	UINT32 *out;
	UINT32 op, src, dst, res;

	PREPART_REG32_EA(op, src, out, 2, 7);
	dst = *out;
	ADC_DWORD(res, dst, src);
	*out = res;
}

void
ADC_ALIb(void)
{
	UINT32 src, dst, res;

	CPU_WORKCLOCK(2);
	GET_PCBYTE(src);
	dst = CPU_AL;
	ADC_BYTE(res, dst, src);
	CPU_AL = (UINT8)res;
}

void
ADC_AXIw(void)
{
	UINT32 src, dst, res;

	CPU_WORKCLOCK(2);
	GET_PCWORD(src);
	dst = CPU_AX;
	ADC_WORD(res, dst, src);
	CPU_AX = (UINT16)res;
}

void
ADC_EAXId(void)
{
	UINT32 src, dst, res;

	CPU_WORKCLOCK(2);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	ADC_DWORD(res, dst, src);
	CPU_EAX = res;
}

void
ADC_EbIb(UINT8 *regp, UINT32 src)
{
	UINT32 dst, res;

	dst = *regp;
	ADC_BYTE(res, dst, src);
	*regp = (UINT8)res;
}

void
ADC_EbIb_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst, res;

	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	ADC_BYTE(res, dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)res);
}

void
ADC_EwIx(UINT16 *regp, UINT32 src)
{
	UINT32 dst, res;

	dst = *regp;
	ADC_WORD(res, dst, src);
	*regp = (UINT16)res;
}

void
ADC_EwIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst, res;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	ADC_WORD(res, dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)res);
}

void
ADC_EdIx(UINT32 *regp, UINT32 src)
{
	UINT32 dst, res;

	dst = *regp;
	ADC_DWORD(res, dst, src);
	*regp = res;
}

void
ADC_EdIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst, res;

	dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	ADC_DWORD(res, dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, res);
}


/*
 * SUB
 */
void
SUB_EbGb(void)
{
	UINT8 *out;
	UINT32 op, src, dst, res, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
		BYTE_SUB(res, dst, src);
		*out = (UINT8)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		BYTE_SUB(res, dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)res);
	}
}

void
SUB_EwGw(void)
{
	UINT16 *out;
	UINT32 op, src, dst, res, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
		WORD_SUB(res, dst, src);
		*out = (UINT16)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_SUB(res, dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)res);
	}
}

void
SUB_EdGd(void)
{
	UINT32 *out;
	UINT32 op, src, dst, res, madr;

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
	UINT8 *out;
	UINT32 op, src, dst, res;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	BYTE_SUB(res, dst, src);
	*out = (UINT8)res;
}

void
SUB_GwEw(void)
{
	UINT16 *out;
	UINT32 op, src, dst, res;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	WORD_SUB(res, dst, src);
	*out = (UINT16)res;
}

void
SUB_GdEd(void)
{
	UINT32 *out;
	UINT32 op, src, dst, res;

	PREPART_REG32_EA(op, src, out, 2, 7);
	dst = *out;
	DWORD_SUB(res, dst, src);
	*out = res;
}

void
SUB_ALIb(void)
{
	UINT32 src, dst, res;

	CPU_WORKCLOCK(2);
	GET_PCBYTE(src);
	dst = CPU_AL;
	BYTE_SUB(res, dst, src);
	CPU_AL = (UINT8)res;
}

void
SUB_AXIw(void)
{
	UINT32 src, dst, res;

	CPU_WORKCLOCK(2);
	GET_PCWORD(src);
	dst = CPU_AX;
	WORD_SUB(res, dst, src);
	CPU_AX = (UINT16)res;
}

void
SUB_EAXId(void)
{
	UINT32 src, dst, res;

	CPU_WORKCLOCK(2);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	DWORD_SUB(res, dst, src);
	CPU_EAX = res;
}

void
SUB_EbIb(UINT8 *regp, UINT32 src)
{
	UINT32 dst, res;

	dst = *regp;
	BYTE_SUB(res, dst, src);
	*regp = (UINT8)res;
}

void
SUB_EbIb_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst, res;

	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_SUB(res, dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)res);
}

void
SUB_EwIx(UINT16 *regp, UINT32 src)
{
	UINT32 dst, res;

	dst = *regp;
	WORD_SUB(res, dst, src);
	*regp = (UINT16)res;
}

void
SUB_EwIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst, res;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_SUB(res, dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)res);
}

void
SUB_EdIx(UINT32 *regp, UINT32 src)
{
	UINT32 dst, res;

	dst = *regp;
	DWORD_SUB(res, dst, src);
	*regp = res;
}

void
SUB_EdIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst, res;

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
	UINT8 *out;
	UINT32 op, src, dst, res, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
		BYTE_SBB(res, dst, src);
		*out = (UINT8)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		BYTE_SBB(res, dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)res);
	}
}

void
SBB_EwGw(void)
{
	UINT16 *out;
	UINT32 op, src, dst, res, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
		WORD_SBB(res, dst, src);
		*out = (UINT16)res;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_SBB(res, dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)res);
	}
}

void
SBB_EdGd(void)
{
	UINT32 *out;
	UINT32 op, src, dst, res, madr;

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
	UINT8 *out;
	UINT32 op, src, dst, res;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	BYTE_SBB(res, dst, src);
	*out = (UINT8)res;
}

void
SBB_GwEw(void)
{
	UINT16 *out;
	UINT32 op, src, dst, res;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	WORD_SBB(res, dst, src);
	*out = (UINT16)res;
}

void
SBB_GdEd(void)
{
	UINT32 *out;
	UINT32 op, src, dst, res;

	PREPART_REG32_EA(op, src, out, 2, 7);
	dst = *out;
	DWORD_SBB(res, dst, src);
	*out = res;
}

void
SBB_ALIb(void)
{
	UINT32 src, dst, res;

	CPU_WORKCLOCK(2);
	GET_PCBYTE(src);
	dst = CPU_AL;
	BYTE_SBB(res, dst, src);
	CPU_AL = (UINT8)res;
}

void
SBB_AXIw(void)
{
	UINT32 src, dst, res;

	CPU_WORKCLOCK(2);
	GET_PCWORD(src);
	dst = CPU_AX;
	WORD_SBB(res, dst, src);
	CPU_AX = (UINT16)res;
}

void
SBB_EAXId(void)
{
	UINT32 src, dst, res;

	CPU_WORKCLOCK(2);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	DWORD_SBB(res, dst, src);
	CPU_EAX = res;
}

void
SBB_EbIb(UINT8 *regp, UINT32 src)
{
	UINT32 dst, res;

	dst = *regp;
	BYTE_SBB(res, dst, src);
	*regp = (UINT8)res;
}

void
SBB_EbIb_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst, res;

	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_SBB(res, dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)res);
}

void
SBB_EwIx(UINT16 *regp, UINT32 src)
{
	UINT32 dst, res;

	dst = *regp;
	WORD_SBB(res, dst, src);
	*regp = (UINT16)res;
}

void
SBB_EwIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst, res;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_SBB(res, dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)res);
}

void
SBB_EdIx(UINT32 *regp, UINT32 src)
{
	UINT32 dst, res;

	dst = *regp;
	DWORD_SBB(res, dst, src);
	*regp = res;
}

void
SBB_EdIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst, res;

	dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_SBB(res, dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, res);
}


/*
 * IMUL
 */
void
IMUL_ALEb(UINT32 op)
{
	UINT32 madr;
	SINT32 res;
	SINT8 src, dst;

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
	CPU_AX = (UINT16)res;
}

void
IMUL_AXEw(UINT32 op)
{
	UINT32 madr;
	SINT32 res;
	SINT16 src, dst;

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
	CPU_AX = (UINT16)(res & 0xffff);
	CPU_DX = (UINT16)(res >> 16);
}

void
IMUL_EAXEd(UINT32 op)
{
	UINT32 madr;
	SINT64 res;
	SINT32 src, dst;

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
	CPU_EAX = (UINT32)res;
	CPU_EDX = (UINT32)(res >> 32);
}

void
IMUL_GwEw(void)
{
	UINT16 *out;
	UINT32 op;
	SINT32 res;
	SINT16 src, dst;

	PREPART_REG16_EA(op, src, out, 21, 27);
	dst = *out;
	WORD_IMUL(res, dst, src);
	*out = (UINT16)res;
}

void
IMUL_GdEd(void)
{
	UINT32 *out;
	UINT32 op;
	SINT64 res;
	SINT32 src, dst;

	PREPART_REG32_EA(op, src, out, 21, 27);
	dst = *out;
	DWORD_IMUL(res, dst, src);
	*out = (UINT32)res;
}

void
IMUL_GwEwIb(void)
{
	UINT16 *out;
	UINT32 op;
	SINT32 res;
	SINT16 src, dst;

	PREPART_REG16_EA(op, src, out, 21, 24);
	GET_PCBYTES(dst);
	WORD_IMUL(res, dst, src);
	*out = (UINT16)res;
}

void
IMUL_GdEdIb(void)
{
	UINT32 *out;
	UINT32 op;
	SINT64 res;
	SINT32 src, dst;

	PREPART_REG32_EA(op, src, out, 21, 24);
	GET_PCBYTESD(dst);
	DWORD_IMUL(res, dst, src);
	*out = (UINT32)res;
}

void
IMUL_GwEwIw(void)
{
	UINT16 *out;
	UINT32 op;
	SINT32 res;
	SINT16 src, dst;

	PREPART_REG16_EA(op, src, out, 21, 24);
	GET_PCWORD(dst);
	WORD_IMUL(res, dst, src);
	*out = (UINT16)res;
}

void
IMUL_GdEdId(void)
{
	UINT32 *out;
	UINT32 op;
	SINT64 res;
	SINT32 src, dst;

	PREPART_REG32_EA(op, src, out, 21, 24);
	GET_PCDWORD(dst);
	DWORD_IMUL(res, dst, src);
	*out = (UINT32)res;
}


/*
 * MUL
 */
void
MUL_ALEb(UINT32 op)
{
	UINT32 res, madr;
	UINT8 src, dst;

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
	CPU_AX = (UINT16)res;
}

void
MUL_AXEw(UINT32 op)
{
	UINT32 res, madr;
	UINT16 src, dst;

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
	CPU_AX = (UINT16)res;
	CPU_DX = (UINT16)(res >> 16);
}

void
MUL_EAXEd(UINT32 op)
{
	UINT32 res, madr;
	UINT32 src, dst;

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
IDIV_ALEb(UINT32 op)
{
	UINT32 madr;
	SINT16 tmp, r;
	SINT8 src;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(17);
		src = *(reg8_b20[op]);
	} else {
		CPU_WORKCLOCK(25);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	}
	tmp = (SINT16)CPU_AX;
	if (src != 0) {
		r = tmp / src;
		if (((r + 0x80) & 0xff00) == 0) {
			CPU_AL = (SINT8)r;
			CPU_AH = tmp % src;
			return;
		}
	}
	EXCEPTION(DE_EXCEPTION, 0);
}

void
IDIV_AXEw(UINT32 op)
{
	SINT32 tmp, r;
	UINT32 madr;
	SINT16 src;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(17);
		src = *(reg16_b20[op]);
	} else {
		CPU_WORKCLOCK(25);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	}
	tmp = (SINT32)(((UINT32)CPU_DX << 16) + (UINT32)CPU_AX);
	if (src != 0) {
		r = tmp / src;
		if (((r + 0x8000) & 0xffff0000) == 0) {
			CPU_AX = (SINT16)r;
			CPU_DX = tmp % src;
			return;
		}
	}
	EXCEPTION(DE_EXCEPTION, 0);
}

void
IDIV_EAXEd(UINT32 op)
{
	SINT64 tmp, r;
	SINT32 src;
	UINT32 madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(17);
		src = *(reg32_b20[op]);
	} else {
		CPU_WORKCLOCK(25);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	}
	tmp = (SINT64)(((UINT64)CPU_EDX << 32) + (SINT64)CPU_EAX);
	if (src != 0) {
		r = tmp / src;
		if (((r + SQWORD_CONST(0x80000000)) & QWORD_CONST(0xffffffff00000000)) == 0) {
			CPU_EAX = (SINT32)r;
			CPU_EDX = (SINT32)(tmp % src);
			return;
		}
	}
	EXCEPTION(DE_EXCEPTION, 0);
}


/*
 * DIV
 */
void
DIV_ALEb(UINT32 op)
{
	UINT32 madr;
	UINT16 tmp;
	UINT8 src;

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
		if (tmp < ((UINT16)src << 8)) {
			CPU_AL = tmp / src;
			CPU_AH = tmp % src;
			return;
		}
	}
	EXCEPTION(DE_EXCEPTION, 0);
}

void
DIV_AXEw(UINT32 op)
{
	UINT32 madr;
	UINT32 tmp;
	UINT16 src;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(17);
		src = *(reg16_b20[op]);
	} else {
		CPU_WORKCLOCK(25);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	}
	tmp = ((UINT32)CPU_DX << 16) + CPU_AX;
	if (src != 0) {
		if (tmp < ((UINT32)src << 16)) {
			CPU_AX = (UINT16)(tmp / src);
			CPU_DX = (UINT16)(tmp % src);
			return;
		}
	}
	EXCEPTION(DE_EXCEPTION, 0);
}

void
DIV_EAXEd(UINT32 op)
{
	UINT32 madr;
	UINT64 tmp;
	UINT32 src;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(17);
		src = *(reg32_b20[op]);
	} else {
		CPU_WORKCLOCK(25);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	}
	tmp = ((UINT64)CPU_EDX << 32) + CPU_EAX;
	if (src != 0) {
		if (tmp < ((UINT64)src << 32)) {
			CPU_EAX = (UINT32)(tmp / src);
			CPU_EDX = (UINT32)(tmp % src);
			return;
		}
	}
	EXCEPTION(DE_EXCEPTION, 0);
}


/*
 * INC
 */
void
INC_Eb(UINT32 op)
{
	UINT8 *out;
	UINT32 madr;
	UINT8 value;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		value = *out;
		BYTE_INC(value);
		*out = value;
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		value = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		BYTE_INC(value);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, value);
	}
}

void
INC_Ew(UINT32 op)
{
	UINT16 *out;
	UINT32 madr;
	UINT16 value;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		value = *out;
		WORD_INC(value);
		*out = value;
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		value = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_INC(value);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, value);
	}
}

void
INC_Ed(UINT32 op)
{
	UINT32 *out;
	UINT32 madr;
	UINT32 value;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(6);
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
DEC_Eb(UINT32 op)
{
	UINT8 *out;
	UINT32 madr;
	UINT8 value;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		value = *out;
		BYTE_DEC(value);
		*out = value;
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		value = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		BYTE_DEC(value);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, value);
	}
}

void
DEC_Ew(UINT32 op)
{
	UINT16 *out;
	UINT32 madr;
	UINT16 value;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		value = *out;
		WORD_DEC(value);
		*out = value;
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		value = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_DEC(value);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, value);
	}
}

void
DEC_Ed(UINT32 op)
{
	UINT32 *out;
	UINT32 madr;
	UINT32 value;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		value = *out;
		DWORD_DEC(value);
		*out = value;
	} else {
		CPU_WORKCLOCK(6);
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
NEG_Eb(UINT32 op)
{
	UINT8 *out;
	UINT32 src, dst, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		src = *out;
		BYTE_NEG(dst, src);
		*out = (UINT8)dst;
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		BYTE_NEG(dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)dst);
	}
}

void
NEG_Ew(UINT32 op)
{
	UINT16 *out;
	UINT32 src, dst, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		src = *out;
		WORD_NEG(dst, src);
		*out = (UINT16)dst;
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_NEG(dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)dst);
	}
}

void
NEG_Ed(UINT32 op)
{
	UINT32 *out;
	UINT32 src, dst, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		src = *out;
		DWORD_NEG(dst, src);
		*out = dst;
	} else {
		CPU_WORKCLOCK(6);
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
	UINT8 *out;
	UINT32 op, src, dst, res, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
	} else {
		CPU_WORKCLOCK(5);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	}
	BYTE_SUB(res, dst, src);
}

void
CMP_EwGw(void)
{
	UINT16 *out;
	UINT32 op, src, dst, res, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
	} else {
		CPU_WORKCLOCK(5);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	}
	WORD_SUB(res, dst, src);
}

void
CMP_EdGd(void)
{
	UINT32 *out;
	UINT32 op, src, dst, res, madr;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		dst = *out;
	} else {
		CPU_WORKCLOCK(5);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	}
	DWORD_SUB(res, dst, src);
}

void
CMP_GbEb(void)
{
	UINT8 *out;
	UINT32 op, src, dst, res;

	PREPART_REG8_EA(op, src, out, 2, 5);
	dst = *out;
	BYTE_SUB(res, dst, src);
}

void
CMP_GwEw(void)
{
	UINT16 *out;
	UINT32 op, src, dst, res;

	PREPART_REG16_EA(op, src, out, 2, 5);
	dst = *out;
	WORD_SUB(res, dst, src);
}

void
CMP_GdEd(void)
{
	UINT32 *out;
	UINT32 op, src, dst, res;

	PREPART_REG32_EA(op, src, out, 2, 5);
	dst = *out;
	DWORD_SUB(res, dst, src);
}

void
CMP_ALIb(void)
{
	UINT32 src, dst, res;

	CPU_WORKCLOCK(2);
	GET_PCBYTE(src);
	dst = CPU_AL;
	BYTE_SUB(res, dst, src);
}

void
CMP_AXIw(void)
{
	UINT32 src, dst, res;

	CPU_WORKCLOCK(2);
	GET_PCWORD(src);
	dst = CPU_AX;
	WORD_SUB(res, dst, src);
}

void
CMP_EAXId(void)
{
	UINT32 src, dst, res;

	CPU_WORKCLOCK(2);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	DWORD_SUB(res, dst, src);
}

void
CMP_EbIb(UINT8 *regp, UINT32 src)
{
	UINT32 dst, res;

	dst = *regp;
	BYTE_SUB(res, dst, src);
}

void
CMP_EbIb_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst, res;

	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_SUB(res, dst, src);
}

void
CMP_EwIx(UINT16 *regp, UINT32 src)
{
	UINT32 dst, res;

	dst = *regp;
	WORD_SUB(res, dst, src);
}

void
CMP_EwIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst, res;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_SUB(res, dst, src);
}

void
CMP_EdIx(UINT32 *regp, UINT32 src)
{
	UINT32 dst, res;

	dst = *regp;
	DWORD_SUB(res, dst, src);
}

void
CMP_EdIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst, res;

	dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_SUB(res, dst, src);
}

/*	$Id: logic_arith.c,v 1.5 2004/02/20 16:09:05 monaka Exp $	*/

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

#include "logic_arith.h"


/*
 * AND
 */
void
AND_EbGb(void)
{
	UINT8 *out;
	UINT32 op, src, dst, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
		AND_BYTE(dst, src);
		*out = (UINT8)dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		AND_BYTE(dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)dst);
	}
}

void
AND_EwGw(void)
{
	UINT16 *out;
	UINT32 op, src, dst, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
		AND_WORD(dst, src);
		*out = (UINT16)dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		AND_WORD(dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)dst);
	}
}

void
AND_EdGd(void)
{
	UINT32 *out;
	UINT32 op, src, dst, madr;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		dst = *out;
		AND_DWORD(dst, src);
		*out = dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		AND_DWORD(dst, src);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
	}
}

void
AND_GbEb(void)
{
	UINT8 *out;
	UINT32 op, src, dst;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	AND_BYTE(dst, src);
	*out = (UINT8)dst;
}

void
AND_GwEw(void)
{
	UINT16 *out;
	UINT32 op, src, dst;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	AND_WORD(dst, src);
	*out = (UINT16)dst;
}

void
AND_GdEd(void)
{
	UINT32 *out;
	UINT32 op, src, dst;

	PREPART_REG32_EA(op, src, out, 2, 7);
	dst = *out;
	AND_DWORD(dst, src);
	*out = dst;
}

void
AND_ALIb(void)
{
	UINT32 src, dst;

	CPU_WORKCLOCK(3);
	GET_PCBYTE(src);
	dst = CPU_AL;
	AND_BYTE(dst, src);
	CPU_AL = (UINT8)dst;
}

void
AND_AXIw(void)
{
	UINT32 src, dst;

	CPU_WORKCLOCK(3);
	GET_PCWORD(src);
	dst = CPU_AX;
	AND_WORD(dst, src);
	CPU_AX = (UINT16)dst;
}

void
AND_EAXId(void)
{
	UINT32 src, dst;

	CPU_WORKCLOCK(3);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	AND_DWORD(dst, src);
	CPU_EAX = dst;
}

void
AND_EbIb(UINT8 *regp, UINT32 src)
{
	UINT32 dst;

	dst = *regp;
	AND_BYTE(dst, src);
	*regp = (UINT8)dst;
}

void
AND_EbIb_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst;

	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	AND_BYTE(dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)dst);
}

void
AND_EwIx(UINT16 *regp, UINT32 src)
{
	UINT32 dst;

	dst = *regp;
	AND_WORD(dst, src);
	*regp = (UINT16)dst;
}

void
AND_EwIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	AND_WORD(dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)dst);
}

void
AND_EdIx(UINT32 *regp, UINT32 src)
{
	UINT32 dst;

	dst = *regp;
	AND_DWORD(dst, src);
	*regp = dst;
}

void
AND_EdIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst;

	dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	AND_DWORD(dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/*
 * OR
 */
void
OR_EbGb(void)
{
	UINT8 *out;
	UINT32 op, src, dst, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
		OR_BYTE(dst, src);
		*out = (UINT8)dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		OR_BYTE(dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)dst);
	}
}

void
OR_EwGw(void)
{
	UINT16 *out;
	UINT32 op, src, dst, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
		OR_WORD(dst, src);
		*out = (UINT16)dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		OR_WORD(dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)dst);
	}
}

void
OR_EdGd(void)
{
	UINT32 *out;
	UINT32 op, src, dst, madr;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		dst = *out;
		OR_DWORD(dst, src);
		*out = dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		OR_DWORD(dst, src);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
	}
}

void
OR_GbEb(void)
{
	UINT8 *out;
	UINT32 op, src, dst;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	OR_BYTE(dst, src);
	*out = (UINT8)dst;
}

void
OR_GwEw(void)
{
	UINT16 *out;
	UINT32 op, src, dst;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	OR_WORD(dst, src);
	*out = (UINT16)dst;
}

void
OR_GdEd(void)
{
	UINT32 *out;
	UINT32 op, src, dst;

	PREPART_REG32_EA(op, src, out, 2, 7);
	dst = *out;
	OR_DWORD(dst, src);
	*out = dst;
}

void
OR_ALIb(void)
{
	UINT32 src, dst;

	CPU_WORKCLOCK(3);
	GET_PCBYTE(src);
	dst = CPU_AL;
	OR_BYTE(dst, src);
	CPU_AL = (UINT8)dst;
}

void
OR_AXIw(void)
{
	UINT32 src, dst;

	CPU_WORKCLOCK(3);
	GET_PCWORD(src);
	dst = CPU_AX;
	OR_WORD(dst, src);
	CPU_AX = (UINT16)dst;
}

void
OR_EAXId(void)
{
	UINT32 src, dst;

	CPU_WORKCLOCK(3);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	OR_DWORD(dst, src);
	CPU_EAX = dst;
}

void
OR_EbIb(UINT8 *regp, UINT32 src)
{
	UINT32 dst;

	dst = *regp;
	OR_BYTE(dst, src);
	*regp = (UINT8)dst;
}

void
OR_EbIb_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst;

	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	OR_BYTE(dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)dst);
}

void
OR_EwIx(UINT16 *regp, UINT32 src)
{
	UINT32 dst;

	dst = *regp;
	OR_WORD(dst, src);
	*regp = (UINT16)dst;
}

void
OR_EwIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	OR_WORD(dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)dst);
}

void
OR_EdIx(UINT32 *regp, UINT32 src)
{
	UINT32 dst;

	dst = *regp;
	OR_DWORD(dst, src);
	*regp = dst;
}

void
OR_EdIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst;

	dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	OR_DWORD(dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/*
 * XOR
 */
void
XOR_EbGb(void)
{
	UINT8 *out;
	UINT32 op, src, dst, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
		BYTE_XOR(dst, src);
		*out = (UINT8)dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		BYTE_XOR(dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)dst);
	}
}

void
XOR_EwGw(void)
{
	UINT16 *out;
	UINT32 op, src, dst, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
		WORD_XOR(dst, src);
		*out = (UINT16)dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_XOR(dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)dst);
	}
}

void
XOR_EdGd(void)
{
	UINT32 *out;
	UINT32 op, src, dst, madr;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		dst = *out;
		DWORD_XOR(dst, src);
		*out = dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		DWORD_XOR(dst, src);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
	}
}

void
XOR_GbEb(void)
{
	UINT8 *out;
	UINT32 op, src, dst;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	BYTE_XOR(dst, src);
	*out = (UINT8)dst;
}

void
XOR_GwEw(void)
{
	UINT16 *out;
	UINT32 op, src, dst;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	WORD_XOR(dst, src);
	*out = (UINT16)dst;
}

void
XOR_GdEd(void)
{
	UINT32 *out;
	UINT32 op, src, dst;

	PREPART_REG32_EA(op, src, out, 2, 7);
	dst = *out;
	DWORD_XOR(dst, src);
	*out = dst;
}

void
XOR_ALIb(void)
{
	UINT32 src, dst;

	CPU_WORKCLOCK(3);
	GET_PCBYTE(src);
	dst = CPU_AL;
	BYTE_XOR(dst, src);
	CPU_AL = (UINT8)dst;
}

void
XOR_AXIw(void)
{
	UINT32 src, dst;

	CPU_WORKCLOCK(3);
	GET_PCWORD(src);
	dst = CPU_AX;
	WORD_XOR(dst, src);
	CPU_AX = (UINT16)dst;
}

void
XOR_EAXId(void)
{
	UINT32 src, dst;

	CPU_WORKCLOCK(3);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	DWORD_XOR(dst, src);
	CPU_EAX = dst;
}

void
XOR_EbIb(UINT8 *regp, UINT32 src)
{
	UINT32 dst;

	dst = *regp;
	BYTE_XOR(dst, src);
	*regp = (UINT8)dst;
}

void
XOR_EbIb_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst;

	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_XOR(dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)dst);
}

void
XOR_EwIx(UINT16 *regp, UINT32 src)
{
	UINT32 dst;

	dst = *regp;
	WORD_XOR(dst, src);
	*regp = (UINT16)dst;
}

void
XOR_EwIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_XOR(dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)dst);
}

void
XOR_EdIx(UINT32 *regp, UINT32 src)
{
	UINT32 dst;

	dst = *regp;
	DWORD_XOR(dst, src);
	*regp = dst;
}

void
XOR_EdIx_ext(UINT32 madr, UINT32 src)
{
	UINT32 dst;

	dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_XOR(dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/*
 * NOT
 */
void
NOT_Eb(UINT32 op)
{
	UINT32 dst, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) ^= 0xff;
	} else {
		CPU_WORKCLOCK(5);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		dst = dst ^ 0xff;
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (UINT8)dst);
	}
}

void
NOT_Ew(UINT32 op)
{
	UINT32 dst, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg16_b20[op]) ^= 0xffff;
	} else {
		CPU_WORKCLOCK(5);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		dst = dst ^ 0xffff;
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (UINT16)dst);
	}
}

void
NOT_Ed(UINT32 op)
{
	UINT32 dst, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg32_b20[op]) ^= 0xffffffff;
	} else {
		CPU_WORKCLOCK(5);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		dst = dst ^ 0xffffffff;
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
	}
}

/*	$Id: logic_arith.c,v 1.1 2003/12/08 00:55:32 yui Exp $	*/

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
	BYTE *out;
	DWORD op, src, dst, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
		ANDBYTE(dst, src);
		*out = (BYTE)dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		ANDBYTE(dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)dst);
	}
}

void
AND_EwGw(void)
{
	WORD *out;
	DWORD op, src, dst, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
		ANDWORD(dst, src);
		*out = (WORD)dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		ANDWORD(dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)dst);
	}
}

void
AND_EdGd(void)
{
	DWORD *out;
	DWORD op, src, dst, madr;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		dst = *out;
		ANDDWORD(dst, src);
		*out = dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		ANDDWORD(dst, src);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
	}
}

void
AND_GbEb(void)
{
	BYTE *out;
	DWORD op, src, dst;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	ANDBYTE(dst, src);
	*out = (BYTE)dst;
}

void
AND_GwEw(void)
{
	WORD *out;
	DWORD op, src, dst;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	ANDWORD(dst, src);
	*out = (WORD)dst;
}

void
AND_GdEd(void)
{
	DWORD *out;
	DWORD op, src, dst;

	PREPART_REG32_EA(op, src, out, 2, 7);
	dst = *out;
	ANDDWORD(dst, src);
	*out = dst;
}

void
AND_ALIb(void)
{
	DWORD src, dst;

	CPU_WORKCLOCK(3);
	GET_PCBYTE(src);
	dst = CPU_AL;
	ANDBYTE(dst, src);
	CPU_AL = (BYTE)dst;
}

void
AND_AXIw(void)
{
	DWORD src, dst;

	CPU_WORKCLOCK(3);
	GET_PCWORD(src);
	dst = CPU_AX;
	ANDWORD(dst, src);
	CPU_AX = (WORD)dst;
}

void
AND_EAXId(void)
{
	DWORD src, dst;

	CPU_WORKCLOCK(3);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	ANDDWORD(dst, src);
	CPU_EAX = dst;
}

void
AND_EbIb(BYTE *p)
{
	DWORD src, dst;

	GET_PCBYTE(src);
	dst = *p;
	ANDBYTE(dst, src);
	*p = (BYTE)dst;
}

void
AND_EbIb_ext(DWORD madr)
{
	DWORD src, dst;

	GET_PCBYTE(src);
	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	ANDBYTE(dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)dst);
}

void
AND_EwIx(WORD *p, DWORD src)
{
	DWORD dst;

	dst = *p;
	ANDWORD(dst, src);
	*p = (WORD)dst;
}

void
AND_EwIx_ext(DWORD madr, DWORD src)
{
	DWORD dst;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	ANDWORD(dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)dst);
}

void
AND_EdIx(DWORD *p, DWORD src)
{
	DWORD dst;

	dst = *p;
	ANDDWORD(dst, src);
	*p = dst;
}

void
AND_EdIx_ext(DWORD madr, DWORD src)
{
	DWORD dst;

	dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	ANDDWORD(dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/*
 * OR
 */
void
OR_EbGb(void)
{
	BYTE *out;
	DWORD op, src, dst, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
		ORBYTE(dst, src);
		*out = (BYTE)dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		ORBYTE(dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)dst);
	}
}

void
OR_EwGw(void)
{
	WORD *out;
	DWORD op, src, dst, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
		ORWORD(dst, src);
		*out = (WORD)dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		ORWORD(dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)dst);
	}
}

void
OR_EdGd(void)
{
	DWORD *out;
	DWORD op, src, dst, madr;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		dst = *out;
		ORDWORD(dst, src);
		*out = dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		ORDWORD(dst, src);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
	}
}

void
OR_GbEb(void)
{
	BYTE *out;
	DWORD op, src, dst;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	ORBYTE(dst, src);
	*out = (BYTE)dst;
}

void
OR_GwEw(void)
{
	WORD *out;
	DWORD op, src, dst;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	ORWORD(dst, src);
	*out = (WORD)dst;
}

void
OR_GdEd(void)
{
	DWORD *out;
	DWORD op, src, dst;

	PREPART_REG32_EA(op, src, out, 2, 7);
	dst = *out;
	ORDWORD(dst, src);
	*out = dst;
}

void
OR_ALIb(void)
{
	DWORD src, dst;

	CPU_WORKCLOCK(3);
	GET_PCBYTE(src);
	dst = CPU_AL;
	ORBYTE(dst, src);
	CPU_AL = (BYTE)dst;
}

void
OR_AXIw(void)
{
	DWORD src, dst;

	CPU_WORKCLOCK(3);
	GET_PCWORD(src);
	dst = CPU_AX;
	ORWORD(dst, src);
	CPU_AX = (WORD)dst;
}

void
OR_EAXId(void)
{
	DWORD src, dst;

	CPU_WORKCLOCK(3);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	ORDWORD(dst, src);
	CPU_EAX = dst;
}

void
OR_EbIb(BYTE *p)
{
	DWORD src, dst;

	GET_PCBYTE(src);
	dst = *p;
	ORBYTE(dst, src);
	*p = (BYTE)dst;
}

void
OR_EbIb_ext(DWORD madr)
{
	DWORD src, dst;

	GET_PCBYTE(src);
	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	ORBYTE(dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)dst);
}

void
OR_EwIx(WORD *p, DWORD src)
{
	DWORD dst;

	dst = *p;
	ORWORD(dst, src);
	*p = (WORD)dst;
}

void
OR_EwIx_ext(DWORD madr, DWORD src)
{
	DWORD dst;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	ORWORD(dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)dst);
}

void
OR_EdIx(DWORD *p, DWORD src)
{
	DWORD dst;

	dst = *p;
	ORDWORD(dst, src);
	*p = dst;
}

void
OR_EdIx_ext(DWORD madr, DWORD src)
{
	DWORD dst;

	dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	ORDWORD(dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/*
 * XOR
 */
void
XOR_EbGb(void)
{
	BYTE *out;
	DWORD op, src, dst, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg8_b20[op];
		dst = *out;
		BYTE_XOR(dst, src);
		*out = (BYTE)dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		BYTE_XOR(dst, src);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)dst);
	}
}

void
XOR_EwGw(void)
{
	WORD *out;
	DWORD op, src, dst, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		dst = *out;
		WORD_XOR(dst, src);
		*out = (WORD)dst;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_XOR(dst, src);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)dst);
	}
}

void
XOR_EdGd(void)
{
	DWORD *out;
	DWORD op, src, dst, madr;

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
	BYTE *out;
	DWORD op, src, dst;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	BYTE_XOR(dst, src);
	*out = (BYTE)dst;
}

void
XOR_GwEw(void)
{
	WORD *out;
	DWORD op, src, dst;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	WORD_XOR(dst, src);
	*out = (WORD)dst;
}

void
XOR_GdEd(void)
{
	DWORD *out;
	DWORD op, src, dst;

	PREPART_REG32_EA(op, src, out, 2, 7);
	dst = *out;
	DWORD_XOR(dst, src);
	*out = dst;
}

void
XOR_ALIb(void)
{
	DWORD src, dst;

	CPU_WORKCLOCK(3);
	GET_PCBYTE(src);
	dst = CPU_AL;
	BYTE_XOR(dst, src);
	CPU_AL = (BYTE)dst;
}

void
XOR_AXIw(void)
{
	DWORD src, dst;

	CPU_WORKCLOCK(3);
	GET_PCWORD(src);
	dst = CPU_AX;
	WORD_XOR(dst, src);
	CPU_AX = (WORD)dst;
}

void
XOR_EAXId(void)
{
	DWORD src, dst;

	CPU_WORKCLOCK(3);
	GET_PCDWORD(src);
	dst = CPU_EAX;
	DWORD_XOR(dst, src);
	CPU_EAX = dst;
}

void
XOR_EbIb(BYTE *p)
{
	DWORD src, dst;

	GET_PCBYTE(src);
	dst = *p;
	BYTE_XOR(dst, src);
	*p = (BYTE)dst;
}

void
XOR_EbIb_ext(DWORD madr)
{
	DWORD src, dst;

	GET_PCBYTE(src);
	dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_XOR(dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)dst);
}

void
XOR_EwIx(WORD *p, DWORD src)
{
	DWORD dst;

	dst = *p;
	WORD_XOR(dst, src);
	*p = (WORD)dst;
}

void
XOR_EwIx_ext(DWORD madr, DWORD src)
{
	DWORD dst;

	dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_XOR(dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)dst);
}

void
XOR_EdIx(DWORD *p, DWORD src)
{
	DWORD dst;

	dst = *p;
	DWORD_XOR(dst, src);
	*p = dst;
}

void
XOR_EdIx_ext(DWORD madr, DWORD src)
{
	DWORD dst;

	dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_XOR(dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/*
 * NOT
 */
void
NOT_Eb(DWORD op)
{
	DWORD dst, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) ^= 0xff;
	} else {
		CPU_WORKCLOCK(5);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
		dst = dst ^ 0xff;
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, (BYTE)dst);
	}
}

void
NOT_Ew(DWORD op)
{
	DWORD dst, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg16_b20[op]) ^= 0xffff;
	} else {
		CPU_WORKCLOCK(5);
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		dst = dst ^ 0xffff;
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)dst);
	}
}

void
NOT_Ed(DWORD op)
{
	DWORD dst, madr;

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

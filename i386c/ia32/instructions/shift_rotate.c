/*	$Id: shift_rotate.c,v 1.2 2004/01/14 16:13:46 monaka Exp $	*/

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

#include "shift_rotate.h"
#include "shift_rotate.mcr"


/*
 * shift
 */
/*
 * SAR
 */
void
SAR_Eb(BYTE *out)
{
	DWORD src, dst;

	src = *out;
	BYTE_SAR1(dst, src);
	*out = (BYTE)dst;
}

void
SAR_Eb_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_SAR1(dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
SAR_Ew(WORD *out)
{
	DWORD src, dst;

	src = *out;
	WORD_SAR1(dst, src);
	*out = (WORD)dst;
}

void
SAR_Ew_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_SAR1(dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
SAR_Ed(DWORD *out)
{
	DWORD src, dst;

	src = *out;
	DWORD_SAR1(dst, src);
	*out = dst;
}

void
SAR_Ed_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_SAR1(dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/* ExCL, ExIb */
void
SAR_EbCL(BYTE *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	BYTE_SARCL(dst, src, cl);
	*out = dst;
}

void
SAR_EbCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_SARCL(dst, src, cl);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
SAR_EwCL(WORD *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	WORD_SARCL(dst, src, cl);
	*out = dst;
}

void
SAR_EwCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_SARCL(dst, src, cl);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
SAR_EdCL(DWORD *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	DWORD_SARCL(dst, src, cl);
	*out = dst;
}

void
SAR_EdCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_SARCL(dst, src, cl);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/*
 * SHR
 */
void
SHR_Eb(BYTE *out)
{
	DWORD src, dst;

	src = *out;
	BYTE_SHR1(dst, src);
	*out = (BYTE)dst;
}

void
SHR_Eb_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_SHR1(dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
SHR_Ew(WORD *out)
{
	DWORD src, dst;

	src = *out;
	WORD_SHR1(dst, src);
	*out = (WORD)dst;
}

void
SHR_Ew_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_SHR1(dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
SHR_Ed(DWORD *out)
{
	DWORD src, dst;

	src = *out;
	DWORD_SHR1(dst, src);
	*out = dst;
}

void
SHR_Ed_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_SHR1(dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/* ExCL, ExIb */
void
SHR_EbCL(BYTE *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	BYTE_SHRCL(dst, src, cl);
	*out = dst;
}

void
SHR_EbCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_SHRCL(dst, src, cl);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
SHR_EwCL(WORD *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	WORD_SHRCL(dst, src, cl);
	*out = dst;
}

void
SHR_EwCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_SHRCL(dst, src, cl);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
SHR_EdCL(DWORD *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	DWORD_SHRCL(dst, src, cl);
	*out = dst;
}

void
SHR_EdCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_SHRCL(dst, src, cl);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/*
 * SHL/SAL
 */
void
SHL_Eb(BYTE *out)
{
	DWORD src, dst;

	src = *out;
	BYTE_SHL1(dst, src);
	*out = (BYTE)dst;
}

void
SHL_Eb_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_SHL1(dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
SHL_Ew(WORD *out)
{
	DWORD src, dst;

	src = *out;
	WORD_SHL1(dst, src);
	*out = (WORD)dst;
}

void
SHL_Ew_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_SHL1(dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
SHL_Ed(DWORD *out)
{
	DWORD src, dst;

	src = *out;
	DWORD_SHL1(dst, src);
	*out = dst;
}

void
SHL_Ed_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_SHL1(dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/* ExCL, ExIb */
void
SHL_EbCL(BYTE *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	BYTE_SHLCL(dst, src, cl);
	*out = dst;
}

void
SHL_EbCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_SHLCL(dst, src, cl);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
SHL_EwCL(WORD *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	WORD_SHLCL(dst, src, cl);
	*out = dst;
}

void
SHL_EwCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_SHLCL(dst, src, cl);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
SHL_EdCL(DWORD *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	DWORD_SHLCL(dst, src, cl);
	*out = dst;
}

void
SHL_EdCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_SHLCL(dst, src, cl);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/*
 * SHRD
 */
void
SHRD_EwGwIb(void)
{
	WORD *out;
	DWORD op, src, dst, madr;
	BYTE cl;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		out = reg16_b20[op];
		GET_PCBYTE(cl);
		dst = *out;
		WORD_SHRD(dst, src, cl);
		*out = dst;
	} else {
		madr = calc_ea_dst(op);
		GET_PCBYTE(cl);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_SHRD(dst, src, cl);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
	}
}

void
SHRD_EdGdIb(void)
{
	DWORD *out;
	DWORD op, src, dst, madr;
	BYTE cl;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		out = reg32_b20[op];
		GET_PCBYTE(cl);
		dst = *out;
		DWORD_SHRD(dst, src, cl);
		*out = dst;
	} else {
		madr = calc_ea_dst(op);
		GET_PCBYTE(cl);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		DWORD_SHRD(dst, src, cl);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
	}
}

void
SHRD_EwGwCL(void)
{
	WORD *out;
	DWORD op, src, dst, madr;
	BYTE cl;

	PREPART_EA_REG16(op, src);
	cl = CPU_CL;
	if (op >= 0xc0) {
		out = reg16_b20[op];
		dst = *out;
		WORD_SHRD(dst, src, cl);
		*out = dst;
	} else {
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_SHRD(dst, src, cl);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
	}
}

void
SHRD_EdGdCL(void)
{
	DWORD *out;
	DWORD op, src, dst, madr;
	BYTE cl;

	PREPART_EA_REG32(op, src);
	cl = CPU_CL;
	if (op >= 0xc0) {
		out = reg32_b20[op];
		dst = *out;
		DWORD_SHRD(dst, src, cl);
		*out = dst;
	} else {
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		DWORD_SHRD(dst, src, cl);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
	}
}

/*
 * SHLD
 */
void
SHLD_EwGwIb(void)
{
	WORD *out;
	DWORD op, src, dst, madr;
	BYTE cl;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		out = reg16_b20[op];
		GET_PCBYTE(cl);
		dst = *out;
		WORD_SHLD(dst, src, cl);
		*out = dst;
	} else {
		madr = calc_ea_dst(op);
		GET_PCBYTE(cl);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_SHLD(dst, src, cl);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
	}
}

void
SHLD_EdGdIb(void)
{
	DWORD *out;
	DWORD op, src, dst, madr;
	BYTE cl;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		out = reg32_b20[op];
		GET_PCBYTE(cl);
		dst = *out;
		DWORD_SHLD(dst, src, cl);
		*out = dst;
	} else {
		madr = calc_ea_dst(op);
		GET_PCBYTE(cl);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		DWORD_SHLD(dst, src, cl);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
	}
}

void
SHLD_EwGwCL(void)
{
	WORD *out;
	DWORD op, src, dst, madr;
	BYTE cl;

	PREPART_EA_REG16(op, src);
	cl = CPU_CL;
	if (op >= 0xc0) {
		out = reg16_b20[op];
		dst = *out;
		WORD_SHLD(dst, src, cl);
		*out = dst;
	} else {
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		WORD_SHLD(dst, src, cl);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
	}
}

void
SHLD_EdGdCL(void)
{
	DWORD *out;
	DWORD op, src, dst, madr;
	BYTE cl;

	PREPART_EA_REG32(op, src);
	cl = CPU_CL;
	if (op >= 0xc0) {
		out = reg32_b20[op];
		dst = *out;
		DWORD_SHLD(dst, src, cl);
		*out = dst;
	} else {
		madr = calc_ea_dst(op);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		DWORD_SHLD(dst, src, cl);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
	}
}

/*
 * rotate
 */
/*
 * ROR
 */
void
ROR_Eb(BYTE *out)
{
	DWORD src, dst;

	src = *out;
	BYTE_ROR1(dst, src);
	*out = (BYTE)dst;
}

void
ROR_Eb_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_ROR1(dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
ROR_Ew(WORD *out)
{
	DWORD src, dst;

	src = *out;
	WORD_ROR1(dst, src);
	*out = (WORD)dst;
}

void
ROR_Ew_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_ROR1(dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
ROR_Ed(DWORD *out)
{
	DWORD src, dst;

	src = *out;
	DWORD_ROR1(dst, src);
	*out = dst;
}

void
ROR_Ed_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_ROR1(dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/* ExCL, ExIb */
void
ROR_EbCL(BYTE *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	BYTE_RORCL(dst, src, cl);
	*out = dst;
}

void
ROR_EbCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_RORCL(dst, src, cl);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
ROR_EwCL(WORD *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	WORD_RORCL(dst, src, cl);
	*out = dst;
}

void
ROR_EwCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_RORCL(dst, src, cl);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
ROR_EdCL(DWORD *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	DWORD_RORCL(dst, src, cl);
	*out = dst;
}

void
ROR_EdCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_RORCL(dst, src, cl);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/*
 * ROL
 */
void
ROL_Eb(BYTE *out)
{
	DWORD src, dst;

	src = *out;
	BYTE_ROL1(dst, src);
	*out = (BYTE)dst;
}

void
ROL_Eb_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_ROL1(dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
ROL_Ew(WORD *out)
{
	DWORD src, dst;

	src = *out;
	WORD_ROL1(dst, src);
	*out = (WORD)dst;
}

void
ROL_Ew_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_ROL1(dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
ROL_Ed(DWORD *out)
{
	DWORD src, dst;

	src = *out;
	DWORD_ROL1(dst, src);
	*out = dst;
}

void
ROL_Ed_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_ROL1(dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/* ExCL, ExIb */
void
ROL_EbCL(BYTE *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	BYTE_ROLCL(dst, src, cl);
	*out = dst;
}

void
ROL_EbCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_ROLCL(dst, src, cl);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
ROL_EwCL(WORD *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	WORD_ROLCL(dst, src, cl);
	*out = dst;
}

void
ROL_EwCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_ROLCL(dst, src, cl);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
ROL_EdCL(DWORD *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	DWORD_ROLCL(dst, src, cl);
	*out = dst;
}

void
ROL_EdCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_ROLCL(dst, src, cl);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/*
 * RCR
 */
void
RCR_Eb(BYTE *out)
{
	DWORD src, dst;

	src = *out;
	BYTE_RCR1(dst, src);
	*out = (BYTE)dst;
}

void
RCR_Eb_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_RCR1(dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
RCR_Ew(WORD *out)
{
	DWORD src, dst;

	src = *out;
	WORD_RCR1(dst, src);
	*out = (WORD)dst;
}

void
RCR_Ew_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_RCR1(dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
RCR_Ed(DWORD *out)
{
	DWORD src, dst;

	src = *out;
	DWORD_RCR1(dst, src);
	*out = dst;
}

void
RCR_Ed_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_RCR1(dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/* ExCL, ExIb */
void
RCR_EbCL(BYTE *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	BYTE_RCRCL(dst, src, cl);
	*out = dst;
}

void
RCR_EbCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_RCRCL(dst, src, cl);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
RCR_EwCL(WORD *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	WORD_RCRCL(dst, src, cl);
	*out = dst;
}

void
RCR_EwCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_RCRCL(dst, src, cl);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
RCR_EdCL(DWORD *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	DWORD_RCRCL(dst, src, cl);
	*out = dst;
}

void
RCR_EdCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_RCRCL(dst, src, cl);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/*
 * RCL
 */
void
RCL_Eb(BYTE *out)
{
	DWORD src, dst;

	src = *out;
	BYTE_RCL1(dst, src);
	*out = (BYTE)dst;
}

void
RCL_Eb_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_RCL1(dst, src);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
RCL_Ew(WORD *out)
{
	DWORD src, dst;

	src = *out;
	WORD_RCL1(dst, src);
	*out = (WORD)dst;
}

void
RCL_Ew_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_RCL1(dst, src);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
RCL_Ed(DWORD *out)
{
	DWORD src, dst;

	src = *out;
	DWORD_RCL1(dst, src);
	*out = dst;
}

void
RCL_Ed_ext(DWORD madr)
{
	DWORD src, dst;

	src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_RCL1(dst, src);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

/* ExCL, ExIb */
void
RCL_EbCL(BYTE *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	BYTE_RCLCL(dst, src, cl);
	*out = dst;
}

void
RCL_EbCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	BYTE_RCLCL(dst, src, cl);
	cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
RCL_EwCL(WORD *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	WORD_RCLCL(dst, src, cl);
	*out = dst;
}

void
RCL_EwCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	WORD_RCLCL(dst, src, cl);
	cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
}

void
RCL_EdCL(DWORD *out, BYTE cl)
{
	DWORD src, dst;

	src = *out;
	DWORD_RCLCL(dst, src, cl);
	*out = dst;
}

void
RCL_EdCL_ext(DWORD madr, BYTE cl)
{
	DWORD src, dst;

	src = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	DWORD_RCLCL(dst, src, cl);
	cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr, dst);
}

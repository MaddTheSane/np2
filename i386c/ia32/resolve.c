/*	$Id: resolve.c,v 1.2 2003/12/22 18:00:31 monaka Exp $	*/

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


static DWORD (*calc_ea_dst_tbl[0x100])(void);
static DWORD (*calc_ea32_dst_tbl[0x100])(void);


DWORD
calc_ea_dst(DWORD op)
{

	__ASSERT(op < 0x100);

	if (!CPU_INST_AS32)
		return ((*calc_ea_dst_tbl[op])() & 0xffff);
	return (*calc_ea32_dst_tbl[op])();
}


/*
 * common
 */
DWORD
ea_nop(void)
{

	ia32_panic("ea_nop");
	return 0;
}


/*
 * 16bit
 */

/*
 * ea_dest
 */
static DWORD
ea_bx_si(void)
{

	PROFILE_INC_EA16(0);

	CPU_INST_SEGREG_INDEX = DS_FIX;
	return (CPU_BX + CPU_SI);
}

static DWORD
ea_bx_si_disp8(void)
{
	SDWORD adrs;

	PROFILE_INC_EA16(1);

	GET_PCBYTESD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return (adrs + CPU_BX + CPU_SI);
}

static DWORD
ea_bx_si_disp16(void)
{
	DWORD adrs;

	PROFILE_INC_EA16(2);

	GET_PCWORD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return (adrs + CPU_BX + CPU_SI);
}

static DWORD
ea_bx_di(void)
{

	PROFILE_INC_EA16(3);

	CPU_INST_SEGREG_INDEX = DS_FIX;
	return (CPU_BX + CPU_DI);
}

static DWORD
ea_bx_di_disp8(void)
{
	SDWORD adrs;

	PROFILE_INC_EA16(4);

	GET_PCBYTESD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return (adrs + CPU_BX + CPU_DI);
}

static DWORD
ea_bx_di_disp16(void)
{
	DWORD adrs;

	PROFILE_INC_EA16(5);

	GET_PCWORD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return (adrs + CPU_BX + CPU_DI);
}

static DWORD
ea_bp_si(void)
{

	PROFILE_INC_EA16(6);

	CPU_INST_SEGREG_INDEX = SS_FIX;
	return (CPU_BP + CPU_SI);
}

static DWORD
ea_bp_si_disp8(void)
{
	SDWORD adrs;

	PROFILE_INC_EA16(7);

	GET_PCBYTESD(adrs);
	CPU_INST_SEGREG_INDEX = SS_FIX;
	return (adrs + CPU_BP + CPU_SI);
}

static DWORD
ea_bp_si_disp16(void)
{
	DWORD adrs;

	PROFILE_INC_EA16(8);

	GET_PCWORD(adrs);
	CPU_INST_SEGREG_INDEX = SS_FIX;
	return (adrs + CPU_BP + CPU_SI);
}

static DWORD
ea_bp_di(void)
{

	PROFILE_INC_EA16(9);

	CPU_INST_SEGREG_INDEX = SS_FIX;
	return (CPU_BP + CPU_DI);
}

static DWORD
ea_bp_di_disp8(void)
{
	SDWORD adrs;

	PROFILE_INC_EA16(10);

	GET_PCBYTESD(adrs);
	CPU_INST_SEGREG_INDEX = SS_FIX;
	return (adrs + CPU_BP + CPU_DI);
}

static DWORD
ea_bp_di_disp16(void)
{
	DWORD adrs;

	PROFILE_INC_EA16(11);

	GET_PCWORD(adrs);
	CPU_INST_SEGREG_INDEX = SS_FIX;
	return (adrs + CPU_BP + CPU_DI);
}

static DWORD
ea_si(void)
{

	PROFILE_INC_EA16(12);

	CPU_INST_SEGREG_INDEX = DS_FIX;
	return CPU_SI;
}

static DWORD
ea_si_disp8(void)
{
	SDWORD adrs;

	PROFILE_INC_EA16(13);

	GET_PCBYTESD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return (adrs + CPU_SI);
}

static DWORD
ea_si_disp16(void)
{
	DWORD adrs;

	PROFILE_INC_EA16(14);

	GET_PCWORD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return (adrs + CPU_SI);
}

static DWORD
ea_di(void)
{

	PROFILE_INC_EA16(15);

	CPU_INST_SEGREG_INDEX = DS_FIX;
	return CPU_DI;
}

static DWORD
ea_di_disp8(void)
{
	SDWORD adrs;

	PROFILE_INC_EA16(16);

	GET_PCBYTESD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return (adrs + CPU_DI);
}

static DWORD
ea_di_disp16(void)
{
	DWORD adrs;

	PROFILE_INC_EA16(17);

	GET_PCWORD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return (adrs + CPU_DI);
}

static DWORD
ea_disp16(void)
{
	DWORD adrs;

	PROFILE_INC_EA16(18);

	GET_PCWORD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return adrs;
}

static DWORD
ea_bp_disp8(void)
{
	SDWORD adrs;

	PROFILE_INC_EA16(19);

	GET_PCBYTESD(adrs);
	CPU_INST_SEGREG_INDEX = SS_FIX;
	return (adrs + CPU_BP);
}

static DWORD
ea_bp_disp16(void)
{
	DWORD adrs;

	PROFILE_INC_EA16(20);

	GET_PCWORD(adrs);
	CPU_INST_SEGREG_INDEX = SS_FIX;
	return (adrs + CPU_BP);
}

static DWORD
ea_bx(void)
{

	PROFILE_INC_EA16(21);

	CPU_INST_SEGREG_INDEX = DS_FIX;
	return CPU_BX;
}

static DWORD
ea_bx_disp8(void)
{
	SDWORD adrs;

	PROFILE_INC_EA16(22);

	GET_PCBYTESD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return (adrs + CPU_BX);
}

static DWORD
ea_bx_disp16(void)
{
	DWORD adrs;

	PROFILE_INC_EA16(23);

	GET_PCWORD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return (adrs + CPU_BX);
}

static DWORD (*c_ea_dst_tbl[])(void) = {
	ea_bx_si,		ea_bx_di,
	ea_bp_si,		ea_bp_di,
	ea_si,			ea_di,
	ea_disp16,		ea_bx,
	ea_bx_si_disp8,		ea_bx_di_disp8,
	ea_bp_si_disp8,		ea_bp_di_disp8,
	ea_si_disp8,		ea_di_disp8,
	ea_bp_disp8,		ea_bx_disp8,
	ea_bx_si_disp16,	ea_bx_di_disp16,
	ea_bp_si_disp16,	ea_bp_di_disp16,
	ea_si_disp16,		ea_di_disp16,
	ea_bp_disp16,		ea_bx_disp16,
};


/*
 * 32bit
 */
/*
 * ea_dest 32
 */
static DWORD
ea32_eax(void)
{

	PROFILE_INC_EA32(0);

	CPU_INST_SEGREG_INDEX = DS_FIX;
	return CPU_EAX;
}

static DWORD
ea32_ecx(void)
{

	PROFILE_INC_EA32(1);

	CPU_INST_SEGREG_INDEX = DS_FIX;
	return CPU_ECX;
}

static DWORD
ea32_edx(void)
{

	PROFILE_INC_EA32(2);

	CPU_INST_SEGREG_INDEX = DS_FIX;
	return CPU_EDX;
}

static DWORD
ea32_ebx(void)
{

	PROFILE_INC_EA32(3);

	CPU_INST_SEGREG_INDEX = DS_FIX;
	return CPU_EBX;
}

static DWORD
ea32_sib(void)
{
	DWORD op, dst;
	DWORD base, idx, scale;

	PROFILE_INC_EA32(4);

	GET_PCBYTE(op);
	dst = 0;	/* compiler happy */

	base = op & 7;
	idx = (op >> 3) & 7;
	scale = (op >> 6) & 3;
	switch (base) {
	case 0: case 1: case 2: case 3: case 6: case 7:
		CPU_INST_SEGREG_INDEX = DS_FIX;
		dst = CPU_REGS_DWORD(base);
		break;

	case 4:
		CPU_INST_SEGREG_INDEX = SS_FIX;
		dst = CPU_ESP;
		break;

	case 5:
		CPU_INST_SEGREG_INDEX = DS_FIX;
		GET_PCDWORD(dst);
		break;
	}
	if (idx != 4)
		dst += CPU_REGS_DWORD(idx) << scale;
	PROFILE_INC_SIB0(op);
	return dst;
}

static DWORD
ea32_disp32(void)
{
	DWORD adrs;

	PROFILE_INC_EA32(5);

	GET_PCDWORD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return adrs;
}

static DWORD
ea32_esi(void)
{

	PROFILE_INC_EA32(6);

	CPU_INST_SEGREG_INDEX = DS_FIX;
	return CPU_ESI;
}

static DWORD
ea32_edi(void)
{

	PROFILE_INC_EA32(7);

	CPU_INST_SEGREG_INDEX = DS_FIX;
	return CPU_EDI;
}

static DWORD
ea32_eax_disp8(void)
{
	SDWORD adrs;

	PROFILE_INC_EA32(8);

	GET_PCBYTESD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return adrs + CPU_EAX;
}

static DWORD
ea32_ecx_disp8(void)
{
	SDWORD adrs;

	PROFILE_INC_EA32(9);

	GET_PCBYTESD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return adrs + CPU_ECX;
}

static DWORD
ea32_edx_disp8(void)
{
	SDWORD adrs;

	PROFILE_INC_EA32(10);

	GET_PCBYTESD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return adrs + CPU_EDX;
}

static DWORD
ea32_ebx_disp8(void)
{
	SDWORD adrs;

	PROFILE_INC_EA32(11);

	GET_PCBYTESD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return adrs + CPU_EBX;
}

static DWORD
ea32_sib_disp8(void)
{
	SDWORD adrs;
	DWORD op;
	DWORD base, idx, scale;

	PROFILE_INC_EA32(12);

	GET_PCBYTE(op);
	GET_PCBYTESD(adrs);

	base = op & 7;
	idx = (op >> 3) & 7;
	scale = (op >> 6) & 3;
	switch (base) {
	case 0: case 1: case 2: case 3: case 6: case 7:
		CPU_INST_SEGREG_INDEX = DS_FIX;
		break;

	case 4: case 5:
		CPU_INST_SEGREG_INDEX = SS_FIX;
		break;
	}
	if (idx != 4)
		adrs += CPU_REGS_DWORD(idx) << scale;
	PROFILE_INC_SIB1(op);
	return CPU_REGS_DWORD(base) + adrs;
}

static DWORD
ea32_ebp_disp8(void)
{
	SDWORD adrs;

	PROFILE_INC_EA32(13);

	GET_PCBYTESD(adrs);
	CPU_INST_SEGREG_INDEX = SS_FIX;
	return adrs + CPU_EBP;
}

static DWORD
ea32_esi_disp8(void)
{
	SDWORD adrs;

	PROFILE_INC_EA32(14);

	GET_PCBYTESD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return adrs + CPU_ESI;
}

static DWORD
ea32_edi_disp8(void)
{
	SDWORD adrs;

	PROFILE_INC_EA32(15);

	GET_PCBYTESD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return adrs + CPU_EDI;
}

static DWORD
ea32_eax_disp32(void)
{
	DWORD adrs;

	PROFILE_INC_EA32(16);

	GET_PCDWORD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return adrs + CPU_EAX;
}

static DWORD
ea32_ecx_disp32(void)
{
	DWORD adrs;

	PROFILE_INC_EA32(17);

	GET_PCDWORD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return adrs + CPU_ECX;
}

static DWORD
ea32_edx_disp32(void)
{
	DWORD adrs;

	PROFILE_INC_EA32(18);

	GET_PCDWORD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return adrs + CPU_EDX;
}

static DWORD
ea32_ebx_disp32(void)
{
	DWORD adrs;

	PROFILE_INC_EA32(19);

	GET_PCDWORD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return adrs + CPU_EBX;
}

static DWORD
ea32_sib_disp32(void)
{
	DWORD adrs;
	DWORD op;
	DWORD base, idx, scale;

	PROFILE_INC_EA32(20);

	GET_PCBYTE(op);
	GET_PCDWORD(adrs);

	base = op & 7;
	idx = (op >> 3) & 7;
	scale = (op >> 6) & 3;
	switch (base) {
	case 0: case 1: case 2: case 3: case 6: case 7:
		CPU_INST_SEGREG_INDEX = DS_FIX;
		break;

	case 4: case 5:
		CPU_INST_SEGREG_INDEX = SS_FIX;
		break;
	}
	if (idx != 4)
		adrs += CPU_REGS_DWORD(idx) << scale;
	PROFILE_INC_SIB2(op);
	return CPU_REGS_DWORD(base) + adrs;
}

static DWORD
ea32_ebp_disp32(void)
{
	DWORD adrs;

	PROFILE_INC_EA32(21);

	GET_PCDWORD(adrs);
	CPU_INST_SEGREG_INDEX = SS_FIX;
	return adrs + CPU_EBP;
}

static DWORD
ea32_esi_disp32(void)
{
	DWORD adrs;

	PROFILE_INC_EA32(22);

	GET_PCDWORD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return adrs + CPU_ESI;
}

static DWORD
ea32_edi_disp32(void)
{
	DWORD adrs;

	PROFILE_INC_EA32(23);

	GET_PCDWORD(adrs);
	CPU_INST_SEGREG_INDEX = DS_FIX;
	return adrs + CPU_EDI;
}

static DWORD (*c_ea32_dst_tbl[])(void) = {
	ea32_eax,		ea32_ecx, 
	ea32_edx,		ea32_ebx,
	ea32_sib,		ea32_disp32,
	ea32_esi,		ea32_edi,
	ea32_eax_disp8,		ea32_ecx_disp8, 
	ea32_edx_disp8,		ea32_ebx_disp8,
	ea32_sib_disp8,		ea32_ebp_disp8,
	ea32_esi_disp8,		ea32_edi_disp8,
	ea32_eax_disp32,	ea32_ecx_disp32, 
	ea32_edx_disp32,	ea32_ebx_disp32,
	ea32_sib_disp32,	ea32_ebp_disp32,
	ea32_esi_disp32,	ea32_edi_disp32,
};


/*
 * init table
 */
void
resolve_init(void)
{
	int i, pos;

	for (i = 0; i < 0xc0; ++i) {
		pos = ((i >> 3) & 0x18) + (i & 0x07);
		calc_ea_dst_tbl[i] = c_ea_dst_tbl[pos];
		calc_ea32_dst_tbl[i] = c_ea32_dst_tbl[pos];
	}
	for (; i < 0x100; ++i) {
		calc_ea_dst_tbl[i] = ea_nop;
		calc_ea32_dst_tbl[i] = ea_nop;
	}
}

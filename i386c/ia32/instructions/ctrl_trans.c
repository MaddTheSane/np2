/*	$Id: ctrl_trans.c,v 1.5 2004/01/29 00:27:29 yui Exp $	*/

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
#include "ctrlxfer.h"

#include "ctrl_trans.h"


/*
 * JMP
 */
void
JMP_Jb(void)
{
	DWORD ip;

	CPU_WORKCLOCK(7);
	GET_PCBYTESD(ip);
	ADD_EIP(ip);
}

void
JMP_Jw(void)
{
	DWORD ip;

	CPU_WORKCLOCK(7);
	GET_PCWORDS(ip);
	ADD_EIP(ip);
}

void
JMP_Jd(void)
{
	DWORD ip;

	CPU_WORKCLOCK(7);
	GET_PCDWORD(ip);
	ADD_EIP(ip);
}

void
JMP_Ew(DWORD op)
{
	DWORD madr;
	DWORD new_ip;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(7);
		new_ip = *(reg16_b20[op]);
		SET_EIP(new_ip);
	} else {
		CPU_WORKCLOCK(11);
		madr = calc_ea_dst(op);
		new_ip = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		SET_EIP(new_ip);
	}
}

void
JMP_Ed(DWORD op)
{
	DWORD madr;
	DWORD new_ip;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(7);
		new_ip = *(reg32_b20[op]);
		SET_EIP(new_ip);
	} else {
		CPU_WORKCLOCK(11);
		madr = calc_ea_dst(op);
		new_ip = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		SET_EIP(new_ip);
	}
}

void
JMP16_Ap(void)
{
	DWORD new_ip;
	WORD new_cs;

	CPU_WORKCLOCK(11);
	GET_PCWORD(new_ip);
	GET_PCWORD(new_cs);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);
	} else {
		/* Protected mode */
		JMPfar_pm(new_cs, new_ip);
	}
}

void
JMP32_Ap(void)
{
	DWORD new_ip;
	WORD new_cs;

	CPU_WORKCLOCK(11);
	GET_PCDWORD(new_ip);
	GET_PCWORD(new_cs);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);
	} else {
		/* Protected mode */
		JMPfar_pm(new_cs, new_ip);
	}
}

void
JMP16_Ep(DWORD op)
{
	DWORD madr;
	DWORD new_ip;
	WORD new_cs;

	CPU_WORKCLOCK(11);
	if (op < 0xc0) {
		madr = get_ea(op);
		new_ip = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		new_cs = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr + 2);
		if (!CPU_STAT_PM || CPU_STAT_VM86) {
			/* Real mode or VM86 mode */
			CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
			SET_EIP(new_ip);
		} else {
			/* Protected mode */
			JMPfar_pm(new_cs, new_ip);
		}
		return;
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

void
JMP32_Ep(DWORD op)
{
	DWORD madr;
	DWORD new_ip;
	WORD new_cs;

	CPU_WORKCLOCK(11);
	if (op < 0xc0) {
		madr = get_ea(op);
		new_ip = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		new_cs = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr + 4);
		if (!CPU_STAT_PM || CPU_STAT_VM86) {
			/* Real mode or VM86 mode */
			CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
			SET_EIP(new_ip);
		} else {
			/* Protected mode */
			JMPfar_pm(new_cs, new_ip);
		}
		return;
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

/* jo */
void
JO_Jb(void)
{

	if (!CPU_OV) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JO_Jw(void)
{

	if (!CPU_OV) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JO_Jd(void)
{

	if (!CPU_OV) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jno */
void
JNO_Jb(void)
{

	if (CPU_OV) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JNO_Jw(void)
{

	if (CPU_OV) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JNO_Jd(void)
{

	if (CPU_OV) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jc */
void
JC_Jb(void)
{

	if (!(CPU_FLAGL & C_FLAG)) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JC_Jw(void)
{

	if (!(CPU_FLAGL & C_FLAG)) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JC_Jd(void)
{

	if (!(CPU_FLAGL & C_FLAG)) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jnc */
void
JNC_Jb(void)
{

	if (CPU_FLAGL & C_FLAG) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}
void
JNC_Jw(void)
{

	if (CPU_FLAGL & C_FLAG) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}
void
JNC_Jd(void)
{

	if (CPU_FLAGL & C_FLAG) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jz */
void
JZ_Jb(void)
{

	if (!(CPU_FLAGL & Z_FLAG)) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JZ_Jw(void)
{

	if (!(CPU_FLAGL & Z_FLAG)) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JZ_Jd(void)
{

	if (!(CPU_FLAGL & Z_FLAG)) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jnz */
void
JNZ_Jb(void)
{

	if (CPU_FLAGL & Z_FLAG) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JNZ_Jw(void)
{

	if (CPU_FLAGL & Z_FLAG) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JNZ_Jd(void)
{

	if (CPU_FLAGL & Z_FLAG) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jna */
void
JNA_Jb(void)
{

	if (!(CPU_FLAGL & (Z_FLAG | C_FLAG))) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JNA_Jw(void)
{

	if (!(CPU_FLAGL & (Z_FLAG | C_FLAG))) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JNA_Jd(void)
{

	if (!(CPU_FLAGL & (Z_FLAG | C_FLAG))) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* ja */
void
JA_Jb(void)
{

	if (CPU_FLAGL & (Z_FLAG | C_FLAG)) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JA_Jw(void)
{

	if (CPU_FLAGL & (Z_FLAG | C_FLAG)) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JA_Jd(void)
{

	if (CPU_FLAGL & (Z_FLAG | C_FLAG)) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* js */
void
JS_Jb(void)
{

	if (!(CPU_FLAGL & S_FLAG)) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JS_Jw(void)
{

	if (!(CPU_FLAGL & S_FLAG)) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JS_Jd(void)
{

	if (!(CPU_FLAGL & S_FLAG)) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jns */
void
JNS_Jb(void)
{

	if (CPU_FLAGL & S_FLAG) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JNS_Jw(void)
{

	if (CPU_FLAGL & S_FLAG) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JNS_Jd(void)
{

	if (CPU_FLAGL & S_FLAG) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jp */
void
JP_Jb(void)
{

	if (!(CPU_FLAGL & P_FLAG)) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JP_Jw(void)
{

	if (!(CPU_FLAGL & P_FLAG)) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JP_Jd(void)
{

	if (!(CPU_FLAGL & P_FLAG)) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jnp */
void
JNP_Jb(void)
{

	if (CPU_FLAGL & P_FLAG) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JNP_Jw(void)
{

	if (CPU_FLAGL & P_FLAG) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JNP_Jd(void)
{

	if (CPU_FLAGL & P_FLAG) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jl */
void
JL_Jb(void)
{

	if ((!(CPU_FLAGL & S_FLAG)) == (!CPU_OV)) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JL_Jw(void)
{

	if ((!(CPU_FLAGL & S_FLAG)) == (!CPU_OV)) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JL_Jd(void)
{

	if ((!(CPU_FLAGL & S_FLAG)) == (!CPU_OV)) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jnl */
void
JNL_Jb(void)
{

	if ((!(CPU_FLAGL & S_FLAG)) != (!CPU_OV)) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JNL_Jw(void)
{

	if ((!(CPU_FLAGL & S_FLAG)) != (!CPU_OV)) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JNL_Jd(void)
{

	if ((!(CPU_FLAGL & S_FLAG)) != (!CPU_OV)) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jle */
void
JLE_Jb(void)
{

	if ((!(CPU_FLAGL & Z_FLAG)) && ((!(CPU_FLAGL & S_FLAG)) == (!CPU_OV))) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JLE_Jw(void)
{

	if ((!(CPU_FLAGL & Z_FLAG)) && ((!(CPU_FLAGL & S_FLAG)) == (!CPU_OV))) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JLE_Jd(void)
{

	if ((!(CPU_FLAGL & Z_FLAG)) && ((!(CPU_FLAGL & S_FLAG)) == (!CPU_OV))) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jnle */
void
JNLE_Jb(void)
{

	if ((CPU_FLAGL & Z_FLAG) || ((!(CPU_FLAGL & S_FLAG)) != (!CPU_OV))) {
		JMPNOP(2, 1);
	} else {
		JMPSHORT(7);
	}
}

void
JNLE_Jw(void)
{

	if ((CPU_FLAGL & Z_FLAG) || ((!(CPU_FLAGL & S_FLAG)) != (!CPU_OV))) {
		JMPNOP(2, 2);
	} else {
		JMPNEAR(7);
	}
}

void
JNLE_Jd(void)
{

	if ((CPU_FLAGL & Z_FLAG) || ((!(CPU_FLAGL & S_FLAG)) != (!CPU_OV))) {
		JMPNOP(2, 4);
	} else {
		JMPNEAR_4(7);
	}
}

/* jcxz */
void
JeCXZ_Jb(void)
{

	if (!CPU_INST_AS32) {
		if (CPU_CX) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	} else {
		if (CPU_ECX) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	}
}

/*
 * LOOPcc
 */
/* loopne */
void
LOOPNE_Jb(void)
{

	if (!CPU_INST_AS32) {
		CPU_CX--;
		if (CPU_CX == 0 || (CPU_FLAGL & Z_FLAG)) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	} else {
		CPU_ECX--;
		if (CPU_ECX == 0 || (CPU_FLAGL & Z_FLAG)) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	}
}

/* loope */
void
LOOPE_Jb(void)
{

	if (!CPU_INST_AS32) {
		CPU_CX--;
		if (CPU_CX == 0 || !(CPU_FLAGL & Z_FLAG)) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	} else {
		CPU_ECX--;
		if (CPU_ECX == 0 || !(CPU_FLAGL & Z_FLAG)) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	}
}

/* loop */
void
LOOP_Jb(void)
{

	if (!CPU_INST_AS32) {
		CPU_CX--;
		if (CPU_CX == 0) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	} else {
		CPU_ECX--;
		if (CPU_ECX == 0) {
			JMPNOP(4, 1);
		} else {
			JMPSHORT(8);
		}
	}
}

/*
 * CALL
 */
void
CALL_Aw(void)
{
	DWORD ip;

	CPU_WORKCLOCK(7);
	GET_PCWORDS(ip);
	PUSH0_16(CPU_IP);
	ADD_EIP(ip);
}

void
CALL_Ad(void)
{
	DWORD ip;

	CPU_WORKCLOCK(7);
	GET_PCDWORD(ip);
	PUSH0_32(CPU_EIP);
	ADD_EIP(ip);
}

void
CALL_Ew(DWORD op)
{
	DWORD madr;
	DWORD new_ip;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(7);
		new_ip = *(reg16_b20[op]);
	} else {
		CPU_WORKCLOCK(11);
		madr = calc_ea_dst(op);
		new_ip = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	}
	PUSH0_16(CPU_IP);
	SET_EIP(new_ip);
}

void
CALL_Ed(DWORD op)
{
	DWORD madr;
	DWORD new_ip;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(7);
		new_ip = *(reg32_b20[op]);
	} else {
		CPU_WORKCLOCK(11);
		madr = calc_ea_dst(op);
		new_ip = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	}
	PUSH0_32(CPU_EIP);
	SET_EIP(new_ip);
}

void
CALL16_Ap(void)
{
	DWORD new_ip;
	WORD new_cs;

	CPU_WORKCLOCK(13);
	GET_PCWORD(new_ip);
	GET_PCWORD(new_cs);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		PUSH0_16(CPU_CS);
		PUSH0_16(CPU_IP);

		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);
	} else {
		/* Protected mode */
		CALLfar_pm(new_cs, new_ip);
	}
}

void
CALL32_Ap(void)
{
	DWORD new_ip;
	WORD new_cs;

	CPU_WORKCLOCK(13);
	GET_PCDWORD(new_ip);
	GET_PCWORD(new_cs);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		PUSH0_32(CPU_CS);
		PUSH0_32(CPU_EIP);

		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);
	} else {
		/* Protected mode */
		CALLfar_pm(new_cs, new_ip);
	}
}

void
CALL16_Ep(DWORD op)
{
	DWORD ad;
	DWORD new_ip;
	WORD new_cs;

	CPU_WORKCLOCK(16);
	if (op < 0xc0) {
		ad = get_ea(op);
		new_ip = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, ad);
		new_cs = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, ad + 2);
		if (!CPU_STAT_PM || CPU_STAT_VM86) {
			/* Real mode or VM86 mode */
			PUSH0_16(CPU_CS);
			PUSH0_16(CPU_IP);

			CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
			SET_EIP(new_ip);
		} else {
			/* Protected mode */
			CALLfar_pm(new_cs, new_ip);
		}
		return;
	} 
	EXCEPTION(UD_EXCEPTION, 0);
}

void
CALL32_Ep(DWORD op)
{
	DWORD ad;
	DWORD new_ip;
	WORD new_cs;

	CPU_WORKCLOCK(16);
	if (op < 0xc0) {
		ad = get_ea(op);
		new_ip = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, ad);
		new_cs = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, ad + 4);
		if (!CPU_STAT_PM || CPU_STAT_VM86) {
			/* Real mode or VM86 mode */
			PUSH0_32(CPU_CS);
			PUSH0_32(CPU_EIP);

			CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
			SET_EIP(new_ip);
		} else {
			/* Protected mode */
			CALLfar_pm(new_cs, new_ip);
		}
		return;
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

/*
 * RET
 */
void
RETnear16(void)
{
	DWORD new_ip;

	CPU_WORKCLOCK(11);
	POP0_16(new_ip);
	SET_EIP(new_ip);
}

void
RETnear32(void)
{
	DWORD new_ip;

	CPU_WORKCLOCK(11);
	POP0_32(new_ip);
	SET_EIP(new_ip);
}

void
RETnear16_Iw(void)
{
	DWORD new_ip;
	WORD ad;

	CPU_WORKCLOCK(11);
	GET_PCWORD(ad);
	POP0_16(new_ip);
	SET_EIP(new_ip);
	if (!CPU_STAT_SS32) {
		CPU_SP += ad;
	} else {
		CPU_ESP += ad;
	}
}

void
RETnear32_Iw(void)
{
	DWORD new_ip;
	DWORD ad;

	CPU_WORKCLOCK(11);
	GET_PCWORD(ad);
	POP0_32(new_ip);
	SET_EIP(new_ip);
	if (CPU_STAT_SS32) {
		CPU_ESP += ad;
	} else {
		CPU_SP += ad;
	}
}

void
RETfar16(void)
{
	DWORD new_ip;
	WORD new_cs;

	CPU_WORKCLOCK(15);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		POP0_16(new_ip);
		POP0_16(new_cs);

		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);
	} else {
		/* Protected mode */
		RETfar_pm(0);
	}
}

void
RETfar32(void)
{
	DWORD new_ip;
	WORD new_cs;

	CPU_WORKCLOCK(15);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		POP0_32(new_ip);
		POP0_32(new_cs);

		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);
	} else {
		/* Protected mode */
		RETfar_pm(0);
	}
}

void
RETfar16_Iw(void)
{
	DWORD ad;
	DWORD new_ip;
	WORD new_cs;

	CPU_WORKCLOCK(15);
	GET_PCWORD(ad);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		POP0_16(new_ip);
		POP0_16(new_cs);

		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);

		if (!CPU_STAT_SS32) {
			CPU_SP += ad;
		} else {
			CPU_ESP += ad;
		}
	} else {
		/* Protected mode */
		RETfar_pm(ad);
	}
}

void
RETfar32_Iw(void)
{
	DWORD ad;
	DWORD new_ip;
	WORD new_cs;

	CPU_WORKCLOCK(15);
	GET_PCWORD(ad);
	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		/* Real mode or VM86 mode */
		POP0_32(new_ip);
		POP0_32(new_cs);

		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);

		if (CPU_STAT_SS32) {
			CPU_ESP += ad;
		} else {
			CPU_SP += ad;
		}
	} else {
		/* Protected mode */
		RETfar_pm(ad);
	}
}

void
IRET(void)
{
	DWORD new_ip;
	WORD flag;
	WORD new_cs;

	CPU_WORKCLOCK(31);
	if (!CPU_STAT_PM) {
		/* Real mode */
		POP0_16(new_ip);
		POP0_16(new_cs);
		POP0_16(flag);

		CPU_FLAG = flag & 0x7fd5;
		CPU_OV = CPU_FLAG & O_FLAG;
		CPU_TRAP = (CPU_FLAG & (I_FLAG|T_FLAG)) == (I_FLAG|T_FLAG);

		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);
	} else {
		/* Protected mode */
		IRET_pm();
	}
	IRQCHECKTERM();
}

void
IRETD(void)
{
	DWORD new_ip;
	DWORD flag;
	WORD new_cs;

	CPU_WORKCLOCK(31);
	if (!CPU_STAT_PM) {
		/* Real mode */
		POP0_32(new_ip);
		POP0_32(new_cs);
		POP0_32(flag);

		CPU_EFLAG = (flag & 0x00257fd5) | (CPU_EFLAG & 0x1a0000);
		CPU_OV = CPU_FLAG & O_FLAG;
		CPU_TRAP = (CPU_FLAG & (I_FLAG|T_FLAG)) == (I_FLAG|T_FLAG);

		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);
	} else {
		/* Protected mode */
		IRET_pm();
	}
	IRQCHECKTERM();
}

/*
 * INT
 */
void
INT1(void)
{

	CPU_WORKCLOCK(23);
	INTERRUPT(1, 1, 0, 0);
}

void
INT3(void)
{

	CPU_WORKCLOCK(23);
	INTERRUPT(3, 2, 0, 0);
}

void
INTO(void)
{

	if (!CPU_OV) {
		CPU_WORKCLOCK(4);
		return;
	}
	CPU_WORKCLOCK(24);
	INTERRUPT(4, 3, 0, 0);
}

void
INT_Ib(void)
{
	BYTE vect;

	CPU_WORKCLOCK(23);
	if (!CPU_STAT_PM || !CPU_STAT_VM86 || (CPU_STAT_IOPL == CPU_IOPL3)) {
		GET_PCBYTE(vect);
		INTERRUPT(vect, -1, 0, 0);
		return;
	}
	EXCEPTION(GP_EXCEPTION, 0);
}

void
BOUND_GwMa(void)
{
	DWORD op, madr;
	WORD reg;
	int vect;

	CPU_WORKCLOCK(13);
	GET_PCBYTE(op);
	if (op < 0xc0) {
		reg = *(reg16_b53[op]);
		madr = calc_ea_dst(op);
		if (reg >= cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr) &&
		    reg <= cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr + 2)) {
				return;
		}
		vect = BR_EXCEPTION;
	} else {
		vect = UD_EXCEPTION;
	}
	EXCEPTION(vect, 0);
}

void
BOUND_GdMa(void)
{
	DWORD op, madr;
	DWORD reg;
	int vect;

	CPU_WORKCLOCK(13);
	GET_PCBYTE(op);
	if (op < 0xc0) {
		reg = *(reg32_b53[op]);
		madr = calc_ea_dst(op);
		if (reg >= cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr) &&
		    reg <= cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr + 4)) {
				return;
		}
		vect = BR_EXCEPTION;
	} else {
		vect = UD_EXCEPTION;
	}
	EXCEPTION(vect, 0);
}

/*
 * STACK
 */
void
ENTER_IwIb(void)
{
	WORD dimsize;
	BYTE level;

	GET_PCWORD(dimsize);
	GET_PCBYTE(level);
	level &= 0x1f;

	/* check stack room size */
	if (CPU_STAT_PM) {
		DWORD size = dimsize;
		DWORD sp;
		if (CPU_INST_OP32) {
			size = (level + 1) * 4;
		} else {
			size = (level + 1) * 2;
		}
		if (CPU_STAT_SS32) {
			sp = CPU_ESP;
		} else {
			sp = CPU_SP;
		}
		CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), sp, size);
	}

	XPUSH0(CPU_EBP);
	if (level == 0) {			/* enter level=0 */
		CPU_WORKCLOCK(11);
		if (!CPU_INST_OP32) {
			CPU_BP = CPU_SP;
		} else {
			CPU_EBP = CPU_ESP;
		}
		if (!CPU_STAT_SS32) {
			CPU_SP -= dimsize;
		} else {
			CPU_ESP -= dimsize;
		}
	} else {
		--level;
		if (level == 0) {		/* enter level=1 */
			CPU_WORKCLOCK(15);
			if (!CPU_INST_OP32) {
				WORD tmp = CPU_SP;
				PUSH0_16(tmp);
				CPU_BP = tmp;
				CPU_SP -= dimsize;
			} else {
				DWORD tmp;
				if (!CPU_STAT_SS32) {
					tmp = CPU_SP;
					REGPUSH0_32_16(tmp);
					CPU_EBP = (WORD)tmp;
					CPU_ESP -= dimsize;
				} else {
					tmp = CPU_ESP;
					REGPUSH0_32(tmp);
					CPU_EBP = tmp;
					CPU_ESP -= dimsize;
				}
			}
		} else {			/* enter level=2-31 */
			CPU_WORKCLOCK(12 + level * 4);
			if (!CPU_INST_OP32) {
				WORD bp = CPU_BP;
				WORD val;

				CPU_BP = CPU_SP;
				if (!CPU_STAT_SS32) {
					while (level--) {
						bp -= 2;
						CPU_SP -= 2;
						val = cpu_vmemoryread_w(CPU_SS_INDEX, bp);
						cpu_vmemorywrite_w(CPU_SS_INDEX, CPU_SP, val);
					}
					REGPUSH0(CPU_BP);
					CPU_SP -= dimsize;
				} else {
					while (level--) {
						bp -= 2;
						CPU_ESP -= 2;
						val = cpu_vmemoryread_w(CPU_SS_INDEX, bp);
						cpu_vmemorywrite_w(CPU_SS_INDEX, CPU_ESP, val);
					}
					REGPUSH0_16_32(CPU_EBP);
					CPU_ESP -= dimsize;
				}
			} else {
				DWORD ebp = CPU_EBP;
				DWORD val;

				if (!CPU_STAT_SS32) {
					CPU_EBP = CPU_SP;
					while (level--) {
						ebp -= 4;
						CPU_SP -= 4;
						val = cpu_vmemoryread_d(CPU_SS_INDEX, ebp);
						cpu_vmemorywrite_d(CPU_SS_INDEX, CPU_SP, val);
					}
					REGPUSH0_32_16(CPU_EBP);
					CPU_SP -= dimsize;
				} else {
					CPU_EBP = CPU_ESP;
					while (level--) {
						ebp -= 4;
						CPU_ESP -= 4;
						val = cpu_vmemoryread_d(CPU_SS_INDEX, ebp);
						cpu_vmemorywrite(CPU_SS_INDEX, CPU_ESP, val);
					}
					REGPUSH0_32(CPU_EBP);
					CPU_ESP -= dimsize;
				}
			}
		}
	}
}

void
LEAVE16(void)
{
	WORD bp;
	DWORD sp, size;

	CPU_WORKCLOCK(5);

	if (CPU_STAT_PM) {
		bp = CPU_BP;
		if (!CPU_STAT_SS32) {
			sp = CPU_SP;
			size = 2;
		} else {
			sp = CPU_ESP;
			size = 4;
		}
		if (bp < sp) {
			ia32_panic("LEAVE16: bp < sp");
		}
		CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), sp, (bp - sp) + size);
	}

	CPU_SP = CPU_BP;
	REGPOP0(CPU_BP);
}

void
LEAVE32(void)
{
	DWORD bp, sp, size;

	CPU_WORKCLOCK(5);

	if (CPU_STAT_PM) {
		bp = CPU_EBP;
		if (CPU_STAT_SS32) {
			sp = CPU_ESP;
			size = 4;
		} else {
			sp = CPU_SP;
			size = 2;
		}
		if (bp < sp) {
			ia32_panic("LEAVE32: bp < sp");
		}
		CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), sp, (bp - sp) + size);
	}

	if (CPU_STAT_SS32) {
		CPU_ESP = CPU_EBP;
		REGPOP0_32(CPU_EBP);
	} else {
		CPU_SP = CPU_BP;
		REGPOP0_32_16(CPU_EBP);
	}
}

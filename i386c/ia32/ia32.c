/*	$Id: ia32.c,v 1.7 2004/02/05 16:41:32 monaka Exp $	*/

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


I386CORE	i386core;

BYTE	*reg8_b20[0x100];
BYTE	*reg8_b53[0x100];
WORD	*reg16_b20[0x100];
WORD	*reg16_b53[0x100];
DWORD	*reg32_b20[0x100];
DWORD	*reg32_b53[0x100];


void
ia32_init(void)
{
	int i;

	memset(&i386core.s, 0, sizeof(i386core.s));
	ia32_initreg();

	for (i = 0; i < 0x100; ++i) {
		/* 8bit */
		if (i & 0x20) {
			/* h */
			reg8_b53[i] = &CPU_REGS_BYTEH((i >> 3) & 3);
		} else {
			/* l */
			reg8_b53[i] = &CPU_REGS_BYTEL((i >> 3) & 3);
		}

		if (i & 0x04) {
			/* h */
			reg8_b20[i] = &CPU_REGS_BYTEH(i & 3);
		} else {
			/* l */
			reg8_b20[i] = &CPU_REGS_BYTEL(i & 3);
		}

		/* 16bit */
		reg16_b53[i] = &CPU_REGS_WORD((i >> 3) & 7);
		reg16_b20[i] = &CPU_REGS_WORD(i & 7);

		/* 32bit */
		reg32_b53[i] = &CPU_REGS_DWORD((i >> 3) & 7);
		reg32_b20[i] = &CPU_REGS_DWORD(i & 7);
	}

	resolve_init();
#if defined(IA32_SUPPORT_TLB)
	tlb_init();
#endif
#ifdef USE_FPU
	fpu_init();
#endif
}

void
ia32_setextsize(UINT32 size)
{

	if (CPU_EXTMEMSIZE != size) {
		if (CPU_EXTMEM) {
			_MFREE(CPU_EXTMEM);
			CPU_EXTMEM = NULL;
		}
		if (size) {
			CPU_EXTMEM = (BYTE *)_MALLOC(size + 16, "EXTMEM");
			if (CPU_EXTMEM == NULL) {
				size = 0;
			}
			ZeroMemory(CPU_EXTMEM, size + 16);
		}
		CPU_EXTMEMSIZE = size;
	}
}

/*
 * モード遷移
 */
void FASTCALL
change_pm(BOOL onoff)
{
	int i;

	if (onoff) {
		for (i = 0; i < CPU_SEGREG_NUM; i++) {
			CPU_STAT_SREG(i).valid = 1;
			CPU_STAT_SREG(i).dpl = 0;
		}
		VERBOSE(("Entering to Protected-Mode..."));
	} else {
		VERBOSE(("Leaveing from Protected-Mode..."));
	}

	CPU_INST_OP32 = CPU_INST_AS32 =
	    CPU_STATSAVE.cpu_inst_default.op_32 = 
	    CPU_STATSAVE.cpu_inst_default.as_32 = 0;
	CPU_STAT_SS32 = 0;
	CPU_SET_CPL(0);
	CPU_STAT_PM = onoff;
}

void FASTCALL
change_pg(BOOL onoff)
{

	if (onoff) {
		VERBOSE(("Entering to Paging-Mode..."));
	} else {
		VERBOSE(("Leaveing from Paging-Mode..."));
	}
	CPU_STAT_PAGING = onoff;
}

void FASTCALL
change_vm(BOOL onoff)
{
	int i;

	CPU_STAT_VM86 = onoff;
	if (onoff) {
		for (i = 0; i < CPU_SEGREG_NUM; i++) {
			CPU_SET_SEGREG(i, CPU_REGS_SREG(i));
		}
		CPU_INST_OP32 = CPU_INST_AS32 =
		    CPU_STATSAVE.cpu_inst_default.op_32 =
		    CPU_STATSAVE.cpu_inst_default.as_32 = 0;
		CPU_STAT_SS32 = 0;
		CPU_SET_CPL(3);
		VERBOSE(("Entering to Virtual-8086-Mode..."));
	} else {
		VERBOSE(("Leaveing from Virtual-8086-Mode..."));
	}
}

/*
 * flags
 */
static void
modify_eflags(DWORD new_flags, DWORD mask)
{
	DWORD orig = CPU_EFLAG;

	new_flags &= ALL_EFLAG;
	mask &= ALL_EFLAG;
	CPU_EFLAG = (REAL_EFLAGREG & ~mask) | (new_flags & mask) | 0x2;

	CPU_OV = CPU_FLAG & O_FLAG;
	CPU_TRAP = (CPU_FLAG & (I_FLAG|T_FLAG)) == (I_FLAG|T_FLAG);
	if ((orig ^ CPU_EFLAG) & VM_FLAG) {
		if (CPU_EFLAG & VM_FLAG) {
			change_vm(1);
		} else {
			change_vm(0);
		}
	}
}

void
set_flags(WORD new_flags, WORD mask)
{

	mask &= I_FLAG|IOPL_FLAG;
	mask |= SZAPC_FLAG|T_FLAG|D_FLAG|O_FLAG|NT_FLAG;
	modify_eflags(new_flags, mask);
}

void
set_eflags(DWORD new_flags, DWORD mask)
{

	mask &= I_FLAG|IOPL_FLAG|RF_FLAG|VM_FLAG|VIF_FLAG|VIP_FLAG;
	mask |= SZAPC_FLAG|T_FLAG|D_FLAG|O_FLAG|NT_FLAG;
	mask |= AC_FLAG|ID_FLAG;
	modify_eflags(new_flags, mask);
}

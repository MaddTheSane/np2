/*	$Id: task.c,v 1.1 2003/12/08 00:55:31 yui Exp $	*/

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


void
load_tr(WORD selector)
{
	selector_t task_sel;
	int rv;

	rv = parse_selector(&task_sel, selector);
	if (rv < 0 || task_sel.ldt || task_sel.desc.s) {
		EXCEPTION(GP_EXCEPTION, task_sel.idx);
	}

	/* check descriptor type & stack room size */
	switch (task_sel.desc.type) {
	case CPU_SYSDESC_TYPE_TSS_16:
		if (task_sel.desc.u.seg.limit < 0x2b) {
			EXCEPTION(TS_EXCEPTION, task_sel.idx);
		}
		break;

	case CPU_SYSDESC_TYPE_TSS_32:
		if (task_sel.desc.u.seg.limit < 0x67) {
			EXCEPTION(TS_EXCEPTION, task_sel.idx);
		}
		break;

	default:
		EXCEPTION(GP_EXCEPTION, task_sel.idx);
		break;
	}

	/* not present */
	rv = selector_is_not_present(&task_sel);
	if (rv < 0) {
		EXCEPTION(NP_EXCEPTION, task_sel.idx);
	}

	CPU_SET_TASK_BUSY(&task_sel.desc);
	CPU_TR = task_sel.selector;
	CPU_TR_DESC = task_sel.desc;
}

void
get_stack_from_tss(BYTE pl, WORD* new_ss, DWORD* new_esp)
{
	DWORD tss_stack_addr;

	switch (CPU_TR_DESC.type) {
	case CPU_SYSDESC_TYPE_TSS_BUSY_32:
		tss_stack_addr = pl * 8 + 4;
		if (tss_stack_addr + 7 > CPU_TR_DESC.u.seg.limit) {
			EXCEPTION(TS_EXCEPTION, CPU_TR & ~3);
		}
		tss_stack_addr += CPU_TR_DESC.u.seg.segbase;
		*new_esp = cpu_lmemoryread_d(tss_stack_addr);
		*new_ss = cpu_lmemoryread_w(tss_stack_addr + 4);
		break;

	case CPU_SYSDESC_TYPE_TSS_BUSY_16:
		tss_stack_addr = pl * 4 + 2;
		if (tss_stack_addr + 4 > CPU_TR_DESC.u.seg.limit) {
			EXCEPTION(TS_EXCEPTION, CPU_TR & ~3);
		}
		tss_stack_addr += CPU_TR_DESC.u.seg.segbase;
		*new_esp = cpu_lmemoryread_w(tss_stack_addr);
		*new_ss = cpu_lmemoryread_w(tss_stack_addr + 2);
		break;

	default:
		ia32_panic("get_stack_from_tss: TR is invalid (%d)\n",
		    CPU_TR_DESC.type);
		break;
	}
}

WORD
get_link_selector_from_tss()
{

	if (CPU_TR_DESC.type == CPU_SYSDESC_TYPE_TSS_BUSY_32) {
		if (4 > CPU_TR_DESC.u.seg.limit) {
			EXCEPTION(TS_EXCEPTION, CPU_TR & ~3);
		}
	} else if (CPU_TR_DESC.type == CPU_SYSDESC_TYPE_TSS_BUSY_16) {
		if (2 > CPU_TR_DESC.u.seg.limit) {
			EXCEPTION(TS_EXCEPTION, CPU_TR & ~3);
		}
	} else {
		ia32_panic("get_link_selector_from_tss: TR is invalid (%d)\n",
		    CPU_TR_DESC.type);
		return 0;	/* compiler happy */
	}

	return cpu_lmemoryread_w(CPU_TR_DESC.u.seg.segbase);
}

void
task_switch(selector_t* task_sel, int type)
{
	DWORD regs[CPU_REG_NUM];
	DWORD eip;
	DWORD new_flags;
	DWORD cr3 = 0;
	WORD sreg[CPU_SEGREG_NUM];
	WORD ldtr;
	WORD t, iobase;

	selector_t cs_sel;
	int rv;

	DWORD cur_base;		/* current task state */
	DWORD task_base;	/* new task state */
	DWORD old_flags = REAL_EFLAGREG;
	BOOL task16;
	int nsreg;
	int i;

	cur_base = CPU_TR_DESC.u.seg.segbase;
	task_base = task_sel->desc.u.seg.segbase;

	/* limit check */
	switch (task_sel->desc.type) {
	case CPU_SYSDESC_TYPE_TSS_32:
	case CPU_SYSDESC_TYPE_TSS_BUSY_32:
		if (task_sel->desc.u.seg.limit < 103) {
			EXCEPTION(TS_EXCEPTION, task_sel->idx);
		}
		task16 = FALSE;
		nsreg = CPU_SEGREG_NUM;
		break;

	case CPU_SYSDESC_TYPE_TSS_16:
	case CPU_SYSDESC_TYPE_TSS_BUSY_16:
		if (task_sel->desc.u.seg.limit < 43) {
			EXCEPTION(TS_EXCEPTION, task_sel->idx);
		}
		task16 = TRUE;
		nsreg = CPU_SEGREG286_NUM;
		break;

	default:
		ia32_panic("task_switch: descriptor type is invalid.");
		task16 = FALSE;		/* compiler happy */
		nsreg = CPU_SEGREG_NUM;	/* compiler happy */
		break;
	}

	if (CPU_STAT_PAGING) {
		/* task state paging check */
		paging_check(cur_base, CPU_TR_DESC.u.seg.limit, CPU_PAGING_PAGE_WRITE);
		paging_check(task_base, task_sel->desc.u.seg.limit, CPU_PAGING_PAGE_WRITE);
	}

	/* load task state */
	memset(sreg, 0, sizeof(sreg));
	if (!task16) {
		if (CPU_STAT_PAGING) {
			cr3 = cpu_lmemoryread_d(task_base + 28);
		}
		eip = cpu_lmemoryread_d(task_base + 32);
		new_flags = cpu_lmemoryread_d(task_base + 36);
		for (i = 0; i < CPU_REG_NUM; i++) {
			regs[i] = cpu_lmemoryread_d(task_base + 40 + i * 4);
		}
		for (i = 0; i < nsreg; i++) {
			sreg[i] = (WORD)cpu_lmemoryread_d(task_base + 72 + i * 4);
		}
		ldtr = (WORD)cpu_lmemoryread_d(task_base + 96);
		t = cpu_lmemoryread_w(task_base + 100);
		iobase = cpu_lmemoryread_w(task_base + 102);
	} else {
		eip = cpu_lmemoryread_w(task_base + 14);
		new_flags = cpu_lmemoryread_w(task_base + 16);
		for (i = 0; i < CPU_REG_NUM; i++) {
			regs[i] = cpu_lmemoryread_w(task_base + 18 + i * 2);
		}
		for (i = 0; i < nsreg; i++) {
			sreg[i] = cpu_lmemoryread_w(task_base + 34 + i * 2);
		}
		ldtr = cpu_lmemoryread_w(task_base + 42);
		iobase = 0;
	}

	/* if IRET or JMP, clear busy flag in this task: need */
	/* if IRET, clear NT_FLAG in current EFLAG: need */
	switch (type) {
	case TASK_SWITCH_IRET:
		/* clear NT_FLAG */
		old_flags &= ~NT_FLAG;
		/*FALLTHROUGH*/
	case TASK_SWITCH_JMP:
		/* clear busy flags in current task */
		CPU_SET_TASK_FREE(&CPU_TR_DESC);
		break;

	case TASK_SWITCH_CALL:
	case TASK_SWITCH_INTR:
		/* Nothing to do */
		break;
	
	default:
		ia32_panic("task_switch(): task switch type is invalid");
		break;
	}

	/* save this task state in this task state segment ind */
	if (!task16) {
		cpu_lmemorywrite_d(cur_base + 28, CPU_CR3);
		cpu_lmemorywrite_d(cur_base + 32, CPU_EIP);
		cpu_lmemorywrite_d(cur_base + 36, old_flags);
		for (i = 0; i < CPU_REG_NUM; i++) {
			cpu_lmemorywrite_d(cur_base + 40 + i * 4, CPU_REGS_DWORD(i));
		}
		for (i = 0; i < nsreg; i++) {
			cpu_lmemorywrite_d(cur_base + 72 + i * 4, CPU_REGS_SREG(i));
		}
		cpu_lmemorywrite_d(cur_base + 96, CPU_LDTR);
	} else {
		cpu_lmemorywrite_w(cur_base + 14, CPU_IP);
		cpu_lmemorywrite_w(cur_base + 16, (WORD)old_flags);
		for (i = 0; i < CPU_REG_NUM; i++) {
			cpu_lmemorywrite_w(cur_base + 18 + i * 2, CPU_REGS_WORD(i));
		}
		for (i = 0; i < nsreg; i++) {
			cpu_lmemorywrite_w(cur_base + 34 + i * 2, CPU_REGS_SREG(i));
		}
		cpu_lmemorywrite_w(cur_base + 42, CPU_LDTR);
	}

	/* set back link selector */
	switch (type) {
	case TASK_SWITCH_CALL:
	case TASK_SWITCH_INTR:
		/* set back link selector */
		cpu_lmemorywrite_d(task_base, CPU_TR);
		break;
	
	case TASK_SWITCH_IRET:
	case TASK_SWITCH_JMP:
		/* Nothing to do */
		break;

	default:
		ia32_panic("task_switch(): task switch type is invalid");
		break;
	}

	/* Now task switching! */

	/* if CALL, INTR, set EFLAG image NT_FLAG */
	/* if CALL, INTR, JMP set busy flag */
	switch (type) {
	case TASK_SWITCH_CALL:
	case TASK_SWITCH_INTR:
		/* set back link selector */
		new_flags |= NT_FLAG;
		/*FALLTHROUGH*/
	case TASK_SWITCH_JMP:
		CPU_SET_TASK_BUSY(&task_sel->desc);
		break;
	
	case TASK_SWITCH_IRET:
		/* Nothing to do */
		/* XXX: if IRET, check busy flag is active? */
		break;

	default:
		ia32_panic("task_switch(): task switch type is invalid");
		break;
	}

	/* set CR0 image CPU_CR0_TS */
	CPU_CR0 |= CPU_CR0_TS;

	/* load task selector to CPU_TR */
	CPU_TR = task_sel->selector;
	CPU_TR_DESC = task_sel->desc;

	/* load task state (CR3, EFLAG, EIP, GPR, segreg, LDTR) */
	if (CPU_STAT_PAGING) {
		/* XXX setCR3()? */
		CPU_CR3 = cr3 & 0xfffff018;
		tlb_flush(FALSE);
	}

	/* set new EFLAGS, EIP, GPR, segment register, LDTR */
	set_eflags(new_flags, I_FLAG|IOPL_FLAG|RF_FLAG|VM_FLAG|VIF_FLAG|VIP_FLAG);
	CPU_PREV_EIP = CPU_EIP = eip;
	for (i = 0; i < CPU_REG_NUM; i++) {
		CPU_REGS_DWORD(i) = regs[i];
	}
	for (i = 0; i < CPU_SEGREG_NUM; i++) {
		CPU_REGS_SREG(i) = sreg[i];
		CPU_STAT_SREG_CLEAR(i);
	}
	CPU_LDTR = ldtr;
	CPU_LDTR_DESC.valid = 0;

	/* load LDTR */
	load_ldtr(ldtr, TS_EXCEPTION);

	/* load CS */
	rv = parse_selector(&cs_sel, sreg[CPU_CS_INDEX]);
	if (rv < 0) {
		EXCEPTION(TS_EXCEPTION, cs_sel.idx);
	}

	/* CS register must be code segment */
	if (!cs_sel.desc.s || !cs_sel.desc.u.seg.c) {
		EXCEPTION(TS_EXCEPTION, cs_sel.idx);
	}

	/* check privilege level */
	if (!cs_sel.desc.u.seg.ec) {
		/* non-confirming code segment */
		if (cs_sel.desc.dpl != cs_sel.rpl) {
			EXCEPTION(TS_EXCEPTION, cs_sel.idx);
		}
	} else {
		/* confirming code segment */
		if (cs_sel.desc.dpl < cs_sel.rpl) {
			EXCEPTION(TS_EXCEPTION, cs_sel.idx);
		}
	}

	/* CS segment is not present */
	rv = selector_is_not_present(&cs_sel);
	if (rv < 0) {
		EXCEPTION(NP_EXCEPTION, cs_sel.idx);
	}

	/* Now loading CS register */
	load_cs(cs_sel.selector, &cs_sel.desc, cs_sel.desc.dpl);

	/* load ES, SS, DS, FS, GS segment register */
	for (i = 0; i < nsreg; i++) {
		if (i != CPU_CS_INDEX) {
			load_segreg(i, sreg[i], TS_EXCEPTION);
		}
	}

	/* I/O deny bitmap */
	if (!task16) {
		if (task_sel->desc.u.seg.limit > iobase) {
			CPU_STAT_IOLIMIT = task_sel->desc.u.seg.limit - iobase;
			CPU_STAT_IOLIMIT *= 8;	/* �ӥå�ñ�̤��ݻ����Ƥ��� */
			CPU_STAT_IOADDR = task_sel->desc.u.seg.segbase + iobase;
		} else {
			CPU_STAT_IOLIMIT = 0;
		}
	} else {
		CPU_STAT_IOLIMIT = 0;
	}

	/* running new task */
	SET_EIP(eip);
}

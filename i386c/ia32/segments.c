/*	$Id: segments.c,v 1.1 2003/12/08 00:55:31 yui Exp $	*/

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
load_segreg(int idx, WORD selector, int exc)
{
	selector_t sel;
	int rv;


	if ((unsigned int)idx >= CPU_SEGREG_NUM) {
		ia32_panic("load_segreg: sreg(%d)", idx);
	}

	if (!CPU_STAT_PM || CPU_STAT_VM86) {
		descriptor_t sd;

		/* real-mode or vm86 mode */
		CPU_REGS_SREG(idx) = selector;

		sd.u.seg.limit = CPU_STAT_SREGLIMIT(idx);
		CPU_SET_SEGDESC_DEFAULT(&sd, idx, selector);
		CPU_STAT_SREG(idx) = sd;
		return;
	}

	/*
	 * protected mode
	 */
	VERBOSE(("load_segreg: idx = %d, selector = %04x, exc = %d", idx, selector, exc));

	if (idx == CPU_CS_INDEX) {
		ia32_panic("load_segreg: sreg(%d)", idx);
	}

	rv = parse_selector(&sel, selector);
	if (rv < 0) {
		if ((rv != -2) || (idx == CPU_SS_INDEX)) {
			EXCEPTION(exc, sel.idx);
		}
		CPU_REGS_SREG(idx) = sel.selector;
		CPU_STAT_SREG_CLEAR(idx);
		return;
	}

	switch (idx) {
	case CPU_SS_INDEX:
		if ((CPU_STAT_CPL != sel.rpl) ||
		    !sel.desc.s || sel.desc.u.seg.c || !sel.desc.u.seg.wr ||
		    (CPU_STAT_CPL != sel.desc.dpl)) {
			EXCEPTION(exc, sel.idx);
		}

		/* not present */
		rv = selector_is_not_present(&sel);
		if (rv < 0) {
			EXCEPTION(SS_EXCEPTION, sel.idx);
		}

		CPU_STAT_SS32 = sel.desc.d;
		CPU_REGS_SREG(idx) = sel.selector;
		CPU_STAT_SREG(idx) = sel.desc;
		break;

	case CPU_ES_INDEX:
	case CPU_DS_INDEX:
	case CPU_FS_INDEX:
	case CPU_GS_INDEX:
		/* !(system segment || non-readble code segment */
		if (!sel.desc.s
		 || (sel.desc.u.seg.c && !sel.desc.u.seg.wr)) {
			EXCEPTION(exc, sel.idx);
		}
		/* data segment || non-conforming code segment */
		if (!sel.desc.u.seg.c || !sel.desc.u.seg.ec) {
			/* check privilege level */
			if ((sel.rpl > sel.desc.dpl) || (CPU_STAT_CPL > sel.desc.dpl)) {
				EXCEPTION(exc, sel.idx);
			}
		}

		/* not present */
		rv = selector_is_not_present(&sel);
		if (rv < 0) {
			EXCEPTION(NP_EXCEPTION, sel.idx);
		}

		CPU_REGS_SREG(idx) = sel.selector;
		CPU_STAT_SREG(idx) = sel.desc;
		break;
	
	default:
		ia32_panic("load_segreg(): segment register index is invalid");
		break;
	}
}

/*
 * load SS register
 */
void
load_ss(WORD selector, descriptor_t* sdp, BYTE cpl)
{

	CPU_STAT_SS32 = sdp->d;
	CPU_REGS_SREG(CPU_SS_INDEX) = (selector & ~3) | (cpl & 3);
	CPU_STAT_SREG(CPU_SS_INDEX) = *sdp;
}

/*
 * load CS register
 */
void
load_cs(WORD selector, descriptor_t* sdp, BYTE cpl)
{

	cpu_inst_default.op_32 = cpu_inst_default.as_32 = sdp->d;
	CPU_REGS_SREG(CPU_CS_INDEX) = (selector & ~3) | (cpl & 3);
	CPU_STAT_SREG(CPU_CS_INDEX) = *sdp;
	CPU_STAT_CPL = cpl & 3;
}

/*
 * load LDT register
 */
void
load_ldtr(WORD selector, int exc)
{
	selector_t sel;
	int rv;

	rv = parse_selector(&sel, selector);
	if (rv < 0 || sel.ldt) {
		if (rv == -2) {
			/* null segment */
			CPU_LDTR = 0;
			memset(&CPU_LDTR_DESC, 0, sizeof(CPU_LDTR_DESC));
			return;
		}
		EXCEPTION(exc, sel.selector);
	}

	/* check descriptor type */
	if (!sel.desc.s || (sel.desc.type != CPU_SYSDESC_TYPE_LDT)) {
		EXCEPTION(exc, sel.selector);
	}

	/* check limit */
	if (sel.desc.u.seg.limit < 7) {
		ia32_panic("load_ldtr: LDTR descriptor limit < 7");
	}

	/* not present */
	rv = selector_is_not_present(&sel);
	if (rv < 0) {
		EXCEPTION((exc == TS_EXCEPTION) ? TS_EXCEPTION : NP_EXCEPTION, sel.selector);
	}

	CPU_LDTR = sel.selector;
	CPU_LDTR_DESC = sel.desc;
}

void
load_descriptor(descriptor_t *descp, DWORD addr)
{

	descp->addr = addr;
	descp->l = cpu_lmemoryread_d(descp->addr);
	descp->h = cpu_lmemoryread_d(descp->addr + 4);

	descp->flag = 0;

	descp->p = (descp->h & CPU_DESC_H_P) == CPU_DESC_H_P;
	descp->type = (descp->h & CPU_DESC_H_TYPE) >> 8;
	descp->dpl = (descp->h & CPU_DESC_H_DPL) >> 13;
	descp->s = (descp->h & CPU_DESC_H_S) == CPU_DESC_H_S;

	if (descp->s) {
		/* code/data */
		descp->valid = 1;

		descp->d = (descp->h & CPU_SEGDESC_H_D) ? 1 : 0;
		descp->u.seg.c = (descp->h & CPU_SEGDESC_H_D_C) ? 1 : 0;
		descp->u.seg.g = (descp->h & CPU_SEGDESC_H_G) ? 1 : 0;
		descp->u.seg.wr = (descp->type & CPU_SEGDESC_TYPE_WR) ? 1 : 0;
		descp->u.seg.ec = (descp->type & CPU_SEGDESC_TYPE_EC) ? 1 : 0;

		descp->u.seg.segbase  = (descp->l >> 16) & 0xffff;
		descp->u.seg.segbase |= (descp->h & 0xff) << 16;
		descp->u.seg.segbase |= descp->h & 0xff000000;

		descp->u.seg.limit = (descp->h & 0xf0000) | (descp->l & 0xffff);
		if (descp->u.seg.g) {
			descp->u.seg.limit <<= 12;
			descp->u.seg.limit |= 0xfff;
		}

		descp->u.seg.segend = descp->u.seg.segbase + descp->u.seg.limit;

		VERBOSE(("load_descriptor: %s segment descriptor: addr = 0x%08x, h = 0x%04x, l = %04x, type = %d, DPL = %d, base = 0x%08x, limit = 0x%08x, d = %s, g = %s, %s, %s", descp->u.seg.c ? "code" : "data", descp->addr, descp->h, descp->l, descp->type, descp->dpl, descp->u.seg.segbase, descp->u.seg.limit, descp->d ? "on" : "off",  descp->u.seg.g ? "on" : "off", descp->u.seg.c ? (descp->u.seg.wr ? "executable/readable" : "execute-only") : (descp->u.seg.wr ? "writable" : "read-only"), (descp->u.seg.c ? (descp->u.seg.ec ? "conforming" : "non-conforming") : (descp->u.seg.ec ? "expand-down" : "expand-up"))));
	} else {
		/* system */
		switch (descp->type) {
		case CPU_SYSDESC_TYPE_LDT:		/* LDT */
			descp->valid = 1;
			VERBOSE(("load_descriptor: LDT descriptor"));
			break;

		case CPU_SYSDESC_TYPE_TASK:
			descp->valid = 1;
			descp->u.gate.selector = descp->l >> 16;
			VERBOSE(("load_descriptor: task descriptor: selector = 0x%04x", descp->u.gate.selector));
			break;

		case CPU_SYSDESC_TYPE_TSS_16:		/* 286 TSS */
		case CPU_SYSDESC_TYPE_TSS_BUSY_16:	/* 286 TSS Busy */
			descp->valid = 1;
			descp->u.seg.segbase |= (descp->h & 0xff) << 16;
			descp->u.seg.segbase |= descp->l >> 16;
			descp->u.seg.limit  = descp->h & 0xf0000;
			descp->u.seg.limit |= descp->l & 0xffff;
			descp->u.seg.segend = descp->u.seg.segbase + descp->u.seg.limit;
			VERBOSE(("load_descriptor: 16bit %sTSS descriptor: base = 0x%08x, limit = 0x%08x", (descp->type & CPU_SYSDESC_TYPE_TSS_BUSY) ? "busy " : "", descp->u.seg.segbase, descp->u.seg.limit));
			break;

		case CPU_SYSDESC_TYPE_CALL_16:		/* 286 call gate */
		case CPU_SYSDESC_TYPE_INTR_16:		/* 286 interrupt gate */
		case CPU_SYSDESC_TYPE_TRAP_16:		/* 286 trap gate */
			if ((descp->h & 0x0000000e0) == 0) {
				descp->valid = 1;
				descp->u.gate.selector = descp->l >> 16;
				descp->u.gate.offset = descp->l & 0xffff;
				descp->u.gate.count = descp->h & 0x1f;
				VERBOSE(("load_descriptor: 16bit %s gate descriptor: selector = 0x%04x, offset = 0x%08x, count = %d", (descp->type == CPU_SYSDESC_TYPE_CALL_16) ? "call" : ((descp->type == CPU_SYSDESC_TYPE_INTR_16) ? "interrupt" : "trap"), descp->u.gate.selector, descp->u.gate.offset, descp->u.gate.count));
			} else {
				ia32_panic("load_descriptor: 286 gate is invalid");
			}
			break;

		case CPU_SYSDESC_TYPE_TSS_32:		/* 386 TSS */
		case CPU_SYSDESC_TYPE_TSS_BUSY_32:	/* 386 TSS Busy */
			descp->valid = 1;
			descp->d = (descp->h & CPU_SEGDESC_H_D) ? 1 : 0;
			descp->u.seg.g = (descp->h & CPU_SEGDESC_H_G) ? 1 : 0;
			descp->u.seg.segbase  = descp->h & 0xff000000;
			descp->u.seg.segbase |= (descp->h & 0xff) << 16;
			descp->u.seg.segbase |= descp->l >> 16;
			descp->u.seg.limit  = descp->h & 0xf0000;
			descp->u.seg.limit |= descp->l & 0xffff;
			if (descp->u.seg.g) {
				descp->u.seg.limit <<= 12;
				descp->u.seg.limit |= 0xfff;
			}
			VERBOSE(("load_descriptor: 32bit %sTSS descriptor: base = 0x%08x, limit = 0x%08x, d = %s, g = %s", (descp->type & CPU_SYSDESC_TYPE_TSS_BUSY) ? "busy " : "", descp->u.seg.segbase, descp->u.seg.limit, descp->d ? "on" : "off", descp->u.seg.g ? "on" : "off"));
			break;

		case CPU_SYSDESC_TYPE_CALL_32:		/* 386 call gate */
		case CPU_SYSDESC_TYPE_INTR_32:		/* 386 interrupt gate */
		case CPU_SYSDESC_TYPE_TRAP_32:		/* 386 trap gate */
			if ((descp->h & 0x0000000e0) == 0) {
				descp->valid = 1;
				descp->d = (descp->h & CPU_GATEDESC_H_D) ? 1:0;
				descp->u.gate.selector = descp->l >> 16;
				descp->u.gate.offset  = descp->h & 0xffff0000;
				descp->u.gate.offset |= descp->l & 0xffff;
				descp->u.gate.count = descp->h & 0x1f;
				VERBOSE(("load_descriptor: 32bit %s gate descriptor: selector = 0x%04x, offset = 0x%08x, count = %d, d = %s", (descp->type == CPU_SYSDESC_TYPE_CALL_16) ? "call" : ((descp->type == CPU_SYSDESC_TYPE_INTR_16) ? "interrupt" : "trap"), descp->u.gate.selector, descp->u.gate.offset, descp->u.gate.count, descp->d ? "on" : "off"));
			} else {
				ia32_panic("load_descriptor: 286 gate is invalid");
			}
			break;

		case 0: case 8: case 10: case 13: /* reserved */
		default:
			descp->valid = 0;
			ia32_panic("bad segment descriptor (%d)", descp->type);
			break;
		}
	}
}

int
parse_selector(selector_t* ssp, WORD selector)
{
	DWORD base;
	WORD limit;
	WORD idx;

	ssp->selector = selector;
	ssp->idx = selector & ~3;
	ssp->rpl = selector & 3;
	ssp->ldt = selector & CPU_SEGMENT_TABLE_IND;

	VERBOSE(("parse_selector: selector = %04x, index = %d, RPL = %d, %cDT", ssp->selector, ssp->idx >> 3, ssp->rpl, ssp->ldt ? 'L' : 'G'));

	/* descriptor table */
	idx = selector & ~7;
	if (ssp->ldt) {
		/* LDT */
		if (!CPU_LDTR_DESC.valid) {
			VERBOSE(("parse_selector: LDT is invalid"));
			return -1;
		}
		base = CPU_LDTR_BASE;
		limit = CPU_LDTR_LIMIT;
	} else {
		/* check null segment */
		if (idx == 0) {
			VERBOSE(("parse_selector: null segment"));
			return -2;
		}
		base = CPU_GDTR_BASE;
		limit = CPU_GDTR_LIMIT;
	}
	if (idx + 7 > limit) {
		VERBOSE(("parse_selector: segment limit check failed"));
		return -3;
	}
	/* load descriptor */
	CPU_SET_SEGDESC(&ssp->desc, base + idx);
	if (!ssp->desc.valid) {
		VERBOSE(("parse_selector: segment descriptor is invalid"));
		return -4;
	}
	return 0;
}

int
selector_is_not_present(selector_t* ssp)
{
	/* not present */
	if (!ssp->desc.p) {
		VERBOSE(("selector_is_not_present: not present"));
		return -1;
	}
	CPU_SET_SEGDESC_POSTPART(&ssp->desc);
	return 0;
}

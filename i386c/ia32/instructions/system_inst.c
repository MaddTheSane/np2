/*	$Id: system_inst.c,v 1.12 2004/01/29 11:42:58 monaka Exp $	*/

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

#include "system_inst.h"


void
LGDT_Ms(DWORD op)
{
	DWORD madr;
	DWORD base;
	WORD limit;

	if (op < 0xc0) {
		if (!CPU_STAT_PM || !CPU_STAT_VM86 || CPU_STAT_CPL == 0) {
			CPU_WORKCLOCK(11);
			madr = get_ea(op);
			limit = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
			base = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr + 2);
			if (!CPU_INST_OP32) {
				base &= 0x00ffffff;
			}

#if defined(MORE_DEBUG)
			gdtr_dump(base, limit);
#endif

			CPU_GDTR_BASE = base;
			CPU_GDTR_LIMIT = limit;
			return;
		}
		VERBOSE(("LGDT: VM86(%s) or CPL(%d) != 0", CPU_STAT_VM86 ? "true" : "false", CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, 0);
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

void
SGDT_Ms(DWORD op)
{
	DWORD madr;
	DWORD base;
	WORD limit;

	if (op < 0xc0) {
		CPU_WORKCLOCK(11);
		limit = CPU_GDTR_LIMIT;
		base = CPU_GDTR_BASE;
		if (!CPU_INST_OP32) {
			base &= 0x00ffffff;
		}
		madr = get_ea(op);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, limit);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr + 2, base);
		return;
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

void
LLDT_Ew(DWORD op)
{
	DWORD src, madr;

	if (CPU_STAT_PM && !CPU_STAT_VM86) {
		if (CPU_STAT_CPL == 0) {
			if (op >= 0xc0) {
				CPU_WORKCLOCK(5);
				src = *(reg16_b20[op]);
			} else {
				CPU_WORKCLOCK(11);
				madr = calc_ea_dst(op);
				src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
			}
			load_ldtr(src, GP_EXCEPTION);
			return;
		}
		VERBOSE(("LLDT: CPL(%d) != 0", CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, 0);
	}
	VERBOSE(("LLDT: VM86"));
	EXCEPTION(UD_EXCEPTION, 0);
}

void
SLDT_Ew(DWORD op)
{
	DWORD madr;
	WORD ldtr;

	if (CPU_STAT_PM && !CPU_STAT_VM86) {
		ldtr = CPU_LDTR;
		if (op >= 0xc0) {
			CPU_WORKCLOCK(5);
			if (CPU_INST_OP32) {
				*(reg32_b20[op]) = ldtr;
			} else {
				*(reg16_b20[op]) = ldtr;
			}
		} else {
			CPU_WORKCLOCK(11);
			madr = calc_ea_dst(op);
			cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, ldtr);
		}
		return;
	}
	VERBOSE(("SLDT: VM86"));
	EXCEPTION(UD_EXCEPTION, 0);
}

void
LTR_Ew(DWORD op)
{
	DWORD src, madr;

	if (CPU_STAT_PM && !CPU_STAT_VM86) {
		if (CPU_STAT_CPL == 0) {
			if (op >= 0xc0) {
				CPU_WORKCLOCK(5);
				src = *(reg16_b20[op]);
			} else {
				CPU_WORKCLOCK(11);
				madr = calc_ea_dst(op);
				src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
			}
			load_tr(src);
			return;
		}
		VERBOSE(("LTR: CPL(%d) != 0", CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, 0);
	}
	VERBOSE(("LTR: VM86"));
	EXCEPTION(UD_EXCEPTION, 0);
}

void
STR_Ew(DWORD op)
{
	DWORD madr;
	WORD tr;

	if (CPU_STAT_PM && !CPU_STAT_VM86) {
		tr = CPU_TR;
		if (op >= 0xc0) {
			CPU_WORKCLOCK(5);
			if (CPU_INST_OP32) {
				*(reg32_b20[op]) = tr;
			} else {
				*(reg16_b20[op]) = tr;
			}
		} else {
			CPU_WORKCLOCK(11);
			madr = calc_ea_dst(op);
			cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, tr);
		}
		return;
	}
	VERBOSE(("STR: VM86"));
	EXCEPTION(UD_EXCEPTION, 0);
}

void
LIDT_Ms(DWORD op)
{
	DWORD madr;
	DWORD base;
	WORD limit;

	if (op < 0xc0) {
		if (!CPU_STAT_PM || !CPU_STAT_VM86 || CPU_STAT_CPL == 0) {
			CPU_WORKCLOCK(11);
			madr = get_ea(op);
			limit = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
			base = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr + 2);
			if (!CPU_INST_OP32) {
				base &= 0x00ffffff;
			}

#if defined(MORE_DEBUG)
			idtr_dump(base, limit);
#endif

			CPU_IDTR_BASE = base;
			CPU_IDTR_LIMIT = limit;
			return;
		}
		VERBOSE(("LIDT: VM86(%s) or CPL(%d) != 0", CPU_STAT_VM86 ? "true" : "false", CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, 0);
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

void
SIDT_Ms(DWORD op)
{
	DWORD madr;
	DWORD base;
	WORD limit;

	if (op < 0xc0) {
		CPU_WORKCLOCK(11);
		limit = CPU_IDTR_LIMIT;
		base = CPU_IDTR_BASE;
		if (!CPU_INST_OP32) {
			base &= 0x00ffffff;
		}
		madr = get_ea(op);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, limit);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, madr + 2, base);
		return;
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

void
MOV_CdRd(void)
{
	DWORD op, src;
	DWORD reg;
	int idx;

	CPU_WORKCLOCK(11);
	GET_PCBYTE(op);
	if (op >= 0xc0) {
		if (CPU_STAT_PM && (CPU_STAT_VM86 || CPU_STAT_CPL != 0)) {
			VERBOSE(("MOV_CdRd: VM86(%s) or CPL(%d) != 0", CPU_STAT_VM86 ? "true" : "false", CPU_STAT_CPL));
			EXCEPTION(GP_EXCEPTION, 0);
		}

		src = *(reg32_b20[op]);
		idx = (op >> 3) & 7;

		switch (idx) {
		case 0: /* CR0 */
			/*
			 * 0 = PE (protect enable)
			 * 1 = MP (monitor coprocesser)
			 * 2 = EM (emulation)
			 * 3 = TS (task switch)
			 * 4 = ET (extend type, FPU present = 1)
			 * 5 = NE (numeric error)
			 * 16 = WP (write protect)
			 * 18 = AM (alignment mask)
			 * 29 = NW (not write-through)
			 * 30 = CD (cache diable)
			 * 31 = PG (pageing)
			 */

			/* 下巻 p.182 割り込み 13 - 一般保護例外 */
			if ((src & (CPU_CR0_PE|CPU_CR0_PG)) == (DWORD)CPU_CR0_PG) {
				EXCEPTION(GP_EXCEPTION, 0);
			}
			if ((src & (CPU_CR0_NW|CPU_CR0_CD)) == CPU_CR0_NW) {
				EXCEPTION(GP_EXCEPTION, 0);
			}

			reg = CPU_CR0;
			src &= 0xe005003f;
#ifndef USE_FPU
			src &= ~CPU_CR0_ET;	/* FPU not present */
#else
			src |= CPU_CR0_ET;	/* FPU present */
#endif
			CPU_CR0 = src;
			VERBOSE(("MOV_CdRd: %04x:%08x: cr0: 0x%08x <- 0x%08x(%s)", CPU_CS, CPU_PREV_EIP, reg, CPU_CR0, reg32_str[op & 7]));

			if ((reg ^ CPU_CR0) & (CPU_CR0_PE|CPU_CR0_PG)) {
				tlb_flush(FALSE);
			}
			if ((reg ^ CPU_CR0) & CPU_CR0_PE) {
				if (CPU_CR0 & CPU_CR0_PE) {
					change_pm(1);
				}
			}
			if ((reg ^ CPU_CR0) & CPU_CR0_PG) {
				if (CPU_CR0 & CPU_CR0_PG) {
					change_pg(1);
				} else {
					change_pg(0);
				}
			}
			if ((reg ^ CPU_CR0) & CPU_CR0_PE) {
				if (!(CPU_CR0 & CPU_CR0_PE)) {
					change_pm(0);
				}
			}
			break;

		case 2: /* CR2 */
			reg = CPU_CR2;
			CPU_CR2 = src;	/* page fault linear address */
			VERBOSE(("MOV_CdRd: %04x:%08x: cr2: 0x%08x <- 0x%08x(%s)", CPU_CS, CPU_PREV_EIP, reg, CPU_CR2, reg32_str[op & 7]));
			break;

		case 3: /* CR3 */
			/*
			 * 31-12 = page directory base
			 * 4 = PCD (page level cache diable)
			 * 3 = PWT (page level write throgh)
			 */
			reg = CPU_CR3;
			set_CR3(src);
			VERBOSE(("MOV_CdRd: %04x:%08x: cr3: 0x%08x <- 0x%08x(%s)", CPU_CS, CPU_PREV_EIP, reg, CPU_CR3, reg32_str[op & 7]));
			break;

#if CPU_FAMILY >= 5
		case 4: /* CR4 */
			/*
			 * 10 = OSXMMEXCPT (support non masking exception by OS)
			 * 9 = OSFXSR (support FXSAVE, FXRSTOR by OS)
			 * 8 = PCE (performance monitoring counter enable)
			 * 7 = PGE (page global enable)
			 * 6 = MCE (machine check enable)
			 * 5 = PAE (physical address extention)
			 * 4 = PSE (page size extention)
			 * 3 = DE (debug extention)
			 * 2 = TSD (time stamp diable)
			 * 1 = PVI (protected mode virtual interrupt)
			 * 0 = VME (VM8086 mode extention)
			 */
			reg = 0;	/* allow */
			if (src & ~reg) {
				if (src & 0xfffffc00) {
					EXCEPTION(GP_EXCEPTION, 0);
				}
				ia32_warning("MOV_CdRd: CR4 <- 0x%08x", src);
			}

			reg = CPU_CR4;
			CPU_CR4 = src;
			VERBOSE(("MOV_CdRd: %04x:%08x: cr4: 0x%08x <- 0x%08x(%s)", CPU_CS, CPU_PREV_EIP, reg, CPU_CR4, reg32_str[op & 7]));

			if ((reg ^ CPU_CR4) & (CPU_CR4_PSE|CPU_CR4_PGE|CPU_CR4_PAE)) {
				tlb_flush(FALSE);
			}
			break;
#endif	/* CPU_FAMILY >= 5 */

		default:
			ia32_panic("MOV_CdRd: CR reg index (%d)", idx);
			/*NOTREACHED*/
			break;
		}
		return;
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

void
MOV_RdCd(void)
{
	DWORD *out;
	DWORD op;
	int idx;

	CPU_WORKCLOCK(11);
	GET_PCBYTE(op);
	if (op >= 0xc0) {
		if (CPU_STAT_PM && (CPU_STAT_VM86 || CPU_STAT_CPL != 0)) {
			VERBOSE(("MOV_CdRd: VM86(%s) or CPL(%d) != 0", CPU_STAT_VM86 ? "true" : "false", CPU_STAT_CPL));
			EXCEPTION(GP_EXCEPTION, 0);
		}

		out = reg32_b20[op];
		idx = (op >> 3) & 7;

		switch (idx) {
		case 0:
			*out = CPU_CR0;
			break;

		case 2:
			*out = CPU_CR2;
			break;

		case 3:
			*out = CPU_CR3;
			break;

#if CPU_FAMILY >= 5
		case 4:
			*out = CPU_CR4;
			break;
#endif	/* CPU_FAMILY >= 5 */

		default:
			ia32_panic("MOV_RdCd: CR reg index (%d)", idx);
			/*NOTREACHED*/
			break;
		}
		VERBOSE(("MOV_RdCd: %04x:%08x: cr%d: 0x%08x -> %s", CPU_CS, CPU_PREV_EIP, idx, *out, reg32_str[op & 7]));
		return;
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

void
LMSW_Ew(DWORD op)
{
	DWORD src, madr;
	DWORD cr0;

	if (!CPU_STAT_PM || CPU_STAT_CPL == 0) {
		if (op >= 0xc0) {
			CPU_WORKCLOCK(2);
			src = *(reg16_b20[op]);
		} else {
			CPU_WORKCLOCK(3);
			madr = calc_ea_dst(op);
			src = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		}

		cr0 = CPU_CR0;
		CPU_CR0 &= ~0xe; /* can't switch back from protected mode */
		CPU_CR0 |= (src & 0xf);	/* TS, EM, MP, PE */
		if (!(cr0 & CPU_CR0_PE) && (src & CPU_CR0_PE)) {
			change_pm(1);	/* switch to protected mode */
		}
		return;
	}
	VERBOSE(("LMSW: CPL(%d) != 0", CPU_STAT_CPL));
	EXCEPTION(GP_EXCEPTION, 0);
}

void
SMSW_Ew(DWORD op)
{
	DWORD madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg16_b20[op]) = (WORD)CPU_CR0;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, (WORD)CPU_CR0);
	}
}

void
CLTS(void)
{

	CPU_WORKCLOCK(2);
	if (CPU_STAT_PM && (CPU_STAT_VM86 || (CPU_STAT_CPL != 0))) {
		VERBOSE(("CLTS: VM86(%s) or CPL(%d) != 0", CPU_STAT_VM86 ? "true" : "false", CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, 0);
	}
	CPU_CR0 &= ~CPU_CR0_TS;
}

void
ARPL_EwGw(void)
{
	DWORD op, src, dst, madr;

	if (CPU_STAT_PM && !CPU_STAT_VM86) {
		PREPART_EA_REG16(op, src);
		if (op >= 0xc0) {
			CPU_WORKCLOCK(2);
			dst = *(reg16_b20[op]);
			if ((dst & 3) < (src & 3)) {
				CPU_FLAGL |= Z_FLAG;
				dst &= ~3;
				dst |= (src & 3);
				*(reg16_b20[op]) = dst;
			} else {
				CPU_FLAGL &= ~Z_FLAG;
			}
		} else {
			CPU_WORKCLOCK(3);
			madr = calc_ea_dst(op);
			dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
			if ((dst & 3) < (src & 3)) {
				CPU_FLAGL |= Z_FLAG;
				dst &= ~3;
				dst |= (src & 3);
				cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, madr, dst);
			} else {
				CPU_FLAGL &= ~Z_FLAG;
			}
		}
		return;
	}
	VERBOSE(("ARPL: VM86"));
	EXCEPTION(UD_EXCEPTION, 0);
}

/*
 * DPL
 */
void
LAR_GwEw(void)
{
	selector_t sel;
	WORD *out;
	DWORD op;
	int rv;
	WORD selector;

	if (CPU_STAT_PM && !CPU_STAT_VM86) {
		PREPART_REG16_EA(op, selector, out, 5, 11);

		rv = parse_selector(&sel, selector);
		if (rv < 0) {
			CPU_FLAGL &= ~Z_FLAG;
			return;
		}

		if (sel.desc.s) {
			if (!(sel.desc.u.seg.c && !sel.desc.u.seg.ec) &&
			    ((sel.desc.dpl < CPU_STAT_CPL) || (sel.desc.dpl < sel.rpl))) {
				CPU_FLAGL &= ~Z_FLAG;
				return;
			}
		} else {
			switch (sel.desc.type) {
			case CPU_SYSDESC_TYPE_TSS_16:
			case CPU_SYSDESC_TYPE_LDT:
			case CPU_SYSDESC_TYPE_TSS_BUSY_16:
			case CPU_SYSDESC_TYPE_CALL_16:
			case CPU_SYSDESC_TYPE_TASK:
			case CPU_SYSDESC_TYPE_TSS_32:
			case CPU_SYSDESC_TYPE_TSS_BUSY_32:
			case CPU_SYSDESC_TYPE_CALL_32:
				break;

			default:
				CPU_FLAGL &= ~Z_FLAG;
				return;
			}
		}

		*out = sel.desc.h & 0xff00;
		CPU_FLAGL |= Z_FLAG;
		return;
	}
	VERBOSE(("LAR: VM86"));
	EXCEPTION(UD_EXCEPTION, 0);
}

void
LAR_GdEw(void)
{
	selector_t sel;
	DWORD *out;
	DWORD op;
	int rv;
	WORD selector;

	if (CPU_STAT_PM && !CPU_STAT_VM86) {
		PREPART_REG32_EA(op, selector, out, 5, 11);

		rv = parse_selector(&sel, selector);
		if (rv < 0) {
			CPU_FLAGL &= ~Z_FLAG;
			return;
		}

		if (sel.desc.s) {
			if (!(sel.desc.u.seg.c && !sel.desc.u.seg.ec)
			 && ((sel.desc.dpl < CPU_STAT_CPL) || (sel.desc.dpl < sel.rpl))) {
				CPU_FLAGL &= ~Z_FLAG;
				return;
			}
		} else {
			switch (sel.desc.type) {
			case CPU_SYSDESC_TYPE_TSS_16:
			case CPU_SYSDESC_TYPE_LDT:
			case CPU_SYSDESC_TYPE_TSS_BUSY_16:
			case CPU_SYSDESC_TYPE_CALL_16:
			case CPU_SYSDESC_TYPE_TASK:
			case CPU_SYSDESC_TYPE_TSS_32:
			case CPU_SYSDESC_TYPE_TSS_BUSY_32:
			case CPU_SYSDESC_TYPE_CALL_32:
				break;

			default:
				CPU_FLAGL &= ~Z_FLAG;
				return;
			}
		}

		*out = sel.desc.h & 0x00ffff00;	/* 0x00fxff00, x? */
		CPU_FLAGL |= Z_FLAG;
		return;
	}
	VERBOSE(("LAR: VM86"));
	EXCEPTION(UD_EXCEPTION, 0);
}

void
LSL_GwEw(void)
{
	selector_t sel;
	WORD *out;
	DWORD op;
	int rv;
	WORD selector;

	if (CPU_STAT_PM && !CPU_STAT_VM86) {
		PREPART_REG16_EA(op, selector, out, 5, 11);

		rv = parse_selector(&sel, selector);
		if (rv < 0) {
			CPU_FLAGL &= ~Z_FLAG;
			return;
		}

		if (sel.desc.s) {
			if (!(sel.desc.u.seg.c && !sel.desc.u.seg.ec)
			 && ((sel.desc.dpl < CPU_STAT_CPL) || (sel.desc.dpl < sel.rpl))) {
				CPU_FLAGL &= ~Z_FLAG;
				return;
			}
		} else {
			switch (sel.desc.type) {
			case CPU_SYSDESC_TYPE_TSS_16:
			case CPU_SYSDESC_TYPE_LDT:
			case CPU_SYSDESC_TYPE_TSS_BUSY_16:
			case CPU_SYSDESC_TYPE_TSS_32:
			case CPU_SYSDESC_TYPE_TSS_BUSY_32:
				break;

			default:
				CPU_FLAGL &= ~Z_FLAG;
				return;
			}
		}

		*out = (WORD)sel.desc.u.seg.limit;
		CPU_FLAGL |= Z_FLAG;
		return;
	}
	VERBOSE(("LSL: VM86"));
	EXCEPTION(UD_EXCEPTION, 0);
}

void
LSL_GdEw(void)
{
	selector_t sel;
	DWORD *out;
	DWORD op;
	int rv;
	WORD selector;

	if (CPU_STAT_PM && !CPU_STAT_VM86) {
		PREPART_REG32_EA(op, selector, out, 5, 11);

		rv = parse_selector(&sel, selector);
		if (rv < 0) {
			CPU_FLAGL &= ~Z_FLAG;
			return;
		}

		if (sel.desc.s) {
			if (!(sel.desc.u.seg.c && !sel.desc.u.seg.ec)
			 && ((CPU_STAT_CPL > sel.desc.dpl) || (sel.rpl > sel.desc.dpl))) {
				CPU_FLAGL &= ~Z_FLAG;
				return;
			}
		} else {
			switch (sel.desc.type) {
			case CPU_SYSDESC_TYPE_TSS_16:
			case CPU_SYSDESC_TYPE_LDT:
			case CPU_SYSDESC_TYPE_TSS_BUSY_16:
			case CPU_SYSDESC_TYPE_TSS_32:
			case CPU_SYSDESC_TYPE_TSS_BUSY_32:
				break;

			default:
				CPU_FLAGL &= ~Z_FLAG;
				return;
			}
		}

		*out = sel.desc.u.seg.limit;
		CPU_FLAGL |= Z_FLAG;
		return;
	}
	VERBOSE(("LSL: VM86"));
	EXCEPTION(UD_EXCEPTION, 0);
}

void
VERR_Ew(DWORD op)
{
	selector_t sel;
	DWORD madr;
	int rv;
	WORD selector;

	if (CPU_STAT_PM && !CPU_STAT_VM86) {
		if (op >= 0xc0) {
			CPU_WORKCLOCK(5);
			selector = *(reg16_b20[op]);
		} else {
			CPU_WORKCLOCK(11);
			madr = calc_ea_dst(op);
			selector = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		}

		rv = parse_selector(&sel, selector);
		if (rv < 0) {
			CPU_FLAGL &= ~Z_FLAG;
			return;
		}

		/* system segment */
		if (!sel.desc.s) {
			CPU_FLAGL &= ~Z_FLAG;
			return;
		}
		/* non-conforming code segment && (CPL > DPL || RPL > DPL) */
		if ((sel.desc.u.seg.c && !sel.desc.u.seg.ec)
		 && ((CPU_STAT_CPL > sel.desc.dpl) || (sel.rpl > sel.desc.dpl))) {
			CPU_FLAGL &= ~Z_FLAG;
			return;
		}
		/* code segment is not readable */
		if (sel.desc.u.seg.c && !sel.desc.u.seg.wr) {
			CPU_FLAGL &= ~Z_FLAG;
			return;
		}
		CPU_FLAGL |= Z_FLAG;
		return;
	}
	VERBOSE(("VERR: VM86"));
	EXCEPTION(UD_EXCEPTION, 0);
}

void
VERW_Ew(DWORD op)
{
	selector_t sel;
	DWORD madr;
	int rv;
	WORD selector;

	if (CPU_STAT_PM && !CPU_STAT_VM86) {
		if (op >= 0xc0) {
			CPU_WORKCLOCK(5);
			selector = *(reg16_b20[op]);
		} else {
			CPU_WORKCLOCK(11);
			madr = calc_ea_dst(op);
			selector = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		}

		rv = parse_selector(&sel, selector);
		if (rv < 0) {
			CPU_FLAGL &= ~Z_FLAG;
			return;
		}

		/* system segment || code segment */
		if (!sel.desc.s || sel.desc.u.seg.c) {
			CPU_FLAGL &= ~Z_FLAG;
			return;
		}
		/* data segment is not writable */
		if (!sel.desc.u.seg.wr) {
			CPU_FLAGL &= ~Z_FLAG;
			return;
		}
		/* privilege level */
		if ((CPU_STAT_CPL > sel.desc.dpl) || (sel.rpl > sel.desc.dpl)) {
			CPU_FLAGL &= ~Z_FLAG;
			return;
		}
		CPU_FLAGL |= Z_FLAG;
		return;
	}
	VERBOSE(("VERW: VM86"));
	EXCEPTION(UD_EXCEPTION, 0);
}

void
MOV_DdRd(void)
{

	ia32_panic("MOV_DdRd: not implemented yet!");
}

void
MOV_RdDd(void)
{

	ia32_panic("MOV_DdRd: not implemented yet!");
}

void
INVD(void)
{

	CPU_WORKCLOCK(11);
	if (CPU_STAT_PM && (CPU_STAT_VM86 || CPU_STAT_CPL != 0)) {
		VERBOSE(("INVD: VM86(%s) or CPL(%d) != 0", CPU_STAT_VM86 ? "true" : "false", CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, 0);
	}
	/* nothing to do */
}

void
WBINVD(void)
{

	CPU_WORKCLOCK(11);
	if (CPU_STAT_PM && (CPU_STAT_VM86 || CPU_STAT_CPL != 0)) {
		VERBOSE(("WBINVD: VM86(%s) or CPL(%d) != 0", CPU_STAT_VM86 ? "true" : "false", CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, 0);
	}
	/* nothing to do */
}

void
INVLPG(DWORD op)
{
	DWORD madr;

	if (CPU_STAT_PM && (CPU_STAT_VM86 || CPU_STAT_CPL != 0)) {
		VERBOSE(("INVLPG: VM86(%s) or CPL(%d) != 0", CPU_STAT_VM86 ? "true" : "false", CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, 0);
	}

	if (op < 0xc0) {
		CPU_WORKCLOCK(11);
		madr = get_ea(op);
		tlb_flush_page(madr);
		return;
	}
	EXCEPTION(UD_EXCEPTION, 0);
}

void
_LOCK(void)
{

	/* Nothing to do */
}

void
HLT(void)
{

	if (CPU_STAT_PM && CPU_STAT_CPL != 0) {
		VERBOSE(("HLT: CPL(%d) != 0", CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, 0);
	}

	CPU_HALT();
	CPU_EIP--;
}

void
RSM(void)
{

	ia32_panic("RSM: not implemented yet!");
}

void
RDMSR(void)
{
	int idx;

	if (CPU_STAT_PM && (CPU_STAT_VM86 || CPU_STAT_CPL != 0)) {
		VERBOSE(("RDMSR: VM86(%s) or CPL(%d) != 0", CPU_STAT_VM86 ? "true" : "false", CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, 0);
	}

	idx = CPU_ECX;
	switch (idx) {
	default:
		EXCEPTION(GP_EXCEPTION, 0);
		break;
	}
}

void
WRMSR(void)
{
	int idx;

	if (CPU_STAT_PM && (CPU_STAT_VM86 || CPU_STAT_CPL != 0)) {
		VERBOSE(("WRMSR: VM86(%s) or CPL(%d) != 0", CPU_STAT_VM86 ? "true" : "false", CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, 0);
	}

	idx = CPU_ECX;
	switch (idx) {
		/* MTRR への書き込み時 tlb_flush(FALSE); */

	default:
		EXCEPTION(GP_EXCEPTION, 0);
		break;
	}
}

void
RDTSC(void)
{

	ia32_panic("RDTSC: not implemented yet!");
}

void
MOV_TdRd(void)
{

	ia32_panic("MOV_TdRd: not implemented yet!");
}

void
MOV_RdTd(void)
{

	ia32_panic("MOV_RdTd: not implemented yet!");
}

/*	$Id: exception.c,v 1.5 2004/01/26 15:23:55 monaka Exp $	*/

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

const char *exception_str[EXCEPTION_NUM] = {
	"DE_EXCEPTION",
	"DB_EXCEPTION",
	"NMI_EXCEPTION",
	"BP_EXCEPTION",
	"OF_EXCEPTION",
	"BR_EXCEPTION",
	"UD_EXCEPTION",
	"NM_EXCEPTION",
	"DF_EXCEPTION",
	"CoProcesser Segment Overrun",
	"TS_EXCEPTION",
	"NP_EXCEPTION",
	"SS_EXCEPTION",
	"GP_EXCEPTION",
	"PF_EXCEPTION",
	"Reserved",
	"MF_EXCEPTION",
	"AC_EXCEPTION",
	"MC_EXCEPTION",
	"XF_EXCEPTION",
};

static const int exctype[EXCEPTION_NUM] = {
	1, 0, 0, 0, 0, 0, 0, 0, 3, 0, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0,
};

static const int dftable[4][4] = {
	{ 0, 0, 0, 1, },
	{ 0, 1, 0, 1, },
	{ 0, 1, 1, 1, },
	{ 1, 1, 1, 1, },
};

void
exception(int num, int error_code)
{
	int errorp = 0;

	__ASSERT((unsigned int)num < EXCEPTION_NUM);

	VERBOSE(("exception: %s, error_code = %x at %04x:%08x", exception_str[num], error_code, CPU_CS, CPU_PREV_EIP));
	VERBOSE(("exception:------------------------------------------------"));
	VERBOSE(("%s", cpu_reg2str()));
	VERBOSE(("exception:------------------------------------------------"));

	CPU_STAT_NERROR++;
	if ((CPU_STAT_NERROR >= 3) 
	 || (CPU_STAT_NERROR == 2 && CPU_STAT_PREV_EXCEPTION == DF_EXCEPTION)) {
		/* Triple fault */
		ia32_panic("exception: catch triple fault!");
	}

	switch (num) {
	case DE_EXCEPTION:	/* (F) 除算エラー */
	case BR_EXCEPTION:	/* (F) BOUND の範囲外 */
	case UD_EXCEPTION:	/* (F) 無効オペコード */
	case NM_EXCEPTION:	/* (F) デバイス使用不可 (FPU が無い) */
		CPU_EIP = CPU_PREV_EIP;
		/*FALLTHROUGH*/
	case DB_EXCEPTION:	/* (F/T) デバッグ */
	case NMI_EXCEPTION:	/* (I) NMI 割り込み */
	case BP_EXCEPTION:	/* (T) ブレークポイント */
	case OF_EXCEPTION:	/* (T) オーバーフロー */
		errorp = 0;
		break;

	case DF_EXCEPTION:	/* (A) ダブルフォルト (errcode: 0) */
		errorp = 1;
		error_code = 0;
		break;

	case AC_EXCEPTION:	/* (F) アラインメントチェック (errcode: 0) */
		error_code = 0;
		/*FALLTHROUGH*/
	case TS_EXCEPTION:	/* (F) 無効 TSS (errcode) */
	case NP_EXCEPTION:	/* (F) セグメント不在 (errcode) */
	case SS_EXCEPTION:	/* (F) スタックセグメントフォルト (errcode) */
	case GP_EXCEPTION:	/* (F) 一般保護例外 (errcode) */
	case PF_EXCEPTION:	/* (F) ページフォルト (errcode) */
		CPU_EIP = CPU_PREV_EIP;
		errorp = 1;
		break;

	case MF_EXCEPTION:	/* (F) 浮動小数点エラー */
		CPU_EIP = CPU_PREV_EIP;
		errorp = 0;
		break;

	case MC_EXCEPTION:	/* (A) マシンチェック */
		CPU_EIP = CPU_PREV_EIP;
		errorp = 0;
		break;

	case XF_EXCEPTION:	/* (F) ストリーミング SIMD 拡張命令 */
		CPU_EIP = CPU_PREV_EIP;
		errorp = 0;
		break;

	default:
		ia32_panic("exception: unknown exception (%d)", num);
		break;
	}

	if (CPU_STAT_NERROR >= 2) {
		if (dftable[exctype[CPU_STAT_PREV_EXCEPTION]][exctype[num]]) {
			num = DF_EXCEPTION;
		}
	}
	CPU_STAT_PREV_EXCEPTION = num;

	INTERRUPT(num, FALSE, errorp, error_code);
	siglongjmp(exec_1step_jmpbuf, 1);
}

/*
 * コール・ゲート・ディスクリプタ
 *
 *  31                                16 15 14 13 12       8 7   5 4       0
 * +------------------------------------+--+-----+----------+-----+---------+
 * |         オフセット 31..16          | P| DPL | 0 D 1 0 0|0 0 0|カウント | 4
 * +------------------------------------+--+-----+----------+-----+---------+
 *  31                                16 15                                0
 * +------------------------------------+-----------------------------------+
 * |        セグメント・セレクタ        |          オフセット 15..0         | 0
 * +------------------------------------+-----------------------------------+
 */

/*
 * 割り込みディスクリプタ
 *--
 * タスク・ゲート
 *
 *  31                                16 15 14 13 12       8 7             0
 * +------------------------------------+--+-----+----------+---------------+
 * |              Reserved              | P| DPL | 0 0 1 0 1|   Reserved    | 4
 * +------------------------------------+--+-----+----------+---------------+
 *  31                                16 15                                0
 * +------------------------------------+-----------------------------------+
 * |      TSS セグメント・セレクタ      |              Reserved             | 0
 * +------------------------------------+-----------------------------------+
 *--
 * 割り込み・ゲート
 *
 *  31                                16 15 14 13 12       8 7   5 4       0
 * +------------------------------------+--+-----+----------+-----+---------+
 * |         オフセット 31..16          | P| DPL | 0 D 1 1 0|0 0 0|Reserved | 4
 * +------------------------------------+--+-----+----------+-----+---------+
 *  31                                16 15                                0
 * +------------------------------------+-----------------------------------+
 * |        セグメント・セレクタ        |          オフセット 15..0         | 0
 * +------------------------------------+-----------------------------------+
 *--
 * トラップ・ゲート
 *
 *  31                                16 15 14 13 12       8 7   5 4       0
 * +------------------------------------+--+-----+----------+-----+---------+
 * |         オフセット 31..16          | P| DPL | 0 D 1 1 1|0 0 0|Reserved | 4
 * +------------------------------------+--+-----+----------+-----+---------+
 *  31                                16 15                                0
 * +------------------------------------+-----------------------------------+
 * |        セグメント・セレクタ        |          オフセット 15..0         | 0
 * +------------------------------------+-----------------------------------+
 *--
 * DPL        : ディスクリプタ特権レベル
 * オフセット : プロシージャ・エントリ・ポイントまでのオフセット
 * P          : セグメント存在フラグ
 * セレクタ   : ディスティネーション・コード・セグメントのセグメント・セレクタ
 * D          : ゲートのサイズ．0 = 16 bit, 1 = 32 bit
 */

static void interrupt_task(descriptor_t *gdp, int softintp, int errorp, int error_code);
static void interrupt_intr_or_trap(descriptor_t *gdp, int softintp, int errorp, int error_code);

void
interrupt(int num, int softintp, int errorp, int error_code)
{
	descriptor_t gd;
	DWORD idt_idx;
	DWORD new_ip;
	WORD new_cs;

	VERBOSE(("interrupt: num = 0x%02x, softintp = %s, errorp = %s, error_code = %02x", num, softintp ? "on" : "off", errorp ? "on" : "off", error_code));

	if (!CPU_STAT_PM) {
		/* real mode */
		idt_idx = num * 4;
		if (idt_idx + 3 > CPU_IDTR_LIMIT) {
			VERBOSE(("interrupt: real-mode IDTR limit check failure (idx = 0x%04x, limit = 0x%08x", idt_idx, CPU_IDTR_LIMIT));
			EXCEPTION(GP_EXCEPTION, num * 4 | 2);
		}

		if (!softintp) {
			BYTE op = cpu_codefetch(CPU_IP);
			if (op == 0xf4)	{	/* hlt */
				CPU_EIP++;
			}
		}

		REGPUSH0(REAL_FLAGREG);
		REGPUSH0(CPU_CS);
		REGPUSH0(CPU_IP);

		CPU_EFLAG &= ~(T_FLAG | I_FLAG | AC_FLAG | RF_FLAG);
		CPU_TRAP = 0;

		new_ip = cpu_memoryread_w(CPU_IDTR_BASE + num * 4);
		new_cs = cpu_memoryread_w(CPU_IDTR_BASE + num * 4 + 2);
		CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
		SET_EIP(new_ip);
		CPU_WORKCLOCK(20);
	} else {
		/* protected mode */

		/* VM86 && IOPL < 3 && interrupt cause == INTn */
		if (CPU_STAT_VM86 && (CPU_STAT_IOPL < CPU_IOPL3) && (softintp == -1)) {
			VERBOSE(("interrupt: VM86 && IOPL < 3 && INTn"));
			EXCEPTION(GP_EXCEPTION, 0);
		}

		idt_idx = num * 8;
		if (idt_idx + 7 > CPU_IDTR_LIMIT) {
			VERBOSE(("interrupt: IDTR limit check failure (idx = 0x%04x, limit = 0x%08x", idt_idx, CPU_IDTR_LIMIT));
			EXCEPTION(GP_EXCEPTION, num * 8 | 2 | !softintp);
		}

		memset(&gd, 0, sizeof(gd));
		CPU_SET_GATEDESC(&gd, CPU_IDTR_BASE + idt_idx);
		if (!gd.valid || !gd.p) {
			VERBOSE(("interrupt: gate descripter is invalid."));
			EXCEPTION(GP_EXCEPTION, num * 8 | 2 | !softintp);
		}

		switch (gd.type) {
		case CPU_SYSDESC_TYPE_TASK:
		case CPU_SYSDESC_TYPE_INTR_16:
		case CPU_SYSDESC_TYPE_INTR_32:
		case CPU_SYSDESC_TYPE_TRAP_16:
		case CPU_SYSDESC_TYPE_TRAP_32:
			break;

		default:
			VERBOSE(("interrupt: invalid gate type (%d)", gd.type));
			EXCEPTION(GP_EXCEPTION, num * 8 | 2 | !softintp);
			break;
		}

		/* 5.10.1.1. 例外／割り込みハンドラ・プロシージャの保護 */
		if (softintp && (gd.dpl < CPU_STAT_CPL)) {
			VERBOSE(("interrupt: softintp && DPL(%d) < CPL(%d)", gd.dpl, CPU_STAT_CPL));
			EXCEPTION(GP_EXCEPTION, num * 8 | 2);
		}

		switch (gd.type) {
		case CPU_SYSDESC_TYPE_TASK:
			interrupt_task(&gd, softintp, errorp, error_code);
			break;

		case CPU_SYSDESC_TYPE_INTR_16:
		case CPU_SYSDESC_TYPE_INTR_32:
		case CPU_SYSDESC_TYPE_TRAP_16:
		case CPU_SYSDESC_TYPE_TRAP_32:
			interrupt_intr_or_trap(&gd, softintp, errorp, error_code);
			break;

		default:
			EXCEPTION(GP_EXCEPTION, num * 8 | 2 | !softintp);
			break;
		}
	}
}

static void
interrupt_task(descriptor_t *gdp, int softintp, int errorp, int error_code)
{
	selector_t task_sel;
	int rv;

	VERBOSE(("interrupt: TASK-GATE"));

	(void)softintp;

	rv = parse_selector(&task_sel, gdp->u.gate.selector);
	if (rv < 0 || task_sel.ldt) {
		VERBOSE(("interrupt: parse_selector (selector = %04x, rv = %d, %cDT)", gdp->u.gate.selector, rv, task_sel.ldt ? 'L' : 'G'));
		EXCEPTION(TS_EXCEPTION, task_sel.idx);
	}

	/* check gate type */
	switch (task_sel.desc.type) {
	case CPU_SYSDESC_TYPE_TSS_16:
	case CPU_SYSDESC_TYPE_TSS_32:
		break;

	case CPU_SYSDESC_TYPE_TSS_BUSY_16:
	case CPU_SYSDESC_TYPE_TSS_BUSY_32:
		VERBOSE(("interrupt: task is busy."));
		/*FALLTHROUGH*/
	default:
		VERBOSE(("interrupt: invalid gate type (%d)", task_sel.desc.type));
		EXCEPTION(TS_EXCEPTION, task_sel.idx);
		break;
	}

	/* not present */
	if (selector_is_not_present(&task_sel)) {
		VERBOSE(("interrupt: selector is not present"));
		EXCEPTION(NP_EXCEPTION, task_sel.idx);
	}

	task_switch(&task_sel, TASK_SWITCH_INTR);

	if (errorp) {
		XPUSH0(error_code);
	}
}

static void
interrupt_intr_or_trap(descriptor_t *gdp, int softintp, int errorp, int error_code)
{
	selector_t intr_sel, ss_sel;
	DWORD flags = REAL_EFLAGREG;
	DWORD mask = 0;
	DWORD stacksize;
	DWORD new_ip, new_sp;
	DWORD old_ip, old_sp;
	WORD old_cs, old_ss, new_ss;
	int rv; 

	switch (gdp->type) {
	case CPU_SYSDESC_TYPE_INTR_16:
	case CPU_SYSDESC_TYPE_INTR_32:
		VERBOSE(("interrupt: INTERRUPT-GATE"));
		flags &= ~I_FLAG;
		mask |= I_FLAG;
		break;

	case CPU_SYSDESC_TYPE_TRAP_16:
	case CPU_SYSDESC_TYPE_TRAP_32:
		VERBOSE(("interrupt: TRAP-GATE"));
		break;
	}

	flags &= ~(T_FLAG|RF_FLAG|NT_FLAG|VM_FLAG);
	mask |= T_FLAG|RF_FLAG|NT_FLAG|VM_FLAG;

	new_ip = gdp->u.gate.offset;
	old_ss = CPU_SS;
	old_cs = CPU_CS;
	old_ip = CPU_EIP;
	old_sp = CPU_ESP;
	switch (gdp->type) {
	case CPU_SYSDESC_TYPE_INTR_16:
	case CPU_SYSDESC_TYPE_TRAP_16:
		old_ip &= 0xffff;
		old_sp &= 0xffff;
		break;
	}
	VERBOSE(("interrupt: old EIP = %04x:%08x, ESP = %04x:%08x", old_cs, old_ip, old_ss, old_sp));

	rv = parse_selector(&intr_sel, gdp->u.gate.selector);
	if (rv < 0) {
		VERBOSE(("interrupt: parse_selector (selector = %04x, rv = %d)", gdp->u.gate.selector, rv));
		EXCEPTION(GP_EXCEPTION, intr_sel.idx | !softintp);
	}

	/* check segment type */
	if (!intr_sel.desc.s) {
		VERBOSE(("interrupt: code segment is system segment"));
		EXCEPTION(GP_EXCEPTION, intr_sel.idx | !softintp);
	}
	if (!intr_sel.desc.u.seg.c) {
		VERBOSE(("interrupt: code segment is data segment"));
		EXCEPTION(GP_EXCEPTION, intr_sel.idx | !softintp);
	}

	/* check privilege level */
	if (intr_sel.desc.dpl > CPU_STAT_CPL) {
		VERBOSE(("interrupt: DPL(%d) > CPL(%d)", intr_sel.desc.dpl, CPU_STAT_CPL));
		EXCEPTION(GP_EXCEPTION, intr_sel.idx | !softintp);
	}

	/* not present */
	if (selector_is_not_present(&intr_sel)) {
		VERBOSE(("interrupt: selector is not present"));
		EXCEPTION(NP_EXCEPTION, intr_sel.idx | !softintp);
	}

	if (!intr_sel.desc.u.seg.ec
	 && (intr_sel.desc.dpl < CPU_STAT_CPL)) {
		if (!CPU_STAT_VM86) {
			VERBOSE(("interrupt: INTER-PRIVILEGE-LEVEL-INTERRUPT"));
			stacksize = errorp ? 12 : 10;
		} else {
			/* VM86 */
			if (intr_sel.desc.dpl != 0) {
				/* 16.3.1.1 */
				VERBOSE(("interrupt: DPL[CS](%d) != 0", intr_sel.desc.dpl));
				EXCEPTION(GP_EXCEPTION, intr_sel.idx);
			}
			VERBOSE(("interrupt: INTERRUPT-FROM-VIRTUAL-8086-MODE"));
			stacksize = errorp ? 20 : 18;
		}
		switch (gdp->type) {
		case CPU_SYSDESC_TYPE_INTR_32:
		case CPU_SYSDESC_TYPE_TRAP_32:
			stacksize *= 2;
			break;
		}

		get_stack_from_tss(intr_sel.desc.dpl, &new_ss, &new_sp);

		rv = parse_selector(&ss_sel, new_ss);
		if (rv < 0) {
			VERBOSE(("interrupt: parse_selector (selector = %04x, rv = %d)", new_ss, rv));
			EXCEPTION(TS_EXCEPTION, ss_sel.idx | !softintp);
		}

		/* check privilege level */
		if (ss_sel.rpl != intr_sel.desc.dpl) {
			VERBOSE(("interrupt: RPL[SS](%d) != DPL[CS](%d)", ss_sel.rpl, intr_sel.desc.dpl));
			EXCEPTION(TS_EXCEPTION, ss_sel.idx | !softintp);
		}
		if (ss_sel.desc.dpl != intr_sel.desc.dpl) {
			VERBOSE(("interrupt: DPL[SS](%d) != DPL[CS](%d)", ss_sel.desc.dpl, intr_sel.desc.dpl));
			EXCEPTION(TS_EXCEPTION, ss_sel.idx | !softintp);
		}

		/* check segment type */
		if (!ss_sel.desc.s) {
			VERBOSE(("interrupt: stack segment is system segment"));
			EXCEPTION(TS_EXCEPTION, ss_sel.idx | !softintp);
		}
		if (ss_sel.desc.u.seg.c) {
			VERBOSE(("interrupt: stack segment is code segment"));
			EXCEPTION(TS_EXCEPTION, ss_sel.idx | !softintp);
		}
		if (!ss_sel.desc.u.seg.wr) {
			VERBOSE(("interrupt: stack segment is read-only data segment"));
			EXCEPTION(TS_EXCEPTION, ss_sel.idx | !softintp);
		}

		/* not present */
		if (selector_is_not_present(&ss_sel)) {
			VERBOSE(("interrupt: selector is not present"));
			EXCEPTION(SS_EXCEPTION, ss_sel.idx | !softintp);
		}

		/* check stack room size */
		CHECK_STACK_PUSH(&ss_sel.desc, new_sp, stacksize);

		/* out of range */
		if (new_ip > intr_sel.desc.u.seg.limit) {
			VERBOSE(("interrupt: new_ip is out of range. new_ip = %08x, limit = %08x", new_ip, intr_sel.desc.u.seg.limit));
			EXCEPTION(GP_EXCEPTION, 0);
		}

		load_ss(new_ss, &ss_sel.desc, intr_sel.desc.dpl);
		CPU_ESP = new_sp;

		load_cs(intr_sel.selector, &intr_sel.desc, intr_sel.desc.dpl);
		SET_EIP(new_ip);

		if (CPU_STAT_VM86) {
			switch (gdp->type) {
			case CPU_SYSDESC_TYPE_INTR_32:
			case CPU_SYSDESC_TYPE_TRAP_32:
				PUSH0_32(CPU_GS);
				PUSH0_32(CPU_FS);
				PUSH0_32(CPU_DS);
				PUSH0_32(CPU_ES);
				break;

			case CPU_SYSDESC_TYPE_INTR_16:
			case CPU_SYSDESC_TYPE_TRAP_16:
				ia32_panic("interrupt: 16bit gate");
				break;
			}

			CPU_SET_SEGREG(CPU_GS_INDEX, 0);
			CPU_SET_SEGREG(CPU_FS_INDEX, 0);
			CPU_SET_SEGREG(CPU_DS_INDEX, 0);
			CPU_SET_SEGREG(CPU_ES_INDEX, 0);
		}

		switch (gdp->type) {
		case CPU_SYSDESC_TYPE_INTR_32:
		case CPU_SYSDESC_TYPE_TRAP_32:
			PUSH0_32(old_ss);
			PUSH0_32(old_sp);
			PUSH0_32(REAL_EFLAGREG);
			PUSH0_32(old_cs);
			PUSH0_32(old_ip);
			if (errorp) {
				PUSH0_32(error_code);
			}
			break;

		case CPU_SYSDESC_TYPE_INTR_16:
		case CPU_SYSDESC_TYPE_TRAP_16:
			PUSH0_16(old_ss);
			PUSH0_16(old_sp);
			PUSH0_16(REAL_FLAGREG);
			PUSH0_16(old_cs);
			PUSH0_16(old_ip);
			if (errorp) {
				PUSH0_16(error_code);
			}
			break;
		}

		set_eflags(flags, mask);
	} else {
		if (CPU_STAT_VM86) {
			VERBOSE(("interrupt: VM86"));
			EXCEPTION(GP_EXCEPTION, intr_sel.idx);
		}
		if (!intr_sel.desc.u.seg.ec && (intr_sel.desc.dpl != CPU_STAT_CPL)) {
			VERBOSE(("interrupt: NON-CONFORMING-CODE-SEGMENT(%s) and DPL[CS](%d) != CPL", intr_sel.desc.u.seg.ec ? "false" : "true", intr_sel.desc.dpl, CPU_STAT_CPL));
			EXCEPTION(GP_EXCEPTION, intr_sel.idx);
		}
		VERBOSE(("interrupt: INTRA-PRIVILEGE-LEVEL-INTERRUPT"));

		if (CPU_STAT_SS32) {
			new_sp = CPU_ESP;
		} else {
			new_sp = CPU_SP;
		}

		stacksize = errorp ? 8 : 6;
		switch (gdp->type) {
		case CPU_SYSDESC_TYPE_INTR_32:
		case CPU_SYSDESC_TYPE_TRAP_32:
			stacksize *= 2;
			break;
		}
		CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), new_sp, stacksize);

		/* out of range */
		if (new_ip > intr_sel.desc.u.seg.limit) {
			VERBOSE(("interrupt: new_ip is out of range. new_ip = %08x, limit = %08x", new_ip, intr_sel.desc.u.seg.limit));
			EXCEPTION(GP_EXCEPTION, 0);
		}

		switch (gdp->type) {
		case CPU_SYSDESC_TYPE_INTR_32:
		case CPU_SYSDESC_TYPE_TRAP_32:
			PUSH0_32(REAL_EFLAGREG);
			PUSH0_32(CPU_CS);
			PUSH0_32(CPU_EIP);
			if (errorp) {
				PUSH0_32(error_code);
			}
			break;

		case CPU_SYSDESC_TYPE_INTR_16:
		case CPU_SYSDESC_TYPE_TRAP_16:
			PUSH0_16(REAL_FLAGREG);
			PUSH0_16(CPU_CS);
			PUSH0_16(CPU_IP);
			if (errorp) {
				PUSH0_16(error_code);
			}
			break;
		}

		load_cs(intr_sel.selector, &intr_sel.desc, CPU_STAT_CPL);
		SET_EIP(new_ip);

		set_eflags(flags, mask);
	}

	VERBOSE(("interrupt: new EIP = %04x:%08x, new ESP = %04x:%08x", CPU_CS, CPU_EIP, CPU_SS, CPU_ESP));
}

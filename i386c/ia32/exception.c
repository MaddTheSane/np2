/*	$Id: exception.c,v 1.2 2003/12/08 02:09:17 yui Exp $	*/

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
memory_dump(int idx, DWORD madr)
{
	DWORD addr;
	size_t size;
	unsigned char buf[16];
	size_t s, i;
	BYTE p;

	if (madr < 0x80) {
		size = madr + 0x80;
		addr = 0;
	} else {
		size = 0x100;
		addr = madr - 0x80;
	}
	printf("memory dump\n-- \n");
	for (s = 0; s < size; s++) {
		if ((s % 16) == 0) {
			printf("%08x: ", addr + s);
			memset(buf, '.', sizeof(buf));
		}

		p = cpu_vmemoryread(idx, addr + s);
		printf("%02x ", p);
		if (p >= 0x20 && p <= 0x7e)
			buf[s % 16] = p;

		if ((s % 16) == 15) {
			printf("| ");
			for (i = 0; i < sizeof(buf); i++)
				printf("%c", buf[i]);
			printf("\n");
		}
	}
}

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

	VERBOSE(("exception: num = 0x%02x, error_code = %x", num, error_code));

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
		ia32_panic("exception(): unknown exception (%d)", num);
		break;
	}

	if (CPU_STAT_NERROR >= 2) {
		if (dftable[exctype[CPU_STAT_PREV_EXCEPTION]][exctype[num]]) {
			num = DF_EXCEPTION;
		}
	}
	CPU_STAT_PREV_EXCEPTION = num;

	INTERRUPT(num, FALSE, errorp, error_code);
	CPU_STAT_NERROR = 0;
#if defined(WIN32)
	longjmp(exec_1step_jmpbuf, 1);
#else
	siglongjmp(exec_1step_jmpbuf, 1);
#endif
}

/*
 * コール・ゲート・ディスクリプタ
 *
 *  31                                16 15 14 13 12       8 7   5 4       0
 * +------------------------------------+--+-----+----------+-----+---------+
 * |         オフセット 31..16          | P| DPL | 0 1 1 0 0|0 0 0|カウント | 4
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
 * |              Reserved              | P| DPL | 0 D 1 0 1|   Reserved    | 4
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

void
interrupt(int num, int softintp, int errorp, int error_code)
{
	DWORD idt_idx;

	VERBOSE(("interrupt: num = 0x%02x, softintp = %s, errorp = %s, error_code = %02x", num, softintp ? "on" : "off", errorp ? "on" : "off", error_code));

	if (!CPU_STAT_PM) {
		/* real mode */
		idt_idx = num * 4;
		if (idt_idx + 3 > CPU_IDTR_LIMIT) {
			EXCEPTION(GP_EXCEPTION, num * 4 | 2);
		}

		if (!softintp) {
			BYTE op = cpu_codefetch(CPU_EIP);
			if (op == 0xf4)	{	/* hlt */
				CPU_EIP++;
			}
		}

		REGPUSH0(REAL_FLAGREG);
		REGPUSH0(CPU_CS);
		REGPUSH0(CPU_IP);

		if (softintp) {
			CPU_EFLAG &= ~(T_FLAG | I_FLAG | AC_FLAG);
			CPU_TRAP = 0;
		}

		CPU_EIP = cpu_memoryread_w(CPU_IDTR_BASE + num * 4);
		CPU_CS = cpu_memoryread_w(CPU_IDTR_BASE + num * 4 + 2);
		CPU_SET_SEGREG(CPU_CS_INDEX, CPU_CS);
		CPU_WORKCLOCK(20);
	} else {
		/* protected mode */
		selector_t task_sel, intr_sel, ss_sel;
		descriptor_t gd;
		int rv;
		DWORD flags = REAL_EFLAGREG;
		DWORD mask = 0;
		DWORD new_ip, new_sp;
		DWORD old_ip, old_sp;
		WORD new_ss;
		WORD old_cs, old_ss;

		/* VM86 && IOPL < 3 && interrupt cause == INTn */
		if (CPU_STAT_VM86 && (CPU_STAT_IOPL < CPU_IOPL3) && (softintp == -1)) {
			EXCEPTION(GP_EXCEPTION, 0);
		}

		idt_idx = num * 8;
		if (idt_idx + 7 > CPU_IDTR_LIMIT) {
			EXCEPTION(GP_EXCEPTION, num * 8 | 2 | !softintp);
		}

		CPU_SET_GATEDESC(&gd, CPU_IDTR_BASE + idt_idx);
		if (!gd.valid || !gd.p) {
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
			EXCEPTION(GP_EXCEPTION, num * 8 | 2 | !softintp);
			break;
		}

		/* 5.10.1.1. 例外／割り込みハンドラ・プロシージャの保護 */
		if (softintp && (gd.dpl < CPU_STAT_CPL)) {
			EXCEPTION(GP_EXCEPTION, num * 8 | 2);
		}

		switch (gd.type) {
		case CPU_SYSDESC_TYPE_TASK:
			rv = parse_selector(&task_sel, gd.u.gate.selector);
			if (rv < 0 || task_sel.ldt) {
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
				EXCEPTION(TS_EXCEPTION, task_sel.idx);
				break;
			}

			/* not present */
			if (selector_is_not_present(&task_sel)) {
				EXCEPTION(NP_EXCEPTION, task_sel.idx);
			}

			task_switch(&task_sel, TASK_SWITCH_INTR);

			if (errorp) {
				if (task_sel.desc.type == CPU_SYSDESC_TYPE_TSS_32) {
					CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 4);
					PUSH0_32(error_code);
				} else {
					CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 2);
					PUSH0_16(error_code);
				}
			}

			/* out of range */
			if (CPU_EIP > CPU_STAT_CS_LIMIT) {
				EXCEPTION(GP_EXCEPTION, 0);
			}
			break;

		case CPU_SYSDESC_TYPE_INTR_16:
		case CPU_SYSDESC_TYPE_INTR_32:
			flags &= I_FLAG;
			mask |= I_FLAG;
			/*FALLTHROUGH*/
		case CPU_SYSDESC_TYPE_TRAP_16:
		case CPU_SYSDESC_TYPE_TRAP_32:
			flags &= ~(T_FLAG|RF_FLAG|NT_FLAG|VM_FLAG);
			mask |= T_FLAG|RF_FLAG|NT_FLAG|VM_FLAG;

			new_ip = intr_sel.desc.u.seg.segbase;
			old_ss = CPU_SS;
			old_cs = CPU_CS;
			old_ip = CPU_EIP;
			old_sp = CPU_ESP;

			VERBOSE(("TRAP-OR-INTERRUPT-GATE"));

			rv = parse_selector(&intr_sel, gd.u.gate.selector);
			if (rv < 0) {
				EXCEPTION(GP_EXCEPTION, intr_sel.idx | !softintp);
			}

			if (!intr_sel.desc.s
			 || !intr_sel.desc.u.seg.c
			 || (intr_sel.desc.dpl > CPU_STAT_CPL)) {
				EXCEPTION(GP_EXCEPTION, intr_sel.idx | !softintp);
			}

			/* not present */
			if (selector_is_not_present(&intr_sel)) {
				EXCEPTION(NP_EXCEPTION, intr_sel.idx | !softintp);
			}

			if (!intr_sel.desc.u.seg.ec && (intr_sel.desc.dpl < CPU_STAT_CPL)) {
				DWORD stacksize;

				if (CPU_STAT_VM86) {
					VERBOSE(("INTER-PRIVILEGE-LEVEL-INTERRUPT"));
					stacksize = errorp ? 12 : 10;
				} else {
					if (intr_sel.desc.dpl != 0) {
						EXCEPTION(GP_EXCEPTION, intr_sel.idx);
					}
					VERBOSE(("INTERRUPT-FROM-VIRTUAL-8086-MODE"));
					stacksize = errorp ? 20 : 18;
				}

				get_stack_from_tss(intr_sel.desc.dpl, &new_ss, &new_sp);
				rv = parse_selector(&ss_sel, new_ss);
				if (rv < 0) {
					EXCEPTION(TS_EXCEPTION, ss_sel.idx | !softintp);
				}

				if ((ss_sel.rpl != intr_sel.desc.dpl)
				 || (ss_sel.desc.dpl != intr_sel.desc.dpl)
				 || !ss_sel.desc.s
				 || ss_sel.desc.u.seg.c
				 || !ss_sel.desc.u.seg.wr) {
					EXCEPTION(TS_EXCEPTION, ss_sel.idx | !softintp);
				}

				/* not present */
				if (selector_is_not_present(&ss_sel)) {
					EXCEPTION(SS_EXCEPTION, ss_sel.idx | !softintp);
				}

				switch (gd.type) {
				case CPU_SYSDESC_TYPE_INTR_32:
				case CPU_SYSDESC_TYPE_TRAP_32:
					CHECK_STACK_PUSH(&ss_sel.desc, new_sp, stacksize * 2);
					break;

				case CPU_SYSDESC_TYPE_INTR_16:
				case CPU_SYSDESC_TYPE_TRAP_16:
					CHECK_STACK_PUSH(&ss_sel.desc, new_sp, stacksize);
					new_ip &= 0xffff;
					break;
				}

				/* out of range */
				if (new_ip > intr_sel.desc.u.seg.limit) {
					EXCEPTION(GP_EXCEPTION, 0);
				}

				load_ss(new_ss, &ss_sel.desc, intr_sel.desc.dpl);
				CPU_ESP = new_sp;

				if (!CPU_STAT_VM86) {
					switch (gd.type) {
					case CPU_SYSDESC_TYPE_INTR_32:
					case CPU_SYSDESC_TYPE_TRAP_32:
						PUSH0_32(CPU_GS);
						PUSH0_32(CPU_FS);
						PUSH0_32(CPU_DS);
						PUSH0_32(CPU_ES);
						break;

					case CPU_SYSDESC_TYPE_INTR_16:
					case CPU_SYSDESC_TYPE_TRAP_16:
						PUSH0_16(CPU_GS);
						PUSH0_16(CPU_FS);
						PUSH0_16(CPU_DS);
						PUSH0_16(CPU_ES);
						break;
					}

					CPU_SET_SEGREG(CPU_GS_INDEX, 0);
					CPU_SET_SEGREG(CPU_FS_INDEX, 0);
					CPU_SET_SEGREG(CPU_DS_INDEX, 0);
					CPU_SET_SEGREG(CPU_ES_INDEX, 0);
				}

				switch (gd.type) {
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

				load_cs(intr_sel.selector, &intr_sel.desc, intr_sel.desc.dpl);
				SET_EIP(new_ip);
				set_eflags(flags, mask);
			} else {
				if (CPU_STAT_VM86
				 || (!intr_sel.desc.u.seg.ec && (intr_sel.desc.dpl != CPU_STAT_CPL))) {
					EXCEPTION(GP_EXCEPTION, intr_sel.idx);
				}
				VERBOSE(("INTRA-PRIVILEGE-LEVEL-INTERRUPT"));

				switch (gd.type) {
				case CPU_SYSDESC_TYPE_INTR_32:
				case CPU_SYSDESC_TYPE_TRAP_32:
					CHECK_STACK_PUSH(&ss_sel.desc, new_sp, errorp ? 16 : 12);
					break;

				case CPU_SYSDESC_TYPE_INTR_16:
				case CPU_SYSDESC_TYPE_TRAP_16:
					CHECK_STACK_PUSH(&ss_sel.desc, new_sp, errorp ? 8 : 6);
					new_ip &= 0xffff;
					break;
				}

				/* out of range */
				if (new_ip > intr_sel.desc.u.seg.limit) {
					EXCEPTION(GP_EXCEPTION, 0);
				}

				switch (gd.type) {
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
			break;

		default:
			EXCEPTION(GP_EXCEPTION, num * 8 | 2 | !softintp);
			break;
		}
	}
}

/*	$Id: ctrlxfer.c,v 1.2 2004/01/13 16:37:42 monaka Exp $	*/

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

#include "ctrlxfer.h"


void
JMPfar_pm(WORD selector, DWORD new_ip)
{
	selector_t jmp_sel;
	selector_t sel2;
	int rv;

	VERBOSE(("JMPfar_pm: EIP = 0x%08x, selector = 0x%04x, new_ip = 0x%08x", CPU_PREV_EIP, selector, new_ip));

	/*
	 * IF effective address in the CS, DS, ES, FS, GS, or SS segment is illegal
	 *     OR segment selector in target operand null
	 *     THEN #GP(0);
	 * FI;
	 */
	/* XXX */

	rv = parse_selector(&jmp_sel, selector);
	if (rv < 0) {
		VERBOSE(("JMPfar_pm: parse_selector (selector = %04x, rv = %d)", selector, rv));
		EXCEPTION(GP_EXCEPTION, jmp_sel.idx);
	}

	if (jmp_sel.desc.s) {
		/* code segment descriptor */
		VERBOSE(("JMPfar_pm: code or data segment descriptor"));

		if (!jmp_sel.desc.u.seg.c) {
			/* data segment */
			VERBOSE(("JMPfar_pm: data segment"));
			EXCEPTION(GP_EXCEPTION, jmp_sel.idx);
		}

		/* check privilege level */
		if (!jmp_sel.desc.u.seg.ec) {
			VERBOSE(("NONCONFORMING-CODE-SEGMENT"));
			/* ���� p.119 4.8.1.1. */
			if ((jmp_sel.rpl > CPU_STAT_CPL)
			 || (jmp_sel.desc.dpl != CPU_STAT_CPL)) {
				VERBOSE(("JMPfar_pm: RPL(%d) > CPL(%d) or DPL(%d) != CPL(%d)", jmp_sel.rpl, CPU_STAT_CPL, jmp_sel.desc.dpl, CPU_STAT_CPL));
				EXCEPTION(GP_EXCEPTION, jmp_sel.idx);
			}
		} else {
			VERBOSE(("CONFORMING-CODE-SEGMENT"));
			/* ���� p.120 4.8.1.2. */
			if (jmp_sel.desc.dpl > CPU_STAT_CPL) {
				VERBOSE(("JMPfar_pm: DPL(%d) > CPL(%d)", jmp_sel.desc.dpl, CPU_STAT_CPL));
				EXCEPTION(GP_EXCEPTION, jmp_sel.idx);
			}
		}

		/* not present */
		if (selector_is_not_present(&jmp_sel)) {
			VERBOSE(("JMPfar_pm: selector is not present"));
			EXCEPTION(NP_EXCEPTION, jmp_sel.idx);
		}

		if (!CPU_INST_OP32) {
			new_ip &= 0xffff;
		}

		/* out of range */
		if (new_ip > jmp_sel.desc.u.seg.limit) {
			VERBOSE(("JMPfar_pm: new_ip is out of range. new_ip = %08x, limit = %08x", new_ip, jmp_sel.desc.u.seg.limit));
			EXCEPTION(GP_EXCEPTION, 0);
		}

		VERBOSE(("JMPfar_pm: new CS = %04x, EIP = %08x", jmp_sel.selector, new_ip));
		load_cs(jmp_sel.selector, &jmp_sel.desc, CPU_STAT_CPL);
		SET_EIP(new_ip);
	} else {
		/* system descriptor */
		VERBOSE(("JMPfar_pm: system descriptor"));

		switch (jmp_sel.desc.type) {
		case CPU_SYSDESC_TYPE_CALL_16:
		case CPU_SYSDESC_TYPE_CALL_32:
			VERBOSE(("CALL-GATE"));

			/* check privilege level */
			if ((jmp_sel.desc.dpl < CPU_STAT_CPL)
			 || (jmp_sel.desc.dpl < jmp_sel.rpl)) {
				VERBOSE(("JMPfar_pm: DPL(%d) < CPL(%d) or DPL(%d) < RPL(%d)", jmp_sel.desc.dpl, CPU_STAT_CPL, jmp_sel.desc.dpl, jmp_sel.rpl));
				EXCEPTION(GP_EXCEPTION, jmp_sel.idx);
			}

			/* not present */
			if (selector_is_not_present(&jmp_sel)) {
				VERBOSE(("JMPfar_pm: selector is not present"));
				EXCEPTION(NP_EXCEPTION, jmp_sel.idx);
			}

			/* parse call gate selector */
			rv = parse_selector(&sel2, jmp_sel.desc.u.gate.selector);
			if (rv < 0) {
				VERBOSE(("JMPfar_pm: parse_selector (selector = %04x, rv = %d)", jmp_sel.desc.u.gate.selector, rv));
				EXCEPTION(GP_EXCEPTION, sel2.idx);
			}

			/* check code segment descriptor */
			if (!sel2.desc.s || !sel2.desc.u.seg.c) {
				VERBOSE(("JMPfar_pm: not code segment (%s, %s)", sel2.desc.s ? "code/data" : "system", sel2.desc.u.seg.c ? "code" : "data"));
				EXCEPTION(GP_EXCEPTION, sel2.idx);
			}

			/* check privilege level */
			if (!sel2.desc.u.seg.ec) {
				/* ���� p.119 4.8.1.1. */
				if ((sel2.rpl > CPU_STAT_CPL)
				 || (sel2.desc.dpl != CPU_STAT_CPL)) {
					VERBOSE(("JMPfar_pm: RPL(%d) > CPL(%d) or DPL(%d) != CPL(%d)", sel2.rpl, CPU_STAT_CPL, sel2.desc.dpl, CPU_STAT_CPL));
					EXCEPTION(GP_EXCEPTION, sel2.idx);
				}
			} else {
				/* ���� p.120 4.8.1.2. */
				if (sel2.desc.dpl > CPU_STAT_CPL) {
					VERBOSE(("JMPfar_pm: DPL(%d) > CPL(%d)", sel2.desc.dpl, CPU_STAT_CPL));
					EXCEPTION(GP_EXCEPTION, sel2.idx);
				}
			}

			/* not present */
			if (selector_is_not_present(&sel2)) {
				VERBOSE(("JMPfar_pm: selector is not present"));
				EXCEPTION(NP_EXCEPTION, sel2.idx);
			}

			new_ip = jmp_sel.desc.u.gate.offset;
			if (jmp_sel.desc.type == CPU_SYSDESC_TYPE_CALL_16) {
				new_ip &= 0xffff;
			}

			/* out of range */
			if (new_ip > sel2.desc.u.seg.limit) {
				VERBOSE(("JMPfar_pm: new_ip is out of range. new_ip = %08x, limit = %08x", new_ip, sel2.desc.u.seg.limit));
				EXCEPTION(GP_EXCEPTION, 0);
			}

			VERBOSE(("JMPfar_pm: new CS = %04x, EIP = %08x", sel2.selector, new_ip));
			load_cs(sel2.selector, &sel2.desc, CPU_STAT_CPL);
			SET_EIP(new_ip);
			break;

		/*
		 * �洬 p.373 JMP ̿��
		 *
		 * JMP ̿��ǥ������������å���¹Ԥ���Ȥ��� EFLAGS �쥸������
		 * �ͥ��Ȥ��줿���������ե饰 (NT) �����åȤ��줺�������� TSS ��
		 * �����Υ���������󥯡��ե�����ɤ����Υ������� TSS ���쥯����
		 * ���ɤ���ʤ��Τ���դ��줿�����������äơ����Υ������ؤ�
		 * �꥿����� IRET ̿��μ¹ԤǤϼ¸��Ǥ��ʤ���JMP ̿���
		 * �������������å���¹Ԥ���Τϡ��������� CALL ̿��Ȱۤʤ롣
		 * ���ʤ����CALL ̿��� NT �ե饰�򥻥åȤ���������
		 * ����������󥯾���򥻡��֤���Τǡ�IRET ̿��ǤΥ����븵
		 * �������ؤΥ꥿���󤬲�ǽ�ˤʤ롣
		 */
		case CPU_SYSDESC_TYPE_TASK:
			VERBOSE(("TASK-GATE"));

			/* check privilege level */
			if ((jmp_sel.desc.dpl < CPU_STAT_CPL)
			 || (jmp_sel.desc.dpl < jmp_sel.rpl)) {
				VERBOSE(("JMPfar_pm: DPL(%d) < CPL(%d) or DPL(%d) < RPL(%d)", jmp_sel.desc.dpl, CPU_STAT_CPL, jmp_sel.desc.dpl, jmp_sel.rpl));
				EXCEPTION(GP_EXCEPTION, jmp_sel.idx);
			}

			/* not present */
			if (selector_is_not_present(&jmp_sel)) {
				VERBOSE(("JMPfar_pm: selector is not present"));
				EXCEPTION(NP_EXCEPTION, jmp_sel.idx);
			}

			/* parse call tss selector */
			rv = parse_selector(&sel2, jmp_sel.desc.u.gate.selector);
			if (rv < 0 || sel2.ldt) {
				VERBOSE(("JMPfar_pm: parse_selector (selector = %04x, rv = %d, %s)", jmp_sel.desc.u.gate.selector, rv, sel2.ldt ? "LDT" : "GDT"));
				EXCEPTION(GP_EXCEPTION, sel2.idx);
			}

			/* check descriptor type */
			switch (sel2.desc.type) {
			case CPU_SYSDESC_TYPE_TSS_16:
			case CPU_SYSDESC_TYPE_TSS_32:
				break;

			default:
				VERBOSE(("JMPfar_pm: invalid descriptor type (type = %d)", sel2.desc.type));
				EXCEPTION(GP_EXCEPTION, sel2.idx);
				break;
			}

			/* not present */
			if (selector_is_not_present(&sel2)) {
				VERBOSE(("JMPfar_pm: selector is not present"));
				EXCEPTION(NP_EXCEPTION, sel2.idx);
			}

			task_switch(&sel2, TASK_SWITCH_JMP);

			/* out of range */
			if (CPU_EIP > CPU_STAT_CS_LIMIT) {
				VERBOSE(("JMPfar_pm: new_ip is out of range. new_ip = %08x, limit = %08x", CPU_EIP, CPU_STAT_CS_LIMIT));
				EXCEPTION(GP_EXCEPTION, 0);
			}
			break;

		case CPU_SYSDESC_TYPE_TSS_16:
		case CPU_SYSDESC_TYPE_TSS_32:
			VERBOSE(("TASK-STATE-SEGMENT"));

			/* check privilege level */
			if ((jmp_sel.desc.dpl < CPU_STAT_CPL)
			 || (jmp_sel.desc.dpl < jmp_sel.rpl)) {
				VERBOSE(("JMPfar_pm: DPL(%d) < CPL(%d) or DPL(%d) < RPL(%d)", jmp_sel.desc.dpl, CPU_STAT_CPL, jmp_sel.desc.dpl, jmp_sel.rpl));
				EXCEPTION(TS_EXCEPTION, jmp_sel.idx);
			}

			/* not present */
			if (selector_is_not_present(&jmp_sel)) {
				VERBOSE(("JMPfar_pm: selector is not present"));
				EXCEPTION(NP_EXCEPTION, jmp_sel.idx);
			}

			task_switch(&jmp_sel, TASK_SWITCH_JMP);

			/* out of range */
			if (CPU_EIP > CPU_STAT_CS_LIMIT) {
				VERBOSE(("JMPfar_pm: new_ip is out of range. new_ip = %08x, limit = %08x", CPU_EIP, CPU_STAT_CS_LIMIT));
				EXCEPTION(GP_EXCEPTION, 0);
			}
			break;

		case CPU_SYSDESC_TYPE_TSS_BUSY_16:
		case CPU_SYSDESC_TYPE_TSS_BUSY_32:
			VERBOSE(("JMPfar_pm: task is busy"));
			/*FALLTHROUGH*/
		default:
			VERBOSE(("JMPfar_pm: invalid descriptor type (type = %d)", sel2.desc.type));
			EXCEPTION(GP_EXCEPTION, jmp_sel.idx);
			break;
		}
	}
}

/*
 * 4.3.6. �ø���٥�֤Υ��������
 *
 * 1. �����������Υ����å�(�ø������å�)��¹Ԥ��롣
 * 2. SS, ESP, CS, EIP �γƥ쥸�����θ����Ϥ���Ū�������˥����֤��롣
 * 3. TSS �쥸�����˳�Ǽ����Ƥ��뿷���������å�(���ʤ�������ߥ����뤵��Ƥ���
 *    �ø���٥��ѤΥ����å�)�Υ������ȥ쥸�����ȥ����å��ݥ��󥿤�
 *    SS �쥸������ ESP �쥸�����˥��ɤ��������������å����ڤ��ؤ��롣
 * 4. �����븵�ץ�������Υ����å����Ф��ư��Ū�˥����֤��Ƥ����� SS �ͤ�
 *    ESP �ͤ򡢤��ο����������å��˥ץå��夹�롣
 * 5. �����븵�ץ�������Υ����å�����ѥ�᡼���򥳥ԡ����롣�����������å�
 *    �˥��ԡ������ѥ�᡼���ο��ϡ������롦�����ȡ��ǥ�������ץ�����ͤ�
 *    ��ޤ롣
 * 6. �����븵�ץ���������Ф��ư��Ū�˥����֤��Ƥ����� CS �ͤ� EIP �ͤ�
 *    �����������å��˥ץå��夹�롣
 * 7. �����������ɡ��������ȤΥ������ȡ����쥯���ȿ�����̿��ݥ��󥿤�
 *    �����롦�����Ȥ��� CS �쥸������ EIP �쥸�����ˤ��줾����ɤ��롣
 * 8. �����뤵�줿�ץ�������μ¹Ԥ򿷤����ø���٥�ǳ��Ϥ��롣
 */
void
CALLfar_pm(WORD selector, DWORD new_ip)
{
	selector_t call_sel;
	selector_t sel2;
	int rv;

	VERBOSE(("CALLfar_pm: EIP = 0x%08x, selector = 0x%04x, new_ip = 0x%08x", CPU_PREV_EIP, selector, new_ip));

	rv = parse_selector(&call_sel, selector);
	if (rv < 0) {
		EXCEPTION(GP_EXCEPTION, call_sel.idx);
	}

	if (call_sel.desc.s) {
		/* code segment descriptor */
		if (!call_sel.desc.u.seg.c) {
			/* data segment */
			EXCEPTION(GP_EXCEPTION, call_sel.idx);
		}

		/* check privilege level */
		if (!call_sel.desc.u.seg.ec) {
			VERBOSE(("NONCONFORMING-CODE-SEGMENT"));

			/* ���� p.119 4.8.1.1. */
			if ((call_sel.rpl > CPU_STAT_CPL)
			 || (call_sel.desc.dpl != CPU_STAT_CPL)) {
				EXCEPTION(GP_EXCEPTION, call_sel.idx);
			}
		} else {
			VERBOSE(("CONFORMING-CODE-SEGMENT"));

			/* ���� p.120 4.8.1.2. */
			if (call_sel.desc.dpl > CPU_STAT_CPL) {
				EXCEPTION(GP_EXCEPTION, call_sel.idx);
			}
		}

		/* not present */
		if (selector_is_not_present(&call_sel)) {
			EXCEPTION(NP_EXCEPTION, call_sel.idx);
		}

		if (CPU_INST_OP32) {
			CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 8);
			/* out of range */
			if (new_ip > call_sel.desc.u.seg.limit) {
				EXCEPTION(GP_EXCEPTION, 0);
			}

			PUSH0_32(CPU_CS);
			PUSH0_32(CPU_EIP);
		} else {
			CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 4);
			/* out of range */
			if (new_ip > call_sel.desc.u.seg.limit) {
				EXCEPTION(GP_EXCEPTION, 0);
			}

			PUSH0_16(CPU_CS);
			PUSH0_16(CPU_IP);
		}

		if (!call_sel.desc.d) {
			new_ip &= 0xffff;
		}

		load_cs(call_sel.selector, &call_sel.desc, CPU_STAT_CPL);
		SET_EIP(new_ip);
	} else {
		DWORD temp_eip, temp_esp;
		WORD temp_cs, temp_ss;

		/* system descriptor */
		switch (call_sel.desc.type) {
		case CPU_SYSDESC_TYPE_CALL_16:
		case CPU_SYSDESC_TYPE_CALL_32:
			VERBOSE(("CALL-GATE"));

			/* check privilege level */
			if ((call_sel.desc.dpl < CPU_STAT_CPL)
			 || (call_sel.desc.dpl < call_sel.rpl)) {
				EXCEPTION(GP_EXCEPTION, call_sel.idx);
			}

			/* not present */
			if (selector_is_not_present(&call_sel)) {
				EXCEPTION(NP_EXCEPTION, call_sel.idx);
			}

			/* parse code segment descriptor */
			rv = parse_selector(&sel2, call_sel.desc.u.gate.selector);
			if (rv < 0) {
				EXCEPTION(GP_EXCEPTION, sel2.idx);
			}

			/* check code segment & privilege level */
			if (!sel2.desc.s
			 || !sel2.desc.u.seg.c
			 || sel2.desc.dpl > CPU_STAT_CPL) {
				EXCEPTION(GP_EXCEPTION, sel2.idx);
			}

			/* not present */
			if (selector_is_not_present(&sel2)) {
				EXCEPTION(NP_EXCEPTION, sel2.idx);
			}

			/* save register */
			temp_cs = CPU_CS;
			temp_ss = CPU_SS;
			temp_eip = CPU_EIP;
			temp_esp = CPU_ESP;
			if (sel2.desc.type == CPU_SYSDESC_TYPE_CALL_16) {
				temp_eip &= 0xffff;
				temp_esp &= 0xffff;
			}

			new_ip = call_sel.desc.u.gate.offset;

			/* out of range */
			if (new_ip > sel2.desc.u.seg.limit) {
				EXCEPTION(GP_EXCEPTION, 0);
			}

			if (!sel2.desc.u.seg.ec
			 && (sel2.desc.dpl < CPU_STAT_CPL)) {
				DWORD param[32];	// copy param
				selector_t ss_sel;
				DWORD tss_esp;
				WORD tss_ss;
				BYTE i;

				VERBOSE(("MORE-PRIVILEGE"));

				get_stack_from_tss(sel2.desc.dpl, &tss_ss, &tss_esp);

				/* parse stack segment descriptor */
				rv = parse_selector(&ss_sel, tss_ss);
				if (rv < 0) {
					EXCEPTION(TS_EXCEPTION, ss_sel.idx);
				}

				/* check privilege level */
				if ((ss_sel.rpl != sel2.desc.dpl)
				 || (ss_sel.desc.dpl != sel2.desc.dpl)
				 || !ss_sel.desc.s
				 || !ss_sel.desc.u.seg.wr) {
					EXCEPTION(TS_EXCEPTION, ss_sel.idx);
				}

				/* not present */
				if (selector_is_not_present(&ss_sel)) {
					EXCEPTION(NP_EXCEPTION, ss_sel.idx);
				}

				if (call_sel.desc.type == CPU_SYSDESC_TYPE_CALL_32){
					CHECK_STACK_PUSH(&ss_sel.desc, tss_esp, 16 + sel2.desc.u.gate.count * 4);

					/* dump param */
					for (i = 0; i < sel2.desc.u.gate.count; i++) {
						param[i] = cpu_vmemoryread_d(CPU_SS_INDEX, CPU_ESP + i * 4);
					}

					load_ss(tss_ss, &ss_sel.desc, ss_sel.desc.dpl);
					CPU_ESP = tss_esp;
					load_cs(sel2.selector, &sel2.desc, sel2.desc.dpl);
					SET_EIP(new_ip);

					PUSH0_32(temp_ss);
					PUSH0_32(temp_esp);

					/* restore param */
					for (i = sel2.desc.u.gate.count; i != 0; i--) {
						PUSH0_32(param[i - 1]);
					}

					PUSH0_32(temp_cs);
					PUSH0_32(temp_eip);
				} else {
					CHECK_STACK_PUSH(&ss_sel.desc, tss_esp, 8 + sel2.desc.u.gate.count * 2);

					new_ip &= 0xffff;

					/* dump param */
					for (i = 0; i < sel2.desc.u.gate.count; i++) {
						param[i] = cpu_vmemoryread_w(CPU_SS_INDEX, CPU_ESP + i * 2);
					}

					load_ss(tss_ss, &ss_sel.desc, ss_sel.desc.dpl);
					CPU_ESP = tss_esp & 0xffff;
					load_cs(sel2.selector, &sel2.desc, sel2.desc.dpl);
					SET_EIP(new_ip);

					PUSH0_16(temp_ss);
					PUSH0_16(temp_esp);

					/* restore param */
					for (i = sel2.desc.u.gate.count; i != 0; i--) {
						PUSH0_16(param[i - 1]);
					}

					PUSH0_16(temp_cs);
					PUSH0_16(temp_eip);
				}
			} else {
				VERBOSE(("SAME-PRIVILEGE"));

				if (call_sel.desc.type == CPU_SYSDESC_TYPE_CALL_32){
					CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 8);

					load_cs(sel2.selector, &sel2.desc, CPU_STAT_CPL);
					SET_EIP(new_ip);

					PUSH0_32(temp_cs);
					PUSH0_32(temp_eip);
				} else {
					CHECK_STACK_PUSH(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 4);

					load_cs(sel2.selector, &sel2.desc, CPU_STAT_CPL);
					new_ip &= 0xffff;
					SET_EIP(new_ip);

					PUSH0_16(temp_cs);
					PUSH0_16(temp_eip);
				}
			}
			break;

		case CPU_SYSDESC_TYPE_TASK:
			VERBOSE(("TASK-GATE"));

			/* check privilege level */
			if ((call_sel.desc.dpl < CPU_STAT_CPL)
			 || (call_sel.desc.dpl < call_sel.rpl)) {
				EXCEPTION(GP_EXCEPTION, call_sel.idx);
			}

			/* not present */
			if (selector_is_not_present(&call_sel)) {
				EXCEPTION(NP_EXCEPTION, call_sel.idx);
			}

			/* tss descriptor */
			rv = parse_selector(&sel2, call_sel.desc.u.gate.selector);
			if (rv < 0 || sel2.ldt) {
				EXCEPTION(GP_EXCEPTION, sel2.idx);
			}

			/* check descriptor type */
			switch (sel2.desc.type) {
			case CPU_SYSDESC_TYPE_TSS_16:
			case CPU_SYSDESC_TYPE_TSS_32:
				break;

			case CPU_SYSDESC_TYPE_TSS_BUSY_16:
			case CPU_SYSDESC_TYPE_TSS_BUSY_32:
				VERBOSE(("CALLfar_pm: task is busy"));
				/*FALLTHROUGH*/
			default:
				EXCEPTION(GP_EXCEPTION, sel2.idx);
				break;
			}

			/* not present */
			if (selector_is_not_present(&sel2)) {
				EXCEPTION(NP_EXCEPTION, sel2.idx);
			}

			task_switch(&sel2, TASK_SWITCH_CALL);

			/* out of range */
			if (CPU_EIP > CPU_STAT_CS_LIMIT) {
				EXCEPTION(GP_EXCEPTION, 0);
			}
			break;

		case CPU_SYSDESC_TYPE_TSS_16:
		case CPU_SYSDESC_TYPE_TSS_32:
			VERBOSE(("TASK-STATE-SEGMENT"));

			/* check privilege level */
			if ((call_sel.desc.dpl < CPU_STAT_CPL)
			 || (call_sel.desc.dpl < call_sel.rpl)) {
				EXCEPTION(GP_EXCEPTION, call_sel.idx);
			}

			/* not present */
			if (selector_is_not_present(&call_sel)) {
				EXCEPTION(NP_EXCEPTION, call_sel.idx);
			}

			task_switch(&call_sel, TASK_SWITCH_CALL);

			/* out of range */
			if (CPU_EIP > CPU_STAT_CS_LIMIT) {
				EXCEPTION(GP_EXCEPTION, 0);
			}
			break;

		case CPU_SYSDESC_TYPE_TSS_BUSY_16:
		case CPU_SYSDESC_TYPE_TSS_BUSY_32:
			VERBOSE(("CALLfar_pm: task is busy"));
			/*FALLTHROUGH*/
		default:
			EXCEPTION(GP_EXCEPTION, call_sel.idx);
			break;
		}
	}
}

/*
 * 4.3.6. �ø���٥�֤Υ꥿�������
 *
 * 1. �ø������å���¹Ԥ��롣
 * 2. CS �쥸������ EIP �쥸�����˥����������ͤ�ꥹ�ȥ����롣
 * 3. RET ̿��˥��ץ����������� n ��������ϡ��ѥ�᡼���򥹥��å�����
 *    �������뤿�ᡢn ���ڥ��ɤǻ��ꤵ�줿�Х��ȿ����������å����ݥ��󥿤�
 *    ���󥯥���Ȥ��롣�����롦�����ȡ��ǥ�������ץ����������å��֤�
 *    1 �İʾ�Υѥ�᡼���򥳥ԡ�����褦���ꤷ�Ƥ�����ϡ�RET n ̿���
 *    ���Ѥ���ξ�����å�����ѥ�᡼���������ʤ���Фʤ�ʤ���n ���ڥ���
 *    �ˤϡ��ƥ����å���ǥѥ�᡼������ͭ����Х��ȿ�����ꤹ�롣
 *    �꥿������ˡ��ץ��å��ϳƥ����å����Ф��� n ���� ESP �򥤥󥯥����
 *    ���������Υѥ�᡼���򥹥��å������Ψ�褯������롣
 * 4. SS �쥸������ ESP �쥸�����ˡ������������ͤ�ꥹ�ȥ����롣����ǡ�
 *    �����븵�ץ�������Υ����å����ڤ��ؤ����롣
 * 5. RET ̿��˥��ץ����������� n ��������ϡ��ѥ�᡼���򥹥��å�����
 *    �������뤿�ᡢn ���ڥ��ɤǻ��ꤵ�줿�Х��ȿ����������å����ݥ��󥿤�
 *    ���󥯥���Ȥ���(���ƥå� 3 �������򻲾�)��
 * 6. �����븵�ץ�������μ¹Ԥ�Ƴ����롣
 */
void
RETfar_pm(DWORD nbytes)
{
	selector_t ret_sel;
	int rv;
	DWORD new_ip;
	WORD selector;

	VERBOSE(("RETfar_pm: EIP = 0x%08x, nbytes = %d", CPU_PREV_EIP, nbytes));

	if (CPU_INST_OP32) {
		CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 8 + nbytes);
		if (CPU_STAT_SS32) {
			new_ip = cpu_vmemoryread_d(CPU_SS_INDEX, CPU_ESP);
			selector = cpu_vmemoryread_d(CPU_SS_INDEX, CPU_ESP + 4);
		} else {
			new_ip = cpu_vmemoryread_d(CPU_SS_INDEX, CPU_SP);
			selector = cpu_vmemoryread_d(CPU_SS_INDEX, CPU_SP + 4);
		}
	} else {
		CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 4 + nbytes);
		if (CPU_STAT_SS32) {
			new_ip = cpu_vmemoryread_w(CPU_SS_INDEX, CPU_ESP);
			selector = cpu_vmemoryread_w(CPU_SS_INDEX, CPU_ESP + 2);
		} else {
			new_ip = cpu_vmemoryread_w(CPU_SS_INDEX, CPU_SP);
			selector = cpu_vmemoryread_w(CPU_SS_INDEX, CPU_SP + 2);
		}
	}

	rv = parse_selector(&ret_sel, selector);
	if (rv < 0) {
		EXCEPTION(GP_EXCEPTION, ret_sel.idx);
	}

	/* check code segment descriptor */
	if (!ret_sel.desc.s || !ret_sel.desc.u.seg.c) {
		EXCEPTION(GP_EXCEPTION, ret_sel.idx);
	}

	/* check privilege level */
	if (ret_sel.rpl < CPU_STAT_CPL
	 || (ret_sel.desc.u.seg.ec && (ret_sel.desc.dpl > ret_sel.rpl))) {
		EXCEPTION(GP_EXCEPTION, ret_sel.idx);
	}

	/* not present */
	if (selector_is_not_present(&ret_sel)) {
		EXCEPTION(NP_EXCEPTION, ret_sel.idx);
	}

	if (ret_sel.rpl > CPU_STAT_CPL) {
		selector_t ss_sel;
		selector_t temp_sel;
		DWORD new_sp;
		WORD new_ss;
		int i;

		VERBOSE(("RETURN-OUTER-PRIVILEGE-LEVEL"));

		if (CPU_INST_OP32) {
			CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 16 + nbytes);
			if (CPU_STAT_SS32) {
				new_sp = cpu_vmemoryread_d(CPU_SS_INDEX, CPU_ESP + 8 + nbytes);
				new_ss = (WORD)cpu_vmemoryread_d(CPU_SS_INDEX, CPU_ESP + 8 + 4 + nbytes);
			} else {
				new_sp = cpu_vmemoryread_d(CPU_SS_INDEX, (WORD)(CPU_SP + 8 + nbytes));
				new_ss = (WORD)cpu_vmemoryread_d(CPU_SS_INDEX, (WORD)(CPU_SP + 8 + 4 + nbytes));
			}
		} else {
			CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 8 + nbytes);
			if (CPU_STAT_SS32) {
				new_sp = cpu_vmemoryread_w(CPU_SS_INDEX, CPU_ESP + 4 + nbytes);
				new_ss = cpu_vmemoryread_w(CPU_SS_INDEX, CPU_ESP + 4 + 2 + nbytes);
			} else {
				new_sp = cpu_vmemoryread_w(CPU_SS_INDEX, (WORD)(CPU_SP + 4 + nbytes));
				new_ss = cpu_vmemoryread_w(CPU_SS_INDEX, (WORD)(CPU_SP + 4 + 2 + nbytes));
			}
		}

		rv = parse_selector(&ss_sel, new_ss);
		if (rv < 0) {
			EXCEPTION(GP_EXCEPTION, ss_sel.idx);
		}

		/* check stack segment descriptor */
		/* check privilege level */
		if ((ss_sel.rpl != ret_sel.rpl)
		 || (ss_sel.desc.dpl != ret_sel.rpl)
		 || !ss_sel.desc.s
		 || !ss_sel.desc.u.seg.wr) {
			EXCEPTION(GP_EXCEPTION, ss_sel.idx);
		}

		/* not present */
		if (selector_is_not_present(&ss_sel)) {
			EXCEPTION(SS_EXCEPTION, ss_sel.idx);
		}

		/* check code segment limit */
		if (new_ip > ret_sel.desc.u.seg.limit) {
			EXCEPTION(GP_EXCEPTION, 0);
		}

		/* set new register */
		load_cs(ret_sel.selector, &ret_sel.desc, ret_sel.rpl);
		SET_EIP(new_ip);
		load_ss(ss_sel.selector, &ss_sel.desc, ret_sel.rpl);
		CPU_ESP = new_sp;

		/* check segment register */
		for (i = 0; i < CPU_SEGREG_NUM; i++) {
			if (i == CPU_CS_INDEX || i == CPU_SS_INDEX)
				continue;

			rv = parse_selector(&temp_sel, CPU_REGS_SREG(i));
			if (rv < 0) {
				CPU_REGS_SREG(i) = 0;
				CPU_STAT_SREG(i).valid = 0;
				continue;
			}

			/* check privilege level */
			if (!temp_sel.desc.s
			 || (temp_sel.desc.u.seg.c && temp_sel.desc.u.seg.wr)
			 || ((!temp_sel.desc.u.seg.c || !temp_sel.desc.u.seg.ec)
			   && ((temp_sel.desc.dpl < CPU_STAT_CPL) || (temp_sel.desc.dpl < temp_sel.rpl)))) {
				CPU_REGS_SREG(i) = 0;
				CPU_STAT_SREG(i).valid = 0;
				continue;
			}
		}
		CPU_ESP += nbytes;
	} else {
		VERBOSE(("RETURN-TO-SAME-PRIVILEGE-LEVEL"));

		if (CPU_INST_OP32) {
			if (new_ip > ret_sel.desc.u.seg.limit) {
				EXCEPTION(GP_EXCEPTION, 0);
			}
			POP0_32(CPU_EIP);
			POP0_32(CPU_CS);
		} else {
			new_ip &= 0xffff;
			if (new_ip > ret_sel.desc.u.seg.limit) {
				EXCEPTION(GP_EXCEPTION, 0);
			}
			POP0_16(CPU_EIP);
			POP0_16(CPU_CS);
		}
		load_cs(ret_sel.selector, &ret_sel.desc, CPU_STAT_CPL);
		SET_EIP(new_ip);
		CPU_ESP += nbytes;
	}
}

void
IRET_pm()
{
	selector_t iret_sel;
	int rv;
	DWORD new_ip, new_flags;
	WORD new_cs;

	VERBOSE(("IRET_pm: EIP = 0x%08x", CPU_PREV_EIP));

	if (CPU_STAT_VM86) {
		/* RETURN-FROM-VIRTUAL-8086-MODE */
		VERBOSE(("Virtual-8086 mode: PE=1, VM=1"));
		VERBOSE(("Processor is in virtual-8086 mode when IRET is executed and stays in virtual-8086 mode"));

		if (CPU_STAT_IOPL == CPU_IOPL3) {
			VERBOSE(("Virtual mode: PE=1, VM=1, IOPL=3"));
			if (CPU_INST_OP32) {
				CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 12);
				POP0_32(new_ip);
				POP0_32(new_cs);
				POP0_32(new_flags);

				set_eflags(new_flags, I_FLAG|RF_FLAG);
			} else {
				CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 6);
				POP0_16(new_ip);
				POP0_16(new_cs);
				POP0_16(new_flags);

				set_flags(new_flags, I_FLAG);
			}
			CPU_SET_SEGREG(CPU_CS_INDEX, new_cs);
			SET_EIP(new_ip);
			return;
		}
		VERBOSE(("trap to virtual-8086 monitor: PE=1, VM=1, IOPL<3"));
		EXCEPTION(GP_EXCEPTION, 0);
	} else if (CPU_EFLAG & NT_FLAG) {
		VERBOSE(("TASK-RETURN: PE=1, VM=0, NT=1"));

		new_cs = get_link_selector_from_tss();
		rv = parse_selector(&iret_sel, new_cs);
		if (rv < 0 || iret_sel.ldt) {
			EXCEPTION(GP_EXCEPTION, iret_sel.idx);
		}

		/* check system segment */
		if (iret_sel.desc.s) {
			EXCEPTION(GP_EXCEPTION, iret_sel.idx);
		}
		switch (iret_sel.desc.type) {
		case CPU_SYSDESC_TYPE_TSS_BUSY_16:
		case CPU_SYSDESC_TYPE_TSS_BUSY_32:
			break;

		case CPU_SYSDESC_TYPE_TSS_16:
		case CPU_SYSDESC_TYPE_TSS_32:
			VERBOSE(("IRET_pm: task is not busy"));
			/*FALLTHROUGH*/
		default:
			EXCEPTION(GP_EXCEPTION, iret_sel.idx);
			break;
		}

		/* not present */
		if (selector_is_not_present(&iret_sel)) {
			EXCEPTION(NP_EXCEPTION, iret_sel.idx);
		}

		task_switch(&iret_sel, TASK_SWITCH_IRET);

		/* out of range */
		if (CPU_EIP > CPU_STAT_CS_LIMIT) {
			EXCEPTION(GP_EXCEPTION, 0);
		}
	} else {
		if (CPU_INST_OP32) {
			CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 12);
			POP0_32(new_ip);
			POP0_32(new_cs);
			POP0_32(new_flags);
		} else {
			CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 6);
			POP0_16(new_ip);
			POP0_16(new_cs);
			POP0_16(new_flags);
		}

		if ((CPU_STAT_CPL == 0) && (new_flags & VM_FLAG)) {
			/* RETURN-TO-VIRTUAL-8086-MODE */
			descriptor_t sd;
			DWORD new_sp;
			WORD segsel[CPU_SEGREG_NUM];
			int i;

			VERBOSE(("Interrupt procedure was in virtual-8086 mode: PE=1, VM=1 in flags image"));

			CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 24);

			segsel[CPU_CS_INDEX] = new_cs;
			POP0_32(new_sp);
			POP0_32(segsel[CPU_SS_INDEX]);
			POP0_32(segsel[CPU_ES_INDEX]);
			POP0_32(segsel[CPU_DS_INDEX]);
			POP0_32(segsel[CPU_FS_INDEX]);
			POP0_32(segsel[CPU_GS_INDEX]);

			for (i = 0; i < CPU_SEGREG_NUM; i++) {
				CPU_REGS_SREG(i) = segsel[i];
				sd.u.seg.limit = 0xffff;
				CPU_SET_SEGDESC_DEFAULT(&sd, i, segsel[i]);
				sd.dpl = 3;
				CPU_STAT_SREG(i) = sd;
			}
			CPU_ESP = new_sp;
			SET_EIP(new_ip & 0xffff);

			set_eflags(new_flags, IOPL_FLAG|RF_FLAG);
		} else {
			DWORD mask;

			/* PROTECTED-MODE-RETURN */
			VERBOSE(("PE=1, VM=0 in flags image"));

			rv = parse_selector(&iret_sel, new_cs);
			if (rv < 0) {
				EXCEPTION(GP_EXCEPTION, iret_sel.idx);
			}

			/* check code segment descriptor */
			if (!iret_sel.desc.s || !iret_sel.desc.u.seg.c) {
				EXCEPTION(GP_EXCEPTION, iret_sel.idx);
			}

			/* check privilege level */
			if ((iret_sel.rpl < CPU_STAT_CPL)
			 || (iret_sel.desc.u.seg.ec && (iret_sel.desc.dpl > iret_sel.rpl))) {
				EXCEPTION(GP_EXCEPTION, iret_sel.idx);
			}

			/* not present */
			if (selector_is_not_present(&iret_sel)) {
				EXCEPTION(NP_EXCEPTION, iret_sel.idx);
			}

			if (iret_sel.rpl > CPU_STAT_CPL) {
				/* RETURN-OUTER-PRIVILEGE-LEVEL */
				selector_t ss_sel;
				WORD new_sp, new_ss;

				if (CPU_INST_OP32) {
					CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 8);
					if (CPU_STAT_SS32) {
						new_sp = cpu_vmemoryread_d(CPU_SS_INDEX, CPU_ESP);
						new_ss = cpu_vmemoryread_d(CPU_SS_INDEX, CPU_ESP + 4);
					} else {
						new_sp = cpu_vmemoryread_d(CPU_SS_INDEX, CPU_SP);
						new_ss = cpu_vmemoryread_d(CPU_SS_INDEX, CPU_SP + 4);
					}
				} else {
					CHECK_STACK_POP(&CPU_STAT_SREG(CPU_SS_INDEX), CPU_ESP, 4);
					if (CPU_STAT_SS32) {
						new_sp = cpu_vmemoryread_w(CPU_SS_INDEX, CPU_ESP);
						new_ss = cpu_vmemoryread_w(CPU_SS_INDEX, CPU_ESP + 2);
					} else {
						new_sp = cpu_vmemoryread_w(CPU_SS_INDEX, CPU_SP);
						new_ss = cpu_vmemoryread_w(CPU_SS_INDEX, CPU_SP + 2);
					}
				}

				rv = parse_selector(&ss_sel, new_ss);
				if (rv < 0) {
					EXCEPTION(GP_EXCEPTION, ss_sel.idx);
				}

				/* check stack segment descriptor */
				/* check privilege level */
				if ((ss_sel.rpl != iret_sel.rpl)
				 || (ss_sel.desc.dpl != iret_sel.rpl)
				 || !ss_sel.desc.s
				 || !ss_sel.desc.u.seg.wr) {
					EXCEPTION(GP_EXCEPTION, ss_sel.idx);
				}

				/* not present */
				if (selector_is_not_present(&ss_sel)) {
					EXCEPTION(SS_EXCEPTION, ss_sel.idx);
				}
			} else {
				VERBOSE(("RETURN-TO-SAME-PRIVILEGE-LEVEL"));
			}

			/* check code segment limit */
			if (new_ip > iret_sel.desc.u.seg.limit) {
				EXCEPTION(GP_EXCEPTION, 0);
			}

			/* set new register */
			load_cs(iret_sel.selector, &iret_sel.desc, iret_sel.rpl);
			SET_EIP(new_ip);

			mask = 0;
			if (CPU_INST_OP32)
				mask |= RF_FLAG;
			if (CPU_STAT_CPL <= CPU_STAT_IOPL)
				mask |= I_FLAG;
			if (CPU_STAT_CPL == 0) {
				mask |= IOPL_FLAG;
				if (CPU_INST_OP32)
					mask |= VM_FLAG|VIF_FLAG|VIP_FLAG;
			}
			set_eflags(new_flags, mask);

			if (iret_sel.rpl > CPU_STAT_CPL) {
				selector_t temp_sel;
				int i;

				/* RETURN-OUTER-PRIVILEGE-LEVEL */
				/* check segment register */
				for (i = 0; i < CPU_SEGREG_NUM; i++) {
					if ((i == CPU_CS_INDEX) || (i == CPU_SS_INDEX))
						continue;

					rv = parse_selector(&temp_sel, CPU_REGS_SREG(i));
					if (rv < 0) {
						CPU_REGS_SREG(i) = 0;
						CPU_STAT_SREG(i).valid = 0;
						continue;
					}

					if ((!temp_sel.desc.u.seg.c || !temp_sel.desc.u.seg.ec)
					 && (CPU_STAT_CPL > temp_sel.desc.dpl)) {
						CPU_REGS_SREG(i) = 0;
						CPU_STAT_SREG(i).valid = 0;
						continue;
					}
				}
			}
		}
	}
}

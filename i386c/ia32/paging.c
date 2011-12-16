/*
 * Copyright (c) 2003-2004 NONAKA Kimihiro
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

/*
 * �ڡ����ե�����㳰
 *
 * 4-31: ͽ��Ѥ�
 *    3: RSVD: 0 = �ե���Ȥθ�����ͽ��ӥåȰ�ȿ�ǤϤʤ��ä���
 *             1 = �ڡ������ե���Ȥθ����ϡ���ȿ�ȥޡ������줿 PTE �ޤ���
 *                 PDE ��ͽ��ӥåȰ��֤Τ�����Ĥǡ�1 �����Ф��줿���ȤǤ��롥
 *    2: U/S:  0 = �ե���Ȥθ����Ȥʤä����������ϥץ��å��������ѥХ�����
 *                 �⡼�ɤǼ¹���˹Ԥ�줿��
 *             1 = �ե���Ȥθ����Ȥʤä����������ϥץ��å����桼�����⡼�ɤ�
 *                 �¹���˹Ԥ�줿��
 *    1: W/R:  0 = �ե���Ȥθ����Ȥʤä������������ɤ߼��Ǥ��ä���
 *             1 = �ե���Ȥθ����Ȥʤä������������񤭹��ߤǤ��ä���
 *    0: P:    0 = �ե���Ȥθ������Ժߥڡ����Ǥ��ä���
 *             1 = �ե���Ȥθ������ڡ�������٥��ݸ��ȿ�Ǥ��ä���
 */

/*
 * ���� 4.12. �ڡ����ݸ�ȥ��������ݸ���Ȥ߹�碌
 * ��ɽ 4-2. �ڡ������ǥ��쥯�ȥ�ȥڡ������ơ��֥���ݸ���Ȥ߹�碌��
 *
 * +------------+------------+------------+
 * |    PDE     |    PTE     |   merge    |
 * +-----+------+-----+------+-----+------+
 * | pri | type | pri | type | pri | type |
 * +-----+------+-----+------+-----+------+
 * |  u  |  ro  |  u  |  ro  |  u  |  ro  |
 * |  u  |  ro  |  u  |  rw  |  u  |  ro  |
 * |  u  |  rw  |  u  |  ro  |  u  |  ro  |
 * |  u  |  rw  |  u  |  rw  |  u  |  rw  |
 * |  u  |  ro  |  s  |  ro  |  s  | rw/p |
 * |  u  |  ro  |  s  |  rw  |  s  | rw/p |
 * |  u  |  rw  |  s  |  ro  |  s  | rw/p |
 * |  u  |  rw  |  s  |  rw  |  s  |  rw  |
 * |  s  |  ro  |  u  |  ro  |  s  | rw/p |
 * |  s  |  ro  |  u  |  rw  |  s  | rw/p |
 * |  s  |  rw  |  u  |  ro  |  s  | rw/p |
 * |  s  |  rw  |  u  |  rw  |  s  |  rw  |
 * |  s  |  ro  |  s  |  ro  |  s  | rw/p |
 * |  s  |  ro  |  s  |  rw  |  s  | rw/p |
 * |  s  |  rw  |  s  |  ro  |  s  | rw/p |
 * |  s  |  rw  |  s  |  rw  |  s  |  rw  |
 * +-----+------+-----+------+-----+------+
 *
 * �� rw/p : CR0 �� WP �ӥåȤ� ON �ξ��ˤ� ro
 */

/*
 * ���ꥢ������/PxE(�嵭����)/CPL/CR0 �ȥڡ��������������δط�
 *
 * +-----+-----+-----+-----+-----+---+
 * | CR0 | CPL | PxE | PxE | ope |   |
 * | W/P | u/s | u/s | r/w | r/w |   |
 * +-----+-----+-----+-----+-----+---+
 * | n/a |  s  |  s  | n/a |  r  | o |
 * | n/a |  s  |  u  | n/a |  r  | o |
 * | n/a |  u  |  s  | n/a |  r  | x |
 * | n/a |  u  |  u  | n/a |  r  | o |
 * +-----+-----+-----+-----+-----+---+
 * |  n  |  s  |  s  |  r  |  w  | o |
 * |  n  |  s  |  u  |  r  |  w  | o |
 * |  n  |  u  |  s  |  r  |  w  | x |
 * |  n  |  u  |  u  |  r  |  w  | x |
 * +-----+-----+-----+-----+-----+---+
 * |  p  |  s  |  s  |  r  |  w  | x |
 * |  p  |  s  |  u  |  r  |  w  | x |
 * |  p  |  u  |  s  |  r  |  w  | x |
 * |  p  |  u  |  u  |  r  |  w  | x |
 * +-----+-----+-----+-----+-----+---+
 * |  n  |  s  |  s  |  w  |  w  | o |
 * |  n  |  s  |  u  |  w  |  w  | o |
 * |  n  |  u  |  s  |  w  |  w  | x |
 * |  n  |  u  |  u  |  w  |  w  | o |
 * +-----+-----+-----+-----+-----+---+
 * |  p  |  s  |  s  |  w  |  w  | o |
 * |  p  |  s  |  u  |  w  |  w  | x |
 * |  p  |  u  |  s  |  w  |  w  | x |
 * |  p  |  u  |  u  |  w  |  w  | o |
 * +-----+-----------+-----+-----+---+
 */
#if !defined(USE_PAGE_ACCESS_TABLE)
#define	page_access	0xd0ddd0ff
#else	/* USE_PAGE_ACCESS_TABLE */
static const UINT8 page_access_bit[32] = {
	1,	/* CR0: n, CPL: s, PTE: s, PTE: r, ope: r */
	1,	/* CR0: n, CPL: s, PTE: s, PTE: r, ope: w */
	1,	/* CR0: n, CPL: s, PTE: s, PTE: w, ope: r */
	1,	/* CR0: n, CPL: s, PTE: s, PTE: w, ope: w */

	1,	/* CR0: n, CPL: s, PTE: u, PTE: r, ope: r */
	1,	/* CR0: n, CPL: s, PTE: u, PTE: r, ope: w */
	1,	/* CR0: n, CPL: s, PTE: u, PTE: w, ope: r */
	1,	/* CR0: n, CPL: s, PTE: u, PTE: w, ope: w */

	0,	/* CR0: n, CPL: u, PTE: s, PTE: r, ope: r */
	0,	/* CR0: n, CPL: u, PTE: s, PTE: r, ope: w */
	0,	/* CR0: n, CPL: u, PTE: s, PTE: w, ope: r */
	0,	/* CR0: n, CPL: u, PTE: s, PTE: w, ope: w */

	1,	/* CR0: n, CPL: u, PTE: u, PTE: r, ope: r */
	0,	/* CR0: n, CPL: u, PTE: u, PTE: r, ope: w */
	1,	/* CR0: n, CPL: u, PTE: u, PTE: w, ope: r */
	1,	/* CR0: n, CPL: u, PTE: u, PTE: w, ope: w */

	1,	/* CR0: p, CPL: s, PTE: s, PTE: r, ope: r */
	0,	/* CR0: p, CPL: s, PTE: s, PTE: r, ope: w */
	1,	/* CR0: p, CPL: s, PTE: s, PTE: w, ope: r */
	1,	/* CR0: p, CPL: s, PTE: s, PTE: w, ope: w */

	1,	/* CR0: p, CPL: s, PTE: u, PTE: r, ope: r */
	0,	/* CR0: p, CPL: s, PTE: u, PTE: r, ope: w */
	1,	/* CR0: p, CPL: s, PTE: u, PTE: w, ope: r */
	1,	/* CR0: p, CPL: s, PTE: u, PTE: w, ope: w */

	0,	/* CR0: p, CPL: u, PTE: s, PTE: r, ope: r */
	0,	/* CR0: p, CPL: u, PTE: s, PTE: r, ope: w */
	0,	/* CR0: p, CPL: u, PTE: s, PTE: w, ope: r */
	0,	/* CR0: p, CPL: u, PTE: s, PTE: w, ope: w */

	1,	/* CR0: p, CPL: u, PTE: u, PTE: r, ope: r */
	0,	/* CR0: p, CPL: u, PTE: u, PTE: r, ope: w */
	1,	/* CR0: p, CPL: u, PTE: u, PTE: w, ope: r */
	1,	/* CR0: p, CPL: u, PTE: u, PTE: w, ope: w */
};
#endif	/* !USE_PAGE_ACCESS_TABLE */

/*
 *--
 * 32bit ʪ�����ɥ쥹 4k �ڡ���
 *
 * ��˥������ɥ쥹
 *  31                    22 21                  12 11                       0
 * +------------------------+----------------------+--------------------------+
 * |  �ڡ������ǥ��쥯�ȥ�  |   �ڡ������ơ��֥�   |        ���ե��å�        |
 * +------------------------+----------------------+--------------------------+
 *             |                        |                       |
 * +-----------+            +-----------+                       +----------+
 * |                        |                                              |
 * |  �ڡ������ǥ��쥯�ȥ�  |   �ڡ������ơ��֥�            �ڡ���         |
 * | +--------------------+ | +-------------------+   +------------------+ |
 * | |                    | | |                   |   |                  | |
 * | |                    | | +-------------------+   |                  | |
 * | |                    | +>| page table entry  |-+ |                  | |
 * | +--------------------+   +-------------------+ | |                  | |
 * +>|page directory entry|-+ |                   | | +------------------+ |
 *   +--------------------+ | |                   | | | physical address |<+
 *   |                    | | |                   | | +------------------+
 *   |                    | | |                   | | |                  |
 * +>+--------------------+ +>+-------------------+ +>+------------------+
 * |
 * +- CR3(ʪ�����ɥ쥹)
 */

static UINT32 MEMCALL paging(const UINT32 laddr, const int ucrw);
static void MEMCALL tlb_update(const UINT32 laddr, const UINT entry, const int ucrw);

#define	PAGE_SIZE	0x1000
#define	PAGE_MASK	(PAGE_SIZE - 1)

UINT8 MEMCALL
cpu_memory_access_la_RMW_b(UINT32 laddr, UINT32 (*func)(UINT32, void *), void *arg)
{
	const int ucrw = CPU_PAGE_WRITE_DATA|CPU_STAT_USER_MODE;
	UINT32 result, value;
	UINT32 paddr;

	paddr = paging(laddr, ucrw);
	value = cpu_memoryread(paddr);
	result = (*func)(value, arg);
	cpu_memorywrite(paddr, (UINT8)result);

	return value;
}

UINT16 MEMCALL
cpu_memory_access_la_RMW_w(UINT32 laddr, UINT32 (*func)(UINT32, void *), void *arg)
{
	const int ucrw = CPU_PAGE_WRITE_DATA|CPU_STAT_USER_MODE;
	UINT32 result, value;
	UINT32 paddr[2];

	paddr[0] = paging(laddr, ucrw);
	if ((laddr + 1) & PAGE_MASK) {
		value = cpu_memoryread_w(paddr[0]);
		result = (*func)(value, arg);
		cpu_memorywrite_w(paddr[0], (UINT16)result);
	} else {
		paddr[1] = paging(laddr + 1, ucrw);
		value = cpu_memoryread_b(paddr[0]);
		value += (UINT16)cpu_memoryread_b(paddr[1]) << 8;
		result = (*func)(value, arg);
		cpu_memorywrite(paddr[0], (UINT8)result);
		cpu_memorywrite(paddr[1], (UINT8)(result >> 8));
	}
	return value;
}

UINT32 MEMCALL
cpu_memory_access_la_RMW_d(UINT32 laddr, UINT32 (*func)(UINT32, void *), void *arg)
{
	const int ucrw = CPU_PAGE_WRITE_DATA|CPU_STAT_USER_MODE;
	UINT32 result, value;
	UINT32 paddr[2];
	UINT remain;

	paddr[0] = paging(laddr, ucrw);
	remain = PAGE_SIZE - (laddr & PAGE_MASK);
	if (remain >= 4) {
		value = cpu_memoryread_d(paddr[0]);
		result = (*func)(value, arg);
		cpu_memorywrite_d(paddr[0], result);
	} else {
		paddr[1] = paging(laddr + remain, ucrw);
		switch (remain) {
		case 3:
			value = cpu_memoryread(paddr[0]);
			value += (UINT32)cpu_memoryread_w(paddr[0] + 1) << 8;
			value += (UINT32)cpu_memoryread(paddr[1]) << 24;
			result = (*func)(value, arg);
			cpu_memorywrite(paddr[0], (UINT8)result);
			cpu_memorywrite_w(paddr[0] + 1, (UINT16)(result >> 8));
			cpu_memorywrite(paddr[1], (UINT8)(result >> 24));
			break;

		case 2:
			value = cpu_memoryread_w(paddr[0]);
			value += (UINT32)cpu_memoryread_w(paddr[1]) << 16;
			result = (*func)(value, arg);
			cpu_memorywrite_w(paddr[0], (UINT16)result);
			cpu_memorywrite_w(paddr[1], (UINT16)(result >> 16));
			break;

		case 1:
			value = cpu_memoryread(paddr[0]);
			value += (UINT32)cpu_memoryread_w(paddr[1]) << 8;
			value += (UINT32)cpu_memoryread(paddr[1] + 2) << 24;
			result = (*func)(value, arg);
			cpu_memorywrite(paddr[0], (UINT8)result);
			cpu_memorywrite_w(paddr[1], (UINT16)(result >> 8));
			cpu_memorywrite(paddr[1] + 2, (UINT8)(result >> 24));
			break;

		default:
			ia32_panic("cpu_memory_access_la_RMW_d(): out of range (remain = %d)\n", remain);
			return (UINT32)-1;
		}
	}
	return value;
}

UINT8 MEMCALL
cpu_linear_memory_read_b(UINT32 laddr, const int ucrw)
{
	UINT32 paddr;

	paddr = paging(laddr, ucrw);
	return cpu_memoryread(paddr);
}

UINT16 MEMCALL
cpu_linear_memory_read_w(UINT32 laddr, const int ucrw)
{
	UINT32 paddr[2];
	UINT16 value;

	paddr[0] = paging(laddr, ucrw);
	if ((laddr + 1) & PAGE_MASK) {
		return cpu_memoryread_w(paddr[0]);
	} else {
		paddr[1] = paging(laddr + 1, ucrw);
		value = cpu_memoryread_b(paddr[0]);
		value += (UINT16)cpu_memoryread_b(paddr[1]) << 8;
		return value;
	}
}

UINT32 MEMCALL
cpu_linear_memory_read_d(UINT32 laddr, const int ucrw)
{
	UINT32 paddr[2];
	UINT32 value;
	UINT remain;

	paddr[0] = paging(laddr, ucrw);
	remain = PAGE_SIZE - (laddr & PAGE_MASK);
	if (remain >= 4) {
		return cpu_memoryread_d(paddr[0]);
	} else {
		paddr[1] = paging(laddr + remain, ucrw);
		switch (remain) {
		case 3:
			value = cpu_memoryread(paddr[0]);
			value += (UINT32)cpu_memoryread_w(paddr[0] + 1) << 8;
			value += (UINT32)cpu_memoryread(paddr[1]) << 24;
			break;

		case 2:
			value = cpu_memoryread_w(paddr[0]);
			value += (UINT32)cpu_memoryread_w(paddr[1]) << 16;
			break;

		case 1:
			value = cpu_memoryread(paddr[0]);
			value += (UINT32)cpu_memoryread_w(paddr[1]) << 8;
			value += (UINT32)cpu_memoryread(paddr[1] + 2) << 24;
			break;

		default:
			ia32_panic("cpu_linear_memory_read_d(): out of range (remain = %d)\n", remain);
			value = (UINT32)-1;
			break;
		}
		return value;
	}
}

UINT64 MEMCALL
cpu_linear_memory_read_q(UINT32 laddr, const int ucrw)
{
	UINT32 paddr[2];
	UINT64 value;
	UINT remain;

	paddr[0] = paging(laddr, ucrw);
	remain = PAGE_SIZE - (laddr & PAGE_MASK);
	if (remain >= 8) {
		return cpu_memoryread_q(paddr[0]);
	} else {
		paddr[1] = paging(laddr + remain, ucrw);
		switch (remain) {
		case 7:
			value = cpu_memoryread(paddr[0]);
			value += (UINT64)cpu_memoryread_w(paddr[0] + 1) << 8;
			value += (UINT64)cpu_memoryread_d(paddr[0] + 3) << 24;
			value += (UINT64)cpu_memoryread(paddr[1]) << 56;
			break;

		case 6:
			value = cpu_memoryread_w(paddr[0]);
			value += (UINT64)cpu_memoryread_d(paddr[0] + 2) << 16;
			value += (UINT64)cpu_memoryread_w(paddr[1]) << 48;
			break;

		case 5:
			value = cpu_memoryread(paddr[0]);
			value += (UINT64)cpu_memoryread_d(paddr[0] + 1) << 8;
			value += (UINT64)cpu_memoryread_w(paddr[1]) << 40;
			value += (UINT64)cpu_memoryread(paddr[1] + 2) << 56;
			break;

		case 4:
			value = cpu_memoryread_d(paddr[0]);
			value += (UINT64)cpu_memoryread_d(paddr[1]) << 32;
			break;

		case 3:
			value = cpu_memoryread(paddr[0]);
			value += (UINT64)cpu_memoryread_w(paddr[0] + 1) << 8;
			value += (UINT64)cpu_memoryread_d(paddr[1]) << 24;
			value += (UINT64)cpu_memoryread(paddr[1] + 4) << 56;
			break;

		case 2:
			value = cpu_memoryread_w(paddr[0]);
			value += (UINT64)cpu_memoryread_d(paddr[1]) << 16;
			value += (UINT64)cpu_memoryread_w(paddr[1] + 4) << 48;
			break;

		case 1:
			value = cpu_memoryread(paddr[0]);
			value += (UINT64)cpu_memoryread_d(paddr[1]) << 8;
			value += (UINT64)cpu_memoryread_w(paddr[1] + 4) << 40;
			value += (UINT64)cpu_memoryread(paddr[1] + 6) << 56;
			break;

		default:
			ia32_panic("cpu_linear_memory_read_q(): out of range (remain = %d)\n", remain);
			value = (UINT64)-1;
			break;
		}
	}
	return value;
}

REG80 MEMCALL
cpu_linear_memory_read_f(UINT32 laddr, const int ucrw)
{
	UINT32 paddr[2];
	REG80 value;
	UINT remain;
	UINT i, j;

	paddr[0] = paging(laddr, ucrw);
	remain = PAGE_SIZE - (laddr & PAGE_MASK);
	if (remain >= 10) {
		return cpu_memoryread_f(paddr[0]);
	} else {
		paddr[1] = paging(laddr + remain, ucrw);
		for (i = 0; i < remain; ++i) {
			value.b[i] = cpu_memoryread(paddr[0] + i);
		}
		for (j = 0; i < 10; ++i, ++j) {
			value.b[i] = cpu_memoryread(paddr[1] + j);
		}
		return value;
	}
}

void MEMCALL
cpu_linear_memory_write_b(UINT32 laddr, UINT8 value, const int user_mode)
{
	const int ucrw = CPU_PAGE_WRITE_DATA|user_mode;
	UINT32 paddr;

	paddr = paging(laddr, ucrw);
	cpu_memorywrite(paddr, value);
}

void MEMCALL
cpu_linear_memory_write_w(UINT32 laddr, UINT16 value, const int user_mode)
{
	const int ucrw = CPU_PAGE_WRITE_DATA|user_mode;
	UINT32 paddr[2];

	paddr[0] = paging(laddr, ucrw);
	if ((laddr + 1) & PAGE_MASK) {
		cpu_memorywrite_w(paddr[0], value);
	} else {
		paddr[1] = paging(laddr + 1, ucrw);
		cpu_memorywrite(paddr[0], (UINT8)value);
		cpu_memorywrite(paddr[1], (UINT8)(value >> 8));
	}
}

void MEMCALL
cpu_linear_memory_write_d(UINT32 laddr, UINT32 value, const int user_mode)
{
	const int ucrw = CPU_PAGE_WRITE_DATA|user_mode;
	UINT32 paddr[2];
	UINT remain;

	paddr[0] = paging(laddr, ucrw);
	remain = PAGE_SIZE - (laddr & PAGE_MASK);
	if (remain >= 4) {
		cpu_memorywrite_d(paddr[0], value);
	} else {
		paddr[1] = paging(laddr + remain, ucrw);
		switch (remain) {
		case 3:
			cpu_memorywrite(paddr[0], (UINT8)value);
			cpu_memorywrite_w(paddr[0] + 1, (UINT16)(value >> 8));
			cpu_memorywrite(paddr[1], (UINT8)(value >> 24));
			break;

		case 2:
			cpu_memorywrite_w(paddr[0], (UINT16)value);
			cpu_memorywrite_w(paddr[1], (UINT16)(value >> 16));
			break;

		case 1:
			cpu_memorywrite(paddr[0], (UINT8)value);
			cpu_memorywrite_w(paddr[1], (UINT16)(value >> 8));
			cpu_memorywrite(paddr[1] + 2, (UINT8)(value >> 24));
			break;
		}
	}
}

void MEMCALL
cpu_linear_memory_write_q(UINT32 laddr, UINT64 value, const int user_mode)
{
	const int ucrw = CPU_PAGE_WRITE_DATA|user_mode;
	UINT32 paddr[2];
	UINT remain;

	paddr[0] = paging(laddr, ucrw);
	remain = PAGE_SIZE - (laddr & PAGE_MASK);
	if (remain >= 8) {
		cpu_memorywrite_q(paddr[0], value);
	} else {
		paddr[1] = paging(laddr + remain, ucrw);
		switch (remain) {
		case 7:
			cpu_memorywrite(paddr[0], (UINT8)value);
			cpu_memorywrite_w(paddr[0] + 1, (UINT16)(value >> 8));
			cpu_memorywrite_d(paddr[0] + 3, (UINT32)(value >> 24));
			cpu_memorywrite(paddr[1], (UINT8)(value >> 56));
			break;

		case 6:
			cpu_memorywrite_w(paddr[0], (UINT16)value);
			cpu_memorywrite_d(paddr[0] + 2, (UINT32)(value >> 16));
			cpu_memorywrite_w(paddr[1], (UINT16)(value >> 48));
			break;

		case 5:
			cpu_memorywrite(paddr[0], (UINT8)value);
			cpu_memorywrite_d(paddr[0] + 1, (UINT32)(value >> 8));
			cpu_memorywrite_w(paddr[1], (UINT16)(value >> 40));
			cpu_memorywrite(paddr[1] + 2, (UINT8)(value >> 56));
			break;

		case 4:
			cpu_memorywrite_d(paddr[0], (UINT32)value);
			cpu_memorywrite_d(paddr[1], (UINT32)(value >> 32));
			break;

		case 3:
			cpu_memorywrite(paddr[0], (UINT8)value);
			cpu_memorywrite_w(paddr[0] + 1, (UINT16)(value >> 8));
			cpu_memorywrite_d(paddr[1], (UINT32)(value >> 24));
			cpu_memorywrite(paddr[1] + 4, (UINT8)(value >> 56));
			break;

		case 2:
			cpu_memorywrite_w(paddr[0], (UINT16)value);
			cpu_memorywrite_d(paddr[1], (UINT32)(value >> 16));
			cpu_memorywrite_w(paddr[1] + 4, (UINT16)(value >> 48));
			break;

		case 1:
			cpu_memorywrite(paddr[0], (UINT8)value);
			cpu_memorywrite_d(paddr[1], (UINT32)(value >> 8));
			cpu_memorywrite_w(paddr[1] + 4, (UINT16)(value >> 40));
			cpu_memorywrite(paddr[1] + 6, (UINT8)(value >> 56));
			break;
		}
	}
}

void MEMCALL
cpu_linear_memory_write_f(UINT32 laddr, const REG80 *value, const int user_mode)
{
	const int ucrw = CPU_PAGE_WRITE_DATA|user_mode;
	UINT32 paddr[2];
	UINT remain;
	UINT i, j;

	paddr[0] = paging(laddr, ucrw);
	remain = PAGE_SIZE - (laddr & PAGE_MASK);
	if (remain >= 10) {
		cpu_memorywrite_f(paddr[0], value);
	} else {
		paddr[1] = paging(laddr + remain, ucrw);
		for (i = 0; i < remain; ++i) {
			cpu_memorywrite(paddr[0] + i, value->b[i]);
		}
		for (j = 0; i < 10; ++i, ++j) {
			cpu_memorywrite(paddr[1] + j, value->b[i]);
		}
	}
}


void MEMCALL
cpu_memory_access_la_region(UINT32 laddr, UINT length, const int ucrw, UINT8 *data)
{
	UINT32 paddr;
	UINT remain;	/* page remain */
	UINT r;

	if (length == 0)
		return;

	remain = PAGE_SIZE - (laddr & PAGE_MASK);
	for (;;) {
		if (!CPU_STAT_PAGING) {
			paddr = laddr;
		} else {
			paddr = paging(laddr, ucrw);
		}

		r = (remain > length) ? length : remain;
		if (!(ucrw & CPU_PAGE_WRITE)) {
			cpu_memoryread_region(paddr, data, r);
		} else {
			cpu_memorywrite_region(paddr, data, r);
		}

		length -= r;
		if (length == 0)
			break;

		data += r;
		laddr += r;
		remain -= r;
		if (remain <= 0) {
			/* next page */
			remain += PAGE_SIZE;
		}
	}
}

UINT32 MEMCALL
laddr2paddr(const UINT32 laddr, const int ucrw)
{

	return paging(laddr, ucrw);
}

static UINT32 MEMCALL
paging(const UINT32 laddr, const int ucrw)
{
	UINT32 paddr;		/* physical address */
	UINT32 pde_addr;	/* page directory entry address */
	UINT32 pde;		/* page directory entry */
	UINT32 pte_addr;	/* page table entry address */
	UINT32 pte;		/* page table entry */
	UINT bit;
	UINT err;
	TLB_ENTRY_T *ep;

	ep = tlb_lookup(laddr, ucrw);
	if (ep != NULL)
		return ep->paddr + (laddr & PAGE_MASK);

	pde_addr = CPU_STAT_PDE_BASE + ((laddr >> 20) & 0xffc);
	pde = cpu_memoryread_d(pde_addr);
	if (!(pde & CPU_PDE_PRESENT)) {
		VERBOSE(("paging: PTE page is not present"));
		VERBOSE(("paging: CPU_CR3 = 0x%08x", CPU_CR3));
		VERBOSE(("paging: laddr = 0x%08x, pde_addr = 0x%08x, pde = 0x%08x", laddr, pde_addr, pde));
		err = 0;
		goto pf_exception;
	}
	if (!(pde & CPU_PDE_ACCESS)) {
		pde |= CPU_PDE_ACCESS;
		cpu_memorywrite_d(pde_addr, pde);
	}

	pte_addr = (pde & CPU_PDE_BASEADDR_MASK) + ((laddr >> 10) & 0xffc);
	pte = cpu_memoryread_d(pte_addr);
	if (!(pte & CPU_PTE_PRESENT)) {
		VERBOSE(("paging: page is not present"));
		VERBOSE(("paging: laddr = 0x%08x, pde_addr = 0x%08x, pde = 0x%08x", laddr, pde_addr, pde));
		VERBOSE(("paging: pte_addr = 0x%08x, pte = 0x%08x", pte_addr, pte));
		err = 0;
		goto pf_exception;
	}
	if (!(pte & CPU_PTE_ACCESS)) {
		pte |= CPU_PTE_ACCESS;
		cpu_memorywrite_d(pte_addr, pte);
	}

	/* make physical address */
	paddr = (pte & CPU_PTE_BASEADDR_MASK) + (laddr & PAGE_MASK);

	bit  = ucrw & (CPU_PAGE_WRITE|CPU_PAGE_USER_MODE);
	bit |= (pde & pte & (CPU_PTE_WRITABLE|CPU_PTE_USER_MODE));
	bit |= CPU_STAT_WP;

#if !defined(USE_PAGE_ACCESS_TABLE)
	if (!(page_access & (1 << bit)))
#else
	if (!(page_access_bit[bit]))
#endif
	{
		VERBOSE(("paging: page access violation."));
		VERBOSE(("paging: laddr = 0x%08x, pde_addr = 0x%08x, pde = 0x%08x", laddr, pde_addr, pde));
		VERBOSE(("paging: pte_addr = 0x%08x, pte = 0x%08x", pte_addr, pte));
		VERBOSE(("paging: paddr = 0x%08x, bit = 0x%08x", paddr, bit));
		err = 1;
		goto pf_exception;
	}

	if ((ucrw & CPU_PAGE_WRITE) && !(pte & CPU_PTE_DIRTY)) {
		pte |= CPU_PTE_DIRTY;
		cpu_memorywrite_d(pte_addr, pte);
	}

	tlb_update(laddr, pte, (bit & (CPU_PTE_WRITABLE|CPU_PTE_USER_MODE)) + ((ucrw & CPU_PAGE_CODE) >> 1));

	return paddr;

pf_exception:
	CPU_CR2 = laddr;
	err |= (ucrw & CPU_PAGE_WRITE) << 1;
	err |= (ucrw & CPU_PAGE_USER_MODE) >> 1;
	EXCEPTION(PF_EXCEPTION, err);
	return 0;	/* compiler happy */
}

/* 
 * TLB
 */
#define	TLB_GET_PADDR(ep, addr)	((ep)->paddr + ((addr) & ~CPU_PTE_BASEADDR_MASK))
#define	TLB_SET_PADDR(ep, addr)	((ep)->paddr = (addr) & CPU_PTE_BASEADDR_MASK)

#define	TLB_TAG_SHIFT		TLB_ENTRY_TAG_MAX_SHIFT
#define	TLB_TAG_MASK		(~((1 << TLB_TAG_SHIFT) - 1))
#define	TLB_GET_TAG_ADDR(ep)	((ep)->tag & TLB_TAG_MASK)
#define	TLB_SET_TAG_ADDR(ep, addr) \
do { \
	(ep)->tag &= ~TLB_TAG_MASK; \
	(ep)->tag |= (addr) & TLB_TAG_MASK; \
} while (/*CONSTCOND(*/ 0)

#define	TLB_IS_VALID(ep)	((ep)->tag & TLB_ENTRY_TAG_VALID)
#define	TLB_SET_VALID(ep)	((ep)->tag = TLB_ENTRY_TAG_VALID)
#define	TLB_SET_INVALID(ep)	((ep)->tag = 0)

#define	TLB_IS_WRITABLE(ep)	((ep)->tag & CPU_PTE_WRITABLE)
#define	TLB_IS_USERMODE(ep)	((ep)->tag & CPU_PTE_USER_MODE)
#define	TLB_IS_DIRTY(ep)	((ep)->tag & TLB_ENTRY_TAG_DIRTY)
#if (CPU_FEATURES & CPU_FEATURE_PGE) == CPU_FEATURE_PGE
#define	TLB_IS_GLOBAL(ep)	((ep)->tag & TLB_ENTRY_TAG_GLOBAL)
#else
#define	TLB_IS_GLOBAL(ep)	0
#endif

#define	TLB_SET_TAG_FLAGS(ep, entry, bit) \
do { \
	(ep)->tag |= (entry) & (CPU_PTE_GLOBAL_PAGE|CPU_PTE_DIRTY); \
	(ep)->tag |= (bit) & (CPU_PTE_WRITABLE|CPU_PTE_USER_MODE); \
} while (/*CONSTCOND*/ 0)

#define	NTLB		2	/* 0: DTLB, 1: ITLB */
#define	NENTRY		(1 << 6)
#define	TLB_ENTRY_SHIFT	12
#define	TLB_ENTRY_MASK	(NENTRY - 1)

typedef struct {
	TLB_ENTRY_T	entry[NENTRY];
} TLB_T;

static TLB_T tlb[NTLB];

#if defined(IA32_PROFILE_TLB)
/* profiling */
typedef struct {
	UINT64 tlb_hits;
	UINT64 tlb_misses;
	UINT64 tlb_lookups;
	UINT64 tlb_updates;
	UINT64 tlb_flushes;
	UINT64 tlb_global_flushes;
	UINT64 tlb_entry_flushes;
} TLB_PROFILE_T;

static TLB_PROFILE_T tlb_profile;

#define	PROFILE_INC(v)	tlb_profile.v++
#else	/* !IA32_PROFILE_TLB */
#define	PROFILE_INC(v)
#endif	/* IA32_PROFILE_TLB */


void
tlb_init(void)
{

	memset(tlb, 0, sizeof(tlb));
#if defined(IA32_PROFILE_TLB)
	memset(&tlb_profile, 0, sizeof(tlb_profile));
#endif	/* IA32_PROFILE_TLB */
}

void MEMCALL
tlb_flush(BOOL allflush)
{
	TLB_ENTRY_T *ep;
	int i;
	int n;

	if (allflush) {
		PROFILE_INC(tlb_global_flushes);
	} else {
		PROFILE_INC(tlb_flushes);
	}

	for (n = 0; n < NTLB; n++) {
		for (i = 0; i < NENTRY ; i++) {
			ep = &tlb[n].entry[i];
			if (TLB_IS_VALID(ep) && (allflush || !TLB_IS_GLOBAL(ep))) {
				TLB_SET_INVALID(ep);
				PROFILE_INC(tlb_entry_flushes);
			}
		}
	}
}

void MEMCALL
tlb_flush_page(UINT32 laddr)
{
	TLB_ENTRY_T *ep;
	int idx;
	int n;

	PROFILE_INC(tlb_flushes);

	idx = (laddr >> TLB_ENTRY_SHIFT) & TLB_ENTRY_MASK;

	for (n = 0; n < NTLB; n++) {
		ep = &tlb[n].entry[idx];
		if (TLB_IS_VALID(ep)) {
			if ((laddr & TLB_TAG_MASK) == TLB_GET_TAG_ADDR(ep)) {
				TLB_SET_INVALID(ep);
				PROFILE_INC(tlb_entry_flushes);
			}
		}
	}
}

TLB_ENTRY_T * MEMCALL
tlb_lookup(const UINT32 laddr, const int ucrw)
{
	TLB_ENTRY_T *ep;
	UINT bit;
	int idx;
	int n;

	PROFILE_INC(tlb_lookups);

	n = (ucrw & CPU_PAGE_CODE) >> 1;
	idx = (laddr >> TLB_ENTRY_SHIFT) & TLB_ENTRY_MASK;
	ep = &tlb[n].entry[idx];

	if (TLB_IS_VALID(ep)) {
		if ((laddr & TLB_TAG_MASK) == TLB_GET_TAG_ADDR(ep)) {
			bit = ucrw & (CPU_PAGE_WRITE|CPU_PAGE_USER_MODE);
			bit |= ep->tag & (CPU_PTE_WRITABLE|CPU_PTE_USER_MODE);
			bit |= CPU_STAT_WP;
#if !defined(USE_PAGE_ACCESS_TABLE)
			if ((page_access & (1 << bit)))
#else
			if (page_access_bit[bit])
#endif
			{
				if (!(ucrw & CPU_PAGE_WRITE) || TLB_IS_DIRTY(ep)) {
					PROFILE_INC(tlb_hits);
					return ep;
				}
			}
		}
	}
	PROFILE_INC(tlb_misses);
	return NULL;
}

static void MEMCALL
tlb_update(const UINT32 laddr, const UINT entry, const int bit)
{
	TLB_ENTRY_T *ep;
	UINT32 pos;
	int idx;
	int n;

	PROFILE_INC(tlb_updates);

	n = bit & 1;
	idx = (laddr >> TLB_ENTRY_SHIFT) & TLB_ENTRY_MASK;
	ep = &tlb[n].entry[idx];

	TLB_SET_VALID(ep);
	TLB_SET_TAG_ADDR(ep, laddr);
	TLB_SET_PADDR(ep, entry);
	TLB_SET_TAG_FLAGS(ep, entry, bit);

	if (ep->paddr < CPU_MEMREADMAX) {
		ep->memp = mem + ep->paddr;
		return;
	} else if (ep->paddr >= USE_HIMEM) {
		pos = (ep->paddr & CPU_ADRSMASK) - 0x100000;
		if (pos < CPU_EXTMEMSIZE) {
			ep->memp = CPU_EXTMEM + pos;
			return;
		}
	}
	ep->memp = NULL;
}

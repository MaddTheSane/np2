/*	$Id: paging.c,v 1.19 2004/03/23 19:32:37 yui Exp $	*/

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

/*
 * ページフォルト例外
 *
 * 4-31: 予約済み
 *    3: RSVD: 0 = フォルトの原因は予約ビット違反ではなかった．
 *             1 = ページ・フォルトの原因は，違反とマークされた PTE または
 *                 PDE の予約ビット位置のうち一つで，1 が検出されたことである．
 *    2: U/S:  0 = フォルトの原因となったアクセスはプロセッサがスーパバイザ・
 *                 モードで実行中に行われた．
 *             1 = フォルトの原因となったアクセスはプロセッサがユーザ・モードで
 *                 実行中に行われた．
 *    1: W/R:  0 = フォルトの原因となったアクセスが読み取りであった．
 *             1 = フォルトの原因となったアクセスが書き込みであった．
 *    0: P:    0 = フォルトの原因が不在ページであった．
 *             1 = フォルトの原因がページ・レベル保護違反であった．
 */

/*
 * 下巻 4.12. ページ保護とセグメント保護の組み合わせ
 * 「表 4-2. ページ・ディレクトリとページ・テーブルの保護の組み合わせ」
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
 * ※ rw/p : CR0 の WP ビットが ON の場合には ro
 */

/*
 * メモリアクセス/PxE(上記参照)/CPL/CR0 とページアクセス権の関係
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
 * 32bit 物理アドレス 4k ページ
 *
 * リニア・アドレス
 *  31                    22 21                  12 11                       0
 * +------------------------+----------------------+--------------------------+
 * |  ページ・ディレクトリ  |   ページ・テーブル   |        オフセット        |
 * +------------------------+----------------------+--------------------------+
 *             |                        |                       |
 * +-----------+            +-----------+                       +----------+
 * |                        |                                              |
 * |  ページ・ディレクトリ  |   ページ・テーブル            ページ         |
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
 * +- CR3(物理アドレス)
 */

static UINT32 paging(const UINT32 laddr, const int crw, const int user_mode);
#if defined(IA32_SUPPORT_TLB)
static BOOL tlb_lookup(const UINT32 vaddr, const int crw, UINT32 *paddr);
static void tlb_update(const UINT32 laddr, const UINT entry, const int crw);
#endif


void MEMCALL
cpu_memory_access_la_region(UINT32 laddr, UINT length, const int crw, const int user_mode, BYTE *data)
{
	UINT32 paddr;
	UINT remain;	/* page remain */
	UINT r;

	if (length == 0)
		return;

	remain = 0x1000 - (laddr & 0x00000fff);
	for (;;) {
		if (!CPU_STAT_PAGING) {
			paddr = laddr;
		} else {
			paddr = paging(laddr, crw, user_mode);
		}

		r = (remain > length) ? length : remain;
		if (!(crw & CPU_PAGE_WRITE)) {
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
			remain += 0x1000;
		}
	}
}

UINT32 MEMCALL
cpu_memory_access_la_RMW(UINT32 laddr, UINT length, const int user_mode, UINT32 (*func)(UINT32, void *), void *arg)
{
	const int crw = (CPU_PAGE_WRITE|CPU_PAGE_DATA);
	UINT32 result, value;
	UINT32 paddr[2];
	UINT remain;

	paddr[0] = paging(laddr, crw, user_mode);
	remain = 0x1000 - (laddr & 0x00000fff);
	if (remain >= length) {
		/* fast mode */
		switch (length) {
		case 4:
			value = cpu_memoryread_d(paddr[0]);
			result = (*func)(value, arg);
			cpu_memorywrite_d(paddr[0], result);
			break;

		case 2:
			value = cpu_memoryread_w(paddr[0]);
			result = (*func)(value, arg);
			cpu_memorywrite_w(paddr[0], (UINT16)result);
			break;

		case 1:
			value = cpu_memoryread(paddr[0]);
			result = (*func)(value, arg);
			cpu_memorywrite(paddr[0], (UINT8)result);
			break;

		default:
			ia32_panic("cpu_memory_access_la_RMW(): invalid length (length = %d)\n", length);
			value = 0;	/* compiler happy */
			break;
		}
		return value;
	}

	/* slow mode */
	paddr[1] = paging(laddr + remain, crw, user_mode);
	switch (remain) {
	case 3:
		value = cpu_memoryread(paddr[0]);
		value += (UINT32)cpu_memoryread_w(paddr[0] + 1) << 8;
		value += (UINT32)cpu_memoryread(paddr[1]) << 24;
		result = (*func)(value, arg);
		cpu_memorywrite(paddr[0], (UINT8)result);
		cpu_memorywrite_w(paddr[0] + 1, (UINT16)(result >> 8));
		cpu_memorywrite(paddr[1], (BYTE)(result >> 24));
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
		value += (UINT32)cpu_memoryread(paddr[1]) << 8;
		if (length == 4) {
			value += (UINT32)cpu_memoryread_w(paddr[1] + 1) << 16;
		}
		result = (*func)(value, arg);
		cpu_memorywrite(paddr[0], (UINT8)result);
		cpu_memorywrite(paddr[1], (UINT8)(result >> 8));
		if (length == 4) {
			cpu_memorywrite_w(paddr[1] + 1, (UINT16)(result >> 16));
		}
		break;

	default:
		ia32_panic("cpu_memory_access_la_RMW(): out of range (remain = %d)\n", remain);
		value = 0;	/* compiler happy */
		break;
	}
	return value;
}


#if defined(IA32_PAGING_EACHSIZE)
UINT8 MEMCALL
cpu_linear_memory_read_b(UINT32 laddr, const int crw, const int user_mode)
{
	UINT32 addr;

	addr = paging(laddr, crw, user_mode);
	return(cpu_memoryread(addr));
}

UINT16 MEMCALL
cpu_linear_memory_read_w(UINT32 laddr, const int crw, const int user_mode)
{
	UINT32 addr;
	UINT32 addr2;
	UINT16 value;

	addr = paging(laddr, crw, user_mode);
	if ((laddr + 1) & 0x00000fff) {
		return(cpu_memoryread_w(addr));
	}
	else {
		addr2 = paging(laddr + 1, crw, user_mode);
		value = cpu_memoryread_b(addr);
		value += (UINT16)cpu_memoryread_b(addr2) << 8;
		return(value);
	}
}

UINT32 MEMCALL
cpu_linear_memory_read_d(UINT32 laddr, const int crw, const int user_mode)
{
	UINT32 addr;
	UINT remain;
	UINT32 addr2;
	UINT32 value;

	addr = paging(laddr, crw, user_mode);
	remain = 0x1000 - (laddr & 0x00000fff);
	if (remain >= 4) {
		return(cpu_memoryread_d(addr));
	}
	else {
		addr2 = paging(laddr + remain, crw, user_mode);
		switch (remain) {
			case 3:
				value = cpu_memoryread(addr);
				value += (UINT32)cpu_memoryread_w(addr + 1) << 8;
				value += (UINT32)cpu_memoryread(addr2) << 24;
				break;

			case 2:
				value = cpu_memoryread_w(addr);
				value += (UINT32)cpu_memoryread_w(addr2) << 16;
				break;

			case 1:
				value = cpu_memoryread(addr);
				value += (UINT32)cpu_memoryread(addr2) << 8;
				value += (UINT32)cpu_memoryread_w(addr2 + 1) << 16;
				break;

			default:
				ia32_panic("cpu_linear_memory_read_d(): out of range (remain = %d)\n", remain);
				value = 0;	/* compiler happy */
				break;
		}
		return value;
	}
}

void MEMCALL
cpu_linear_memory_write_b(UINT32 laddr, UINT8 value, const int user_mode)
{
	const int crw = (CPU_PAGE_WRITE|CPU_PAGE_DATA);
	UINT32 addr;

	addr = paging(laddr, crw, user_mode);
	cpu_memorywrite(addr, value);
}

void MEMCALL
cpu_linear_memory_write_w(UINT32 laddr, UINT16 value, const int user_mode)
{
	const int crw = (CPU_PAGE_WRITE|CPU_PAGE_DATA);
	UINT32 addr;
	UINT32 addr2;

	addr = paging(laddr, crw, user_mode);
	if ((laddr + 1) & 0x00000fff) {
		cpu_memorywrite_w(addr, value);
	}
	else {
		addr2 = paging(laddr + 1, crw, user_mode);
		cpu_memorywrite(addr, (UINT8)value);
		cpu_memorywrite(addr2, (UINT8)(value >> 8));
	}
}

void MEMCALL
cpu_linear_memory_write_d(UINT32 laddr, UINT32 value, const int user_mode)
{
	const int crw = (CPU_PAGE_WRITE|CPU_PAGE_DATA);
	UINT32 addr;
	UINT remain;
	UINT32 addr2;

	addr = paging(laddr, crw, user_mode);
	remain = 0x1000 - (laddr & 0x00000fff);
	if (remain >= 4) {
		cpu_memorywrite_d(addr, value);
	}
	else {
		addr2 = paging(laddr + remain, crw, user_mode);
		switch (remain) {
			case 3:
				cpu_memorywrite(addr, (UINT8)value);
				cpu_memorywrite_w(addr + 1, (UINT16)(value >> 8));
				cpu_memorywrite(addr2, (UINT8)(value >> 24));
				break;

			case 2:
				cpu_memorywrite_w(addr, (UINT16)value);
				cpu_memorywrite_w(addr2, (UINT16)(value >> 16));
				break;

			case 1:
				cpu_memorywrite(addr, (UINT8)value);
				cpu_memorywrite(addr2, (UINT8)(value >> 8));
				cpu_memorywrite_w(addr2 + 1, (UINT16)(value >> 16));
				break;
		}
	}
}
#else	// IA32_PAGING_EACHSIZE
UINT32 MEMCALL
cpu_linear_memory_read(UINT32 laddr, UINT length, const int crw, const int user_mode)
{
	UINT32 value;
	UINT32 paddr[2];
	UINT remain;

	paddr[0] = paging(laddr, crw, user_mode);
	remain = 0x1000 - (laddr & 0x00000fff);
	if (remain >= length) {
		/* fast mode */
		switch (length) {
		case 4:
			value = cpu_memoryread_d(paddr[0]);
			break;

		case 2:
			value = cpu_memoryread_w(paddr[0]);
			break;

		case 1:
			value = cpu_memoryread(paddr[0]);
			break;

		default:
			ia32_panic("cpu_linear_memory_read(): invalid length (length = %d)\n", length);
			value = 0;	/* compiler happy */
			break;
		}
		return value;
	}

	/* slow mode */
	paddr[1] = paging(laddr + remain, crw, user_mode);
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
		value += (UINT32)cpu_memoryread(paddr[1]) << 8;
		if (length == 4) {
			value += (UINT32)cpu_memoryread_w(paddr[1] + 1) << 16;
		}
		break;

	default:
		ia32_panic("cpu_linear_memory_read(): out of range (remain = %d)\n", remain);
		value = 0;	/* compiler happy */
		break;
	}
	return value;
}

void MEMCALL
cpu_linear_memory_write(UINT32 laddr, UINT32 value, UINT length, const int user_mode)
{
	const int crw = (CPU_PAGE_WRITE|CPU_PAGE_DATA);
	UINT32 paddr[2];
	UINT remain;

	paddr[0] = paging(laddr, crw, user_mode);
	remain = 0x1000 - (laddr & 0x00000fff);
	if (remain >= length) {
		/* fast mode */
		switch (length) {
		case 4:
			cpu_memorywrite_d(paddr[0], value);
			break;

		case 2:
			cpu_memorywrite_w(paddr[0], (UINT16)value);
			break;

		case 1:
			cpu_memorywrite(paddr[0], (UINT8)value);
			break;

		default:
			ia32_panic("cpu_linear_memory_write(): invalid length (length = %d)\n", length);
			break;
		}
		return;
	}

	/* slow mode */
	paddr[1] = paging(laddr + remain, crw, user_mode);
	switch (remain) {
	case 3:
		cpu_memorywrite(paddr[0], (UINT8)value);
		cpu_memorywrite_w(paddr[0] + 1, (UINT16)(value >> 8));
		cpu_memorywrite(paddr[1], (BYTE)(value >> 24));
		break;

	case 2:
		cpu_memorywrite_w(paddr[0], (UINT16)value);
		cpu_memorywrite_w(paddr[1], (UINT16)(value >> 16));
		break;

	case 1:
		cpu_memorywrite(paddr[0], (UINT8)value);
		cpu_memorywrite(paddr[1], (UINT8)(value >> 8));
		if (length == 4) {
			cpu_memorywrite_w(paddr[1] + 1, (UINT16)(value >> 16));
		}
		break;

	default:
		ia32_panic("cpu_linear_memory_write(): out of range (remain = %d)\n", remain);
		break;
	}
}
#endif	// IA32_PAGING_EACHSIZE

void MEMCALL
paging_check(UINT32 laddr, UINT length, const int crw, const int user_mode)
{
	UINT32 paddr;
	UINT remain;	/* page remain */
	UINT r;

	remain = 0x1000 - (laddr & 0x00000fff);
	for (;;) {
		paddr = paging(laddr, crw, user_mode);

		r = (remain > length) ? length : remain;

		length -= r;
		if (length == 0)
			break;

		laddr += r;
		remain -= r;
		if (remain <= 0) {
			/* next page */
			remain += 0x1000;
		}
	}
}

static UINT32
paging(const UINT32 laddr, const int crw, const int user_mode)
{
	UINT32 paddr;		/* physical address */
	UINT32 pde_addr;	/* page directory entry address */
	UINT32 pde;		/* page directory entry */
	UINT32 pte_addr;	/* page table entry address */
	UINT32 pte;		/* page table entry */
	UINT bit;
	UINT err;

#if defined(IA32_SUPPORT_TLB)
	if (tlb_lookup(laddr, crw, &paddr))
		return paddr;
#endif	/* IA32_SUPPORT_TLB */

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
	paddr = (pte & CPU_PTE_BASEADDR_MASK) + (laddr & 0x00000fff);

	bit  = crw & CPU_PAGE_WRITE;
	bit |= (pde & pte & (CPU_PTE_WRITABLE|CPU_PTE_USER_MODE));
	bit |= (user_mode << 3);
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

	if ((crw & CPU_PAGE_WRITE) && !(pte & CPU_PTE_DIRTY)) {
		pte |= CPU_PTE_DIRTY;
		cpu_memorywrite_d(pte_addr, pte);
	}

#if defined(IA32_SUPPORT_TLB)
	tlb_update(laddr, pte, crw);
#endif	/* IA32_SUPPORT_TLB */

	return paddr;

pf_exception:
	CPU_CR2 = laddr;
	err |= ((crw & CPU_PAGE_WRITE) << 1) | (user_mode << 2);
	EXCEPTION(PF_EXCEPTION, err);
	return 0;	/* compiler happy */
}

#if defined(IA32_SUPPORT_TLB)
/* 
 * TLB
 */

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


typedef struct {
	UINT32	tag;	/* linear address */
#define	TLB_ENTRY_VALID		(1 << 0)
#define	TLB_ENTRY_GLOBAL	CPU_PTE_GLOBAL_PAGE

	UINT32	paddr;	/* physical address */
} TLB_ENTRY_T;

#define	TLB_GET_PADDR(ep, addr)	((ep)->paddr + ((addr) & ~CPU_PTE_BASEADDR_MASK))
#define	TLB_SET_PADDR(ep, addr)	((ep)->paddr = (addr) & CPU_PTE_BASEADDR_MASK)

#define	TLB_TAG_SHIFT	17
#define	TLB_TAG_MASK	~((1 << TLB_TAG_SHIFT) - 1)
#define	TLB_GET_TAG_ADDR(ep)	((ep)->tag & TLB_TAG_MASK)
#define	TLB_SET_TAG_ADDR(ep, addr) \
	((ep)->tag = ((addr) & TLB_TAG_MASK) + ((ep)->tag & ~TLB_TAG_MASK))

#define	TLB_IS_VALID(ep)	((ep)->tag & TLB_ENTRY_VALID)
#define	TLB_SET_VALID(ep)	((ep)->tag |= TLB_ENTRY_VALID)
#define	TLB_CLEAR_VALID(ep)	((ep)->tag &= ~TLB_ENTRY_VALID)

#if CPU_FAMILY == 4
#define	TLB_IS_GLOBAL(ep)	FALSE
#define	TLB_SET_GLOBAL(ep)	(void)(ep)
#define	TLB_CLEAR_GLOBAL(ep)	(void)(ep)
#else
#define	TLB_IS_GLOBAL(ep)	((ep)->tag & TLB_ENTRY_GLOBAL)
#define	TLB_SET_GLOBAL(ep)	((ep)->tag |= TLB_ENTRY_GLOBAL)
#define	TLB_CLEAR_GLOBAL(ep)	((ep)->tag &= ~TLB_ENTRY_GLOBAL)
#endif


#if CPU_FAMILY == 4
#define	NTLB	1
#define	NENTRY	(1 << 3)
#define	NWAY	(1 << 2)

#define	TLB_ENTRY_SHIFT	12
#define	TLB_ENTRY_MASK	(NENTRY - 1)
#define	TLB_WAY_SHIFT	15
#define	TLB_WAY_MASK	(NWAY - 1)
#endif

typedef struct {
	TLB_ENTRY_T	entry[NENTRY][NWAY];
} TLB_T;

static TLB_T tlb;


void
tlb_init(void)
{

	memset(&tlb, 0, sizeof(tlb));
#if defined(IA32_PROFILE_TLB)
	memset(&tlb_profile, 0, sizeof(tlb_profile));
#endif	/* IA32_PROFILE_TLB */
}

void
tlb_flush(BOOL allflush)
{
	TLB_ENTRY_T *ep;
	int i, j;

	if (allflush) {
		PROFILE_INC(tlb_global_flushes);
	} else {
		PROFILE_INC(tlb_flushes);
	}

	for (i = 0; i < NENTRY ; i++) {
		for (j = 0; j < NWAY; j++) {
			ep = &tlb.entry[i][j];
			if (TLB_IS_VALID(ep) && (!TLB_IS_GLOBAL(ep) || allflush)) {
				TLB_CLEAR_VALID(ep);
				PROFILE_INC(tlb_entry_flushes);
			}
		}
	}
}

void
tlb_flush_page(UINT32 laddr)
{
	TLB_ENTRY_T *ep;
	int idx;
	int way;

	PROFILE_INC(tlb_flushes);

	idx = (laddr >> TLB_ENTRY_SHIFT) & (NENTRY - 1);
	way = (laddr >> TLB_WAY_SHIFT) & (NWAY - 1);
	ep = &tlb.entry[idx][way];

	if (TLB_IS_VALID(ep)) {
		if ((laddr & TLB_TAG_MASK) == TLB_GET_TAG_ADDR(ep)) {
			TLB_CLEAR_VALID(ep);
			return;
		}
	}
}

static BOOL
tlb_lookup(const UINT32 laddr, const int crw, UINT32 *paddr)
{
	TLB_ENTRY_T *ep;
	int idx;
	int way;

	PROFILE_INC(tlb_lookups);

	idx = (laddr >> TLB_ENTRY_SHIFT) & (NENTRY - 1);
	way = (laddr >> TLB_WAY_SHIFT) & (NWAY - 1);
	ep = &tlb.entry[idx][way];

	ep = &tlb.entry[idx][way];
	if (TLB_IS_VALID(ep)) {
		if ((laddr & TLB_TAG_MASK) == TLB_GET_TAG_ADDR(ep)) {
			*paddr = TLB_GET_PADDR(ep, laddr);
			PROFILE_INC(tlb_hits);
			return TRUE;
		}
	}
	(void)crw;
	PROFILE_INC(tlb_misses);
	return FALSE;
}

static void
tlb_update(const UINT32 laddr, const UINT entry, const int crw)
{
	TLB_ENTRY_T *ep;
	int idx;
	int way;

	PROFILE_INC(tlb_updates);

	idx = (laddr >> TLB_ENTRY_SHIFT) & (NENTRY - 1);
	way = (laddr >> TLB_WAY_SHIFT) & (NWAY - 1);
	ep = &tlb.entry[idx][way];

	TLB_SET_VALID(ep);
#if CPU_FAMILY >= 5
	if (entry & CPU_PTE_GLOBAL_PAGE) {
		TLB_SET_GLOBAL(ep);
	}
#endif
	TLB_SET_TAG_ADDR(ep, laddr);
	TLB_SET_PADDR(ep, entry);
	(void)crw;
}
#endif	/* IA32_SUPPORT_TLB */

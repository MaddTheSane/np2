/*	$Id: paging.c,v 1.11 2004/02/05 16:43:44 monaka Exp $	*/

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
static const BYTE page_access_bit[32] = {
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

static DWORD paging(DWORD laddr, int crw, int user_mode);
#if defined(IA32_SUPPORT_TLB)
static BOOL tlb_lookup(DWORD vaddr, int crw, DWORD* paddr);
static void tlb_update(DWORD paddr, DWORD entry, int crw);
#endif


DWORD MEMCALL
cpu_linear_memory_read(DWORD laddr, DWORD length, int crw, int user_mode)
{
	DWORD paddr;
	DWORD remain;	/* page remain */
	DWORD r;
	DWORD shift = 0;
	DWORD value = 0;

	/* XXX: 4MB pages... */
	remain = 0x1000 - (laddr & 0x00000fff);
	for (;;) {
		paddr = paging(laddr, crw, user_mode);

		r = (remain > length) ? length : remain;
		switch (r) {
		case 4:
			value = cpu_memoryread_d(paddr);
			break;

		case 3:
			value += (DWORD)cpu_memoryread(paddr) << shift;
			shift += 8;
			paddr++;
			/*FALLTHROUGH*/
		case 2:
			value += (DWORD)cpu_memoryread_w(paddr) << shift;
			shift += 16;
			break;

		case 1:
			value += (DWORD)cpu_memoryread(paddr) << shift;
			shift += 8;
			break;

		default:
			ia32_panic("cpu_linear_memory_read(): out of range (r = %d)\n", r);
			break;
		}

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

	return value;
}

void MEMCALL
cpu_linear_memory_write(DWORD laddr, DWORD value, DWORD length, int user_mode)
{
	DWORD paddr;
	DWORD remain;	/* page remain */
	DWORD r;
	int crw = (CPU_PAGE_WRITE|CPU_PAGE_DATA);

	/* XXX: 4MB pages... */
	remain = 0x1000 - (laddr & 0x00000fff);
	for (;;) {
		paddr = paging(laddr, crw, user_mode);

		r = (remain > length) ? length : remain;
		switch (r) {
		case 4:
			cpu_memorywrite_d(paddr, value);
			break;

		case 3:
			cpu_memorywrite(paddr, value & 0xff);
			value >>= 8;
			paddr++;
			/*FALLTHROUGH*/
		case 2:
			cpu_memorywrite_w(paddr, value & 0xffff);
			value >>= 16;
			break;

		case 1:
			cpu_memorywrite(paddr, value & 0xff);
			value >>= 8;
			break;

		default:
			ia32_panic("cpu_linear_memory_write(): out of range (r = %d)\n", r);
			break;
		}

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

void MEMCALL
paging_check(DWORD laddr, DWORD length, int crw, int user_mode)
{
	DWORD paddr;
	DWORD remain;	/* page remain */
	DWORD r;

	/* XXX: 4MB pages... */
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

static DWORD
paging(DWORD laddr, int crw, int user_mode)
{
	DWORD paddr;	/* physical address */
	DWORD pde_addr;	/* page directory entry address */
	DWORD pde;	/* page directory entry */
	DWORD pte_addr;	/* page table entry address */
	DWORD pte;	/* page table entry */
	DWORD bit;
	DWORD err;

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

#if CPU_FAMILY >= 5
	/* no support PAE */
	__ASSERT(!(CPU_CR4 & CPU_CR4_PAE));

	if ((CPU_CR4 & CPU_CR4_PSE) && (pde & CPU_PDE_PAGE_SIZE)) {
		/* 4MB page size */

		/* fake PTE bit */
		pte = pde | CPU_PTE_DIRTY;
		pte_addr = 0;	/* compiler happy */

		/* make physical address */
		paddr = (pde & CPU_PDE_4M_BASEADDR_MASK) + (laddr & 0x003fffff);
	} else
#endif	/* CPU_FAMILY >= 5 */
	{
		/* 4KB page size */
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
	}

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
	tlb_update(paddr, pte, crw);
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
typedef struct {
	BYTE	valid;	/* TLB entry is valid */
	BYTE	global;	/* this TLB entry is global */
	BYTE	score;
	BYTE	pad;

	DWORD	tag;
	DWORD	mask;	/* 4K or 2M or 4M */

	DWORD	paddr;	/* physical addr */
} TLB_ENTRY_T;

typedef struct {
	BYTE		kind;
#define	TLB_KIND_INSTRUCTION	(1 << 1)
#define	TLB_KIND_DATA		(1 << 2)
#define	TLB_KIND_COMBINE	(TLB_KIND_INSTRUCTION|TLB_KIND_DATA)
#define	TLB_KIND_SMALL		(1 << 3)
#define	TLB_KIND_LARGE		(1 << 4)
#define	TLB_KIND_BOTH		(TLB_KIND_SMALL|TLB_KIND_LARGE)

	BYTE		way;	/* n-way associative */
	BYTE		idx;	/* number of TLB index */
	BYTE		bpad;

	WORD		num;	/* number of TLB entry */
	WORD		wpad;

	TLB_ENTRY_T*	entry;	/* entry[assoc][idx] or entry[assoc] if idx == 1*/
} TLB_T;

static int ntlb;
static TLB_T tlb[4];	/* i TLB, i (lp) TLB, d TLB, d (lp) TLB */

#if defined(IA32_PROFILE_TLB)
/* profiling */
static DWORD tlb_hits;
static DWORD tlb_misses;
static DWORD tlb_lookups;
static DWORD tlb_updates;
static DWORD tlb_flushes;
static DWORD tlb_global_flushes;
static DWORD tlb_entry_flushes;

#define	PROFILE_INC(v)	(v)++;
#else	/* !IA32_PROFILE_TLB */
#define	PROFILE_INC(v)
#endif	/* IA32_PROFILE_TLB */

void
tlb_init()
{
	int i;

	for (i = 0; i < NELEMENTS(tlb); i++) {
		if (tlb[i].entry) {
			free(tlb[i].entry);
		}
	}
	memset(tlb, 0, sizeof(tlb));

#if defined(IA32_PROFILE_TLB)
	tlb_hits = 0;
	tlb_misses = 0;
	tlb_lookups = 0;
	tlb_updates = 0;
	tlb_flushes = 0;
	tlb_global_flushes = 0;
	tlb_entry_flushes = 0;
#endif	/* IA32_PROFILE_TLB */

#if CPU_FAMILY == 4
	/* �Ȥꤢ���� i486 �����ǡ� */
	/* combine (I/D) TLB: 4KB Pages, 4-way set associative 32 entries */
	ntlb = 1;
	tlb[0].kind = TLB_KIND_COMBINE | TLB_KIND_SMALL;
	tlb[0].num = 32;
	tlb[0].way = 4;
#endif

	for (i = 0; i < ntlb; i++) {
		tlb[i].idx = tlb[i].num / tlb[i].way;

		tlb[i].entry = (TLB_ENTRY_T*)calloc(sizeof(TLB_ENTRY_T), tlb[i].num);
		if (tlb[i].entry == 0) {
			ia32_panic("tlb_init(): can't alloc TLB entry\n");
		}
	}
}

void
tlb_flush(BOOL allflush)
{
	TLB_ENTRY_T* ep;
	int i, j;

	if (allflush) {
		PROFILE_INC(tlb_global_flushes);
	} else {
		PROFILE_INC(tlb_flushes);
	}

	for (i = 0; i < ntlb; i++) {
		ep = tlb[i].entry;
		for (j = 0; j < tlb[i].num; j++, ep++) {
			if (ep->valid && (allflush || !ep->global)) {
				ep->valid = 0;
				PROFILE_INC(tlb_entry_flushes);
			}
		}
	}
}

void
tlb_flush_page(DWORD vaddr)
{
	TLB_ENTRY_T* ep;
	int idx;
	int i;

	for (i = 0; i < ntlb; i++) {
		if (tlb[i].idx == 1) {
			/* fully set associative */
			idx = 0;
		} else {
			if (tlb[i].kind & TLB_KIND_SMALL) {
				idx = (vaddr >> 12) & (tlb[i].idx - 1);
			} else {
				idx = (vaddr >> 22) & (tlb[i].idx - 1);
			}
		}

		/* search */
		ep = &tlb[i].entry[idx * tlb[i].way];
		for (i = 0; i < tlb[i].way; i++) {
			if (ep->valid) {
				if ((vaddr & ep->mask) == ep->tag) {
					ep->valid = 0;
					PROFILE_INC(tlb_entry_flushes);
					break;
				}
			}
		}
	}
}

static BOOL
tlb_lookup(DWORD laddr, int crw, DWORD* paddr)
{
	TLB_ENTRY_T* ep;
	int idx;
	int i;

	PROFILE_INC(tlb_lookups);

	crw &= CPU_PAGE_CODE | CPU_PAGE_DATA;
	for (i = 0; i < ntlb; i++) {
		if (tlb[i].kind & crw) {
			if (tlb[i].idx == 1) {
				/* fully set associative */
				idx = 0;
			} else {
				if (tlb[i].kind & TLB_KIND_SMALL) {
					idx = (laddr >> 12) & (tlb[i].idx - 1);
				} else {
					idx = (laddr >> 22) & (tlb[i].idx - 1);
				}
			}

			/* search */
			ep = &tlb[i].entry[idx * tlb[i].way];
			for (i = 0; i < tlb[i].way; i++) {
				if (ep->valid) {
					if ((laddr & ep->mask) == ep->tag) {
						if (ep->score != (BYTE)~0)
							ep->score++;
						*paddr = ep->paddr;
						PROFILE_INC(tlb_hits);
						return TRUE;
					}
				}
			}
		}
	}
	PROFILE_INC(tlb_misses);
	return FALSE;
}

static void
tlb_update(DWORD paddr, DWORD entry, int crw)
{
	TLB_ENTRY_T* ep;
	int idx;
	int i, j;
	int min_way;
	WORD min_score = ~0;

	PROFILE_INC(tlb_updates);

	crw &= CPU_PAGE_CODE | CPU_PAGE_DATA;
	for (i = 0; i < ntlb; i++) {
		if (tlb[i].kind & crw) {
			if (tlb[i].idx == 1) {
				/* fully set associative */
				idx = 0;
			} else {
				/* n-way set associative */
				if (!(entry & CPU_PDE_PAGE_SIZE)) {
					if (!(tlb[i].kind & TLB_KIND_SMALL))
						continue;
					idx = (entry >> 12) & (tlb[i].idx - 1);
				} else {
					if (!(tlb[i].kind & TLB_KIND_LARGE))
						continue;
					idx = (entry >> 22) & (tlb[i].idx - 1);
				}
			}

			/* search */
			ep = &tlb[i].entry[idx * tlb[i].way];
			for (min_way = 0, j = 0; j < tlb[i].way; j++, ep++) {
				if (ep->valid) {
					if (min_score >= ep->score) {
						min_way = j;
						min_score = ep->score;
					}
				} else {
					min_way = j;
					min_score = 0;
					break;
				}
			}

			/* replace */
			ep = &tlb[i].entry[idx * tlb[i].way + min_way];
			ep->valid = 1;
			ep->global = (entry & CPU_PTE_GLOBAL_PAGE) ? 1 : 0;
			ep->score = 0;
			ep->mask = (entry & CPU_PDE_PAGE_SIZE) ?  CPU_PDE_4M_BASEADDR_MASK : CPU_PTE_BASEADDR_MASK;
			ep->tag = entry & ep->mask;
			ep->paddr = paddr;
			break;
		}
	}
	__ASSERT(i != ntlb);
}
#endif	/* IA32_SUPPORT_TLB */

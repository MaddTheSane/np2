/*	$Id: paging.h,v 1.5 2004/01/27 15:56:57 monaka Exp $	*/

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

#ifndef	IA32_CPU_PAGING_H__
#define	IA32_CPU_PAGING_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ページ・ディレクトリ・エントリ (4K バイトページ使用時)
 *
 *  31                                    12 11   9 8  7 6 5  4   3   2   1  0 
 * +----------------------------------------+------+-+--+-+-+---+---+---+---+-+
 * |   ページ・テーブルのベース・アドレス   |使用可|G|PS|0|A|PCD|PWT|U/S|R/W|P|
 * +----------------------------------------+------+-+--+-+-+---+---+---+---+-+
 *                                              |   |  | | |  |   |   |   |  |
 * 9-11: システム・プログラマが使用可能 --------+   |  | | |  |   |   |   |  |
 *    8: グローバル・ページ(無視される) ------------+  | | |  |   |   |   |  |
 *    7: ページ・サイズ (0 = 4k バイトページ) ---------+ | |  |   |   |   |  |
 *    6: 予約 (0) ---------------------------------------+ |  |   |   |   |  |
 *    5: アクセス -----------------------------------------+  |   |   |   |  |
 *    4: キャッシュ無効 --------------------------------------+   |   |   |  |
 *    3: ライトスルー --------------------------------------------+   |   |  |
 *    2: ユーザ／スーパバイザ (0 = スーパバイザ) ---------------------+   |  |
 *    1: 読み取り／書き込み (0 = 読み取りのみ) ---------------------------+  |
 *    0: ページ存在 ---------------------------------------------------------+
 */
#define	CPU_PDE_BASEADDR_MASK	0xfffff000
#define	CPU_PDE_PAGE_SIZE	(1 << 7)
#define	CPU_PDE_ACCESS		(1 << 5)
#define	CPU_PDE_CACHE_DISABLE	(1 << 4)
#define	CPU_PDE_WRITE_THROUGH	(1 << 3)
#define	CPU_PDE_USER_MODE	(1 << 2)
#define	CPU_PDE_WRITABLE	(1 << 1)
#define	CPU_PDE_PRESENT		(1 << 0)

/*
 * ページ・ディレクトリ・エントリ (4M バイトページ使用時)
 * 
 *  31                        22 21       12 11   9 8  7 6 5  4   3   2   1  0 
 * +----------------------------+-----------+------+-+--+-+-+---+---+---+---+-+
 * |ページテーブルの物理アドレス|  予約済み |使用可|G|PS|D|A|PCD|PWT|U/S|R/W|P|
 * +----------------------------+-----------+------+-+--+-+-+---+---+---+---+-+
 *                                              |   |  | | |  |   |   |   |  |
 * 9-11: システム・プログラマが使用可能 --------+   |  | | |  |   |   |   |  |
 *    8: グローバル・ページ ------------------------+  | | |  |   |   |   |  |
 *    7: ページ・サイズ (1 = 4M バイトページ) ---------+ | |  |   |   |   |  |
 *    6: ダーティ ---------------------------------------+ |  |   |   |   |  |
 *    5: アクセス -----------------------------------------+  |   |   |   |  |
 *    4: キャッシュ無効 --------------------------------------+   |   |   |  |
 *    3: ライトスルー --------------------------------------------+   |   |  |
 *    2: ユーザ／スーパバイザ (0 = スーパバイザ) ---------------------+   |  |
 *    1: 読み取り／書き込み (0 = 読み取りのみ) ---------------------------+  |
 *    0: ページ存在 ---------------------------------------------------------+
 */
#define	CPU_PDE_4M_BASEADDR_MASK	0xffc00000
#define	CPU_PDE_4M_GLOBAL_PAGE		(1 << 8)
#define	CPU_PDE_4M_PAGE_SIZE		(1 << 7)
#define	CPU_PDE_4M_DIRTY		(1 << 6)
#define	CPU_PDE_4M_ACCESS		(1 << 5)
#define	CPU_PDE_4M_CACHE_DISABLE	(1 << 4)
#define	CPU_PDE_4M_WRITE_THROUGH	(1 << 3)
#define	CPU_PDE_4M_USER_MODE		(1 << 2)
#define	CPU_PDE_4M_WRITABLE		(1 << 1)
#define	CPU_PDE_4M_PRESENT		(1 << 0)

/*
 * ページ・テーブル・エントリ (4k バイト・ページ)
 *
 *  31                                    12 11   9 8 7 6 5  4   3   2   1  0 
 * +----------------------------------------+------+-+-+-+-+---+---+---+---+-+
 * |        ページのベース・アドレス        |使用可|G|0|D|A|PCD|PWT|U/S|R/W|P|
 * +----------------------------------------+------+-+-+-+-+---+---+---+---+-+
 *                                              |   | | | |  |   |   |   |  |
 *  9-11: システム・プログラマが使用可能 -------+   | | | |  |   |   |   |  |
 *     8: グローバル・ページ -----------------------+ | | |  |   |   |   |  |
 *     7: 予約 (0) -----------------------------------+ | |  |   |   |   |  |
 *     6: ダーティ -------------------------------------+ |  |   |   |   |  |
 *     5: アクセス ---------------------------------------+  |   |   |   |  |
 *     4: キャッシュ無効 ------------------------------------+   |   |   |  |
 *     3: ライトスルー ------------------------------------------+   |   |  |
 *     2: ユーザ／スーパバイザ (0 = スーパバイザ) -------------------+   |  |
 *     1: 読み取り／書き込み (0 = 読み取りのみ) -------------------------+  |
 *     0: ページ存在 -------------------------------------------------------+
 */
#define	CPU_PTE_BASEADDR_MASK	0xfffff000
#define	CPU_PTE_GLOBAL_PAGE	(1 << 8)
#define	CPU_PTE_DIRTY		(1 << 6)
#define	CPU_PTE_ACCESS		(1 << 5)
#define	CPU_PTE_CACHE_DISABLE	(1 << 4)
#define	CPU_PTE_WRITE_THROUGH	(1 << 3)
#define	CPU_PTE_USER_MODE	(1 << 2)
#define	CPU_PTE_WRITABLE	(1 << 1)
#define	CPU_PTE_PRESENT		(1 << 0)

/* paging_check(): rw */
#define	CPU_PAGING_PAGE_READ	(0 << 0)
#define	CPU_PAGING_PAGE_WRITE	(1 << 0)
#define	CPU_PAGING_PAGE_CODE	(1 << 1)
#define	CPU_PAGING_PAGE_DATA	(1 << 2)


/* enter/leave paging mode */
void FASTCALL change_pg(int onoff);

/* paging check */
void MEMCALL paging_check(DWORD laddr, DWORD length, int rw);

/*
 * linear address function
 */
DWORD MEMCALL cpu_linear_memory_read(DWORD address, DWORD length, int code);
void MEMCALL cpu_linear_memory_write(DWORD address, DWORD length, DWORD value);

#define	cpu_lmemoryread(a) \
	(CPU_STAT_PAGING) ? \
		(BYTE)cpu_linear_memory_read(a, 1, FALSE) : \
		cpu_memoryread(a);
#define	cpu_lmemoryread_w(a) \
	(CPU_STAT_PAGING) ? \
		(WORD)cpu_linear_memory_read(a, 2, FALSE) : \
		cpu_memoryread_w(a);
#define	cpu_lmemoryread_d(a) \
	(CPU_STAT_PAGING) ? \
		cpu_linear_memory_read(a, 4, FALSE) : \
		cpu_memoryread_d(a);

#define	cpu_lmemorywrite(a,v) \
	(CPU_STAT_PAGING) ? \
		cpu_linear_memory_write(a, 1, v) : \
		cpu_memorywrite(a,v);
#define	cpu_lmemorywrite_w(a,v) \
	(CPU_STAT_PAGING) ? \
		cpu_linear_memory_write(a, 2, v) : \
		cpu_memorywrite_w(a,v);
#define	cpu_lmemorywrite_d(a,v) \
	(CPU_STAT_PAGING) ? \
		cpu_linear_memory_write(a, 4, v) : \
		cpu_memorywrite_d(a,v);

#define	cpu_lcmemoryread(a) \
	(CPU_STAT_PAGING) ? \
		(BYTE)cpu_linear_memory_read(a, 1, TRUE) : \
		cpu_memoryread(a);
#define	cpu_lcmemoryread_w(a) \
	(CPU_STAT_PAGING) ? \
		(WORD)cpu_linear_memory_read(a, 2, TRUE) : \
		cpu_memoryread_w(a);
#define	cpu_lcmemoryread_d(a) \
	(CPU_STAT_PAGING) ? \
		cpu_linear_memory_read(a, 4, TRUE) : \
		cpu_memoryread_d(a);

#define	set_CR3(cr3) \
do { \
	CPU_CR3 = (cr3) & CPU_CR3_MASK; \
	tlb_flush(FALSE); \
} while (/*CONSTCOND*/ 0)


/*
 * TLB function
 */
void tlb_init();
void tlb_flush(BOOL allflush);
void tlb_flush_page(DWORD vaddr);

#ifdef __cplusplus
}
#endif

#endif	/* !IA32_CPU_PAGING_H__ */

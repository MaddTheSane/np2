/*	$Id: task.h,v 1.1 2003/12/08 00:55:31 yui Exp $	*/

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

#ifndef	IA32_CPU_TASK_H__
#define	IA32_CPU_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
	struct {
		WORD	backlink;	/* 元のタスクセレクタ値 */
		struct {
			WORD	sp;	/* スタックポインタ */
			WORD	ss;	/* スタックセグメント */
		} stack[3];			/* 特権レベル 0-2 */
		WORD	ip;		/* IP */
		WORD	flags;		/* FLAGS */
		WORD	regs[8];	/* レジスタ */
		WORD	sreg[4];	/* セグメントレジスタ */
		WORD	ldt;		/* タスク毎の LDT セレクタ値 */
	} tss16;

	struct {
		REG32	backlink;	/* REG16: 元のタスクセレクタ値 */
		struct {
			REG32	sp;	/* REG32: スタックポインタ */
			REG32	ss;	/* REG16: スタックセグメント */
		} stack[3];			/* 特権レベル 0-2 */
		REG32	cr3;		/* REG32: タスク毎の CR3 */
		REG32	ip;		/* REG32: EIP */
		REG32	flags;		/* REG32: EFLAGS */
		REG32	regs[8];	/* REG32: レジスタ */
		REG32	sreg[6];	/* REG16: セグメントレジスタ */
		REG32	ldt;		/* REG16: タスク毎の LDT セレクタ値 */
		WORD	t;		/* 最下位ビットが1のとき、タスク切替時
					   に割り込み番号1のフォールトを発生 */
		WORD	iobase;		/* TSS 先頭から I/O 許可テーブルマップ
					   までのバイト数 */
	} tss32;
} TASK_STATE_T;

void load_tr(WORD selector);
void get_stack_from_tss(BYTE pl, WORD* new_ss, DWORD* new_esp);
WORD get_link_selector_from_tss();

void task_switch(selector_t* selector, int type);

/* type */
enum {
	TASK_SWITCH_JMP,
	TASK_SWITCH_CALL,
	TASK_SWITCH_IRET,
	TASK_SWITCH_INTR,
};

#ifdef __cplusplus
}
#endif

#endif	/* !IA32_CPU_TASK_H__ */

/*	$Id: cpu.h,v 1.28 2004/03/25 08:51:24 yui Exp $	*/

/*
 * Copyright (c) 2002-2003 NONAKA Kimihiro
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

/*
	Intel Architecture 32-bit Processor Interpreter Engine for Pentium

				Copyright by Yui/Studio Milmake 1999-2000
				Copyright by Norio HATTORI 2000,2001
				Copyright by NONAKA Kimihiro 2002-2004
*/

#ifndef IA32_CPU_CPU_H__
#define IA32_CPU_CPU_H__

#include "interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
#if defined(BYTESEX_LITTLE)
	struct {
		UINT8	l;
		UINT8	h;
		UINT8	_hl;
		UINT8	_hh;
	} b;
	struct {
		UINT16	w;
		UINT16	_hw;
	} w;
#elif defined(BYTESEX_BIG)
	struct {
		UINT8	_hh;
		UINT8	_hl;
		UINT8	h;
		UINT8	l;
	} b;
	struct {
		UINT16	_hw;
		UINT16	w;
	} w;
#endif
	UINT32	d;
} REG32;

#ifdef __cplusplus
}
#endif

#include "segments.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	CPU_EAX_INDEX = 0,
	CPU_ECX_INDEX = 1,
	CPU_EDX_INDEX = 2,
	CPU_EBX_INDEX = 3,
	CPU_ESP_INDEX = 4,
	CPU_EBP_INDEX = 5,
	CPU_ESI_INDEX = 6,
	CPU_EDI_INDEX = 7,
	CPU_REG_NUM
};

enum {
	CPU_ES_INDEX = 0,
	CPU_CS_INDEX = 1,
	CPU_SS_INDEX = 2,
	CPU_DS_INDEX = 3,
	CPU_SEGREG286_NUM = 4,
	CPU_FS_INDEX = 4,
	CPU_GS_INDEX = 5,
	CPU_SEGREG_NUM
};

enum {
	CPU_TEST_REG_NUM = 8
};

enum {
	CPU_DEBUG_REG_NUM = 8,
	CPU_DEBUG_REG_INDEX_NUM = 4
};

enum {
	MAX_PREFIX = 8
};

enum {
	CPU_PREFETCH_QUEUE_LENGTH = 16
};

typedef struct {
	REG32		reg[CPU_REG_NUM];
	UINT16		sreg[CPU_SEGREG_NUM];

	REG32		eflags;
	REG32		eip;

	REG32		prev_eip;
	REG32		prev_esp;

	UINT32		tr[CPU_TEST_REG_NUM];
	UINT32		dr[CPU_DEBUG_REG_NUM];
} CPU_REGS;

typedef struct {
	UINT16		gdtr_limit;
	UINT32		gdtr_base;
	UINT16		idtr_limit;
	UINT32		idtr_base;

	UINT16		ldtr;
	UINT16		tr;

	UINT32		cr0;
	UINT32		cr1;
	UINT32		cr2;
	UINT32		cr3;
	UINT32		cr4;
	UINT32		mxcsr;
} CPU_SYSREGS;

typedef struct {
	descriptor_t	sreg[CPU_SEGREG_NUM];
	descriptor_t	ldtr;
	descriptor_t	tr;

	BYTE		prefetch[CPU_PREFETCH_QUEUE_LENGTH];
	SINT8		prefetch_remain;
	UINT8		pad2[3];

	UINT32		adrsmask;
	UINT32		ovflag;

	UINT8		ss_32;
	UINT8		resetreq;
	UINT8		trap;

	UINT8		page_wp;

	UINT8		protected_mode;
	UINT8		paging;
	UINT8		vm86;
	UINT8		user_mode;

	UINT8		hlt;
	UINT8		bp;	/* break point bitmap */
	UINT8		bp_ev;	/* break point event */
	UINT8		pad;

	UINT32		pde_base;

	UINT32		ioaddr;		/* I/O bitmap linear address */
	UINT16		iolimit;	/* I/O bitmap count */

	UINT8		nerror;		/* double fault/ triple fault */
	UINT8		prev_exception;
} CPU_STAT;

typedef struct {
	UINT8		op_32;
	UINT8		as_32;
	UINT8		rep_used;
	UINT8		seg_used;
	UINT32		seg_base;
} CPU_INST;

typedef struct {
	CPU_REGS	cpu_regs;
	CPU_SYSREGS	cpu_sysregs;
	CPU_STAT	cpu_stat;
	CPU_INST	cpu_inst;
	CPU_INST	cpu_inst_default;

	/* protected by cpu shut */
	UINT8		cpu_type;
	UINT8		itfbank;
	UINT16		ram_d0;
	SINT32		remainclock;
	SINT32		baseclock;
	UINT32		clock;
} I386STAT;

typedef struct {					/* for ver0.73 */
	BYTE		*ext;
	UINT32		extsize;
	UINT32		inport;
} I386EXT;

typedef struct {
	I386STAT	s;				/* STATsave������� */
	I386EXT		e;
} I386CORE;

extern I386CORE		i386core;

#define	CPU_STATSAVE	i386core.s

#define	CPU_ADRSMASK	i386core.s.cpu_stat.adrsmask
#define	CPU_RESETREQ	i386core.s.cpu_stat.resetreq

#define	CPU_REMCLOCK	i386core.s.remainclock
#define	CPU_BASECLOCK	i386core.s.baseclock
#define	CPU_CLOCK	i386core.s.clock
#define	CPU_ITFBANK	i386core.s.itfbank
#define	CPU_RAM_D000	i386core.s.ram_d0

#define CPU_TYPE	i386core.s.cpu_type
#define CPUTYPE_V30	0x01

#define	CPU_EXTMEM	i386core.e.ext
#define	CPU_EXTMEMSIZE	i386core.e.extsize
#define	CPU_INPADRS	i386core.e.inport

extern sigjmp_buf	exec_1step_jmpbuf;


/*
 * CPUID
 */
/* vendor */
#define	CPU_VENDOR_1	0x756e6547	/* "Genu" */
#define	CPU_VENDOR_2	0x49656e69	/* "ineI" */
#define	CPU_VENDOR_3	0x6c65746e	/* "ntel" */

/* version */
#define	CPU_FAMILY	4
#if defined(USE_FPU)
#define	CPU_MODEL	1	/* 486DX */
#else
#define	CPU_MODEL	2	/* 486SX */
#endif
#define	CPU_STEPPING	3

/* feature */
#define	CPU_FEATURE_FPU		(1 << 0)
#define	CPU_FEATURE_VME		(1 << 1)
#define	CPU_FEATURE_DE		(1 << 2)
#define	CPU_FEATURE_PSE		(1 << 3)
#define	CPU_FEATURE_TSC		(1 << 4)
#define	CPU_FEATURE_MSR		(1 << 5)
#define	CPU_FEATURE_PAE		(1 << 6)
#define	CPU_FEATURE_MCE		(1 << 7)
#define	CPU_FEATURE_CX8		(1 << 8)
#define	CPU_FEATURE_APIC	(1 << 9)
/*				(1 << 10) */
#define	CPU_FEATURE_SEP		(1 << 11)
#define	CPU_FEATURE_MTRR	(1 << 12)
#define	CPU_FEATURE_PGE		(1 << 13)
#define	CPU_FEATURE_MCA		(1 << 14)
#define	CPU_FEATURE_CMOV	(1 << 15)
#define	CPU_FEATURE_FGPAT	(1 << 16)
#define	CPU_FEATURE_PSE36	(1 << 17)
#define	CPU_FEATURE_PN		(1 << 18)
/*				(1 << 19) */
/*				(1 << 20) */
/*				(1 << 21) */
/*				(1 << 22) */
#define	CPU_FEATURE_MMX		(1 << 23)
#define	CPU_FEATURE_FXSR	(1 << 24)
#define	CPU_FEATURE_XMM		(1 << 25)
/*				(1 << 26) */
/*				(1 << 27) */
/*				(1 << 28) */
/*				(1 << 29) */
/*				(1 << 30) */
/*				(1 << 31) */
#ifdef USE_FPU
#define	CPU_FEATURES		(CPU_FEATURE_CMOV|CPU_FEATURE_FPU)
#else
#define	CPU_FEATURES		(CPU_FEATURE_CMOV)
#endif


#define	CPU_REGS_BYTEL(n)	CPU_STATSAVE.cpu_regs.reg[(n)].b.l
#define	CPU_REGS_BYTEH(n)	CPU_STATSAVE.cpu_regs.reg[(n)].b.h
#define	CPU_REGS_WORD(n)	CPU_STATSAVE.cpu_regs.reg[(n)].w.w
#define	CPU_REGS_DWORD(n)	CPU_STATSAVE.cpu_regs.reg[(n)].d
#define	CPU_REGS_SREG(n)	CPU_STATSAVE.cpu_regs.sreg[(n)]

#define	CPU_STAT_SREG(n)	CPU_STATSAVE.cpu_stat.sreg[(n)]
#define	CPU_STAT_SREGBASE(n)	CPU_STATSAVE.cpu_stat.sreg[(n)].u.seg.segbase
#define	CPU_STAT_SREGEND(n)	CPU_STATSAVE.cpu_stat.sreg[(n)].u.seg.segend
#define	CPU_STAT_SREGLIMIT(n)	CPU_STATSAVE.cpu_stat.sreg[(n)].u.seg.limit
#define	CPU_STAT_SREG_CLEAR(n) \
do { \
	memset(&CPU_STAT_SREG(n), 0, sizeof(descriptor_t)); \
} while (/*CONSTCOND*/ 0)
#define	CPU_STAT_SREG_INIT(n) \
do { \
	descriptor_t sd; \
\
	memset(&sd, 0, sizeof(sd)); \
	sd.u.seg.limit = 0xffff; \
	CPU_SET_SEGDESC_DEFAULT(&sd, (n), 0); \
	CPU_STAT_SREG(n) = sd; \
} while (/*CONSTCOND*/ 0)


#define CPU_AL		CPU_REGS_BYTEL(CPU_EAX_INDEX)
#define CPU_CL		CPU_REGS_BYTEL(CPU_ECX_INDEX)
#define CPU_DL		CPU_REGS_BYTEL(CPU_EDX_INDEX)
#define CPU_BL		CPU_REGS_BYTEL(CPU_EBX_INDEX)
#define CPU_AH		CPU_REGS_BYTEH(CPU_EAX_INDEX)
#define CPU_CH		CPU_REGS_BYTEH(CPU_ECX_INDEX)
#define CPU_DH		CPU_REGS_BYTEH(CPU_EDX_INDEX)
#define CPU_BH		CPU_REGS_BYTEH(CPU_EBX_INDEX)

#define	CPU_AX		CPU_REGS_WORD(CPU_EAX_INDEX)
#define	CPU_CX		CPU_REGS_WORD(CPU_ECX_INDEX)
#define	CPU_DX		CPU_REGS_WORD(CPU_EDX_INDEX)
#define	CPU_BX		CPU_REGS_WORD(CPU_EBX_INDEX)
#define	CPU_SP		CPU_REGS_WORD(CPU_ESP_INDEX)
#define	CPU_BP		CPU_REGS_WORD(CPU_EBP_INDEX)
#define	CPU_SI		CPU_REGS_WORD(CPU_ESI_INDEX)
#define	CPU_DI		CPU_REGS_WORD(CPU_EDI_INDEX)
#define CPU_IP		CPU_STATSAVE.cpu_regs.eip.w.w

#define	CPU_EAX		CPU_REGS_DWORD(CPU_EAX_INDEX)
#define	CPU_ECX		CPU_REGS_DWORD(CPU_ECX_INDEX)
#define	CPU_EDX		CPU_REGS_DWORD(CPU_EDX_INDEX)
#define	CPU_EBX		CPU_REGS_DWORD(CPU_EBX_INDEX)
#define	CPU_ESP		CPU_REGS_DWORD(CPU_ESP_INDEX)
#define	CPU_EBP		CPU_REGS_DWORD(CPU_EBP_INDEX)
#define	CPU_ESI		CPU_REGS_DWORD(CPU_ESI_INDEX)
#define	CPU_EDI		CPU_REGS_DWORD(CPU_EDI_INDEX)
#define CPU_EIP		CPU_STATSAVE.cpu_regs.eip.d
#define CPU_PREV_EIP	CPU_STATSAVE.cpu_regs.prev_eip.d
#define CPU_PREV_ESP	CPU_STATSAVE.cpu_regs.prev_esp.d

#define	CPU_ES		CPU_REGS_SREG(CPU_ES_INDEX)
#define	CPU_CS		CPU_REGS_SREG(CPU_CS_INDEX)
#define	CPU_SS		CPU_REGS_SREG(CPU_SS_INDEX)
#define	CPU_DS		CPU_REGS_SREG(CPU_DS_INDEX)
#define	CPU_FS		CPU_REGS_SREG(CPU_FS_INDEX)
#define	CPU_GS		CPU_REGS_SREG(CPU_GS_INDEX)

#define ES_BASE		CPU_STAT_SREGBASE(CPU_ES_INDEX)
#define CS_BASE		CPU_STAT_SREGBASE(CPU_CS_INDEX)
#define SS_BASE		CPU_STAT_SREGBASE(CPU_SS_INDEX)
#define DS_BASE		CPU_STAT_SREGBASE(CPU_DS_INDEX)
#define FS_BASE		CPU_STAT_SREGBASE(CPU_FS_INDEX)
#define GS_BASE		CPU_STAT_SREGBASE(CPU_GS_INDEX)

#define CPU_EFLAG	CPU_STATSAVE.cpu_regs.eflags.d
#define CPU_FLAG	CPU_STATSAVE.cpu_regs.eflags.w.w
#define CPU_FLAGL	CPU_STATSAVE.cpu_regs.eflags.b.l
#define CPU_FLAGH	CPU_STATSAVE.cpu_regs.eflags.b.h
#define CPU_TRAP	CPU_STATSAVE.cpu_stat.trap
#define CPU_INPORT	CPU_STATSAVE.cpu_stat.inport
#define CPU_OV		CPU_STATSAVE.cpu_stat.ovflag

#define C_FLAG		(1 << 0)
#define P_FLAG		(1 << 2)
#define A_FLAG		(1 << 4)
#define Z_FLAG		(1 << 6)
#define S_FLAG		(1 << 7)
#define T_FLAG		(1 << 8)
#define I_FLAG		(1 << 9)
#define D_FLAG		(1 << 10)
#define O_FLAG		(1 << 11)
#define IOPL_FLAG	(3 << 12)
#define NT_FLAG		(1 << 14)
#define RF_FLAG		(1 << 16)
#define VM_FLAG		(1 << 17)
#define AC_FLAG		(1 << 18)
#define VIF_FLAG	(1 << 19)
#define VIP_FLAG	(1 << 20)
#define ID_FLAG		(1 << 21)
#define	SZP_FLAG	(P_FLAG|Z_FLAG|S_FLAG)
#define	SZAP_FLAG	(P_FLAG|A_FLAG|Z_FLAG|S_FLAG)
#define	SZPC_FLAG	(C_FLAG|P_FLAG|Z_FLAG|S_FLAG)
#define	SZAPC_FLAG	(C_FLAG|P_FLAG|A_FLAG|Z_FLAG|S_FLAG)
#define	ALL_FLAG	(SZAPC_FLAG|T_FLAG|I_FLAG|D_FLAG|O_FLAG|IOPL_FLAG|NT_FLAG)
#define	ALL_EFLAG	(ALL_FLAG|RF_FLAG|VM_FLAG|AC_FLAG|VIF_FLAG|VIP_FLAG|ID_FLAG)

#define	REAL_FLAGREG	((CPU_FLAG & 0xf7ff) | (CPU_OV ? O_FLAG : 0) | 2)
#define	REAL_EFLAGREG	((CPU_EFLAG & 0xfffff7ff) | (CPU_OV ? O_FLAG : 0) | 2)

#if !defined(IA32_DONT_USE_SET_EFLAGS_FUNCTION)
void set_flags(UINT16 new_flags, UINT16 mask);
void set_eflags(UINT32 new_flags, UINT32 mask);
#endif


#define	CPU_INST_OP32		CPU_STATSAVE.cpu_inst.op_32
#define	CPU_INST_AS32		CPU_STATSAVE.cpu_inst.as_32
#define	CPU_INST_REPUSE		CPU_STATSAVE.cpu_inst.rep_used
#define	CPU_INST_SEGUSE		CPU_STATSAVE.cpu_inst.seg_used
#define	CPU_INST_SEGREG_INDEX	CPU_STATSAVE.cpu_inst.seg_base
#define	DS_FIX	(!CPU_INST_SEGUSE ? CPU_DS_INDEX : CPU_INST_SEGREG_INDEX)
#define	SS_FIX	(!CPU_INST_SEGUSE ? CPU_SS_INDEX : CPU_INST_SEGREG_INDEX)

#define	CPU_STAT_CS_BASE	CPU_STAT_SREGBASE(CPU_CS_INDEX)
#define	CPU_STAT_CS_LIMIT	CPU_STAT_SREGLIMIT(CPU_CS_INDEX)
#define	CPU_STAT_CS_END		CPU_STAT_SREGEND(CPU_CS_INDEX)

#define	CPU_STAT_ADRSMASK	CPU_STATSAVE.cpu_stat.adrsmask
#define	CPU_STAT_SS32		CPU_STATSAVE.cpu_stat.ss_32
#define	CPU_STAT_RESETREQ	CPU_STATSAVE.cpu_stat.resetreq
#define	CPU_STAT_PM		CPU_STATSAVE.cpu_stat.protected_mode
#define	CPU_STAT_PAGING		CPU_STATSAVE.cpu_stat.paging
#define	CPU_STAT_VM86		CPU_STATSAVE.cpu_stat.vm86
#define	CPU_STAT_WP		CPU_STATSAVE.cpu_stat.page_wp
#define	CPU_STAT_CPL		CPU_STAT_SREG(CPU_CS_INDEX).rpl
#define	CPU_STAT_USER_MODE	CPU_STATSAVE.cpu_stat.user_mode
#define	CPU_STAT_PDE_BASE	CPU_STATSAVE.cpu_stat.pde_base

#define	CPU_STAT_HLT		CPU_STATSAVE.cpu_stat.hlt

#define	CPU_STAT_IOPL		((CPU_EFLAG & IOPL_FLAG) >> 12)
#define	CPU_IOPL0		0
#define	CPU_IOPL1		1
#define	CPU_IOPL2		2
#define	CPU_IOPL3		3

#define	CPU_STAT_IOADDR		CPU_STATSAVE.cpu_stat.ioaddr
#define	CPU_STAT_IOLIMIT	CPU_STATSAVE.cpu_stat.iolimit

#define	CPU_STAT_PREV_EXCEPTION		CPU_STATSAVE.cpu_stat.prev_exception
#define	CPU_STAT_EXCEPTION_COUNTER		CPU_STATSAVE.cpu_stat.nerror
#define	CPU_STAT_EXCEPTION_COUNTER_INC()	CPU_STATSAVE.cpu_stat.nerror++
#define	CPU_STAT_EXCEPTION_COUNTER_CLEAR()	CPU_STATSAVE.cpu_stat.nerror = 0

#define	CPU_PREFETCHQ		CPU_STATSAVE.cpu_stat.prefetch
#define	CPU_PREFETCHQ_REMAIN	CPU_STATSAVE.cpu_stat.prefetch_remain

#if defined(IA32_SUPPORT_PREFETCH_QUEUE)
#define	CPU_PREFETCH_CLEAR()	CPU_PREFETCHQ_REMAIN = 0
#else	/* !IA32_SUPPORT_PREFETCH_QUEUE */
#define	CPU_PREFETCH_CLEAR()
#endif	/* IA32_SUPPORT_PREFETCH_QUEUE */

#define	CPU_MODE_SUPERVISER	0
#define	CPU_MODE_USER		(1 << 3)
#define	CPU_SET_CPL(cpl) \
do { \
	UINT8 __t = (UINT8)((cpl) & 3); \
	CPU_STAT_CPL = __t; \
	CPU_STAT_USER_MODE = (__t == 3) ? CPU_MODE_USER : CPU_MODE_SUPERVISER; \
} while (/*CONSTCOND*/ 0)

#define CPU_CLI \
do { \
	CPU_FLAG &= ~I_FLAG; \
	CPU_TRAP = 0; \
} while (/*CONSTCOND*/0)

#define CPU_STI \
do { \
	CPU_FLAG |= I_FLAG; \
	CPU_TRAP = (CPU_FLAG & (I_FLAG|T_FLAG)) == (I_FLAG|T_FLAG) ; \
} while (/*CONSTCOND*/0)

#define CPU_GDTR_LIMIT	CPU_STATSAVE.cpu_sysregs.gdtr_limit
#define CPU_GDTR_BASE	CPU_STATSAVE.cpu_sysregs.gdtr_base
#define CPU_IDTR_LIMIT	CPU_STATSAVE.cpu_sysregs.idtr_limit
#define CPU_IDTR_BASE	CPU_STATSAVE.cpu_sysregs.idtr_base
#define CPU_LDTR	CPU_STATSAVE.cpu_sysregs.ldtr
#define CPU_LDTR_DESC	CPU_STATSAVE.cpu_stat.ldtr
#define CPU_LDTR_BASE	CPU_STATSAVE.cpu_stat.ldtr.u.seg.segbase
#define CPU_LDTR_END	CPU_STATSAVE.cpu_stat.ldtr.u.seg.segend
#define CPU_LDTR_LIMIT	CPU_STATSAVE.cpu_stat.ldtr.u.seg.limit
#define CPU_TR		CPU_STATSAVE.cpu_sysregs.tr
#define CPU_TR_DESC	CPU_STATSAVE.cpu_stat.tr
#define CPU_TR_BASE	CPU_STATSAVE.cpu_stat.tr.u.seg.segbase
#define CPU_TR_END	CPU_STATSAVE.cpu_stat.tr.u.seg.segend
#define CPU_TR_LIMIT	CPU_STATSAVE.cpu_stat.tr.u.seg.limit

/*
 * control register
 */
#define CPU_MSW			CPU_STATSAVE.cpu_sysregs.cr0

#define CPU_CR0			CPU_STATSAVE.cpu_sysregs.cr0
#define CPU_CR1			CPU_STATSAVE.cpu_sysregs.cr1
#define CPU_CR2			CPU_STATSAVE.cpu_sysregs.cr2
#define CPU_CR3			CPU_STATSAVE.cpu_sysregs.cr3
#define CPU_CR4			CPU_STATSAVE.cpu_sysregs.cr4
#define CPU_MXCSR		CPU_STATSAVE.cpu_sysregs.mxcsr

#define	CPU_CR0_PE		(1 << 0)
#define	CPU_CR0_MP		(1 << 1)
#define	CPU_CR0_EM		(1 << 2)
#define	CPU_CR0_TS		(1 << 3)
#define	CPU_CR0_ET		(1 << 4)
#define	CPU_CR0_NE		(1 << 5)
#define	CPU_CR0_WP		(1 << 16)
#define	CPU_CR0_AM		(1 << 18)
#define	CPU_CR0_NW		(1 << 29)
#define	CPU_CR0_CD		(1 << 30)
#define	CPU_CR0_PG		(1 << 31)
#define	CPU_CR0_ALL		(CPU_CR0_PE|CPU_CR0_MP|CPU_CR0_EM|CPU_CR0_TS|CPU_CR0_ET|CPU_CR0_NE|CPU_CR0_WP|CPU_CR0_AM|CPU_CR0_NW|CPU_CR0_CD|CPU_CR0_PG)

#define	CPU_CR3_PD_MASK		0xfffff000
#define	CPU_CR3_PWT		(1 << 3)
#define	CPU_CR3_PCD		(1 << 4)
#define	CPU_CR3_MASK		(CPU_CR3_PD_MASK|CPU_CR3_PWT|CPU_CR3_PCD)

#define	CPU_CR4_VME		(1 << 0)
#define	CPU_CR4_PVI		(1 << 1)
#define	CPU_CR4_TSD		(1 << 2)
#define	CPU_CR4_DE		(1 << 3)
#define	CPU_CR4_PSE		(1 << 4)
#define	CPU_CR4_PAE		(1 << 5)
#define	CPU_CR4_MCE		(1 << 6)
#define	CPU_CR4_PGE		(1 << 7)
#define	CPU_CR4_PCE		(1 << 8)
#define	CPU_CR4_OSFXSR		(1 << 9)
#define	CPU_CR4_OSXMMEXCPT	(1 << 10)

/*
 * debug register
 */
#define	CPU_DR(r)		CPU_STATSAVE.cpu_regs.dr[(r)]
#define	CPU_DR6			CPU_DR(6)
#define	CPU_DR7			CPU_DR(7)

#define	CPU_STAT_BP		CPU_STATSAVE.cpu_stat.bp
#define	CPU_STAT_BP_EVENT	CPU_STATSAVE.cpu_stat.bp_ev
#define	CPU_STAT_BP_EVENT_B(r)	(1 << (r))
#define	CPU_STAT_BP_EVENT_DR	(1 << 4)	/* fault */
#define	CPU_STAT_BP_EVENT_STEP	(1 << 5)	/* as CPU_TRAP */
#define	CPU_STAT_BP_EVENT_TASK	(1 << 6)
#define	CPU_STAT_BP_EVENT_RF	(1 << 7)	/* RF_FLAG */

#define	CPU_DR6_B(r)		(1 << (r))
#define	CPU_DR6_BD		(1 << 13)
#define	CPU_DR6_BS		(1 << 14)
#define	CPU_DR6_BT		(1 << 15)

#define	CPU_DR7_L(r)		(1 << ((r) * 2))
#define	CPU_DR7_G(r)		(1 << ((r) * 2 + 1))
#define	CPU_DR7_LE		(1 << 8)
#define	CPU_DR7_GE		(1 << 9)
#define	CPU_DR7_GD		(1 << 13)
#define	CPU_DR7_RW(r)		(3 << ((r) * 4 + 16))
#define	CPU_DR7_LEN(r)		(3 << ((r) * 4 + 16 + 2))

#define	CPU_DR7_GET_RW(r)	((CPU_DR7) >> (16 + (r) * 4))
#define	CPU_DR7_RW_CODE		0
#define	CPU_DR7_RW_RO		1
#define	CPU_DR7_RW_IO		2
#define	CPU_DR7_RW_RW		3

#define	CPU_DR7_GET_LEN(r)	((CPU_DR7) >> (16 + 2 + (r) * 4))

void ia32_init(void);
void ia32_initreg(void);
void ia32_setextsize(UINT32 size);

void ia32reset(void);
void ia32shut(void);
void ia32a20enable(BOOL enable);
void ia32(void);
void ia32_step(void);
void CPUCALL ia32_interrupt(int vect, int soft);
void CPUCALL ia32_exception(int vect, int p1, int p2);

void exec_1step(void);
#define	INST_PREFIX	(1 << 0)
#define	INST_STRING	(1 << 1)
#define	REP_CHECKZF	(1 << 7)

void ia32_printf(const char *buf, ...);
void ia32_warning(const char *buf, ...);
void ia32_panic(const char *buf, ...);

void ia32_bioscall(void);

void FASTCALL change_pm(BOOL onoff);
void FASTCALL change_vm(BOOL onoff);
void FASTCALL change_pg(BOOL onoff);

extern const UINT8 iflags[];
#define	szpcflag	iflags
extern UINT8 szpflag_w[0x10000];

extern UINT8 *reg8_b20[0x100];
extern UINT8 *reg8_b53[0x100];
extern UINT16 *reg16_b20[0x100];
extern UINT16 *reg16_b53[0x100];
extern UINT32 *reg32_b20[0x100];
extern UINT32 *reg32_b53[0x100];

extern const char *reg8_str[8];
extern const char *reg16_str[8];
extern const char *reg32_str[8];

char *cpu_reg2str(void);
#if defined(USE_FPU)
char *fpu_reg2str(void);
#endif
void put_cpuinfo(void);
void dbg_printf(const char *str, ...);


/*
 * Misc.
 */
void gdtr_dump(UINT32 base, UINT limit);
void idtr_dump(UINT32 base, UINT limit);
void ldtr_dump(UINT32 base, UINT limit);
void tr_dump(UINT16 selector, UINT32 base, UINT limit);

/*
 * disasm
 */
/* context */
typedef struct {
	UINT32 val;

	UINT32 eip;
	BOOL op32;
	BOOL as32;

	UINT32 baseaddr;
	UINT8 opcode[3];
	UINT8 modrm;
	UINT8 sib;

	BOOL useseg;
	int seg;

	UINT8 opbyte[32];
	int nopbytes;

	char str[256];
	size_t remain;

	char *next;
	char *prefix;
	char *op;
	char *arg[3];
	int narg;

	char pad;
} disasm_context_t;

int disasm(UINT32 *eip, disasm_context_t *ctx);


#ifdef __cplusplus
}
#endif

#include "cpu_io.h"
#include "cpu_mem.h"
#include "exception.h"
#include "paging.h"
#include "resolve.h"
#include "task.h"

#endif	/* !IA32_CPU_CPU_H__ */

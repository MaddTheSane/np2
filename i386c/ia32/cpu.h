/*	$Id: cpu.h,v 1.8 2004/01/15 15:50:33 monaka Exp $	*/

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
				Copyright by NONAKA Kimihiro 2002-2003
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
		BYTE	l;
		BYTE	h;
		BYTE	_hl;
		BYTE	_hh;
	} b;
	struct {
		WORD	w;
		WORD	_hw;
	} w;
#elif defined(BYTESEX_BIG)
	struct {
		BYTE	_hh;
		BYTE	_hl;
		BYTE	h;
		BYTE	l;
	} b;
	struct {
		WORD	_hw;
		WORD	w;
	} w;
#endif
	DWORD	d;
} REG32;

#ifdef __cplusplus
}
#endif

#include "cpu_io.h"
#include "cpu_mem.h"
#include "exception.h"
#include "paging.h"
#include "resolve.h"
#include "segments.h"
#include "task.h"

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

typedef struct {
	REG32		reg[CPU_REG_NUM];
	WORD		sreg[CPU_SEGREG_NUM];

	REG32		eflags;

	REG32		eip;
	REG32		prev_eip;
} CPU_REGS;

typedef struct {
	WORD		gdtr_limit;
	DWORD		gdtr_base;
	WORD		idtr_limit;
	DWORD		idtr_base;

	WORD		ldtr;
	WORD		tr;
	descriptor_t	ldtr_desc;
	descriptor_t	tr_desc;

	DWORD		cr0;
	DWORD		cr1;
	DWORD		cr2;
	DWORD		cr3;
	DWORD		cr4;
	DWORD		mxcsr;
} CPU_SYSREGS;

typedef struct {
	descriptor_t	sreg[CPU_SEGREG_NUM];

	UINT32		adrsmask;
	DWORD		ovflag;

	BYTE		ss_32;
	BYTE		resetreq;
	BYTE		trap;
	BYTE		_dummy;

	BYTE		cpl;
	BYTE		protected_mode;
	BYTE		paging;
	BYTE		vm86;

	DWORD		ioaddr;		/* I/O bitmap linear address */
	WORD		iolimit;	/* I/O bitmap count */

	BYTE		nerror;		/* double fault/ triple fault */
	BYTE		prev_exception;
} CPU_STAT;

typedef struct {
	BYTE		op_32;
	BYTE		as_32;
	BYTE		rep_used;
	BYTE		seg_used;
	DWORD		seg_base;
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
	I386STAT	s;				/* STATsave§µ§Ï§Î≈€ */
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

extern BYTE 		iflags[];
extern jmp_buf		exec_1step_jmpbuf;


/*
 * CPUID
 */
/* vendor */
#define	CPU_VENDOR_1	0x756e6547	/* "Genu" */
#define	CPU_VENDOR_2	0x49656e69	/* "ineI" */
#define	CPU_VENDOR_3	0x6c65746e	/* "ntel" */

/* version */
#define	CPU_FAMILY	6
#define	CPU_MODEL	1
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

#define	CPU_ES		CPU_REGS_SREG(CPU_ES_INDEX)
#define	CPU_CS		CPU_REGS_SREG(CPU_CS_INDEX)
#define	CPU_SS		CPU_REGS_SREG(CPU_SS_INDEX)
#define	CPU_DS		CPU_REGS_SREG(CPU_DS_INDEX)
#define	CPU_FS		CPU_REGS_SREG(CPU_FS_INDEX)
#define	CPU_GS		CPU_REGS_SREG(CPU_GS_INDEX)

#define ES_BASE		CPU_STATSAVE.cpu_stat.sreg[CPU_ES_INDEX].u.seg.segbase
#define CS_BASE		CPU_STATSAVE.cpu_stat.sreg[CPU_CS_INDEX].u.seg.segbase
#define SS_BASE		CPU_STATSAVE.cpu_stat.sreg[CPU_SS_INDEX].u.seg.segbase
#define DS_BASE		CPU_STATSAVE.cpu_stat.sreg[CPU_DS_INDEX].u.seg.segbase
#define FS_BASE		CPU_STATSAVE.cpu_stat.sreg[CPU_FS_INDEX].u.seg.segbase
#define GS_BASE		CPU_STATSAVE.cpu_stat.sreg[CPU_GS_INDEX].u.seg.segbase

#define CPU_EFLAG	CPU_STATSAVE.cpu_regs.eflags.d
#define CPU_FLAG	CPU_STATSAVE.cpu_regs.eflags.w.w
#define CPU_FLAGL	CPU_STATSAVE.cpu_regs.eflags.b.l
#define CPU_FLAGH	CPU_STATSAVE.cpu_regs.eflags.b.h
#define CPU_TRAP	CPU_STATSAVE.cpu_stat.trap
// #define CPU_INPORT	CPU_STATSAVE.cpu_stat.inport
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

#define	REAL_FLAGREG	((CPU_FLAG & 0xf7ff) | (CPU_OV ? O_FLAG : 0))
#define	REAL_EFLAGREG	((CPU_EFLAG & 0xfffff7ff) | (CPU_OV ? O_FLAG : 0))

void set_flags(WORD new_flags, WORD mask);
void set_eflags(DWORD new_flags, DWORD mask);


#define	CPU_INST_OP32		CPU_STATSAVE.cpu_inst.op_32
#define	CPU_INST_AS32		CPU_STATSAVE.cpu_inst.as_32
#define	CPU_INST_REPUSE		CPU_STATSAVE.cpu_inst.rep_used
#define	CPU_INST_SEGUSE		CPU_STATSAVE.cpu_inst.seg_used
#define	CPU_INST_SEGREG_INDEX	CPU_STATSAVE.cpu_inst.seg_base
#define	DS_FIX	(!CPU_INST_SEGUSE ? CPU_DS_INDEX : CPU_INST_SEGREG_INDEX)
#define	SS_FIX	(!CPU_INST_SEGUSE ? CPU_SS_INDEX : CPU_INST_SEGREG_INDEX)

#define	CPU_STAT_CS_BASE	CPU_STATSAVE.cpu_stat.sreg[CPU_CS_INDEX].u.seg.limit
#define	CPU_STAT_CS_LIMIT	CPU_STATSAVE.cpu_stat.sreg[CPU_CS_INDEX].u.seg.limit
#define	CPU_STAT_CS_END		CPU_STATSAVE.cpu_stat.sreg[CPU_CS_INDEX].u.seg.segend

#define	CPU_STAT_ADRSMASK	CPU_STATSAVE.cpu_stat.adrsmask
#define	CPU_STAT_SS32		CPU_STATSAVE.cpu_stat.ss_32
#define	CPU_STAT_RESETREQ	CPU_STATSAVE.cpu_stat.resetreq
#define	CPU_STAT_PM		CPU_STATSAVE.cpu_stat.protected_mode
#define	CPU_STAT_VM86		CPU_STATSAVE.cpu_stat.vm86
#define	CPU_STAT_PAGING		CPU_STATSAVE.cpu_stat.paging
#define	CPU_STAT_CPL		CPU_STATSAVE.cpu_stat.cpl

#define	CPU_STAT_IOPL		((CPU_EFLAG & IOPL_FLAG) >> 12)
#define	CPU_IOPL0		0
#define	CPU_IOPL1		1
#define	CPU_IOPL2		2
#define	CPU_IOPL3		3

#define	CPU_STAT_IOADDR		CPU_STATSAVE.cpu_stat.ioaddr
#define	CPU_STAT_IOLIMIT	CPU_STATSAVE.cpu_stat.iolimit

#define	CPU_STAT_NERROR		CPU_STATSAVE.cpu_stat.nerror
#define	CPU_STAT_PREV_EXCEPTION	CPU_STATSAVE.cpu_stat.prev_exception

#define CPU_CLI		do { CPU_FLAG &= ~I_FLAG;	\
					CPU_TRAP = 0; } while (/*CONSTCOND*/ 0)
#define CPU_STI		do { CPU_FLAG |= I_FLAG;	\
					CPU_TRAP = (CPU_FLAG >> 8) & 1; } while (/*CONSTCOND*/0)

#define CPU_GDTR_LIMIT	CPU_STATSAVE.cpu_sysregs.gdtr_limit
#define CPU_GDTR_BASE	CPU_STATSAVE.cpu_sysregs.gdtr_base
#define CPU_IDTR_LIMIT	CPU_STATSAVE.cpu_sysregs.idtr_limit
#define CPU_IDTR_BASE	CPU_STATSAVE.cpu_sysregs.idtr_base
#define CPU_LDTR	CPU_STATSAVE.cpu_sysregs.ldtr
#define CPU_LDTR_DESC	CPU_STATSAVE.cpu_sysregs.ldtr_desc
#define CPU_LDTR_BASE	CPU_STATSAVE.cpu_sysregs.ldtr_desc.u.seg.segbase
#define CPU_LDTR_END	CPU_STATSAVE.cpu_sysregs.ldtr_desc.u.seg.segend
#define CPU_LDTR_LIMIT	CPU_STATSAVE.cpu_sysregs.ldtr_desc.u.seg.limit
#define CPU_TR		CPU_STATSAVE.cpu_sysregs.tr
#define CPU_TR_DESC	CPU_STATSAVE.cpu_sysregs.tr_desc
#define CPU_TR_BASE	CPU_STATSAVE.cpu_sysregs.tr_desc.u.seg.segbase
#define CPU_TR_END	CPU_STATSAVE.cpu_sysregs.tr_desc.u.seg.segend
#define CPU_TR_LIMIT	CPU_STATSAVE.cpu_sysregs.tr_desc.u.seg.limit

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

#define	CPU_CR3_PD_MASK		0xfffff000
#define	CPU_CR3_PWT		(1 << 3)
#define	CPU_CR3_PCD		(1 << 4)

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


void ia32_init(void);

void ia32reset(void);
void ia32shut(void);
void ia32(void);
void ia32withtrap(void);
void ia32withdma(void);

void ia32_step(void);
void CPUCALL ia32_interrupt(BYTE vect);
void CPUCALL ia32_exception(DWORD vect, DWORD p1, DWORD p2);

void exec_1step(void);
#define	INST_PREFIX	(1 << 0)
#define	INST_STRING	(1 << 1)
#define	REP_CHECKZF	(1 << 7)

void disasm(WORD cs, DWORD maddr);

void ia32_printf(const char *buf, ...);
void ia32_warning(const char *buf, ...);
void ia32_panic(const char *buf, ...);

void ia32_bioscall(void);

void FASTCALL change_pm(BOOL onoff);
void FASTCALL change_vm(BOOL onoff);

extern BYTE szpcflag[0x200];
extern BYTE szpflag_w[0x10000];

extern BYTE  *reg8_b20[0x100];
extern BYTE  *reg8_b53[0x100];
extern WORD  *reg16_b20[0x100];
extern WORD  *reg16_b53[0x100];
extern DWORD *reg32_b20[0x100];
extern DWORD *reg32_b53[0x100];

/*
 * Profile
 */
#if defined(IA32_PROFILE_INSTRUCTION)
extern UINT32	inst_1byte_count[2][256];
extern UINT32	inst_2byte_count[2][256];
extern UINT32	ea16_count[24];
extern UINT32	ea32_count[24];
extern UINT32	sib0_count[256];
extern UINT32	sib1_count[256];
extern UINT32	sib2_count[256];

#define	PROFILE_INC_INST_1BYTE(op)	inst_1byte_count[CPU_INST_OP32][op]++
#define	PROFILE_INC_INST_2BYTE(op)	inst_2byte_count[CPU_INST_OP32][op]++
#define	PROFILE_INC_EA16(idx)		ea16_count[idx]++
#define	PROFILE_INC_EA32(idx)		ea32_count[idx]++
#define	PROFILE_INC_SIB0(op)		sib0_count[op]++
#define	PROFILE_INC_SIB1(op)		sib1_count[op]++
#define	PROFILE_INC_SIB2(op)		sib2_count[op]++
#else
#define	PROFILE_INC_INST_1BYTE(op)
#define	PROFILE_INC_INST_2BYTE(op)
#define	PROFILE_INC_EA16(idx)
#define	PROFILE_INC_EA32(idx)
#define	PROFILE_INC_SIB0(op)
#define	PROFILE_INC_SIB1(op)
#define	PROFILE_INC_SIB2(op)
#endif

#ifdef __cplusplus
}
#endif

#endif	/* !IA32_CPU_CPU_H__ */

//----------------------------------------------------------------------------
//
//  i286a : 80286 Engine for ARM  ver0.01
//
//                                    Copyright by Yui/Studio Milmake 2003
//
//----------------------------------------------------------------------------

#include "memory.h"

#if defined(CPUCORE_IA32)
#error : not support CPUCORE_IA32
#endif

#if !defined(CPUDEBUG)
enum {
	I286_MEMREADMAX		= 0xa4000,
	I286_MEMWRITEMAX	= 0xa0000
};
#else									// �_�C���N�g�A�N�Z�X�͈͂����߂�
enum {
	I286_MEMREADMAX		= 0x00400,
	I286_MEMWRITEMAX	= 0x00400
};
#endif

enum {
	C_FLAG			= 0x0001,
	P_FLAG			= 0x0004,
	A_FLAG			= 0x0010,
	Z_FLAG			= 0x0040,
	S_FLAG			= 0x0080,
	T_FLAG			= 0x0100,
	I_FLAG			= 0x0200,
	D_FLAG			= 0x0400,
	O_FLAG			= 0x0800
};

enum {
	CPUTYPE_V30		= 0x01
};

#ifndef CPUCALL
#define	CPUCALL
#endif

#if defined(BYTESEX_LITTLE)

typedef struct {
	UINT8	al;
	UINT8	ah;
	UINT8	cl;
	UINT8	ch;
	UINT8	dl;
	UINT8	dh;
	UINT8	bl;
	UINT8	bh;
	UINT8	sp_l;
	UINT8	sp_h;
	UINT8	bp_l;
	UINT8	bp_h;
	UINT8	si_l;
	UINT8	si_h;
	UINT8	di_l;
	UINT8	di_h;
	UINT8	es_l;
	UINT8	es_h;
	UINT8	cs_l;
	UINT8	cs_h;
	UINT8	ss_l;
	UINT8	ss_h;
	UINT8	ds_l;
	UINT8	ds_h;
	UINT8	flag_l;
	UINT8	flag_h;
	UINT8	ip_l;
	UINT8	ip_h;
} I286REG8;

#else

typedef struct {
	UINT8	ah;
	UINT8	al;
	UINT8	ch;
	UINT8	cl;
	UINT8	dh;
	UINT8	dl;
	UINT8	bh;
	UINT8	bl;
	UINT8	sp_h;
	UINT8	sp_l;
	UINT8	bp_h;
	UINT8	bp_l;
	UINT8	si_h;
	UINT8	si_l;
	UINT8	di_h;
	UINT8	di_l;
	UINT8	es_h;
	UINT8	es_l;
	UINT8	cs_h;
	UINT8	cs_l;
	UINT8	ss_h;
	UINT8	ss_l;
	UINT8	ds_h;
	UINT8	ds_l;
	UINT8	flag_h;
	UINT8	flag_l;
	UINT8	ip_h;
	UINT8	ip_l;
} I286REG8;

#endif

typedef struct {
	UINT16	ax;
	UINT16	cx;
	UINT16	dx;
	UINT16	bx;
	UINT16	sp;
	UINT16	bp;
	UINT16	si;
	UINT16	di;
	UINT16	es;
	UINT16	cs;
	UINT16	ss;
	UINT16	ds;
	UINT16	flag;
	UINT16	ip;
} I286REG16;

typedef struct {
	UINT16	limit;
	UINT16	base;
	UINT8	base24;
	UINT8	reserved;
} I286DTR;

typedef struct {
	union {
		I286REG8	b;
		I286REG16	w;
	}		r;
	SINT32	remainclock;
	SINT32	baseclock;
	UINT32	clock;
	UINT32	adrsmask;						// ver0.72
	UINT32	es_base;
	UINT32	cs_base;
	UINT32	ss_base;
	UINT32	ds_base;
	UINT32	ss_fix;
	UINT32	ds_fix;
	UINT16	prefix;
	UINT8	trap;
	UINT8	cpu_type;
	UINT32	____pf_semaphore;				// ���g�p
	UINT32	____repbak;						// ���g�p
	UINT32	inport;
	UINT32	ovflag;
	I286DTR	GDTR;
	I286DTR	IDTR;
	UINT16	MSW;
	UINT8	resetreq;						// ver0.72
	UINT8	itfbank;						// ver0.72
} I286STAT;

typedef struct {							// for ver0.73
	BYTE	*ext;
	UINT32	extsize;
#if defined(CPUSTRUC_MEMWAIT)
	UINT8	tramwait;
	UINT8	vramwait;
	UINT8	grcgwait;
	UINT8	padding;
#endif
} I286EXT;

typedef struct {
	I286STAT	s;							// STATsave�����z
	I286EXT		e;
	BYTE		m[0x200000];
} I286CORE;


#ifdef __cplusplus
extern "C" {
#endif

extern	I286CORE	i286acore;
extern	const UINT8	iflags[];

void i286a_reset(void);
void i286a_shut(void);

void CPUCALL i286a_interrupt(REG8 vect);

void i286a(void);
void i286a_step(void);

#ifdef __cplusplus
}
#endif


// ---- macros

#define	mem				i286acore.m

#define	CPU_STATSAVE	i286acore.s

#define	CPU_AX			i286acore.s.r.w.ax
#define	CPU_BX			i286acore.s.r.w.bx
#define	CPU_CX			i286acore.s.r.w.cx
#define	CPU_DX			i286acore.s.r.w.dx
#define	CPU_SI			i286acore.s.r.w.si
#define	CPU_DI			i286acore.s.r.w.di
#define	CPU_BP			i286acore.s.r.w.bp
#define	CPU_SP			i286acore.s.r.w.sp
#define	CPU_CS			i286acore.s.r.w.cs
#define	CPU_DS			i286acore.s.r.w.ds
#define	CPU_ES			i286acore.s.r.w.es
#define	CPU_SS			i286acore.s.r.w.ss
#define	CPU_IP			i286acore.s.r.w.ip

#define	ES_BASE			i286acore.s.es_base
#define	CS_BASE			i286acore.s.cs_base
#define	SS_BASE			i286acore.s.ss_base
#define	DS_BASE			i286acore.s.ds_base

#define	CPU_AL			i286acore.s.r.b.al
#define	CPU_BL			i286acore.s.r.b.bl
#define	CPU_CL			i286acore.s.r.b.cl
#define	CPU_DL			i286acore.s.r.b.dl
#define	CPU_AH			i286acore.s.r.b.ah
#define	CPU_BH			i286acore.s.r.b.bh
#define	CPU_CH			i286acore.s.r.b.ch
#define	CPU_DH			i286acore.s.r.b.dh

#define	CPU_FLAG		i286acore.s.r.w.flag
#define	CPU_FLAGL		i286acore.s.r.b.flag_l

#define	SS_FIX			i286acore.s.ss_fix
#define	DS_FIX			i286acore.s.ds_fix

#define	CPU_REMCLOCK	i286acore.s.remainclock
#define	CPU_BASECLOCK	i286acore.s.baseclock
#define	CPU_CLOCK		i286acore.s.clock
#define	CPU_ADRSMASK	i286acore.s.adrsmask
#define	CPU_MSW			i286acore.s.MSW
#define	CPU_RESETREQ	i286acore.s.resetreq
#define	CPU_ITFBANK		i286acore.s.itfbank
#define	CPU_INPADRS		i286acore.s.inport

#define	CPU_EXTMEM		i286acore.e.ext
#define	CPU_EXTMEMSIZE	i286acore.e.extsize

#define	CPU_TYPE		i286acore.s.cpu_type

#if defined(CPUSTRUC_MEMWAIT)
#define	MEMWAIT_TRAM	i286acore.e.tramwait
#define	MEMWAIT_VRAM	i286acore.e.vramwait
#define	MEMWAIT_GRCG	i286acore.e.grcgwait
#endif


#define	CPU_isDI		(!(i286acore.s.r.w.flag & I_FLAG))
#define	CPU_isEI		(i286acore.s.r.w.flag & I_FLAG)
#define	CPU_CLI			i286acore.s.r.w.flag &= ~I_FLAG;					\
						i286acore.s.trap = 0;
#define	CPU_STI			i286acore.s.r.w.flag |= I_FLAG;						\
						i286acore.s.trap = (i286acore.s.r.w.flag >> 8) & 1;

#define	CPU_INITIALIZE()
#define	CPU_RESET			i286a_reset
#define	CPU_CLEARPREFETCH()
#define	CPU_INTERRUPT(v)	i286a_interrupt(v)
#define	CPU_EXEC			i286a
#define	CPU_EXECV30			i286a
#define	CPU_SHUT			i286a_shut


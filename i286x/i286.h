//----------------------------------------------------------------------------
//
//  i286c : 80286 Engine for Pentium  ver0.02
//
//                               Copyright by Yui/Studio Milmake 1999-2000
//
//----------------------------------------------------------------------------

#if !defined(CPUDEBUG)
enum {
	I286_MEMREADMAX		= 0xa4000,
	I286_MEMWRITEMAX	= 0xa0000
};
#else									// ダイレクトアクセス範囲を狭める
enum {
	I286_MEMREADMAX		= 0x00400,
	I286_MEMWRITEMAX	= 0x00400
};
#endif

#ifndef CPUCALL
#define	CPUCALL			__fastcall
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
	UINT32	pf_semaphore;
	UINT32	repbak;
	UINT32	inport;
	BYTE	prefetchque[4];
	I286DTR	GDTR;
	I286DTR	IDTR;
	UINT16	MSW;
	UINT8	resetreq;						// ver0.72
	UINT8	itfbank;						// ver0.72
} I286STAT;

typedef struct {							// for ver0.73
	UINT	dummy;
} I286EXT;

typedef struct {
	I286STAT	s;							// STATsaveされる奴
	I286EXT		e;
} I286CORE;


#ifdef __cplusplus
extern "C" {
#endif

extern	I286CORE	i286core;
extern	const BYTE	iflags[];


#define		I286_MEM		mem

#define		I286_REG		i286core.s.r
#define		I286_SEGREG		i286core.s.r.w.es

#define		I286_AX			i286core.s.r.w.ax
#define		I286_BX			i286core.s.r.w.bx
#define		I286_CX			i286core.s.r.w.cx
#define		I286_DX			i286core.s.r.w.dx
#define		I286_SI			i286core.s.r.w.si
#define		I286_DI			i286core.s.r.w.di
#define		I286_BP			i286core.s.r.w.bp
#define		I286_SP			i286core.s.r.w.sp
#define		I286_CS			i286core.s.r.w.cs
#define		I286_DS			i286core.s.r.w.ds
#define		I286_ES			i286core.s.r.w.es
#define		I286_SS			i286core.s.r.w.ss
#define		I286_IP			i286core.s.r.w.ip

#define		SEG_BASE		i286core.s.es_base
#define		ES_BASE			i286core.s.es_base
#define		CS_BASE			i286core.s.cs_base
#define		SS_BASE			i286core.s.ss_base
#define		DS_BASE			i286core.s.ds_base
#define		SS_FIX			i286core.s.ss_fix
#define		DS_FIX			i286core.s.ds_fix

#define		I286_AL			i286core.s.r.b.al
#define		I286_BL			i286core.s.r.b.bl
#define		I286_CL			i286core.s.r.b.cl
#define		I286_DL			i286core.s.r.b.dl
#define		I286_AH			i286core.s.r.b.ah
#define		I286_BH			i286core.s.r.b.bh
#define		I286_CH			i286core.s.r.b.ch
#define		I286_DH			i286core.s.r.b.dh

#define		I286_FLAG		i286core.s.r.w.flag
#define		I286_FLAGL		i286core.s.r.b.flag_l
#define		I286_FLAGH		i286core.s.r.b.flag_h
#define		I286_TRAP		i286core.s.trap
#define		I286_OV			i286core.s.ovflag

#define		I286_REMCLOCK	i286core.s.remainclock
#define		I286_BASECLOCK	i286core.s.baseclock
#define		I286_CLOCK		i286core.s.clock


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

#define		CPUTYPE			i286core.s.cpu_type
#define		CPUTYPE_V30		0x01

// ver0.28
#define		REPPOSBAK		i286core.s.repbak
#define		PREFIX_SMP		i286core.s.pf_semaphore

#define		isI286DI		(!(I286_FLAG & I_FLAG))
#define		isI286EI		(I286_FLAG & I_FLAG)
#define		I286_CLI		I286_FLAG &= ~I_FLAG;				\
							I286_TRAP = 0;
#define		I286_STI		I286_FLAG |= I_FLAG;				\
							I286_TRAP = (I286_FLAG >> 8) & 1;


void i286_initialize(void);
void i286_reset(void);
void i286_resetprefetch(void);

void CPUCALL i286_interrupt(BYTE vect);

void i286(void);
void i286_step(void);

void v30(void);
void v30_step(void);

#ifdef __cplusplus
}
#endif


//----------------------------------------------------------------------------
//
//  i286y : 80286 Engine for Pentium  ver0.02
//
//                               Copyright by Yui/Studio Milmake 1999-2000
//
//----------------------------------------------------------------------------

enum {
	I286_MEMREADMAX		= 0xa4000,
	I286_MEMWRITEMAX	= 0xa0000
};

#ifndef CPUCALL
#define	CPUCALL
#endif

#if defined(BYTESEX_LITTLE)

typedef struct {
	BYTE	al;
	BYTE	ah;
	BYTE	cl;
	BYTE	ch;
	BYTE	dl;
	BYTE	dh;
	BYTE	bl;
	BYTE	bh;
	BYTE	sp_l;
	BYTE	sp_h;
	BYTE	bp_l;
	BYTE	bp_h;
	BYTE	si_l;
	BYTE	si_h;
	BYTE	di_l;
	BYTE	di_h;
	BYTE	es_l;
	BYTE	es_h;
	BYTE	cs_l;
	BYTE	cs_h;
	BYTE	ss_l;
	BYTE	ss_h;
	BYTE	ds_l;
	BYTE	ds_h;

	BYTE	flag_l;
	BYTE	flag_h;
	BYTE	ip_l;
	BYTE	ip_h;
} I286REG_BYTE;

#else

typedef struct {
	BYTE	ah;
	BYTE	al;
	BYTE	ch;
	BYTE	cl;
	BYTE	dh;
	BYTE	dl;
	BYTE	bh;
	BYTE	bl;
	BYTE	sp_h;
	BYTE	sp_l;
	BYTE	bp_h;
	BYTE	bp_l;
	BYTE	si_h;
	BYTE	si_l;
	BYTE	di_h;
	BYTE	di_l;
	BYTE	es_h;
	BYTE	es_l;
	BYTE	cs_h;
	BYTE	cs_l;
	BYTE	ss_h;
	BYTE	ss_l;
	BYTE	ds_h;
	BYTE	ds_l;
	BYTE	flag_h;
	BYTE	flag_l;
	BYTE	ip_h;
	BYTE	ip_l;
} I286REG_BYTE;

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
} I286REG_WORD;

typedef union {
	I286REG_BYTE	b;
	I286REG_WORD	w;
} I286REGS;

typedef struct {
	UINT32	es_base;
	UINT32	cs_base;
	UINT32	ss_base;
	UINT32	ds_base;
	UINT32	ss_fix;
	UINT32	ds_fix;
	UINT16	prefix;
	BYTE	trap;
	BYTE	reserved;
	UINT32	pf_semaphore;
	UINT32	repbak;
	UINT32	inport;
	UINT32	ovflag;
	BYTE	cpu_type;
} I286STAT;

typedef struct {
	UINT16	limit;
	UINT16	base;
	BYTE	base24;
	BYTE	reserved;
} I286DTR;


#ifdef __cplusplus
extern "C" {
#endif

extern	I286REGS	i286r;
extern	I286STAT	i286s;
extern	I286DTR		GDTR;
extern	I286DTR		IDTR;
extern	UINT16		MSW;
extern	const BYTE	iflags[];


#define		I286_MEM		mem

#define		I286_REG		i286r
#define		I286_SEGREG		i286r.w.es

#define		I286_AX			i286r.w.ax
#define		I286_BX			i286r.w.bx
#define		I286_CX			i286r.w.cx
#define		I286_DX			i286r.w.dx
#define		I286_SI			i286r.w.si
#define		I286_DI			i286r.w.di
#define		I286_BP			i286r.w.bp
#define		I286_SP			i286r.w.sp
#define		I286_CS			i286r.w.cs
#define		I286_DS			i286r.w.ds
#define		I286_ES			i286r.w.es
#define		I286_SS			i286r.w.ss
#define		I286_IP			i286r.w.ip

#define		SEG_BASE		i286s.es_base
#define		ES_BASE			i286s.es_base
#define		CS_BASE			i286s.cs_base
#define		SS_BASE			i286s.ss_base
#define		DS_BASE			i286s.ds_base
#define		SS_FIX			i286s.ss_fix
#define		DS_FIX			i286s.ds_fix

#define		I286_AL			i286r.b.al
#define		I286_BL			i286r.b.bl
#define		I286_CL			i286r.b.cl
#define		I286_DL			i286r.b.dl
#define		I286_AH			i286r.b.ah
#define		I286_BH			i286r.b.bh
#define		I286_CH			i286r.b.ch
#define		I286_DH			i286r.b.dh

#define		I286_FLAG		i286r.w.flag
#define		I286_FLAGL		i286r.b.flag_l
#define		I286_FLAGH		i286r.b.flag_h
#define		I286_TRAP		i286s.trap
#define		I286_OV			i286s.ovflag

#define		C_FLAG			0x0001
#define		P_FLAG			0x0004
#define		A_FLAG			0x0010
#define		Z_FLAG			0x0040
#define		S_FLAG			0x0080
#define		T_FLAG			0x0100
#define		I_FLAG			0x0200
#define		D_FLAG			0x0400
#define		O_FLAG			0x0800

#define		CPUTYPE			i286s.cpu_type
#define		CPUTYPE_V30		0x01

// ver0.28
#define		REPPOSBAK		i286s.repbak
#define		PREFIX_SMP		i286s.pf_semaphore

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

#define	v30				i286
#define	v30_step		i286_step

#ifdef __cplusplus
}
#endif


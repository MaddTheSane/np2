/*	$Id: disasm.c,v 1.1 2003/12/08 00:55:31 yui Exp $	*/
/*	$NetBSD: db_disasm.c,v 1.28 2002/10/01 12:56:49 fvdl Exp $	*/

/* 
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 *
 *	Id: db_disasm.c,v 2.3 91/02/05 17:11:03 mrt (CMU)
 */

/*
 * Instruction disassembler.
 */

#include "cpu.h"

typedef	BOOL	boolean_t;
typedef	DWORD	db_addr_t;
typedef	long	db_expr_t;

#define	db_radix				16
#define	db_printf				ia32_printf
#define	db_printsym(off, strategy, funcp)	db_printf(db_num_to_str(off))
#define	db_get_value(cs,loc,size,is_signed) \
	(db_expr_t)((size == 4) ? cpu_vmemoryread_d(cs,loc) \
	          : ((size == 2) ? cpu_vmemoryread_w(cs,loc) \
	          : ((size == 1) ? cpu_vmemoryread(cs,loc) : 0)))

static char *
db_num_to_str(db_expr_t val)
{
	static char buf[25];

	if (db_radix == 16) {
		snprintf(buf, sizeof(buf), "%lx", val);
	} else if (db_radix == 8) {
		snprintf(buf, sizeof(buf), "%lo", val);
	} else {
		snprintf(buf, sizeof(buf), "%lu", val);
	}
	return buf;
}

static void
db_format_radix(char *buf, size_t bufsiz, SQWORD val, int altflag)
{
	const char *fmt;

	UNUSED(altflag);

	if (db_radix == 16) {
		fmt = "-%llx";
	} else if (db_radix == 8) {
		fmt = "-%llo";
	} else {
		fmt = "-%llu";
	}
	if (val < 0)
		val = -val;
	else
		++fmt;

	snprintf(buf, bufsiz, fmt, val);
}

/*
 * Size attributes
 */
#define	DISASM_BYTE	0
#define	DISASM_WORD	1
#define	DISASM_LONG	2
#define	DISASM_QUAD	3
#define	DISASM_SNGL	4
#define	DISASM_DBLR	5
#define	DISASM_EXTR	6
#define	DISASM_SDEP	7
#define	DISASM_NONE	8

/*
 * Addressing modes
 */
#define	E	1			/* general effective address */
#define	Eind	2			/* indirect address (jump, call) */
#define	Ew	3			/* address, word size */
#define	Eb	4			/* address, byte size */
#define	R	5			/* register, in 'reg' field */
#define	Rw	6			/* word register, in 'reg' field */
#define	Ri	7			/* register in instruction */
#define	S	8			/* segment reg, in 'reg' field */
#define	Si	9			/* segment reg, in instruction */
#define	A	10			/* accumulator */
#define	BX	11			/* (bx) */
#define	CL	12			/* cl, for shifts */
#define	DX	13			/* dx, for IO */
#define	SI	14			/* si */
#define	DI	15			/* di */
#define	CR	16			/* control register */
#define	DR	17			/* debug register */
#define	TR	18			/* test register */
#define	I	19			/* immediate, unsigned */
#define	Is	20			/* immediate, signed */
#define	Ib	21			/* byte immediate, unsigned */
#define	Ibs	22			/* byte immediate, signed */
#define	Iw	23			/* word immediate, unsigned */
#define	Il	24			/* long immediate */
#define	O	25			/* direct address */
#define	Db	26			/* byte displacement from EIP */
#define	Dl	27			/* long displacement from EIP */
#define	o1	28			/* constant 1 */
#define	o3	29			/* constant 3 */
#define	OS	30			/* immediate offset/segment */
#define	ST	31			/* FP stack top */
#define	STI	32			/* FP stack */
#define	X	33			/* extended FP op */
#define	XA	34			/* for 'fstcw %ax' */

struct inst {
	char *	i_name;			/* name */
	short	i_has_modrm;		/* has regmodrm byte */
	short	i_size;			/* operand size */
	int	i_mode;			/* addressing modes */
	char *	i_extra;		/* pointer to extra opcode table */
};

#define	op1(x)		(x)
#define	op2(x,y)	((x)|((y)<<8))
#define	op3(x,y,z)	((x)|((y)<<8)|((z)<<16))

struct finst {
	char *	f_name;			/* name for memory instruction */
	int	f_size;			/* size for memory instruction */
	int	f_rrmode;		/* mode for rr instruction */
	char *	f_rrname;		/* name for rr instruction
					   (or pointer to table) */
};

const char * const db_Grp6[] = {
	"sldt",
	"str",
	"lldt",
	"ltr",
	"verr",
	"verw",
	"",
	""
};

const char * const db_Grp7[] = {
	"sgdt",
	"sidt",
	"lgdt",
	"lidt",
	"smsw",
	"",
	"lmsw",
	"invlpg"
};

const char * const db_Grp8[] = {
	"",
	"",
	"",
	"",
	"bt",
	"bts",
	"btr",
	"btc"
};

const char * const db_Grp9[] = {
	"",
	"cmpxchg8b",
	"",
	"",
	"",
	"",
	"",
	"",
};

const struct inst db_inst_0f0x[] = {
/*00*/	{ "",	   TRUE,  DISASM_NONE,  op1(Ew),     (char *)db_Grp6 },
/*01*/	{ "",	   TRUE,  DISASM_NONE,  op1(Ew),     (char *)db_Grp7 },
/*02*/	{ "lar",   TRUE,  DISASM_LONG,  op2(E,R),    0 },
/*03*/	{ "lsl",   TRUE,  DISASM_LONG,  op2(E,R),    0 },
/*04*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*05*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*06*/	{ "clts",  FALSE, DISASM_NONE,  0,	      0 },
/*07*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },

/*08*/	{ "invd",  FALSE, DISASM_NONE,  0,	      0 },
/*09*/	{ "wbinvd",FALSE, DISASM_NONE,  0,	      0 },
/*0a*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*0b*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*0c*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*0d*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*0e*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*0f*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
};

const struct inst	db_inst_0f2x[] = {
/*20*/	{ "mov",   TRUE,  DISASM_LONG,  op2(CR,E),   0 }, /* use E for reg */
/*21*/	{ "mov",   TRUE,  DISASM_LONG,  op2(DR,E),   0 }, /* since mod == 11 */
/*22*/	{ "mov",   TRUE,  DISASM_LONG,  op2(E,CR),   0 },
/*23*/	{ "mov",   TRUE,  DISASM_LONG,  op2(E,DR),   0 },
/*24*/	{ "mov",   TRUE,  DISASM_LONG,  op2(TR,E),   0 },
/*25*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*26*/	{ "mov",   TRUE,  DISASM_LONG,  op2(E,TR),   0 },
/*27*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },

/*28*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*29*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*2a*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*2b*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*2c*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*2d*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*2e*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*2f*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
};

const struct inst	db_inst_0f3x[] = {
/*30*/	{ "wrmsr", FALSE, DISASM_NONE,  0,	      0 },
/*31*/	{ "rdtsc", FALSE, DISASM_NONE,  0,	      0 },
/*32*/	{ "rdmsr", FALSE, DISASM_NONE,  0,	      0 },
/*33*/	{ "rdpmc", FALSE, DISASM_NONE,  0,	      0 },
/*34*/	{ "",	   FALSE, DISASM_NONE,  0,	      0 },
/*35*/	{ "",	   FALSE, DISASM_NONE,  0,	      0 },
/*36*/	{ "",	   FALSE, DISASM_NONE,  0,	      0 },
/*37*/	{ "",	   FALSE, DISASM_NONE,  0,	      0 },

/*38*/	{ "",	   FALSE, DISASM_NONE,  0,	      0 },
/*39*/	{ "",	   FALSE, DISASM_NONE,  0,	      0 },
/*3a*/	{ "",	   FALSE, DISASM_NONE,  0,	      0 },
/*3v*/	{ "",	   FALSE, DISASM_NONE,  0,	      0 },
/*3c*/	{ "",	   FALSE, DISASM_NONE,  0,	      0 },
/*3d*/	{ "",	   FALSE, DISASM_NONE,  0,	      0 },
/*3e*/	{ "",	   FALSE, DISASM_NONE,  0,	      0 },
/*3f*/	{ "",	   FALSE, DISASM_NONE,  0,	      0 },
};

const struct inst	db_inst_0f8x[] = {
/*80*/	{ "jo",    FALSE, DISASM_NONE,  op1(Dl),     0 },
/*81*/	{ "jno",   FALSE, DISASM_NONE,  op1(Dl),     0 },
/*82*/	{ "jb",    FALSE, DISASM_NONE,  op1(Dl),     0 },
/*83*/	{ "jnb",   FALSE, DISASM_NONE,  op1(Dl),     0 },
/*84*/	{ "jz",    FALSE, DISASM_NONE,  op1(Dl),     0 },
/*85*/	{ "jnz",   FALSE, DISASM_NONE,  op1(Dl),     0 },
/*86*/	{ "jbe",   FALSE, DISASM_NONE,  op1(Dl),     0 },
/*87*/	{ "jnbe",  FALSE, DISASM_NONE,  op1(Dl),     0 },

/*88*/	{ "js",    FALSE, DISASM_NONE,  op1(Dl),     0 },
/*89*/	{ "jns",   FALSE, DISASM_NONE,  op1(Dl),     0 },
/*8a*/	{ "jp",    FALSE, DISASM_NONE,  op1(Dl),     0 },
/*8b*/	{ "jnp",   FALSE, DISASM_NONE,  op1(Dl),     0 },
/*8c*/	{ "jl",    FALSE, DISASM_NONE,  op1(Dl),     0 },
/*8d*/	{ "jnl",   FALSE, DISASM_NONE,  op1(Dl),     0 },
/*8e*/	{ "jle",   FALSE, DISASM_NONE,  op1(Dl),     0 },
/*8f*/	{ "jnle",  FALSE, DISASM_NONE,  op1(Dl),     0 },
};

const struct inst	db_inst_0f9x[] = {
/*90*/	{ "seto",  TRUE,  DISASM_NONE,  op1(Eb),     0 },
/*91*/	{ "setno", TRUE,  DISASM_NONE,  op1(Eb),     0 },
/*92*/	{ "setb",  TRUE,  DISASM_NONE,  op1(Eb),     0 },
/*93*/	{ "setnb", TRUE,  DISASM_NONE,  op1(Eb),     0 },
/*94*/	{ "setz",  TRUE,  DISASM_NONE,  op1(Eb),     0 },
/*95*/	{ "setnz", TRUE,  DISASM_NONE,  op1(Eb),     0 },
/*96*/	{ "setbe", TRUE,  DISASM_NONE,  op1(Eb),     0 },
/*97*/	{ "setnbe",TRUE,  DISASM_NONE,  op1(Eb),     0 },

/*98*/	{ "sets",  TRUE,  DISASM_NONE,  op1(Eb),     0 },
/*99*/	{ "setns", TRUE,  DISASM_NONE,  op1(Eb),     0 },
/*9a*/	{ "setp",  TRUE,  DISASM_NONE,  op1(Eb),     0 },
/*9b*/	{ "setnp", TRUE,  DISASM_NONE,  op1(Eb),     0 },
/*9c*/	{ "setl",  TRUE,  DISASM_NONE,  op1(Eb),     0 },
/*9d*/	{ "setnl", TRUE,  DISASM_NONE,  op1(Eb),     0 },
/*9e*/	{ "setle", TRUE,  DISASM_NONE,  op1(Eb),     0 },
/*9f*/	{ "setnle",TRUE,  DISASM_NONE,  op1(Eb),     0 },
};

const struct inst	db_inst_0fax[] = {
/*a0*/	{ "push",  FALSE, DISASM_NONE,  op1(Si),     0 },
/*a1*/	{ "pop",   FALSE, DISASM_NONE,  op1(Si),     0 },
/*a2*/	{ "cpuid", FALSE, DISASM_NONE,  0,	      0 },
/*a3*/	{ "bt",    TRUE,  DISASM_LONG,  op2(R,E),    0 },
/*a4*/	{ "shld",  TRUE,  DISASM_LONG,  op3(Ib,E,R), 0 },
/*a5*/	{ "shld",  TRUE,  DISASM_LONG,  op3(CL,E,R), 0 },
/*a6*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*a7*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },

/*a8*/	{ "push",  FALSE, DISASM_NONE,  op1(Si),     0 },
/*a9*/	{ "pop",   FALSE, DISASM_NONE,  op1(Si),     0 },
/*aa*/	{ "rsm",   FALSE, DISASM_NONE,  0,	      0 },
/*ab*/	{ "bts",   TRUE,  DISASM_LONG,  op2(R,E),    0 },
/*ac*/	{ "shrd",  TRUE,  DISASM_LONG,  op3(Ib,E,R), 0 },
/*ad*/	{ "shrd",  TRUE,  DISASM_LONG,  op3(CL,E,R), 0 },
/*ae*/	{ "fxsave",TRUE,  DISASM_LONG,  0,	      0 },
/*af*/	{ "imul",  TRUE,  DISASM_LONG,  op2(E,R),    0 },
};

const struct inst	db_inst_0fbx[] = {
/*b0*/	{ "cmpxchg",TRUE, DISASM_BYTE,	 op2(R, E),   0 },
/*b1*/	{ "cmpxchg",TRUE, DISASM_LONG,	 op2(R, E),   0 },
/*b2*/	{ "lss",   TRUE,  DISASM_LONG,  op2(E, R),   0 },
/*b3*/	{ "btr",   TRUE,  DISASM_LONG,  op2(R, E),   0 },
/*b4*/	{ "lfs",   TRUE,  DISASM_LONG,  op2(E, R),   0 },
/*b5*/	{ "lgs",   TRUE,  DISASM_LONG,  op2(E, R),   0 },
/*b6*/	{ "movzb", TRUE,  DISASM_LONG,  op2(E, R),   0 },
/*b7*/	{ "movzw", TRUE,  DISASM_LONG,  op2(E, R),   0 },

/*b8*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*b9*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*ba*/	{ "",      TRUE,  DISASM_LONG,  op2(Ib, E),  (char *)db_Grp8 },
/*bb*/	{ "btc",   TRUE,  DISASM_LONG,  op2(R, E),   0 },
/*bc*/	{ "bsf",   TRUE,  DISASM_LONG,  op2(E, R),   0 },
/*bd*/	{ "bsr",   TRUE,  DISASM_LONG,  op2(E, R),   0 },
/*be*/	{ "movsb", TRUE,  DISASM_LONG,  op2(E, R),   0 },
/*bf*/	{ "movsw", TRUE,  DISASM_LONG,  op2(E, R),   0 },
};

const struct inst	db_inst_0fcx[] = {
/*c0*/	{ "xadd",  TRUE,  DISASM_BYTE,	 op2(R, E),   0 },
/*c1*/	{ "xadd",  TRUE,  DISASM_LONG,	 op2(R, E),   0 },
/*c2*/	{ "",	   FALSE, DISASM_NONE,	 0,	      0 },
/*c3*/	{ "",	   FALSE, DISASM_NONE,	 0,	      0 },
/*c4*/	{ "",	   FALSE, DISASM_NONE,	 0,	      0 },
/*c5*/	{ "",	   FALSE, DISASM_NONE,	 0,	      0 },
/*c6*/	{ "",	   FALSE, DISASM_NONE,	 0,	      0 },
/*c7*/	{ "",	   TRUE,  DISASM_NONE,	 op1(E),      (char *)db_Grp9 },
/*c8*/	{ "bswap", FALSE, DISASM_LONG,  op1(Ri),     0 },
/*c9*/	{ "bswap", FALSE, DISASM_LONG,  op1(Ri),     0 },
/*ca*/	{ "bswap", FALSE, DISASM_LONG,  op1(Ri),     0 },
/*cb*/	{ "bswap", FALSE, DISASM_LONG,  op1(Ri),     0 },
/*cc*/	{ "bswap", FALSE, DISASM_LONG,  op1(Ri),     0 },
/*cd*/	{ "bswap", FALSE, DISASM_LONG,  op1(Ri),     0 },
/*ce*/	{ "bswap", FALSE, DISASM_LONG,  op1(Ri),     0 },
/*cf*/	{ "bswap", FALSE, DISASM_LONG,  op1(Ri),     0 },
};

const struct inst * const db_inst_0f[] = {
	db_inst_0f0x,
	0,
	db_inst_0f2x,
	db_inst_0f3x,
	0,
	0,
	0,
	0,
	db_inst_0f8x,
	db_inst_0f9x,
	db_inst_0fax,
	db_inst_0fbx,
	db_inst_0fcx,
	0,
	0,
	0
};

const char * const db_Esc92[] = {
	"fnop",	"",	"",	"",	"",	"",	"",	""
};
const char * const db_Esc93[] = {
	"",	"",	"",	"",	"",	"",	"",	""
};
const char * const db_Esc94[] = {
	"fchs",	"fabs",	"",	"",	"ftst",	"fxam",	"",	""
};
const char * const db_Esc95[] = {
	"fld1",	"fldl2t","fldl2e","fldpi","fldlg2","fldln2","fldz",""
};
const char * const db_Esc96[] = {
	"f2xm1","fyl2x","fptan","fpatan","fxtract","fprem1","fdecstp",
	"fincstp"
};
const char * const db_Esc97[] = {
	"fprem","fyl2xp1","fsqrt","fsincos","frndint","fscale","fsin","fcos"
};

const char * const db_Esca4[] = {
	"",	"fucompp","",	"",	"",	"",	"",	""
};

const char * const db_Escb4[] = {
	"",	"",	"fnclex","fninit","",	"",	"",	""
};

const char * const db_Esce3[] = {
	"",	"fcompp","",	"",	"",	"",	"",	""
};

const char * const db_Escf4[] = {
	"fnstsw","",	"",	"",	"",	"",	"",	""
};

const struct finst db_Esc8[] = {
/*0*/	{ "fadd",   DISASM_SNGL,  op2(STI,ST),	0 },
/*1*/	{ "fmul",   DISASM_SNGL,  op2(STI,ST),	0 },
/*2*/	{ "fcom",   DISASM_SNGL,  op2(STI,ST),	0 },
/*3*/	{ "fcomp",  DISASM_SNGL,  op2(STI,ST),	0 },
/*4*/	{ "fsub",   DISASM_SNGL,  op2(STI,ST),	0 },
/*5*/	{ "fsubr",  DISASM_SNGL,  op2(STI,ST),	0 },
/*6*/	{ "fdiv",   DISASM_SNGL,  op2(STI,ST),	0 },
/*7*/	{ "fdivr",  DISASM_SNGL,  op2(STI,ST),	0 },
};

const struct finst db_Esc9[] = {
/*0*/	{ "fld",    DISASM_SNGL,  op1(STI),	0 },
/*1*/	{ "",       DISASM_NONE,  op1(STI),	"fxch" },
/*2*/	{ "fst",    DISASM_SNGL,  op1(X),	(char *)db_Esc92 },
/*3*/	{ "fstp",   DISASM_SNGL,  op1(X),	(char *)db_Esc93 },
/*4*/	{ "fldenv", DISASM_NONE,  op1(X),	(char *)db_Esc94 },
/*5*/	{ "fldcw",  DISASM_NONE,  op1(X),	(char *)db_Esc95 },
/*6*/	{ "fnstenv",DISASM_NONE,  op1(X),	(char *)db_Esc96 },
/*7*/	{ "fnstcw", DISASM_NONE,  op1(X),	(char *)db_Esc97 },
};

const struct finst db_Esca[] = {
/*0*/	{ "fiadd",  DISASM_WORD,  0,		0 },
/*1*/	{ "fimul",  DISASM_WORD,  0,		0 },
/*2*/	{ "ficom",  DISASM_WORD,  0,		0 },
/*3*/	{ "ficomp", DISASM_WORD,  0,		0 },
/*4*/	{ "fisub",  DISASM_WORD,  op1(X),	(char *)db_Esca4 },
/*5*/	{ "fisubr", DISASM_WORD,  0,		0 },
/*6*/	{ "fidiv",  DISASM_WORD,  0,		0 },
/*7*/	{ "fidivr", DISASM_WORD,  0,		0 }
};

const struct finst db_Escb[] = {
/*0*/	{ "fild",   DISASM_WORD,  0,		0 },
/*1*/	{ "",       DISASM_NONE,  0,		0 },
/*2*/	{ "fist",   DISASM_WORD,  0,		0 },
/*3*/	{ "fistp",  DISASM_WORD,  0,		0 },
/*4*/	{ "",       DISASM_WORD,  op1(X),	(char *)db_Escb4 },
/*5*/	{ "fld",    DISASM_EXTR,  0,		0 },
/*6*/	{ "",       DISASM_WORD,  0,		0 },
/*7*/	{ "fstp",   DISASM_EXTR,  0,		0 },
};

const struct finst db_Escc[] = {
/*0*/	{ "fadd",   DISASM_DBLR,  op2(ST,STI),	0 },
/*1*/	{ "fmul",   DISASM_DBLR,  op2(ST,STI),	0 },
/*2*/	{ "fcom",   DISASM_DBLR,  op2(ST,STI),	0 },
/*3*/	{ "fcomp",  DISASM_DBLR,  op2(ST,STI),	0 },
/*4*/	{ "fsub",   DISASM_DBLR,  op2(ST,STI),	"fsubr" },
/*5*/	{ "fsubr",  DISASM_DBLR,  op2(ST,STI),	"fsub" },
/*6*/	{ "fdiv",   DISASM_DBLR,  op2(ST,STI),	"fdivr" },
/*7*/	{ "fdivr",  DISASM_DBLR,  op2(ST,STI),	"fdiv" },
};

const struct finst db_Escd[] = {
/*0*/	{ "fld",    DISASM_DBLR,  op1(STI),	"ffree" },
/*1*/	{ "",       DISASM_NONE,  0,		0 },
/*2*/	{ "fst",    DISASM_DBLR,  op1(STI),	0 },
/*3*/	{ "fstp",   DISASM_DBLR,  op1(STI),	0 },
/*4*/	{ "frstor", DISASM_NONE,  op1(STI),	"fucom" },
/*5*/	{ "",       DISASM_NONE,  op1(STI),	"fucomp" },
/*6*/	{ "fnsave", DISASM_NONE,  0,		0 },
/*7*/	{ "fnstsw", DISASM_NONE,  0,		0 },
};

const struct finst db_Esce[] = {
/*0*/	{ "fiadd",  DISASM_LONG,  op2(ST,STI),	"faddp" },
/*1*/	{ "fimul",  DISASM_LONG,  op2(ST,STI),	"fmulp" },
/*2*/	{ "ficom",  DISASM_LONG,  0,		0 },
/*3*/	{ "ficomp", DISASM_LONG,  op1(X),	(char *)db_Esce3 },
/*4*/	{ "fisub",  DISASM_LONG,  op2(ST,STI),	"fsubrp" },
/*5*/	{ "fisubr", DISASM_LONG,  op2(ST,STI),	"fsubp" },
/*6*/	{ "fidiv",  DISASM_LONG,  op2(ST,STI),	"fdivrp" },
/*7*/	{ "fidivr", DISASM_LONG,  op2(ST,STI),	"fdivp" },
};

const struct finst db_Escf[] = {
/*0*/	{ "fild",   DISASM_LONG,  0,		0 },
/*1*/	{ "",       DISASM_LONG,  0,		0 },
/*2*/	{ "fist",   DISASM_LONG,  0,		0 },
/*3*/	{ "fistp",  DISASM_LONG,  0,		0 },
/*4*/	{ "fbld",   DISASM_NONE,  op1(XA),	(char *)db_Escf4 },
/*5*/	{ "fld",    DISASM_QUAD,  0,		0 },
/*6*/	{ "fbstp",  DISASM_NONE,  0,		0 },
/*7*/	{ "fstp",   DISASM_QUAD,  0,		0 },
};

const struct finst * const db_Esc_inst[] = {
	db_Esc8, db_Esc9, db_Esca, db_Escb,
	db_Escc, db_Escd, db_Esce, db_Escf
};

const char * const db_Grp1[] = {
	"add",
	"or",
	"adc",
	"sbb",
	"and",
	"sub",
	"xor",
	"cmp"
};

const char * const db_Grp2[] = {
	"rol",
	"ror",
	"rcl",
	"rcr",
	"shl",
	"shr",
	"shl",
	"sar"
};

const struct inst db_Grp3[] = {
	{ "test",  TRUE, DISASM_NONE, op2(I,E), 0 },
	{ "test",  TRUE, DISASM_NONE, op2(I,E), 0 },
	{ "not",   TRUE, DISASM_NONE, op1(E),   0 },
	{ "neg",   TRUE, DISASM_NONE, op1(E),   0 },
	{ "mul",   TRUE, DISASM_NONE, op2(E,A), 0 },
	{ "imul",  TRUE, DISASM_NONE, op2(E,A), 0 },
	{ "div",   TRUE, DISASM_NONE, op2(E,A), 0 },
	{ "idiv",  TRUE, DISASM_NONE, op2(E,A), 0 },
};

const struct inst	db_Grp4[] = {
	{ "inc",   TRUE, DISASM_BYTE, op1(E),   0 },
	{ "dec",   TRUE, DISASM_BYTE, op1(E),   0 },
	{ "",      TRUE, DISASM_NONE, 0,	 0 },
	{ "",      TRUE, DISASM_NONE, 0,	 0 },
	{ "",      TRUE, DISASM_NONE, 0,	 0 },
	{ "",      TRUE, DISASM_NONE, 0,	 0 },
	{ "",      TRUE, DISASM_NONE, 0,	 0 },
	{ "",      TRUE, DISASM_NONE, 0,	 0 }
};

const struct inst	db_Grp5[] = {
	{ "inc",   TRUE, DISASM_LONG, op1(E),   0 },
	{ "dec",   TRUE, DISASM_LONG, op1(E),   0 },
	{ "call",  TRUE, DISASM_NONE, op1(Eind),0 },
	{ "lcall", TRUE, DISASM_NONE, op1(Eind),0 },
	{ "jmp",   TRUE, DISASM_NONE, op1(Eind),0 },
	{ "ljmp",  TRUE, DISASM_NONE, op1(Eind),0 },
	{ "push",  TRUE, DISASM_LONG, op1(E),   0 },
	{ "",      TRUE, DISASM_NONE, 0,	 0 }
};

const struct inst db_inst_table[256] = {
/*00*/	{ "add",   TRUE,  DISASM_BYTE,  op2(R, E),  0 },
/*01*/	{ "add",   TRUE,  DISASM_LONG,  op2(R, E),  0 },
/*02*/	{ "add",   TRUE,  DISASM_BYTE,  op2(E, R),  0 },
/*03*/	{ "add",   TRUE,  DISASM_LONG,  op2(E, R),  0 },
/*04*/	{ "add",   FALSE, DISASM_BYTE,  op2(Is, A), 0 },
/*05*/	{ "add",   FALSE, DISASM_LONG,  op2(Is, A), 0 },
/*06*/	{ "push",  FALSE, DISASM_NONE,  op1(Si),    0 },
/*07*/	{ "pop",   FALSE, DISASM_NONE,  op1(Si),    0 },

/*08*/	{ "or",    TRUE,  DISASM_BYTE,  op2(R, E),  0 },
/*09*/	{ "or",    TRUE,  DISASM_LONG,  op2(R, E),  0 },
/*0a*/	{ "or",    TRUE,  DISASM_BYTE,  op2(E, R),  0 },
/*0b*/	{ "or",    TRUE,  DISASM_LONG,  op2(E, R),  0 },
/*0c*/	{ "or",    FALSE, DISASM_BYTE,  op2(I, A),  0 },
/*0d*/	{ "or",    FALSE, DISASM_LONG,  op2(I, A),  0 },
/*0e*/	{ "push",  FALSE, DISASM_NONE,  op1(Si),    0 },
/*0f*/	{ "",      FALSE, DISASM_NONE,  0,	     0 },

/*10*/	{ "adc",   TRUE,  DISASM_BYTE,  op2(R, E),  0 },
/*11*/	{ "adc",   TRUE,  DISASM_LONG,  op2(R, E),  0 },
/*12*/	{ "adc",   TRUE,  DISASM_BYTE,  op2(E, R),  0 },
/*13*/	{ "adc",   TRUE,  DISASM_LONG,  op2(E, R),  0 },
/*14*/	{ "adc",   FALSE, DISASM_BYTE,  op2(Is, A), 0 },
/*15*/	{ "adc",   FALSE, DISASM_LONG,  op2(Is, A), 0 },
/*16*/	{ "push",  FALSE, DISASM_NONE,  op1(Si),    0 },
/*17*/	{ "pop",   FALSE, DISASM_NONE,  op1(Si),    0 },

/*18*/	{ "sbb",   TRUE,  DISASM_BYTE,  op2(R, E),  0 },
/*19*/	{ "sbb",   TRUE,  DISASM_LONG,  op2(R, E),  0 },
/*1a*/	{ "sbb",   TRUE,  DISASM_BYTE,  op2(E, R),  0 },
/*1b*/	{ "sbb",   TRUE,  DISASM_LONG,  op2(E, R),  0 },
/*1c*/	{ "sbb",   FALSE, DISASM_BYTE,  op2(Is, A), 0 },
/*1d*/	{ "sbb",   FALSE, DISASM_LONG,  op2(Is, A), 0 },
/*1e*/	{ "push",  FALSE, DISASM_NONE,  op1(Si),    0 },
/*1f*/	{ "pop",   FALSE, DISASM_NONE,  op1(Si),    0 },

/*20*/	{ "and",   TRUE,  DISASM_BYTE,  op2(R, E),  0 },
/*21*/	{ "and",   TRUE,  DISASM_LONG,  op2(R, E),  0 },
/*22*/	{ "and",   TRUE,  DISASM_BYTE,  op2(E, R),  0 },
/*23*/	{ "and",   TRUE,  DISASM_LONG,  op2(E, R),  0 },
/*24*/	{ "and",   FALSE, DISASM_BYTE,  op2(I, A),  0 },
/*25*/	{ "and",   FALSE, DISASM_LONG,  op2(I, A),  0 },
/*26*/	{ "",      FALSE, DISASM_NONE,  0,	     0 },
/*27*/	{ "aaa",   FALSE, DISASM_NONE,  0,	     0 },

/*28*/	{ "sub",   TRUE,  DISASM_BYTE,  op2(R, E),  0 },
/*29*/	{ "sub",   TRUE,  DISASM_LONG,  op2(R, E),  0 },
/*2a*/	{ "sub",   TRUE,  DISASM_BYTE,  op2(E, R),  0 },
/*2b*/	{ "sub",   TRUE,  DISASM_LONG,  op2(E, R),  0 },
/*2c*/	{ "sub",   FALSE, DISASM_BYTE,  op2(Is, A), 0 },
/*2d*/	{ "sub",   FALSE, DISASM_LONG,  op2(Is, A), 0 },
/*2e*/	{ "",      FALSE, DISASM_NONE,  0,	     0 },
/*2f*/	{ "das",   FALSE, DISASM_NONE,  0,	     0 },

/*30*/	{ "xor",   TRUE,  DISASM_BYTE,  op2(R, E),  0 },
/*31*/	{ "xor",   TRUE,  DISASM_LONG,  op2(R, E),  0 },
/*32*/	{ "xor",   TRUE,  DISASM_BYTE,  op2(E, R),  0 },
/*33*/	{ "xor",   TRUE,  DISASM_LONG,  op2(E, R),  0 },
/*34*/	{ "xor",   FALSE, DISASM_BYTE,  op2(I, A),  0 },
/*35*/	{ "xor",   FALSE, DISASM_LONG,  op2(I, A),  0 },
/*36*/	{ "",      FALSE, DISASM_NONE,  0,	     0 },
/*37*/	{ "daa",   FALSE, DISASM_NONE,  0,	     0 },

/*38*/	{ "cmp",   TRUE,  DISASM_BYTE,  op2(R, E),  0 },
/*39*/	{ "cmp",   TRUE,  DISASM_LONG,  op2(R, E),  0 },
/*3a*/	{ "cmp",   TRUE,  DISASM_BYTE,  op2(E, R),  0 },
/*3b*/	{ "cmp",   TRUE,  DISASM_LONG,  op2(E, R),  0 },
/*3c*/	{ "cmp",   FALSE, DISASM_BYTE,  op2(Is, A), 0 },
/*3d*/	{ "cmp",   FALSE, DISASM_LONG,  op2(Is, A), 0 },
/*3e*/	{ "",      FALSE, DISASM_NONE,  0,	     0 },
/*3f*/	{ "aas",   FALSE, DISASM_NONE,  0,	     0 },

/*40*/	{ "inc",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*41*/	{ "inc",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*42*/	{ "inc",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*43*/	{ "inc",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*44*/	{ "inc",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*45*/	{ "inc",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*46*/	{ "inc",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*47*/	{ "inc",   FALSE, DISASM_LONG,  op1(Ri),    0 },

/*48*/	{ "dec",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*49*/	{ "dec",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*4a*/	{ "dec",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*4b*/	{ "dec",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*4c*/	{ "dec",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*4d*/	{ "dec",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*4e*/	{ "dec",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*4f*/	{ "dec",   FALSE, DISASM_LONG,  op1(Ri),    0 },

/*50*/	{ "push",  FALSE, DISASM_LONG,  op1(Ri),    0 },
/*51*/	{ "push",  FALSE, DISASM_LONG,  op1(Ri),    0 },
/*52*/	{ "push",  FALSE, DISASM_LONG,  op1(Ri),    0 },
/*53*/	{ "push",  FALSE, DISASM_LONG,  op1(Ri),    0 },
/*54*/	{ "push",  FALSE, DISASM_LONG,  op1(Ri),    0 },
/*55*/	{ "push",  FALSE, DISASM_LONG,  op1(Ri),    0 },
/*56*/	{ "push",  FALSE, DISASM_LONG,  op1(Ri),    0 },
/*57*/	{ "push",  FALSE, DISASM_LONG,  op1(Ri),    0 },

/*58*/	{ "pop",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*59*/	{ "pop",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*5a*/	{ "pop",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*5b*/	{ "pop",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*5c*/	{ "pop",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*5d*/	{ "pop",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*5e*/	{ "pop",   FALSE, DISASM_LONG,  op1(Ri),    0 },
/*5f*/	{ "pop",   FALSE, DISASM_LONG,  op1(Ri),    0 },

/*60*/	{ "pusha", FALSE, DISASM_LONG,  0,	     0 },
/*61*/	{ "popa",  FALSE, DISASM_LONG,  0,	     0 },
/*62*/  { "bound", TRUE,  DISASM_LONG,  op2(E, R),  0 },
/*63*/	{ "arpl",  TRUE,  DISASM_NONE,  op2(Ew,Rw), 0 },

/*64*/	{ "",      FALSE, DISASM_NONE,  0,	     0 },
/*65*/	{ "",      FALSE, DISASM_NONE,  0,	     0 },
/*66*/	{ "",      FALSE, DISASM_NONE,  0,	     0 },
/*67*/	{ "",      FALSE, DISASM_NONE,  0,	     0 },

/*68*/	{ "push",  FALSE, DISASM_LONG,  op1(I),     0 },
/*69*/  { "imul",  TRUE,  DISASM_LONG,  op3(I,E,R), 0 },
/*6a*/	{ "push",  FALSE, DISASM_LONG,  op1(Ib),    0 },
/*6b*/  { "imul",  TRUE,  DISASM_LONG,  op3(Ibs,E,R),0 },
/*6c*/	{ "ins",   FALSE, DISASM_BYTE,  op2(DX, DI), 0 },
/*6d*/	{ "ins",   FALSE, DISASM_LONG,  op2(DX, DI), 0 },
/*6e*/	{ "outs",  FALSE, DISASM_BYTE,  op2(SI, DX), 0 },
/*6f*/	{ "outs",  FALSE, DISASM_LONG,  op2(SI, DX), 0 },

/*70*/	{ "jo",    FALSE, DISASM_NONE,  op1(Db),     0 },
/*71*/	{ "jno",   FALSE, DISASM_NONE,  op1(Db),     0 },
/*72*/	{ "jb",    FALSE, DISASM_NONE,  op1(Db),     0 },
/*73*/	{ "jnb",   FALSE, DISASM_NONE,  op1(Db),     0 },
/*74*/	{ "jz",    FALSE, DISASM_NONE,  op1(Db),     0 },
/*75*/	{ "jnz",   FALSE, DISASM_NONE,  op1(Db),     0 },
/*76*/	{ "jbe",   FALSE, DISASM_NONE,  op1(Db),     0 },
/*77*/	{ "jnbe",  FALSE, DISASM_NONE,  op1(Db),     0 },

/*78*/	{ "js",    FALSE, DISASM_NONE,  op1(Db),     0 },
/*79*/	{ "jns",   FALSE, DISASM_NONE,  op1(Db),     0 },
/*7a*/	{ "jp",    FALSE, DISASM_NONE,  op1(Db),     0 },
/*7b*/	{ "jnp",   FALSE, DISASM_NONE,  op1(Db),     0 },
/*7c*/	{ "jl",    FALSE, DISASM_NONE,  op1(Db),     0 },
/*7d*/	{ "jnl",   FALSE, DISASM_NONE,  op1(Db),     0 },
/*7e*/	{ "jle",   FALSE, DISASM_NONE,  op1(Db),     0 },
/*7f*/	{ "jnle",  FALSE, DISASM_NONE,  op1(Db),     0 },

/*80*/  { "",	   TRUE,  DISASM_BYTE,  op2(I, E),   (char *)db_Grp1 },
/*81*/  { "",	   TRUE,  DISASM_LONG,  op2(I, E),   (char *)db_Grp1 },
/*82*/  { "",	   TRUE,  DISASM_BYTE,  op2(Is,E),   (char *)db_Grp1 },
/*83*/  { "",	   TRUE,  DISASM_LONG,  op2(Ibs,E),  (char *)db_Grp1 },
/*84*/	{ "test",  TRUE,  DISASM_BYTE,  op2(R, E),   0 },
/*85*/	{ "test",  TRUE,  DISASM_LONG,  op2(R, E),   0 },
/*86*/	{ "xchg",  TRUE,  DISASM_BYTE,  op2(R, E),   0 },
/*87*/	{ "xchg",  TRUE,  DISASM_LONG,  op2(R, E),   0 },

/*88*/	{ "mov",   TRUE,  DISASM_BYTE,  op2(R, E),   0 },
/*89*/	{ "mov",   TRUE,  DISASM_LONG,  op2(R, E),   0 },
/*8a*/	{ "mov",   TRUE,  DISASM_BYTE,  op2(E, R),   0 },
/*8b*/	{ "mov",   TRUE,  DISASM_LONG,  op2(E, R),   0 },
/*8c*/  { "mov",   TRUE,  DISASM_NONE,  op2(S, Ew),  0 },
/*8d*/	{ "lea",   TRUE,  DISASM_LONG,  op2(E, R),   0 },
/*8e*/	{ "mov",   TRUE,  DISASM_NONE,  op2(Ew, S),  0 },
/*8f*/	{ "pop",   TRUE,  DISASM_LONG,  op1(E),      0 },

/*90*/	{ "nop",   FALSE, DISASM_NONE,  0,	      0 },
/*91*/	{ "xchg",  FALSE, DISASM_LONG,  op2(A, Ri),  0 },
/*92*/	{ "xchg",  FALSE, DISASM_LONG,  op2(A, Ri),  0 },
/*93*/	{ "xchg",  FALSE, DISASM_LONG,  op2(A, Ri),  0 },
/*94*/	{ "xchg",  FALSE, DISASM_LONG,  op2(A, Ri),  0 },
/*95*/	{ "xchg",  FALSE, DISASM_LONG,  op2(A, Ri),  0 },
/*96*/	{ "xchg",  FALSE, DISASM_LONG,  op2(A, Ri),  0 },
/*97*/	{ "xchg",  FALSE, DISASM_LONG,  op2(A, Ri),  0 },

/*98*/	{ "cbw",   FALSE, DISASM_SDEP,  0,	      "cwde" },	/* cbw/cwde */
/*99*/	{ "cwd",   FALSE, DISASM_SDEP,  0,	      "cdq"  },	/* cwd/cdq */
/*9a*/	{ "lcall", FALSE, DISASM_NONE,  op1(OS),     0 },
/*9b*/	{ "wait",  FALSE, DISASM_NONE,  0,	      0 },
/*9c*/	{ "pushf", FALSE, DISASM_LONG,  0,	      0 },
/*9d*/	{ "popf",  FALSE, DISASM_LONG,  0,	      0 },
/*9e*/	{ "sahf",  FALSE, DISASM_NONE,  0,	      0 },
/*9f*/	{ "lahf",  FALSE, DISASM_NONE,  0,	      0 },

/*a0*/	{ "mov",   FALSE, DISASM_BYTE,  op2(O, A),   0 },
/*a1*/	{ "mov",   FALSE, DISASM_LONG,  op2(O, A),   0 },
/*a2*/	{ "mov",   FALSE, DISASM_BYTE,  op2(A, O),   0 },
/*a3*/	{ "mov",   FALSE, DISASM_LONG,  op2(A, O),   0 },
/*a4*/	{ "movs",  FALSE, DISASM_BYTE,  op2(SI,DI),  0 },
/*a5*/	{ "movs",  FALSE, DISASM_LONG,  op2(SI,DI),  0 },
/*a6*/	{ "cmps",  FALSE, DISASM_BYTE,  op2(SI,DI),  0 },
/*a7*/	{ "cmps",  FALSE, DISASM_LONG,  op2(SI,DI),  0 },

/*a8*/	{ "test",  FALSE, DISASM_BYTE,  op2(I, A),   0 },
/*a9*/	{ "test",  FALSE, DISASM_LONG,  op2(I, A),   0 },
/*aa*/	{ "stos",  FALSE, DISASM_BYTE,  op1(DI),     0 },
/*ab*/	{ "stos",  FALSE, DISASM_LONG,  op1(DI),     0 },
/*ac*/	{ "lods",  FALSE, DISASM_BYTE,  op1(SI),     0 },
/*ad*/	{ "lods",  FALSE, DISASM_LONG,  op1(SI),     0 },
/*ae*/	{ "scas",  FALSE, DISASM_BYTE,  op1(SI),     0 },
/*af*/	{ "scas",  FALSE, DISASM_LONG,  op1(SI),     0 },

/*b0*/	{ "mov",   FALSE, DISASM_BYTE,  op2(I, Ri),  0 },
/*b1*/	{ "mov",   FALSE, DISASM_BYTE,  op2(I, Ri),  0 },
/*b2*/	{ "mov",   FALSE, DISASM_BYTE,  op2(I, Ri),  0 },
/*b3*/	{ "mov",   FALSE, DISASM_BYTE,  op2(I, Ri),  0 },
/*b4*/	{ "mov",   FALSE, DISASM_BYTE,  op2(I, Ri),  0 },
/*b5*/	{ "mov",   FALSE, DISASM_BYTE,  op2(I, Ri),  0 },
/*b6*/	{ "mov",   FALSE, DISASM_BYTE,  op2(I, Ri),  0 },
/*b7*/	{ "mov",   FALSE, DISASM_BYTE,  op2(I, Ri),  0 },

/*b8*/	{ "mov",   FALSE, DISASM_LONG,  op2(I, Ri),  0 },
/*b9*/	{ "mov",   FALSE, DISASM_LONG,  op2(I, Ri),  0 },
/*ba*/	{ "mov",   FALSE, DISASM_LONG,  op2(I, Ri),  0 },
/*bb*/	{ "mov",   FALSE, DISASM_LONG,  op2(I, Ri),  0 },
/*bc*/	{ "mov",   FALSE, DISASM_LONG,  op2(I, Ri),  0 },
/*bd*/	{ "mov",   FALSE, DISASM_LONG,  op2(I, Ri),  0 },
/*be*/	{ "mov",   FALSE, DISASM_LONG,  op2(I, Ri),  0 },
/*bf*/	{ "mov",   FALSE, DISASM_LONG,  op2(I, Ri),  0 },

/*c0*/	{ "",	   TRUE,  DISASM_BYTE,  op2(Ib, E),  (char *)db_Grp2 },
/*c1*/	{ "",	   TRUE,  DISASM_LONG,  op2(Ib, E),  (char *)db_Grp2 },
/*c2*/	{ "ret",   FALSE, DISASM_NONE,  op1(Iw),     0 },
/*c3*/	{ "ret",   FALSE, DISASM_NONE,  0,	      0 },
/*c4*/	{ "les",   TRUE,  DISASM_LONG,  op2(E, R),   0 },
/*c5*/	{ "lds",   TRUE,  DISASM_LONG,  op2(E, R),   0 },
/*c6*/	{ "mov",   TRUE,  DISASM_BYTE,  op2(I, E),   0 },
/*c7*/	{ "mov",   TRUE,  DISASM_LONG,  op2(I, E),   0 },

/*c8*/	{ "enter", FALSE, DISASM_NONE,  op2(Ib, Iw), 0 },
/*c9*/	{ "leave", FALSE, DISASM_NONE,  0,           0 },
/*ca*/	{ "lret",  FALSE, DISASM_NONE,  op1(Iw),     0 },
/*cb*/	{ "lret",  FALSE, DISASM_NONE,  0,	      0 },
/*cc*/	{ "int",   FALSE, DISASM_NONE,  op1(o3),     0 },
/*cd*/	{ "int",   FALSE, DISASM_NONE,  op1(Ib),     0 },
/*ce*/	{ "into",  FALSE, DISASM_NONE,  0,	      0 },
/*cf*/	{ "iret",  FALSE, DISASM_NONE,  0,	      0 },

/*d0*/	{ "",	   TRUE,  DISASM_BYTE,  op2(o1, E),  (char *)db_Grp2 },
/*d1*/	{ "",	   TRUE,  DISASM_LONG,  op2(o1, E),  (char *)db_Grp2 },
/*d2*/	{ "",	   TRUE,  DISASM_BYTE,  op2(CL, E),  (char *)db_Grp2 },
/*d3*/	{ "",	   TRUE,  DISASM_LONG,  op2(CL, E),  (char *)db_Grp2 },
/*d4*/	{ "aam",   TRUE,  DISASM_NONE,  0,	      0 },
/*d5*/	{ "aad",   TRUE,  DISASM_NONE,  0,	      0 },
/*d6*/	{ "",      FALSE, DISASM_NONE,  0,	      0 },
/*d7*/	{ "xlat",  FALSE, DISASM_BYTE,  op1(BX),     0 },

/*d8*/  { "",      TRUE,  DISASM_NONE,  0,	      (char *)db_Esc8 },
/*d9*/  { "",      TRUE,  DISASM_NONE,  0,	      (char *)db_Esc9 },
/*da*/  { "",      TRUE,  DISASM_NONE,  0,	      (char *)db_Esca },
/*db*/  { "",      TRUE,  DISASM_NONE,  0,	      (char *)db_Escb },
/*dc*/  { "",      TRUE,  DISASM_NONE,  0,	      (char *)db_Escc },
/*dd*/  { "",      TRUE,  DISASM_NONE,  0,	      (char *)db_Escd },
/*de*/  { "",      TRUE,  DISASM_NONE,  0,	      (char *)db_Esce },
/*df*/  { "",      TRUE,  DISASM_NONE,  0,	      (char *)db_Escf },

/*e0*/	{ "loopne",FALSE, DISASM_NONE,  op1(Db),     0 },
/*e1*/	{ "loope", FALSE, DISASM_NONE,  op1(Db),     0 },
/*e2*/	{ "loop",  FALSE, DISASM_NONE,  op1(Db),     0 },
/*e3*/	{ "jcxz",  FALSE, DISASM_SDEP,  op1(Db),     "jecxz" },
/*e4*/	{ "in",    FALSE, DISASM_BYTE,  op2(Ib, A),  0 },
/*e5*/	{ "in",    FALSE, DISASM_LONG,  op2(Ib, A) , 0 },
/*e6*/	{ "out",   FALSE, DISASM_BYTE,  op2(A, Ib),  0 },
/*e7*/	{ "out",   FALSE, DISASM_LONG,  op2(A, Ib) , 0 },

/*e8*/	{ "call",  FALSE, DISASM_NONE,  op1(Dl),     0 },
/*e9*/	{ "jmp",   FALSE, DISASM_NONE,  op1(Dl),     0 },
/*ea*/	{ "ljmp",  FALSE, DISASM_NONE,  op1(OS),     0 },
/*eb*/	{ "jmp",   FALSE, DISASM_NONE,  op1(Db),     0 },
/*ec*/	{ "in",    FALSE, DISASM_BYTE,  op2(DX, A),  0 },
/*ed*/	{ "in",    FALSE, DISASM_LONG,  op2(DX, A) , 0 },
/*ee*/	{ "out",   FALSE, DISASM_BYTE,  op2(A, DX),  0 },
/*ef*/	{ "out",   FALSE, DISASM_LONG,  op2(A, DX) , 0 },

/*f0*/	{ "",      FALSE, DISASM_NONE,  0,	     0 },
/*f1*/	{ "",      FALSE, DISASM_NONE,  0,	     0 },
/*f2*/	{ "",      FALSE, DISASM_NONE,  0,	     0 },
/*f3*/	{ "",      FALSE, DISASM_NONE,  0,	     0 },
/*f4*/	{ "hlt",   FALSE, DISASM_NONE,  0,	     0 },
/*f5*/	{ "cmc",   FALSE, DISASM_NONE,  0,	     0 },
/*f6*/	{ "",      TRUE,  DISASM_BYTE,  0,	     (char *)db_Grp3 },
/*f7*/	{ "",	   TRUE,  DISASM_LONG,  0,	     (char *)db_Grp3 },

/*f8*/	{ "clc",   FALSE, DISASM_NONE,  0,	     0 },
/*f9*/	{ "stc",   FALSE, DISASM_NONE,  0,	     0 },
/*fa*/	{ "cli",   FALSE, DISASM_NONE,  0,	     0 },
/*fb*/	{ "sti",   FALSE, DISASM_NONE,  0,	     0 },
/*fc*/	{ "cld",   FALSE, DISASM_NONE,  0,	     0 },
/*fd*/	{ "std",   FALSE, DISASM_NONE,  0,	     0 },
/*fe*/	{ "",	   TRUE,  DISASM_NONE,  0,	     (char *)db_Grp4 },
/*ff*/	{ "",	   TRUE,  DISASM_NONE,  0,	     (char *)db_Grp5 },
};

const struct inst	db_bad_inst =
	{ "???",   FALSE, DISASM_NONE,  0,	      0 }
;

#define	f_mod(byte)	((byte)>>6)
#define	f_reg(byte)	(((byte)>>3)&0x7)
#define	f_rm(byte)	((byte)&0x7)

#define	sib_ss(byte)	((byte)>>6)
#define	sib_index(byte)	(((byte)>>3)&0x7)
#define	sib_base(byte)	((byte)&0x7)

struct i_addr {
	int		is_reg;	/* if reg, reg number is in 'disp' */
	int		disp;
	const char *	base;
	const char *	index;
	int		ss;
};

const char * const db_index_reg_16[8] = {
	"%bx,%si",
	"%bx,%di",
	"%bp,%si",
	"%bp,%di",
	"%si",
	"%di",
	"%bp",
	"%bx"
};

const char * const db_reg[3][8] = {
	{ "%al",  "%cl",  "%dl",  "%bl",  "%ah",  "%ch",  "%dh",  "%bh" },
	{ "%ax",  "%cx",  "%dx",  "%bx",  "%sp",  "%bp",  "%si",  "%di" },
	{ "%eax", "%ecx", "%edx", "%ebx", "%esp", "%ebp", "%esi", "%edi" }
};

const char * const db_seg_reg[8] = {
	"%es", "%cs", "%ss", "%ds", "%fs", "%gs", "", ""
};

/*
 * lengths for size attributes
 */
const int db_lengths[] = {
	1,	/* DISASM_BYTE */
	2,	/* DISASM_WORD */
	4,	/* DISASM_LONG */
	8,	/* DISASM_QUAD */
	4,	/* DISASM_SNGL */
	8,	/* DISASM_DBLR */
	10,	/* DISASM_EXTR */
};

#define	get_value_inc(result, cs, loc, size, is_signed) \
	do { \
		result = db_get_value((cs), (loc), (size), (is_signed)); \
		(loc) += (size); \
	} while (0)


static db_addr_t db_read_address(WORD, db_addr_t, int, int, struct i_addr *);
static void db_print_address(char *, int, struct i_addr *);
static db_addr_t db_disasm_esc(WORD cs, db_addr_t, int, int, int, char *);

/*
 * Read address at location and return updated location.
 */
static db_addr_t
db_read_address(cs, loc, short_addr, regmodrm, addrp)
	WORD		cs;
	db_addr_t	loc;
	int		short_addr;
	int		regmodrm;
	struct i_addr	*addrp;		/* out */
{
	int		mod, rm, sib, index, disp;

	mod = f_mod(regmodrm);
	rm  = f_rm(regmodrm);

	if (mod == 3) {
		addrp->is_reg = TRUE;
		addrp->disp = rm;
		return (loc);
	}
	addrp->is_reg = FALSE;
	addrp->index = 0;

	if (short_addr) {
		addrp->index = 0;
		addrp->ss = 0;
		switch (mod) {
		    case 0:
			if (rm == 6) {
				get_value_inc(disp, cs, loc, 2, TRUE);
				addrp->disp = disp;
				addrp->base = 0;
			} else {
				addrp->disp = 0;
				addrp->base = db_index_reg_16[rm];
			}
			break;
		    case 1:
			get_value_inc(disp, cs, loc, 1, TRUE);
			addrp->disp = disp;
			addrp->base = db_index_reg_16[rm];
			break;
		    case 2:
			get_value_inc(disp, cs, loc, 2, TRUE);
			addrp->disp = disp;
			addrp->base = db_index_reg_16[rm];
			break;
		}
	} else {
		if (mod != 3 && rm == 4) {
			get_value_inc(sib, cs, loc, 1, FALSE);
			rm = sib_base(sib);
			index = sib_index(sib);
			if (index != 4)
				addrp->index = db_reg[DISASM_LONG][index];
			addrp->ss = sib_ss(sib);
		}

		switch (mod) {
		    case 0:
			if (rm == 5) {
				get_value_inc(addrp->disp, cs, loc, 4, FALSE);
				addrp->base = 0;
			} else {
				addrp->disp = 0;
				addrp->base = db_reg[DISASM_LONG][rm];
			}
			break;
		    case 1:
			get_value_inc(disp, cs, loc, 1, TRUE);
			addrp->disp = disp;
			addrp->base = db_reg[DISASM_LONG][rm];
			break;
		    case 2:
			get_value_inc(disp, cs, loc, 4, FALSE);
			addrp->disp = disp;
			addrp->base = db_reg[DISASM_LONG][rm];
			break;
		}
	}
	return (loc);
}

static void
db_print_address(seg, size, addrp)
	char *		seg;
	int		size;
	struct i_addr	*addrp;
{
	if (addrp->is_reg) {
		db_printf("%s", db_reg[size][addrp->disp]);
		return;
	}

	if (seg)
		db_printf("%s:", seg);

	db_printsym((db_addr_t)addrp->disp, DB_STGY_ANY, db_printf);
	if (addrp->base != 0 || addrp->index != 0) {
		db_printf("(");
		if (addrp->base)
			db_printf("%s", addrp->base);
		if (addrp->index)
			db_printf(", %s, %d", addrp->index, 1<<addrp->ss);
		db_printf(")");
	}
}

/*
 * Disassemble floating-point ("escape") instruction
 * and return updated location.
 */
static db_addr_t
db_disasm_esc(cs, loc, inst, short_addr, size, seg)
	WORD		cs;
	db_addr_t	loc;
	int		inst;
	int		short_addr;
	int		size;
	char *		seg;
{
	int		regmodrm;
	const struct finst	*fp;
	int		mod;
	struct i_addr	address;
	char *		name;

	UNUSED(size);

	get_value_inc(regmodrm, cs, loc, 1, FALSE);
	fp = &db_Esc_inst[inst - 0xd8][f_reg(regmodrm)];
	mod = f_mod(regmodrm);
	if (mod != 3) {
		/*
		 * Normal address modes.
		 */
		loc = db_read_address(cs, loc, short_addr, regmodrm, &address);
		db_printf("%s", fp->f_name);
		switch(fp->f_size) {
		    case DISASM_SNGL:
			db_printf("s");
			break;
		    case DISASM_DBLR:
			db_printf("l");
			break;
		    case DISASM_EXTR:
			db_printf("t");
			break;
		    case DISASM_WORD:
			db_printf("s");
			break;
		    case DISASM_LONG:
			db_printf("l");
			break;
		    case DISASM_QUAD:
			db_printf("q");
			break;
		    default:
			break;
		}
		db_printf("\t");
		db_print_address(seg, DISASM_BYTE, &address);
	} else {
		/*
		 * 'reg-reg' - special formats
		 */
		switch (fp->f_rrmode) {
		    case op2(ST,STI):
			name = (fp->f_rrname) ? fp->f_rrname : fp->f_name;
			db_printf("%s\t%%st, %%st(%d)",name,f_rm(regmodrm));
			break;
		    case op2(STI,ST):
			name = (fp->f_rrname) ? fp->f_rrname : fp->f_name;
			db_printf("%s\t%%st(%d), %%st",name, f_rm(regmodrm));
			break;
		    case op1(STI):
			name = (fp->f_rrname) ? fp->f_rrname : fp->f_name;
			db_printf("%s\t%%st(%d)",name, f_rm(regmodrm));
			break;
		    case op1(X):
			db_printf("%s", ((char **)fp->f_rrname)[f_rm(regmodrm)]);
			break;
		    case op1(XA):
			db_printf("%s\t%%ax",
				  ((char **)fp->f_rrname)[f_rm(regmodrm)]);
			break;
		    default:
			db_printf("<bad instruction>");
			break;
		}
	}

	return (loc);
}

/*
 * Disassemble instruction at 'loc'.  'altfmt' specifies an
 * (optional) alternate format.  Return address of start of
 * next instruction.
 */
static db_addr_t
db_disasm(cs, loc, altfmt)
	WORD		cs;
	db_addr_t	loc;
	boolean_t	altfmt;
{
	int	inst;
	int	size;
	int	short_addr;
	char *	seg;
	const struct inst *	ip;
	char *	i_name;
	int	i_size;
	int	i_mode;
	int	regmodrm = 0;
	boolean_t	first;
	int	displ;
	int	prefix;
	int	imm;
	int	imm2;
	int	len;
	struct i_addr	address;

	UNUSED(altfmt);

	get_value_inc(inst, cs, loc, 1, FALSE);
	short_addr = CPU_INST_OP32 ? FALSE : TRUE;
	size = short_addr ? DISASM_WORD : DISASM_LONG;
	seg = 0;

	/*
	 * Get prefixes
	 */
	prefix = TRUE;
	do {
		switch (inst) {
		    case 0x66:		/* data16 */
			size = short_addr ? DISASM_LONG : DISASM_WORD;
			break;
		    case 0x67:
			short_addr = CPU_INST_OP32 ? TRUE : FALSE;
			break;
		    case 0x26:
			seg = "%es";
			break;
		    case 0x36:
			seg = "%ss";
			break;
		    case 0x2e:
			seg = "%cs";
			break;
		    case 0x3e:
			seg = "%ds";
			break;
		    case 0x64:
			seg = "%fs";
			break;
		    case 0x65:
			seg = "%gs";
			break;
		    case 0xf0:
			db_printf("lock ");
			break;
		    case 0xf2:
			db_printf("repne ");
			break;
		    case 0xf3:
			db_printf("repe ");	/* XXX repe VS rep */
			break;
		    default:
			prefix = FALSE;
			break;
		}
		if (prefix)
			get_value_inc(inst, cs, loc, 1, FALSE);
	} while (prefix);

	if (inst >= 0xd8 && inst <= 0xdf) {
		loc = db_disasm_esc(cs, loc, inst, short_addr, size, seg);
		db_printf("\n");
		return (loc);
	}

	if (inst == 0x0f) {
		get_value_inc(inst, cs, loc, 1, FALSE);
		ip = db_inst_0f[inst>>4];
		if (ip == 0)
			ip = &db_bad_inst;
		else
			ip = &ip[inst&0xf];
	} else {
		ip = &db_inst_table[inst];
	}

	if (ip->i_has_modrm) {
		get_value_inc(regmodrm, cs, loc, 1, FALSE);
		loc = db_read_address(cs, loc, short_addr, regmodrm, &address);
	}

	i_name = ip->i_name;
	i_size = ip->i_size;
	i_mode = ip->i_mode;

	if (ip->i_extra == (char *)db_Grp1 ||
	    ip->i_extra == (char *)db_Grp2 ||
	    ip->i_extra == (char *)db_Grp6 ||
	    ip->i_extra == (char *)db_Grp7 ||
	    ip->i_extra == (char *)db_Grp8) {
		i_name = ((char **)ip->i_extra)[f_reg(regmodrm)];
	} else if (ip->i_extra == (char *)db_Grp3) {
		ip = (struct inst *)ip->i_extra;
		ip = &ip[f_reg(regmodrm)];
		i_name = ip->i_name;
		i_mode = ip->i_mode;
	} else if (ip->i_extra == (char *)db_Grp4 ||
		   ip->i_extra == (char *)db_Grp5) {
		ip = (struct inst *)ip->i_extra;
		ip = &ip[f_reg(regmodrm)];
		i_name = ip->i_name;
		i_mode = ip->i_mode;
		i_size = ip->i_size;
	}

	if (i_size == DISASM_SDEP) {
		if (size == DISASM_WORD)
			db_printf("%s", i_name);
		else
			db_printf("%s", ip->i_extra);
	} else {
		db_printf("%s", i_name);
		if (i_size != DISASM_NONE) {
			if (i_size == DISASM_BYTE) {
				db_printf("b");
				size = DISASM_BYTE;
			} else if (i_size == DISASM_WORD) {
				db_printf("w");
				size = DISASM_WORD;
			} else if (size == DISASM_WORD) {
				db_printf("w");
			} else {
				db_printf("l");
			}
		}
	}
	db_printf("\t");
	for (first = TRUE;
	     i_mode != 0;
	     i_mode >>= 8, first = FALSE) {
		char tbuf[24];

		if (!first)
			db_printf(", ");

		switch (i_mode & 0xFF) {
		    case E:
			db_print_address(seg, size, &address);
			break;
		    case Eind:
			db_printf("*");
			db_print_address(seg, size, &address);
			break;
		    case Ew:
			db_print_address(seg, DISASM_WORD, &address);
			break;
		    case Eb:
			db_print_address(seg, DISASM_BYTE, &address);
			break;
		    case R:
			db_printf("%s", db_reg[size][f_reg(regmodrm)]);
			break;
		    case Rw:
			db_printf("%s", db_reg[DISASM_WORD][f_reg(regmodrm)]);
			break;
		    case Ri:
			db_printf("%s", db_reg[size][f_rm(inst)]);
			break;
		    case S:
			db_printf("%s", db_seg_reg[f_reg(regmodrm)]);
			break;
		    case Si:
			db_printf("%s", db_seg_reg[f_reg(inst)]);
			break;
		    case A:
			db_printf("%s", db_reg[size][0]);	/* acc */
			break;
		    case BX:
			if (seg)
				db_printf("%s:", seg);
			db_printf("(%s)", short_addr ? "%bx" : "%ebx");
			break;
		    case CL:
			db_printf("%%cl");
			break;
		    case DX:
			db_printf("%%dx");
			break;
		    case SI:
			if (seg)
				db_printf("%s:", seg);
			db_printf("(%s)", short_addr ? "%si" : "%esi");
			break;
		    case DI:
			db_printf("%%es:(%s)", short_addr ? "%di" : "%edi");
			break;
		    case CR:
			db_printf("%%cr%d", f_reg(regmodrm));
			break;
		    case DR:
			db_printf("%%dr%d", f_reg(regmodrm));
			break;
		    case TR:
			db_printf("%%tr%d", f_reg(regmodrm));
			break;
		    case I:
			len = db_lengths[size];
			get_value_inc(imm, cs, loc, len, FALSE);/* unsigned */
			db_format_radix(tbuf, 24, (unsigned int)imm, TRUE);
			db_printf("$%s", tbuf);
			break;
		    case Is:
			len = db_lengths[size];
			get_value_inc(imm, cs, loc, len, TRUE);	/* signed */
			db_format_radix(tbuf, 24, imm, TRUE);
			db_printf("$%s", tbuf);
			break;
		    case Ib:
			get_value_inc(imm, cs, loc, 1, FALSE);	/* unsigned */
			db_format_radix(tbuf, 24, (unsigned int)imm, TRUE);
			db_printf("$%s", tbuf);
			break;
		    case Ibs:
			get_value_inc(imm, cs, loc, 1, TRUE);	/* signed */
			db_format_radix(tbuf, 24, imm, TRUE);
			db_printf("$%s", tbuf);
			break;
		    case Iw:
			get_value_inc(imm, cs, loc, 2, FALSE);	/* unsigned */
			db_format_radix(tbuf, 24, (unsigned int)imm, TRUE);
			db_printf("$%s", tbuf);
			break;
		    case Il:
			get_value_inc(imm, cs, loc, 4, FALSE);
			db_format_radix(tbuf, 24, (unsigned int)imm, TRUE);
			db_printf("$%s", tbuf);
			break;
		    case O:
			if (short_addr)
				get_value_inc(displ, cs, loc, 2, TRUE);
			else
				get_value_inc(displ, cs, loc, 4, TRUE);
			if (seg) {
				db_format_radix(tbuf, 24, displ, TRUE);
				db_printf("%s:%s", seg, tbuf);
			} else
				db_printsym((db_addr_t)displ, DB_STGY_ANY,
				    db_printf);
			break;
		    case Db:
			get_value_inc(displ, cs, loc, 1, TRUE);
			db_printsym((db_addr_t)(displ + loc), DB_STGY_XTRN,
			    db_printf);
			break;
		    case Dl:
			get_value_inc(displ, cs, loc, 4, TRUE);
			db_printsym((db_addr_t)(displ + loc), DB_STGY_XTRN,
			    db_printf);
			break;
		    case o1:
			db_printf("$1");
			break;
		    case o3:
			db_printf("$3");
			break;
		    case OS:
			get_value_inc(imm, cs, loc, 4, FALSE);	/* offset */
			db_format_radix(tbuf, 24, (unsigned int)imm, TRUE);
			db_printf("$%s", tbuf);
			get_value_inc(imm2, cs, loc, 2, FALSE);	/* segment */
			db_format_radix(tbuf, 24, (unsigned int)imm2, TRUE);
			db_printf(",%s", tbuf);
			break;
		}
	}

	db_printf("\n");
	return (loc);
}

void
disasm(WORD cs, DWORD loc)
{

	db_disasm(cs, loc, FALSE);
}

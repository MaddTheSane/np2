/*	$Id: cpu.c,v 1.10 2004/02/05 16:43:44 monaka Exp $	*/

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

#include "compiler.h"
#include "cpu.h"
#include "ia32.mcr"

#include "inst_table.h"


#if defined(IA32_INSTRUCTION_TRACE)
static FILE *fp = NULL;
BOOL cpu_inst_trace = FALSE;

static const char *opcode_1byte[2][256] = {
/* 16bit */
{
/*00*/	"addb",  "addw",  "addb",  "addw",  "addb",  "addw",  "push",  "pop",
	"orb",   "orw",   "orb",   "orw",   "orb",   "orw",   "push",  NULL,
/*10*/	"adcb",  "adcw",  "adcb",  "adcw",  "adcb",  "adcw",  "push",  "pop",
	"sbbb",  "sbbw",  "sbbb",  "sbbw",  "sbbb",  "sbbw",  "push",  "pop",
/*20*/	"andb",  "andw",  "andb",  "andw",  "andb",  "andw",  "es:",   "daa",
	"subb",  "subw",  "subb",  "subw",  "subb",  "subw",  "cs:",   "das",
/*30*/	"xorb",  "xorw",  "xorb",  "xorw",  "xorb",  "xorw",  "ss:",   "aaa",
	"cmpb",  "cmpw",  "cmpb",  "cmpw",  "cmpb",  "cmpw",  "ds:",   "aas",
/*40*/	"incw",  "incw",  "incw",  "incw",  "incw",  "incw",  "incw",  "incw",
	"decw",  "decw",  "decw",  "decw",  "decw",  "decw",  "decw",  "decw",
/*50*/	"push",  "push",  "push",  "push",  "push",  "push",  "push",  "push",
	"pop",   "pop",   "pop",   "pop",   "pop",   "pop",   "pop",   "pop",
/*60*/	"pusha", "popa",  "bound", "arpl",  "fs:",   "gs:",   NULL,    NULL,
	"push",  "imul",  "push",  "imul",  "insb",  "insw",  "outsb", "outsw",
/*70*/	"jo",    "jno",   "jc",    "jnc",   "jz",    "jnz",   "jna",   "ja",
	"js",    "jns",   "jp",    "jnp",   "jl",    "jnl",   "jle",   "jnle",
/*80*/	NULL,    NULL,    NULL,    NULL,    "testb", "testw", "xchgb", "xchgw",
	"movb",  "movw",  "movb",  "movw",  "movw",  "lea",   "movw",  "pop",
/*90*/	"nop",   "xchgw", "xchgw", "xchgw", "xchgw", "xchgw", "xchgw", "xchgw",
	"cbw",   "cwd",   "callf", "fwait", "pushf", "popf",  "sahf",  "lahf",
/*a0*/	"movb",  "movw",  "movb",  "movw",  "movsb", "movsw", "cmpsb", "cmpsw",
	"testb", "testw", "stosb", "stosw", "lodsb", "lodsw", "scasb", "scasw",
/*b0*/	"movb",  "movb",  "movb",  "movb",  "movb",  "movb",  "movb",  "movb",  
	"movw",  "movw",  "movw",  "movw",  "movw",  "movw",  "movw",  "movw",  
/*c0*/	NULL,    NULL,    "ret",   "ret",   "les",   "lds",   "movb",  "movw",
	"enter", "leave", "ret",   "ret",   "int3",  "int",   "into",  "iret",
/*d0*/	NULL,    NULL,    NULL,    NULL,    "aam",   "aad",   "salc",  "xlat",
	"esc0",  "esc1",  "esc2",  "esc3",  "esc4",  "esc5",  "esc6",  "esc7",
/*e0*/	"loopne","loope", "loop",  "jcxz",  "inb",   "inw",   "outb",  "outw",
	"call",  "jmp",   "jmpf",  "jmp",   "inb",   "inw",   "outb",  "outw",
/*f0*/	"lock:", "int1",  "repne", "repe",  "hlt",   "cmc",   NULL,    NULL,
	"clc",   "stc",   "cli",   "sti",   "cld",   "std",   NULL,    NULL,
},
/* 32bit */
{
/*00*/	"addb",  "addl",  "addb",  "addl",  "addb",  "addl",  "pushl", "popl",
	"orb",   "orl",   "orb",   "orl",   "orb",   "orl",   "pushl", NULL,
/*10*/	"adcb",  "adcl",  "adcb",  "adcl",  "adcb",  "adcl",  "pushl", "popl",
	"sbbb",  "sbbl",  "sbbb",  "sbbl",  "sbbb",  "sbbl",  "pushl", "popl",
/*20*/	"andb",  "andl",  "andb",  "andl",  "andb",  "andl",  "es:",   "daa",
	"subb",  "subl",  "subb",  "subl",  "subb",  "subl",  "cs:",   "das",
/*30*/	"xorb",  "xorl",  "xorb",  "xorl",  "xorb",  "xorl",  "ss:",   "aaa",
	"cmpb",  "cmpl",  "cmpb",  "cmpl",  "cmpb",  "cmpl",  "ds:",   "aas",
/*40*/	"incl",  "incl",  "incl",  "incl",  "incl",  "incl",  "incl",  "incl",
	"decl",  "decl",  "decl",  "decl",  "decl",  "decl",  "decl",  "decl",
/*50*/	"pushl", "pushl", "pushl", "pushl", "pushl", "pushl", "pushl", "pushl",
	"popl",  "popl",  "popl",  "popl",  "popl",  "popl",  "popl",  "pop",
/*60*/	"pushad","popad", "bound", "arpl",  "fs:",   "gs:",   NULL,    NULL,
	"pushl", "imul",  "pushl", "imul",  "insb",  "insl",  "outsb", "outsl",
/*70*/	"jo",    "jno",   "jc",    "jnc",   "jz",    "jnz",   "jna",   "ja",
	"js",    "jns",   "jp",    "jnp",   "jl",    "jnl",   "jle",   "jnle",
/*80*/	NULL,    NULL,    NULL,    NULL,    "testb", "testl", "xchgb", "xchgl",
	"movb",  "movl",  "movb",  "movl",  "movl",  "lea",   "movl",  "popl",
/*90*/	"nop",   "xchgl", "xchgl", "xchgl", "xchgl", "xchgl", "xchgl", "xchgl",
	"cwde",  "cdq",   "callfl","fwait", "pushfd","popfd", "sahf",  "lahf",
/*a0*/	"movb",  "movl",  "movb",  "movl",  "movsb", "movsd", "cmpsb", "cmpsd",
	"testb", "testl", "stosb", "stosd", "lodsb", "lodsd", "scasb", "scasd",
/*b0*/	"movb",  "movb",  "movb",  "movb",  "movb",  "movb",  "movb",  "movb",  
	"movl",  "movl",  "movl",  "movl",  "movl",  "movl",  "movl",  "movl",  
/*c0*/	NULL,    NULL,    "ret",   "ret",   "les",   "lds",   "movb",  "movl",
	"enter", "leave", "ret",   "ret",   "int3",  "int",   "into",  "iretd",
/*d0*/	NULL,    NULL,    NULL,    NULL,    "aam",   "aad",   "salc",  "xlat",
	"esc0",  "esc1",  "esc2",  "esc3",  "esc4",  "esc5",  "esc6",  "esc7",
/*e0*/	"loopne","loope", "loop",  "jecxz", "inb",   "inl",   "outb",  "outl",
	"call",  "jmp",   "jmpf",  "jmp",   "inb",   "inl",   "outb",  "outl",
/*f0*/	"lock:", "int1",  "repne", "repe",  "hlt",   "cmc",   NULL,    NULL,
	"clc",   "stc",   "cli",   "sti",   "cld",   "std",   NULL,    NULL,
}
};

static const char *opcode_2byte[2][256] = {
/* 16bit */
{
/*00*/	NULL,      NULL,      "lar",     "lsl",
	NULL,      "loadall", "clts",    NULL,
	"invd",    "wbinvd",  NULL,      "UD2",
	NULL,      NULL,      NULL,      NULL,
/*10*/	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*20*/	"movl",    "movl",    "movl",    "movl",
	"movl",    NULL,      "movl",    NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*30*/	"wrmsr",   "rdtsc",   "rdmsr",   NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*40*/	"cmovo",   "cmovno",  "cmovc",   "cmovnc",
	"cmovz",   "cmovnz",  "cmovna",  "cmova",
	"cmovs",   "cmovns",  "cmovp",   "cmovnp",
	"cmovl",   "cmovnl",  "cmovle",  "cmovnle",
/*50*/	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*60*/	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*70*/	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*80*/	"jo",      "jno",     "jc",      "jnc",
	"jz",      "jnz",     "jna",     "ja",
	"js",      "jns",     "jp",      "jnp",
	"jl",      "jnl",     "jle",     "jnle",
/*90*/	"seto",    "setno",   "setc",    "setnc",
	"setz",    "setnz",   "setna",   "seta",
	"sets",    "setns",   "setp",    "setnp",
	"setl",    "setnl",   "setle",   "setnle",
/*a0*/	"push",    "pop",     "cpuid",   "bt",
	"shldb",   "shldw",   "cmpxchgb","cmpxchgw",
	"push",    "pop",     "rsm",     "bts",
	"shrdb",   "shrdw",   NULL,      "imul",
/*b0*/	"cmpxchgb","cmpxchgw","lss",     "btr",
	"lfs",     "lgs",     "movzb",   "movzw",
	NULL,      "UD2",     NULL,      "btc",
	"bsf",     "bsr",     "movsb",   "movsw",
/*c0*/	"xaddb",   "xaddw",   NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	"bswap",   "bswap",   "bswap",   "bswap",
	"bswap",   "bswap",   "bswap",   "bswap",
/*d0*/	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*e0*/	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*f0*/	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
},
/* 32bit */
{
/*00*/	NULL,      NULL,      "lar",     "lsl",
	NULL,      "loadall", "clts",    NULL,
	"invd",    "wbinvd",  NULL,      "UD2",
	NULL,      NULL,      NULL,      NULL,
/*10*/	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*20*/	"movl",    "movl",    "movl",    "movl",
	"movl",    NULL,      "movl",    NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*30*/	"wrmsr",   "rdtsc",   "rdmsr",   NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*40*/	"cmovo",   "cmovno",  "cmovc",   "cmovnc",
	"cmovz",   "cmovnz",  "cmovna",  "cmova",
	"cmovs",   "cmovns",  "cmovp",   "cmovnp",
	"cmovl",   "cmovnl",  "cmovle",  "cmovnle",
/*50*/	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*60*/	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*70*/	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*80*/	"jo",      "jno",     "jc",      "jnc",
	"jz",      "jnz",     "jna",     "ja",
	"js",      "jns",     "jp",      "jnp",
	"jl",      "jnl",     "jle",     "jnle",
/*90*/	"seto",    "setno",   "setc",    "setnc",
	"setz",    "setnz",   "setna",   "seta",
	"sets",    "setns",   "setp",    "setnp",
	"setl",    "setnl",   "setle",   "setnle",
/*a0*/	"push",    "pop",     "cpuid",   "bt",
	"shldb",   "shldl",   "cmpxchgb","cmpxchgl",
	"push",    "pop",     "rsm",     "bts",
	"shrdb",   "shrdl",   NULL,      "imul",
/*b0*/	"cmpxchgb","cmpxchgd","lss",     "btr",
	"lfs",     "lgs",     "movzbl",  "movzwl",
	NULL,      "UD2",     NULL,      "btc",
	"bsf",     "bsr",     "movsbl",  "movswl",
/*c0*/	"xaddb",   "xaddl",   NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	"bswapl",  "bswapl",  "bswapl",  "bswapl",
	"bswapl",  "bswapl",  "bswapl",  "bswapl",
/*d0*/	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*e0*/	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
/*f0*/	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
	NULL,      NULL,      NULL,      NULL,
}
};

static const char *opcode_0x8x[2][2][8] = {
/* 16bit */
{
	{ "addb", "orb", "adcb", "sbbb", "andb", "subb", "xorb", "cmpb" },
	{ "addw", "orw", "adcw", "sbbw", "andw", "subw", "xorw", "cmpw" }
},
/* 32bit */
{
	{ "addb", "orb", "adcb", "sbbb", "andb", "subb", "xorb", "cmpb" },
	{ "addl", "orl", "adcl", "sbbl", "andl", "subl", "xorl", "cmpl" }
}
};

static const char *opcode_shift[2][2][8] = {
/* 16bit */
{
	{ "rolb", "rorb", "rclb", "rcrb", "shlb", "shrb", "shlb", "sarb" },
	{ "rolw", "rorw", "rclw", "rcrw", "shlw", "shrw", "shlw", "sarw" }
},
/* 32bit */
{
	{ "rolb", "rorb", "rclb", "rcrb", "shlb", "shrb", "shlb", "sarb" },
	{ "roll", "rorl", "rcll", "rcrl", "shll", "shrl", "shll", "sarl" }
},
};

static const char *opcode_0xf6[2][2][8] = {
/* 16bit */
{
	{ "testb", "testb", "notb", "negb", "mul", "imul", "div", "idiv" },
	{ "testw", "testw", "notw", "negw", "mulw", "imulw", "divw", "idivw" }
},
/* 32bit */
{
	{ "testb", "testb", "notb", "negb", "mul", "imul", "div", "idiv" },
	{ "testl", "testl", "notl", "negl", "mull", "imull", "divl", "idivl" }
},
};

static const char *opcode_0xfe[2][2][8] = {
/* 16bit */
{
	{ "incb", "decb", NULL, NULL, NULL, NULL, NULL, NULL },
	{ "incw", "decw", "call", "callf", "jmp", "jmpf", "push", NULL }
},
/* 32bit */
{
	{ "incb", "decb", NULL, NULL, NULL, NULL, NULL, NULL },
	{ "incl", "decl", "call", "callf", "jmp", "jmpf", "pushl", NULL }
}
};

static const char *opcode2_g6[8] = {
	"sldt", "str", "lldt", "ltr", "verr", "verw", NULL, NULL
};

static const char *opcode2_g7[8] = {
	"sgdt", "sidt", "lgdt", "lidt", "smsw", NULL, "lmsw", "invlpg"
};

static const char *opcode2_g8[8] = {
	NULL, NULL, NULL, NULL, "bt", "bts", "btr", "btc"
};

static const char *opcode2_g9[8] = {
	NULL, "cmpxchg8b", NULL, NULL, NULL, NULL, NULL, NULL
};


static DWORD
ea(DWORD eip, DWORD op)
{
	static const char *ea16[8] = {
		"bx + si", "bx + di", "bp + si", "bp + di",
		"si", "di", "bp", "bx"
	};

	char buf[256];
	DWORD mod = (op >> 6) & 3;
	DWORD rm = op & 7;
	DWORD val;

	__ASSERT(mod != 3);

	buf[0] = '\0';

	if (!CPU_INST_AS32) {
		if (mod == 0) {
			if (rm == 6) {
				/* disp16 */
				val = cpu_codefetch_w(eip);
				eip += 2;
				SPRINTF(buf, "0x%04x", val);
			} else {
				SPRINTF(buf, "%s", ea16[rm]);
			}
		} else {
			if (mod == 1) {
				/* disp8 */
				val = cpu_codefetch(eip);
				if (val & 0x80) {
					val |= 0xff00;
				}
				eip++;
			} else {
				/* disp16 */
				val = cpu_codefetch_w(eip);
				eip += 2;
			}
			SPRINTF(buf, "%s + 0x%04x", ea16[rm], val);
		}
	} else {
		char tmp[32];
		BYTE count[9];
		int n;
		int i;

		memset(count, 0, sizeof(count));

		if (rm == 5) {
			DWORD sib;
			DWORD scale;
			DWORD idx;
			DWORD base;

			sib = cpu_codefetch(eip);
			eip++;

			scale = (sib >> 6) & 3;
			idx = (sib >> 3) & 7;
			base = sib & 7;

			if (base == 5 && mod == 0) {
				val = cpu_codefetch_d(eip);
				eip += 4;
				count[8] += val;
			} else {
				count[base]++;
			}
			if (idx != 4) {
				count[idx] += 1 << scale;
			}
		}

		if (rm == 6 && mod == 0) {
			/* disp32 */
			val = cpu_codefetch_d(eip);
			eip += 4;
			count[8] += val;
		} else {
			if (mod == 1) {
				/* disp8 */
				val = cpu_codefetch(eip);
				eip++;
				if (val & 0x80) {
					val |= 0xffffff00;
				}
				count[8] += val;
			} else if (mod == 2) {
				/* disp32 */
				val = cpu_codefetch_d(eip);
				eip += 4;
				count[8] += val;
			}
			if (rm != 5) {
				count[rm]++;
			}
		}

		n = 0;
		for (i = 0; i < 8; i++) {
			if (count[i] != 0) {
				if (n > 0) {
					milstr_ncat(buf, " + ", sizeof(buf));
				}
				if (count[i] > 1) {
					SPRINTF(tmp, "%s * %d",
					    reg32_str[i], count[i]);
				} else {
					milstr_ncpy(tmp, reg32_str[i], sizeof(tmp));
				}
				milstr_ncat(buf, tmp, sizeof(buf));
				n++;
			}
		}
		if (count[8] != 0) {
			if (n > 0) {
				milstr_ncat(buf, " + ", sizeof(buf));
			}
			SPRINTF(tmp, "0x%08x", count[8]);
			milstr_ncat(buf, tmp, sizeof(buf));
		}
	}
	fprintf(fp, "[%s]", buf);

	return eip;
}

void
close_instruction_trace(void)
{

	if (fp) {
		fclose(fp);
		fp = NULL;
	}
}
#endif	/* IA32_INSTRUCTION_TRACE */


sigjmp_buf exec_1step_jmpbuf;

void
exec_1step(void)
{
	int prefix;
	BYTE op;

	CPU_PREV_EIP = CPU_EIP;

	CPU_STATSAVE.cpu_inst = CPU_STATSAVE.cpu_inst_default;

#ifdef	IA32_INSTRUCTION_TRACE
{
	BYTE opcode[MAX_PREFIX + 1];
	DWORD eip = CPU_EIP;
	int num = 0;
	int i;
	BOOL t = cpu_inst_trace;

	if ((fp == NULL) && t) {
		fp = fopen("ia32trace.txt", "a");
	}
	if (fp && t) {
		fprintf(fp, "%04x:%08x:", CPU_CS, eip);
	}
#endif

	for (prefix = 0; prefix < MAX_PREFIX; prefix++) {
		GET_PCBYTE(op);

#ifdef	IA32_INSTRUCTION_TRACE
		eip++;
		if (fp && t) {
			opcode[num++] = op;
			fprintf(fp, " %02x", op);
		}
#endif

		/* prefix */
		if (insttable_info[op] & INST_PREFIX) {
			(*insttable_1byte[0][op])();
			continue;
		}
		break;
	}
	if (prefix == MAX_PREFIX) {
		EXCEPTION(UD_EXCEPTION, 0);
	}

#ifdef	IA32_INSTRUCTION_TRACE
	if (fp && t) {
		BYTE op2 = 0;
		if ((op == 0x0f)
		 || (op >= 0x80 && op <= 0x83)
		 || (op >= 0xc0 && op <= 0xc1)
		 || (op >= 0xd0 && op <= 0xd3)
		 || (op >= 0xf6 && op <= 0xf7)
		 || (op >= 0xfe /* && op <= 0xff */)
		) {
			op2 = cpu_codefetch(eip);
			eip++;
			fprintf(fp, " %02x", op2);
		}

		fprintf(fp, "\t\t");
		for (i = 0; i < num; i++) {
			BYTE c = opcode[i];
			const char *p = opcode_1byte[CPU_INST_OP32][c];
			if (p) {
				fprintf(fp, "%s ", p);
			} else {
				BYTE t = (op2 >> 3) & 7;

				switch (c) {
				case 0x80: case 0x81: case 0x82: case 0x83:
					p = opcode_0x8x[CPU_INST_OP32][c&1][t];
					break;

				case 0xc0: case 0xc1:
				case 0xd0: case 0xd1: case 0xd2: case 0xd3:
					p = opcode_shift[CPU_INST_OP32][c&1][t];
					break;

				case 0xf6:
				case 0xf7:
					p = opcode_0xf6[CPU_INST_OP32][c&1][t];
					break;

				case 0xfe:
				case 0xff:
					p = opcode_0xfe[CPU_INST_OP32][c&1][t];
					break;
				}
				if (p) {
					fprintf(fp, "%s ", p);
				}
			}
		}
		if (op == 0x0f) {
			const char *p = opcode_2byte[CPU_INST_OP32][op2];
			if (p) {
				fprintf(fp, "%s ", p);
			} else {
				BYTE t;

				t = cpu_codefetch(eip);
				eip++;
				t = (t >> 3) & 7;

				switch (op2) {
				case 0x00:
					p = opcode2_g6[t];
					break;

				case 0x01:
					p = opcode2_g7[t];
					break;

				case 0xba:
					p = opcode2_g8[t];
					break;

				case 0xc7:
					p = opcode2_g9[t];
					break;
				}
				if (p) {
					fprintf(fp, "%s ", p);
				}
			}
		}

		/* arg */
		switch (op) {
		case 0xcd:
			op2 = cpu_codefetch(eip);
			eip++;
			fprintf(fp, "0x%02x", op2);
			break;

		case 0xe4: case 0xe5: case 0xe6: case 0xe7:
			op2 = cpu_codefetch(eip);
			eip++;
			fprintf(fp, "0x%02x", op2);
			break;
		}
		fprintf(fp, "\n");
	}
}
#endif

	/* normal / rep, but not use */
	if (!(insttable_info[op] & INST_STRING) || !CPU_INST_REPUSE) {
		(*insttable_1byte[CPU_INST_OP32][op])();
		return;
	}

	/* rep */
	CPU_WORKCLOCK(5);
	if (!CPU_INST_AS32) {
		if (CPU_CX != 0) {
			if (!(insttable_info[op] & REP_CHECKZF)) {
				/* rep */
				do {
					(*insttable_1byte[CPU_INST_OP32][op])();
				} while (--CPU_CX);
			} else if (CPU_INST_REPUSE != 0xf2) {
				/* repe */
				do {
					(*insttable_1byte[CPU_INST_OP32][op])();
				} while (--CPU_CX && (CPU_FLAGL & Z_FLAG));
			} else {
				/* repne */
				do {
					(*insttable_1byte[CPU_INST_OP32][op])();
				} while (--CPU_CX && !(CPU_FLAGL & Z_FLAG));
			}
		}
	} else {
		if (CPU_ECX != 0) {
			if (!(insttable_info[op] & REP_CHECKZF)) {
				/* rep */
				do {
					(*insttable_1byte[CPU_INST_OP32][op])();
				} while (--CPU_ECX);
			} else if (CPU_INST_REPUSE != 0xf2) {
				/* repe */
				do {
					(*insttable_1byte[CPU_INST_OP32][op])();
				} while (--CPU_ECX && (CPU_FLAGL & Z_FLAG));
			} else {
				/* repne */
				do {
					(*insttable_1byte[CPU_INST_OP32][op])();
				} while (--CPU_ECX && !(CPU_FLAGL & Z_FLAG));
			}
		}
	}
}

/*	$Id: ia32xc.mcr,v 1.1 2003/12/08 00:55:31 yui Exp $	*/

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

#ifndef	IA32_CPU_IA32XC_MCR__
#define	IA32_CPU_IA32XC_MCR__

#if defined(IA32_CROSS_CHECK) && defined(__GNUC__) && (defined(i386) || defined(__i386__))

#define	IA32_CPU_ENABLE_XC

/*
 * arith
 */
#define	XC_ADDBYTE(r, d, s) \
do { \
	BYTE __s = (s) & 0xff; \
	BYTE __d = (d) & 0xff; \
	BYTE __r = __d; \
	BYTE __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_ADDBYTE((r), (d), (s)); \
	__R = (r) & 0xff; \
	\
	__asm__ __volatile__ ( \
		"pushl %%eax\n\t" \
		"movb %3, %%al\n\t" \
		"addb %4, %%al\n\t" \
		"movb %%al, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAPC_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_ADDBYTE: __s = %02x, __d = %02x", __s, __d); \
		ia32_warning("XC_ADDBYTE: __R = %02x, __r = %02x", \
		    __R, __r); \
		ia32_warning("XC_ADDBYTE: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAPC_FLAG); \
		ia32_warning("XC_ADDBYTE: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAPC_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_ADDWORD(r, d, s) \
do { \
	WORD __s = (s) & 0xffff; \
	WORD __d = (d) & 0xffff; \
	WORD __r = __d; \
	WORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_ADDWORD((r), (d), (s)); \
	__R = (r) & 0xffff; \
	\
	__asm__ __volatile__ ( \
		"pushl %%eax\n\t" \
		"movw %3, %%ax\n\t" \
		"addw %4, %%ax\n\t" \
		"movw %%ax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s) \
		: "eax", "ecx"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAPC_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_ADDWORD: __s = %04x, __d = %04x", __s, __d); \
		ia32_warning("XC_ADDWORD: __R = %04x, __r = %04x", \
		    __R, __r); \
		ia32_warning("XC_ADDWORD: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAPC_FLAG); \
		ia32_warning("XC_ADDWORD: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAPC_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_ADDDWORD(r, d, s) \
do { \
	DWORD __s = (s); \
	DWORD __d = (d); \
	DWORD __r = __d; \
	DWORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_ADDDWORD((r), (d), (s)); \
	__R = (r); \
	\
	__asm__ __volatile__ ( \
		"pushl %%eax\n\t" \
		"movl %3, %%eax\n\t" \
		"addl %4, %%eax\n\t" \
		"movl %%eax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s) \
		: "eax", "ecx"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAPC_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_ADDDWORD: __s = %08x, __d = %08x", __s, __d); \
		ia32_warning("XC_ADDDWORD: __R = %08x, __r = %08x", \
		    __R, __r); \
		ia32_warning("XC_ADDDWORD: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAPC_FLAG); \
		ia32_warning("XC_ADDDWORD: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAPC_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_ORBYTE(d, s) \
do { \
	BYTE __s = (s) & 0xff; \
	BYTE __d = (d) & 0xff; \
	BYTE __r = __d; \
	BYTE __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_ORBYTE((d), (s)); \
	__R = (d) & 0xff; \
	\
	__asm__ __volatile__ ( \
		"pushl %%eax\n\t" \
		"movb %3, %%al\n\t" \
		"orb %4, %%al\n\t" \
		"movb %%al, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZP_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_ORBYTE: __s = %02x, __d = %02x", __s, __d); \
		ia32_warning("XC_ORBYTE: __R = %02x, __r = %02x", \
		    __R, __r); \
		ia32_warning("XC_ORBYTE: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZP_FLAG); \
		ia32_warning("XC_ORBYTE: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZP_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_ORWORD(d, s) \
do { \
	WORD __s = (s) & 0xffff; \
	WORD __d = (d) & 0xffff; \
	WORD __r = __d; \
	WORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_ORWORD((d), (s)); \
	__R = (d) & 0xffff; \
	\
	__asm__ __volatile__ ( \
		"pushl %%eax\n\t" \
		"movw %3, %%ax\n\t" \
		"orw %4, %%ax\n\t" \
		"movw %%ax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZP_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_ORWORD: __s = %04x, __d = %04x", __s, __d); \
		ia32_warning("XC_ORWORD: __R = %04x, __r = %04x", \
		    __R, __r); \
		ia32_warning("XC_ORWORD: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZP_FLAG); \
		ia32_warning("XC_ORWORD: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZP_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_ORDWORD(d, s) \
do { \
	DWORD __s = (s); \
	DWORD __d = (d); \
	DWORD __r = __d; \
	DWORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_ORDWORD((d), (s)); \
	__R = (d); \
	\
	__asm__ __volatile__ ( \
		"pushl %%eax\n\t" \
		"movl %3, %%eax\n\t" \
		"orl %4, %%eax\n\t" \
		"movl %%eax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZP_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_ORDWORD: __s = %08x, __d = %08x", __s, __d); \
		ia32_warning("XC_ORDWORD: __R = %08x, __r = %08x", \
		    __R, __r); \
		ia32_warning("XC_ORDWORD: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZP_FLAG); \
		ia32_warning("XC_ORDWORD: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZP_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

/* flag no check */
#define	XC_ADCBYTE(r, d, s) \
do { \
	BYTE __s = (s) & 0xff; \
	BYTE __d = (d) & 0xff; \
	BYTE __r = __d; \
	BYTE __R; \
	BYTE __f; \
	BYTE __o; \
	BYTE __xc_flagl = CPU_FLAGL; \
	\
	_ADCBYTE((r), (d), (s)); \
	__R = (r) & 0xff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movzbl %5, %%eax\n\t" \
		"bt $0, %%eax\n\t" \
		"movb %3, %%al\n\t" \
		"adcb %4, %%al\n\t" \
		"movb %%al, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s), "m" (__xc_flagl) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAPC_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_ADCBYTE: __s = %02x, __d = %02x", __s, __d); \
		ia32_warning("XC_ADCBYTE: __R = %02x, __r = %02x", \
		    __R, __r); \
		ia32_warning("XC_ADCBYTE: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAPC_FLAG); \
		ia32_warning("XC_ADCBYTE: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAPC_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_ADCWORD(r, d, s) \
do { \
	WORD __s = (s) & 0xffff; \
	WORD __d = (d) & 0xffff; \
	WORD __r = __d; \
	WORD __R; \
	BYTE __f; \
	BYTE __o; \
	BYTE __xc_flagl = CPU_FLAGL; \
	\
	_ADCWORD((r), (d), (s)); \
	__R = (r) & 0xffff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movzbl %5, %%eax\n\t" \
		"bt $0, %%eax\n\t" \
		"movw %3, %%ax\n\t" \
		"adcw %4, %%ax\n\t" \
		"movw %%ax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s), "m" (__xc_flagl) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAPC_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_ADCWORD: __s = %04x, __d = %04x", __s, __d); \
		ia32_warning("XC_ADCWORD: __R = %04x, __r = %04x", \
		    __R, __r); \
		ia32_warning("XC_ADCWORD: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAPC_FLAG); \
		ia32_warning("XC_ADCWORD: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAPC_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_ADCDWORD(r, d, s) \
do { \
	DWORD __s = (s); \
	DWORD __d = (d); \
	DWORD __r = __d; \
	DWORD __R; \
	BYTE __f; \
	BYTE __o; \
	BYTE __xc_flagl = CPU_FLAGL; \
	\
	_ADCDWORD((r), (d), (s)); \
	__R = (r); \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movzbl %5, %%eax\n\t" \
		"bt $0, %%eax\n\t" \
		"movl %3, %%eax\n\t" \
		"adcl %4, %%eax\n\t" \
		"movl %%eax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s), "m" (__xc_flagl) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAPC_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_ADCDWORD: __s = %08x, __d = %08x", __s, __d); \
		ia32_warning("XC_ADCDWORD: __R = %08x, __r = %08x", \
		    __R, __r); \
		ia32_warning("XC_ADCDWORD: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAPC_FLAG); \
		ia32_warning("XC_ADCDWORD: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAPC_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

/* flag no check */
#define	XC_BYTE_SBB(r, d, s) \
do { \
	BYTE __s = (s) & 0xff; \
	BYTE __d = (d) & 0xff; \
	BYTE __r = __d; \
	BYTE __R; \
	BYTE __f; \
	BYTE __o; \
	BYTE __xc_flagl = CPU_FLAGL; \
	\
	_BYTE_SBB((r), (d), (s)); \
	__R = (r) & 0xff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movzbl %5, %%eax\n\t" \
		"bt $0, %%eax\n\t" \
		"movb %3, %%al\n\t" \
		"sbbb %4, %%al\n\t" \
		"movb %%al, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s), "m" (__xc_flagl) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAPC_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_BYTE_SBB: __s = %02x, __d = %02x", __s, __d); \
		ia32_warning("XC_BYTE_SBB: __R = %02x, __r = %02x", \
		    __R, __r); \
		ia32_warning("XC_BYTE_SBB: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAPC_FLAG); \
		ia32_warning("XC_BYTE_SBB: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAPC_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_WORD_SBB(r, d, s) \
do { \
	WORD __s = (s) & 0xffff; \
	WORD __d = (d) & 0xffff; \
	WORD __r = __d; \
	WORD __R; \
	BYTE __f; \
	BYTE __o; \
	BYTE __xc_flagl = CPU_FLAGL; \
	\
	_WORD_SBB((r), (d), (s)); \
	__R = (r) & 0xffff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movzbl %5, %%eax\n\t" \
		"bt $0, %%eax\n\t" \
		"movw %3, %%ax\n\t" \
		"sbbw %4, %%ax\n\t" \
		"movw %%ax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s), "m" (__xc_flagl) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAPC_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_WORD_SBB: __s = %04x, __d = %04x", __s, __d); \
		ia32_warning("XC_WORD_SBB: __R = %04x, __r = %04x", \
		    __R, __r); \
		ia32_warning("XC_WORD_SBB: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAPC_FLAG); \
		ia32_warning("XC_WORD_SBB: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAPC_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_DWORD_SBB(r, d, s) \
do { \
	DWORD __s = (s); \
	DWORD __d = (d); \
	DWORD __r = __d; \
	DWORD __R; \
	BYTE __f; \
	BYTE __o; \
	BYTE __xc_flagl = CPU_FLAGL; \
	\
	_DWORD_SBB((r), (d), (s)); \
	__R = (r); \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movzbl %5, %%eax\n\t" \
		"bt $0, %%eax\n\t" \
		"movl %3, %%eax\n\t" \
		"sbbl %4, %%eax\n\t" \
		"movl %%eax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s), "m" (__xc_flagl) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAPC_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_DWORD_SBB: __s = %08x, __d = %08x", __s, __d);\
		ia32_warning("XC_DWORD_SBB: __R = %08x, __r = %08x", \
		    __R, __r); \
		ia32_warning("XC_DWORD_SBB: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAPC_FLAG); \
		ia32_warning("XC_DWORD_SBB: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAPC_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_ANDBYTE(d, s) \
do { \
	BYTE __s = (s) & 0xff; \
	BYTE __d = (d) & 0xff; \
	BYTE __r = __d; \
	BYTE __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_ANDBYTE((d), (s)); \
	__R = (d) & 0xff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movb %3, %%al\n\t" \
		"andb %4, %%al\n\t" \
		"movb %%al, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZP_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_ANDBYTE: __s = %02x, __d = %02x", __s, __d); \
		ia32_warning("XC_ANDBYTE: __R = %02x, __r = %02x", \
		    __R, __r); \
		ia32_warning("XC_ANDBYTE: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZP_FLAG); \
		ia32_warning("XC_ANDBYTE: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZP_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_ANDWORD(d, s) \
do { \
	WORD __s = (s) & 0xffff; \
	WORD __d = (d) & 0xffff; \
	WORD __r = __d; \
	WORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_ANDWORD((d), (s)); \
	__R = (d) & 0xffff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movw %3, %%ax\n\t" \
		"andw %4, %%ax\n\t" \
		"movw %%ax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZP_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_ANDWORD: __s = %04x, __d = %04x", __s, __d); \
		ia32_warning("XC_ANDWORD: __R = %04x, __r = %04x", \
		    __R, __r); \
		ia32_warning("XC_ANDWORD: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZP_FLAG); \
		ia32_warning("XC_ANDWORD: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZP_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_ANDDWORD(d, s) \
do { \
	DWORD __s = (s); \
	DWORD __d = (d); \
	DWORD __r = __d; \
	DWORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_ANDDWORD((d), (s)); \
	__R = (d); \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movl %3, %%eax\n\t" \
		"andl %4, %%eax\n\t" \
		"movl %%eax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZP_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_ANDDWORD: __s = %08x, __d = %08x", __s, __d); \
		ia32_warning("XC_ANDDWORD: __R = %08x, __r = %08x", \
		    __R, __r); \
		ia32_warning("XC_ANDDWORD: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZP_FLAG); \
		ia32_warning("XC_ANDDWORD: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZP_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_BYTE_SUB(r, d, s) \
do { \
	BYTE __s = (s) & 0xff; \
	BYTE __d = (d) & 0xff; \
	BYTE __r = __d; \
	BYTE __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_BYTE_SUB((r), (d), (s)); \
	__R = (r) & 0xff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movb %3, %%al\n\t" \
		"subb %4, %%al\n\t" \
		"movb %%al, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAPC_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_BYTE_SUB: __s = %02x, __d = %02x", __s, __d); \
		ia32_warning("XC_BYTE_SUB: __R = %02x, __r = %02x", \
		    __R, __r); \
		ia32_warning("XC_BYTE_SUB: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAPC_FLAG); \
		ia32_warning("XC_BYTE_SUB: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAPC_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_WORD_SUB(r, d, s) \
do { \
	WORD __s = (s) & 0xffff; \
	WORD __d = (d) & 0xffff; \
	WORD __r = __d; \
	WORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_WORD_SUB((r), (d), (s)); \
	__R = (r) & 0xffff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movw %3, %%ax\n\t" \
		"subw %4, %%ax\n\t" \
		"movw %%ax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAPC_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_WORD_SUB: __s = %04x, __d = %04x", __s, __d); \
		ia32_warning("XC_WORD_SUB: __R = %04x, __r = %04x", \
		    __R, __r); \
		ia32_warning("XC_WORD_SUB: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAPC_FLAG); \
		ia32_warning("XC_WORD_SUB: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAPC_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_DWORD_SUB(r, d, s) \
do { \
	DWORD __s = (s); \
	DWORD __d = (d); \
	DWORD __r = __d; \
	DWORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_DWORD_SUB((r), (d), (s)); \
	__R = (r); \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movl %3, %%eax\n\t" \
		"subl %4, %%eax\n\t" \
		"movl %%eax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAPC_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_DWORD_SUB: __s = %08x, __d = %08x", __s, __d);\
		ia32_warning("XC_DWORD_SUB: __R = %08x, __r = %08x", \
		    __R, __r); \
		ia32_warning("XC_DWORD_SUB: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAPC_FLAG); \
		ia32_warning("XC_DWORD_SUB: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAPC_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_BYTE_XOR(d, s) \
do { \
	BYTE __s = (s) & 0xff; \
	BYTE __d = (d) & 0xff; \
	BYTE __r = __d; \
	BYTE __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_BYTE_XOR((d), (s)); \
	__R = (d) & 0xff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movb %3, %%al\n\t" \
		"xorb %4, %%al\n\t" \
		"movb %%al, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZP_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_XORBYTE: __s = %02x, __d = %02x", __s, __d); \
		ia32_warning("XC_XORBYTE: __R = %02x, __r = %02x", \
		    __R, __r); \
		ia32_warning("XC_XORBYTE: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZP_FLAG); \
		ia32_warning("XC_XORBYTE: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZP_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_WORD_XOR(d, s) \
do { \
	WORD __s = (s) & 0xffff; \
	WORD __d = (d) & 0xffff; \
	WORD __r = __d; \
	WORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_WORD_XOR((d), (s)); \
	__R = (d) & 0xffff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movw %3, %%ax\n\t" \
		"xorw %4, %%ax\n\t" \
		"movw %%ax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZP_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_XORWORD: __s = %04x, __d = %04x", __s, __d); \
		ia32_warning("XC_XORWORD: __R = %04x, __r = %04x", \
		    __R, __r); \
		ia32_warning("XC_XORWORD: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZP_FLAG); \
		ia32_warning("XC_XORWORD: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZP_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_DWORD_XOR(d, s) \
do { \
	DWORD __s = (s); \
	DWORD __d = (d); \
	DWORD __r = __d; \
	DWORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_DWORD_XOR((d), (s)); \
	__R = (d); \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movl %3, %%eax\n\t" \
		"xorl %4, %%eax\n\t" \
		"movl %%eax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r), "m" (__s) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZP_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_XORDWORD: __s = %08x, __d = %08x", __s, __d); \
		ia32_warning("XC_XORDWORD: __R = %08x, __r = %08x", \
		    __R, __r); \
		ia32_warning("XC_XORDWORD: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZP_FLAG); \
		ia32_warning("XC_XORDWORD: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZP_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_BYTE_NEG(d, s) \
do { \
	BYTE __s = (s) & 0xff; \
	BYTE __r = __s; \
	BYTE __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_BYTE_NEG((d), (s)); \
	__R = (d) & 0xff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"negb %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAPC_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_BYTE_NEG: __s = %02x", __s); \
		ia32_warning("XC_BYTE_NEG: __R = %02x, __r = %02x", \
		    __R, __r); \
		ia32_warning("XC_BYTE_NEG: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAPC_FLAG); \
		ia32_warning("XC_BYTE_NEG: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAPC_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_WORD_NEG(d, s) \
do { \
	WORD __s = (s) & 0xffff; \
	WORD __r = __s; \
	WORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_WORD_NEG((d), (s)); \
	__R = (d) & 0xffff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"negw %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAPC_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_WORD_NEG: __s = %04x", __s); \
		ia32_warning("XC_WORD_NEG: __R = %04x, __r = %04x", \
		    __R, __r); \
		ia32_warning("XC_WORD_NEG: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAPC_FLAG); \
		ia32_warning("XC_WORD_NEG: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAPC_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_DWORD_NEG(d, s) \
do { \
	DWORD __s = (s); \
	DWORD __r = __s; \
	DWORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_DWORD_NEG((d), (s)); \
	__R = (d); \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"negl %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAPC_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_DWORD_NEG: __s = %08x", __s);\
		ia32_warning("XC_DWORD_NEG: __R = %08x, __r = %08x", \
		    __R, __r); \
		ia32_warning("XC_DWORD_NEG: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAPC_FLAG); \
		ia32_warning("XC_DWORD_NEG: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAPC_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_BYTE_MUL(r, d, s) \
do { \
	BYTE __s = (s) & 0xff; \
	BYTE __d = (d) & 0xff; \
	WORD __r; \
	WORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_BYTE_MUL((r), (d), (s)); \
	__R = (r) & 0xffff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movb %3, %%al\n\t" \
		"movb %4, %%ah\n\t" \
		"mulb %%ah\n\t" \
		"movw %%ax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "m" (__d), "m" (__s) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & C_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_BYTE_MUL: __s = %02x, __d = %02x", __s, __d); \
		ia32_warning("XC_BYTE_MUL: __R = %02x, __r = %02x", \
		    __R, __r); \
		ia32_warning("XC_BYTE_MUL: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, C_FLAG); \
		ia32_warning("XC_BYTE_MUL: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & C_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_WORD_MUL(r, d, s) \
do { \
	WORD __s = (s) & 0xffff; \
	WORD __d = (d) & 0xffff; \
	DWORD __r; \
	DWORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_WORD_MUL((r), (d), (s)); \
	__R = (r); \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"push %%edx\n\t" \
		"movw %3, %%ax\n\t" \
		"movw %4, %%dx\n\t" \
		"mulw %%dx\n\t" \
		"movw %%ax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"andl $0x0000ffff, %0\n\t" \
		"shll $16, %%edx\n\t" \
		"orl %%edx, %0\n\t" \
		"popl %%edx\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "m" (__d), "m" (__s) \
		: "eax", "edx"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & C_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_WORD_MUL: __s = %04x, __d = %04x", __s, __d); \
		ia32_warning("XC_WORD_MUL: __R = %08x, __r = %08x", \
		    __R, __r); \
		ia32_warning("XC_WORD_MUL: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, C_FLAG); \
		ia32_warning("XC_WORD_MUL: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & C_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_DWORD_MUL(r, d, s) \
do { \
	DWORD __s = (s); \
	DWORD __d = (d); \
	DWORD __r; \
	DWORD __h; \
	DWORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_DWORD_MUL((r), (d), (s)); \
	__R = (r); \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"push %%edx\n\t" \
		"movl %4, %%eax\n\t" \
		"movl %5, %%edx\n\t" \
		"mull %%edx\n\t" \
		"movl %%eax, %0\n\t" \
		"movl %%edx, %1\n\t" \
		"lahf\n\t" \
		"movb %%ah, %2\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %3\n\t" \
		"popl %%edx\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__h), "=m" (__f), "=m" (__o) \
		: "m" (__d), "m" (__s) \
		: "eax", "edx"); \
	if ((__R != __r) || \
	    (CPU_OV != __h) || \
	    (((__f ^ CPU_FLAGL) & C_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_DWORD_MUL: __s = %08x, __d = %08x", __s, __d);\
		ia32_warning("XC_DWORD_MUL: __R = %08x, __r = %08x", \
		    __R, __r); \
		ia32_warning("XC_DWORD_MUL: CPU_OV == %08x, __h == %08x", \
		    CPU_OV, __h); \
		ia32_warning("XC_DWORD_MUL: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, C_FLAG); \
		ia32_warning("XC_DWORD_MUL: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & C_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_BYTE_IMUL(r, d, s) \
do { \
	SBYTE __s = (s) & 0xff; \
	SBYTE __d = (d) & 0xff; \
	SWORD __R; \
	SWORD __r; \
	BYTE __f; \
	BYTE __o; \
	\
	_BYTE_IMUL((r), (d), (s)); \
	__R = (r) & 0xffff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"movb %3, %%al\n\t" \
		"movb %4, %%ah\n\t" \
		"imulb %%ah\n\t" \
		"movw %%ax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "m" (__d), "m" (__s) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & C_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_BYTE_IMUL: __s = %02x, __d = %02x", __s, __d);\
		ia32_warning("XC_BYTE_IMUL: __R = %04x, __r = %04x", \
		    __R, __r); \
		ia32_warning("XC_BYTE_IMUL: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, C_FLAG); \
		ia32_warning("XC_BYTE_IMUL: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & C_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_WORD_IMUL(r, d, s) \
do { \
	SWORD __s = (s) & 0xffff; \
	SWORD __d = (d) & 0xffff; \
	SDWORD __r; \
	SDWORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_WORD_IMUL((r), (d), (s)); \
	__R = (r); \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"push %%edx\n\t" \
		"movw %3, %%ax\n\t" \
		"movw %4, %%dx\n\t" \
		"imulw %%dx\n\t" \
		"movw %%ax, %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"andl $0x0000ffff, %0\n\t" \
		"shll $16, %%edx\n\t" \
		"orl %%edx, %0\n\t" \
		"popl %%edx\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "m" (__d), "m" (__s) \
		: "eax", "edx"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & C_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_WORD_IMUL: __s = %04x, __d = %04x", __s, __d);\
		ia32_warning("XC_WORD_IMUL: __R = %08x, __r = %08x", \
		    __R, __r); \
		ia32_warning("XC_WORD_IMUL: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, C_FLAG); \
		ia32_warning("XC_WORD_IMUL: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & C_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_DWORD_IMUL(r, d, s) \
do { \
	SQWORD __R; \
	SDWORD __s = (s); \
	SDWORD __d = (d); \
	DWORD __r; \
	DWORD __h; \
	BYTE __f; \
	BYTE __o; \
	\
	_DWORD_IMUL((r), (d), (s)); \
	__R = (r); \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"push %%edx\n\t" \
		"movl %4, %%eax\n\t" \
		"movl %5, %%edx\n\t" \
		"imull %%edx\n\t" \
		"movl %%eax, %0\n\t" \
		"movl %%edx, %1\n\t" \
		"lahf\n\t" \
		"movb %%ah, %2\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %3\n\t" \
		"popl %%edx\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__h), "=m" (__f), "=m" (__o) \
		: "m" (__d), "m" (__s) \
		: "eax", "edx"); \
	if (((DWORD)__R != __r) || \
	    ((DWORD)(__R >> 32) != __h) || \
	    (((__f ^ CPU_FLAGL) & C_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_DWORD_IMUL: __s = %08x, __d = %08x",__s, __d);\
		ia32_warning("XC_DWORD_IMUL: __Rl = %08x, __r = %08x", \
		    (DWORD)__R, __r); \
		ia32_warning("XC_DWORD_IMUL: __Rh == %08x, __h == %08x", \
		    (DWORD)(__R >> 32), __h); \
		ia32_warning("XC_DWORD_IMUL: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, C_FLAG); \
		ia32_warning("XC_DWORD_IMUL: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert((DWORD)__R == __r); \
		assert((DWORD)(__R >> 32) == __h); \
		assert(((__f ^ CPU_FLAGL) & C_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

/* flag no check */
#define	XC_BYTE_INC(s) \
do { \
	BYTE __s = (s) & 0xff; \
	BYTE __r = __s; \
	BYTE __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_BYTE_INC((s)); \
	__R = (s) & 0xff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"incb %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAP_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_BYTE_INC: __s = %02x", __s); \
		ia32_warning("XC_BYTE_INC: __R = %02x, __r = %02x", \
		    __R, __r); \
		ia32_warning("XC_BYTE_INC: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAP_FLAG); \
		ia32_warning("XC_BYTE_INC: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAP_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_WORD_INC(s) \
do { \
	WORD __s = (s) & 0xffff; \
	WORD __r = __s; \
	WORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_WORD_INC((s)); \
	__R = (s) & 0xffff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"incw %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAP_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_WORD_INC: __s = %04x", __s); \
		ia32_warning("XC_WORD_INC: __R = %04x, __r = %04x", \
		    __R, __r); \
		ia32_warning("XC_WORD_INC: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAP_FLAG); \
		ia32_warning("XC_WORD_INC: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAP_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_DWORD_INC(s) \
do { \
	DWORD __s = (s); \
	DWORD __r = __s; \
	DWORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_DWORD_INC((s)); \
	__R = (s); \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"incl %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAP_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_DWORD_INC: __s = %08x", __s); \
		ia32_warning("XC_DWORD_INC: __R = %08x, __r = %08x", \
		    __R, __r); \
		ia32_warning("XC_DWORD_INC: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAP_FLAG); \
		ia32_warning("XC_DWORD_INC: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAP_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

/* flag no check */
#define	XC_BYTE_DEC(s) \
do { \
	BYTE __s = (s) & 0xff; \
	BYTE __r = __s; \
	BYTE __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_BYTE_DEC((s)); \
	__R = (s) & 0xff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"decb %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAP_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_BYTE_DEC: __s = %02x", __s); \
		ia32_warning("XC_BYTE_DEC: __R = %02x, __r = %02x", \
		    __R, __r); \
		ia32_warning("XC_BYTE_DEC: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAP_FLAG); \
		ia32_warning("XC_BYTE_DEC: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAP_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_WORD_DEC(s) \
do { \
	WORD __s = (s) & 0xffff; \
	WORD __r = __s; \
	WORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_WORD_DEC((s)); \
	__R = (s) & 0xffff; \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"decw %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAP_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_WORD_DEC: __s = %04x", __s); \
		ia32_warning("XC_WORD_DEC: __R = %04x, __r = %04x", \
		    __R, __r); \
		ia32_warning("XC_WORD_DEC: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAP_FLAG); \
		ia32_warning("XC_WORD_DEC: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAP_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XC_DWORD_DEC(s) \
do { \
	DWORD __s = (s); \
	DWORD __r = __s; \
	DWORD __R; \
	BYTE __f; \
	BYTE __o; \
	\
	_DWORD_DEC((s)); \
	__R = (s); \
	\
	__asm__ __volatile__ ( \
		"pushf\n\t" \
		"push %%eax\n\t" \
		"decl %0\n\t" \
		"lahf\n\t" \
		"movb %%ah, %1\n\t" \
		"seto %%ah\n\t" \
		"movb %%ah, %2\n\t" \
		"popl %%eax\n\t" \
		"popf\n\t" \
		: "=m" (__r), "=m" (__f), "=m" (__o) \
		: "0" (__r) \
		: "eax"); \
	if ((__R != __r) || \
	    (((__f ^ CPU_FLAGL) & SZAP_FLAG) != 0) || \
	    (!CPU_OV != !__o)) { \
		ia32_warning("XC_DWORD_DEC: __s = %08x", __s); \
		ia32_warning("XC_DWORD_DEC: __R = %08x, __r = %08x", \
		    __R, __r); \
		ia32_warning("XC_DWORD_DEC: CPU_FLAGL = %02x, __f = %02x, " \
		    "mask = %02x", CPU_FLAGL, __f, SZAP_FLAG); \
		ia32_warning("XC_DWORD_DEC: CPU_OV = %s, __o = %s", \
		    CPU_OV ? "OV" : "NV", __o ? "OV" : "NV"); \
		assert(__R == __r); \
		assert(((__f ^ CPU_FLAGL) & SZAP_FLAG) == 0); \
		assert(!CPU_OV == !__o); \
	} \
} while (/*CONSTCOND*/ 0)

#define	ADDBYTE(r, d, s)	XC_ADDBYTE(r, d, s)
#define	ADDWORD(r, d, s)	XC_ADDWORD(r, d, s)
#define	ADDDWORD(r, d, s)	XC_ADDDWORD(r, d, s)
#define	ORBYTE(d, s)		XC_ORBYTE(d, s)
#define	ORWORD(d, s)		XC_ORWORD(d, s)
#define	ORDWORD(d, s)		XC_ORDWORD(d, s)
#define	ADCBYTE(r, d, s)	XC_ADCBYTE(r, d, s)
#define	ADCWORD(r, d, s)	XC_ADCWORD(r, d, s)
#define	ADCDWORD(r, d, s)	XC_ADCDWORD(r, d, s)
#define	BYTE_SBB(r, d, s)	XC_BYTE_SBB(r, d, s)
#define	WORD_SBB(r, d, s)	XC_WORD_SBB(r, d, s)
#define	DWORD_SBB(r, d, s)	XC_DWORD_SBB(r, d, s)
#define	ANDBYTE(d, s)		XC_ANDBYTE(d, s)
#define	ANDWORD(d, s)		XC_ANDWORD(d, s)
#define	ANDDWORD(d, s)		XC_ANDDWORD(d, s)
#define	BYTE_SUB(r, d, s)	XC_BYTE_SUB(r, d, s)
#define	WORD_SUB(r, d, s)	XC_WORD_SUB(r, d, s)
#define	DWORD_SUB(r, d, s)	XC_DWORD_SUB(r, d, s)
#define	BYTE_XOR(d, s)		XC_BYTE_XOR(d, s)
#define	WORD_XOR(d, s)		XC_WORD_XOR(d, s)
#define	DWORD_XOR(d, s)		XC_DWORD_XOR(d, s)
#define	BYTE_NEG(d, s)		XC_BYTE_NEG(d, s)
#define	WORD_NEG(d, s)		XC_WORD_NEG(d, s)
#define	DWORD_NEG(d, s)		XC_DWORD_NEG(d, s)
#define	BYTE_MUL(r, d, s)	XC_BYTE_MUL(r, d, s)
#define	WORD_MUL(r, d, s)	XC_WORD_MUL(r, d, s)
#define	DWORD_MUL(r, d, s)	XC_DWORD_MUL(r, d, s)
#define	BYTE_IMUL(r, d, s)	XC_BYTE_IMUL(r, d, s)
#define	WORD_IMUL(r, d, s)	XC_WORD_IMUL(r, d, s)
#define	DWORD_IMUL(r, d, s)	XC_DWORD_IMUL(r, d, s)
#define	BYTE_INC(s)		XC_BYTE_INC(s)
#define	WORD_INC(s)		XC_WORD_INC(s)
#define	DWORD_INC(s)		XC_DWORD_INC(s)
#define	BYTE_DEC(s)		XC_BYTE_DEC(s)
#define	WORD_DEC(s)		XC_WORD_DEC(s)
#define	DWORD_DEC(s)		XC_DWORD_DEC(s)

#define	XC_STORE_FLAGL()	WORD __xc_flagl = CPU_FLAGL

#else	/* !(IA32_CROSS_CHECK && __GNUC__ && (i386) || __i386__)) */

#define	ADDBYTE(r, d, s)	_ADDBYTE(r, d, s)
#define	ADDWORD(r, d, s)	_ADDWORD(r, d, s)
#define	ADDDWORD(r, d, s)	_ADDDWORD(r, d, s)
#define	ORBYTE(d, s)		_ORBYTE(d, s)
#define	ORWORD(d, s)		_ORWORD(d, s)
#define	ORDWORD(d, s)		_ORDWORD(d, s)
#define	ADCBYTE(r, d, s)	_ADCBYTE(r, d, s)
#define	ADCWORD(r, d, s)	_ADCWORD(r, d, s)
#define	ADCDWORD(r, d, s)	_ADCDWORD(r, d, s)
#define	BYTE_SBB(r, d, s)	_BYTE_SBB(r, d, s)
#define	WORD_SBB(r, d, s)	_WORD_SBB(r, d, s)
#define	DWORD_SBB(r, d, s)	_DWORD_SBB(r, d, s)
#define	ANDBYTE(d, s)		_ANDBYTE(d, s)
#define	ANDWORD(d, s)		_ANDWORD(d, s)
#define	ANDDWORD(d, s)		_ANDDWORD(d, s)
#define	BYTE_SUB(r, d, s)	_BYTE_SUB(r, d, s)
#define	WORD_SUB(r, d, s)	_WORD_SUB(r, d, s)
#define	DWORD_SUB(r, d, s)	_DWORD_SUB(r, d, s)
#define	BYTE_XOR(d, s)		_BYTE_XOR(d, s)
#define	WORD_XOR(d, s)		_WORD_XOR(d, s)
#define	DWORD_XOR(d, s)		_DWORD_XOR(d, s)
#define	BYTE_NEG(d, s)		_BYTE_NEG(d, s)
#define	WORD_NEG(d, s)		_WORD_NEG(d, s)
#define	DWORD_NEG(d, s)		_DWORD_NEG(d, s)
#define	BYTE_MUL(r, d, s)	_BYTE_MUL(r, d, s)
#define	WORD_MUL(r, d, s)	_WORD_MUL(r, d, s)
#define	DWORD_MUL(r, d, s)	_DWORD_MUL(r, d, s)
#define	BYTE_IMUL(r, d, s)	_BYTE_IMUL(r, d, s)
#define	WORD_IMUL(r, d, s)	_WORD_IMUL(r, d, s)
#define	DWORD_IMUL(r, d, s)	_DWORD_IMUL(r, d, s)
#define	BYTE_INC(s)		_BYTE_INC(s)
#define	WORD_INC(s)		_WORD_INC(s)
#define	DWORD_INC(s)		_DWORD_INC(s)
#define	BYTE_DEC(s)		_BYTE_DEC(s)
#define	WORD_DEC(s)		_WORD_DEC(s)
#define	DWORD_DEC(s)		_DWORD_DEC(s)

#define	XC_STORE_FLAGL()

#endif	/* IA32_CROSS_CHECK && __GNUC__ && (i386) || __i386__) */

#endif	/* IA32_CPU_IA32_MCR__ */

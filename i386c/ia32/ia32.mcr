/*	$Id: ia32.mcr,v 1.9 2004/02/06 16:49:51 monaka Exp $	*/

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

#ifndef	IA32_CPU_IA32_MCR__
#define	IA32_CPU_IA32_MCR__

/*
 * misc
 */
#define	__CBW(src)	((WORD)((SBYTE)(src)))
#define	__CBD(src)	((DWORD)((SBYTE)(src)))
#define	__CWDE(src)	((SWORD)(src))

#define	SWAPBYTE(p, q) \
do { \
	BYTE __tmp = (p); \
	(p) = (q); \
	(q) = __tmp; \
} while (/*CONSTCOND*/ 0)

#define	SWAPWORD(p, q) \
do { \
	WORD __tmp = (p); \
	(p) = (q); \
	(q) = __tmp; \
} while (/*CONSTCOND*/ 0)

#define	SWAPDWORD(p, q) \
do { \
	DWORD __tmp = (p); \
	(p) = (q); \
	(q) = __tmp; \
} while (/*CONSTCOND*/ 0)


/*
 * clock
 */
#define	CPU_WORKCLOCK(clock) \
do { \
	CPU_REMCLOCK -= (clock); \
} while (/*CONSTCOND*/ 0)

#define	CPU_HALT() \
do { \
        CPU_REMCLOCK = -1; \
} while (/*CONSTCOND*/ 0)

#define	IRQCHECKTERM() \
do { \
	if (CPU_REMCLOCK > 0) { \
		CPU_BASECLOCK -= CPU_REMCLOCK; \
		CPU_REMCLOCK = 0; \
	} \
} while (/*CONSTCOND*/ 0)


/*
 * instruction pointer
 */
#define	SET_EIP(v) \
do { \
	DWORD __new_ip = (v); \
	if (__new_ip > CPU_STAT_CS_LIMIT) { \
		VERBOSE(("SET_EIP: new_ip = %08x, limit = %08x", __new_ip, CPU_STAT_CS_LIMIT)); \
		EXCEPTION(GP_EXCEPTION, 0); \
	} \
	CPU_EIP = __new_ip; \
} while (/*CONSTCOND*/ 0)

#define	ADD_EIP(v) \
do { \
	DWORD __tmp_ip = CPU_EIP + (v); \
	if (!CPU_STATSAVE.cpu_inst_default.op_32) { \
		__tmp_ip &= 0xffff; \
	} \
	CPU_EIP = __tmp_ip; \
} while (/*CONSTCOND*/ 0)


#define	GET_PCBYTE(v) \
do { \
	(v) = cpu_codefetch(CPU_EIP); \
	ADD_EIP(1); \
} while (/*CONSTCOND*/ 0)

#define	GET_PCBYTES(v) \
do { \
	(v) = __CBW(cpu_codefetch(CPU_EIP)); \
	ADD_EIP(1); \
} while (/*CONSTCOND*/ 0)

#define	GET_PCBYTESD(v) \
do { \
	(v) = __CBD(cpu_codefetch(CPU_EIP)); \
	ADD_EIP(1); \
} while (/*CONSTCOND*/ 0)

#define	GET_PCWORD(v) \
do { \
	(v) = cpu_codefetch_w(CPU_EIP); \
	ADD_EIP(2); \
} while (/*CONSTCOND*/ 0)

#define	GET_PCWORDS(v) \
do { \
	(v) = __CWDE(cpu_codefetch_w(CPU_EIP)); \
	ADD_EIP(2); \
} while (/*CONSTCOND*/ 0)

#define	GET_PCDWORD(v) \
do { \
	(v) = cpu_codefetch_d(CPU_EIP); \
	ADD_EIP(4); \
} while (/*CONSTCOND*/ 0)

#define	PREPART_EA_REG8(b, d_s) \
do { \
	GET_PCBYTE((b)); \
	(d_s) = *(reg8_b53[(b)]); \
} while (/*CONSTCOND*/ 0)

#define	PREPART_EA_REG8P(b, d_s) \
do { \
	GET_PCBYTE((b)); \
	(d_s) = reg8_b53[(b)]; \
} while (/*CONSTCOND*/ 0)

#define	PREPART_EA_REG16(b, d_s) \
do { \
	GET_PCBYTE((b)); \
	(d_s) = *(reg16_b53[(b)]); \
} while (/*CONSTCOND*/ 0)

#define	PREPART_EA_REG16P(b, d_s) \
do { \
	GET_PCBYTE((b)); \
	(d_s) = reg16_b53[(b)]; \
} while (/*CONSTCOND*/ 0)

#define	PREPART_EA_REG32(b, d_s) \
do { \
	GET_PCBYTE((b)); \
	(d_s) = *(reg32_b53[(b)]); \
} while (/*CONSTCOND*/ 0)

#define	PREPART_EA_REG32P(b, d_s) \
do { \
	GET_PCBYTE((b)); \
	(d_s) = reg32_b53[(b)]; \
} while (/*CONSTCOND*/ 0)

#define	PREPART_REG8_EA(b, s, d, regclk, memclk) \
do { \
	GET_PCBYTE((b)); \
	if ((b) >= 0xc0) { \
		CPU_WORKCLOCK(regclk); \
		(s) = *(reg8_b20[(b)]); \
	} else { \
		DWORD __t; \
		CPU_WORKCLOCK(memclk); \
		__t = calc_ea_dst((b)); \
		(s) = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, __t); \
	} \
	(d) = reg8_b53[(b)]; \
} while (/*CONSTCOND*/ 0)

#define	PREPART_REG16_EA(b, s, d, regclk, memclk) \
do { \
	GET_PCBYTE((b)); \
	if ((b) >= 0xc0) { \
		CPU_WORKCLOCK(regclk); \
		(s) = *(reg16_b20[(b)]); \
	} else { \
		DWORD __t; \
		CPU_WORKCLOCK(memclk); \
		__t = calc_ea_dst((b)); \
		(s) = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, __t); \
	} \
	(d) = reg16_b53[(b)]; \
} while (/*CONSTCOND*/ 0)

#define	PREPART_REG16_EA8(b, s, d, regclk, memclk) \
do { \
	GET_PCBYTE((b)); \
	if ((b) >= 0xc0) { \
		CPU_WORKCLOCK(regclk); \
		(s) = *(reg8_b20[(b)]); \
	} else { \
		DWORD __t; \
		CPU_WORKCLOCK(memclk); \
		__t = calc_ea_dst((b)); \
		(s) = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, __t); \
	} \
	(d) = reg16_b53[(b)]; \
} while (/*CONSTCOND*/ 0)

#define	PREPART_REG32_EA(b, s, d, regclk, memclk) \
do { \
	GET_PCBYTE((b)); \
	if ((b) >= 0xc0) { \
		CPU_WORKCLOCK(regclk); \
		(s) = *(reg32_b20[(b)]); \
	} else { \
		DWORD __t; \
		CPU_WORKCLOCK(memclk); \
		__t = calc_ea_dst((b)); \
		(s) = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, __t); \
	} \
	(d) = reg32_b53[(b)]; \
} while (/*CONSTCOND*/ 0)

#define	PREPART_REG32_EA8(b, s, d, regclk, memclk) \
do { \
	GET_PCBYTE((b)); \
	if ((b) >= 0xc0) { \
		CPU_WORKCLOCK(regclk); \
		(s) = *(reg8_b20[(b)]); \
	} else { \
		DWORD __t; \
		CPU_WORKCLOCK(memclk); \
		__t = calc_ea_dst((b)); \
		(s) = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, __t); \
	} \
	(d) = reg32_b53[(b)]; \
} while (/*CONSTCOND*/ 0)

#define	PREPART_REG32_EA16(b, s, d, regclk, memclk) \
do { \
	GET_PCBYTE((b)); \
	if ((b) >= 0xc0) { \
		CPU_WORKCLOCK(regclk); \
		(s) = *(reg16_b20[(b)]); \
	} else { \
		DWORD __t; \
		CPU_WORKCLOCK(memclk); \
		__t = calc_ea_dst((b)); \
		(s) = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, __t); \
	} \
	(d) = reg32_b53[(b)]; \
} while (/*CONSTCOND*/ 0)


/*
 * arith
 */
#define	_ADDBYTE(r, d, s) \
do { \
	(r) = (s) + (d); \
	CPU_OV = ((r) ^ (s)) & ((r) ^ (d)) & 0x80; \
	CPU_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG); \
	CPU_FLAGL |= szpcflag[(r) & 0x1ff]; \
} while (/*CONSTCOND*/ 0)

#define	_ADDWORD(r, d, s) \
do { \
	(r) = (s) + (d); \
	CPU_OV = ((r) ^ (s)) & ((r) ^ (d)) & 0x8000; \
	CPU_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG); \
	if ((r) & 0xffff0000) { \
		(r) &= 0x0000ffff; \
		CPU_FLAGL |= C_FLAG; \
	} \
	CPU_FLAGL |= szpflag_w[(WORD)(r)]; \
} while (/*CONSTCOND*/ 0)

#define	_ADDDWORD(r, d, s) \
do { \
	(r) = (s) + (d); \
	CPU_OV = ((r) ^ (s)) & ((r) ^ (d)) & 0x80000000; \
	CPU_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG); \
	if ((r) < (s)) { \
		CPU_FLAGL |= C_FLAG; \
	} \
	if ((r) == 0) { \
		CPU_FLAGL |= Z_FLAG; \
	} \
	if ((r) & 0x80000000) { \
		CPU_FLAGL |= S_FLAG; \
	} \
	CPU_FLAGL |= szpcflag[(BYTE)(r)] & P_FLAG; \
} while (/*CONSTCOND*/ 0)

#define	_ORBYTE(d, s) \
do { \
	(d) |= (s); \
	CPU_OV = 0; \
	CPU_FLAGL = szpcflag[(BYTE)(d)]; \
} while (/*CONSTCOND*/ 0)

#define	_ORWORD(d, s) \
do { \
	(d) |= (s); \
	CPU_OV = 0; \
	CPU_FLAGL = szpflag_w[(WORD)(d)]; \
} while (/*CONSTCOND*/ 0)

#define	_ORDWORD(d, s) \
do { \
	(d) |= (s); \
	CPU_OV = 0; \
	CPU_FLAGL = szpcflag[(BYTE)(d)] & P_FLAG; \
	if ((d) == 0) { \
		CPU_FLAGL |= Z_FLAG; \
	} \
	if ((d) & 0x80000000) { \
		CPU_FLAGL |= S_FLAG; \
	} \
} while (/*CONSTCOND*/ 0)

/* flag no check */
#define	_ADCBYTE(r, d, s) \
do { \
	(r) = (CPU_FLAGL & C_FLAG) + (s) + (d); \
	CPU_OV = ((r) ^ (s)) & ((r) ^ (d)) & 0x80; \
	CPU_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG); \
	CPU_FLAGL |= szpcflag[(r) & 0x1ff]; \
} while (/*CONSTCOND*/ 0)

#define	_ADCWORD(r, d, s) \
do { \
	(r) = (CPU_FLAGL & C_FLAG) + (s) + (d); \
	CPU_OV = ((r) ^ (s)) & ((r) ^ (d)) & 0x8000; \
	CPU_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG); \
	if ((r) & 0xffff0000) { \
		(r) &= 0x0000ffff; \
		CPU_FLAGL |= C_FLAG; \
	} \
	CPU_FLAGL |= szpflag_w[(WORD)(r)]; \
} while (/*CONSTCOND*/ 0)

#define	_ADCDWORD(r, d, s) \
do { \
	DWORD __c = (CPU_FLAGL & C_FLAG); \
	(r) = (s) + (d) + __c; \
	CPU_OV = ((r) ^ (s)) & ((r) ^ (d)) & 0x80000000; \
	CPU_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG); \
	if ((!__c && (r) < (s)) || (__c && (r) <= (s))) { \
		CPU_FLAGL |= C_FLAG; \
	} \
	if ((r) == 0) { \
		CPU_FLAGL |= Z_FLAG; \
	} \
	if ((r) & 0x80000000) { \
		CPU_FLAGL |= S_FLAG; \
	} \
	CPU_FLAGL |= szpcflag[(BYTE)(r)] & P_FLAG; \
} while (/*CONSTCOND*/ 0)

/* flag no check */
#define	_BYTE_SBB(r, d, s) \
do { \
	(r) = (d) - (s) - (CPU_FLAGL & C_FLAG); \
	CPU_OV = ((d) ^ (r)) & ((d) ^ (s)) & 0x80; \
	CPU_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG); \
	CPU_FLAGL |= szpcflag[(r) & 0x1ff]; \
} while (/*CONSTCOND*/ 0)

#define	_WORD_SBB(r, d, s) \
do { \
	(r) = (d) - (s) - (CPU_FLAGL & C_FLAG); \
	CPU_OV = ((d) ^ (r)) & ((d) ^ (s)) & 0x8000; \
	CPU_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG); \
	if ((r) & 0xffff0000) { \
		(r) &= 0x0000ffff; \
		CPU_FLAGL |= C_FLAG; \
	} \
	CPU_FLAGL |= szpflag_w[(WORD)(r)]; \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_SBB(r, d, s) \
do { \
	DWORD __c = (CPU_FLAGL & C_FLAG); \
	(r) = (d) - (s) - __c; \
	CPU_OV = ((d) ^ (r)) & ((d) ^ (s)) & 0x80000000; \
	CPU_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG); \
	if ((!__c && (d) < (s)) || (__c && (d) <= (s))) { \
		CPU_FLAGL |= C_FLAG; \
	} \
	if ((r) == 0) { \
		CPU_FLAGL |= Z_FLAG; \
	} \
	if ((r) & 0x80000000) { \
		CPU_FLAGL |= S_FLAG; \
	} \
	CPU_FLAGL |= szpcflag[(BYTE)(r)] & P_FLAG; \
} while (/*CONSTCOND*/ 0)

#define	_ANDBYTE(d, s) \
do { \
	(d) &= (s); \
	CPU_OV = 0; \
	CPU_FLAGL = szpcflag[(BYTE)(d)]; \
} while (/*CONSTCOND*/ 0)

#define	_ANDWORD(d, s) \
do { \
	(d) &= (s); \
	CPU_OV = 0; \
	CPU_FLAGL = szpflag_w[(WORD)(d)]; \
} while (/*CONSTCOND*/ 0)

#define	_ANDDWORD(d, s) \
do { \
	(d) &= (s); \
	CPU_OV = 0; \
	CPU_FLAGL = szpcflag[(BYTE)(d)] & P_FLAG; \
	if ((d) == 0) { \
		CPU_FLAGL |= Z_FLAG; \
	} \
	if ((d) & 0x80000000) { \
		CPU_FLAGL |= S_FLAG; \
	} \
} while (/*CONSTCOND*/ 0)

#define	_BYTE_SUB(r, d, s) \
do { \
	(r) = (d) - (s); \
	CPU_OV = ((d) ^ (r)) & ((d) ^ (s)) & 0x80; \
	CPU_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG); \
	CPU_FLAGL |= szpcflag[(r) & 0x1ff]; \
} while (/*CONSTCOND*/ 0)

#define	_WORD_SUB(r, d, s) \
do { \
	(r) = (d) - (s); \
	CPU_OV = ((d) ^ (r)) & ((d) ^ (s)) & 0x8000; \
	CPU_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG); \
	if ((r) & 0xffff0000) { \
		(r) &= 0x0000ffff; \
		CPU_FLAGL |= C_FLAG; \
	} \
	CPU_FLAGL |= szpflag_w[(WORD)(r)]; \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_SUB(r, d, s) \
do { \
	(r) = (d) - (s); \
	CPU_OV = ((d) ^ (r)) & ((d) ^ (s)) & 0x80000000; \
	CPU_FLAGL = (BYTE)(((r) ^ (d) ^ (s)) & A_FLAG); \
	if ((d) < (s)) { \
		CPU_FLAGL |= C_FLAG; \
	} \
	if ((r) == 0) { \
		CPU_FLAGL |= Z_FLAG; \
	} \
	if ((r) & 0x80000000) { \
		CPU_FLAGL |= S_FLAG; \
	} \
	CPU_FLAGL |= szpcflag[(BYTE)(r)] & P_FLAG; \
} while (/*CONSTCOND*/ 0)

#define	_BYTE_XOR(d, s) \
do { \
	(d) ^= s; \
	CPU_OV = 0; \
	CPU_FLAGL = szpcflag[(BYTE)(d)]; \
} while (/*CONSTCOND*/ 0)

#define	_WORD_XOR(d, s) \
do { \
	(d) ^= (s); \
	CPU_OV = 0; \
	CPU_FLAGL = szpflag_w[(WORD)(d)]; \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_XOR(d, s) \
do { \
	(d) ^= (s); \
	CPU_OV = 0; \
	CPU_FLAGL = szpcflag[(BYTE)(d)] & P_FLAG; \
	if ((d) == 0) { \
		CPU_FLAGL |= Z_FLAG; \
	} \
	if ((d) & 0x80000000) { \
		CPU_FLAGL |= S_FLAG; \
	} \
} while (/*CONSTCOND*/ 0)

#define	_BYTE_NEG(d, s) \
do { \
	(d) = 0 - (s); \
	CPU_OV = ((d) & (s)) & 0x80; \
	CPU_FLAGL = (BYTE)(((d) ^ (s)) & A_FLAG); \
	CPU_FLAGL |= szpcflag[(d) & 0x1ff]; \
} while (/*CONSTCOND*/ 0)

#define	_WORD_NEG(d, s) \
do { \
	(d) = 0 - (s); \
	CPU_OV = ((d) & (s)) & 0x8000; \
	CPU_FLAGL = (BYTE)(((d) ^ (s)) & A_FLAG); \
	if ((d) & 0xffff0000) { \
		(d) &= 0x0000ffff; \
		CPU_FLAGL |= C_FLAG; \
	} \
	CPU_FLAGL |= szpflag_w[(WORD)(d)]; \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_NEG(d, s) \
do { \
	(d) = 0 - (s); \
	CPU_OV = ((d) & (s)) & 0x80000000; \
	CPU_FLAGL = (BYTE)(((d) ^ (s)) & A_FLAG); \
	if ((d) == 0) { \
		CPU_FLAGL |= Z_FLAG; \
	} else { \
		CPU_FLAGL |= C_FLAG; \
	} \
	if ((d) & 0x80000000) { \
		CPU_FLAGL |= S_FLAG; \
	} \
	CPU_FLAGL |= szpcflag[(BYTE)(d)] & P_FLAG; \
} while (/*CONSTCOND*/ 0)

#define	_BYTE_MUL(r, d, s) \
do { \
	CPU_FLAGL &= (Z_FLAG | S_FLAG | A_FLAG | P_FLAG); \
	(r) = (BYTE)(d) * (BYTE)(s); \
	CPU_OV = (r) >> 8; \
	if (CPU_OV) { \
		CPU_FLAGL |= C_FLAG; \
	} \
} while (/*CONSTCOND*/ 0)

#define	_WORD_MUL(r, d, s) \
do { \
	CPU_FLAGL &= (Z_FLAG | S_FLAG | A_FLAG | P_FLAG); \
	(r) = (WORD)(d) * (WORD)(s); \
	CPU_OV = (r) >> 16; \
	if (CPU_OV) { \
		CPU_FLAGL |= C_FLAG; \
	} \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_MUL(r, d, s) \
do { \
	QWORD __v; \
	CPU_FLAGL &= (Z_FLAG | S_FLAG | A_FLAG | P_FLAG); \
	__v = (QWORD)(d) * (QWORD)(s); \
	(r) = (DWORD)__v; \
	CPU_OV = (DWORD)(__v >> 32); \
	if (CPU_OV) { \
		CPU_FLAGL |= C_FLAG; \
	} \
} while (/*CONSTCOND*/ 0)

#define	_BYTE_IMUL(r, d, s) \
do { \
	CPU_FLAGL &= (Z_FLAG | S_FLAG | A_FLAG | P_FLAG); \
	(r) = (SBYTE)(d) * (SBYTE)(s); \
	CPU_OV = ((r) + 0x80) & 0xffffff00; \
	if (CPU_OV) { \
		CPU_FLAGL |= C_FLAG; \
	} \
} while (/*CONSTCOND*/ 0)

#define	_WORD_IMUL(r, d, s) \
do { \
	CPU_FLAGL &= (Z_FLAG | S_FLAG | A_FLAG | P_FLAG); \
	(r) = (SWORD)(d) * (SWORD)(s); \
	/*     -32768 < r          < 32767      (CF = OV = 0) */ \
	/* 0xffff8000 < r          < 0x00007fff (CF = OV = 0) */ \
	/* 0x00000000 < r + 0x8000 < 0x0000ffff (CF = OV = 0) */ \
	CPU_OV = ((r) + 0x8000) & 0xffff0000; \
	if (CPU_OV) { \
		CPU_FLAGL |= C_FLAG; \
	} \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_IMUL(r, d, s) \
do { \
	CPU_FLAGL &= (Z_FLAG | S_FLAG | A_FLAG | P_FLAG); \
	(r) = (SQWORD)(d) * (SQWORD)(s); \
	CPU_OV = (DWORD)(((r) + 0x80000000ULL) >> 32); \
	if (CPU_OV) { \
		CPU_FLAGL |= C_FLAG; \
	} \
} while (/*CONSTCOND*/ 0)

/* flag no check */
#define	_BYTE_INC(s) \
do { \
	BYTE __b = (s); \
	__b++; \
	CPU_OV = __b & (__b ^ (s)) & 0x80; \
	CPU_FLAGL &= C_FLAG; \
	CPU_FLAGL |= (BYTE)((__b ^ (s)) & A_FLAG); \
	CPU_FLAGL |= szpcflag[__b]; \
	(s) = __b; \
} while (/*CONSTCOND*/ 0)

#define	_WORD_INC(s) \
do { \
	WORD __b = (s); \
	__b++; \
	CPU_OV = __b & (__b ^ (s)) & 0x8000; \
	CPU_FLAGL &= C_FLAG; \
	CPU_FLAGL |= (BYTE)((__b ^ (s)) & A_FLAG); \
	CPU_FLAGL |= szpflag_w[__b]; \
	(s) = __b; \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_INC(s) \
do { \
	DWORD __b = (s); \
	__b++; \
	CPU_OV = __b & (__b ^ (s)) & 0x80000000; \
	CPU_FLAGL &= C_FLAG; \
	CPU_FLAGL |= (BYTE)((__b ^ (s)) & A_FLAG); \
	if (__b == 0) { \
		CPU_FLAGL |= Z_FLAG; \
	} \
	if (__b & 0x80000000) { \
		CPU_FLAGL |= S_FLAG; \
	} \
	CPU_FLAGL |= szpcflag[(BYTE)(__b)] & P_FLAG; \
	(s) = __b; \
} while (/*CONSTCOND*/ 0)

/* flag no check */
#define	_BYTE_DEC(s) \
do { \
	BYTE __b = (s); \
	__b--; \
	CPU_OV = (s) & (__b ^ (s)) & 0x80; \
	CPU_FLAGL &= C_FLAG; \
	CPU_FLAGL |= (BYTE)((__b ^ (s)) & A_FLAG); \
	CPU_FLAGL |= szpcflag[__b]; \
	(s) = __b; \
} while (/*CONSTCOND*/ 0)

#define	_WORD_DEC(s) \
do { \
	WORD __b = (s); \
	__b--; \
	CPU_OV = (s) & (__b ^ (s)) & 0x8000; \
	CPU_FLAGL &= C_FLAG; \
	CPU_FLAGL |= (BYTE)((__b ^ (s)) & A_FLAG); \
	CPU_FLAGL |= szpflag_w[__b]; \
	(s) = __b; \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_DEC(s) \
do { \
	DWORD __b = (s); \
	__b--; \
	CPU_OV = (s) & (__b ^ (s)) & 0x80000000; \
	CPU_FLAGL &= C_FLAG; \
	CPU_FLAGL |= (BYTE)((__b ^ (s)) & A_FLAG); \
	if ((__b) == 0) { \
		CPU_FLAGL |= Z_FLAG; \
	} \
	if ((__b) & 0x80000000) { \
		CPU_FLAGL |= S_FLAG; \
	} \
	CPU_FLAGL |= szpcflag[(BYTE)(__b)] & P_FLAG; \
	(s) = __b; \
} while (/*CONSTCOND*/ 0)


/*
 * stack
 */
#define	REGPUSH(reg, clock) \
do { \
	CPU_SP -= 2; \
	cpu_vmemorywrite_w(CPU_SS_INDEX, CPU_SP, reg); \
	CPU_WORKCLOCK(clock); \
} while (/*CONSTCOND*/ 0)

#define	REGPUSH_32(reg, clock) \
do { \
	CPU_ESP -= 4; \
	cpu_vmemorywrite_d(CPU_SS_INDEX, CPU_ESP, reg); \
	CPU_WORKCLOCK(clock); \
} while (/*CONSTCOND*/ 0)

#define	REGPUSH0(reg) \
do { \
	CPU_SP -= 2; \
	cpu_vmemorywrite_w(CPU_SS_INDEX, CPU_SP, reg); \
} while (/*CONSTCOND*/ 0)

/* Operand Size == 16 && Stack Size == 32 */
#define	REGPUSH0_16_32(reg) \
do { \
	CPU_ESP -= 2; \
	cpu_vmemorywrite_w(CPU_SS_INDEX, CPU_ESP, reg); \
} while (/*CONSTCOND*/ 0)

/* Operand Size == 32 && Stack Size == 16 */
#define	REGPUSH0_32_16(reg) \
do { \
	CPU_SP -= 4; \
	cpu_vmemorywrite_d(CPU_SS_INDEX, CPU_SP, reg); \
} while (/*CONSTCOND*/ 0)

#define	REGPUSH0_32(reg) \
do { \
	CPU_ESP -= 4; \
	cpu_vmemorywrite_d(CPU_SS_INDEX, CPU_ESP, reg); \
} while (/*CONSTCOND*/ 0)

#define	PUSH0_16(reg) \
do { \
	if (!CPU_STAT_SS32) { \
		REGPUSH0(reg); \
	} else { \
		REGPUSH0_16_32(reg); \
	} \
} while (/*CONSTCOND*/ 0)

#define	PUSH0_32(reg) \
do { \
	if (CPU_STAT_SS32) { \
		REGPUSH0_32(reg); \
	} else { \
		REGPUSH0_32_16(reg); \
	} \
} while (/*CONSTCOND*/ 0)

#define	XPUSH0(reg) \
do { \
	if (!CPU_INST_OP32) { \
		PUSH0_16(reg); \
	} else { \
		PUSH0_32(reg); \
	} \
} while (/*CONSTCOND*/ 0)

#define	REGPOP(reg, clock) \
do { \
	(reg) = cpu_vmemoryread_w(CPU_SS_INDEX, CPU_SP); \
	CPU_SP += 2; \
	CPU_WORKCLOCK(clock); \
} while (/*CONSTCOND*/ 0)

#define	REGPOP_32(reg, clock) \
do { \
	(reg) = cpu_vmemoryread_d(CPU_SS_INDEX, CPU_ESP); \
	CPU_ESP += 4; \
	CPU_WORKCLOCK(clock); \
} while (/*CONSTCOND*/ 0)

#define	REGPOP0(reg) \
do { \
	(reg) = cpu_vmemoryread_w(CPU_SS_INDEX, CPU_SP); \
	CPU_SP += 2; \
} while (/*CONSTCOND*/ 0)

#define	REGPOP0_16_32(reg) \
do { \
	(reg) = cpu_vmemoryread_w(CPU_SS_INDEX, CPU_ESP); \
	CPU_ESP += 2; \
} while (/*CONSTCOND*/ 0)

#define	REGPOP0_32_16(reg) \
do { \
	(reg) = cpu_vmemoryread_d(CPU_SS_INDEX, CPU_SP); \
	CPU_SP += 4; \
} while (/*CONSTCOND*/ 0)

#define	REGPOP0_32(reg) \
do { \
	(reg) = cpu_vmemoryread_d(CPU_SS_INDEX, CPU_ESP); \
	CPU_ESP += 4; \
} while (/*CONSTCOND*/ 0)

#define	POP0_16(reg) \
do { \
	if (!CPU_STAT_SS32) { \
		REGPOP0(reg); \
	} else { \
		REGPOP0_16_32(reg); \
	} \
} while (/*CONSTCOND*/ 0)

#define	POP0_32(reg) \
do { \
	if (CPU_STAT_SS32) { \
		REGPOP0_32(reg); \
	} else { \
		REGPOP0_32_16(reg); \
	} \
} while (/*CONSTCOND*/ 0)

/*
 * stack pointer
 */
#define	SP_PUSH_16(reg) \
do { \
	WORD sp = CPU_SP; \
	if (!CPU_STAT_SS32) { \
		REGPUSH0(sp); \
	} else { \
		REGPUSH0_16_32(sp); \
	} \
} while (/*CONSTCOND*/ 0)

#define	ESP_PUSH_32(reg) \
do { \
	DWORD sp = CPU_ESP; \
	if (!CPU_STAT_SS32) { \
		REGPUSH0_32_16(sp); \
	} else { \
		REGPUSH0_32(sp); \
	} \
} while (/*CONSTCOND*/ 0)

#define	SP_POP_16(reg) \
do { \
	DWORD sp; \
	if (!CPU_STAT_SS32) { \
		sp = CPU_SP; \
	} else { \
		sp = CPU_ESP; \
	} \
	CPU_SP = cpu_vmemoryread_w(CPU_SS_INDEX, sp); \
} while (/*CONSTCOND*/ 0)

#define	ESP_POP_32(reg) \
do { \
	DWORD sp; \
	if (!CPU_STAT_SS32) { \
		sp = CPU_SP; \
	} else { \
		sp = CPU_ESP; \
	} \
	CPU_ESP = cpu_vmemoryread_d(CPU_SS_INDEX, sp); \
} while (/*CONSTCOND*/ 0)


/*
 * jump
 */
#define	JMPSHORT(clock) \
do { \
	DWORD __ip; \
	CPU_WORKCLOCK(clock); \
	__ip = __CBD(cpu_codefetch(CPU_EIP)); \
	__ip++; \
	ADD_EIP(__ip); \
} while (/*CONSTCOND*/ 0)

#define	JMPNEAR(clock) \
do { \
	DWORD __ip; \
	CPU_WORKCLOCK(clock); \
	__ip = __CWDE(cpu_codefetch_w(CPU_EIP)); \
	__ip += 2; \
	ADD_EIP(__ip); \
} while (/*CONSTCOND*/ 0)

#define	JMPNEAR_4(clock) \
do { \
	DWORD __ip; \
	CPU_WORKCLOCK(clock); \
	__ip = cpu_codefetch_d(CPU_EIP); \
	__ip += 4; \
	ADD_EIP(__ip); \
} while (/*CONSTCOND*/ 0)

#define	JMPNOP(clock, d) \
do { \
	CPU_WORKCLOCK(clock); \
	ADD_EIP((d)); \
} while (/*CONSTCOND*/ 0)


/*
 * instruction check
 */
#include "ia32xc.mcr"

#endif	/* IA32_CPU_IA32_MCR__ */

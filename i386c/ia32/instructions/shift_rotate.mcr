/*	$Id: shift_rotate.mcr,v 1.2 2004/01/15 15:50:33 monaka Exp $	*/

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

#ifndef	IA32_CPU_SHIFT_ROTATE_MCR__
#define	IA32_CPU_SHIFT_ROTATE_MCR__

/*
 * SAR
 */
#define	_BYTE_SAR1(d, s) \
do { \
	(d) = (BYTE)(((SBYTE)(s)) >> 1); \
	CPU_OV = 0; \
	CPU_FLAGL = (BYTE)(szpcflag[(BYTE)(d)] | A_FLAG | ((s) & 1)); \
} while (/*CONSTCOND*/ 0)

#define	_WORD_SAR1(d, s) \
do { \
	(d) = (WORD)(((SWORD)(s)) >> 1); \
	CPU_OV = 0; \
	CPU_FLAGL = (BYTE)(szpflag_w[(WORD)(d)] | A_FLAG | ((s) & 1)); \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_SAR1(d, s) \
do { \
	(d) = (DWORD)(((SDWORD)(s)) >> 1); \
	CPU_OV = 0; \
	CPU_FLAGL = (BYTE)(A_FLAG | ((s) & 1)); /* C_FLAG */ \
	if ((d) == 0) { \
		CPU_FLAGL |= Z_FLAG; \
	} \
	if ((d) & 0x80000000) { \
		CPU_FLAGL |= S_FLAG; \
	} \
	CPU_FLAGL |= (szpcflag[(BYTE)(d)] & P_FLAG); \
} while (/*CONSTCOND*/ 0)

#define	_BYTE_SARCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if (c) { \
		(s) = ((SBYTE)(s)) >> ((c) - 1); \
		CPU_FLAGL = (BYTE)((s) & 1); /* C_FLAG */ \
		(s) = (BYTE)(((SBYTE)(s)) >> 1); \
		CPU_OV = 0; \
		CPU_FLAGL |= (szpcflag[(BYTE)(s)] | A_FLAG); \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

#define	_WORD_SARCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if (c) { \
		(s) = ((SWORD)(s)) >> ((c) - 1); \
		CPU_FLAGL = (BYTE)((s) & 1); /* C_FLAG */ \
		(s) = (WORD)(((SWORD)(s)) >> 1); \
		CPU_OV = 0; \
		CPU_FLAGL |= szpflag_w[(WORD)(s)]; \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_SARCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		(s) = ((SDWORD)(s)) >> ((c) - 1); \
		CPU_FLAGL = (BYTE)((s) & 1); /* C_FLAG */ \
		(s) = (DWORD)(((SDWORD)(s)) >> 1); \
		CPU_OV = 0; \
		if ((s) == 0) { \
			CPU_FLAGL |= Z_FLAG; \
		} \
		if ((s) & 0x80000000) { \
			CPU_FLAGL |= S_FLAG; \
		} \
		CPU_FLAGL |= (szpcflag[(BYTE)(s)] & P_FLAG); \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

/*
 * SHR
 */
#define	_BYTE_SHR1(d, s) \
do { \
	(d) = (s) >> 1; \
	CPU_OV = (s) & 0x80; \
	CPU_FLAGL = (BYTE)(szpcflag[(BYTE)(d)] | A_FLAG | ((s) & 1)); \
} while (/*CONSTCOND*/ 0)

#define	_WORD_SHR1(d, s) \
do { \
	(d) = (s) >> 1; \
	CPU_OV = (s) & 0x8000; \
	CPU_FLAGL = (BYTE)(szpflag_w[(WORD)(d)] | A_FLAG | ((s) & 1)); \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_SHR1(d, s) \
do { \
	(d) = (s) >> 1; \
	CPU_OV = (s) & 0x80000000; \
	CPU_FLAGL = (BYTE)(A_FLAG | ((s) & 1)); \
	if ((d) == 0) { \
		CPU_FLAGL |= Z_FLAG; \
	} \
	CPU_FLAGL |= (szpcflag[(BYTE)(d)] & P_FLAG); \
} while (/*CONSTCOND*/ 0)

#define	_BYTE_SHRCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if (c) { \
		if ((c) >= 0x10) { \
			(c) &= 7; \
			(c) |= 8; \
		} \
		(s) >>= (c) - 1; \
		CPU_FLAGL = (BYTE)((s) & 1); \
		(s) >>= 1; \
		CPU_OV = ((s) ^ ((s) >> 1)) & 0x40; \
		CPU_FLAGL |= (szpcflag[(BYTE)(s)] | A_FLAG); \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

#define	_WORD_SHRCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if (c) { \
		(c)--; \
		if (c) { \
			(s) >>= (c); \
			CPU_OV = 0; \
		} else { \
			CPU_OV = (s) & 0x8000; \
		} \
		CPU_FLAGL = (BYTE)((s) & 1); \
		(s) >>= 1; \
		CPU_FLAGL |= szpflag_w[(WORD)(s)]; \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_SHRCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if (c) { \
		(c)--; \
		if (c) { \
			(s) >>= (c); \
			CPU_OV = 0; \
		} else { \
			CPU_OV = (s) & 0x80000000; \
		} \
		CPU_FLAGL = (BYTE)((s) & 1); \
		(s) >>= 1; \
		if ((s) == 0) { \
			CPU_FLAGL |= Z_FLAG; \
		} \
		CPU_FLAGL |= (szpcflag[(BYTE)(s)] & P_FLAG); \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

/*
 * SHL
 */
#define	_BYTE_SHL1(d, s) \
do { \
	(d) = (s) << 1; \
	CPU_OV = ((s) ^ (d)) & 0x80; \
	CPU_FLAGL = szpcflag[(d) & 0x1ff] | A_FLAG; \
} while (/*CONSTCOND*/ 0)

#define	_WORD_SHL1(d, s) \
do { \
	(d) = (s) << 1; \
	CPU_OV = ((s) ^ (d)) & 0x8000; \
	CPU_FLAGL = (BYTE)(szpflag_w[(WORD)(d)] | A_FLAG | ((d) >> 16)); \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_SHL1(d, s) \
do { \
	(d) = (s) << 1; \
	CPU_OV = ((s) ^ (d)) & 0x80000000; \
	CPU_FLAGL = (BYTE)(A_FLAG | (szpcflag[(BYTE)(d)] & P_FLAG)); \
	if ((s) & 0x80000000) { \
		CPU_FLAGL |= C_FLAG; \
	} \
	if ((d) == 0) { \
		CPU_FLAGL |= Z_FLAG; \
	} \
	if ((d) & 0x80000000) { \
		CPU_FLAGL |= S_FLAG; \
	} \
} while (/*CONSTCOND*/ 0)

#define	_BYTE_SHLCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if (c) { \
		if ((c) >= 0x10) { \
			(c) &= 7; \
			(c) |= 8; \
		} \
		(s) <<= (c); \
		(s) &= 0x1ff; \
		CPU_FLAGL = (szpcflag[(s) & 0x1ff] | A_FLAG); \
		CPU_OV = ((s) ^ ((s) >> 1)) & 0x80; \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

#define	_WORD_SHLCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if (c) { \
		CPU_OV = 0; \
		if ((c) == 1) { \
			CPU_OV = ((s) + 0x4000) & 0x8000; \
		} \
		(s) <<= (c); \
		(s) &= 0x1ffff; \
		CPU_FLAGL = szpflag_w[(WORD)(s)] | A_FLAG; \
		CPU_FLAGL |= (BYTE)((s) >> 16); /* C_FLAG */ \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_SHLCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if (c) { \
		(c)--; \
		if ((c)) { \
			(s) <<= (c); \
			CPU_OV = 0; \
		} else { \
			CPU_OV = ((s) + 0x40000000) & 0x80000000; \
		} \
		CPU_FLAGL = A_FLAG; \
		if ((s) & 0x80000000) { \
			CPU_FLAGL |= C_FLAG; \
		} \
		(s) <<= 1; \
		if ((s) == 0) { \
			CPU_FLAGL |= Z_FLAG; \
		} \
		if ((s) & 0x80000000) { \
			CPU_FLAGL |= S_FLAG; \
		} \
		CPU_FLAGL |= (szpcflag[(BYTE)(s)] & P_FLAG); \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

/*
 * SHRD
 */
#define _WORD_SHRD(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		if ((c) < 16) { \
			CPU_FLAGL = (BYTE)(((d) >> ((c) - 1)) & 1); /*C_FLAG*/ \
			(d) |= (s) << 16; \
			(d) >>= (c); \
			(d) &= 0xffff; \
			CPU_FLAGL |= szpflag_w[(WORD)(d)] | A_FLAG; \
			CPU_OV = (d) & 0x8000; \
		} \
	} \
} while (/*CONSTCOND*/ 0)

#define _DWORD_SHRD(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		CPU_FLAGL = (BYTE)(((d) >> ((c) - 1)) & 1); /* C_FLAG */ \
		(d) >>= (c); \
		(d) |= (s) << (32 - (c)); \
		if ((d) == 0) { \
			CPU_FLAGL |= Z_FLAG; \
		} \
		if ((d) & 0x80000000) { \
			CPU_FLAGL |= S_FLAG; \
		} \
		CPU_FLAGL |= (szpcflag[(BYTE)(d)] & P_FLAG); \
		CPU_OV = (d) & 0x80000000; \
	} \
} while (/*CONSTCOND*/ 0)

/*
 * SHLD
 */
#define _WORD_SHLD(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		if ((c) < 16) { \
			CPU_FLAGL = (BYTE)(((d) >> (16 - (c))) & 1); /*C_FLAG*/\
			(d) = ((d) << 16) | (s); \
			(d) <<= (c); \
			(d) >>= 16; \
			CPU_FLAGL |= szpflag_w[(d)] | A_FLAG; \
			CPU_OV = (d) & 0x8000; \
		} \
	} \
} while (/*CONSTCOND*/ 0)

#define _DWORD_SHLD(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		CPU_FLAGL = (BYTE)(((d) >> (32 - (c))) & 1); /* C_FLAG */ \
		(d) <<= (c); \
		(d) |= ((s) >> (32 - (c))); \
		if ((d) == 0) { \
			CPU_FLAGL |= Z_FLAG; \
		} \
		if ((d) & 0x80000000) { \
			CPU_FLAGL |= S_FLAG; \
		} \
		CPU_FLAGL |= (szpcflag[(BYTE)(d)] & P_FLAG); \
		CPU_OV = (d) & 0x80000000; \
	} \
} while (/*CONSTCOND*/ 0)

/*
 * ROR
 */
#define _BYTE_ROR1(d, s) \
do { \
	DWORD tmp = (s) & 1; \
	(d) = (tmp << 7) + ((s) >> 1); \
	CPU_FLAGL &= ~C_FLAG; \
	CPU_FLAGL |= tmp; \
	CPU_OV = ((s) ^ (d)) & 0x80; \
} while (/*CONSTCOND*/ 0)

#define _WORD_ROR1(d, s) \
do { \
	DWORD tmp = (s) & 1; \
	(d) = (tmp << 15) + ((s) >> 1); \
	CPU_FLAGL &= ~C_FLAG; \
	CPU_FLAGL |= tmp; \
	CPU_OV = ((s) ^ (d)) & 0x8000; \
} while (/*CONSTCOND*/ 0)

#define _DWORD_ROR1(d, s) \
do { \
	DWORD tmp = (s) & 1; \
	(d) = (tmp << 31) + ((s) >> 1); \
	CPU_FLAGL &= ~C_FLAG; \
	CPU_FLAGL |= tmp; \
	CPU_OV = ((s) ^ (d)) & 0x80000000; \
} while (/*CONSTCOND*/ 0)

#define	_BYTE_RORCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		(c) = ((c) - 1) & 7; \
		if ((c)) { \
			(s) = ((s) >> (c)) | ((s) << (8 - (c))); \
			(s) &= 0xff; \
		} \
		_BYTE_ROR1(d, s); \
	} else { \
		(d) = (s); \
	} \
} while (/*CONSTCOND*/ 0)

#define	_WORD_RORCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		DWORD tmp; \
		(c)--; \
		if ((c)) { \
			(c) &= 0x0f; \
			(s) = ((s) >> (c)) | ((s) << (16 - (c))); \
			(s) &= 0xffff; \
			CPU_OV = 0; \
		} else { \
			CPU_OV = ((s) >> 15) ^ ((s) & 1); \
		} \
		tmp = (s) & 1; \
		(s) = (tmp << 15) + ((s) >> 1); \
		CPU_FLAGL &= ~C_FLAG; \
		CPU_FLAGL |= tmp; \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_RORCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		DWORD tmp; \
		(c)--; \
		if ((c)) { \
			(s) = ((s) >> (c)) | ((s) << (32 - (c))); \
			CPU_OV = 0; \
		} else { \
			CPU_OV = ((s) >> 31) ^ ((s) & 1); \
		} \
		tmp = (s) & 1; \
		(s) = (tmp << 31) + ((s) >> 1); \
		CPU_FLAGL &= ~C_FLAG; \
		CPU_FLAGL |= tmp; \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

/*
 * ROL
 */
#define	_BYTE_ROL1(d, s)	\
do { \
	DWORD tmp = (s) >> 7; /* C_FLAG */ \
	(d) = ((s) << 1) + tmp; \
	CPU_FLAGL &= ~C_FLAG; \
	CPU_FLAGL |= tmp; \
	CPU_OV = ((s) ^ (d)) & 0x80; \
} while (/*CONSTCOND*/ 0)

#define	_WORD_ROL1(d, s)	\
do { \
	DWORD tmp = (s) >> 15; /* C_FLAG */ \
	(d) = ((s) << 1) + tmp; \
	CPU_FLAGL &= ~C_FLAG; \
	CPU_FLAGL |= tmp; \
	CPU_OV = ((s) ^ (d)) & 0x8000; \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_ROL1(d, s)	\
do { \
	DWORD tmp = (s) >> 31; /* C_FLAG */ \
	(d) = ((s) << 1) + tmp; \
	CPU_FLAGL &= ~C_FLAG; \
	CPU_FLAGL |= tmp; \
	CPU_OV = ((s) ^ (d)) & 0x80000000; \
} while (/*CONSTCOND*/ 0)

#define	_BYTE_ROLCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		(c) = ((c) - 1) & 7; \
		if ((c)) { \
			(s) = ((s) << (c)) | ((s) >> (8 - (c))); \
			(s) &= 0xff; \
		} \
		_BYTE_ROL1(d, s); \
	} else { \
		(d) = (s); \
	} \
} while (/*CONSTCOND*/ 0)

#define	_WORD_ROLCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		DWORD tmp; \
		(c)--; \
		if ((c)) { \
			(c) &= 0x0f; \
			(s) = ((s) << (c)) | ((s) >> (16 - (c))); \
			(s) &= 0xffff; \
			CPU_OV = 0; \
		} else { \
			CPU_OV = ((s) + 0x4000) & 0x8000; \
		} \
		tmp = (s) >> 15; \
		(s) = ((s) << 1) + tmp; \
		CPU_FLAGL &= ~C_FLAG; \
		CPU_FLAGL |= tmp; \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_ROLCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		DWORD tmp; \
		(c)--; \
		if ((c)) { \
			(s) = ((s) << (c)) | ((s) >> (32 - (c))); \
			CPU_OV = 0; \
		} else { \
			CPU_OV = ((s) + 0x40000000) & 0x80000000; \
		} \
		tmp = (s) >> 31; \
		(s) = ((s) << 1) + tmp; \
		CPU_FLAGL &= ~C_FLAG; \
		CPU_FLAGL |= tmp; \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

/*
 * RCR
 */
#define	_BYTE_RCR1(d, s) \
do { \
	(d) = ((CPU_FLAGL & C_FLAG) << 7) + ((s) >> 1); \
	CPU_FLAGL &= ~C_FLAG; \
	CPU_FLAGL |= (s) & 1; \
	CPU_OV = ((s) ^ (d)) & 0x80; \
} while (/*CONSTCOND*/ 0)

#define	_WORD_RCR1(d, s) \
do { \
	(d) = ((CPU_FLAGL & C_FLAG) << 15) + ((s) >> 1); \
	CPU_FLAGL &= ~C_FLAG; \
	CPU_FLAGL |= (s) & 1; \
	CPU_OV = ((s) ^ (d)) & 0x8000; \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_RCR1(d, s) \
do { \
	(d) = ((CPU_FLAGL & C_FLAG) << 31) + ((s) >> 1); \
	CPU_FLAGL &= ~C_FLAG; \
	CPU_FLAGL |= (s) & 1; \
	CPU_OV = ((s) ^ (d)) & 0x80000000; \
} while (/*CONSTCOND*/ 0)

#define	_BYTE_RCRCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		DWORD tmp = CPU_FLAGL & C_FLAG; \
		CPU_FLAGL &= ~C_FLAG; \
		while ((c)--) { \
			(s) |= (tmp << 8); \
			tmp = (s) & 1; \
			(s) >>= 1; \
		} \
		CPU_OV = ((s) ^ ((s) >> 1)) & 0x40; \
		CPU_FLAGL |= tmp; \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

#define	_WORD_RCRCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		DWORD tmp = CPU_FLAGL & C_FLAG; \
		CPU_FLAGL &= ~C_FLAG; \
		CPU_OV = 0; \
		if ((c) == 1) { \
			CPU_OV = ((s) >> 15) ^ tmp; \
		} \
		while ((c)--) { \
			(s) |= (tmp << 16); \
			tmp = (s) & 1; \
			(s) >>= 1; \
		} \
		CPU_FLAGL |= tmp; \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_RCRCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		DWORD tmp = CPU_FLAGL & C_FLAG; \
		CPU_FLAGL &= ~C_FLAG; \
		CPU_OV = 0; \
		if ((c) == 1) { \
			CPU_OV = ((s) >> 31) ^ tmp; \
		} \
		while ((c)--) { \
			tmp = (s) & 1; \
			(s) = (tmp << 31) | ((s) >> 1); \
		} \
		CPU_FLAGL |= tmp; \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

/*
 * RCL
 */
#define _BYTE_RCL1(d, s) \
do { \
	(d) = ((s) << 1) | (CPU_FLAGL & C_FLAG); \
	CPU_FLAGL &= ~C_FLAG; \
	CPU_FLAGL |= (s) >> 7; /* C_FLAG */ \
	CPU_OV = ((s) ^ (d)) & 0x80; \
} while (/*CONSTCOND*/ 0)

#define _WORD_RCL1(d, s) \
do { \
	(d) = ((s) << 1) | (CPU_FLAGL & C_FLAG); \
	CPU_FLAGL &= ~C_FLAG; \
	CPU_FLAGL |= (s) >> 15; /* C_FLAG */ \
	CPU_OV = ((s) ^ (d)) & 0x8000; \
} while (/*CONSTCOND*/ 0)

#define _DWORD_RCL1(d, s) \
do { \
	(d) = ((s) << 1) | (CPU_FLAGL & C_FLAG); \
	CPU_FLAGL &= ~C_FLAG; \
	CPU_FLAGL |= (s) >> 31; /* C_FLAG */ \
	CPU_OV = ((s) ^ (d)) & 0x80000000; \
} while (/*CONSTCOND*/ 0)

#define	_BYTE_RCLCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		DWORD tmp = CPU_FLAGL & C_FLAG; \
		CPU_FLAGL &= ~C_FLAG; \
		while ((c)--) { \
			(s) = (((s) << 1) | tmp) & 0x1ff; \
			tmp = (s) >> 8; \
		} \
		CPU_OV = ((s) ^ ((s) >> 1)) & 0x80; \
		CPU_FLAGL |= tmp; \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

#define	_WORD_RCLCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		DWORD tmp = CPU_FLAGL & C_FLAG; \
		CPU_FLAGL &= ~C_FLAG; \
		CPU_OV = 0; \
		if ((c) == 1) { \
			CPU_OV = ((s) + 0x4000) & 0x8000; \
		} \
		while ((c)--) { \
			(s) = (((s) << 1) | tmp) & 0x1ffff; \
			tmp = (s) >> 16; \
		} \
		CPU_FLAGL |= tmp; \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

#define	_DWORD_RCLCL(d, s, c) \
do { \
	(c) &= 0x1f; \
	if ((c)) { \
		DWORD tmp = CPU_FLAGL & C_FLAG; \
		CPU_FLAGL &= ~C_FLAG; \
		CPU_OV = 0; \
		if ((c) == 1) { \
			CPU_OV = ((s) + 0x40000000) & 0x80000000; \
		} \
		while ((c)--) { \
			tmp = (s) & 0x80000000; \
			(s) = ((s) << 1) | (tmp & 1); \
			tmp >>= 31; \
		} \
		CPU_FLAGL |= tmp; \
	} \
	(d) = (s); \
} while (/*CONSTCOND*/ 0)

#if defined(IA32_CROSS_CHECK) && defined(__GNUC__) && (defined(i386) || defined(i386))

#include "shift_rotatexc.mcr"

#define	BYTE_SAR1(d, s)		XC_BYTE_SAR1(d, s)
#define	WORD_SAR1(d, s)		XC_WORD_SAR1(d, s)
#define	DWORD_SAR1(d, s)	XC_DWORD_SAR1(d, s)
#define	BYTE_SARCL(d, s, c)	XC_BYTE_SARCL(d, s, c)
#define	WORD_SARCL(d, s, c)	XC_WORD_SARCL(d, s, c)
#define	DWORD_SARCL(d, s, c)	XC_DWORD_SARCL(d, s, c)
#define	BYTE_SHR1(d, s)		XC_BYTE_SHR1(d, s)
#define	WORD_SHR1(d, s)		XC_WORD_SHR1(d, s)
#define	DWORD_SHR1(d, s)	XC_DWORD_SHR1(d, s)
#define	BYTE_SHRCL(d, s, c)	XC_BYTE_SHRCL(d, s, c)
#define	WORD_SHRCL(d, s, c)	XC_WORD_SHRCL(d, s, c)
#define	DWORD_SHRCL(d, s, c)	XC_DWORD_SHRCL(d, s, c)
#define	BYTE_SHL1(d, s)		XC_BYTE_SHL1(d, s)
#define	WORD_SHL1(d, s)		XC_WORD_SHL1(d, s)
#define	DWORD_SHL1(d, s)	XC_DWORD_SHL1(d, s)
#define	BYTE_SHLCL(d, s, c)	XC_BYTE_SHLCL(d, s, c)
#define	WORD_SHLCL(d, s, c)	XC_WORD_SHLCL(d, s, c)
#define	DWORD_SHLCL(d, s, c)	XC_DWORD_SHLCL(d, s, c)
#define	WORD_SHRD(d, s, c)	XC_WORD_SHRD(d, s, c)
#define	DWORD_SHRD(d, s, c)	XC_DWORD_SHRD(d, s, c)
#define	WORD_SHLD(d, s, c)	XC_WORD_SHLD(d, s, c)
#define	DWORD_SHLD(d, s, c)	XC_DWORD_SHLD(d, s, c)
#define	BYTE_ROR1(d, s)		XC_BYTE_ROR1(d, s)
#define	WORD_ROR1(d, s)		XC_WORD_ROR1(d, s)
#define	DWORD_ROR1(d, s)	XC_DWORD_ROR1(d, s)
#define	BYTE_RORCL(d, s, c)	XC_BYTE_RORCL(d, s, c)
#define	WORD_RORCL(d, s, c)	XC_WORD_RORCL(d, s, c)
#define	DWORD_RORCL(d, s, c)	XC_DWORD_RORCL(d, s, c)
#define	BYTE_ROL1(d, s)		XC_BYTE_ROL1(d, s)
#define	WORD_ROL1(d, s)		XC_WORD_ROL1(d, s)
#define	DWORD_ROL1(d, s)	XC_DWORD_ROL1(d, s)
#define	BYTE_ROLCL(d, s, c)	XC_BYTE_ROLCL(d, s, c)
#define	WORD_ROLCL(d, s, c)	XC_WORD_ROLCL(d, s, c)
#define	DWORD_ROLCL(d, s, c)	XC_DWORD_ROLCL(d, s, c)
#define	BYTE_RCR1(d, s)		XC_BYTE_RCR1(d, s)
#define	WORD_RCR1(d, s)		XC_WORD_RCR1(d, s)
#define	DWORD_RCR1(d, s)	XC_DWORD_RCR1(d, s)
#define	BYTE_RCRCL(d, s, c)	XC_BYTE_RCRCL(d, s, c)
#define	WORD_RCRCL(d, s, c)	XC_WORD_RCRCL(d, s, c)
#define	DWORD_RCRCL(d, s, c)	XC_DWORD_RCRCL(d, s, c)
#define	BYTE_RCL1(d, s)		XC_BYTE_RCL1(d, s)
#define	WORD_RCL1(d, s)		XC_WORD_RCL1(d, s)
#define	DWORD_RCL1(d, s)	XC_DWORD_RCL1(d, s)
#define	BYTE_RCLCL(d, s, c)	XC_BYTE_RCLCL(d, s, c)
#define	WORD_RCLCL(d, s, c)	XC_WORD_RCLCL(d, s, c)
#define	DWORD_RCLCL(d, s, c)	XC_DWORD_RCLCL(d, s, c)

#else	/* !(IA32_CROSS_CHECK && __GNUC__ && (i386) || __i386__)) */

#define	BYTE_SAR1(d, s)		_BYTE_SAR1(d, s)
#define	WORD_SAR1(d, s)		_WORD_SAR1(d, s)
#define	DWORD_SAR1(d, s)	_DWORD_SAR1(d, s)
#define	BYTE_SARCL(d, s, c)	_BYTE_SARCL(d, s, c)
#define	WORD_SARCL(d, s, c)	_WORD_SARCL(d, s, c)
#define	DWORD_SARCL(d, s, c)	_DWORD_SARCL(d, s, c)
#define	BYTE_SHR1(d, s)		_BYTE_SHR1(d, s)
#define	WORD_SHR1(d, s)		_WORD_SHR1(d, s)
#define	DWORD_SHR1(d, s)	_DWORD_SHR1(d, s)
#define	BYTE_SHRCL(d, s, c)	_BYTE_SHRCL(d, s, c)
#define	WORD_SHRCL(d, s, c)	_WORD_SHRCL(d, s, c)
#define	DWORD_SHRCL(d, s, c)	_DWORD_SHRCL(d, s, c)
#define	BYTE_SHL1(d, s)		_BYTE_SHL1(d, s)
#define	WORD_SHL1(d, s)		_WORD_SHL1(d, s)
#define	DWORD_SHL1(d, s)	_DWORD_SHL1(d, s)
#define	BYTE_SHLCL(d, s, c)	_BYTE_SHLCL(d, s, c)
#define	WORD_SHLCL(d, s, c)	_WORD_SHLCL(d, s, c)
#define	DWORD_SHLCL(d, s, c)	_DWORD_SHLCL(d, s, c)
#define	WORD_SHRD(d, s, c)	_WORD_SHRD(d, s, c)
#define	DWORD_SHRD(d, s, c)	_DWORD_SHRD(d, s, c)
#define	WORD_SHLD(d, s, c)	_WORD_SHLD(d, s, c)
#define	DWORD_SHLD(d, s, c)	_DWORD_SHLD(d, s, c)
#define	BYTE_ROR1(d, s)		_BYTE_ROR1(d, s)
#define	WORD_ROR1(d, s)		_WORD_ROR1(d, s)
#define	DWORD_ROR1(d, s)	_DWORD_ROR1(d, s)
#define	BYTE_RORCL(d, s, c)	_BYTE_RORCL(d, s, c)
#define	WORD_RORCL(d, s, c)	_WORD_RORCL(d, s, c)
#define	DWORD_RORCL(d, s, c)	_DWORD_RORCL(d, s, c)
#define	BYTE_ROL1(d, s)		_BYTE_ROL1(d, s)
#define	WORD_ROL1(d, s)		_WORD_ROL1(d, s)
#define	DWORD_ROL1(d, s)	_DWORD_ROL1(d, s)
#define	BYTE_ROLCL(d, s, c)	_BYTE_ROLCL(d, s, c)
#define	WORD_ROLCL(d, s, c)	_WORD_ROLCL(d, s, c)
#define	DWORD_ROLCL(d, s, c)	_DWORD_ROLCL(d, s, c)
#define	BYTE_RCR1(d, s)		_BYTE_RCR1(d, s)
#define	WORD_RCR1(d, s)		_WORD_RCR1(d, s)
#define	DWORD_RCR1(d, s)	_DWORD_RCR1(d, s)
#define	BYTE_RCRCL(d, s, c)	_BYTE_RCRCL(d, s, c)
#define	WORD_RCRCL(d, s, c)	_WORD_RCRCL(d, s, c)
#define	DWORD_RCRCL(d, s, c)	_DWORD_RCRCL(d, s, c)
#define	BYTE_RCL1(d, s)		_BYTE_RCL1(d, s)
#define	WORD_RCL1(d, s)		_WORD_RCL1(d, s)
#define	DWORD_RCL1(d, s)	_DWORD_RCL1(d, s)
#define	BYTE_RCLCL(d, s, c)	_BYTE_RCLCL(d, s, c)
#define	WORD_RCLCL(d, s, c)	_WORD_RCLCL(d, s, c)
#define	DWORD_RCLCL(d, s, c)	_DWORD_RCLCL(d, s, c)

#endif	/* IA32_CROSS_CHECK && __GNUC__ && (i386) || __i386__) */

#endif	/* IA32_CPU_SHIFT_ROTATE_MCR__ */

/*	$Id: bit_byte.c,v 1.1 2003/12/08 00:55:32 yui Exp $	*/

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

#include "bit_byte.h"


/*
 * BT
 */
void
BT_EwGw(void)
{
	DWORD op, src, dst, madr, ad;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		dst = *(reg16_b20[op]);
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		ad += 2 * (src >> 4);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, ad);
	}
	if (dst & (1 << (src & 0x0f))) {
		CPU_FLAGL |= C_FLAG;
	} else {
		CPU_FLAGL &= ~C_FLAG;
	}
}

void
BT_EdGd(void)
{
	DWORD op, src, dst, madr, ad;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		dst = *(reg32_b20[op]);
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		ad += 4 * (src >> 5);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, ad);
	}
	if (dst & (1 << (src & 0x1f))) {
		CPU_FLAGL |= C_FLAG;
	} else {
		CPU_FLAGL &= ~C_FLAG;
	}
}

void
BT_EwIb(DWORD op)
{
	DWORD src, dst, madr, ad;

	GET_PCBYTE(src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		dst = *(reg16_b20[op]);
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		ad += 2 * (src >> 4);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, ad);
	}
	if (dst & (1 << (src & 0x0f))) {
		CPU_FLAGL |= C_FLAG;
	} else {
		CPU_FLAGL &= ~C_FLAG;
	}
}

void
BT_EdIb(DWORD op)
{
	DWORD src, dst, madr, ad;

	GET_PCBYTE(src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		dst = *(reg32_b20[op]);
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		ad += 4 * (src >> 5);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, ad);
	}
	if (dst & (1 << (src & 0x1f))) {
		CPU_FLAGL |= C_FLAG;
	} else {
		CPU_FLAGL &= ~C_FLAG;
	}
}

/*
 * BTS
 */
void
BTS_EwGw(void)
{
	WORD *out;
	DWORD op, src, dst, madr, ad;
	WORD bit;

	PREPART_EA_REG16(op, src);
	bit = 1 << (src & 0x0f);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		if (*out & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			*out |= bit;
			CPU_FLAGL &= ~C_FLAG;
		}
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		ad += 2 * (src >> 4);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, ad);
		if (dst & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			dst |= bit;
			cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, ad, dst);
			CPU_FLAGL &= ~C_FLAG;
		}
	}
}

void
BTS_EdGd(void)
{
	DWORD *out;
	DWORD op, src, dst, madr, ad;
	DWORD bit;

	PREPART_EA_REG32(op, src);
	bit = 1 << (src & 0x1f);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		if (*out & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			*out |= bit;
			CPU_FLAGL &= ~C_FLAG;
		}
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		ad += 4 * (src >> 5);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, ad);
		if (dst & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			dst |= bit;
			cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, ad, dst);
			CPU_FLAGL &= ~C_FLAG;
		}
	}
}

void
BTS_EwIb(DWORD op)
{
	WORD *out;
	DWORD src, dst, madr, ad;
	WORD bit;

	GET_PCBYTE(src);
	bit = 1 << (src & 0x0f);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		if (*out & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			*out |= bit;
			CPU_FLAGL &= ~C_FLAG;
		}
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		ad += 2 * (src >> 4);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, ad);
		if (dst & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			dst |= bit;
			cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, ad, dst);
			CPU_FLAGL &= ~C_FLAG;
		}
	}
}

void
BTS_EdIb(DWORD op)
{
	DWORD *out;
	DWORD src, dst, madr, ad;
	DWORD bit;

	GET_PCBYTE(src);
	bit = 1 << (src & 0x1f);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		if (*out & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			*out |= bit;
			CPU_FLAGL &= ~C_FLAG;
		}
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		ad += 4 * (src >> 5);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, ad);
		if (dst & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			dst |= bit;
			cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, ad, dst);
			CPU_FLAGL &= ~C_FLAG;
		}
	}
}

/*
 * BTR
 */
void
BTR_EwGw(void)
{
	WORD *out;
	DWORD op, src, dst, madr, ad;
	WORD bit;

	PREPART_EA_REG16(op, src);
	bit = 1 << (src & 0x0f);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		if (*out & bit) {
			*out &= ~bit;
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		ad += 2 * (src >> 4);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, ad);
		if (dst & bit) {
			dst &= ~bit;
			cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, ad, dst);
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
	}
}

void
BTR_EdGd(void)
{
	DWORD *out;
	DWORD op, src, dst, madr, ad;
	DWORD bit;

	PREPART_EA_REG32(op, src);
	bit = 1 << (src & 0x1f);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		dst = *out;
		if (*out & bit) {
			*out &= ~bit;
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		ad += 4 * (src >> 5);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, ad);
		if (dst & bit) {
			dst &= ~bit;
			cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, ad, dst);
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
	}
}

void
BTR_EwIb(DWORD op)
{
	WORD *out;
	DWORD src, dst, madr, ad;
	WORD bit;

	GET_PCBYTE(src);
	bit = 1 << (src & 0x0f);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		if (*out & bit) {
			*out &= ~bit;
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		ad += 2 * (src >> 4);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, ad);
		if (dst & bit) {
			dst &= ~bit;
			cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, ad, dst);
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
	}
}

void
BTR_EdIb(DWORD op)
{
	DWORD *out;
	DWORD src, dst, madr, ad;
	DWORD bit;

	GET_PCBYTE(src);
	bit = 1 << (src & 0x1f);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		if (*out & bit) {
			*out &= ~bit;
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		ad += 4 * (src >> 5);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, ad);
		if (dst & bit) {
			dst &= ~bit;
			cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, ad, dst);
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
	}
}


/*
 * BTC
 */
void
BTC_EwGw(void)
{
	WORD *out;
	DWORD op, src, dst, madr, ad;
	WORD bit;

	PREPART_EA_REG16(op, src);
	bit = 1 << (src & 0x0f);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		if (*out & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
		*out ^= bit;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		ad += 2 * (src >> 4);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, ad);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, ad, dst ^ bit);
		if (dst & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
	}
}

void
BTC_EdGd(void)
{
	DWORD *out;
	DWORD op, src, dst, madr, ad;
	DWORD bit;

	PREPART_EA_REG32(op, src);
	bit = 1 << (src & 0x1f);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		if (*out & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
		*out ^= bit;
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		ad += 4 * (src >> 5);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, ad);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, ad, dst ^ bit);
		if (dst & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
	}
}

void
BTC_EwIb(DWORD op)
{
	WORD *out;
	DWORD src, dst, madr, ad;
	WORD bit;

	GET_PCBYTE(src);
	bit = 1 << (src & 0x0f);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg16_b20[op];
		if (*out & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
		*out ^= bit;
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
		ad += 2 * (src >> 4);
		dst = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, ad);
		cpu_vmemorywrite_w(CPU_INST_SEGREG_INDEX, ad, dst ^ bit);
		if (dst & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
	}
}

void
BTC_EdIb(DWORD op)
{
	DWORD *out;
	DWORD src, dst, madr, ad;
	DWORD bit;

	GET_PCBYTE(src);
	bit = 1 << (src & 0x1f);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		out = reg32_b20[op];
		if (*out & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
		*out ^= bit;
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		ad = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
		ad += 4 * (src >> 5);
		dst = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, ad);
		cpu_vmemorywrite_d(CPU_INST_SEGREG_INDEX, ad, dst ^ bit);
		if (dst & bit) {
			CPU_FLAGL |= C_FLAG;
		} else {
			CPU_FLAGL &= ~C_FLAG;
		}
	}
}

/*
 * BSF
 */
void
BSF_GwEw(void)
{
	WORD *out;
	DWORD op, src;
	int bit;

	PREPART_REG16_EA(op, src, out, 2, 7);
	if (src == 0) {
		CPU_FLAGL |= Z_FLAG;
		/* dest reg is undefined */
	} else {
		for (bit = 0; (bit < 16) && !(src & (1 << bit)); bit++)
			continue;
		*out = (WORD)bit;
		CPU_FLAGL &= ~Z_FLAG;
	}
}

void
BSF_GdEd(void)
{
	DWORD *out;
	DWORD op, src;
	int bit;

	PREPART_REG32_EA(op, src, out, 2, 7);
	if (src == 0) {
		CPU_FLAGL |= Z_FLAG;
		/* dest reg is undefined */
	} else {
		for (bit = 0; (bit < 32) && !(src & (1 << bit)); bit++)
			continue;
		*out = (DWORD)bit;
		CPU_FLAGL &= ~Z_FLAG;
	}
}

/*
 * BSR
 */
void
BSR_GwEw(void)
{
	WORD *out;
	DWORD op, src;
	int bit;

	PREPART_REG16_EA(op, src, out, 2, 7);
	if (src == 0) {
		CPU_FLAGL |= Z_FLAG;
		/* dest reg is undefined */
	} else {
		for (bit = 15; (bit >= 0) && !(src & (1 << bit)); bit--)
			continue;
		*out = (WORD)bit;
		CPU_FLAGL &= ~Z_FLAG;
	}
}

void
BSR_GdEd(void)
{
	DWORD *out;
	DWORD op, src;
	int bit;

	PREPART_REG32_EA(op, src, out, 2, 7);
	if (src == 0) {
		CPU_FLAGL |= Z_FLAG;
		/* dest reg is undefined */
	} else {
		for (bit = 31; (bit >= 0) && !(src & (1 << bit)); bit--)
			continue;
		*out = (DWORD)bit;
		CPU_FLAGL &= ~Z_FLAG;
	}
}

/*
 * SETcc
 */
void
SETO_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = CPU_OV ? 1 : 0;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

void
SETNO_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = CPU_OV ? 0 : 1;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

void
SETC_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = CPU_FLAGL & C_FLAG;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

void
SETNC_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = (CPU_FLAGL & C_FLAG) ? 0 : 1;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

void
SETZ_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = (CPU_FLAGL & Z_FLAG) ? 1 : 0;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

void
SETNZ_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = (CPU_FLAGL & Z_FLAG) ? 0 : 1;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

void
SETA_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = (CPU_FLAGL & (Z_FLAG|C_FLAG)) ? 0 : 1;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

void
SETNA_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = (CPU_FLAGL & (Z_FLAG|C_FLAG)) ? 1 : 0;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

void
SETS_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = (CPU_FLAGL & S_FLAG) ? 1 : 0;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

void
SETNS_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = (CPU_FLAGL & S_FLAG) ? 0 : 1;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

void
SETP_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = (CPU_FLAGL & P_FLAG) ? 1 : 0;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

void
SETNP_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = (CPU_FLAGL & P_FLAG) ? 0 : 1;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

void
SETL_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = (!CPU_OV == !(CPU_FLAGL & S_FLAG)) ? 1 : 0;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

void
SETNL_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = (!CPU_OV == !(CPU_FLAGL & S_FLAG)) ? 0 : 1;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

void
SETLE_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = ((CPU_FLAGL & Z_FLAG) || (!CPU_OV == !(CPU_FLAGL & S_FLAG))) ? 1 : 0;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

void
SETNLE_Eb(void)
{
	DWORD op, src, madr;
	BYTE v = ((CPU_FLAGL & Z_FLAG) || (!CPU_OV == !(CPU_FLAGL & S_FLAG))) ? 0 : 1;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		*(reg8_b20[op]) = v;
	} else {
		CPU_WORKCLOCK(3);
		madr = calc_ea_dst(op);
		cpu_vmemorywrite(CPU_INST_SEGREG_INDEX, madr, v);
	}
}

/*
 * TEST
 */
void
TEST_EbGb(void)
{
	DWORD op, src, tmp, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		tmp = *(reg8_b20[op]);
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		tmp = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	}
	ANDBYTE(tmp, src);
}

void
TEST_EwGw(void)
{
	DWORD op, src, tmp, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		tmp = *(reg16_b20[op]);
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		tmp = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	}
	ANDWORD(tmp, src);
}

void
TEST_EdGd(void)
{
	DWORD op, src, tmp, madr;

	PREPART_EA_REG32(op, src);
	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		tmp = *(reg32_b20[op]);
	} else {
		CPU_WORKCLOCK(7);
		madr = calc_ea_dst(op);
		tmp = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	}
	ANDDWORD(tmp, src);
}

void
TEST_ALIb(void)
{
	DWORD src, tmp;

	CPU_WORKCLOCK(3);
	tmp = CPU_AL;
	GET_PCBYTE(src);
	ANDBYTE(tmp, src);
}

void
TEST_AXIw(void)
{
	DWORD src, tmp;

	CPU_WORKCLOCK(3);
	tmp = CPU_AX;
	GET_PCWORD(src);
	ANDWORD(tmp, src);
}

void
TEST_EAXId(void)
{
	DWORD src, tmp;

	CPU_WORKCLOCK(3);
	tmp = CPU_EAX;
	GET_PCDWORD(src);
	ANDDWORD(tmp, src);
}

void
TEST_EbIb(DWORD op)
{
	DWORD src, tmp, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		tmp = *(reg8_b20[op]);
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		tmp = cpu_vmemoryread(CPU_INST_SEGREG_INDEX, madr);
	}
	GET_PCBYTE(src);
	ANDBYTE(tmp, src);
}

void
TEST_EwIw(DWORD op)
{
	DWORD src, tmp, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		tmp = *(reg16_b20[op]);
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		tmp = cpu_vmemoryread_w(CPU_INST_SEGREG_INDEX, madr);
	}
	GET_PCWORD(src);
	ANDWORD(tmp, src);
}

void
TEST_EdId(DWORD op)
{
	DWORD src, tmp, madr;

	if (op >= 0xc0) {
		CPU_WORKCLOCK(2);
		tmp = *(reg32_b20[op]);
	} else {
		CPU_WORKCLOCK(6);
		madr = calc_ea_dst(op);
		tmp = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, madr);
	}
	GET_PCDWORD(src);
	ANDDWORD(tmp, src);
}

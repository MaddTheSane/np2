#include	"compiler.h"
#include	"i286.h"
#include	"i286c.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"i286c.mcr"


#define	MAX_PREFIX		8

#define	NEXT_OPCODE													\
		if (nevent.remainclock < 1) {								\
			nevent.baseclock += (1 - nevent.remainclock);			\
			nevent.remainclock = 1;									\
		}

#define	REMAIN_ADJUST(c)											\
		if (nevent.remainclock != (c)) {							\
			nevent.baseclock += ((c) - nevent.remainclock);			\
			nevent.remainclock = (c);								\
		}


// ----

I286FN _reserved(void) {

	I286_CLOCK(23);								// ToDo
	INT_NUM(6, I286_IP - 1);
}

I286FN _add_ea_r8(void) {						// 00: add EA, REG8

	BYTE	*out;
	UINT	op;
	UINT	src;
	UINT	dst;
	UINT	res;
	UINT32	madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg8_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			dst = i286_memoryread(madr);
			ADDBYTE(res, dst, src);
			i286_memorywrite(madr, (BYTE)res);
			return;
		}
		out = mem + madr;
	}
	dst = *out;
	ADDBYTE(res, dst, src);
	*out = (BYTE)res;
}

I286FN _add_ea_r16(void) {						// 01: add EA, REG16

	UINT16	*out;
	UINT	op;
	UINT	src;
	UINT	dst;
	UINT	res;
	UINT32	madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg16_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			dst = i286_memoryread_w(madr);
			ADDWORD(res, dst, src);
			i286_memorywrite_w(madr, (WORD)res);
			return;
		}
		out = (UINT16 *)(mem + madr);
	}
	dst = *out;
	ADDWORD(res, dst, src);
	*out = (UINT16)res;
}

I286FN _add_r8_ea(void) {						// 02: add REG8, EA

	BYTE	*out;
	UINT	op;
	UINT	src;
	UINT	dst;
	UINT	res;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	ADDBYTE(res, dst, src);
	*out = (BYTE)res;
}

I286FN _add_r16_ea(void) {						// 03: add REG16, EA

	UINT16	*out;
	UINT	op;
	UINT	src;
	UINT	dst;
	UINT	res;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	ADDWORD(res, dst, src);
	*out = (UINT16)res;
}

I286FN _add_al_data8(void) {					// 04: add al, DATA8

	UINT	src;
	UINT	res;

	I286_CLOCK(3)
	GET_PCBYTE(src);
	ADDBYTE(res, I286_AL, src);
	I286_AL = (BYTE)res;
}

I286FN _add_ax_data16(void) {					// 05: add ax, DATA16

	UINT	src;
	UINT	res;

	I286_CLOCK(3)
	GET_PCWORD(src);
	ADDWORD(res, I286_AX, src);
	I286_AX = (WORD)res;
}

I286FN _push_es(void) {							// 06: push es

	REGPUSH(I286_ES, 3);
}

I286FN _pop_es(void) {							// 07: pop es

	REGPOP(I286_ES, 5)
	ES_BASE = (UINT32)I286_ES << 4;
}

I286FN _or_ea_r8(void) {						// 08: or EA, REG8

	BYTE	*out;
	DWORD	op, src, dst, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg8_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			dst = i286_memoryread(madr);
			ORBYTE(dst, src);
			i286_memorywrite(madr, (BYTE)dst);
			return;
		}
		out = mem + madr;
	}
	ORBYTE(*out, src);
}

I286FN _or_ea_r16(void) {							// 09: or EA, REG16

	WORD	*out;
	DWORD	op, src, dst, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg16_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			dst = i286_memoryread_w(madr);
			ORWORD(dst, src);
			i286_memorywrite_w(madr, (WORD)dst);
			return;
		}
		out = (WORD *)(mem + madr);
	}
	ORWORD(*out, src);
}

I286FN _or_r8_ea(void) {						// 0a: or REG8, EA

	BYTE	*out;
	DWORD	op, src;

	PREPART_REG8_EA(op, src, out, 2, 7);
	ORBYTE(*out, src);
}

I286FN _or_r16_ea(void) {						// 0b: or REG16, EA

	WORD	*out;
	DWORD	op, src;

	PREPART_REG16_EA(op, src, out, 2, 7);
	ORWORD(*out, src);
}

I286FN _or_al_data8(void) {						// 0c: or al, DATA8

	DWORD	src;

	I286_CLOCK(3)
	GET_PCBYTE(src);
	ORBYTE(I286_AL, src);
}

I286FN _or_ax_data16(void) {					// 0d: or ax, DATA16

	DWORD	src;

	I286_CLOCK(3)
	GET_PCWORD(src);
	ORWORD(I286_AX, src);
}

I286FN _push_cs(void) {							// 0e: push cs

	REGPUSH(I286_CS, 3);
}

I286FN _adc_ea_r8(void) {						// 10: adc EA, REG8

	BYTE	*out;
	DWORD	op, src, dst, res, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg8_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			dst = i286_memoryread(madr);
			ADCBYTE(res, dst, src);
			i286_memorywrite(madr, (BYTE)res);
			return;
		}
		out = mem + madr;
	}
	dst = *out;
	ADCBYTE(res, dst, src);
	*out = (BYTE)res;
}

I286FN _adc_ea_r16(void) {						// 11: adc EA, REG16

	WORD	*out;
	DWORD	op, src, dst, res, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg16_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			dst = i286_memoryread_w(madr);
			ADCWORD(res, dst, src);
			i286_memorywrite_w(madr, (WORD)res);
			return;
		}
		out = (WORD *)(mem + madr);
	}
	dst = *out;
	ADCWORD(res, dst, src);
	*out = (WORD)res;
}

I286FN _adc_r8_ea(void) {						// 12: adc REG8, EA

	BYTE	*out;
	DWORD	op, src, dst, res;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	ADCBYTE(res, dst, src);
	*out = (BYTE)res;
}

I286FN _adc_r16_ea(void) {						// 13: adc REG16, EA

	WORD	*out;
	DWORD	op, src, dst, res;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	ADCWORD(res, dst, src);
	*out = (WORD)res;
}

I286FN _adc_al_data8(void) {					// 14: adc al, DATA8

	DWORD	src, res;

	I286_CLOCK(3)
	GET_PCBYTE(src);
	ADCBYTE(res, I286_AL, src);
	I286_AL = (BYTE)res;
}

I286FN _adc_ax_data16(void) {					// 15: adc ax, DATA16

	DWORD	src, res;

	I286_CLOCK(3)
	GET_PCWORD(src);
	ADCWORD(res, I286_AX, src);
	I286_AX = (WORD)res;
}

I286FN _push_ss(void) {							// 16: push ss

	REGPUSH(I286_SS, 3);
}

I286FN _pop_ss(void) {							// 17: pop ss

	REGPOP(I286_SS, 5)
	SS_BASE = (DWORD)I286_SS << 4;
	SS_FIX = SS_BASE;
	NEXT_OPCODE
}

I286FN _sbb_ea_r8(void) {						// 18: sbb EA, REG8

	BYTE	*out;
	DWORD	op, src, dst, res, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg8_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			dst = i286_memoryread(madr);
			SBBBYTE(res, dst, src);
			i286_memorywrite(madr, (BYTE)res);
			return;
		}
		out = mem + madr;
	}
	dst = *out;
	SBBBYTE(res, dst, src);
	*out = (BYTE)res;
}

I286FN _sbb_ea_r16(void) {						// 19: sbb EA, REG16

	WORD	*out;
	DWORD	op, src, dst, res, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg16_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			dst = i286_memoryread_w(madr);
			SBBWORD(res, dst, src);
			i286_memorywrite_w(madr, (WORD)res);
			return;
		}
		out = (WORD *)(mem + madr);
	}
	dst = *out;
	SBBWORD(res, dst, src);
	*out = (WORD)res;
}

I286FN _sbb_r8_ea(void) {						// 1a: sbb REG8, EA

	BYTE	*out;
	DWORD	op, src, dst, res;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	SBBBYTE(res, dst, src);
	*out = (BYTE)res;
}

I286FN _sbb_r16_ea(void) {						// 1b: sbb REG16, EA

	WORD	*out;
	DWORD	op, src, dst, res;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	SBBWORD(res, dst, src);
	*out = (WORD)res;
}

I286FN _sbb_al_data8(void) {					// 1c: adc al, DATA8

	DWORD	src, res;

	I286_CLOCK(3)
	GET_PCBYTE(src);
	SBBBYTE(res, I286_AL, src);
	I286_AL = (BYTE)res;
}

I286FN _sbb_ax_data16(void) {					// 1d: adc ax, DATA16

	DWORD	src, res;

	I286_CLOCK(3)
	GET_PCWORD(src);
	SBBWORD(res, I286_AX, src);
	I286_AX = (WORD)res;
}

I286FN _push_ds(void) {							// 1e: push ds

	REGPUSH(I286_DS, 3);
}

I286FN _pop_ds(void) {							// 1f: pop ds

	REGPOP(I286_DS, 5)
	DS_BASE = (DWORD)I286_DS << 4;
	DS_FIX = DS_BASE;
}

I286FN _and_ea_r8(void) {						// 20: and EA, REG8

	BYTE	*out;
	DWORD	op, src, dst, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg8_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			dst = i286_memoryread(madr);
			ANDBYTE(dst, src);
			i286_memorywrite(madr, (BYTE)dst);
			return;
		}
		out = mem + madr;
	}
	ANDBYTE(*out, src);
}

I286FN _and_ea_r16(void) {						// 21: and EA, REG16

	WORD	*out;
	DWORD	op, src, dst, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg16_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			dst = i286_memoryread_w(madr);
			ANDWORD(dst, src);
			i286_memorywrite_w(madr, (WORD)dst);
			return;
		}
		out = (WORD *)(mem + madr);
	}
	ANDWORD(*out, src);
}

I286FN _and_r8_ea(void) {						// 22: and REG8, EA

	BYTE	*out;
	DWORD	op, src;

	PREPART_REG8_EA(op, src, out, 2, 7);
	ANDBYTE(*out, src);
}

I286FN _and_r16_ea(void) {						// 23: and REG16, EA

	WORD	*out;
	DWORD	op, src;

	PREPART_REG16_EA(op, src, out, 2, 7);
	ANDWORD(*out, src);
}

I286FN _and_al_data8(void) {					// 24: and al, DATA8

	DWORD	src;

	I286_CLOCK(3)
	GET_PCBYTE(src);
	ANDBYTE(I286_AL, src);
}

I286FN _and_ax_data16(void) {					// 25: and ax, DATA16

	DWORD	src;

	I286_CLOCK(3)
	GET_PCWORD(src);
	ANDWORD(I286_AX, src);
}

I286FN _segprefix_es(void) {					// 26: es:

	SS_FIX = ES_BASE;
	DS_FIX = ES_BASE;
	i286s.prefix++;
	if (i286s.prefix < MAX_PREFIX) {
		DWORD	op;
		GET_PCBYTE(op);
		i286op[op]();
		REMOVE_PREFIX
		i286s.prefix = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286FN _daa(void) {								// 27: daa

	I286_CLOCK(3);
	I286_OV = ((I286_AL < 0x80) && 
				((I286_AL >= 0x7a) ||
				((I286_AL >= 0x1a) && (I286_FLAGL & C_FLAG))));
	if ((I286_FLAGL & A_FLAG) || ((I286_AL & 0x0f) > 9)) {
		I286_FLAGL |= A_FLAG;
		I286_FLAGL |= (BYTE)((I286_AL + 6) >> 8);
		I286_AL += 6;
	}
	if ((I286_FLAGL & C_FLAG) || (I286_AL > 0x9f)) {
		I286_FLAGL |= C_FLAG;
		I286_AL += 0x60;
	}
	I286_FLAGL &= A_FLAG | C_FLAG;
	I286_FLAGL |= szpcflag[I286_AL];
}

I286FN _sub_ea_r8(void) {						// 28: sub EA, REG8

	BYTE	*out;
	DWORD	op, src, dst, res, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg8_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			dst = i286_memoryread(madr);
			BYTE_SUB(res, dst, src);
			i286_memorywrite(madr, (BYTE)res);
			return;
		}
		out = mem + madr;
	}
	dst = *out;
	BYTE_SUB(res, dst, src);
	*out = (BYTE)res;
}

I286FN _sub_ea_r16(void) {						// 29: sub EA, REG16

	WORD	*out;
	DWORD	op, src, dst, res, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg16_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			dst = i286_memoryread_w(madr);
			WORD_SUB(res, dst, src);
			i286_memorywrite_w(madr, (WORD)res);
			return;
		}
		out = (WORD *)(mem + madr);
	}
	dst = *out;
	WORD_SUB(res, dst, src);
	*out = (WORD)res;
}

I286FN _sub_r8_ea(void) {						// 2a: sub REG8, EA

	BYTE	*out;
	DWORD	op, src, dst, res;

	PREPART_REG8_EA(op, src, out, 2, 7);
	dst = *out;
	BYTE_SUB(res, dst, src);
	*out = (BYTE)res;
}

I286FN _sub_r16_ea(void) {						// 2b: sub REG16, EA

	WORD	*out;
	DWORD	op, src, dst, res;

	PREPART_REG16_EA(op, src, out, 2, 7);
	dst = *out;
	WORD_SUB(res, dst, src);
	*out = (WORD)res;
}

I286FN _sub_al_data8(void) {					// 2c: sub al, DATA8

	DWORD	src, res;

	I286_CLOCK(3)
	GET_PCBYTE(src);
	BYTE_SUB(res, I286_AL, src);
	I286_AL = (BYTE)res;
}

I286FN _sub_ax_data16(void) {					// 2d: sub ax, DATA16

	DWORD	src, res;

	I286_CLOCK(3)
	GET_PCWORD(src);
	WORD_SUB(res, I286_AX, src);
	I286_AX = (WORD)res;
}

I286FN _segprefix_cs(void) {					// 2e: cs:

	SS_FIX = CS_BASE;
	DS_FIX = CS_BASE;
	i286s.prefix++;
	if (i286s.prefix < MAX_PREFIX) {
		DWORD	op;
		GET_PCBYTE(op);
		i286op[op]();
		REMOVE_PREFIX
		i286s.prefix = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286FN _das(void) {								// 2f: das

	I286_CLOCK(3)
	if ((I286_FLAGL & C_FLAG) || (I286_AL > 0x99)) {
		I286_FLAGL |= C_FLAG;
		I286_AL -= 0x60;
	}
	if ((I286_FLAGL & A_FLAG) || ((I286_AL & 0x0f) > 9)) {
		I286_FLAGL |= A_FLAG;
		I286_FLAGL |= (((DWORD)I286_AL - 6) >> 8) & 1;
		I286_AL -= 6;
	}
	I286_FLAGL &= A_FLAG | C_FLAG;
	I286_FLAGL |= szpcflag[I286_AL];
}

I286FN _xor_ea_r8(void) {						// 30: xor EA, REG8

	BYTE	*out;
	DWORD	op, src, dst, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg8_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			dst = i286_memoryread(madr);
			BYTE_XOR(dst, src);
			i286_memorywrite(madr, (BYTE)dst);
			return;
		}
		out = mem + madr;
	}
	BYTE_XOR(*out, src);
}

I286FN _xor_ea_r16(void) {						// 31: xor EA, REG16

	WORD	*out;
	DWORD	op, src, dst, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg16_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			dst = i286_memoryread_w(madr);
			WORD_XOR(dst, src);
			i286_memorywrite_w(madr, (WORD)dst);
			return;
		}
		out = (WORD *)(mem + madr);
	}
	WORD_XOR(*out, src);
}

I286FN _xor_r8_ea(void) {						// 32: xor REG8, EA

	BYTE	*out;
	DWORD	op, src;

	PREPART_REG8_EA(op, src, out, 2, 7);
	BYTE_XOR(*out, src);
}

I286FN _xor_r16_ea(void) {						// 33: or REG16, EA

	WORD	*out;
	DWORD	op, src;

	PREPART_REG16_EA(op, src, out, 2, 7);
	WORD_XOR(*out, src);
}

I286FN _xor_al_data8(void) {					// 34: or al, DATA8

	DWORD	src;

	I286_CLOCK(3)
	GET_PCBYTE(src);
	BYTE_XOR(I286_AL, src);
}

I286FN _xor_ax_data16(void) {					// 35: or ax, DATA16

	DWORD	src;

	I286_CLOCK(3)
	GET_PCWORD(src);
	WORD_XOR(I286_AX, src);
}

I286FN _segprefix_ss(void) {					// 36: cs:

	SS_FIX = SS_BASE;
	DS_FIX = SS_BASE;
	i286s.prefix++;
	if (i286s.prefix < MAX_PREFIX) {
		DWORD	op;
		GET_PCBYTE(op);
		i286op[op]();
		REMOVE_PREFIX
		i286s.prefix = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286FN _aaa(void) {								// 37: aaa

	I286_CLOCK(3)
	if ((I286_FLAGL & A_FLAG) || ((I286_AL & 0xf) > 9)) {
		I286_FLAGL |= A_FLAG | C_FLAG;
		I286_AX += 6;
		I286_AH++;
	}
	else {
		I286_FLAGL &= ~(A_FLAG | C_FLAG);
	}
}

I286FN _cmp_ea_r8(void) {						// 38: cmp EA, REG8

	DWORD	op, src, dst, res;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		dst = *(reg8_b20[op]);
		BYTE_SUB(res, dst, src);
	}
	else {
		I286_CLOCK(7);
		dst = i286_memoryread(c_calc_ea_dst[op]());
		BYTE_SUB(res, dst, src);
	}
}

I286FN _cmp_ea_r16(void) {						// 39: cmp EA, REG16

	DWORD	op, src, dst, res;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		dst = *(reg16_b20[op]);
		WORD_SUB(res, dst, src);
	}
	else {
		I286_CLOCK(7);
		dst = i286_memoryread_w(c_calc_ea_dst[op]());
		WORD_SUB(res, dst, src);
	}
}

I286FN _cmp_r8_ea(void) {						// 3a: cmp REG8, EA

	BYTE	*out;
	DWORD	op, src, dst, res;

	PREPART_REG8_EA(op, src, out, 2, 6);
	dst = *out;
	BYTE_SUB(res, dst, src);
}

I286FN _cmp_r16_ea(void) {						// 3b: cmp REG16, EA

	WORD	*out;
	DWORD	op, src, dst, res;

	PREPART_REG16_EA(op, src, out, 2, 6);
	dst = *out;
	WORD_SUB(res, dst, src);
}

I286FN _cmp_al_data8(void) {					// 3c: cmp al, DATA8

	DWORD	src, res;

	I286_CLOCK(3)
	GET_PCBYTE(src);
	BYTE_SUB(res, I286_AL, src);
}

I286FN _cmp_ax_data16(void) {					// 3d: cmp ax, DATA16

	DWORD	src, res;

	I286_CLOCK(3)
	GET_PCWORD(src);
	WORD_SUB(res, I286_AX, src);
}

I286FN _segprefix_ds(void) {					// 3e: ds:

	SS_FIX = DS_BASE;
	DS_FIX = DS_BASE;
	i286s.prefix++;
	if (i286s.prefix < MAX_PREFIX) {
		DWORD	op;
		GET_PCBYTE(op);
		i286op[op]();
		REMOVE_PREFIX
		i286s.prefix = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286FN _aas(void) {								// 3f: aas

	I286_CLOCK(3)
	if ((I286_FLAGL & A_FLAG) || ((I286_AL & 0xf) > 9)) {
		I286_FLAGL |= A_FLAG | C_FLAG;
		I286_AX -= 6;
		I286_AH--;
	}
	else {
		I286_FLAGL &= ~(A_FLAG | C_FLAG);
	}
}

I286_P _inc_ax(void) INCWORD(I286_AX, 2) 	// 40:	inc		ax
I286_P _inc_cx(void) INCWORD(I286_CX, 2)	// 41:	inc		cx
I286_P _inc_dx(void) INCWORD(I286_DX, 2)	// 42:	inc		dx
I286_P _inc_bx(void) INCWORD(I286_BX, 2)	// 43:	inc		bx
I286_P _inc_sp(void) INCWORD(I286_SP, 2)	// 44:	inc		sp
I286_P _inc_bp(void) INCWORD(I286_BP, 2)	// 45:	inc		bp
I286_P _inc_si(void) INCWORD(I286_SI, 2)	// 46:	inc		si
I286_P _inc_di(void) INCWORD(I286_DI, 2)	// 47:	inc		di
I286_P _dec_ax(void) DECWORD(I286_AX, 2)	// 48:	dec		ax
I286_P _dec_cx(void) DECWORD(I286_CX, 2)	// 49:	dec		cx
I286_P _dec_dx(void) DECWORD(I286_DX, 2)	// 4a:	dec		dx
I286_P _dec_bx(void) DECWORD(I286_BX, 2)	// 4b:	dec		bx
I286_P _dec_sp(void) DECWORD(I286_SP, 2)	// 4c:	dec		sp
I286_P _dec_bp(void) DECWORD(I286_BP, 2)	// 4d:	dec		bp
I286_P _dec_si(void) DECWORD(I286_SI, 2)	// 4e:	dec		si
I286_P _dec_di(void) DECWORD(I286_DI, 2)	// 4f:	dec		di

I286_P _push_ax(void) REGPUSH(I286_AX, 3)	// 50:	push	ax
I286_P _push_cx(void) REGPUSH(I286_CX, 3)	// 51:	push	cx
I286_P _push_dx(void) REGPUSH(I286_DX, 3)	// 52:	push	dx
I286_P _push_bx(void) REGPUSH(I286_BX, 3)	// 53:	push	bx
I286_P _push_sp(void) SP_PUSH(I286_SP, 3)	// 54:	push	sp
I286_P _push_bp(void) REGPUSH(I286_BP, 3)	// 55:	push	bp
I286_P _push_si(void) REGPUSH(I286_SI, 3)	// 56:	push	si
I286_P _push_di(void) REGPUSH(I286_DI, 3)	// 57:	push	di
I286_P _pop_ax(void) REGPOP(I286_AX, 5)		// 58:	pop		ax
I286_P _pop_cx(void) REGPOP(I286_CX, 5)		// 59:	pop		cx
I286_P _pop_dx(void) REGPOP(I286_DX, 5)		// 5A:	pop		dx
I286_P _pop_bx(void) REGPOP(I286_BX, 5)		// 5B:	pop		bx
I286_P _pop_sp(void) REGPOP(I286_SP, 5)		// 5C:	pop		sp
I286_P _pop_bp(void) REGPOP(I286_BP, 5)		// 5D:	pop		bp
I286_P _pop_si(void) REGPOP(I286_SI, 5)		// 5E:	pop		si
I286_P _pop_di(void) REGPOP(I286_DI, 5)		// 5F:	pop		di

I286_P _pusha(void) {						// 60:	pusha

	WORD	tmp = I286_SP;

	REGPUSH0(I286_AX)
	REGPUSH0(I286_CX)
	REGPUSH0(I286_DX)
	REGPUSH0(I286_BX)
    REGPUSH0(tmp)
	REGPUSH0(I286_BP)
	REGPUSH0(I286_SI)
	REGPUSH0(I286_DI)
	I286_CLOCK(17)
}

I286_P _popa(void) {						// 61:	popa

	REGPOP0(I286_DI);
	REGPOP0(I286_SI);
	REGPOP0(I286_BP);
	I286_SP += 2;
	REGPOP0(I286_BX);
	REGPOP0(I286_DX);
	REGPOP0(I286_CX);
	REGPOP0(I286_AX);
	I286_CLOCK(19)
}

I286_P _bound(void) {						// 62:	bound

	DWORD	vect = 0;
	DWORD	op;
	DWORD	madr;
	WORD	reg;

	I286_CLOCK(13);										// ToDo
	GET_PCBYTE(op);
	if (op < 0xc0) {
		reg = *(reg16_b53[op]);
		madr = c_calc_ea_dst[op]();
		if (reg >= i286_memoryread_w(madr)) {
			madr += 2;									// ToDo
			if (reg <= i286_memoryread_w(madr)) {
				return;
			}
		}
		vect = 5;
	}
	else {
		vect = 6;
	}
	INT_NUM(vect, I286_IP);
}

I286_P _arpl(void) {						// 63:	arpl

	DWORD	op;
	DWORD	tmp;

	GET_PCBYTE(op)
	tmp = ((op < 0xc0)?1:0);
	I286_IP += (BYTE)tmp;
	I286_CLOCK(tmp + 10);
	INT_NUM(6, I286_IP);
}

I286_P _push_data16(void) {				// 68:	push	DATA16

	WORD	tmp;

	GET_PCWORD(tmp)
	REGPUSH(tmp, 3)
}

I286_P _imul_reg_ea_data16(void) {		// 69:	imul	REG, EA, DATA16

	WORD	*out;
	DWORD	op;
	short	src, dst;
	long	res;

	PREPART_REG16_EA(op, src, out, 21, 24)
	GET_PCWORD(dst)
	WORD_IMUL(res, dst, src)
	*out = (WORD)res;
}

I286_P _push_data8(void) {				// 6A:	push	DATA8

	WORD	tmp;

	GET_PCBYTES(tmp)
	REGPUSH(tmp, 3)
}

I286_P _imul_reg_ea_data8(void) {		// 6B:	imul	REG, EA, DATA8

	WORD	*out;
	DWORD	op;
	short	src, dst;
	long	res;

	PREPART_REG16_EA(op, src, out, 21, 24)
	GET_PCBYTES(dst)
	WORD_IMUL(res, dst, src)
	*out = (WORD)res;
}

I286_P _insb(void) {						// 6C:	insb

	BYTE	dat;

	I286_CLOCK(5)
	dat = iocore_inp8(I286_DX);
	i286_memorywrite(I286_DI + ES_BASE, dat);
	I286_DI += STRING_DIR;
}

I286_P _insw(void) {						// 6D:	insw

	WORD	dat;

	I286_CLOCK(5)
	dat = iocore_inp16(I286_DX);
	i286_memorywrite_w(I286_DI + ES_BASE, dat);
	I286_DI += STRING_DIRx2;
}

I286_P _outsb(void) {						// 6E:	outsb

	BYTE	dat;

	I286_CLOCK(3)
	dat = i286_memoryread(I286_SI + DS_FIX);
	I286_SI += STRING_DIR;
	iocore_out8(I286_DX, dat);
}

I286_P _outsw(void) {						// 6F:	outsw

	WORD	dat;

	I286_CLOCK(3)
	dat = i286_memoryread_w(I286_SI + DS_FIX);
	I286_SI += STRING_DIRx2;
	iocore_out16(I286_DX, dat);
}

I286_P _jo_short(void) {					// 70:	jo short

	if (!I286_OV) JMPNOP(2) else JMPSHORT(7)
}

I286_P _jno_short(void) {					// 71:	jno short

	if (I286_OV) JMPNOP(2) else JMPSHORT(7)
}

I286_P _jc_short(void) {					// 72:	jnae/jb/jc short

	if (!(I286_FLAGL & C_FLAG)) JMPNOP(2) else JMPSHORT(7)
}

I286_P _jnc_short(void) {					// 73:	jae/jnb/jnc short

	if (I286_FLAGL & C_FLAG) JMPNOP(2) else JMPSHORT(7)
}

I286_P _jz_short(void) {					// 74:	je/jz short

	if (!(I286_FLAGL & Z_FLAG)) JMPNOP(2) else JMPSHORT(7)
}

I286_P _jnz_short(void) {					// 75:	jne/jnz short

	if (I286_FLAGL & Z_FLAG) JMPNOP(2) else JMPSHORT(7)
}

I286_P _jna_short(void) {					// 76:	jna/jbe short

	if (!(I286_FLAGL & (Z_FLAG | C_FLAG))) JMPNOP(2) else JMPSHORT(7)
}

I286_P _ja_short(void) {					// 77:	ja/jnbe short
	if (I286_FLAGL & (Z_FLAG | C_FLAG)) JMPNOP(2) else JMPSHORT(7)
}

I286_P _js_short(void) {					// 78:	js short

	if (!(I286_FLAGL & S_FLAG)) JMPNOP(2) else JMPSHORT(7)
}

I286_P _jns_short(void) {					// 79:	jns short

	if (I286_FLAGL & S_FLAG) JMPNOP(2) else JMPSHORT(7)
}

I286_P _jp_short(void) {					// 7A:	jp/jpe short

	if (!(I286_FLAGL & P_FLAG)) JMPNOP(2) else JMPSHORT(7)
}

I286_P _jnp_short(void) {					// 7B:	jnp/jpo short

	if (I286_FLAGL & P_FLAG) JMPNOP(2) else JMPSHORT(7)
}

I286_P _jl_short(void) {					// 7C:	jl/jnge short

	if (((I286_FLAGL & S_FLAG) == 0) == (I286_OV == 0))
												JMPNOP(2) else JMPSHORT(7)
}

I286_P _jnl_short(void) {					// 7D:	jnl/jge short

	if (((I286_FLAGL & S_FLAG) == 0) != (I286_OV == 0))
												JMPNOP(2) else JMPSHORT(7)
}

I286_P _jle_short(void) {					// 7E:	jle/jng short

	if ((!(I286_FLAGL & Z_FLAG)) &&
		(((I286_FLAGL & S_FLAG) == 0) == (I286_OV == 0)))
												JMPNOP(2) else JMPSHORT(7)
}

I286_P _jnle_short(void) {					// 7F:	jg/jnle short

	if ((I286_FLAGL & Z_FLAG) ||
		(((I286_FLAGL & S_FLAG) == 0) != (I286_OV == 0)))
												JMPNOP(2) else JMPSHORT(7)
}

I286_P _calc_ea8_i8(void) {					// 80:	op		EA8, DATA8
											// 82:	op		EA8, DATA8
	BYTE	*out;
	DWORD	op, madr;

	GET_PCBYTE(op)
	if (op >= 0xc0) {
		I286_CLOCK(3)
		out = reg8_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			c_op8xext8_table[(op >> 3) & 7](madr);
			return;
		}
		out = mem + madr;
	}
	c_op8xreg8_table[(op >> 3) & 7](out);
}

I286_P _calc_ea16_i16(void) {				// 81:	op		EA16, DATA16

	WORD	*out;
	DWORD	op, madr, src;

	GET_PCBYTE(op)
	if (op >= 0xc0) {
		I286_CLOCK(3)
		out = reg16_b20[op];
	}
	else {
		I286_CLOCK(7)
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			GET_PCWORD(src);
			c_op8xext16_table[(op >> 3) & 7](madr, src);
			return;
		}
		out = (WORD *)(mem + madr);
	}
	GET_PCWORD(src);
	c_op8xreg16_table[(op >> 3) & 7](out, src);
}

I286_P _calc_ea16_i8(void) {				// 83:	op		EA16, DATA8

	WORD	*out;
	DWORD	op, madr, src;

	GET_PCBYTE(op)
	if (op >= 0xc0) {
		I286_CLOCK(3)
		out = reg16_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			GET_PCBYTES(src);
			c_op8xext16_table[(op >> 3) & 7](madr, src);
			return;
		}
		out = (WORD *)(mem + madr);
	}
	GET_PCBYTES(src);
	c_op8xreg16_table[(op >> 3) & 7](out, src);
}

I286_P _test_ea_r8(void) {					// 84:	test	EA, REG8

	BYTE	*out;
	DWORD	op, src, tmp, madr;

	PREPART_EA_REG8(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg8_b20[op];
	}
	else {
		I286_CLOCK(6);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			tmp = i286_memoryread(madr);
			ANDBYTE(tmp, src);
			return;
		}
		out = mem + madr;
	}
	tmp = *out;
	ANDBYTE(tmp, src);
}

I286_P _test_ea_r16(void) {					// 85:	test	EA, REG16

	WORD	*out;
	DWORD	op, src, tmp, madr;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg16_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			tmp = i286_memoryread_w(madr);
			ANDWORD(tmp, src);
			return;
		}
		out = (WORD *)(mem + madr);
	}
	tmp = *out;
	ANDWORD(tmp, src);
}

I286_P _xchg_ea_r8(void) {					// 86:	xchg	EA, REG8

	BYTE	*out, *src;
	DWORD	op, madr;

	PREPART_EA_REG8P(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(3);
		out = reg8_b20[op];
	}
	else {
		I286_CLOCK(5);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			BYTE tmp = i286_memoryread(madr);
			i286_memorywrite(madr, *src);
			*src = tmp;
			return;
		}
		out = mem + madr;
	}
	SWAPBYTE(*out, *src);
}

I286_P _xchg_ea_r16(void) {					// 87:	xchg	EA, REG16

	WORD	*out, *src;
	DWORD	op, madr;

	PREPART_EA_REG16P(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(3);
		out = reg16_b20[op];
	}
	else {
		I286_CLOCK(5);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			WORD tmp = i286_memoryread_w(madr);
			i286_memorywrite_w(madr, *src);
			*src = tmp;
			return;
		}
		out = (WORD *)(mem + madr);
	}
	SWAPWORD(*out, *src);
}

I286_P _mov_ea_r8(void) {					// 88:	mov		EA, REG8

	BYTE	src;
	DWORD	op, madr;

	PREPART_EA_REG8(op, src)
	if (op >= 0xc0) {
		I286_CLOCK(2);
		*(reg8_b20[op]) = src;
	}
	else {
		I286_CLOCK(3);
		madr = c_calc_ea_dst[op]();
		i286_memorywrite(madr, src);
	}
}

I286_P _mov_ea_r16(void) {					// 89:	mov		EA, REG16

	WORD	src;
	DWORD	op;

	PREPART_EA_REG16(op, src);
	if (op >= 0xc0) {
		I286_CLOCK(2);
		*(reg16_b20[op]) = src;
	}
	else {
		I286_CLOCK(3);
		i286_memorywrite_w(c_calc_ea_dst[op](), src);
	}
}

I286_P _mov_r8_ea(void) {					// 8A:	mov		REG8, EA

	BYTE	*out;
	BYTE	src;
	DWORD	op;

	PREPART_REG8_EA(op, src, out, 2, 5);
	*out = src;
}

I286_P _mov_r16_ea(void) {					// 8B:	mov		REG16, EA

	WORD	*out;
	WORD	src;
	DWORD	op;

	PREPART_REG16_EA(op, src, out, 2, 5);
	*out = src;
}

I286_P _mov_ea_seg(void) {					// 8C:	mov		EA, segreg

	DWORD	op;
	WORD	tmp;

	GET_PCBYTE(op);
	tmp = *SEGMENTPTR((op >> 3) & 3);
	if (op >= 0xc0) {
		I286_CLOCK(2)
		*(reg16_b20[op]) = tmp;
	}
	else {
		I286_CLOCK(3)
		i286_memorywrite_w(c_calc_ea_dst[op](), tmp);
	}
}

I286_P _lea_r16_ea(void) {					// 8D:	lea		REG16, EA

	DWORD	op;

	I286_CLOCK(3)
	GET_PCBYTE(op)
	if (op < 0xc0) {
		*(reg16_b53[op]) = c_calc_lea[op]();
	}
	else {
		INT_NUM(6, I286_SP - 2);
	}
}

I286_P _mov_seg_ea(void) {					// 8E:	mov		segrem, EA

	DWORD	op;
	WORD	tmp;
	WORD	ipbak;

	ipbak = I286_IP;
	GET_PCBYTE(op);
	if (op >= 0xc0) {
		I286_CLOCK(2)
		tmp = *(reg16_b20[op]);
	}
	else {
		I286_CLOCK(5)
		tmp = i286_memoryread_w(c_calc_ea_dst[op]());
	}
	switch(op & 0x18) {
		case 0x00:			// es
			I286_ES = tmp;
			ES_BASE = (DWORD)tmp << 4;
			break;

		case 0x10:			// ss
			I286_SS = tmp;
			SS_BASE = (DWORD)tmp << 4;
			SS_FIX = SS_BASE;
			NEXT_OPCODE
			break;

		case 0x18:			// ds
			I286_DS = tmp;
			DS_BASE = (DWORD)tmp << 4;
			DS_FIX = DS_BASE;
			break;

		default:			// cs
			INT_NUM(6, ipbak - 1);
			break;
	}
}

I286_P _pop_ea(void) {						// 8F:	pop		EA

	DWORD	op;
	WORD	tmp;

	I286_CLOCK(5)
	REGPOP0(tmp)

	GET_PCBYTE(op)
	if (op < 0xc0) {
		i286_memorywrite_w(c_calc_ea_dst[op](), tmp);
	}
	else {
		*(reg16_b20[op]) = tmp;
	}
}

I286_P _nop(void) {							// 90: nop / bios func

#if 1										// call BIOS
	DWORD	adrs;

	adrs = ((I286_IP - 1) & 0xffff) + CS_BASE;
	if ((adrs >= 0xf8000) && (adrs < 0x100000)) {
		biosfunc(adrs);
		ES_BASE = (DWORD)I286_ES << 4;
		CS_BASE = (DWORD)I286_CS << 4;
		SS_BASE = (DWORD)I286_SS << 4;
		SS_FIX = SS_BASE;
		DS_BASE = (DWORD)I286_DS << 4;
		DS_FIX = DS_BASE;
	}
#endif
	I286_CLOCK(3)
}

I286_P _xchg_ax_cx(void) { 					// 91:	xchg	ax, cx

	I286_CLOCK(3);
	SWAPWORD(I286_AX, I286_CX);
}

I286_P _xchg_ax_dx(void) { 					// 92:	xchg	ax, dx

	I286_CLOCK(3);
	SWAPWORD(I286_AX, I286_DX);
}

I286_P _xchg_ax_bx(void) { 					// 93:	xchg	ax, bx

	I286_CLOCK(3);
	SWAPWORD(I286_AX, I286_BX);
}

I286_P _xchg_ax_sp(void) { 					// 94:	xchg	ax, sp

	I286_CLOCK(3);
	SWAPWORD(I286_AX, I286_SP);
}

I286_P _xchg_ax_bp(void) { 					// 95:	xchg	ax, bp

	I286_CLOCK(3);
	SWAPWORD(I286_AX, I286_BP);
}

I286_P _xchg_ax_si(void) { 					// 96:	xchg	ax, si

	I286_CLOCK(3);
	SWAPWORD(I286_AX, I286_SI);
}

I286_P _xchg_ax_di(void) { 					// 97:	xchg	ax, di

	I286_CLOCK(3);
	SWAPWORD(I286_AX, I286_DI);
}

I286_P _cbw(void) {							// 98:	cbw

	I286_CLOCK(2)
	I286_AX = __CBW(I286_AL);
}

I286_P _cwd(void) {							// 99:	cwd

	I286_CLOCK(2)
	I286_DX = ((I286_AH & 0x80)?0xffff:0x0000);
}

I286_P _call_far(void) {					// 9A:	call far

	WORD	newip;

	I286_CLOCK(13)
	REGPUSH0(I286_CS)
	GET_PCWORD(newip)
	GET_PCWORD(I286_CS)
	CS_BASE = (DWORD)I286_CS << 4;
	REGPUSH0(I286_IP)
	I286_IP = newip;
}

I286_P _wait(void) {						// 9B:	wait

	I286_CLOCK(2)
}

I286_P _pushf(void) {						// 9C:	pushf

	REGPUSH(REAL_FLAGREG, 3)
}

I286_P _popf(void) {						// 9D:	popf

	I286_CLOCK(5)
	REGPOP0(I286_FLAG)
	I286_OV = I286_FLAG & O_FLAG;
	I286_FLAG &= (0xfff ^ O_FLAG);
	I286_TRAP = ((I286_FLAG & 0x300) == 0x300);
	I286IRQCHECKTERM
}

I286_P _sahf(void) {						// 9E:	sahf

	I286_CLOCK(2)
	I286_FLAGL = I286_AH;
}

I286_P _lahf(void) {						// 9F:	lahf

	I286_CLOCK(2)
	I286_AH = I286_FLAGL;
}

I286_P _mov_al_m8(void) {					// A0:	mov		al, m8

	DWORD	op;

	I286_CLOCK(5)
	GET_PCWORD(op)
	I286_AL = i286_memoryread(DS_FIX + op);
}

I286_P _mov_ax_m16(void) {					// A1:	mov		ax, m16

	DWORD	op;

	I286_CLOCK(5)
	GET_PCWORD(op)
	I286_AX = i286_memoryread_w(DS_FIX + op);
}

I286_P _mov_m8_al(void) {					// A2:	mov		m8, al

	DWORD	op;

	I286_CLOCK(3)
	GET_PCWORD(op)
	i286_memorywrite(DS_FIX + op, I286_AL);
}

I286_P _mov_m16_ax(void) {					// A3:	mov		m16, ax

	DWORD	op;

	I286_CLOCK(3)
	GET_PCWORD(op);
	i286_memorywrite_w(DS_FIX + op, I286_AX);
}

I286_P _movsb(void) {						// A4:	movsb

	BYTE	tmp;

	I286_CLOCK(5)
	tmp = i286_memoryread(I286_SI + DS_FIX);
	i286_memorywrite(I286_DI + ES_BASE, tmp);
	I286_SI += STRING_DIR;
	I286_DI += STRING_DIR;
}

I286_P _movsw(void) {						// A5:	movsw

	WORD	tmp;

	I286_CLOCK(5)
	tmp = i286_memoryread_w(I286_SI + DS_FIX);
	i286_memorywrite_w(I286_DI + ES_BASE, tmp);
	I286_SI += STRING_DIRx2;
	I286_DI += STRING_DIRx2;
}

I286_P _cmpsb(void) {						// A6:	cmpsb

	DWORD	src, dst, res;

	I286_CLOCK(8)
	dst = i286_memoryread(I286_SI + DS_FIX);
	src = i286_memoryread(I286_DI + ES_BASE);
	BYTE_SUB(res, dst, src)
	I286_SI += STRING_DIR;
	I286_DI += STRING_DIR;
}

I286_P _cmpsw(void) {						// A7:	cmpsw

	DWORD	src, dst, res;

	I286_CLOCK(8)
	dst = i286_memoryread_w(I286_SI + DS_FIX);
	src = i286_memoryread_w(I286_DI + ES_BASE);
	WORD_SUB(res, dst, src)
	I286_SI += STRING_DIRx2;
	I286_DI += STRING_DIRx2;
}

I286_P _test_al_data8(void) {				// A8:	test	al, DATA8

	DWORD	src, dst;

	I286_CLOCK(3)
	GET_PCBYTE(src)
	dst = I286_AL;
	ANDBYTE(dst, src)
}

I286_P _test_ax_data16(void) {				// A9:	test	ax, DATA16

	DWORD	src, dst;

	I286_CLOCK(3)
	GET_PCWORD(src)
	dst = I286_AX;
	ANDWORD(dst, src)
}

I286_P _stosb(void) {						// AA:	stosw

	I286_CLOCK(3)
	i286_memorywrite(I286_DI + ES_BASE, I286_AL);
	I286_DI += STRING_DIR;
}

I286_P _stosw(void) {						// AB:	stosw

	I286_CLOCK(3)
	i286_memorywrite_w(I286_DI + ES_BASE, I286_AX);
	I286_DI += STRING_DIRx2;
}

I286_P _lodsb(void) {						// AC:	lodsb

	I286_CLOCK(5)
	I286_AL = i286_memoryread(I286_SI + DS_FIX);
	I286_SI += STRING_DIR;
}

I286_P _lodsw(void) {						// AD:	lodsw

	I286_CLOCK(5)
	I286_AX = i286_memoryread_w(I286_SI + DS_FIX);
	I286_SI += STRING_DIRx2;
}

I286_P _scasb(void) {						// AE:	scasb

	DWORD	src, dst, res;

	I286_CLOCK(7)
	src = i286_memoryread(I286_DI + ES_BASE);
	dst = I286_AL;
	BYTE_SUB(res, dst, src)
	I286_DI += STRING_DIR;
}

I286_P _scasw(void) {						// AF:	scasw

	DWORD	src, dst, res;

	I286_CLOCK(7)
	src = i286_memoryread_w(I286_DI + ES_BASE);
	dst = I286_AX;
	WORD_SUB(res, dst, src)
	I286_DI += STRING_DIRx2;
}

I286_P _mov_al_imm(void) MOVIMM8(I286_AL)	// B0:	mov		al, imm8
I286_P _mov_cl_imm(void) MOVIMM8(I286_CL)	// B1:	mov		cl, imm8
I286_P _mov_dl_imm(void) MOVIMM8(I286_DL)	// B2:	mov		dl, imm8
I286_P _mov_bl_imm(void) MOVIMM8(I286_BL)	// B3:	mov		bl, imm8
I286_P _mov_ah_imm(void) MOVIMM8(I286_AH)	// B4:	mov		ah, imm8
I286_P _mov_ch_imm(void) MOVIMM8(I286_CH)	// B5:	mov		ch, imm8
I286_P _mov_dh_imm(void) MOVIMM8(I286_DH)	// B6:	mov		dh, imm8
I286_P _mov_bh_imm(void) MOVIMM8(I286_BH)	// B7:	mov		bh, imm8
I286_P _mov_ax_imm(void) MOVIMM16(I286_AX)	// B8:	mov		ax, imm16
I286_P _mov_cx_imm(void) MOVIMM16(I286_CX)	// B9:	mov		cx, imm16
I286_P _mov_dx_imm(void) MOVIMM16(I286_DX)	// BA:	mov		dx, imm16
I286_P _mov_bx_imm(void) MOVIMM16(I286_BX)	// BB:	mov		bx, imm16
I286_P _mov_sp_imm(void) MOVIMM16(I286_SP)	// BC:	mov		sp, imm16
I286_P _mov_bp_imm(void) MOVIMM16(I286_BP)	// BD:	mov		bp, imm16
I286_P _mov_si_imm(void) MOVIMM16(I286_SI)	// BE:	mov		si, imm16
I286_P _mov_di_imm(void) MOVIMM16(I286_DI)	// BF:	mov		di, imm16

I286_P _shift_ea8_data8(void) {				// C0:	shift	EA8, DATA8

	BYTE	*out;
	DWORD	op, madr;
	BYTE	cl;

	GET_PCBYTE(op)
	if (op >= 0xc0) {
		I286_CLOCK(5)
		out = reg8_b20[op];
	}
	else {
		I286_CLOCK(8);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			GET_PCBYTE(cl)
			I286_CLOCK(cl);
			sft_e8cl_table[(op >> 3) & 7](madr, cl);
			return;
		}
		out = mem + madr;
	}
	GET_PCBYTE(cl)
	I286_CLOCK(cl);
	sft_r8cl_table[(op >> 3) & 7](out, cl);
}

I286_P _shift_ea16_data8(void) {			// C1:	shift	EA16, DATA8

	WORD	*out;
	DWORD	op, madr;
	BYTE	cl;

	GET_PCBYTE(op)
	if (op >= 0xc0) {
		I286_CLOCK(5)
		out = reg16_b20[op];
	}
	else {
		I286_CLOCK(8);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			GET_PCBYTE(cl);
			I286_CLOCK(cl);
			sft_e16cl_table[(op >> 3) & 7](madr, cl);
			return;
		}
		out = (WORD *)(mem + madr);
	}
	GET_PCBYTE(cl);
	I286_CLOCK(cl);
	sft_r16cl_table[(op >> 3) & 7](out, cl);
}

I286_P _ret_near_data16(void) {				// C2:	ret near DATA16

	WORD	ad;

	I286_CLOCK(11)
	GET_PCWORD(ad)
	REGPOP0(I286_IP)
	I286_SP += ad;
}

I286_P _ret_near(void) {					// C3:	ret near

	I286_CLOCK(11)
	REGPOP0(I286_IP)
}

I286_P _les_r16_ea(void) {					// C4:	les		REG16, EA

	DWORD	op;
	WORD	ad;

	I286_CLOCK(3)
	GET_PCBYTE(op)
	if (op < 0xc0) {
		ad = c_get_ea[op]();
		*(reg16_b53[op]) = i286_memoryread_w(ad + EA_FIX);
		ad += 2;
		I286_ES = i286_memoryread_w(ad + EA_FIX);
		ES_BASE = (DWORD)I286_ES << 4;
	}
	else {
		INT_NUM(6, I286_IP - 2);
	}
}

I286_P _lds_r16_ea(void) {					// C5:	lds		REG16, EA

	DWORD	op;
	WORD	ad;

	I286_CLOCK(3)
	GET_PCBYTE(op)
	if (op < 0xc0) {
		ad = c_get_ea[op]();
		*(reg16_b53[op]) = i286_memoryread_w(ad + EA_FIX);
		ad += 2;
		I286_DS = i286_memoryread_w(ad + EA_FIX);
		DS_BASE = (DWORD)I286_DS << 4;
		DS_FIX = DS_BASE;
	}
	else {
		INT_NUM(6, I286_IP - 2);
	}
}

I286_P _mov_ea8_data8(void) {				// C6:	mov		EA8, DATA8

	DWORD	op;

	GET_PCBYTE(op)
	if (op >= 0xc0) {
		I286_CLOCK(2)
		GET_PCBYTE(*(reg8_b53[op]))
	}
	else {
		WORD	ad;
		BYTE	val;
		I286_CLOCK(3)
		ad = c_get_ea[op]();
		GET_PCBYTE(val)
		i286_memorywrite(ad + EA_FIX, val);
	}
}

I286_P _mov_ea16_data16(void) {				// C7:	mov		EA16, DATA16

	DWORD	op;

	GET_PCBYTE(op)
	if (op >= 0xc0) {
		I286_CLOCK(2)
		GET_PCWORD(*(reg16_b53[op]))
	}
	else {
		WORD	ad;
		WORD	val;
		I286_CLOCK(3)
		ad = c_get_ea[op]();
		GET_PCWORD(val)
		i286_memorywrite_w(ad + EA_FIX, val);
	}
}

I286_P _enter(void) {						// C8:	enter	DATA16, DATA8

	WORD	dimsize;
	BYTE	level;

	GET_PCWORD(dimsize)
	GET_PCBYTE(level)
	REGPUSH0(I286_BP)
	level &= 0x1f;
	if (!level) {								// enter level=0
		I286_CLOCK(11)
		I286_BP = I286_SP;
		I286_SP -= dimsize;
	}
	else {
		level--;
		if (!level) {							// enter level=1
			WORD tmp;
			I286_CLOCK(15)
			tmp = I286_SP;
			REGPUSH0(tmp)
			I286_BP = tmp;
			I286_SP -= dimsize;
		}
		else {									// enter level=2-31
			WORD	bp;
			I286_CLOCK(12 + level*4)
			bp = I286_BP;
			I286_BP = I286_SP;
			while(level--) {
				WORD val = i286_memoryread_w(bp + SS_BASE);
				i286_memorywrite_w(I286_SP + SS_BASE, val);
				bp -= 2;
				I286_SP -= 2;
			}
			REGPUSH0(I286_BP)
			I286_SP -= dimsize;
		}
	}
}

I286_P leave(void) {						// C9:	leave

	I286_CLOCK(5)
	I286_SP = I286_BP;
	REGPOP0(I286_BP)
}

I286_P _ret_far_data16(void) {				// CA:	ret far	DATA16

	WORD	ad;

	I286_CLOCK(15)
	GET_PCWORD(ad)
	REGPOP0(I286_IP)
	REGPOP0(I286_CS)
	I286_SP += ad;
	CS_BASE = (DWORD)I286_CS << 4;
}

I286_P _ret_far(void) {						// CB:	ret far

	I286_CLOCK(15)
	REGPOP0(I286_IP)
	REGPOP0(I286_CS)
	CS_BASE = (DWORD)I286_CS << 4;
}

I286_P _int_03(void) {						// CC:	int		3

	I286_CLOCK(23)
	INT_NUM(3, I286_IP);
}

I286_P _int_data8(void) {					// CD:	int		DATA8

	BYTE vect;

	I286_CLOCK(23)
	GET_PCBYTE(vect)
	INT_NUM(vect, I286_IP);
}

I286_P _into(void) {						// CE:	into

	I286_CLOCK(4)
	if (I286_OV) {
		I286_CLOCK(24 - 4)
		INT_NUM(4, I286_IP);
	}
}

I286_P _iret(void) {						// CF:	iret

	extirq_pop();
	I286_CLOCK(31)
	REGPOP0(I286_IP)
	REGPOP0(I286_CS)
	REGPOP0(I286_FLAG)
	I286_OV = I286_FLAG & O_FLAG;
	I286_FLAG &= 0x7ff;
	I286_TRAP = ((I286_FLAG & 0x300) == 0x300);
	CS_BASE = (DWORD)I286_CS << 4;
	I286IRQCHECKTERM
}

I286_P _shift_ea8_1(void) {				// D0:	shift EA8, 1

	BYTE	*out;
	DWORD	op, madr;

	GET_PCBYTE(op)
	if (op >= 0xc0) {
		I286_CLOCK(2)
		out = reg8_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			sft_e8_table[(op >> 3) & 7](madr);
			return;
		}
		out = mem + madr;
	}
	sft_r8_table[(op >> 3) & 7](out);
}

I286_P _shift_ea16_1(void) {			// D1:	shift EA16, 1

	WORD	*out;
	DWORD	op, madr;

	GET_PCBYTE(op)
	if (op >= 0xc0) {
		I286_CLOCK(2)
		out = reg16_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			sft_e16_table[(op >> 3) & 7](madr);
			return;
		}
		out = (WORD *)(mem + madr);
	}
	sft_r16_table[(op >> 3) & 7](out);
}

I286_P _shift_ea8_cl(void) {			// D2:	shift EA8, cl

	BYTE	*out;
	DWORD	op, madr;
	BYTE	cl;

	GET_PCBYTE(op)
	if (op >= 0xc0) {
		I286_CLOCK(5)
		out = reg8_b20[op];
	}
	else {
		I286_CLOCK(8);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			cl = I286_CL;
			I286_CLOCK(cl);
			sft_e8cl_table[(op >> 3) & 7](madr, cl);
			return;
		}
		out = mem + madr;
	}
	cl = I286_CL;
	I286_CLOCK(cl);
	sft_r8cl_table[(op >> 3) & 7](out, cl);
}

I286_P _shift_ea16_cl(void) {			// D3:	shift EA16, cl

	WORD	*out;
	DWORD	op, madr;
	BYTE	cl;

	GET_PCBYTE(op)
	if (op >= 0xc0) {
		I286_CLOCK(5)
		out = reg16_b20[op];
	}
	else {
		I286_CLOCK(8);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			cl = I286_CL;
			I286_CLOCK(cl);
			sft_e16cl_table[(op >> 3) & 7](madr, cl);
			return;
		}
		out = (WORD *)(mem + madr);
	}
	cl = I286_CL;
	I286_CLOCK(cl);
	sft_r16cl_table[(op >> 3) & 7](out, cl);
}

I286_P _aam(void) {							// D4:	AAM

	BYTE	al;
	BYTE	div;

	I286_CLOCK(16)
	GET_PCBYTE(div);
	if (div) {
		al = I286_AL;
		I286_AH = al / div;
		I286_AL = al % div;
		I286_FLAGL &= ~(S_FLAG | Z_FLAG | P_FLAG);
		I286_FLAGL |= szpflag_w[I286_AX];
	}
	else {
		INT_NUM(0, I286_IP - 2);				// 80286
//		INT_NUM(0, I286_IP);					// V30
	}
}

I286_P _aad(void) {							// D5:	AAD

	BYTE	mul;

	I286_CLOCK(14)
	GET_PCBYTE(mul);
	I286_AL += (BYTE)(I286_AH * mul);
	I286_AH = 0;
	I286_FLAGL &= ~(S_FLAG | Z_FLAG | P_FLAG);
	I286_FLAGL |= szpcflag[I286_AL];
}

I286_P _setalc(void) {						// D6:	setalc (80286)

	I286_AL = ((I286_FLAGL & C_FLAG)?0xff:0);
}

I286_P _xlat(void) {						// D7:	xlat

	I286_CLOCK(5)
	I286_AL = i286_memoryread(((I286_AL + I286_BX) & 0xffff) + DS_FIX);
}

I286_P _esc(void) {							// D8:	esc

	DWORD	op;

	I286_CLOCK(2)
	GET_PCBYTE(op)
	if (op < 0xc0) {
		c_calc_lea[op]();
	}
}

I286_P _loopnz(void) {						// E0:	loopnz

	I286_CX--;
	if ((!I286_CX) || (I286_FLAGL & Z_FLAG)) JMPNOP(4) else JMPSHORT(8)
}

I286_P _loopz(void) {						// E1:	loopz

	I286_CX--;
	if ((!I286_CX) || (!(I286_FLAGL & Z_FLAG))) JMPNOP(4) else JMPSHORT(8)
}

I286_P _loop(void) {						// E2:	loop

	I286_CX--;
	if (!I286_CX) JMPNOP(4) else JMPSHORT(8)
}

I286_P _jcxz(void) {						// E3:	jcxz

	if (I286_CX) JMPNOP(4) else JMPSHORT(8)
}

I286_P _in_al_data8(void) {					// E4:	in		al, DATA8

	WORD	port;

	I286_CLOCK(5)
	GET_PCBYTE(port)
	I286_AL = iocore_inp8(port);
}

I286_P _in_ax_data8(void) {					// E5:	in		ax, DATA8

	WORD	port;

	I286_CLOCK(5)
	GET_PCBYTE(port)
	I286_AX = iocore_inp16(port);
}

I286_P _out_data8_al(void) {				// E6:	out		DATA8, al

	WORD	port;

	I286_CLOCK(3)
	GET_PCBYTE(port);
	iocore_out8(port, I286_AL);
}

I286_P _out_data8_ax(void) {				// E7:	out		DATA8, ax

	WORD	port;

	I286_CLOCK(3)
	GET_PCBYTE(port);
	iocore_out16(port, I286_AX);
}

I286_P _call_near(void) {					// E8:	call near

	WORD	ad;

	I286_CLOCK(7)
	GET_PCWORD(ad)
	REGPUSH0(I286_IP)
	I286_IP += ad;
}

I286_P _jmp_near(void) {					// E9:	jmp near

	WORD	ad;

	I286_CLOCK(7)
	GET_PCWORD(ad)
	I286_IP += ad;
}

I286_P _jmp_far(void) {						// EA:	jmp far

	WORD	ad;

	I286_CLOCK(11)
	GET_PCWORD(ad);
	GET_PCWORD(I286_CS);
	CS_BASE = (DWORD)I286_CS << 4;
	I286_IP = ad;
}

I286_P _jmp_short(void) {					// EB:	jmp short

	WORD	ad;

	I286_CLOCK(7)
	GET_PCBYTES(ad)
	I286_IP += ad;
}

I286_P _in_al_dx(void) {					// EC:	in		al, dx

	I286_CLOCK(5)
	I286_AL = iocore_inp8(I286_DX);
}

I286_P _in_ax_dx(void) {					// ED:	in		ax, dx

	I286_CLOCK(5)
	I286_AX = iocore_inp16(I286_DX);
}

I286_P _out_dx_al(void) {					// EE:	out		dx, al

	I286_CLOCK(3)
	iocore_out8(I286_DX, I286_AL);
}

I286_P _out_dx_ax(void) {					// EF:	out		dx, ax

	I286_CLOCK(3)
	iocore_out16(I286_DX, I286_AX);
}

I286_P _lock(void) {						// F0:	lock
											// F1:	lock
	I286_CLOCK(2)
}

I286_P _repne(void) {						// F2:	repne

	i286s.prefix++;
	if (i286s.prefix < MAX_PREFIX) {
		DWORD op;
		GET_PCBYTE(op);
		i286op_repne[op]();
		i286s.prefix = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286_P _repe(void) {						// F3:	repe

	i286s.prefix++;
	if (i286s.prefix < MAX_PREFIX) {
		DWORD op;
		GET_PCBYTE(op);
		i286op_repe[op]();
		i286s.prefix = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286_P _hlt(void) {							// F4:	hlt

	nevent.remainclock = -1;
	I286_IP--;
}

I286_P _cmc(void) {							// F5:	cmc

	I286_CLOCK(2)
	I286_FLAGL ^= C_FLAG;
}

I286_P _ope0xf6(void) {						// F6:	

	DWORD	op;

	GET_PCBYTE(op);
	c_ope0xf6_table[(op >> 3) & 7](op);
}

I286_P _ope0xf7(void) {						// F7:	

	DWORD	op;

	GET_PCBYTE(op);
	c_ope0xf7_table[(op >> 3) & 7](op);
}

I286_P _clc(void) {							// F8:	clc

	I286_CLOCK(2)
	I286_FLAGL &= ~C_FLAG;
}

I286_P _stc(void) {							// F9:	stc

	I286_CLOCK(2)
	I286_FLAGL |= C_FLAG;
}

I286_P _cli(void) {							// FA:	cli

	I286_CLOCK(2)
	I286_FLAG &= ~I_FLAG;
	I286_TRAP = 0;
}

I286_P _sti(void) {							// FB:	sti

	I286_CLOCK(2)
	I286_FLAG |= I_FLAG;
	I286_TRAP = (I286_FLAG & T_FLAG) >> 8;		// ToDo
	REMAIN_ADJUST(1)
}

I286_P _cld(void) {							// FC:	cld

	I286_CLOCK(2)
	I286_FLAG &= ~D_FLAG;
}

I286_P _std(void) {							// FD:	std

	I286_CLOCK(2)
	I286_FLAG |= D_FLAG;
}

I286_P _ope0xfe(void) {						// FE:	

	DWORD	op;

	GET_PCBYTE(op);
	c_ope0xfe_table[(op >> 3) & 1](op);
}

I286_P _ope0xff(void) {						// FF:	

	DWORD	op;

	GET_PCBYTE(op);
	c_ope0xff_table[(op >> 3) & 7](op);
}

// -------------------------------------------------------------------------

const I286OP i286op[] = {
			_add_ea_r8,						// 00:	add		EA, REG8
			_add_ea_r16,					// 01:	add		EA, REG16
			_add_r8_ea,						// 02:	add		REG8, EA
			_add_r16_ea,					// 03:	add		REG16, EA
			_add_al_data8,					// 04:	add		al, DATA8
			_add_ax_data16,					// 05:	add		ax, DATA16
			_push_es,						// 06:	push	es
			_pop_es,						// 07:	pop		es
			_or_ea_r8,						// 08:	or		EA, REGF8
			_or_ea_r16,						// 09:	or		EA, REG16
			_or_r8_ea,						// 0A:	or		REG8, EA
			_or_r16_ea,						// 0B:	or		REG16, EA
			_or_al_data8,					// 0C:	or		al, DATA8
			_or_ax_data16,					// 0D:	or		ax, DATA16
			_push_cs,						// 0E:	push	cs
			i286c_cts,						// 0F:	i286 upper opcode

			_adc_ea_r8,						// 10:	adc		EA, REG8
			_adc_ea_r16,					// 11:	adc		EA, REG16
			_adc_r8_ea,						// 12:	adc		REG8, EA
			_adc_r16_ea,					// 13:	adc		REG16, EA
			_adc_al_data8,					// 14:	adc		al, DATA8
			_adc_ax_data16,					// 15:	adc		ax, DATA16
			_push_ss,						// 16:	push	ss
			_pop_ss,						// 17:	pop		ss
			_sbb_ea_r8,						// 18:	sbb		EA, REG8
			_sbb_ea_r16,					// 19:	sbb		EA, REG16
			_sbb_r8_ea,						// 1A:	sbb		REG8, EA
			_sbb_r16_ea,					// 1B:	sbb		REG16, EA
			_sbb_al_data8,					// 1C:	sbb		al, DATA8
			_sbb_ax_data16,					// 1D:	sbb		ax, DATA16
			_push_ds,						// 1E:	push	ds
			_pop_ds,						// 1F:	pop		ds

			_and_ea_r8,						// 20:	and		EA, REG8
			_and_ea_r16,					// 21:	and		EA, REG16
			_and_r8_ea,						// 22:	and		REG8, EA
			_and_r16_ea,					// 23:	and		REG16, EA
			_and_al_data8,					// 24:	and		al, DATA8
			_and_ax_data16,					// 25:	and		ax, DATA16
			_segprefix_es,					// 26:	es:
			_daa,							// 27:	daa
			_sub_ea_r8,						// 28:	sub		EA, REG8
			_sub_ea_r16,					// 29:	sub		EA, REG16
			_sub_r8_ea,						// 2A:	sub		REG8, EA
			_sub_r16_ea,					// 2B:	sub		REG16, EA
			_sub_al_data8,					// 2C:	sub		al, DATA8
			_sub_ax_data16,					// 2D:	sub		ax, DATA16
			_segprefix_cs,					// 2E:	cs:
			_das,							// 2F:	das

			_xor_ea_r8,						// 30:	xor		EA, REG8
			_xor_ea_r16,					// 31:	xor		EA, REG16
			_xor_r8_ea,						// 32:	xor		REG8, EA
			_xor_r16_ea,					// 33:	xor		REG16, EA
			_xor_al_data8,					// 34:	xor		al, DATA8
			_xor_ax_data16,					// 35:	xor		ax, DATA16
			_segprefix_ss,					// 36:	ss:
			_aaa,							// 37:	aaa
			_cmp_ea_r8,						// 38:	cmp		EA, REG8
			_cmp_ea_r16,					// 39:	cmp		EA, REG16
			_cmp_r8_ea,						// 3A:	cmp		REG8, EA
			_cmp_r16_ea,					// 3B:	cmp		REG16, EA
			_cmp_al_data8,					// 3C:	cmp		al, DATA8
			_cmp_ax_data16,					// 3D:	cmp		ax, DATA16
			_segprefix_ds,					// 3E:	ds:
			_aas,							// 3F:	aas

			_inc_ax,						// 40:	inc		ax
			_inc_cx,						// 41:	inc		cx
			_inc_dx,						// 42:	inc		dx
			_inc_bx,						// 43:	inc		bx
			_inc_sp,						// 44:	inc		sp
			_inc_bp,						// 45:	inc		bp
			_inc_si,						// 46:	inc		si
			_inc_di,						// 47:	inc		di
			_dec_ax,						// 48:	dec		ax
			_dec_cx,						// 49:	dec		cx
			_dec_dx,						// 4A:	dec		dx
			_dec_bx,						// 4B:	dec		bx
			_dec_sp,						// 4C:	dec		sp
			_dec_bp,						// 4D:	dec		bp
			_dec_si,						// 4E:	dec		si
			_dec_di,						// 4F:	dec		di

			_push_ax,						// 50:	push	ax
			_push_cx,						// 51:	push	cx
			_push_dx,						// 52:	push	dx
			_push_bx,						// 53:	push	bx
			_push_sp,						// 54:	push	sp
			_push_bp,						// 55:	push	bp
			_push_si,						// 56:	push	si
			_push_di,						// 57:	push	di
			_pop_ax,						// 58:	pop		ax
			_pop_cx,						// 59:	pop		cx
			_pop_dx,						// 5A:	pop		dx
			_pop_bx,						// 5B:	pop		bx
			_pop_sp,						// 5C:	pop		sp
			_pop_bp,						// 5D:	pop		bp
			_pop_si,						// 5E:	pop		si
			_pop_di,						// 5F:	pop		di

			_pusha,							// 60:	pusha
			_popa,							// 61:	popa
			_bound,							// 62:	bound
			_arpl,							// 63:	arpl
			_reserved,						// 64:	reserved
			_reserved,						// 65:	reserved
			_reserved,						// 66:	reserved
			_reserved,						// 67:	reserved
			_push_data16,					// 68:	push	DATA16
			_imul_reg_ea_data16,			// 69:	imul	REG, EA, DATA16
			_push_data8,					// 6A:	push	DATA8
			_imul_reg_ea_data8,				// 6B:	imul	REG, EA, DATA8
			_insb,							// 6C:	insb
			_insw,							// 6D:	insw
			_outsb,							// 6E:	outsb
			_outsw,							// 6F:	outsw

			_jo_short,						// 70:	jo short
			_jno_short,						// 71:	jno short
			_jc_short,						// 72:	jnae/jb/jc short
			_jnc_short,						// 73:	jae/jnb/jnc short
			_jz_short,						// 74:	je/jz short
			_jnz_short,						// 75:	jne/jnz short
			_jna_short,						// 76:	jna/jbe short
			_ja_short,						// 77:	ja/jnbe short
			_js_short,						// 78:	js short
			_jns_short,						// 79:	jns short
			_jp_short,						// 7A:	jp/jpe short
			_jnp_short,						// 7B:	jnp/jpo short
			_jl_short,						// 7C:	jl/jnge short
			_jnl_short,						// 7D:	jnl/jge short
			_jle_short,						// 7E:	jle/jng short
			_jnle_short,					// 7F:	jg/jnle short

			_calc_ea8_i8,					// 80:	op		EA8, DATA8
			_calc_ea16_i16,					// 81:	op		EA16, DATA16
			_calc_ea8_i8,					// 82:	op		EA8, DATA8
			_calc_ea16_i8,					// 83:	op		EA16, DATA8
			_test_ea_r8,					// 84:	test	EA, REG8
			_test_ea_r16,					// 85:	test	EA, REG16
			_xchg_ea_r8,					// 86:	xchg	EA, REG8
			_xchg_ea_r16,					// 87:	xchg	EA, REG16
			_mov_ea_r8,						// 88:	mov		EA, REG8
			_mov_ea_r16,					// 89:	mov		EA, REG16
			_mov_r8_ea,						// 8A:	mov		REG8, EA
			_mov_r16_ea,					// 8B:	mov		REG16, EA
			_mov_ea_seg,					// 8C:	mov		EA, segreg
			_lea_r16_ea,					// 8D:	lea		REG16, EA
			_mov_seg_ea,					// 8E:	mov		segrem, EA
			_pop_ea,						// 8F:	pop		EA

			_nop,							// 90:	xchg	ax, ax
			_xchg_ax_cx,					// 91:	xchg	ax, cx
			_xchg_ax_dx,					// 92:	xchg	ax, dx
			_xchg_ax_bx,					// 93:	xchg	ax, bx
			_xchg_ax_sp,					// 94:	xchg	ax, sp
			_xchg_ax_bp,					// 95:	xchg	ax, bp
			_xchg_ax_si,					// 96:	xchg	ax, si
			_xchg_ax_di,					// 97:	xchg	ax, di
			_cbw,							// 98:	cbw
			_cwd,							// 99:	cwd
			_call_far,						// 9A:	call far
			_wait,							// 9B:	wait
			_pushf,							// 9C:	pushf
			_popf,							// 9D:	popf
			_sahf,							// 9E:	sahf
			_lahf,							// 9F:	lahf

			_mov_al_m8,						// A0:	mov		al, m8
			_mov_ax_m16,					// A1:	mov		ax, m16
			_mov_m8_al,						// A2:	mov		m8, al
			_mov_m16_ax,					// A3:	mov		m16, ax
			_movsb,							// A4:	movsb
			_movsw,							// A5:	movsw
			_cmpsb,							// A6:	cmpsb
			_cmpsw,							// A7:	cmpsw
			_test_al_data8,					// A8:	test	al, DATA8
			_test_ax_data16,				// A9:	test	ax, DATA16
			_stosb,							// AA:	stosw
			_stosw,							// AB:	stosw
			_lodsb,							// AC:	lodsb
			_lodsw,							// AD:	lodsw
			_scasb,							// AE:	scasb
			_scasw,							// AF:	scasw

			_mov_al_imm,					// B0:	mov		al, imm8
			_mov_cl_imm,					// B1:	mov		cl, imm8
			_mov_dl_imm,					// B2:	mov		dl, imm8
			_mov_bl_imm,					// B3:	mov		bl, imm8
			_mov_ah_imm,					// B4:	mov		ah, imm8
			_mov_ch_imm,					// B5:	mov		ch, imm8
			_mov_dh_imm,					// B6:	mov		dh, imm8
			_mov_bh_imm,					// B7:	mov		bh, imm8
			_mov_ax_imm,					// B8:	mov		ax, imm16
			_mov_cx_imm,					// B9:	mov		cx, imm16
			_mov_dx_imm,					// BA:	mov		dx, imm16
			_mov_bx_imm,					// BB:	mov		bx, imm16
			_mov_sp_imm,					// BC:	mov		sp, imm16
			_mov_bp_imm,					// BD:	mov		bp, imm16
			_mov_si_imm,					// BE:	mov		si, imm16
			_mov_di_imm,					// BF:	mov		di, imm16

			_shift_ea8_data8,				// C0:	shift	EA8, DATA8
			_shift_ea16_data8,				// C1:	shift	EA16, DATA8
			_ret_near_data16,				// C2:	ret near DATA16
			_ret_near,						// C3:	ret near
			_les_r16_ea,					// C4:	les		REG16, EA
			_lds_r16_ea,					// C5:	lds		REG16, EA
			_mov_ea8_data8,					// C6:	mov		EA8, DATA8
			_mov_ea16_data16,				// C7:	mov		EA16, DATA16
			_enter,							// C8:	enter	DATA16, DATA8
			leave,							// C9:	leave
			_ret_far_data16,				// CA:	ret far	DATA16
			_ret_far,						// CB:	ret far
			_int_03,						// CC:	int		3
			_int_data8,						// CD:	int		DATA8
			_into,							// CE:	into
			_iret,							// CF:	iret

			_shift_ea8_1,					// D0:	shift EA8, 1
			_shift_ea16_1,					// D1:	shift EA16, 1
			_shift_ea8_cl,					// D2:	shift EA8, cl
			_shift_ea16_cl,					// D3:	shift EA16, cl
			_aam,							// D4:	AAM
			_aad,							// D5:	AAD
			_setalc,						// D6:	setalc (80286)
			_xlat,							// D7:	xlat
			_esc,							// D8:	esc
			_esc,							// D9:	esc
			_esc,							// DA:	esc
			_esc,							// DB:	esc
			_esc,							// DC:	esc
			_esc,							// DD:	esc
			_esc,							// DE:	esc
			_esc,							// DF:	esc

			_loopnz,						// E0:	loopnz
			_loopz,							// E1:	loopz
			_loop,							// E2:	loop
			_jcxz,							// E3:	jcxz
			_in_al_data8,					// E4:	in		al, DATA8
			_in_ax_data8,					// E5:	in		ax, DATA8
			_out_data8_al,					// E6:	out		DATA8, al
			_out_data8_ax,					// E7:	out		DATA8, ax
			_call_near,						// E8:	call near
			_jmp_near,						// E9:	jmp near
			_jmp_far,						// EA:	jmp far
			_jmp_short,						// EB:	jmp short
			_in_al_dx,						// EC:	in		al, dx
			_in_ax_dx,						// ED:	in		ax, dx
			_out_dx_al,						// EE:	out		dx, al
			_out_dx_ax,						// EF:	out		dx, ax

			_lock,							// F0:	lock
			_lock,							// F1:	lock
			_repne,							// F2:	repne
			_repe,							// F3:	repe
			_hlt,							// F4:	hlt
			_cmc,							// F5:	cmc
			_ope0xf6,						// F6:	
			_ope0xf7,						// F7:	
			_clc,							// F8:	clc
			_stc,							// F9:	stc
			_cli,							// FA:	cli
			_sti,							// FB:	sti
			_cld,							// FC:	cld
			_std,							// FD:	std
			_ope0xfe,						// FE:	
			_ope0xff,						// FF:	
};



// ----------------------------------------------------------------- repe

I286_P _repe_segprefix_es(void) {

	DS_FIX = ES_BASE;
	SS_FIX = ES_BASE;
	i286s.prefix++;
	if (i286s.prefix < MAX_PREFIX) {
		DWORD	op;
		GET_PCBYTE(op);
		i286op_repe[op]();
		REMOVE_PREFIX
		i286s.prefix = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286_P _repe_segprefix_cs(void) {

	DS_FIX = CS_BASE;
	SS_FIX = CS_BASE;
	i286s.prefix++;
	if (i286s.prefix < MAX_PREFIX) {
		DWORD	op;
		GET_PCBYTE(op);
		i286op_repe[op]();
		REMOVE_PREFIX
		i286s.prefix = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286_P _repe_segprefix_ss(void) {

	DS_FIX = SS_BASE;
	SS_FIX = SS_BASE;
	i286s.prefix++;
	if (i286s.prefix < MAX_PREFIX) {
		DWORD	op;
		GET_PCBYTE(op);
		i286op_repe[op]();
		REMOVE_PREFIX
		i286s.prefix = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286_P _repe_segprefix_ds(void) {

	DS_FIX = DS_BASE;
	SS_FIX = DS_BASE;
	i286s.prefix++;
	if (i286s.prefix < MAX_PREFIX) {
		DWORD	op;
		GET_PCBYTE(op);
		i286op_repe[op]();
		REMOVE_PREFIX
		i286s.prefix = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

const I286OP i286op_repe[] = {
			_add_ea_r8,						// 00:	add		EA, REG8
			_add_ea_r16,					// 01:	add		EA, REG16
			_add_r8_ea,						// 02:	add		REG8, EA
			_add_r16_ea,					// 03:	add		REG16, EA
			_add_al_data8,					// 04:	add		al, DATA8
			_add_ax_data16,					// 05:	add		ax, DATA16
			_push_es,						// 06:	push	es
			_pop_es,						// 07:	pop		es
			_or_ea_r8,						// 08:	or		EA, REGF8
			_or_ea_r16,						// 09:	or		EA, REG16
			_or_r8_ea,						// 0A:	or		REG8, EA
			_or_r16_ea,						// 0B:	or		REG16, EA
			_or_al_data8,					// 0C:	or		al, DATA8
			_or_ax_data16,					// 0D:	or		ax, DATA16
			_push_cs,						// 0E:	push	cs
			i286c_cts,						// 0F:	i286 upper opcode

			_adc_ea_r8,						// 10:	adc		EA, REG8
			_adc_ea_r16,					// 11:	adc		EA, REG16
			_adc_r8_ea,						// 12:	adc		REG8, EA
			_adc_r16_ea,					// 13:	adc		REG16, EA
			_adc_al_data8,					// 14:	adc		al, DATA8
			_adc_ax_data16,					// 15:	adc		ax, DATA16
			_push_ss,						// 16:	push	ss
			_pop_ss,						// 17:	pop		ss
			_sbb_ea_r8,						// 18:	sbb		EA, REG8
			_sbb_ea_r16,					// 19:	sbb		EA, REG16
			_sbb_r8_ea,						// 1A:	sbb		REG8, EA
			_sbb_r16_ea,					// 1B:	sbb		REG16, EA
			_sbb_al_data8,					// 1C:	sbb		al, DATA8
			_sbb_ax_data16,					// 1D:	sbb		ax, DATA16
			_push_ds,						// 1E:	push	ds
			_pop_ds,						// 1F:	pop		ds

			_and_ea_r8,						// 20:	and		EA, REG8
			_and_ea_r16,					// 21:	and		EA, REG16
			_and_r8_ea,						// 22:	and		REG8, EA
			_and_r16_ea,					// 23:	and		REG16, EA
			_and_al_data8,					// 24:	and		al, DATA8
			_and_ax_data16,					// 25:	and		ax, DATA16
			_repe_segprefix_es,				// 26:	repe es:
			_daa,							// 27:	daa
			_sub_ea_r8,						// 28:	sub		EA, REG8
			_sub_ea_r16,					// 29:	sub		EA, REG16
			_sub_r8_ea,						// 2A:	sub		REG8, EA
			_sub_r16_ea,					// 2B:	sub		REG16, EA
			_sub_al_data8,					// 2C:	sub		al, DATA8
			_sub_ax_data16,					// 2D:	sub		ax, DATA16
			_repe_segprefix_cs,				// 2E:	repe cs:
			_das,							// 2F:	das

			_xor_ea_r8,						// 30:	xor		EA, REG8
			_xor_ea_r16,					// 31:	xor		EA, REG16
			_xor_r8_ea,						// 32:	xor		REG8, EA
			_xor_r16_ea,					// 33:	xor		REG16, EA
			_xor_al_data8,					// 34:	xor		al, DATA8
			_xor_ax_data16,					// 35:	xor		ax, DATA16
			_repe_segprefix_ss,				// 36:	repe ss:
			_aaa,							// 37:	aaa
			_cmp_ea_r8,						// 38:	cmp		EA, REG8
			_cmp_ea_r16,					// 39:	cmp		EA, REG16
			_cmp_r8_ea,						// 3A:	cmp		REG8, EA
			_cmp_r16_ea,					// 3B:	cmp		REG16, EA
			_cmp_al_data8,					// 3C:	cmp		al, DATA8
			_cmp_ax_data16,					// 3D:	cmp		ax, DATA16
			_repe_segprefix_ds,				// 3E:	repe ds:
			_aas,							// 3F:	aas

			_inc_ax,						// 40:	inc		ax
			_inc_cx,						// 41:	inc		cx
			_inc_dx,						// 42:	inc		dx
			_inc_bx,						// 43:	inc		bx
			_inc_sp,						// 44:	inc		sp
			_inc_bp,						// 45:	inc		bp
			_inc_si,						// 46:	inc		si
			_inc_di,						// 47:	inc		di
			_dec_ax,						// 48:	dec		ax
			_dec_cx,						// 49:	dec		cx
			_dec_dx,						// 4A:	dec		dx
			_dec_bx,						// 4B:	dec		bx
			_dec_sp,						// 4C:	dec		sp
			_dec_bp,						// 4D:	dec		bp
			_dec_si,						// 4E:	dec		si
			_dec_di,						// 4F:	dec		di

			_push_ax,						// 50:	push	ax
			_push_cx,						// 51:	push	cx
			_push_dx,						// 52:	push	dx
			_push_bx,						// 53:	push	bx
			_push_sp,						// 54:	push	sp
			_push_bp,						// 55:	push	bp
			_push_si,						// 56:	push	si
			_push_di,						// 57:	push	di
			_pop_ax,						// 58:	pop		ax
			_pop_cx,						// 59:	pop		cx
			_pop_dx,						// 5A:	pop		dx
			_pop_bx,						// 5B:	pop		bx
			_pop_sp,						// 5C:	pop		sp
			_pop_bp,						// 5D:	pop		bp
			_pop_si,						// 5E:	pop		si
			_pop_di,						// 5F:	pop		di

			_pusha,							// 60:	pusha
			_popa,							// 61:	popa
			_bound,							// 62:	bound
			_arpl,							// 63:	arpl
			_reserved,						// 64:	reserved
			_reserved,						// 65:	reserved
			_reserved,						// 66:	reserved
			_reserved,						// 67:	reserved
			_push_data16,					// 68:	push	DATA16
			_imul_reg_ea_data16,			// 69:	imul	REG, EA, DATA16
			_push_data8,					// 6A:	push	DATA8
			_imul_reg_ea_data8,				// 6B:	imul	REG, EA, DATA8
			i286c_rep_insb,					// 6C:	rep insb
			i286c_rep_insw,					// 6D:	rep insw
			i286c_rep_outsb,				// 6E:	rep outsb
			i286c_rep_outsb,				// 6F:	rep outsw

			_jo_short,						// 70:	jo short
			_jno_short,						// 71:	jno short
			_jc_short,						// 72:	jnae/jb/jc short
			_jnc_short,						// 73:	jae/jnb/jnc short
			_jz_short,						// 74:	je/jz short
			_jnz_short,						// 75:	jne/jnz short
			_jna_short,						// 76:	jna/jbe short
			_ja_short,						// 77:	ja/jnbe short
			_js_short,						// 78:	js short
			_jns_short,						// 79:	jns short
			_jp_short,						// 7A:	jp/jpe short
			_jnp_short,						// 7B:	jnp/jpo short
			_jl_short,						// 7C:	jl/jnge short
			_jnl_short,						// 7D:	jnl/jge short
			_jle_short,						// 7E:	jle/jng short
			_jnle_short,					// 7F:	jg/jnle short

			_calc_ea8_i8,					// 80:	op		EA8, DATA8
			_calc_ea16_i16,					// 81:	op		EA16, DATA16
			_calc_ea8_i8,					// 82:	op		EA8, DATA8
			_calc_ea16_i8,					// 83:	op		EA16, DATA8
			_test_ea_r8,					// 84:	test	EA, REG8
			_test_ea_r16,					// 85:	test	EA, REG16
			_xchg_ea_r8,					// 86:	xchg	EA, REG8
			_xchg_ea_r16,					// 87:	xchg	EA, REG16
			_mov_ea_r8,						// 88:	mov		EA, REG8
			_mov_ea_r16,					// 89:	mov		EA, REG16
			_mov_r8_ea,						// 8A:	mov		REG8, EA
			_mov_r16_ea,					// 8B:	add		REG16, EA
			_mov_ea_seg,					// 8C:	mov		EA, segreg
			_lea_r16_ea,					// 8D:	lea		REG16, EA
			_mov_seg_ea,					// 8E:	mov		segrem, EA
			_pop_ea,						// 8F:	pop		EA

			_nop,							// 90:	xchg	ax, ax
			_xchg_ax_cx,					// 91:	xchg	ax, cx
			_xchg_ax_dx,					// 92:	xchg	ax, dx
			_xchg_ax_bx,					// 93:	xchg	ax, bx
			_xchg_ax_sp,					// 94:	xchg	ax, sp
			_xchg_ax_bp,					// 95:	xchg	ax, bp
			_xchg_ax_si,					// 96:	xchg	ax, si
			_xchg_ax_di,					// 97:	xchg	ax, di
			_cbw,							// 98:	cbw
			_cwd,							// 99:	cwd
			_call_far,						// 9A:	call far
			_wait,							// 9B:	wait
			_pushf,							// 9C:	pushf
			_popf,							// 9D:	popf
			_sahf,							// 9E:	sahf
			_lahf,							// 9F:	lahf

			_mov_al_m8,						// A0:	mov		al, m8
			_mov_ax_m16,					// A1:	mov		ax, m16
			_mov_m8_al,						// A2:	mov		m8, al
			_mov_m16_ax,					// A3:	mov		m16, ax
			i286c_rep_movsb,				// A4:	rep movsb
			i286c_rep_movsw,				// A5:	rep movsw
			i286c_repe_cmpsb,				// A6:	repe cmpsb
			i286c_repe_cmpsw,				// A7:	repe cmpsw
			_test_al_data8,					// A8:	test	al, DATA8
			_test_ax_data16,				// A9:	test	ax, DATA16
			i286c_rep_stosb,				// AA:	rep stosw
			i286c_rep_stosw,				// AB:	rep stosw
			i286c_rep_lodsb,				// AC:	rep lodsb
			i286c_rep_lodsw,				// AD:	rep lodsw
			i286c_repe_scasb,				// AE:	repe scasb
			i286c_repe_scasw,				// AF:	repe scasw

			_mov_al_imm,					// B0:	mov		al, imm8
			_mov_cl_imm,					// B1:	mov		cl, imm8
			_mov_dl_imm,					// B2:	mov		dl, imm8
			_mov_bl_imm,					// B3:	mov		bl, imm8
			_mov_ah_imm,					// B4:	mov		ah, imm8
			_mov_ch_imm,					// B5:	mov		ch, imm8
			_mov_dh_imm,					// B6:	mov		dh, imm8
			_mov_bh_imm,					// B7:	mov		bh, imm8
			_mov_ax_imm,					// B8:	mov		ax, imm16
			_mov_cx_imm,					// B9:	mov		cx, imm16
			_mov_dx_imm,					// BA:	mov		dx, imm16
			_mov_bx_imm,					// BB:	mov		bx, imm16
			_mov_sp_imm,					// BC:	mov		sp, imm16
			_mov_bp_imm,					// BD:	mov		bp, imm16
			_mov_si_imm,					// BE:	mov		si, imm16
			_mov_di_imm,					// BF:	mov		di, imm16

			_shift_ea8_data8,				// C0:	shift	EA8, DATA8
			_shift_ea16_data8,				// C1:	shift	EA16, DATA8
			_ret_near_data16,				// C2:	ret near DATA16
			_ret_near,						// C3:	ret near
			_les_r16_ea,					// C4:	les		REG16, EA
			_lds_r16_ea,					// C5:	lds		REG16, EA
			_mov_ea8_data8,					// C6:	mov		EA8, DATA8
			_mov_ea16_data16,				// C7:	mov		EA16, DATA16
			_enter,							// C8:	enter	DATA16, DATA8
			leave,							// C9:	leave
			_ret_far_data16,				// CA:	ret far	DATA16
			_ret_far,						// CB:	ret far
			_int_03,						// CC:	int		3
			_int_data8,						// CD:	int		DATA8
			_into,							// CE:	into
			_iret,							// CF:	iret

			_shift_ea8_1,					// D0:	shift EA8, 1
			_shift_ea16_1,					// D1:	shift EA16, 1
			_shift_ea8_cl,					// D2:	shift EA8, cl
			_shift_ea16_cl,					// D3:	shift EA16, cl
			_aam,							// D4:	AAM
			_aad,							// D5:	AAD
			_setalc,						// D6:	setalc (80286)
			_xlat,							// D7:	xlat
			_esc,							// D8:	esc
			_esc,							// D9:	esc
			_esc,							// DA:	esc
			_esc,							// DB:	esc
			_esc,							// DC:	esc
			_esc,							// DD:	esc
			_esc,							// DE:	esc
			_esc,							// DF:	esc

			_loopnz,						// E0:	loopnz
			_loopz,							// E1:	loopz
			_loop,							// E2:	loop
			_jcxz,							// E3:	jcxz
			_in_al_data8,					// E4:	in		al, DATA8
			_in_ax_data8,					// E5:	in		ax, DATA8
			_out_data8_al,					// E6:	out		DATA8, al
			_out_data8_ax,					// E7:	out		DATA8, ax
			_call_near,						// E8:	call near
			_jmp_near,						// E9:	jmp near
			_jmp_far,						// EA:	jmp far
			_jmp_short,						// EB:	jmp short
			_in_al_dx,						// EC:	in		al, dx
			_in_ax_dx,						// ED:	in		ax, dx
			_out_dx_al,						// EE:	out		dx, al
			_out_dx_ax,						// EF:	out		dx, ax

			_lock,							// F0:	lock
			_lock,							// F1:	lock
			_repne,							// F2:	repne
			_repe,							// F3:	repe
			_hlt,							// F4:	hlt
			_cmc,							// F5:	cmc
			_ope0xf6,						// F6:	
			_ope0xf7,						// F7:	
			_clc,							// F8:	clc
			_stc,							// F9:	stc
			_cli,							// FA:	cli
			_sti,							// FB:	sti
			_cld,							// FC:	cld
			_std,							// FD:	std
			_ope0xfe,						// FE:	
			_ope0xff,						// FF:	
};


// ----------------------------------------------------------------- repne

I286_P _repne_segprefix_es(void) {

	DS_FIX = ES_BASE;
	SS_FIX = ES_BASE;
	i286s.prefix++;
	if (i286s.prefix < MAX_PREFIX) {
		DWORD	op;
		GET_PCBYTE(op);
		i286op_repne[op]();
		REMOVE_PREFIX
		i286s.prefix = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286_P _repne_segprefix_cs(void) {

	DS_FIX = CS_BASE;
	SS_FIX = CS_BASE;
	i286s.prefix++;
	if (i286s.prefix < MAX_PREFIX) {
		DWORD	op;
		GET_PCBYTE(op);
		i286op_repne[op]();
		REMOVE_PREFIX
		i286s.prefix = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286_P _repne_segprefix_ss(void) {

	DS_FIX = SS_BASE;
	SS_FIX = SS_BASE;
	i286s.prefix++;
	if (i286s.prefix < MAX_PREFIX) {
		DWORD	op;
		GET_PCBYTE(op);
		i286op_repne[op]();
		REMOVE_PREFIX
		i286s.prefix = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286_P _repne_segprefix_ds(void) {

	DS_FIX = DS_BASE;
	SS_FIX = DS_BASE;
	i286s.prefix++;
	if (i286s.prefix < MAX_PREFIX) {
		DWORD	op;
		GET_PCBYTE(op);
		i286op_repne[op]();
		REMOVE_PREFIX
		i286s.prefix = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

const I286OP i286op_repne[] = {
			_add_ea_r8,						// 00:	add		EA, REG8
			_add_ea_r16,					// 01:	add		EA, REG16
			_add_r8_ea,						// 02:	add		REG8, EA
			_add_r16_ea,					// 03:	add		REG16, EA
			_add_al_data8,					// 04:	add		al, DATA8
			_add_ax_data16,					// 05:	add		ax, DATA16
			_push_es,						// 06:	push	es
			_pop_es,						// 07:	pop		es
			_or_ea_r8,						// 08:	or		EA, REGF8
			_or_ea_r16,						// 09:	or		EA, REG16
			_or_r8_ea,						// 0A:	or		REG8, EA
			_or_r16_ea,						// 0B:	or		REG16, EA
			_or_al_data8,					// 0C:	or		al, DATA8
			_or_ax_data16,					// 0D:	or		ax, DATA16
			_push_cs,						// 0E:	push	cs
			i286c_cts,						// 0F:	i286 upper opcode

			_adc_ea_r8,						// 10:	adc		EA, REG8
			_adc_ea_r16,					// 11:	adc		EA, REG16
			_adc_r8_ea,						// 12:	adc		REG8, EA
			_adc_r16_ea,					// 13:	adc		REG16, EA
			_adc_al_data8,					// 14:	adc		al, DATA8
			_adc_ax_data16,					// 15:	adc		ax, DATA16
			_push_ss,						// 16:	push	ss
			_pop_ss,						// 17:	pop		ss
			_sbb_ea_r8,						// 18:	sbb		EA, REG8
			_sbb_ea_r16,					// 19:	sbb		EA, REG16
			_sbb_r8_ea,						// 1A:	sbb		REG8, EA
			_sbb_r16_ea,					// 1B:	sbb		REG16, EA
			_sbb_al_data8,					// 1C:	sbb		al, DATA8
			_sbb_ax_data16,					// 1D:	sbb		ax, DATA16
			_push_ds,						// 1E:	push	ds
			_pop_ds,						// 1F:	pop		ds

			_and_ea_r8,						// 20:	and		EA, REG8
			_and_ea_r16,					// 21:	and		EA, REG16
			_and_r8_ea,						// 22:	and		REG8, EA
			_and_r16_ea,					// 23:	and		REG16, EA
			_and_al_data8,					// 24:	and		al, DATA8
			_and_ax_data16,					// 25:	and		ax, DATA16
			_repne_segprefix_es,			// 26:	repne es:
			_daa,							// 27:	daa
			_sub_ea_r8,						// 28:	sub		EA, REG8
			_sub_ea_r16,					// 29:	sub		EA, REG16
			_sub_r8_ea,						// 2A:	sub		REG8, EA
			_sub_r16_ea,					// 2B:	sub		REG16, EA
			_sub_al_data8,					// 2C:	sub		al, DATA8
			_sub_ax_data16,					// 2D:	sub		ax, DATA16
			_repne_segprefix_cs,			// 2E:	repne cs:
			_das,							// 2F:	das

			_xor_ea_r8,						// 30:	xor		EA, REG8
			_xor_ea_r16,					// 31:	xor		EA, REG16
			_xor_r8_ea,						// 32:	xor		REG8, EA
			_xor_r16_ea,					// 33:	xor		REG16, EA
			_xor_al_data8,					// 34:	xor		al, DATA8
			_xor_ax_data16,					// 35:	xor		ax, DATA16
			_repne_segprefix_ss,			// 36:	repne ss:
			_aaa,							// 37:	aaa
			_cmp_ea_r8,						// 38:	cmp		EA, REG8
			_cmp_ea_r16,					// 39:	cmp		EA, REG16
			_cmp_r8_ea,						// 3A:	cmp		REG8, EA
			_cmp_r16_ea,					// 3B:	cmp		REG16, EA
			_cmp_al_data8,					// 3C:	cmp		al, DATA8
			_cmp_ax_data16,					// 3D:	cmp		ax, DATA16
			_repne_segprefix_ds,			// 3E:	repne ds:
			_aas,							// 3F:	aas

			_inc_ax,						// 40:	inc		ax
			_inc_cx,						// 41:	inc		cx
			_inc_dx,						// 42:	inc		dx
			_inc_bx,						// 43:	inc		bx
			_inc_sp,						// 44:	inc		sp
			_inc_bp,						// 45:	inc		bp
			_inc_si,						// 46:	inc		si
			_inc_di,						// 47:	inc		di
			_dec_ax,						// 48:	dec		ax
			_dec_cx,						// 49:	dec		cx
			_dec_dx,						// 4A:	dec		dx
			_dec_bx,						// 4B:	dec		bx
			_dec_sp,						// 4C:	dec		sp
			_dec_bp,						// 4D:	dec		bp
			_dec_si,						// 4E:	dec		si
			_dec_di,						// 4F:	dec		di

			_push_ax,						// 50:	push	ax
			_push_cx,						// 51:	push	cx
			_push_dx,						// 52:	push	dx
			_push_bx,						// 53:	push	bx
			_push_sp,						// 54:	push	sp
			_push_bp,						// 55:	push	bp
			_push_si,						// 56:	push	si
			_push_di,						// 57:	push	di
			_pop_ax,						// 58:	pop		ax
			_pop_cx,						// 59:	pop		cx
			_pop_dx,						// 5A:	pop		dx
			_pop_bx,						// 5B:	pop		bx
			_pop_sp,						// 5C:	pop		sp
			_pop_bp,						// 5D:	pop		bp
			_pop_si,						// 5E:	pop		si
			_pop_di,						// 5F:	pop		di

			_pusha,							// 60:	pusha
			_popa,							// 61:	popa
			_bound,							// 62:	bound
			_arpl,							// 63:	arpl
			_reserved,						// 64:	reserved
			_reserved,						// 65:	reserved
			_reserved,						// 66:	reserved
			_reserved,						// 67:	reserved
			_push_data16,					// 68:	push	DATA16
			_imul_reg_ea_data16,			// 69:	imul	REG, EA, DATA16
			_push_data8,					// 6A:	push	DATA8
			_imul_reg_ea_data8,				// 6B:	imul	REG, EA, DATA8
			i286c_rep_insb,					// 6C:	rep insb
			i286c_rep_insw,					// 6D:	rep insw
			i286c_rep_outsb,				// 6E:	rep outsb
			i286c_rep_outsb,				// 6F:	rep outsw

			_jo_short,						// 70:	jo short
			_jno_short,						// 71:	jno short
			_jc_short,						// 72:	jnae/jb/jc short
			_jnc_short,						// 73:	jae/jnb/jnc short
			_jz_short,						// 74:	je/jz short
			_jnz_short,						// 75:	jne/jnz short
			_jna_short,						// 76:	jna/jbe short
			_ja_short,						// 77:	ja/jnbe short
			_js_short,						// 78:	js short
			_jns_short,						// 79:	jns short
			_jp_short,						// 7A:	jp/jpe short
			_jnp_short,						// 7B:	jnp/jpo short
			_jl_short,						// 7C:	jl/jnge short
			_jnl_short,						// 7D:	jnl/jge short
			_jle_short,						// 7E:	jle/jng short
			_jnle_short,					// 7F:	jg/jnle short

			_calc_ea8_i8,					// 80:	op		EA8, DATA8
			_calc_ea16_i16,					// 81:	op		EA16, DATA16
			_calc_ea8_i8,					// 82:	op		EA8, DATA8
			_calc_ea16_i8,					// 83:	op		EA16, DATA8
			_test_ea_r8,					// 84:	test	EA, REG8
			_test_ea_r16,					// 85:	test	EA, REG16
			_xchg_ea_r8,					// 86:	xchg	EA, REG8
			_xchg_ea_r16,					// 87:	xchg	EA, REG16
			_mov_ea_r8,						// 88:	mov		EA, REG8
			_mov_ea_r16,					// 89:	mov		EA, REG16
			_mov_r8_ea,						// 8A:	mov		REG8, EA
			_mov_r16_ea,					// 8B:	add		REG16, EA
			_mov_ea_seg,					// 8C:	mov		EA, segreg
			_lea_r16_ea,					// 8D:	lea		REG16, EA
			_mov_seg_ea,					// 8E:	mov		segrem, EA
			_pop_ea,						// 8F:	pop		EA

			_nop,							// 90:	xchg	ax, ax
			_xchg_ax_cx,					// 91:	xchg	ax, cx
			_xchg_ax_dx,					// 92:	xchg	ax, dx
			_xchg_ax_bx,					// 93:	xchg	ax, bx
			_xchg_ax_sp,					// 94:	xchg	ax, sp
			_xchg_ax_bp,					// 95:	xchg	ax, bp
			_xchg_ax_si,					// 96:	xchg	ax, si
			_xchg_ax_di,					// 97:	xchg	ax, di
			_cbw,							// 98:	cbw
			_cwd,							// 99:	cwd
			_call_far,						// 9A:	call far
			_wait,							// 9B:	wait
			_pushf,							// 9C:	pushf
			_popf,							// 9D:	popf
			_sahf,							// 9E:	sahf
			_lahf,							// 9F:	lahf

			_mov_al_m8,						// A0:	mov		al, m8
			_mov_ax_m16,					// A1:	mov		ax, m16
			_mov_m8_al,						// A2:	mov		m8, al
			_mov_m16_ax,					// A3:	mov		m16, ax
			i286c_rep_movsb,				// A4:	rep movsb
			i286c_rep_movsw,				// A5:	rep movsw
			i286c_repne_cmpsb,				// A6:	repne cmpsb
			i286c_repne_cmpsw,				// A7:	repne cmpsw
			_test_al_data8,					// A8:	test	al, DATA8
			_test_ax_data16,				// A9:	test	ax, DATA16
			i286c_rep_stosb,				// AA:	rep stosw
			i286c_rep_stosw,				// AB:	rep stosw
			i286c_rep_lodsb,				// AC:	rep lodsb
			i286c_rep_lodsw,				// AD:	rep lodsw
			i286c_repne_scasb,				// AE:	repne scasb
			i286c_repne_scasw,				// AF:	repne scasw

			_mov_al_imm,					// B0:	mov		al, imm8
			_mov_cl_imm,					// B1:	mov		cl, imm8
			_mov_dl_imm,					// B2:	mov		dl, imm8
			_mov_bl_imm,					// B3:	mov		bl, imm8
			_mov_ah_imm,					// B4:	mov		ah, imm8
			_mov_ch_imm,					// B5:	mov		ch, imm8
			_mov_dh_imm,					// B6:	mov		dh, imm8
			_mov_bh_imm,					// B7:	mov		bh, imm8
			_mov_ax_imm,					// B8:	mov		ax, imm16
			_mov_cx_imm,					// B9:	mov		cx, imm16
			_mov_dx_imm,					// BA:	mov		dx, imm16
			_mov_bx_imm,					// BB:	mov		bx, imm16
			_mov_sp_imm,					// BC:	mov		sp, imm16
			_mov_bp_imm,					// BD:	mov		bp, imm16
			_mov_si_imm,					// BE:	mov		si, imm16
			_mov_di_imm,					// BF:	mov		di, imm16

			_shift_ea8_data8,				// C0:	shift	EA8, DATA8
			_shift_ea16_data8,				// C1:	shift	EA16, DATA8
			_ret_near_data16,				// C2:	ret near DATA16
			_ret_near,						// C3:	ret near
			_les_r16_ea,					// C4:	les		REG16, EA
			_lds_r16_ea,					// C5:	lds		REG16, EA
			_mov_ea8_data8,					// C6:	mov		EA8, DATA8
			_mov_ea16_data16,				// C7:	mov		EA16, DATA16
			_enter,							// C8:	enter	DATA16, DATA8
			leave,							// C9:	leave
			_ret_far_data16,				// CA:	ret far	DATA16
			_ret_far,						// CB:	ret far
			_int_03,						// CC:	int		3
			_int_data8,						// CD:	int		DATA8
			_into,							// CE:	into
			_iret,							// CF:	iret

			_shift_ea8_1,					// D0:	shift EA8, 1
			_shift_ea16_1,					// D1:	shift EA16, 1
			_shift_ea8_cl,					// D2:	shift EA8, cl
			_shift_ea16_cl,					// D3:	shift EA16, cl
			_aam,							// D4:	AAM
			_aad,							// D5:	AAD
			_setalc,						// D6:	setalc (80286)
			_xlat,							// D7:	xlat
			_esc,							// D8:	esc
			_esc,							// D9:	esc
			_esc,							// DA:	esc
			_esc,							// DB:	esc
			_esc,							// DC:	esc
			_esc,							// DD:	esc
			_esc,							// DE:	esc
			_esc,							// DF:	esc

			_loopnz,						// E0:	loopnz
			_loopz,							// E1:	loopz
			_loop,							// E2:	loop
			_jcxz,							// E3:	jcxz
			_in_al_data8,					// E4:	in		al, DATA8
			_in_ax_data8,					// E5:	in		ax, DATA8
			_out_data8_al,					// E6:	out		DATA8, al
			_out_data8_ax,					// E7:	out		DATA8, ax
			_call_near,						// E8:	call near
			_jmp_near,						// E9:	jmp near
			_jmp_far,						// EA:	jmp far
			_jmp_short,						// EB:	jmp short
			_in_al_dx,						// EC:	in		al, dx
			_in_ax_dx,						// ED:	in		ax, dx
			_out_dx_al,						// EE:	out		dx, al
			_out_dx_ax,						// EF:	out		dx, ax

			_lock,							// F0:	lock
			_lock,							// F1:	lock
			_repne,							// F2:	repne
			_repe,							// F3:	repe
			_hlt,							// F4:	hlt
			_cmc,							// F5:	cmc
			_ope0xf6,						// F6:	
			_ope0xf7,						// F7:	
			_clc,							// F8:	clc
			_stc,							// F9:	stc
			_cli,							// FA:	cli
			_sti,							// FB:	sti
			_cld,							// FC:	cld
			_std,							// FD:	std
			_ope0xfe,						// FE:	
			_ope0xff,						// FF:	
};




// ----

#if 0
void INTR_CALL i286c_interrupt(BYTE vect) {

	BYTE	op;

	op = i286_memoryread(I286_IP + CS_BASE);
	if (op == 0xf4) {							// hlt
		I286_IP++;
	}
	REGPUSH0(REAL_FLAGREG)
	REGPUSH0(I286_CS)
	REGPUSH0(I286_IP)
	I286_IP = GETWORD(mem + vect*4);			// real mode!
	I286_CS = GETWORD(mem + vect*4 + 2);		// real mode!
	CS_BASE = (DWORD)I286_CS << 4;
	I286_CLOCK(20)
}



void i286c(void) {

	DWORD	opcode;

	do {
		GET_PCBYTE(opcode);
		i286op[opcode]();
	} while(nevent.remainclock > 0);
}

void i286c_withtrap(void) {

	DWORD	opcode;

	do {
		GET_PCBYTE(opcode);
		i286op[opcode]();
		if (I286_TRAP) {
			i286c_interrupt(1);
		}
		dma_proc();

	} while(nevent.remainclock > 0);
}

void i286c_withdma(void) {

	DWORD	opcode;

	do {
		GET_PCBYTE(opcode);
		i286op[opcode]();
		dma_proc();
	} while(nevent.remainclock > 0);
}


void i286c_step(void) {

	BYTE	opcode;

	I286_OV = I286_FLAG & O_FLAG;
	I286_FLAG &= ~(O_FLAG);

	GET_PCBYTE(opcode);
	i286op[opcode]();

	I286_FLAG &= ~(O_FLAG);
	if (I286_OV) {
		I286_FLAG |= (O_FLAG);
	}
	dma_proc();
}
#endif

#include	"compiler.h"
#include	"i286.h"
#include	"i286c.h"
#include	"i286c.mcr"
#include	"memory.h"
#include	"nevent.h"


// ------------------------------------------------------------ opecode 0xf6,7

I286_F6 _test_ea8_data8(DWORD op) {

	DWORD	src, dst;

	if (op >= 0xc0) {
		I286_CLOCK(2);
		dst = *(reg8_b20[op]);
	}
	else {
		I286_CLOCK(6);
		dst = i286_memoryread(c_calc_ea_dst[op]());
	}
	GET_PCBYTE(src)
	ANDBYTE(dst, src)
}

I286_F6 _not_ea8(DWORD op) {

	DWORD	madr;

	if (op >= 0xc0) {
		I286_CLOCK(2);
		*(reg8_b20[op]) ^= 0xff;
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			BYTE value = i286_memoryread(madr);
			value = ~value;
			i286_memorywrite(madr, value);
			return;
		}
		*(mem + madr) ^= 0xff;
	}
}

I286_F6 _neg_ea8(DWORD op) {

	BYTE	*out;
	DWORD	src, dst, madr;

	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg8_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			src = i286_memoryread(madr);
			BYTE_NEG(dst, src)
			i286_memorywrite(madr, (BYTE)dst);
			return;
		}
		out = mem + madr;
	}
	src = *out;
	BYTE_NEG(dst, src)
	*out = (BYTE)dst;
}

I286_F6 _mul_ea8(DWORD op) {

	BYTE	src;
	DWORD	res;

	if (op >= 0xc0) {
		I286_CLOCK(13);
		src = *(reg8_b20[op]);
	}
	else {
		I286_CLOCK(16);
		src = i286_memoryread(c_calc_ea_dst[op]());
	}
	BYTE_MUL(res, I286_AL, src)
	I286_AX = (WORD)res;
}

I286_F6 _imul_ea8(DWORD op) {

	BYTE	src;
	long	res;

	if (op >= 0xc0) {
		I286_CLOCK(13);
		src = *(reg8_b20[op]);
	}
	else {
		I286_CLOCK(16);
		src = i286_memoryread(c_calc_ea_dst[op]());
	}
	BYTE_IMUL(res, I286_AL, src)
	I286_AX = (WORD)res;
}

I286_F6 _div_ea8(DWORD op) {

	WORD	tmp;
	BYTE	src;
	WORD	ip;

	ip = I286_IP;
	if (op >= 0xc0) {
		I286_CLOCK(2);
		src = *(reg8_b20[op]);
	}
	else {
		I286_CLOCK(7);
		src = i286_memoryread(c_calc_ea_dst[op]());
	}
	tmp = I286_AX;
	if (tmp < ((WORD)src << 8)) {
		I286_AL = tmp / src;
		I286_AH = tmp % src;
	}
	else {
		INT_NUM(0, ip - 2);										// 80x86
	}
}

I286_F6 _idiv_ea8(DWORD op) {

	short	tmp, r;
	char	src;
	WORD	ip;

	ip = I286_IP;
	if (op >= 0xc0) {
		I286_CLOCK(17);
		src = *(reg8_b20[op]);
	}
	else {
		I286_CLOCK(25);
		src = i286_memoryread(c_calc_ea_dst[op]());
	}
	tmp = (short)I286_AX;
	if (src) {
		r = tmp / src;
		if (!((r + 0x80) & 0xff00)) {
			I286_AL = (char)r;
			I286_AH = tmp % src;
			return;
		}
	}
	INT_NUM(0, ip - 2);											// 80x86
}


I286_F6 _test_ea16_data16(DWORD op) {

	DWORD	src, dst;

	if (op >= 0xc0) {
		I286_CLOCK(2);
		dst = *(reg16_b20[op]);
	}
	else {
		I286_CLOCK(6);
		dst = i286_memoryread_w(c_calc_ea_dst[op]());
	}
	GET_PCWORD(src)
	ANDWORD(dst, src)
}

I286_F6 _not_ea16(DWORD op) {

	DWORD	madr;

	if (op >= 0xc0) {
		I286_CLOCK(2);
		*(reg16_b20[op]) ^= 0xffff;
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (!(INHIBIT_WORDP(madr))) {
			*(mem + madr) ^= 0xffff;
		}
		else {
			WORD value = i286_memoryread_w(madr);
			value = ~value;
			i286_memorywrite_w(madr, value);
		}
	}
}

I286_F6 _neg_ea16(DWORD op) {

	WORD	*out;
	DWORD	src, dst, madr;

	if (op >= 0xc0) {
		I286_CLOCK(2);
		out = reg16_b20[op];
	}
	else {
		I286_CLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			src = i286_memoryread_w(madr);
			WORD_NEG(dst, src)
			i286_memorywrite_w(madr, (WORD)dst);
			return;
		}
		out = (WORD *)(mem + madr);
	}
	src = *out;
	WORD_NEG(dst, src)
	*out = (WORD)dst;
}

I286_F6 _mul_ea16(DWORD op) {

	WORD	src;
	DWORD	res;

	if (op >= 0xc0) {
		I286_CLOCK(21);
		src = *(reg16_b20[op]);
	}
	else {
		I286_CLOCK(24);
		src = i286_memoryread_w(c_calc_ea_dst[op]());
	}
	WORD_MUL(res, I286_AX, src)
	I286_AX = (WORD)res;
	I286_DX = (WORD)(res >> 16);
}

I286_F6 _imul_ea16(DWORD op) {

	short	src;
	long	res;

	if (op >= 0xc0) {
		I286_CLOCK(21);
		src = *(reg16_b20[op]);
	}
	else {
		I286_CLOCK(24);
		src = i286_memoryread_w(c_calc_ea_dst[op]());
	}
	WORD_IMUL(res, I286_AX, src)
	I286_AX = (WORD)res;
	I286_DX = (WORD)(res >> 16);
}

I286_F6 _div_ea16(DWORD op) {

	DWORD	tmp;
	WORD	src;
	WORD	ip;

	ip = I286_IP;
	if (op >= 0xc0) {
		I286_CLOCK(17);
		src = *(reg16_b20[op]);
	}
	else {
		I286_CLOCK(25);
		src = i286_memoryread_w(c_calc_ea_dst[op]());
	}
	tmp = ((DWORD)I286_DX << 16) | I286_AX;
	if (tmp < ((DWORD)src << 16)) {
		I286_AX = (short)(tmp / src);
		I286_DX = (short)(tmp % src);
	}
	else {
		INT_NUM(0, ip - 2);										// 80x86
	}
}

I286_F6 _idiv_ea16(DWORD op) {

	long	tmp, r;
	short	src;
	WORD	ip;

	ip = I286_IP;
	if (op >= 0xc0) {
		I286_CLOCK(17);
		src = *(reg16_b20[op]);
	}
	else {
		I286_CLOCK(25);
		src = i286_memoryread_w(c_calc_ea_dst[op]());
	}
	tmp = (long)(((DWORD)I286_DX << 16) | I286_AX);
	if (src) {
		r = tmp / src;
		if (!((r + 0x8000) & 0xffff0000)) {
			I286_AX = (short)r;
			I286_DX = tmp % src;
			return;
		}
	}
	INT_NUM(0, ip - 2);											// 80x86
}


const I286OPF6 c_ope0xf6_table[] = {
			_test_ea8_data8,	_test_ea8_data8,
			_not_ea8,			_neg_ea8,
			_mul_ea8,			_imul_ea8,
			_div_ea8,			_idiv_ea8};

const I286OPF6 c_ope0xf7_table[] = {
			_test_ea16_data16,	_test_ea16_data16,
			_not_ea16,			_neg_ea16,
			_mul_ea16,			_imul_ea16,
			_div_ea16,			_idiv_ea16};


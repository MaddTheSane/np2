#include	"compiler.h"
#include	"i286.h"
#include	"i286c.h"
#include	"memory.h"
#include	"i286c.mcr"


// ------------------------------------------------------------ opecode 0xfe,f

#if 0
I286_F6 _nop_int(UINT op) {

	INT_NUM(6, I286_IP - 2);
}
#endif

I286_F6 _inc_ea8(UINT op) {

	UINT32	madr;
	BYTE	*out;

	if (op >= 0xc0) {
		I286_WORKCLOCK(2);
		out = REG8_B20(op);
	}
	else {
		I286_WORKCLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			BYTE value = i286_memoryread(madr);
			BYTE_INC(value)
			i286_memorywrite(madr, value);
			return;
		}
		out = mem + madr;
	}
	BYTE_INC(*out)
}

I286_F6 _dec_ea8(UINT op) {

	UINT32	madr;
	BYTE	*out;

	if (op >= 0xc0) {
		I286_WORKCLOCK(2);
		out = REG8_B20(op);
	}
	else {
		I286_WORKCLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (madr >= I286_MEMWRITEMAX) {
			BYTE value = i286_memoryread(madr);
			BYTE_DEC(value)
			i286_memorywrite(madr, value);
			return;
		}
		out = mem + madr;
	}
	BYTE_DEC(*out)
}

I286_F6 _inc_ea16(UINT op) {

	UINT32	madr;
	UINT16	*out;

	if (op >= 0xc0) {
		I286_WORKCLOCK(2);
		out = REG16_B20(op);
	}
	else {
		I286_WORKCLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			UINT16 value = i286_memoryread_w(madr);
			WORD_INC(value)
			i286_memorywrite_w(madr, value);
			return;
		}
		out = (UINT16 *)(mem + madr);
	}
	WORD_INC(*out)
}

I286_F6 _dec_ea16(UINT op) {

	UINT32	madr;
	UINT16	*out;

	if (op >= 0xc0) {
		I286_WORKCLOCK(2);
		out = REG16_B20(op);
	}
	else {
		I286_WORKCLOCK(7);
		madr = c_calc_ea_dst[op]();
		if (INHIBIT_WORDP(madr)) {
			UINT16 value = i286_memoryread_w(madr);
			WORD_DEC(value)
			i286_memorywrite_w(madr, value);
			return;
		}
		out = (UINT16 *)(mem + madr);
	}
	WORD_DEC(*out)
}

I286_F6 _call_ea16(UINT op) {

	UINT16	src;

	if (op >= 0xc0) {
		I286_WORKCLOCK(7);
		src = *(REG16_B20(op));
	}
	else {
		I286_WORKCLOCK(11);
		src = i286_memoryread_w(c_calc_ea_dst[op]());
	}
	REGPUSH0(I286_IP);
	I286_IP = src;
}

I286_F6 _call_far_ea16(UINT op) {

	I286_WORKCLOCK(16);
	if (op < 0xc0) {
		UINT16 ad = c_get_ea[op]();
		REGPUSH0(I286_CS)								// ToDo
		REGPUSH0(I286_IP)
		I286_IP = i286_memoryread_w(ad + EA_FIX);
		ad += 2;
		I286_CS = i286_memoryread_w(ad + EA_FIX);
		CS_BASE = I286_CS << 4;
	}
	else {
		INT_NUM(6, I286_IP - 2);
	}
}

I286_F6 _jmp_ea16(UINT op) {

	if (op >= 0xc0) {
		I286_WORKCLOCK(7);
		I286_IP = *(REG16_B20(op));
	}
	else {
		I286_WORKCLOCK(11);
		I286_IP = i286_memoryread_w(c_calc_ea_dst[op]());
	}
}

I286_F6 _jmp_far_ea16(UINT op) {

	I286_WORKCLOCK(11);
	if (op < 0xc0) {
		UINT16 ad = c_get_ea[op]();
		I286_IP = i286_memoryread_w(ad + EA_FIX);
		ad += 2;
		I286_CS = i286_memoryread_w(ad + EA_FIX);
		CS_BASE = I286_CS << 4;
	}
	else {
		INT_NUM(6, I286_IP - 2);
	}
}

I286_F6 _push_ea16(UINT op) {

	UINT16	src;

	if (op >= 0xc0) {
		I286_WORKCLOCK(3);
		src = *(REG16_B20(op));
	}
	else {
		I286_WORKCLOCK(5);
		src = i286_memoryread_w(c_calc_ea_dst[op]());
	}
	REGPUSH0(src);
}

I286_F6 _pop_ea16(UINT op) {

	UINT16	src;

	REGPOP0(src);
	I286_WORKCLOCK(5);
	if (op >= 0xc0) {
		*(REG16_B20(op)) = src;
	}
	else {
		i286_memorywrite_w(c_calc_ea_dst[op](), src);
	}
}


const I286OPF6 c_ope0xfe_table[] = {
			_inc_ea8,			_dec_ea8};

const I286OPF6 c_ope0xff_table[] = {
			_inc_ea16,			_dec_ea16,
			_call_ea16,			_call_far_ea16,
			_jmp_ea16,			_jmp_far_ea16,
			_push_ea16,			_pop_ea16};


#include	"compiler.h"
#include	"i286.h"
#include	"i286c.h"
#include	"i286c.mcr"
#include	"memory.h"
#include	"nevent.h"


I286_0F _sgdt(DWORD op) {

	I286_CLOCK(11)
	if (op < 0xc0) {
		UINT16 ad = c_get_ea[op]();
		i286_memorywrite_w(ad + EA_FIX, GDTR.limit);
		ad += 2;
		i286_memorywrite_w(ad + EA_FIX, GDTR.base);
		ad += 2;
		i286_memorywrite_w(ad + EA_FIX, (UINT16)(0xff00 + GDTR.base24));
	}
	else {
		INT_NUM(6, I286_IP - 2);
	}
}

I286_0F _sidt(DWORD op) {

	I286_CLOCK(12)
	if (op < 0xc0) {
		UINT16 ad = c_get_ea[op]();
		i286_memorywrite_w(ad + EA_FIX, IDTR.limit);
		ad += 2;
		i286_memorywrite_w(ad + EA_FIX, IDTR.base);
		ad += 2;
		i286_memorywrite_w(ad + EA_FIX, (UINT16)(0xff00 + IDTR.base24));
	}
	else {
		INT_NUM(6, I286_IP - 2);
	}
}

I286_0F _lgdt(DWORD op) {

	I286_CLOCK(11)
	if (op < 0xc0) {
		WORD ad = c_get_ea[op]();
		GDTR.limit = i286_memoryread_w(ad + EA_FIX);
		ad += 2;
		GDTR.base = i286_memoryread_w(ad + EA_FIX);
		ad += 2;
		GDTR.base24 = i286_memoryread(ad + EA_FIX);
		ad++;
		GDTR.reserved = i286_memoryread(ad + EA_FIX);
	}
	else {
		INT_NUM(6, I286_IP - 2);
	}
}

I286_0F _lidt(DWORD op) {

	I286_CLOCK(11)
	if (op < 0xc0) {
		WORD ad = c_get_ea[op]();
		IDTR.limit = i286_memoryread_w(ad + EA_FIX);
		ad += 2;
		IDTR.base = i286_memoryread_w(ad + EA_FIX);
		ad += 2;
		IDTR.base24 = i286_memoryread(ad + EA_FIX);
		ad++;
		IDTR.reserved = i286_memoryread(ad + EA_FIX);
	}
	else {
		INT_NUM(6, I286_IP - 2);
	}
}

I286_0F _smsw(DWORD op) {

	if (op >= 0xc0) {
		I286_CLOCK(3)
		*(reg16_b20[op]) = MSW;
	}
	else {
		I286_CLOCK(6)
		i286_memorywrite_w(c_calc_ea_dst[op](), MSW);
	}
}

I286_0F _lmsw(DWORD op) {

	if (op >= 0xc0) {
		I286_CLOCK(2)
		MSW = *(reg16_b20[op]);
	}
	else {
		I286_CLOCK(3)
		MSW = i286_memoryread_w(c_calc_ea_dst[op]());
	}
}

static const I286OP_0F cts1_table[] = {
			_sgdt,				_sidt,
			_lgdt,				_lidt,
			_smsw,				_smsw,
			_lmsw,				_lmsw};


I286_0F _loadall286(void) {

	UINT16	tmp;

	I286_CLOCK(195);
	MSW = LOADINTELWORD(mem + 0x804);
	tmp = LOADINTELWORD(mem + 0x818);
	I286_OV = tmp & O_FLAG;
	I286_FLAG = tmp & (0xfff ^ O_FLAG);
	I286_TRAP = ((tmp & 0x300) == 0x300);
	I286_IP = LOADINTELWORD(mem + 0x81a);
	I286_DS = LOADINTELWORD(mem + 0x81e);
	I286_SS = LOADINTELWORD(mem + 0x820);
	I286_CS = LOADINTELWORD(mem + 0x822);
	I286_ES = LOADINTELWORD(mem + 0x824);
	I286_DI = LOADINTELWORD(mem + 0x826);
	I286_SI = LOADINTELWORD(mem + 0x828);
	I286_BP = LOADINTELWORD(mem + 0x82a);
	I286_SP = LOADINTELWORD(mem + 0x82c);
	I286_BX = LOADINTELWORD(mem + 0x82e);
	I286_DX = LOADINTELWORD(mem + 0x830);
	I286_CX = LOADINTELWORD(mem + 0x832);
	I286_AX = LOADINTELWORD(mem + 0x834);
	ES_BASE = LOADINTELDWORD(mem + 0x836) & 0x00ffffff;
	CS_BASE = LOADINTELDWORD(mem + 0x83c) & 0x00ffffff;
	SS_BASE = LOADINTELDWORD(mem + 0x842) & 0x00ffffff;
	SS_FIX = SS_BASE;
	DS_BASE = LOADINTELDWORD(mem + 0x848) & 0x00ffffff;
	DS_FIX = DS_BASE;
	I286IRQCHECKTERM
}

I286EXT i286c_cts(void) {

	WORD	ip;
	DWORD	op, op2;

	ip = I286_IP;
	GET_PCBYTE(op);

	if (op == 1) {
		GET_PCBYTE(op2);
		cts1_table[(op2 >> 3) & 7](op2);
	}
	else if (op == 5) {
		_loadall286();
	}
	else {
		I286_CLOCK(20)
		INT_NUM(6, ip - 1);
	}
}


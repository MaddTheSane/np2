#include	"compiler.h"
#include	"cpucore.h"
#include	"i286c.h"
#include	"i286c.mcr"


I286_0F _sgdt(UINT op) {

	UINT32	seg;
	UINT	ad;

	I286_WORKCLOCK(11);
	if (op < 0xc0) {
		ad = GET_EA(op, &seg);
		i286_memorywrite_w(seg + ad, I286_GDTR.limit);
		i286_memorywrite_w(seg + LOW16(ad + 2), I286_GDTR.base);
		i286_memorywrite_w(seg + LOW16(ad + 4),
										(REG16)(0xff00 + I286_GDTR.base24));
	}
	else {
		INT_NUM(6, I286_IP - 2);
	}
}

I286_0F _sidt(UINT op) {

	UINT32	seg;
	UINT	ad;

	I286_WORKCLOCK(12);
	if (op < 0xc0) {
		ad = GET_EA(op, &seg);
		i286_memorywrite_w(seg + ad, I286_IDTR.limit);
		i286_memorywrite_w(seg + LOW16(ad + 2), I286_IDTR.base);
		i286_memorywrite_w(seg + LOW16(ad + 4),
										(REG16)(0xff00 + I286_IDTR.base24));
	}
	else {
		INT_NUM(6, I286_IP - 2);
	}
}

I286_0F _lgdt(UINT op) {

	UINT32	seg;
	UINT	ad;

	I286_WORKCLOCK(11);
	if (op < 0xc0) {
		ad = GET_EA(op, &seg);
		I286_GDTR.limit = i286_memoryread_w(seg + ad);
		I286_GDTR.base = i286_memoryread_w(seg + LOW16(ad + 2));
		I286_GDTR.base24 = i286_memoryread(seg + LOW16(ad + 4));
		I286_GDTR.reserved = i286_memoryread(seg + LOW16(ad + 5));
	}
	else {
		INT_NUM(6, I286_IP - 2);
	}
}

I286_0F _lidt(UINT op) {

	UINT32	seg;
	UINT	ad;

	I286_WORKCLOCK(11);
	if (op < 0xc0) {
		ad = GET_EA(op, &seg);
		I286_IDTR.limit = i286_memoryread_w(seg + ad);
		I286_IDTR.base = i286_memoryread_w(seg + LOW16(ad + 2));
		I286_IDTR.base24 = i286_memoryread(seg + LOW16(ad + 4));
		I286_IDTR.reserved = i286_memoryread(seg + LOW16(ad + 5));
	}
	else {
		INT_NUM(6, I286_IP - 2);
	}
}

I286_0F _smsw(UINT op) {

	if (op >= 0xc0) {
		I286_WORKCLOCK(3);
		*(REG16_B20(op)) = I286_MSW;
	}
	else {
		I286_WORKCLOCK(6);
		i286_memorywrite_w(CALC_EA(op), I286_MSW);
	}
}

I286_0F _lmsw(UINT op) {

	if (op >= 0xc0) {
		I286_WORKCLOCK(2);
		I286_MSW = *(REG16_B20(op));
	}
	else {
		I286_WORKCLOCK(3);
		I286_MSW = i286_memoryread_w(CALC_EA(op));
	}
}

static const I286OP_0F cts1_table[] = {
			_sgdt,				_sidt,
			_lgdt,				_lidt,
			_smsw,				_smsw,
			_lmsw,				_lmsw};


I286_0F _loadall286(void) {

	UINT16	tmp;

	I286_WORKCLOCK(195);
	I286_MSW = LOADINTELWORD(mem + 0x804);
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

	UINT16	ip;
	UINT	op;
	UINT	op2;

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
		INT_NUM(6, ip - 1);
	}
}


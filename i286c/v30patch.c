#include	"compiler.h"
#include	"cpucore.h"
#include	"i286c.h"
#include	"v30patch.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"dmap.h"
#include	"i286c.mcr"


// victory30 patch

#define	MAX_PREFIX		8

#define	NEXT_OPCODE												\
		if (I286_REMCLOCK < 1) {								\
			I286_BASECLOCK += (1 - I286_REMCLOCK);				\
			I286_REMCLOCK = 1;									\
		}

#define REAL_V30FLAG	(UINT16)((I286_FLAG & 0x7ff) + \
											(I286_OV?O_FLAG:0) + 0xf000)

typedef struct {
	UINT	opnum;
	void	(*v30opcode)(void);
} V30PATCH;

static void (*v30op[256])(void);
static void (*v30op_repne[256])(void);
static void (*v30op_repe[256])(void);


static const BYTE shiftbase16[256] =
				{0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15};

static const BYTE shiftbase09[256] =
				{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6,
				 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4,
				 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2,
				 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9,
				 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7,
				 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5,
				 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3,
				 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1,
				 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8,
				 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6,
				 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4,
				 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2,
				 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9,
				 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7,
				 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3, 4, 5,
				 6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 1, 2, 3};

static const BYTE shiftbase17[256] =
				{0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
				16,17, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
				15,16,17, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,
				14,15,16,17, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,
				13,14,15,16,17, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,
				12,13,14,15,16,17, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,
				11,12,13,14,15,16,17, 1, 2, 3, 4, 5, 6, 7, 8, 9,
				10,11,12,13,14,15,16,17, 1, 2, 3, 4, 5, 6, 7, 8,
				 9,10,11,12,13,14,15,16,17, 1, 2, 3, 4, 5, 6, 7,
				 8, 9,10,11,12,13,14,15,16,17, 1, 2, 3, 4, 5, 6,
				 7, 8, 9,10,11,12,13,14,15,16,17, 1, 2, 3, 4, 5,
				 6, 7, 8, 9,10,11,12,13,14,15,16,17, 1, 2, 3, 4,
				 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17, 1, 2, 3,
				 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17, 1, 2,
				 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17, 1,
				 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17};


I286FN v30_reserved(void) {

	I286_WORKCLOCK(2);
}

I286FN v30segprefix_es(void) {				// 26: es:

	SS_FIX = ES_BASE;
	DS_FIX = ES_BASE;
	I286_PREFIX++;
	if (I286_PREFIX < MAX_PREFIX) {
		UINT op;
		GET_PCBYTE(op);
		v30op[op]();
		REMOVE_PREFIX
		I286_PREFIX = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286FN v30segprefix_cs(void) {				// 2e: cs:

	SS_FIX = CS_BASE;
	DS_FIX = CS_BASE;
	I286_PREFIX++;
	if (I286_PREFIX < MAX_PREFIX) {
		UINT op;
		GET_PCBYTE(op);
		v30op[op]();
		REMOVE_PREFIX
		I286_PREFIX = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286FN v30segprefix_ss(void) {				// 36: ss:

	SS_FIX = SS_BASE;
	DS_FIX = SS_BASE;
	I286_PREFIX++;
	if (I286_PREFIX < MAX_PREFIX) {
		UINT op;
		GET_PCBYTE(op);
		v30op[op]();
		REMOVE_PREFIX
		I286_PREFIX = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286FN v30segprefix_ds(void) {				// 3e: ds:

	SS_FIX = DS_BASE;
	DS_FIX = DS_BASE;
	I286_PREFIX++;
	if (I286_PREFIX < MAX_PREFIX) {
		UINT op;
		GET_PCBYTE(op);
		v30op[op]();
		REMOVE_PREFIX
		I286_PREFIX = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286FN v30push_sp(void) REGPUSH(I286_SP, 3)	// 54: push sp
I286FN v30pop_sp(void) REGPOP(I286_SP, 5)	// 5C: pop sp

I286FN v30mov_seg_ea(void) {				// 8E:	mov		segrem, EA

	UINT	op;
	UINT	tmp;
	UINT16	ipbak;

	ipbak = I286_IP;
	GET_PCBYTE(op);
	if (op >= 0xc0) {
		I286_WORKCLOCK(2);
		tmp = *(REG16_B20(op));
	}
	else {
		I286_WORKCLOCK(5);
		tmp = i286_memoryread_w(CALC_EA(op));
	}
	switch(op & 0x18) {
		case 0x00:			// es
			I286_ES = (UINT16)tmp;
			ES_BASE = tmp << 4;
			break;

		case 0x08:			// cs
			I286_CS = (UINT16)tmp;
			CS_BASE = tmp << 4;
			break;

		case 0x10:			// ss
			I286_SS = (UINT16)tmp;
			SS_BASE = tmp << 4;
			SS_FIX = SS_BASE;
			NEXT_OPCODE
			break;

		case 0x18:			// ds
			I286_DS = (UINT16)tmp;
			DS_BASE = tmp << 4;
			DS_FIX = DS_BASE;
			break;
	}
}

I286FN v30_pushf(void) {					// 9C:	pushf

	REGPUSH(REAL_V30FLAG, 3)
}

I286FN v30_popf(void) {						// 9D:	popf

	I286_WORKCLOCK(5);
	REGPOP0(I286_FLAG)
	I286_FLAG |= 0xf000;
	I286_OV = I286_FLAG & O_FLAG;
	I286_FLAG &= (0xfff ^ O_FLAG);
	I286_TRAP = ((I286_FLAG & 0x300) == 0x300);
	I286IRQCHECKTERM
}

I286FN v30shift_ea8_data8(void) {			// C0:	shift	EA8, DATA8

	BYTE	*out;
	UINT	op;
	UINT32	madr;
	BYTE	cl;

	GET_PCBYTE(op)
	if (op >= 0xc0) {
		I286_WORKCLOCK(5);
		out = REG8_B20(op);
	}
	else {
		I286_WORKCLOCK(8);
		madr = CALC_EA(op);
		if (madr >= I286_MEMWRITEMAX) {
			GET_PCBYTE(cl)
			if ((op & 0x30) == 0x10) {		// rotate with carry
				cl = shiftbase09[cl];
			}
			else {
				cl = shiftbase16[cl];
			}
			I286_WORKCLOCK(cl);
			sft_e8cl_table[(op >> 3) & 7](madr, cl);
			return;
		}
		out = mem + madr;
	}
	GET_PCBYTE(cl)
	if ((op & 0x30) == 0x10) {		// rotate with carry
		cl = shiftbase09[cl];
	}
	else {
		cl = shiftbase16[cl];
	}
	I286_WORKCLOCK(cl);
	sft_r8cl_table[(op >> 3) & 7](out, cl);
}

I286FN v30shift_ea16_data8(void) {			// C1:	shift	EA16, DATA8

	UINT16	*out;
	UINT	op;
	UINT32	madr;
	BYTE	cl;

	GET_PCBYTE(op)
	if (op >= 0xc0) {
		I286_WORKCLOCK(5);
		out = REG16_B20(op);
	}
	else {
		I286_WORKCLOCK(8);
		madr = CALC_EA(op);
		if (INHIBIT_WORDP(madr)) {
			GET_PCBYTE(cl);
			if ((op & 0x30) == 0x10) {		// rotate with carry
				cl = shiftbase17[cl];
			}
			else {
				cl = shiftbase16[cl];
			}
			I286_WORKCLOCK(cl);
			sft_e16cl_table[(op >> 3) & 7](madr, cl);
			return;
		}
		out = (UINT16 *)(mem + madr);
	}
	GET_PCBYTE(cl);
	if ((op & 0x30) == 0x10) {		// rotate with carry
		cl = shiftbase17[cl];
	}
	else {
		cl = shiftbase16[cl];
	}
	I286_WORKCLOCK(cl);
	sft_r16cl_table[(op >> 3) & 7](out, cl);
}

I286FN v30shift_ea8_cl(void) {				// D2:	shift EA8, cl

	BYTE	*out;
	UINT	op;
	UINT32	madr;
	BYTE	cl;

	GET_PCBYTE(op)
	if (op >= 0xc0) {
		I286_WORKCLOCK(5);
		out = REG8_B20(op);
	}
	else {
		I286_WORKCLOCK(8);
		madr = CALC_EA(op);
		if (madr >= I286_MEMWRITEMAX) {
			cl = I286_CL;
			I286_WORKCLOCK(cl);
			if ((op & 0x30) == 0x10) {		// rotate with carry
				cl = shiftbase09[cl];
			}
			else {
				cl = shiftbase16[cl];
			}
			sft_e8cl_table[(op >> 3) & 7](madr, cl);
			return;
		}
		out = mem + madr;
	}
	cl = I286_CL;
	I286_WORKCLOCK(cl);
	if ((op & 0x30) == 0x10) {		// rotate with carry
		cl = shiftbase09[cl];
	}
	else {
		cl = shiftbase16[cl];
	}
	sft_r8cl_table[(op >> 3) & 7](out, cl);
}

I286FN v30shift_ea16_cl(void) {				// D3:	shift EA16, cl

	UINT16	*out;
	UINT	op;
	UINT32	madr;
	BYTE	cl;

	GET_PCBYTE(op)
	if (op >= 0xc0) {
		I286_WORKCLOCK(5);
		out = REG16_B20(op);
	}
	else {
		I286_WORKCLOCK(8);
		madr = CALC_EA(op);
		if (INHIBIT_WORDP(madr)) {
			cl = I286_CL;
			I286_WORKCLOCK(cl);
			if ((op & 0x30) == 0x10) {		// rotate with carry
				cl = shiftbase17[cl];
			}
			else {
				cl = shiftbase16[cl];
			}
			sft_e16cl_table[(op >> 3) & 7](madr, cl);
			return;
		}
		out = (UINT16 *)(mem + madr);
	}
	cl = I286_CL;
	I286_WORKCLOCK(cl);
	if ((op & 0x30) == 0x10) {		// rotate with carry
		cl = shiftbase17[cl];
	}
	else {
		cl = shiftbase16[cl];
	}
	sft_r16cl_table[(op >> 3) & 7](out, cl);
}

I286FN v30_aam(void) {						// D4:	AAM

	BYTE	al;

	I286_WORKCLOCK(16);
	I286_IP++;								// is 10
	al = I286_AL;
	I286_AH = al / 10;
	I286_AL = al % 10;
	I286_FLAGL &= ~(S_FLAG | Z_FLAG | P_FLAG);
	I286_FLAGL |= WORDSZPF(I286_AX);
}

I286FN v30_aad(void) {						// D5:	AAD

	I286_WORKCLOCK(14);
	I286_IP++;								// is 10
	I286_AL += (BYTE)(I286_AH * 10);
	I286_AH = 0;
	I286_FLAGL &= ~(S_FLAG | Z_FLAG | P_FLAG);
	I286_FLAGL |= BYTESZPF(I286_AL);
}

I286FN v30_xlat(void) {						// D6:	xlat

	I286_WORKCLOCK(5);
	I286_AL = i286_memoryread(LOW16(I286_AL + I286_BX) + DS_FIX);
}

I286FN v30_repne(void) {					// F2:	repne

	I286_PREFIX++;
	if (I286_PREFIX < MAX_PREFIX) {
		UINT op;
		GET_PCBYTE(op);
		v30op_repne[op]();
		I286_PREFIX = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286FN v30_repe(void) {						// F3:	repe

	I286_PREFIX++;
	if (I286_PREFIX < MAX_PREFIX) {
		UINT op;
		GET_PCBYTE(op);
		v30op_repe[op]();
		I286_PREFIX = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

static const V30PATCH v30patch_op[] = {
			{0x26, v30segprefix_es},		// 26:	es:
			{0x2e, v30segprefix_cs},		// 2E:	cs:
			{0x36, v30segprefix_ss},		// 36:	ss:
			{0x3e, v30segprefix_ds},		// 3E:	ds:
			{0x54, v30push_sp},				// 54:	push	sp
			{0x5c, v30pop_sp},				// 5C:	pop		sp
			{0x63, v30_reserved},			// 63:	reserved
			{0x64, v30_reserved},			// 64:	reserved
			{0x65, v30_reserved},			// 65:	reserved
			{0x66, v30_reserved},			// 66:	reserved
			{0x67, v30_reserved},			// 67:	reserved
			{0x8e, v30mov_seg_ea},			// 8E:	mov		segrem, EA
			{0x9c, v30_pushf},				// 9C:	pushf
			{0x9d, v30_popf},				// 9D:	popf
			{0xc0, v30shift_ea8_data8},		// C0:	shift	EA8, DATA8
			{0xc1, v30shift_ea16_data8},	// C1:	shift	EA16, DATA8
			{0xd2, v30shift_ea8_cl},		// D2:	shift EA8, cl
			{0xd3, v30shift_ea16_cl},		// D3:	shift EA16, cl
			{0xd4, v30_aam},				// D4:	AAM
			{0xd5, v30_aad},				// D5:	AAD
			{0xd6, v30_xlat},				// D6:	xlat (8086/V30)
			{0xf2, v30_repne},				// F2:	repne
			{0xf3, v30_repe}				// F3:	repe
};


// ----------------------------------------------------------------- repe

I286FN v30repe_segprefix_es(void) {

	DS_FIX = ES_BASE;
	SS_FIX = ES_BASE;
	I286_PREFIX++;
	if (I286_PREFIX < MAX_PREFIX) {
		UINT op;
		GET_PCBYTE(op);
		v30op_repe[op]();
		REMOVE_PREFIX
		I286_PREFIX = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286FN v30repe_segprefix_cs(void) {

	DS_FIX = CS_BASE;
	SS_FIX = CS_BASE;
	I286_PREFIX++;
	if (I286_PREFIX < MAX_PREFIX) {
		UINT op;
		GET_PCBYTE(op);
		v30op_repe[op]();
		REMOVE_PREFIX
		I286_PREFIX = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286FN v30repe_segprefix_ss(void) {

	DS_FIX = SS_BASE;
	SS_FIX = SS_BASE;
	I286_PREFIX++;
	if (I286_PREFIX < MAX_PREFIX) {
		UINT op;
		GET_PCBYTE(op);
		v30op_repe[op]();
		REMOVE_PREFIX
		I286_PREFIX = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286FN v30repe_segprefix_ds(void) {

	DS_FIX = DS_BASE;
	SS_FIX = DS_BASE;
	I286_PREFIX++;
	if (I286_PREFIX < MAX_PREFIX) {
		UINT op;
		GET_PCBYTE(op);
		v30op_repe[op]();
		REMOVE_PREFIX
		I286_PREFIX = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

static const V30PATCH v30patch_repe[] = {
			{0x26, v30repe_segprefix_es},	// 26:	repe es:
			{0x2e, v30repe_segprefix_cs},	// 2E:	repe cs:
			{0x36, v30repe_segprefix_ss},	// 36:	repe ss:
			{0x3e, v30repe_segprefix_ds},	// 3E:	repe ds:
			{0x54, v30push_sp},				// 54:	push	sp
			{0x5c, v30pop_sp},				// 5C:	pop		sp
			{0x63, v30_reserved},			// 63:	reserved
			{0x64, v30_reserved},			// 64:	reserved
			{0x65, v30_reserved},			// 65:	reserved
			{0x66, v30_reserved},			// 66:	reserved
			{0x67, v30_reserved},			// 67:	reserved
			{0x8e, v30mov_seg_ea},			// 8E:	mov		segrem, EA
			{0x9c, v30_pushf},				// 9C:	pushf
			{0x9d, v30_popf},				// 9D:	popf
			{0xc0, v30shift_ea8_data8},		// C0:	shift	EA8, DATA8
			{0xc1, v30shift_ea16_data8},	// C1:	shift	EA16, DATA8
			{0xd2, v30shift_ea8_cl},		// D2:	shift EA8, cl
			{0xd3, v30shift_ea16_cl},		// D3:	shift EA16, cl
			{0xd4, v30_aam},				// D4:	AAM
			{0xd5, v30_aad},				// D5:	AAD
			{0xd6, v30_xlat},				// D6:	xlat (8086/V30)
			{0xf2, v30_repne},				// F2:	repne
			{0xf3, v30_repe}				// F3:	repe
};


// ----------------------------------------------------------------- repne

I286FN v30repne_segprefix_es(void) {

	DS_FIX = ES_BASE;
	SS_FIX = ES_BASE;
	I286_PREFIX++;
	if (I286_PREFIX < MAX_PREFIX) {
		UINT op;
		GET_PCBYTE(op);
		v30op_repne[op]();
		REMOVE_PREFIX
		I286_PREFIX = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286FN v30repne_segprefix_cs(void) {

	DS_FIX = CS_BASE;
	SS_FIX = CS_BASE;
	I286_PREFIX++;
	if (I286_PREFIX < MAX_PREFIX) {
		UINT op;
		GET_PCBYTE(op);
		v30op_repne[op]();
		REMOVE_PREFIX
		I286_PREFIX = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286FN v30repne_segprefix_ss(void) {

	DS_FIX = SS_BASE;
	SS_FIX = SS_BASE;
	I286_PREFIX++;
	if (I286_PREFIX < MAX_PREFIX) {
		UINT op;
		GET_PCBYTE(op);
		v30op_repne[op]();
		REMOVE_PREFIX
		I286_PREFIX = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

I286FN v30repne_segprefix_ds(void) {

	DS_FIX = DS_BASE;
	SS_FIX = DS_BASE;
	I286_PREFIX++;
	if (I286_PREFIX < MAX_PREFIX) {
		UINT op;
		GET_PCBYTE(op);
		v30op_repne[op]();
		REMOVE_PREFIX
		I286_PREFIX = 0;
	}
	else {
		INT_NUM(6, I286_IP);
	}
}

static const V30PATCH v30patch_repne[] = {
			{0x26, v30repne_segprefix_es},	// 26:	repne es:
			{0x2e, v30repne_segprefix_cs},	// 2E:	repne cs:
			{0x36, v30repne_segprefix_ss},	// 36:	repne ss:
			{0x3e, v30repne_segprefix_ds},	// 3E:	repne ds:
			{0x54, v30push_sp},				// 54:	push	sp
			{0x5c, v30pop_sp},				// 5C:	pop		sp
			{0x63, v30_reserved},			// 63:	reserved
			{0x64, v30_reserved},			// 64:	reserved
			{0x65, v30_reserved},			// 65:	reserved
			{0x66, v30_reserved},			// 66:	reserved
			{0x67, v30_reserved},			// 67:	reserved
			{0x8e, v30mov_seg_ea},			// 8E:	mov		segrem, EA
			{0x9c, v30_pushf},				// 9C:	pushf
			{0x9d, v30_popf},				// 9D:	popf
			{0xc0, v30shift_ea8_data8},		// C0:	shift	EA8, DATA8
			{0xc1, v30shift_ea16_data8},	// C1:	shift	EA16, DATA8
			{0xd2, v30shift_ea8_cl},		// D2:	shift EA8, cl
			{0xd3, v30shift_ea16_cl},		// D3:	shift EA16, cl
			{0xd4, v30_aam},				// D4:	AAM
			{0xd5, v30_aad},				// D5:	AAD
			{0xd6, v30_xlat},				// D6:	xlat (8086/V30)
			{0xf2, v30_repne},				// F2:	repne
			{0xf3, v30_repe}				// F3:	repe
};


// ---------------------------------------------------------------------------

static void v30patching(void (*op[])(void), const V30PATCH *patch, int cnt) {

	do {
		op[patch->opnum] = patch->v30opcode;
		patch++;
	} while(--cnt);
}

#define	V30PATCHING(a, b)	v30patching(a, b, sizeof(b)/sizeof(V30PATCH))

void v30cinit(void) {

	CopyMemory(v30op, i286op, sizeof(v30op));
	V30PATCHING(v30op, v30patch_op);
	CopyMemory(v30op_repne, i286op_repne, sizeof(v30op_repne));
	V30PATCHING(v30op_repne, v30patch_repne);
	CopyMemory(v30op_repe, i286op_repe, sizeof(v30op_repe));
	V30PATCHING(v30op_repe, v30patch_repe);
}

void v30c(void) {

	UINT	opcode;

	if (I286_TRAP) {
		do {
			GET_PCBYTE(opcode);
			v30op[opcode]();
			if (I286_TRAP) {
				i286c_interrupt(1);
			}
			dmap_v30();
		} while(I286_REMCLOCK > 0);
	}
	else if (dmac.working) {
		do {
			GET_PCBYTE(opcode);
			v30op[opcode]();
			dmap_v30();
		} while(I286_REMCLOCK > 0);
	}
	else {
		do {
			GET_PCBYTE(opcode);
			v30op[opcode]();
		} while(I286_REMCLOCK > 0);
	}
}

void v30c_step(void) {

	UINT	opcode;

	I286_OV = I286_FLAG & O_FLAG;
	I286_FLAG &= ~(O_FLAG);

	GET_PCBYTE(opcode);
	v30op[opcode]();

	I286_FLAG &= ~(O_FLAG);
	if (I286_OV) {
		I286_FLAG |= (O_FLAG);
	}
	dmap_v30();
}


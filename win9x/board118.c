#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"board118.h"
#include	"cs4231io.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"s98.h"
#include	"juliet.h"
#include	"keydisp.h"


// ROMEO‘Î‰ž”Å PC-9801-118

static void romeo_restore(BOOL opna) {

	REG8	i;

	for (i=0x30; i<0xa0; i++) {
		juliet_YMF288A(i, opn.reg[i]);
	}
	for (i=0; i<3; i++) {
		juliet_YMF288A((UINT8)(i + 0xa4), opn.reg[i + 0x0a4]);
		juliet_YMF288A((UINT8)(i + 0xa0), opn.reg[i + 0x0a0]);
		juliet_YMF288A((UINT8)(i + 0xb0), opn.reg[i + 0x0b0]);
		juliet_YMF288A((UINT8)(i + 0xb4), opn.reg[i + 0x0b4]);
	}
	if (opna) {
		for (i=0x30; i<0xa0; i++) {
			juliet_YMF288B(i, opn.reg[i + 0x100]);
		}
		for (i=0; i<3; i++) {
			juliet_YMF288B((UINT8)(i + 0xa4), opn.reg[i + 0x1a4]);
			juliet_YMF288B((UINT8)(i + 0xa0), opn.reg[i + 0x1a0]);
			juliet_YMF288B((UINT8)(i + 0xb0), opn.reg[i + 0x1b0]);
			juliet_YMF288B((UINT8)(i + 0xb4), opn.reg[i + 0x1b4]);
		}
		juliet_YMF288A(0x11, opn.reg[0x11]);
		juliet_YMF288A(0x18, opn.reg[0x18]);
		juliet_YMF288A(0x19, opn.reg[0x19]);
		juliet_YMF288A(0x1a, opn.reg[0x1a]);
		juliet_YMF288A(0x1b, opn.reg[0x1b]);
		juliet_YMF288A(0x1c, opn.reg[0x1c]);
		juliet_YMF288A(0x1d, opn.reg[0x1d]);
	}
	for (i=0; i<0x0e; i++) {
		juliet_YMF288A(i, ((BYTE *)&psg1.reg)[i]);
	}
}


// ----

static void IOOUTCALL ymf_o188(UINT port, REG8 dat) {

	opn.opnreg = dat;
	(void)port;
}

static void IOOUTCALL ymf_o18a(UINT port, REG8 dat) {

	S98_put(NORMAL2608, opn.opnreg, dat);
	if (opn.opnreg < 0x10) {
		if (opn.opnreg != 0x0e) {
			psggen_setreg(&psg1, opn.opnreg, dat);
		}
	}
	else {
		if (opn.opnreg < 0x20) {
			rhythm_setreg(&rhythm, opn.opnreg, dat);
		}
		else if (opn.opnreg < 0x30) {
			if (opn.opnreg == 0x28) {
				if ((dat & 0x0f) < 3) {
					opngen_keyon(dat & 0x0f, dat);
				}
				else if (((dat & 0x0f) != 3) &&
						((dat & 0x0f) < 7)) {
					opngen_keyon((dat & 0x07) - 1, dat);
				}
			}
			else {
				fmtimer_setreg(opn.opnreg, dat);
			}
		}
		else if (opn.opnreg < 0xc0) {
			opngen_setreg(0, opn.opnreg, dat);
		}
		opn.reg[opn.opnreg] = dat;
	}
	(void)port;
}

static void IOOUTCALL ymf_o18c(UINT port, REG8 dat) {

	opn.extreg = dat;
	(void)port;
}

static void IOOUTCALL ymf_o18e(UINT port, REG8 dat) {

	S98_put(EXTEND2608, opn.extreg, dat);
	opn.reg[opn.extreg + 0x100] = dat;
	if (opn.extreg >= 0x30) {
		opngen_setreg(3, opn.extreg, dat);
	}
	else {
		if (opn.extreg == 0x10) {
			if (!(dat & 0x80)) {
				opn.adpcmmask = ~(dat & 0x1c);
			}
		}
	}
	(void)port;
}

static REG8 IOINPCALL ymf_i188(UINT port) {

	(void)port;
	return(fmtimer.status);
}

static REG8 IOINPCALL ymf_i18a(UINT port) {

	if (opn.opnreg == 0x0e) {
		return(fmboard_getjoy(&psg1));
	}
	else if (opn.opnreg < 0x10) {
		return(psggen_getreg(&psg1, opn.opnreg));
	}
	(void)port;
	return(opn.reg[opn.opnreg]);
}

static REG8 IOINPCALL ymf_i18c(UINT port) {

	if (opn.extend) {
		return(fmtimer.status & 3);
	}
	(void)port;
	return(0xff);
}

static REG8 IOINPCALL ymf_i18e(UINT port) {

	if (opn.extend) {
		return(opn.reg[opn.opnreg]);
	}
	(void)port;
	return(0xff);
}

static void extendchannel(REG8 enable) {

	opn.extend = enable;
	if (enable) {
		opn.channels = 6;
		opngen_setcfg(6, OPN_STEREO | 0x007);
	}
	else {
		opn.channels = 3;
		opngen_setcfg(3, OPN_MONORAL | 0x007);
		rhythm_setreg(&rhythm, 0x10, 0xff);
	}
}

static void IOOUTCALL ymf_oa460(UINT port, REG8 dat) {

	cs4231.extfunc = dat;
	extendchannel((REG8)(dat & 1));
	(void)port;
}

static REG8 IOINPCALL ymf_ia460(UINT port) {

	(void)port;
	return(0x80 | (cs4231.extfunc & 1));
}


// ---- with romeo

static void IOOUTCALL ymfr_o18a(UINT port, REG8 dat) {

	S98_put(NORMAL2608, opn.opnreg, dat);
	if (opn.opnreg < 0x10) {
		if (opn.opnreg != 0x0e) {
			*((BYTE *)(&psg1.reg) + opn.opnreg) = dat;
			juliet_YMF288A(opn.opnreg, dat);
			if (opn.opnreg == 0x07) {
				keydisp_psgmix(&psg1);
			}
			else if ((opn.opnreg == 0x08) || (opn.opnreg == 0x09) ||
					(opn.opnreg == 0x0a)) {
				keydisp_psgvol(&psg1, (BYTE)(opn.opnreg - 8));
			}
		}
	}
	else {
		if (opn.opnreg < 0x20) {
			juliet_YMF288A(opn.opnreg, dat);
		}
		else if (opn.opnreg < 0x30) {
			if (opn.opnreg == 0x28) {
				juliet_YMF288A(opn.opnreg, dat);
				if ((dat & 0x0f) < 3) {
					keydisp_fmkeyon((BYTE)(dat & 0x0f), dat);
				}
				else if (((dat & 0x0f) != 3) && ((dat & 0x0f) < 7)) {
					keydisp_fmkeyon((UINT8)((dat & 0x0f) - 1), dat);
				}
			}
			else {
				fmtimer_setreg(opn.opnreg, dat);
			}
		}
		else if (opn.opnreg < 0xc0) {
			juliet_YMF288A(opn.opnreg, dat);
		}
		opn.reg[opn.opnreg] = dat;
	}
	(void)port;
}

static void IOOUTCALL ymfr_o18e(UINT port, REG8 dat) {

	S98_put(EXTEND2608, opn.extreg, dat);
	opn.reg[opn.extreg + 0x100] = dat;
	if (opn.extreg >= 0x30) {
		juliet_YMF288B(opn.extreg, dat);
	}
	else {
		if (opn.extreg == 0x10) {
			if (!(dat & 0x80)) {
				opn.adpcmmask = ~(dat & 0x1c);
			}
		}
	}
	(void)port;
}


// ----

static const IOOUT ymf_o[4] = {
			ymf_o188,	ymf_o18a,	ymf_o18c,	ymf_o18e};

static const IOINP ymf_i[4] = {
			ymf_i188,	ymf_i18a,	ymf_i18c,	ymf_i18e};

static const IOOUT ymfr_o[4] = {
			ymf_o188,	ymfr_o18a,	ymf_o18c,	ymfr_o18e};


void board118_reset(void) {

	fmtimer_reset(0xc0);
	opngen_setcfg(3, OPN_STEREO | 0x038);
	cs4231io_reset();
	soundrom_load(0xcc000, "118");
	fmboard_extreg(extendchannel);
	juliet_YMF288Reset();
}

void board118_bind(void) {

	if (juliet_YMF288IsEnable()) {
		juliet_YMF288A(0x22, 0x00);
		juliet_YMF288A(0x29, 0x80);
		juliet_YMF288A(0x10, 0xbf);
		juliet_YMF288A(0x11, 0x30);
		Sleep(100);
		romeo_restore(TRUE);
		cbuscore_attachsndex(0x188, ymfr_o, ymf_i);
	}
	else {
		fmboard_fmrestore(0, 0);
		fmboard_fmrestore(3, 1);
		psggen_restore(&psg1);
		fmboard_rhyrestore(&rhythm, 0);
		sound_streamregist(&opngen, (SOUNDCB)opngen_getpcm);
		sound_streamregist(&psg1, (SOUNDCB)psggen_getpcm);
		sound_streamregist(&rhythm, (SOUNDCB)rhythm_getpcm);
		cbuscore_attachsndex(0x188, ymf_o, ymf_i);
	}
	cs4231io_bind();
	iocore_attachout(0xa460, ymf_oa460);
	iocore_attachinp(0xa460, ymf_ia460);
}


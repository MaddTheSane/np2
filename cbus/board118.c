#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"board118.h"
#include	"cs4231io.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"s98.h"


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

	(void)port;
	if (opn.opnreg == 0x0e) {
		return(fmboard_getjoy(&psg1));
	}
	else if (opn.opnreg < 0x10) {
		return(psggen_getreg(&psg1, opn.opnreg));
	}
#if 1
	else if (opn.opnreg == 0xff) {
		return(1);
	}
	return(opn.opnreg);
#else
	return(opn.reg[opn.opnreg]);
#endif
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


// ----

static const IOOUT ymf_o[4] = {
			ymf_o188,	ymf_o18a,	ymf_o18c,	ymf_o18e};

static const IOINP ymf_i[4] = {
			ymf_i188,	ymf_i18a,	ymf_i18c,	ymf_i18e};


void board118_reset(void) {

	fmtimer_reset(0xc0);
	opngen_setcfg(3, OPN_STEREO | 0x038);
	cs4231io_reset();
	soundrom_load(0xcc000, OEMTEXT("118"));
	fmboard_extreg(extendchannel);
}

void board118_bind(void) {

	fmboard_fmrestore(0, 0);
	fmboard_fmrestore(3, 1);
	psggen_restore(&psg1);
	fmboard_rhyrestore(&rhythm, 0);
	sound_streamregist(&opngen, (SOUNDCB)opngen_getpcm);
	sound_streamregist(&psg1, (SOUNDCB)psggen_getpcm);
	rhythm_bind(&rhythm);
	cs4231io_bind();
	cbuscore_attachsndex(0x188, ymf_o, ymf_i);
	iocore_attachout(0xa460, ymf_oa460);
	iocore_attachinp(0xa460, ymf_ia460);
}


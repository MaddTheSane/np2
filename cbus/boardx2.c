#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"boardx2.h"
#include	"pcm86io.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"s98.h"


static void IOOUTCALL opn_o088(UINT port, REG8 dat) {

	opn.opn2reg = dat;
	(void)port;
}

static void IOOUTCALL opn_o08a(UINT port, REG8 dat) {

	if (opn.opn2reg < 0x10) {
		if (opn.opn2reg != 0x0e) {
			psggen_setreg(&psg1, opn.opn2reg, dat);
		}
	}
	else {
		if (opn.opn2reg < 0x30) {
			if (opn.opn2reg == 0x28) {
				if ((dat & 0x0f) < 3) {
					opngen_keyon(dat & 0x0f, dat);
				}
			}
			else {
				fmtimer_setreg(opn.opn2reg, dat);
				if (opn.opn2reg == 0x27) {
					opnch[2].extop = dat & 0xc0;
				}
			}
		}
		else if (opn.opn2reg < 0xc0) {
			opngen_setreg(0, opn.opn2reg, dat);
		}
		opn.reg[opn.opn2reg + 0x200] = dat;
	}
	(void)port;
}

static REG8 IOINPCALL opn_i088(UINT port) {

	(void)port;
	return(fmtimer.status);
}

static REG8 IOINPCALL opn_i08a(UINT port) {

	if (opn.opn2reg == 0x0e) {
		return(0xff);
	}
	if (opn.opn2reg < 0x10) {
		return(psggen_getreg(&psg1, opn.opn2reg));
	}
	(void)port;
	return(0xff);
}


// ----

static void IOOUTCALL opna_o188(UINT port, REG8 dat) {

	opn.opnreg = dat;
	(void)port;
}

static void IOOUTCALL opna_o18a(UINT port, REG8 dat) {

	S98_put(NORMAL2608, opn.opnreg, dat);
	if (opn.opnreg < 0x10) {
		if (opn.opnreg != 0x0e) {
			psggen_setreg(&psg2, opn.opnreg, dat);
		}
	}
	else {
		if (opn.opnreg < 0x20) {
			if (opn.extend) {
				rhythm_setreg(&rhythm, opn.opnreg, dat);
			}
		}
		else if (opn.opnreg < 0x30) {
			if (opn.opnreg == 0x28) {
				if ((dat & 0x0f) < 3) {
					opngen_keyon((dat & 0x0f) + 3, dat);
				}
				else if (((dat & 0x0f) != 3) &&
						((dat & 0x0f) < 7)) {
					opngen_keyon((dat & 0x0f) + 2, dat);
				}
			}
			else {
				fmtimer_setreg(opn.opnreg, dat);
				if (opn.opnreg == 0x27) {
					opnch[2].extop = dat & 0xc0;
				}
			}
		}
		else if (opn.opnreg < 0xc0) {
			opngen_setreg(3, opn.opnreg, dat);
		}
		opn.reg[opn.opnreg] = dat;
	}
	(void)port;
}

static void IOOUTCALL opna_o18c(UINT port, REG8 dat) {

	opn.extreg = dat;
	(void)port;
}

static void IOOUTCALL opna_o18e(UINT port, REG8 dat) {

	S98_put(EXTEND2608, opn.extreg, dat);
	opn.reg[opn.extreg + 0x100] = dat;
	if (opn.extreg >= 0x30) {
		opngen_setreg(6, opn.extreg, dat);
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

static REG8 IOINPCALL opna_i188(UINT port) {

	(void)port;
	return(fmtimer.status);
}

static REG8 IOINPCALL opna_i18a(UINT port) {

	if (opn.opnreg == 0x0e) {
		return(fmboard_getjoy(&psg2));
	}
	else if (opn.opnreg < 0x10) {
		return(psggen_getreg(&psg2, opn.opnreg));
	}
	(void)port;
	return(opn.reg[opn.opnreg]);
}

static REG8 IOINPCALL opna_i18c(UINT port) {

	if (opn.extend) {
		return((fmtimer.status & 3) | (opn.adpcmmask & 8));
	}
	(void)port;
	return(0xff);
}

static REG8 IOINPCALL opna_i18e(UINT port) {

	if (opn.extend) {
		return(opn.reg[opn.opnreg]);
	}
	(void)port;
	return(0xff);
}

static void extendchannel(REG8 enable) {

	opn.extend = enable;
	if (enable) {
		opn.channels = 9;
		opngen_setcfg(9, OPN_STEREO | 0x038);
	}
	else {
		opn.channels = 6;
		opngen_setcfg(6, OPN_MONORAL | 0x038);
		rhythm_setreg(&rhythm, 0x10, 0xff);
	}
}


// ----

static const IOOUT opn_o[4] = {
			opn_o088,	opn_o08a,	NULL,		NULL};

static const IOINP opn_i[4] = {
			opn_i088,	opn_i08a,	NULL,		NULL};

static const IOOUT opna_o[4] = {
			opna_o188,	opna_o18a,	opna_o18c,	opna_o18e};

static const IOINP opna_i[4] = {
			opna_i188,	opna_i18a,	opna_i18c,	opna_i18e};


void boardx2_reset(void) {

	fmtimer_reset(0xc0);
	opn.channels = 6;
	opngen_setcfg(6, OPN_STEREO | 0x1c0);
	soundrom_load(0xcc000, "86");
	fmboard_extreg(extendchannel);
}

void boardx2_bind(void) {

	sound_streamregist(&opngen, (SOUNDCB)opngen_getpcm);
	sound_streamregist(&psg1, (SOUNDCB)psggen_getpcm);
	sound_streamregist(&psg2, (SOUNDCB)psggen_getpcm);
	sound_streamregist(&rhythm, (SOUNDCB)rhythm_getpcm);
	pcm86io_bind();
	cbuscore_attachsndex(0x088, opn_o, opn_i);
	cbuscore_attachsndex(0x188, opna_o, opna_i);
}


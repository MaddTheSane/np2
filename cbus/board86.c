#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"board86.h"
#include	"pcm86io.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"s98.h"


static void IOOUTCALL opna_o188(UINT port, BYTE dat) {

	opn.opnreg = dat;
	(void)port;
}

static void IOOUTCALL opna_o18a(UINT port, BYTE dat) {

	S98_put(NORMAL2608, opn.opnreg, dat);
	if (opn.opnreg < 0x10) {
		if (opn.opnreg != 0x0e) {
			psggen_setreg(&psg1, opn.opnreg, dat);
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
					opngen_keyon(dat & 0x0f, dat);
				}
				else if (((dat & 0x0f) != 3) &&
						((dat & 0x0f) < 7)) {
					opngen_keyon((dat & 0x07) - 1, dat);
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
			opngen_setreg(0, opn.opnreg, dat);
		}
		opn.reg[opn.opnreg] = dat;
	}
	(void)port;
}

static void IOOUTCALL opna_o18c(UINT port, BYTE dat) {

	opn.extreg = dat;
	(void)port;
}

static void IOOUTCALL opna_o18e(UINT port, BYTE dat) {

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

static BYTE IOINPCALL opna_i188(UINT port) {

	(void)port;
	return(fmtimer.status);
}

static BYTE IOINPCALL opna_i18a(UINT port) {

	if (opn.opnreg == 0x0e) {
		return(fmboard_getjoy(&psg1));
	}
	else if (opn.opnreg < 0x10) {
		return(psggen_getreg(&psg1, opn.opnreg));
	}
	(void)port;
	return(opn.reg[opn.opnreg]);
}

static BYTE IOINPCALL opna_i18c(UINT port) {

	if (opn.extend) {
		return((fmtimer.status & 3) | (opn.adpcmmask & 8));
	}
	(void)port;
	return(0xff);
}

static BYTE IOINPCALL opna_i18e(UINT port) {

	if (opn.extend) {
		return(opn.reg[opn.opnreg]);
	}
	(void)port;
	return(0xff);
}

static void extendchannel(BYTE enable) {

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


// ----

static const IOOUT opna_o[4] = {
			opna_o188,	opna_o18a,	opna_o18c,	opna_o18e};

static const IOINP opna_i[4] = {
			opna_i188,	opna_i18a,	opna_i18c,	opna_i18e};


void board86_reset(void) {

	fmtimer_reset((BYTE)((np2cfg.snd86opt & 0x10) |
						((np2cfg.snd86opt & 0x4) << 5) |
						((np2cfg.snd86opt & 0x8) << 3)));
	opngen_setcfg(3, OPN_STEREO | 0x038);
	if (np2cfg.snd86opt & 2) {
		soundrom_load(0xcc000, "86");
	}
	opn.base = (np2cfg.snd86opt & 0x01)?0x000:0x100;
	fmboard_extreg(extendchannel);
}

void board86_bind(void) {

	sound_streamregist(&opngen, (SOUNDCB)opngen_getpcm);
	sound_streamregist(&psg1, (SOUNDCB)psggen_getpcm);
	sound_streamregist(&rhythm, (SOUNDCB)rhythm_getpcm);
	pcm86io_bind();
	cbuscore_attachsndex(0x188 + opn.base, opna_o, opna_i);
}


// ---- + chibioto

static void IOOUTCALL opnac_o18e(UINT port, BYTE dat) {

	S98_put(EXTEND2608, opn.extreg, dat);
	opn.reg[opn.extreg + 0x100] = dat;
	if (opn.extreg >= 0x30) {
		opngen_setreg(3, opn.extreg, dat);
	}
	else {
		if (opn.extreg < 0x12) {
			sound_sync();
			adpcm_setreg(&adpcm, opn.extreg, dat);
		}
	}
	(void)port;
}

static BYTE IOINPCALL opnac_i18c(UINT port) {

	if (opn.extend) {
		return((fmtimer.status & 3) | adpcm_status(&adpcm));
//		return((fmtimer.status & 3) | (opn.adpcmmask & 8));
	}
	(void)port;
	return(0xff);
}

static BYTE IOINPCALL opnac_i18e(UINT port) {

	if (opn.extend) {
		if (opn.extreg == 0x08) {
			return(adpcm_readsample(&adpcm));
		}
		return(opn.reg[opn.opnreg]);
	}
	(void)port;
	return(0xff);
}

static const IOOUT opnac_o[4] = {
			opna_o188,	opna_o18a,	opna_o18c,	opnac_o18e};

static const IOINP opnac_i[4] = {
			opna_i188,	opna_i18a,	opnac_i18c,	opnac_i18e};


void board86c_bind(void) {

	sound_streamregist(&opngen, (SOUNDCB)opngen_getpcm);
	sound_streamregist(&psg1, (SOUNDCB)psggen_getpcm);
	sound_streamregist(&rhythm, (SOUNDCB)rhythm_getpcm);
	sound_streamregist(&adpcm, (SOUNDCB)adpcm_getpcm);
	pcm86io_bind();
	cbuscore_attachsndex(0x188 + opn.base, opnac_o, opnac_i);
}


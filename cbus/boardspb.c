#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"boardspb.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"beep.h"
#include	"s98.h"


static void IOOUTCALL spb_o188(UINT port, BYTE dat) {

	opn.opnreg = dat;
	(void)port;
}

static void IOOUTCALL spb_o18a(UINT port, BYTE dat) {

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
					opngen_keyon((dat & 0x0f) - 1, dat);
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

static void IOOUTCALL spb_o18c(UINT port, BYTE dat) {

	opn.extreg = dat;
	(void)port;
}

static void IOOUTCALL spb_o18e(UINT port, BYTE dat) {

	S98_put(EXTEND2608, opn.extreg, dat);
	opn.reg[opn.extreg + 0x100] = dat;
	if (opn.extreg >= 0x30) {
		opngen_setreg(3, opn.extreg, dat);
	}
	else if (opn.extreg < 0x12) {
		sound_sync();
		adpcm_setreg(&adpcm, opn.extreg, dat);
	}
	(void)port;
}

static BYTE IOINPCALL spb_i188(UINT port) {

	(void)port;
	return((fmtimer.status & 3) | adpcm_status(&adpcm));
}

static BYTE IOINPCALL spb_i18a(UINT port) {

	if (opn.opnreg == 0x0e) {
		return(fmboard_getjoy(&psg1));
	}
	else if (opn.opnreg < 0x10) {
		return(psggen_getreg(&psg1, opn.opnreg));
	}
	(void)port;
	return(opn.reg[opn.opnreg]);
}

static BYTE IOINPCALL spb_i18e(UINT port) {

	if (opn.extreg == 0x08) {
		return(adpcm_readsample(&adpcm));
	}
	(void)port;
	return(opn.reg[opn.opnreg]);
}


// ---- spark board

static void IOOUTCALL spr_o588(UINT port, BYTE dat) {

	opn.opn2reg = dat;
	(void)port;
}

static void IOOUTCALL spr_o58a(UINT port, BYTE dat) {

	if (opn.opn2reg < 0x30) {
		if (opn.opn2reg == 0x28) {
			if ((dat & 0x0f) < 3) {
				opngen_keyon((dat & 0x0f) + 6, dat);
			}
			else if (((dat & 0x0f) != 3) &&
					((dat & 0x0f) < 7)) {
				opngen_keyon((dat & 0x0f) + 5, dat);
			}
		}
		else {
			if (opn.opn2reg == 0x27) {
				opnch[8].extop = dat & 0xc0;
			}
		}
	}
	else if (opn.opn2reg < 0xc0) {
		opngen_setreg(6, opn.opn2reg, dat);
	}
	opn.reg[opn.opn2reg + 0x200] = dat;
	(void)port;
}

static void IOOUTCALL spr_o58c(UINT port, BYTE dat) {

	opn.ext2reg = dat;
	(void)port;
}

static void IOOUTCALL spr_o58e(UINT port, BYTE dat) {

	opn.reg[opn.ext2reg + 0x300] = dat;
	if (opn.ext2reg >= 0x30) {
		sound_sync();
		opngen_setreg(9, opn.ext2reg, dat);
	}
	(void)port;
}

static BYTE IOINPCALL spr_i588(UINT port) {

	(void)port;
	return(fmtimer.status);
}

static BYTE IOINPCALL spr_i58a(UINT port) {

	if (opn.opn2reg >= 0x20) {
		return(opn.reg[opn.opn2reg + 0x200]);
	}
	(void)port;
	return(0xff);
}

static BYTE IOINPCALL spr_i58c(UINT port) {

	(void)port;
	return(fmtimer.status & 3);
}

static BYTE IOINPCALL spr_i58e(UINT port) {

	(void)port;
	return(opn.reg[opn.opn2reg + 0x200]);
}


// ----

static const IOOUT spb_o[4] = {
			spb_o188,	spb_o18a,	spb_o18c,	spb_o18e};

static const IOINP spb_i[4] = {
			spb_i188,	spb_i18a,	spb_i188,	spb_i18e};


void boardspb_reset(void) {

	fmtimer_reset((BYTE)(np2cfg.spbopt & 0xc0));
	opn.channels = 6;
	opngen_setcfg(6, OPN_STEREO | 0x03f);
	soundrom_loadex((BYTE)(np2cfg.spbopt & 7), "SPB");
	opn.base = ((np2cfg.spbopt & 0x10)?0x000:0x100);
}

void boardspb_bind(void) {

	sound_streamregist(&opngen, (SOUNDCB)opngen_getpcm);
	sound_streamregist(&psg1, (SOUNDCB)psggen_getpcm);
	sound_streamregist(&rhythm, (SOUNDCB)rhythm_getpcm);
	sound_streamregist(&adpcm, (SOUNDCB)adpcm_getpcm);
	cbuscore_attachsndex(0x188 - opn.base, spb_o, spb_i);
}


// ----

static const IOOUT spr_o[4] = {
			spr_o588,	spr_o58a,	spr_o58c,	spr_o58e};

static const IOINP spr_i[4] = {
			spr_i588,	spr_i58a,	spr_i58c,	spr_i58e};


void boardspr_reset(void) {

	fmtimer_reset((BYTE)(np2cfg.spbopt & 0xc0));
	opn.reg[0x2ff] = 0;
	opn.channels = 12;
	opngen_setcfg(12, OPN_STEREO | 0x03f);
	soundrom_loadex((BYTE)(np2cfg.spbopt & 7), "SPB");
	opn.base = (np2cfg.spbopt & 0x10)?0x000:0x100;
}

void boardspr_bind(void) {

	sound_streamregist(&opngen, (SOUNDCB)opngen_getpcm);
	sound_streamregist(&psg1, (SOUNDCB)psggen_getpcm);
	sound_streamregist(&rhythm, (SOUNDCB)rhythm_getpcm);
	sound_streamregist(&adpcm, (SOUNDCB)adpcm_getpcm);
	cbuscore_attachsndex(0x188 - opn.base, spb_o, spb_i);
	cbuscore_attachsndex(0x588 - opn.base, spr_o, spr_i);
}


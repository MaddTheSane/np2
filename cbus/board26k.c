#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"board26k.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"s98.h"


static void IOOUTCALL opn_o188(UINT port, REG8 dat) {

	opn.opnreg = dat;
	(void)port;
}

static void IOOUTCALL opn_o18a(UINT port, REG8 dat) {

	S98_put(NORMAL2608, opn.opnreg, dat);
	if (opn.opnreg < 0x10) {
		if (opn.opnreg != 0x0e) {
			psggen_setreg(&psg1, opn.opnreg, dat);
		}
	}
	else {
		if (opn.opnreg < 0x30) {
			if (opn.opnreg == 0x28) {
				if ((dat & 0x0f) < 3) {
					opngen_keyon(dat & 0x0f, dat);
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

static REG8 IOINPCALL opn_i188(UINT port) {

	(void)port;
#if 1							// ドラッケンで未定義フラグ見てる　テスト終了
	return(fmtimer.status);
#else
	return(fmtimer.status | 0x7c);
#endif
}

static REG8 IOINPCALL opn_i18a(UINT port) {

	if (opn.opnreg == 0x0e) {
		return(fmboard_getjoy(&psg1));
	}
	if (opn.opnreg < 0x10) {
		return(psggen_getreg(&psg1, opn.opnreg));
	}
	(void)port;
	return(0xff);
}


// ----

static const IOOUT opn_o[4] = {
			opn_o188,	opn_o18a,	NULL,		NULL};

static const IOINP opn_i[4] = {
			opn_i188,	opn_i18a,	NULL,		NULL};


void board26k_reset(void) {

	opngen_setcfg(3, 0);
	fmtimer_reset(np2cfg.snd26opt & 0xc0);
	soundrom_loadex(np2cfg.snd26opt & 7, "26");
	opn.base = (np2cfg.snd26opt & 0x10)?0x000:0x100;
}

void board26k_bind(void) {

	fmboard_fmrestore(0, 0);
	psggen_restore(&psg1);
	sound_streamregist(&opngen, (SOUNDCB)opngen_getpcm);
	sound_streamregist(&psg1, (SOUNDCB)psggen_getpcm);
	cbuscore_attachsndex(0x188 - opn.base, opn_o, opn_i);
}


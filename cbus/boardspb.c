#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"boardspb.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"s98.h"
#include "sound/soundrom.h"

static void IOOUTCALL spb_o188(UINT port, REG8 dat) {

	g_opn.addr1l = dat;
//	g_opn.data1 = dat;
	(void)port;
}

static void IOOUTCALL spb_o18a(UINT port, REG8 dat) {

	UINT	addr;

//	g_opn.data1 = dat;
	addr = g_opn.addr1l;
	S98_put(NORMAL2608, addr, dat);
	g_opn.reg[addr] = dat;
	if (addr < 0x10) {
		psggen_setreg(&g_psg1, addr, dat);
	}
	else {
		if (addr < 0x20) {
			rhythm_setreg(&g_rhythm, addr, dat);
		}
		else if (addr < 0x30) {
			if (addr == 0x28) {
				if ((dat & 0x0f) < 3) {
					opngen_keyon(dat & 0x0f, dat);
				}
				else if (((dat & 0x0f) != 3) &&
						((dat & 0x0f) < 7)) {
					opngen_keyon((dat & 0x0f) - 1, dat);
				}
			}
			else {
				fmtimer_setreg(addr, dat);
				if (addr == 0x27) {
					opngen.opnch[2].extop = dat & 0xc0;
				}
			}
		}
		else if (addr < 0xc0) {
			opngen_setreg(0, addr, dat);
		}
	}
	(void)port;
}

static void IOOUTCALL spb_o18c(UINT port, REG8 dat) {

	g_opn.addr1h = dat;
//	g_opn.data1 = dat;
	(void)port;
}

static void IOOUTCALL spb_o18e(UINT port, REG8 dat) {

	UINT	addr;

//	g_opn.data1 = dat;
	addr = g_opn.addr1h;
	S98_put(EXTEND2608, addr, dat);
	g_opn.reg[addr + 0x100] = dat;
	if (addr >= 0x30) {
		opngen_setreg(3, addr, dat);
	}
	else if (addr < 0x12) {
		adpcm_setreg(&g_adpcm, addr, dat);
	}
	(void)port;
}

static REG8 IOINPCALL spb_i188(UINT port) {

	(void)port;
	return((g_fmtimer.status & 3) | adpcm_status(&g_adpcm));
}

static REG8 IOINPCALL spb_i18a(UINT port) {

	UINT	addr;

	addr = g_opn.addr1l;
	if (addr == 0x0e) {
		return(fmboard_getjoy(&g_psg1));
	}
	else if (addr == 0xff) {
		return(1);
	}
	else {
		(void)port;
		return(g_opn.reg[addr]);
	}
}

static REG8 IOINPCALL spb_i18e(UINT port) {

	UINT	addr;

	addr = g_opn.addr1h;
	if (addr == 0x08) {
		return(adpcm_readsample(&g_adpcm));
	}
	else if (addr == 0x0f) {
		return(g_opn.reg[addr + 0x100]);
	}
	else {
		(void)port;
		return(g_opn.reg[g_opn.addr1l]);
	}
}


// ---- spark board

static void IOOUTCALL spr_o588(UINT port, REG8 dat) {

	g_opn.addr2l = dat;
//	g_opn.data2 = dat;
	(void)port;
}

static void IOOUTCALL spr_o58a(UINT port, REG8 dat) {

	UINT	addr;

//	g_opn.data2l = dat;
	addr = g_opn.addr2l;
	g_opn.reg[addr + 0x200] = dat;
	if (addr < 0x30) {
		if (addr == 0x28) {
			if ((dat & 0x0f) < 3) {
				opngen_keyon((dat & 0x0f) + 6, dat);
			}
			else if (((dat & 0x0f) != 3) &&
					((dat & 0x0f) < 7)) {
				opngen_keyon((dat & 0x0f) + 5, dat);
			}
		}
		else {
			if (addr == 0x27) {
				opngen.opnch[8].extop = dat & 0xc0;
			}
		}
	}
	else if (addr < 0xc0) {
		opngen_setreg(6, addr, dat);
	}
	(void)port;
}

static void IOOUTCALL spr_o58c(UINT port, REG8 dat) {

	g_opn.addr2h = dat;
//	g_opn.data2 = dat;
	(void)port;
}

static void IOOUTCALL spr_o58e(UINT port, REG8 dat) {

	UINT	addr;

//	g_opn.data2 = dat;
	addr = g_opn.addr2h;
	g_opn.reg[addr + 0x300] = dat;
	if (addr >= 0x30) {
		opngen_setreg(9, addr, dat);
	}
	(void)port;
}

static REG8 IOINPCALL spr_i588(UINT port) {

	(void)port;
	return(g_fmtimer.status);
}

static REG8 IOINPCALL spr_i58a(UINT port) {

	UINT	addr;

	addr = g_opn.addr2l;
	if ((addr >= 0x20) && (addr < 0xff)) {
		return(g_opn.reg[addr + 0x200]);
	}
	else if (addr == 0xff) {
		return(0);
	}
	else {
		(void)port;
//		return(g_opn.data2);
		return(0xff);
	}
}

static REG8 IOINPCALL spr_i58c(UINT port) {

	(void)port;
	return(g_fmtimer.status & 3);
}

static REG8 IOINPCALL spr_i58e(UINT port) {

	(void)port;
	return(g_opn.reg[g_opn.addr2l + 0x200]);
}


// ----

static const IOOUT spb_o[4] = {
			spb_o188,	spb_o18a,	spb_o18c,	spb_o18e};

static const IOINP spb_i[4] = {
			spb_i188,	spb_i18a,	spb_i188,	spb_i18e};


void boardspb_reset(const NP2CFG *pConfig) {

	fmtimer_reset(pConfig->spbopt & 0xc0);
	g_opn.channels = 6;
	opngen_setcfg(6, OPN_STEREO | 0x03f);
	soundrom_loadex(pConfig->spbopt & 7, OEMTEXT("SPB"));
	g_opn.base = ((pConfig->spbopt & 0x10)?0x000:0x100);
}

void boardspb_bind(void) {

	fmboard_fmrestore(&g_opn, 0, 0);
	fmboard_fmrestore(&g_opn, 3, 1);
	fmboard_psgrestore(&g_opn, &g_psg1, 0);
	fmboard_rhyrestore(&g_opn, &g_rhythm, 0);
	sound_streamregist(&opngen, (SOUNDCB)opngen_getpcmvr);
	sound_streamregist(&g_psg1, (SOUNDCB)psggen_getpcm);
	rhythm_bind(&g_rhythm);
	sound_streamregist(&g_adpcm, (SOUNDCB)adpcm_getpcm);
	cbuscore_attachsndex(0x188 - g_opn.base, spb_o, spb_i);
}


// ----

static const IOOUT spr_o[4] = {
			spr_o588,	spr_o58a,	spr_o58c,	spr_o58e};

static const IOINP spr_i[4] = {
			spr_i588,	spr_i58a,	spr_i58c,	spr_i58e};


void boardspr_reset(const NP2CFG *pConfig) {

	fmtimer_reset(pConfig->spbopt & 0xc0);
	g_opn.reg[0x2ff] = 0;
	g_opn.channels = 12;
	opngen_setcfg(12, OPN_STEREO | 0x03f);
	soundrom_loadex(pConfig->spbopt & 7, OEMTEXT("SPB"));
	g_opn.base = (pConfig->spbopt & 0x10)?0x000:0x100;
}

void boardspr_bind(void) {

	fmboard_fmrestore(&g_opn, 0, 0);
	fmboard_fmrestore(&g_opn, 3, 1);
	fmboard_fmrestore(&g_opn, 6, 2);
	fmboard_fmrestore(&g_opn, 9, 3);
	fmboard_psgrestore(&g_opn, &g_psg1, 0);
	fmboard_rhyrestore(&g_opn, &g_rhythm, 0);
	sound_streamregist(&opngen, (SOUNDCB)opngen_getpcmvr);
	sound_streamregist(&g_psg1, (SOUNDCB)psggen_getpcm);
	rhythm_bind(&g_rhythm);
	sound_streamregist(&g_adpcm, (SOUNDCB)adpcm_getpcm);
	cbuscore_attachsndex(0x188 - g_opn.base, spb_o, spb_i);
	cbuscore_attachsndex(0x588 - g_opn.base, spr_o, spr_i);
}


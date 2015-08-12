#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"boardx2.h"
#include	"pcm86io.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"s98.h"
#include "sound/soundrom.h"

static void IOOUTCALL opn_o088(UINT port, REG8 dat) {

	g_opn.addr2l = dat;
	g_opn.data2 = dat;
	(void)port;
}

static void IOOUTCALL opn_o08a(UINT port, REG8 dat) {

	UINT	addr;

	g_opn.data2 = dat;
	addr = g_opn.addr2l;
	g_opn.reg[addr + 0x200] = dat;
	if (addr < 0x10) {
		psggen_setreg(&g_psg1, addr, dat);
	}
	else {
		if (addr < 0x30) {
			if (addr == 0x28) {
				if ((dat & 0x0f) < 3) {
					opngen_keyon(&g_opngen, dat & 0x0f, dat);
				}
			}
			else {
				fmtimer_setreg(addr, dat);
				if (addr == 0x27) {
					g_opngen.opnch[2].extop = dat & 0xc0;
				}
			}
		}
		else if (addr < 0xc0) {
			opngen_setreg(&g_opngen, 0, addr, dat);
		}
	}
	(void)port;
}

static REG8 IOINPCALL opn_i088(UINT port) {

	(void)port;
	return(g_fmtimer.status);
}

static REG8 IOINPCALL opn_i08a(UINT port) {

	UINT	addr;

	addr = g_opn.addr2l;
	if (addr == 0x0e) {
		return(0xff);
	}
	if (addr < 0x10) {
		return g_opn.reg[addr + 0x200];
	}
	else {
		(void)port;
		return(g_opn.data2);
	}
}


// ----

static void IOOUTCALL opna_o188(UINT port, REG8 dat) {

	g_opn.addr1l = dat;
	g_opn.data1 = dat;
	(void)port;
}

static void IOOUTCALL opna_o18a(UINT port, REG8 dat) {

	UINT	addr;

	g_opn.data1 = dat;
	addr = g_opn.addr1l;
	S98_put(NORMAL2608, addr, dat);
	g_opn.reg[addr] = dat;
	if (addr < 0x10) {
		psggen_setreg(&g_psg2, addr, dat);
	}
	else {
		if (addr < 0x20) {
			if (g_opn.extend) {
				rhythm_setreg(&g_rhythm, addr, dat);
			}
		}
		else if (addr < 0x30) {
			if (addr == 0x28) {
				if ((dat & 0x0f) < 3) {
					opngen_keyon(&g_opngen, (dat & 0x0f) + 3, dat);
				}
				else if (((dat & 0x0f) != 3) &&
						((dat & 0x0f) < 7)) {
					opngen_keyon(&g_opngen, (dat & 0x0f) + 2, dat);
				}
			}
			else {
				fmtimer_setreg(addr, dat);
				if (addr == 0x27) {
					g_opngen.opnch[2].extop = dat & 0xc0;
				}
			}
		}
		else if (addr < 0xc0) {
			opngen_setreg(&g_opngen, 3, addr, dat);
		}
	}
	(void)port;
}

static void IOOUTCALL opna_o18c(UINT port, REG8 dat) {

	if (g_opn.extend) {
		g_opn.addr1h = dat;
		g_opn.data1 = dat;
	}
	(void)port;
}

static void IOOUTCALL opna_o18e(UINT port, REG8 dat) {

	UINT	addr;

	if (!g_opn.extend) {
		return;
	}
	addr = g_opn.addr1h;
	S98_put(EXTEND2608, addr, dat);
	g_opn.reg[addr + 0x100] = dat;
	if (addr >= 0x30) {
		opngen_setreg(&g_opngen, 6, addr, dat);
	}
	else {
		if (addr == 0x10) {
			if (!(dat & 0x80)) {
				g_opn.adpcmmask = ~(dat & 0x1c);
			}
		}
	}
	(void)port;
}

static REG8 IOINPCALL opna_i188(UINT port) {

	(void)port;
	return(g_fmtimer.status);
}

static REG8 IOINPCALL opna_i18a(UINT port) {

	UINT	addr;

	addr = g_opn.addr1l;
	if (addr == 0x0e) {
		return(fmboard_getjoy(&g_psg2));
	}
	else if (addr < 0x10) {
		return g_opn.reg[addr];
	}
	else if (addr == 0xff) {
		return(1);
	}
	else {
		(void)port;
		return(g_opn.data1);
	}
}

static REG8 IOINPCALL opna_i18c(UINT port) {

	if (g_opn.extend) {
		return((g_fmtimer.status & 3) | (g_opn.adpcmmask & 8));
	}
	(void)port;
	return(0xff);
}

static REG8 IOINPCALL opna_i18e(UINT port) {

	if (g_opn.extend) {
		UINT addr = g_opn.addr1h;
		if ((addr == 0x08) || (addr == 0x0f)) {
			return(g_opn.reg[addr + 0x100]);
		}
		return(g_opn.data1);
	}
	(void)port;
	return(0xff);
}

static void extendchannel(REG8 enable) {

	g_opn.extend = enable;
	if (enable) {
		g_opn.channels = 9;
		opngen_setcfg(&g_opngen, 9, OPN_STEREO | 0x038);
	}
	else {
		g_opn.channels = 6;
		opngen_setcfg(&g_opngen, 6, OPN_MONORAL | 0x038);
		rhythm_setreg(&g_rhythm, 0x10, 0xff);
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


void boardx2_reset(const NP2CFG *pConfig) {

	g_opn.reg[0x207] = 0xbf;
	g_opn.reg[0x20e] = 0xff;
	g_opn.reg[0x20f] = 0xff;

	fmtimer_reset(0xc0);
	g_opn.channels = 6;
	opngen_setcfg(&g_opngen, 6, OPN_STEREO | 0x1c0);
	soundrom_load(0xcc000, OEMTEXT("86"));
	fmboard_extreg(extendchannel);

	(void)pConfig;
}

void boardx2_bind(void) {

	fmboard_fmrestore(&g_opn, 0, 2);
	fmboard_fmrestore(&g_opn, 3, 0);
	fmboard_fmrestore(&g_opn, 6, 1);
	fmboard_psgrestore(&g_opn, &g_psg1, 2);
	fmboard_psgrestore(&g_opn, &g_psg2, 0);
	fmboard_rhyrestore(&g_opn, &g_rhythm, 0);
	sound_streamregist(&g_opngen, (SOUNDCB)opngen_getpcm);
	sound_streamregist(&g_psg1, (SOUNDCB)psggen_getpcm);
	sound_streamregist(&g_psg2, (SOUNDCB)psggen_getpcm);
	rhythm_bind(&g_rhythm);
	pcm86io_bind();
	cbuscore_attachsndex(0x088, opn_o, opn_i);
	cbuscore_attachsndex(0x188, opna_o, opna_i);
}


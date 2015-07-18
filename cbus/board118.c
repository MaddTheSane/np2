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

	g_opn.addr1l = dat;
	g_opn.addr1h = 0;
	g_opn.data1 = dat;
	(void)port;
}

static void IOOUTCALL ymf_o18a(UINT port, REG8 dat) {

	UINT	addr;

	g_opn.data1 = dat;
	if (g_opn.addr1h != 0) {
		return;
	}

	addr = g_opn.addr1l;
	S98_put(NORMAL2608, addr, dat);
	if (addr < 0x10) {
		if (addr != 0x0e) {
			psggen_setreg(&g_psg1, addr, dat);
		}
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
					opngen_keyon((dat & 0x07) - 1, dat);
				}
			}
			else {
				fmtimer_setreg(addr, dat);
			}
		}
		else if (addr < 0xc0) {
			opngen_setreg(0, addr, dat);
		}
		g_opn.reg[addr] = dat;
	}
	(void)port;
}

static void IOOUTCALL ymf_o18c(UINT port, REG8 dat) {

	if (g_opn.extend) {
		g_opn.addr1l = dat;
		g_opn.addr1h = 1;
		g_opn.data1 = dat;
	}
	(void)port;
}

static void IOOUTCALL ymf_o18e(UINT port, REG8 dat) {

	UINT	addr;

	if (!g_opn.extend) {
		return;
	}
	g_opn.data1 = dat;

	if (g_opn.addr1h != 1) {
		return;
	}
	addr = g_opn.addr1l;
	S98_put(EXTEND2608, addr, dat);
	g_opn.reg[addr + 0x100] = dat;
	if (addr >= 0x30) {
		opngen_setreg(3, addr, dat);
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

static REG8 IOINPCALL ymf_i188(UINT port) {

	(void)port;
	return(g_fmtimer.status);
}

static REG8 IOINPCALL ymf_i18a(UINT port) {

	UINT	addr;

	if (g_opn.addr1h == 0) {
		addr = g_opn.addr1l;
		if (addr == 0x0e) {
			return(fmboard_getjoy(&g_psg1));
		}
		else if (addr < 0x10) {
			return(psggen_getreg(&g_psg1, addr));
		}
		else if (addr == 0xff) {
			return(1);
		}
	}
	(void)port;
	return(g_opn.data1);
}

static REG8 IOINPCALL ymf_i18c(UINT port) {

	if (g_opn.extend) {
		return(g_fmtimer.status & 3);
	}
	(void)port;
	return(0xff);
}

static void extendchannel(REG8 enable) {

	g_opn.extend = enable;
	if (enable) {
		g_opn.channels = 6;
		opngen_setcfg(6, OPN_STEREO | 0x007);
	}
	else {
		g_opn.channels = 3;
		opngen_setcfg(3, OPN_MONORAL | 0x007);
		rhythm_setreg(&g_rhythm, 0x10, 0xff);
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
			ymf_i188,	ymf_i18a,	ymf_i18c,	NULL};


void board118_reset(const NP2CFG *pConfig) {

	fmtimer_reset(0xc0);
	opngen_setcfg(3, OPN_STEREO | 0x038);
	cs4231io_reset();
	soundrom_load(0xcc000, OEMTEXT("118"));
	fmboard_extreg(extendchannel);

	(void)pConfig;
}

void board118_bind(void) {

	fmboard_fmrestore(0, 0);
	fmboard_fmrestore(3, 1);
	psggen_restore(&g_psg1);
	fmboard_rhyrestore(&g_rhythm, 0);
	sound_streamregist(&opngen, (SOUNDCB)opngen_getpcm);
	sound_streamregist(&g_psg1, (SOUNDCB)psggen_getpcm);
	rhythm_bind(&g_rhythm);
	cs4231io_bind();
	cbuscore_attachsndex(0x188, ymf_o, ymf_i);
	iocore_attachout(0xa460, ymf_oa460);
	iocore_attachinp(0xa460, ymf_ia460);
}


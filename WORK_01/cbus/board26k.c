#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"board26k.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"s98.h"


static void IOOUTCALL opn_o188(UINT port, REG8 dat) {

	g_opn.addr1l = dat;
	g_opn.data1 = dat;
	(void)port;
}

static void IOOUTCALL opn_o18a(UINT port, REG8 dat) {

	UINT	addr;

	g_opn.data1 = dat;
	addr = g_opn.addr1l;
	S98_put(NORMAL2608, addr, dat);
	g_opn.reg[addr] = dat;
	if (addr < 0x10) {
		if (addr != 0x0e) {
			psggen_setreg(&g_psg1, addr, dat);
		}
	}
	else if (addr < 0x100) {
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

static REG8 IOINPCALL opn_i188(UINT port) {

	(void)port;
	return(g_fmtimer.status);
}

static REG8 IOINPCALL opn_i18a(UINT port) {

	UINT	addr;

	addr = g_opn.addr1l;
	if (addr == 0x0e) {
		return(fmboard_getjoy(&g_psg1));
	}
	else if (addr < 0x10) {
		return g_opn.reg[addr];
	}
	(void)port;
	return(g_opn.data1);
}


// ----

static const IOOUT opn_o[4] = {
			opn_o188,	opn_o18a,	NULL,		NULL};

static const IOINP opn_i[4] = {
			opn_i188,	opn_i18a,	NULL,		NULL};


void board26k_reset(const NP2CFG *pConfig) {

	opngen_setcfg(&g_opngen, 3, 0);
	fmtimer_reset(pConfig->snd26opt & 0xc0);
	soundrom_loadex(pConfig->snd26opt & 7, OEMTEXT("26"));
	g_opn.base = (pConfig->snd26opt & 0x10)?0x000:0x100;
}

void board26k_bind(void) {

	fmboard_fmrestore(&g_opn, 0, 0);
	psggen_restore(&g_psg1);
	sound_streamregist(&g_opngen, (SOUNDCB)opngen_getpcm);
	sound_streamregist(&g_psg1, (SOUNDCB)psggen_getpcm);
	cbuscore_attachsndex(0x188 - g_opn.base, opn_o, opn_i);
}


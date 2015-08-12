#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"boardpx.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"s98.h"
#include	"pcm86io.h"
#include "sound/soundrom.h"

#if defined(SUPPORT_PX)

static void IOOUTCALL spb_o188(UINT port, REG8 dat) {

	g_opn.addr1l = dat;
//	g_opn.data1 = dat;
	(void)port;
}

static void IOOUTCALL spb_o18a(UINT port, REG8 dat) {

	UINT	addr;

//	g_opn.data1 = dat;
	addr = g_opn.addr1l;
//	S98_put(NORMAL2608, addr, dat);
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
					opnch[2].extop = dat & 0xc0;
				}
			}
		}
		else if (addr < 0xc0) {
			opngen_setreg(0, addr, dat);
		}
		g_opn.reg[addr] = dat;
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
//	S98_put(EXTEND2608, addr, dat);
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
	else if (addr < 0x10) {
		return(psggen_getreg(&g_psg1, addr));
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


static void IOOUTCALL spb_o088(UINT port, REG8 dat) {

	g_opn2.addr1l = dat;
//	g_opn2.data1 = dat;
	(void)port;
}

static void IOOUTCALL spb_o08a(UINT port, REG8 dat) {

	UINT	addr;

//	g_opn2.data1 = dat;
	addr = g_opn2.addr1l;
//	S98_put(NORMAL2608, addr, dat);
	if (addr < 0x10) {
		psggen_setreg(&g_psg2, addr, dat);
	}
	else {
		if (addr < 0x20) {
			rhythm_setreg(&g_rhythm2, addr, dat);
		}
		else if (addr < 0x30) {
			if (addr == 0x28) {
				if ((dat & 0x0f) < 3) {
					opngen_keyon((dat & 0x0f) + 12, dat);
				}
				else if (((dat & 0x0f) != 3) &&
						((dat & 0x0f) < 7)) {
					opngen_keyon((dat & 0x0f) + 11, dat);
				}
			}
			else {
				fmtimer_setreg(addr, dat);
				if (addr == 0x27) {
					opnch[14].extop = dat & 0xc0;
				}
			}
		}
		else if (addr < 0xc0) {
			opngen_setreg(12, addr, dat);
		}
		g_opn2.reg[addr] = dat;
	}
	(void)port;
}

static void IOOUTCALL spb_o08c(UINT port, REG8 dat) {

	g_opn2.addr1h = dat;
//	g_opn2.data1 = dat;
	(void)port;
}

static void IOOUTCALL spb_o08e(UINT port, REG8 dat) {

	UINT	addr;

//	g_opn2.data1 = dat;
	addr = g_opn2.addr1h;
//	S98_put(EXTEND2608, addr, dat);
	g_opn2.reg[addr + 0x100] = dat;
	if (addr >= 0x30) {
		opngen_setreg(15, addr, dat);
	}
	else if (addr < 0x12) {
		adpcm_setreg(&g_adpcm2, addr, dat);
	}
	(void)port;
}

static REG8 IOINPCALL spb_i088(UINT port) {

	(void)port;
	return((g_fmtimer.status & 3) | adpcm_status(&g_adpcm2));
}

static REG8 IOINPCALL spb_i08a(UINT port) {

	UINT	addr;

	addr = g_opn2.addr1l;
	if (addr == 0x0e) {
		return(fmboard_getjoy(&g_psg2));
	}
	else if (addr < 0x10) {
		return(psggen_getreg(&g_psg2, addr));
	}
	else if (addr == 0xff) {
		return(1);
	}
	else {
		(void)port;
		return(g_opn2.reg[addr]);
	}
}

static REG8 IOINPCALL spb_i08e(UINT port) {

	UINT	addr;

	addr = g_opn2.addr1h;
	if (addr == 0x08) {
		return(adpcm_readsample(&g_adpcm2));
	}
	else if (addr == 0x0f) {
		return(g_opn2.reg[addr + 0x100]);
	}
	else
	{
		(void)port;
		return(g_opn2.reg[g_opn2.addr1l]);
	}
}



static void IOOUTCALL p86_o288(UINT port, REG8 dat) {

	g_opn3.addr1l = dat;
//	g_opn3.data1 = dat;
	(void)port;
}

static void IOOUTCALL p86_o28a(UINT port, REG8 dat) {

	UINT	addr;

//	g_opn3.data1 = dat;
	addr = g_opn.addr1l;
//	S98_put(NORMAL2608, addr, dat);
	if (addr < 0x10) {
		psggen_setreg(&g_psg3, addr, dat);
	}
	else {
		if (addr < 0x20) {
			rhythm_setreg(&g_rhythm3, addr, dat);
		}
		else if (addr < 0x30) {
			if (addr == 0x28) {
				if ((dat & 0x0f) < 3) {
					opngen_keyon((dat & 0x0f) + 24, dat);
				}
				else if (((dat & 0x0f) != 3) &&
						((dat & 0x0f) < 7)) {
					opngen_keyon((dat & 0x0f) + 23, dat);
				}
			}
			else {
				fmtimer_setreg(addr, dat);
				if (addr == 0x27) {
					opnch[26].extop = dat & 0xc0;
				}
			}
		}
		else if (addr < 0xc0) {
			opngen_setreg(24, addr, dat);
		}
		g_opn3.reg[addr] = dat;
	}
	(void)port;
}

static void IOOUTCALL p86_o28c(UINT port, REG8 dat) {

	g_opn3.addr1h = dat;
//	g_opn3.data1 = dat;
	(void)port;
}

static void IOOUTCALL p86_o28e(UINT port, REG8 dat) {

	UINT	addr;

//	g_opn3.data1 = dat;
	addr = g_opn3.addr1h;
	if (addr >= 0x100) {
		return;
	}
//	S98_put(EXTEND2608, addr, dat);
	g_opn3.reg[addr + 0x100] = dat;
	if (addr >= 0x30) {
		opngen_setreg(27, addr, dat);
	}
	else if (addr < 0x12) {
		adpcm_setreg(&g_adpcm3, addr, dat);
	}
	(void)port;
}

static REG8 IOINPCALL p86_i288(UINT port) {

	(void)port;
	return((g_fmtimer.status & 3) | adpcm_status(&g_adpcm3));
}

static REG8 IOINPCALL p86_i28a(UINT port) {

	UINT	addr;

	addr = g_opn3.addr1l;
	if (addr == 0x0e) {
		return(fmboard_getjoy(&g_psg3));
	}
	else if (addr < 0x10) {
		return(psggen_getreg(&g_psg3, addr));
	}
	else if (addr == 0xff) {
		return(1);
	}
	else {
		(void)port;
		return(g_opn3.reg[addr]);
	}
}

static REG8 IOINPCALL p86_i28e(UINT port) {

	UINT	addr;

	addr = g_opn3.addr1h;
	if (addr == 0x08) {
		return(adpcm_readsample(&g_adpcm3));
	}
	else if (addr == 0x0f) {
		return(g_opn3.reg[addr + 0x100]);
	}
	else {
		(void)port;
		return(g_opn3.reg[g_opn3.addr1l]);
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

//	g_opn.data2 = dat;
	addr = g_opn.addr2l;
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
				opnch[8].extop = dat & 0xc0;
			}
		}
	}
	else if (addr < 0xc0) {
		opngen_setreg(6, addr, dat);
	}
	g_opn.reg[addr + 0x200] = dat;
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


static void IOOUTCALL spr_o488(UINT port, REG8 dat) {

	g_opn2.addr2l = dat;
//	g_opn2.data2 = dat;
	(void)port;
}

static void IOOUTCALL spr_o48a(UINT port, REG8 dat) {

	UINT	addr;

//	g_opn2.data2 = dat;
	addr = g_opn2.addr2l;
	if (addr >= 0x100) {
		return;
	}
	if (addr < 0x30) {
		if (addr == 0x28) {
			if ((dat & 0x0f) < 3) {
				opngen_keyon((dat & 0x0f) + 18, dat);
			}
			else if (((dat & 0x0f) != 3) &&
					((dat & 0x0f) < 7)) {
				opngen_keyon((dat & 0x0f) + 17, dat);
			}
		}
		else {
			if (addr == 0x27) {
				opnch[20].extop = dat & 0xc0;
			}
		}
	}
	else if (addr < 0xc0) {
		opngen_setreg(18, addr, dat);
	}
	g_opn2.reg[addr + 0x200] = dat;
	(void)port;
}

static void IOOUTCALL spr_o48c(UINT port, REG8 dat) {

	g_opn2.addr2h = dat;
//	g_opn2.data2 = dat;
	(void)port;
}

static void IOOUTCALL spr_o48e(UINT port, REG8 dat) {

	UINT	addr;

//	g_opn.data2 = dat;
	addr = g_opn2.addr2h;
	g_opn2.reg[addr + 0x300] = dat;
	if (addr >= 0x30) {
		opngen_setreg(21, addr, dat);
	}
	(void)port;
}

static REG8 IOINPCALL spr_i488(UINT port) {

	(void)port;
	return(g_fmtimer.status);
}

static REG8 IOINPCALL spr_i48a(UINT port) {

	UINT	addr;

	addr = g_opn2.addr2l;
	if ((addr >= 0x20) && (addr < 0xff)) {
		return(g_opn2.reg[addr + 0x200]);
	}
	else if (addr == 0xff) {
		return(0);
	}
	else {
		(void)port;
//		return(g_opn2.data2);
		return(0xff);
	}
}

static REG8 IOINPCALL spr_i48c(UINT port) {

	(void)port;
	return(g_fmtimer.status & 3);
}

static REG8 IOINPCALL spr_i48e(UINT port) {

	(void)port;
	return(g_opn2.reg[g_opn2.addr2l + 0x200]);
}


// ----

static const IOOUT spb_o[4] = {
			spb_o188,	spb_o18a,	spb_o18c,	spb_o18e};

static const IOINP spb_i[4] = {
			spb_i188,	spb_i18a,	spb_i188,	spb_i18e};

static const IOOUT spb_o2[4] = {
			spb_o088,	spb_o08a,	spb_o08c,	spb_o08e};

static const IOINP spb_i2[4] = {
			spb_i088,	spb_i08a,	spb_i088,	spb_i08e};

static const IOOUT p86_o3[4] = {
			p86_o288,	p86_o28a,	p86_o28c,	p86_o28e};

static const IOINP p86_i3[4] = {
			p86_i288,	p86_i28a,	p86_i288,	p86_i28e};

// ----

static const IOOUT spr_o[4] = {
			spr_o588,	spr_o58a,	spr_o58c,	spr_o58e};

static const IOINP spr_i[4] = {
			spr_i588,	spr_i58a,	spr_i58c,	spr_i58e};

static const IOOUT spr_o2[4] = {
			spr_o488,	spr_o48a,	spr_o48c,	spr_o48e};

static const IOINP spr_i2[4] = {
			spr_i488,	spr_i48a,	spr_i48c,	spr_i48e};

void boardpx1_reset(const NP2CFG *pConfig) {

	fmtimer_reset(pConfig->spbopt & 0xc0);
	g_opn.reg[0x2ff] = 0;
	g_opn.channels = 12;
	g_opn2.reg[0x2ff] = 0;
	g_opn2.channels = 12;
	opngen_setcfg(24, OPN_STEREO | 0x00ffffff);
	soundrom_loadex(pConfig->spbopt & 7, OEMTEXT("SPB"));
	g_opn.base = (pConfig->spbopt & 0x10)?0x000:0x100;
}

void boardpx1_bind(void) {

	fmboard_fmrestore(&g_opn, 0, 0);
	fmboard_fmrestore(&g_opn, 3, 1);
	fmboard_fmrestore(&g_opn, 6, 2);
	fmboard_fmrestore(&g_opn, 9, 3);
	fmboard_fmrestore(&g_opn2, 12, 0);
	fmboard_fmrestore(&g_opn2, 15, 1);
	fmboard_fmrestore(&g_opn2, 18, 2);
	fmboard_fmrestore(&g_opn2, 21, 3);
	psggen_restore(&g_psg1);
	psggen_restore(&g_psg2);
	fmboard_rhyrestore(&g_opn, &g_rhythm, 0);
	fmboard_rhyrestore(&g_opn2, &g_rhythm2, 0);
	sound_streamregist(&opngen, (SOUNDCB)opngen_getpcmvr);
	sound_streamregist(&g_psg1, (SOUNDCB)psggen_getpcm);
	sound_streamregist(&g_psg2, (SOUNDCB)psggen_getpcm);
	rhythm_bind(&g_rhythm);
	rhythm_bind(&g_rhythm2);
	sound_streamregist(&g_adpcm, (SOUNDCB)adpcm_getpcm);
	sound_streamregist(&g_adpcm2, (SOUNDCB)adpcm_getpcm);

	cbuscore_attachsndex(0x188, spb_o, spb_i);
	cbuscore_attachsndex(0x588, spr_o, spr_i);
	cbuscore_attachsndex(0x088, spb_o2, spb_i2);
	cbuscore_attachsndex(0x488, spr_o2, spr_i2);
}


static void extendchannelx2(REG8 enable) {

	g_opn3.extend = enable;
	if (enable) {
		g_opn3.channels = 6;
		opngen_setcfg(30, OPN_STEREO | 0x07000000);
	}
	else {
		g_opn3.channels = 3;
		opngen_setcfg(27, OPN_MONORAL | 0x07000000);
		rhythm_setreg(&g_rhythm2, 0x10, 0xff);
	}
}

void boardpx2_reset(const NP2CFG *pConfig) {

	fmtimer_reset(pConfig->spbopt & 0xc0);
	g_opn.reg[0x2ff] = 0;
	g_opn.channels = 12;
	g_opn2.reg[0x2ff] = 0;
	g_opn2.channels = 12;
	g_opn3.channels = 3;
	opngen_setcfg(27, OPN_STEREO | 0x38ffffff);
	soundrom_loadex(pConfig->spbopt & 7, OEMTEXT("SPB"));
	g_opn.base = (pConfig->spbopt & 0x10)?0x000:0x100;
	fmboard_extreg(extendchannelx2);
}

void boardpx2_bind(void) {

	fmboard_fmrestore(&g_opn, 0, 0);
	fmboard_fmrestore(&g_opn, 3, 1);
	fmboard_fmrestore(&g_opn, 6, 2);
	fmboard_fmrestore(&g_opn, 9, 3);
	fmboard_fmrestore(&g_opn2, 12, 0);
	fmboard_fmrestore(&g_opn2, 15, 1);
	fmboard_fmrestore(&g_opn2, 18, 2);
	fmboard_fmrestore(&g_opn2, 21, 3);
	fmboard_fmrestore(&g_opn3, 24, 0);
	fmboard_fmrestore(&g_opn3, 27, 1);
	psggen_restore(&g_psg1);
	psggen_restore(&g_psg2);
	psggen_restore(&g_psg3);
	fmboard_rhyrestore(&g_opn, &g_rhythm, 0);
	fmboard_rhyrestore(&g_opn2, &g_rhythm2, 0);
	fmboard_rhyrestore(&g_opn3, &g_rhythm3, 0);
	sound_streamregist(&opngen, (SOUNDCB)opngen_getpcmvr);
	sound_streamregist(&g_psg1, (SOUNDCB)psggen_getpcm);
	sound_streamregist(&g_psg2, (SOUNDCB)psggen_getpcm);
	sound_streamregist(&g_psg3, (SOUNDCB)psggen_getpcm);
	rhythm_bind(&g_rhythm);
	rhythm_bind(&g_rhythm2);
	rhythm_bind(&g_rhythm3);
	sound_streamregist(&g_adpcm, (SOUNDCB)adpcm_getpcm);
	sound_streamregist(&g_adpcm2, (SOUNDCB)adpcm_getpcm);
	sound_streamregist(&g_adpcm3, (SOUNDCB)adpcm_getpcm);

	pcm86io_bind();

	cbuscore_attachsndex(0x188, spb_o, spb_i);
	cbuscore_attachsndex(0x588, spr_o, spr_i);
	cbuscore_attachsndex(0x088, spb_o2, spb_i2);
	cbuscore_attachsndex(0x488, spr_o2, spr_i2);
	cbuscore_attachsndex(0x288, p86_o3, p86_i3);
}

#endif	// defined(SUPPORT_PX)


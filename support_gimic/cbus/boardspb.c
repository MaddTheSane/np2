/**
 * @file	boardspb.c
 * @brief	Implementation of Speak board
 */

#include "compiler.h"
#include "boardspb.h"
#include "iocore.h"
#include "cbuscore.h"
#include "sound/fmboard.h"
#include "sound/sound.h"
#include "sound/soundrom.h"

static void IOOUTCALL spb_o188(UINT port, REG8 dat)
{
	g_opn.s.addr1l = dat;
//	g_opn.s.data1 = dat;

	(void)port;
}

static void IOOUTCALL spb_o18a(UINT port, REG8 dat)
{
//	g_opn.s.data1 = dat;
	opna_writeRegister(&g_opn, g_opn.s.addr1l, dat);

	(void)port;
}

static void IOOUTCALL spb_o18c(UINT port, REG8 dat)
{
	g_opn.s.addr1h = dat;
//	g_opn.s.data1 = dat;
	(void)port;
}

static void IOOUTCALL spb_o18e(UINT port, REG8 dat)
{
//	g_opn.s.data1 = dat;
	opna_writeExtendedRegister(&g_opn, g_opn.s.addr1h, dat);

	(void)port;
}

static REG8 IOINPCALL spb_i188(UINT port)
{
	(void)port;

	return opna_readExtendedStatus(&g_opn);
}

static REG8 IOINPCALL spb_i18a(UINT port)
{
	UINT nAddress;

	nAddress = g_opn.s.addr1l;
	if (nAddress == 0x0e)
	{
		return fmboard_getjoy(&g_opn.psg);
	}

	(void)port;
	return opna_readRegister(&g_opn, nAddress);
}

static REG8 IOINPCALL spb_i18e(UINT port)
{
	UINT nAddress;

	nAddress = g_opn.s.addr1h;
	if ((nAddress == 0x08) || (nAddress == 0x0f))
	{
		return opna_readExtendedRegister(&g_opn, nAddress);
	}

	(void)port;
	return g_opn.s.reg[g_opn.s.addr1l];
}


// ---- spark board

static void IOOUTCALL spr_o588(UINT port, REG8 dat)
{
	g_opn.s.addr2l = dat;
//	g_opn.s.data2 = dat;
	(void)port;
}

static void IOOUTCALL spr_o58a(UINT port, REG8 dat)
{
//	g_opn.s.data2 = dat;
	opna_write3438Register(&g_opn, g_opn.s.addr2l, dat);

	(void)port;
}

static void IOOUTCALL spr_o58c(UINT port, REG8 dat)
{
	g_opn.s.addr2h = dat;
//	g_opn.s.data2 = dat;
	(void)port;
}

static void IOOUTCALL spr_o58e(UINT port, REG8 dat)
{
//	g_opn.s.data2 = dat;
	opna_write3438ExtRegister(&g_opn, g_opn.s.addr2h, dat);

	(void)port;
}

static REG8 IOINPCALL spr_i588(UINT port)
{
	(void)port;
	return g_fmtimer.status;
}

static REG8 IOINPCALL spr_i58a(UINT port)
{
	(void)port;
	return opna_read3438Register(&g_opn, g_opn.s.addr2l);
}

static REG8 IOINPCALL spr_i58c(UINT port)
{
	(void)port;
	return (g_fmtimer.status & 3);
}

static REG8 IOINPCALL spr_i58e(UINT port)
{
	(void)port;
	return opna_read3438ExtRegister(&g_opn, g_opn.s.addr2l);
}


// ----

static const IOOUT spb_o[4] =
{
	spb_o188,	spb_o18a,	spb_o18c,	spb_o18e
};

static const IOINP spb_i[4] =
{
	spb_i188,	spb_i18a,	spb_i188,	spb_i18e
};

/**
 * Reset
 * @param[in] pConfig A pointer to a configure structure
 */
void boardspb_reset(const NP2CFG *pConfig)
{
	opna_reset(&g_opn, OPNA_HAS_TIMER | OPNA_HAS_EXTENDEDFM | OPNA_HAS_ADPCM | OPNA_HAS_VR | OPNA_S98);

	fmtimer_reset(pConfig->spbopt & 0xc0);
	g_opn.s.channels = 6;
	opngen_setcfg(&g_opn.opngen, 6, OPN_STEREO | 0x03f);
	soundrom_loadex(pConfig->spbopt & 7, OEMTEXT("SPB"));
	g_opn.s.base = ((pConfig->spbopt & 0x10) ? 0x000 : 0x100);
}

/**
 * Bind
 */
void boardspb_bind(void)
{
	opna_bind(&g_opn);
	cbuscore_attachsndex(0x188 - g_opn.s.base, spb_o, spb_i);
}


// ----

static const IOOUT spr_o[4] = {
			spr_o588,	spr_o58a,	spr_o58c,	spr_o58e};

static const IOINP spr_i[4] = {
			spr_i588,	spr_i58a,	spr_i58c,	spr_i58e};

/**
 * Reset
 * @param[in] pConfig A pointer to a configure structure
 */
void boardspr_reset(const NP2CFG *pConfig)
{
	opna_reset(&g_opn, OPNA_HAS_TIMER | OPNA_HAS_EXTENDEDFM | OPNA_HAS_ADPCM | OPNA_HAS_YM3438 | OPNA_HAS_VR | OPNA_S98);

	fmtimer_reset(pConfig->spbopt & 0xc0);
	g_opn.s.reg[0x2ff] = 0;
	g_opn.s.channels = 12;
	opngen_setcfg(&g_opn.opngen, 12, OPN_STEREO | 0x03f);
	soundrom_loadex(pConfig->spbopt & 7, OEMTEXT("SPB"));
	g_opn.s.base = (pConfig->spbopt & 0x10) ? 0x000 : 0x100;
}

/**
 * Bind
 */
void boardspr_bind(void)
{
	opna_bind(&g_opn);
	cbuscore_attachsndex(0x188 - g_opn.s.base, spb_o, spb_i);
	cbuscore_attachsndex(0x588 - g_opn.s.base, spr_o, spr_i);
}

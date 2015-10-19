/**
 * @file	board118.c
 * @brief	Implementation of PC-9801-118
 */

#include "compiler.h"
#include "board118.h"
#include "pccore.h"
#include "iocore.h"
#include "cbuscore.h"
#include "cs4231io.h"
#include "sound/fmboard.h"
#include "sound/sound.h"
#include "sound/soundrom.h"

static void IOOUTCALL ymf_o188(UINT port, REG8 dat)
{
	g_opn.s.addr1l = dat;
	g_opn.s.addr1h = 0;
	g_opn.s.data1 = dat;
	(void)port;
}

static void IOOUTCALL ymf_o18a(UINT port, REG8 dat)
{
	g_opn.s.data1 = dat;
	if (g_opn.s.addr1h != 0) {
		return;
	}

	opna_writeRegister(&g_opn, g_opn.s.addr1l, dat);

	(void)port;
}

static void IOOUTCALL ymf_o18c(UINT port, REG8 dat)
{
	if (g_opn.s.extend)
	{
		g_opn.s.addr1l = dat;
		g_opn.s.addr1h = 1;
		g_opn.s.data1 = dat;
	}
	(void)port;
}

static void IOOUTCALL ymf_o18e(UINT port, REG8 dat)
{
	if (!g_opn.s.extend)
	{
		return;
	}
	g_opn.s.data1 = dat;

	if (g_opn.s.addr1h != 1)
	{
		return;
	}

	opna_writeExtendedRegister(&g_opn, g_opn.s.addr1h, dat);

	(void)port;
}

static REG8 IOINPCALL ymf_i188(UINT port)
{
	(void)port;
	return g_fmtimer.status;
}

static REG8 IOINPCALL ymf_i18a(UINT port)
{
	UINT nAddress;

	if (g_opn.s.addr1h == 0)
	{
		nAddress = g_opn.s.addr1l;
		if (nAddress == 0x0e)
		{
			return fmboard_getjoy(&g_opn.psg);
		}
		else if (nAddress < 0x10)
		{
			return opna_readRegister(&g_opn, nAddress);
		}
		else if (nAddress == 0xff)
		{
			return 1;
		}
	}

	(void)port;
	return g_opn.s.data1;
}

static REG8 IOINPCALL ymf_i18c(UINT port)
{
	if (g_opn.s.extend)
	{
		return (g_fmtimer.status & 3);
	}

	(void)port;
	return 0xff;
}

static void extendchannel(REG8 enable)
{
	g_opn.s.extend = enable;
	if (enable)
	{
		g_opn.s.channels = 6;
		opngen_setcfg(&g_opn.opngen, 6, OPN_STEREO | 0x007);
	}
	else
	{
		g_opn.s.channels = 3;
		opngen_setcfg(&g_opn.opngen, 3, OPN_MONORAL | 0x007);
		rhythm_setreg(&g_opn.rhythm, 0x10, 0xff);
	}
}

static void IOOUTCALL ymf_oa460(UINT port, REG8 dat)
{
	cs4231.extfunc = dat;
	extendchannel((REG8)(dat & 1));
	(void)port;
}

static REG8 IOINPCALL ymf_ia460(UINT port)
{
	(void)port;
	return (0x80 | (cs4231.extfunc & 1));
}


// ----

static const IOOUT ymf_o[4] = {
			ymf_o188,	ymf_o18a,	ymf_o18c,	ymf_o18e};

static const IOINP ymf_i[4] = {
			ymf_i188,	ymf_i18a,	ymf_i18c,	NULL};

/**
 * Reset
 * @param[in] pConfig A pointer to a configure structure
 */
void board118_reset(const NP2CFG *pConfig)
{
	opna_reset(&g_opn, OPNA_HAS_TIMER | OPNA_HAS_EXTENDEDFM | OPNA_S98);

	fmtimer_reset(0xc0);
	opngen_setcfg(&g_opn.opngen, 3, OPN_STEREO | 0x038);
	cs4231io_reset();
	soundrom_load(0xcc000, OEMTEXT("118"));
	fmboard_extreg(extendchannel);

	(void)pConfig;
}

/**
 * Bind
 */
void board118_bind(void)
{
	opna_bind(&g_opn);
	cs4231io_bind();
	cbuscore_attachsndex(0x188, ymf_o, ymf_i);
	iocore_attachout(0xa460, ymf_oa460);
	iocore_attachinp(0xa460, ymf_ia460);
}

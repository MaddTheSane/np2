/**
 * @file	boardx2.c
 * @brief	Implementation of PC-9801-86 + 26K
 */

#include "compiler.h"
#include "boardx2.h"
#include "iocore.h"
#include "cbuscore.h"
#include "pcm86io.h"
#include "sound/fmboard.h"
#include "sound/sound.h"
#include "sound/soundrom.h"

static void IOOUTCALL opn_o088(UINT port, REG8 dat)
{
	g_opn2.s.addr1l = dat;
	g_opn2.s.data1 = dat;
	(void)port;
}

static void IOOUTCALL opn_o08a(UINT port, REG8 dat)
{
	g_opn2.s.data1 = dat;
	opna_writeRegister(&g_opn2, g_opn2.s.addr1l, dat);

	(void)port;
}

static REG8 IOINPCALL opn_i088(UINT port)
{
	(void)port;
	return g_fmtimer.status;
}

static REG8 IOINPCALL opn_i08a(UINT port)
{
	UINT nAddress;

	nAddress = g_opn2.s.addr1l;
	if (nAddress == 0x0e)
	{
		return fmboard_getjoy(&g_opn2.psg);
	}
	else if (nAddress < 0x10)
	{
		return opna_readRegister(&g_opn2, nAddress);
	}

	(void)port;
	return g_opn2.s.data1;
}


// ----

static void IOOUTCALL opna_o188(UINT port, REG8 dat)
{
	g_opn.s.addr1l = dat;
	g_opn.s.data1 = dat;
	(void)port;
}

static void IOOUTCALL opna_o18a(UINT port, REG8 dat)
{
	g_opn.s.data1 = dat;
	opna_writeRegister(&g_opn, g_opn.s.addr1l, dat);

	(void)port;
}

static void IOOUTCALL opna_o18c(UINT port, REG8 dat)
{
	if (g_opn.s.extend)
	{
		g_opn.s.addr1h = dat;
		g_opn.s.data1 = dat;
	}

	(void)port;
}

static void IOOUTCALL opna_o18e(UINT port, REG8 dat)
{
	if (g_opn.s.extend)
	{
		g_opn.s.data1 = dat;
		opna_writeExtendedRegister(&g_opn, g_opn.s.addr1h, dat);
	}

	(void)port;
}

static REG8 IOINPCALL opna_i188(UINT port)
{
	(void)port;
	return g_fmtimer.status;
}

static REG8 IOINPCALL opna_i18a(UINT port)
{
	UINT nAddress;

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

	(void)port;
	return g_opn.s.data1;
}

static REG8 IOINPCALL opna_i18c(UINT port)
{
	if (g_opn.s.extend)
	{
		return opna_readExtendedStatus(&g_opn);
	}

	(void)port;
	return 0xff;
}

static REG8 IOINPCALL opna_i18e(UINT port)
{
	UINT nAddress;

	if (g_opn.s.extend)
	{
		nAddress = g_opn.s.addr1h;
		if ((nAddress == 0x08) || (nAddress == 0x0f))
		{
			return opna_readExtendedRegister(&g_opn, nAddress);
		}
		return g_opn.s.data1;
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


// ----

static const IOOUT opn_o[4] = {
			opn_o088,	opn_o08a,	NULL,		NULL};

static const IOINP opn_i[4] = {
			opn_i088,	opn_i08a,	NULL,		NULL};

static const IOOUT opna_o[4] = {
			opna_o188,	opna_o18a,	opna_o18c,	opna_o18e};

static const IOINP opna_i[4] = {
			opna_i188,	opna_i18a,	opna_i18c,	opna_i18e};


/**
 * Reset
 * @param[in] pConfig A pointer to a configure structure
 * @param[in] adpcm Enable ADPCM
 */
void boardx2_reset(const NP2CFG *pConfig)
{
	opna_reset(&g_opn, OPNA_HAS_TIMER | OPNA_HAS_EXTENDEDFM | OPNA_S98);
	opna_reset(&g_opn2, 0);

	fmtimer_reset(0xc0);
	g_opn.s.channels = 3;
	opngen_setcfg(&g_opn.opngen, 3, OPN_STEREO | 0x038);
	g_opn2.s.channels = 3;
	opngen_setcfg(&g_opn2.opngen, 3, 0);
	if (pConfig->snd86opt & 2)
	{
		soundrom_load(0xcc000, OEMTEXT("86"));
	}
	fmboard_extreg(extendchannel);
}

void boardx2_bind(void)
{
	opna_bind(&g_opn);
	opna_bind(&g_opn2);
	pcm86io_bind();
	cbuscore_attachsndex(0x088, opn_o, opn_i);
	cbuscore_attachsndex(0x188, opna_o, opna_i);
}


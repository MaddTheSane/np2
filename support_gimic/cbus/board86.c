/**
 * @file	board86.c
 * @brief	Implementation of PC-9801-86
 */

#include "compiler.h"
#include "board86.h"
#include "iocore.h"
#include "cbuscore.h"
#include "pcm86io.h"
#include "sound/fmboard.h"
#include "sound/sound.h"
#include "sound/soundrom.h"

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

static const IOOUT opna_o[4] = {
			opna_o188,	opna_o18a,	opna_o18c,	opna_o18e};

static const IOINP opna_i[4] = {
			opna_i188,	opna_i18a,	opna_i18c,	opna_i18e};


/**
 * Reset
 * @param[in] pConfig A pointer to a configure structure
 * @param[in] adpcm Enable ADPCM
 */
void board86_reset(const NP2CFG *pConfig, BOOL adpcm)
{
	REG8 cCaps;

	cCaps = OPNA_HAS_TIMER | OPNA_HAS_EXTENDEDFM | OPNA_S98;
	if (adpcm)
	{
		cCaps |= OPNA_HAS_ADPCM;
	}
	opna_reset(&g_opn, cCaps);

	fmtimer_reset((pConfig->snd86opt & 0x10) |
					((pConfig->snd86opt & 0x4) << 5) |
					((pConfig->snd86opt & 0x8) << 3));
	opngen_setcfg(&g_opn.opngen, 3, OPN_STEREO | 0x038);
	if (pConfig->snd86opt & 2)
	{
		soundrom_load(0xcc000, OEMTEXT("86"));
	}
	g_opn.s.base = (pConfig->snd86opt & 0x01) ? 0x000 : 0x100;
	fmboard_extreg(extendchannel);
}

/**
 * Bind
 */
void board86_bind(void)
{
	opna_bind(&g_opn);
	pcm86io_bind();
	cbuscore_attachsndex(0x188 + g_opn.s.base, opna_o, opna_i);
}

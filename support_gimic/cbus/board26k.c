/**
 * @file	board26k.c
 * @brief	Implementation of PC-9801-26K
 */

#include "compiler.h"
#include "pccore.h"
#include "iocore.h"
#include "cbuscore.h"
#include "board26k.h"
#include "sound.h"
#include "fmboard.h"
#include "opna.h"

static void IOOUTCALL opn_o188(UINT port, REG8 dat)
{
	g_opn.addr1l = dat;
	g_opn.data1 = dat;
	(void)port;
}

static void IOOUTCALL opn_o18a(UINT port, REG8 dat)
{
	g_opn.data1 = dat;
	opna_writeRegister(&g_opn, g_opn.addr1l, dat);

	(void)port;
}

static REG8 IOINPCALL opn_i188(UINT port)
{
	(void)port;
	return g_fmtimer.status;
}

static REG8 IOINPCALL opn_i18a(UINT port)
{
	UINT nAddress;

	nAddress = g_opn.addr1l;
	if (nAddress == 0x0e)
	{
		return fmboard_getjoy(&g_psg1);
	}
	else if (nAddress < 0x10)
	{
		return opna_readRegister(&g_opn, nAddress);
	}

	(void)port;
	return g_opn.data1;
}


// ----

static const IOOUT opn_o[4] = {
			opn_o188,	opn_o18a,	NULL,		NULL};

static const IOINP opn_i[4] = {
			opn_i188,	opn_i18a,	NULL,		NULL};

/**
 * Reset
 * @param[in] pConfig A pointer to a configure structure
 */
void board26k_reset(const NP2CFG *pConfig)
{
	opna_reset(&g_opn, OPNA_HAS_TIMER | OPNA_S98);

	opngen_setcfg(3, 0);
	fmtimer_reset(pConfig->snd26opt & 0xc0);
	soundrom_loadex(pConfig->snd26opt & 7, OEMTEXT("26"));
	g_opn.base = (pConfig->snd26opt & 0x10)?0x000:0x100;
}

/**
 * Bind
 */
void board26k_bind(void)
{
	opna_bind(&g_opn);
	cbuscore_attachsndex(0x188 - g_opn.base, opn_o, opn_i);
}

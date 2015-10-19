/**
 * @file	boardpx.c
 * @brief	Implementation of PX
 */

#include "compiler.h"

#if defined(SUPPORT_PX)

#include "boardpx.h"
#include "iocore.h"
#include "cbuscore.h"
#include "pcm86io.h"
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



static void IOOUTCALL spb_o088(UINT port, REG8 dat)
{
	g_opn2.s.addr1l = dat;
//	g_opn2.s.data1 = dat;
	(void)port;
}

static void IOOUTCALL spb_o08a(UINT port, REG8 dat)
{
//	g_opn2.s.data1 = dat;
	opna_writeRegister(&g_opn2, g_opn2.s.addr1l, dat);

	(void)port;
}

static void IOOUTCALL spb_o08c(UINT port, REG8 dat)
{
	g_opn2.s.addr1h = dat;
//	g_opn2.s.data1 = dat;
	(void)port;
}

static void IOOUTCALL spb_o08e(UINT port, REG8 dat)
{
//	g_opn2.2.data1 = dat;
	opna_writeExtendedRegister(&g_opn2, g_opn2.s.addr1h, dat);

	(void)port;
}

static REG8 IOINPCALL spb_i088(UINT port)
{
	(void)port;

	return opna_readExtendedStatus(&g_opn2);
}

static REG8 IOINPCALL spb_i08a(UINT port)
{
	UINT nAddress;

	nAddress = g_opn2.s.addr1l;
	if (nAddress == 0x0e)
	{
		return fmboard_getjoy(&g_opn2.psg);
	}

	(void)port;
	return opna_readRegister(&g_opn2, nAddress);
}

static REG8 IOINPCALL spb_i08e(UINT port)
{
	UINT nAddress;

	nAddress = g_opn2.s.addr1h;
	if ((nAddress == 0x08) || (nAddress == 0x0f))
	{
		return opna_readExtendedRegister(&g_opn2, nAddress);
	}

	(void)port;
	return g_opn2.s.reg[g_opn2.s.addr1l];
}



static void IOOUTCALL p86_o288(UINT port, REG8 dat)
{
	g_opn3.s.addr1l = dat;
	g_opn3.s.data1 = dat;
	(void)port;
}

static void IOOUTCALL p86_o28a(UINT port, REG8 dat)
{
	g_opn3.s.data1 = dat;
	opna_writeRegister(&g_opn3, g_opn3.s.addr1l, dat);

	(void)port;
}

static void IOOUTCALL p86_o28c(UINT port, REG8 dat)
{
	if (g_opn3.s.extend)
	{
		g_opn3.s.addr1h = dat;
		g_opn3.s.data1 = dat;
	}

	(void)port;
}

static void IOOUTCALL p86_o28e(UINT port, REG8 dat)
{
	if (g_opn3.s.extend)
	{
		g_opn3.s.data1 = dat;
		opna_writeExtendedRegister(&g_opn3, g_opn3.s.addr1h, dat);
	}

	(void)port;
}

static REG8 IOINPCALL p86_i288(UINT port)
{
	(void)port;
	return g_fmtimer.status;
}

static REG8 IOINPCALL p86_i28a(UINT port)
{
	UINT nAddress;

	nAddress = g_opn3.s.addr1l;
	if (nAddress == 0x0e)
	{
		return fmboard_getjoy(&g_opn3.psg);
	}
	else if (nAddress < 0x10)
	{
		return opna_readRegister(&g_opn3, nAddress);
	}
	else if (nAddress == 0xff)
	{
		return 1;
	}

	(void)port;
	return g_opn3.s.data1;
}

static REG8 IOINPCALL p86_i28e(UINT port)
{
	UINT nAddress;

	if (g_opn3.s.extend)
	{
		nAddress = g_opn3.s.addr1h;
		if ((nAddress == 0x08) || (nAddress == 0x0f))
		{
			return opna_readExtendedRegister(&g_opn3, nAddress);
		}
		return g_opn3.s.data1;
	}

	(void)port;
	return 0xff;
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



static void IOOUTCALL spr_o488(UINT port, REG8 dat)
{
	g_opn2.s.addr2l = dat;
//	g_opn2.s.data2 = dat;
	(void)port;
}

static void IOOUTCALL spr_o48a(UINT port, REG8 dat)
{
//	g_opn2.s.data2 = dat;
	opna_write3438Register(&g_opn2, g_opn2.s.addr2l, dat);

	(void)port;
}

static void IOOUTCALL spr_o48c(UINT port, REG8 dat)
{
	g_opn2.s.addr2h = dat;
//	g_opn2.s.data2 = dat;
	(void)port;
}

static void IOOUTCALL spr_o48e(UINT port, REG8 dat)
{
//	g_opn.s.data2 = dat;
	opna_write3438ExtRegister(&g_opn2, g_opn2.s.addr2h, dat);

	(void)port;
}

static REG8 IOINPCALL spr_i488(UINT port)
{
	(void)port;
	return g_fmtimer.status;
}

static REG8 IOINPCALL spr_i48a(UINT port)
{
	(void)port;
	return opna_read3438Register(&g_opn2, g_opn2.s.addr2l);
}

static REG8 IOINPCALL spr_i48c(UINT port)
{
	(void)port;
	return (g_fmtimer.status & 3);
}

static REG8 IOINPCALL spr_i48e(UINT port)
{
	(void)port;
	return opna_read3438ExtRegister(&g_opn2, g_opn2.s.addr2l);
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

/**
 * Reset
 * @param[in] pConfig A pointer to a configure structure
 */
void boardpx1_reset(const NP2CFG *pConfig)
{
	opna_reset(&g_opn, OPNA_HAS_TIMER | OPNA_HAS_EXTENDEDFM | OPNA_HAS_ADPCM | OPNA_HAS_YM3438 | OPNA_HAS_VR | OPNA_S98);
	opna_reset(&g_opn2, OPNA_HAS_TIMER | OPNA_HAS_EXTENDEDFM | OPNA_HAS_ADPCM | OPNA_HAS_YM3438 | OPNA_HAS_VR);

	fmtimer_reset(pConfig->spbopt & 0xc0);
	g_opn.s.reg[0x2ff] = 0;
	g_opn.s.channels = 12;
	g_opn2.s.reg[0x2ff] = 0;
	g_opn2.s.channels = 12;
	opngen_setcfg(&g_opn.opngen, 12, OPN_STEREO | 0xfff);
	opngen_setcfg(&g_opn2.opngen, 12, OPN_STEREO | 0xfff);
	soundrom_loadex(pConfig->spbopt & 7, OEMTEXT("SPB"));
	g_opn.s.base = (pConfig->spbopt & 0x10)?0x000:0x100;
}

/**
 * Bind
 */
void boardpx1_bind(void)
{
	opna_bind(&g_opn);
	opna_bind(&g_opn2);
	cbuscore_attachsndex(0x188, spb_o, spb_i);
	cbuscore_attachsndex(0x588, spr_o, spr_i);
	cbuscore_attachsndex(0x088, spb_o2, spb_i2);
	cbuscore_attachsndex(0x488, spr_o2, spr_i2);
}


static void extendchannelx2(REG8 enable) {

	g_opn3.s.extend = enable;
	if (enable)
	{
		g_opn3.s.channels = 6;
		opngen_setcfg(&g_opn3.opngen, 6, OPN_STEREO | 0x007);
	}
	else
	{
		g_opn3.s.channels = 3;
		opngen_setcfg(&g_opn3.opngen, 3, OPN_MONORAL | 0x007);
		rhythm_setreg(&g_opn3.rhythm, 0x10, 0xff);
	}
}

/**
 * Reset
 * @param[in] pConfig A pointer to a configure structure
 */
void boardpx2_reset(const NP2CFG *pConfig)
{
	opna_reset(&g_opn, OPNA_HAS_TIMER | OPNA_HAS_EXTENDEDFM | OPNA_HAS_ADPCM | OPNA_HAS_YM3438 | OPNA_HAS_VR | OPNA_S98);
	opna_reset(&g_opn2, OPNA_HAS_TIMER | OPNA_HAS_EXTENDEDFM | OPNA_HAS_ADPCM | OPNA_HAS_YM3438 | OPNA_HAS_VR);
	opna_reset(&g_opn3, OPNA_HAS_EXTENDEDFM | OPNA_HAS_ADPCM);

	fmtimer_reset(pConfig->spbopt & 0xc0);
	g_opn.s.reg[0x2ff] = 0;
	g_opn.s.channels = 12;
	g_opn2.s.reg[0x2ff] = 0;
	g_opn2.s.channels = 12;
	g_opn3.s.channels = 3;
	opngen_setcfg(&g_opn.opngen, 12, OPN_STEREO | 0xfff);
	opngen_setcfg(&g_opn2.opngen, 12, OPN_STEREO | 0xfff);
	opngen_setcfg(&g_opn3.opngen,  3, OPN_STEREO | 0x038);
	soundrom_loadex(pConfig->spbopt & 7, OEMTEXT("SPB"));
	g_opn.s.base = (pConfig->spbopt & 0x10)?0x000:0x100;
	fmboard_extreg(extendchannelx2);
}

/**
 * Bind
 */
void boardpx2_bind(void)
{
	opna_bind(&g_opn);
	opna_bind(&g_opn2);
	opna_bind(&g_opn3);
	pcm86io_bind();
	cbuscore_attachsndex(0x188, spb_o, spb_i);
	cbuscore_attachsndex(0x588, spr_o, spr_i);
	cbuscore_attachsndex(0x088, spb_o2, spb_i2);
	cbuscore_attachsndex(0x488, spr_o2, spr_i2);
	cbuscore_attachsndex(0x288, p86_o3, p86_i3);
}

#endif	// defined(SUPPORT_PX)


#include	"compiler.h"
#include	"joymng.h"
#include	"soundmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"board14.h"
#include	"board26k.h"
#include	"board86.h"
#include	"boardx2.h"
#include	"board118.h"
#include	"boardspb.h"
#if defined(SUPPORT_PX)
#include	"boardpx.h"
#endif	// defined(SUPPORT_PX)
#include	"amd98.h"
#include	"pcm86io.h"
#include	"cs4231io.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"beep.h"
#include "soundrom.h"
#include	"keydisp.h"
#include	"keystat.h"


	UINT32		g_usesound;
	OPN_T		g_opn;

	_FMTIMER	g_fmtimer;
	_OPNGEN		g_opngen;
	_PSGGEN		g_psg[3];
	_RHYTHM		g_rhythm;
	_ADPCM		g_adpcm;
	_PCM86		pcm86;
	_CS4231		cs4231;

#if defined(SUPPORT_PX)
	OPN_T		g_opn2;
	OPN_T		g_opn3;
	_RHYTHM		g_rhythm2;
	_RHYTHM		g_rhythm3;
	_ADPCM		g_adpcm2;
	_ADPCM		g_adpcm3;
#endif	// defined(SUPPORT_PX)


static void	(*extfn)(REG8 enable);


// ----

static	REG8	rapids = 0;

REG8 fmboard_getjoy(PSGGEN psg) {

	REG8	ret;

	rapids ^= 0xf0;											// ver0.28
	ret = 0xff;
	if (!(psg->reg.io2 & 0x40)) {
		ret &= (joymng_getstat() | (rapids & 0x30));
		if (np2cfg.KEY_MODE == 1) {
			ret &= keystat_getjoy();
		}
	}
	else {
		if (np2cfg.KEY_MODE == 2) {
			ret &= keystat_getjoy();
		}
	}
	if (np2cfg.BTN_RAPID) {
		ret |= rapids;
	}

	// rapid‚Æ”ñrapid‚ğ‡¬								// ver0.28
	ret &= ((ret >> 2) | (~0x30));

	if (np2cfg.BTN_MODE) {
		UINT8 bit1 = (ret & 0x20) >> 1;					// ver0.28
		UINT8 bit2 = (ret & 0x10) << 1;
		ret = (ret & (~0x30)) | bit1 | bit2;
	}

	// intr ”½‰f‚µ‚ÄI‚í‚è								// ver0.28
	ret &= 0x3f;
	ret |= g_fmtimer.intr;
	return(ret);
}


// ----

void fmboard_extreg(void (*ext)(REG8 enable)) {

	extfn = ext;
}

void fmboard_extenable(REG8 enable) {

	if (extfn) {
		(*extfn)(enable);
	}
}



// ----

static void setfmregs(UINT8 *reg) {

	FillMemory(reg + 0x30, 0x60, 0xff);
	FillMemory(reg + 0x90, 0x20, 0x00);
	FillMemory(reg + 0xb0, 0x04, 0x00);
	FillMemory(reg + 0xb4, 0x04, 0xc0);
}

void fmboard_reset(const NP2CFG *pConfig, UINT32 type) {

	UINT8	cross;

	soundrom_reset();
	beep_reset();												// ver0.27a
	cross = pConfig->snd_x;										// ver0.30

	extfn = NULL;
	ZeroMemory(&g_opn, sizeof(g_opn));
	setfmregs(g_opn.reg + 0x000);
	setfmregs(g_opn.reg + 0x100);
	setfmregs(g_opn.reg + 0x200);
	setfmregs(g_opn.reg + 0x300);
	g_opn.reg[0x07] = 0xbf;
	g_opn.reg[0x0e] = 0xff;
	g_opn.reg[0x0f] = 0xff;
	g_opn.reg[0xff] = 0x01;
	g_opn.channels = 3;
	g_opn.adpcmmask = (UINT8)~(0x1c);

#if defined(SUPPORT_PX)
	ZeroMemory(&g_opn2, sizeof(g_opn2));
	setfmregs(g_opn2.reg + 0x000);
	setfmregs(g_opn2.reg + 0x100);
	setfmregs(g_opn2.reg + 0x200);
	setfmregs(g_opn2.reg + 0x300);
	g_opn2.reg[0x07] = 0xbf;
	g_opn2.reg[0x0e] = 0xff;
	g_opn2.reg[0x0f] = 0xff;
	g_opn2.reg[0xff] = 0x01;
	g_opn2.channels = 3;
	g_opn2.adpcmmask = (UINT8)~(0x1c);

	ZeroMemory(&g_opn3, sizeof(g_opn3));
	setfmregs(g_opn3.reg + 0x000);
	setfmregs(g_opn3.reg + 0x100);
	setfmregs(g_opn3.reg + 0x200);
	setfmregs(g_opn3.reg + 0x300);
	g_opn3.reg[0x07] = 0xbf;
	g_opn3.reg[0x0e] = 0xff;
	g_opn3.reg[0x0f] = 0xff;
	g_opn3.reg[0xff] = 0x01;
	g_opn3.channels = 3;
	g_opn3.adpcmmask = (UINT8)~(0x1c);
#endif	// defined(SUPPORT_PX)

	opngen_reset(&g_opngen);
	psggen_reset(&g_psg1);
	psggen_reset(&g_psg2);
	psggen_reset(&g_psg3);
	rhythm_reset(&g_rhythm);
#if defined(SUPPORT_PX)
	rhythm_reset(&g_rhythm2);
	rhythm_reset(&g_rhythm3);
#endif	// defined(SUPPORT_PX)
	adpcm_reset(&g_adpcm);
#if defined(SUPPORT_PX)
	adpcm_reset(&g_adpcm2);
	adpcm_reset(&g_adpcm3);
#endif	// defined(SUPPORT_PX)
	pcm86_reset();
	cs4231_reset();

	board14_reset(pConfig, (type == 1) ? TRUE : FALSE);
	amd98_reset(pConfig);

	switch (type)
	{
		case 0x01:
			break;

		case 0x02:
			board26k_reset(pConfig);
			break;

		case 0x04:
			board86_reset(pConfig);
			break;

		case 0x06:
			boardx2_reset(pConfig);
			break;

		case 0x08:
			board118_reset(pConfig);
			break;

		case 0x14:
			board86_reset(pConfig);
			break;

		case 0x20:
			boardspb_reset(pConfig);
			cross ^= pConfig->spb_x;
			break;

		case 0x40:
			boardspr_reset(pConfig);
			cross ^= pConfig->spb_x;
			break;

		case 0x80:
			break;

#if	defined(SUPPORT_PX)
		case 0x30:
			boardpx1_reset(pConfig);
			break;

		case 0x50:
			boardpx2_reset(pConfig);
			break;
#endif	// defined(SUPPORT_PX)

		default:
			type = 0;
			break;
	}
	g_usesound = type;
	soundmng_setreverse(cross);
	keydisp_setfmboard(type);
	opngen_setVR(pConfig->spb_vrc, pConfig->spb_vrl);
}

void fmboard_bind(void) {

	switch(g_usesound) {
		case 0x01:
			board14_bind();
			break;

		case 0x02:
			board26k_bind();
			break;

		case 0x04:
			board86_bind();
			break;

		case 0x06:
			boardx2_bind();
			break;

		case 0x08:
			board118_bind();
			break;

		case 0x14:
			board86c_bind();
			break;

		case 0x20:
			boardspb_bind();
			break;

		case 0x40:
			boardspr_bind();
			break;

		case 0x80:
			amd98_bind();
			break;

#if defined(SUPPORT_PX)
		case 0x30:
			boardpx1_bind();
			break;

		case 0x50:
			boardpx2_bind();
			break;
#endif	// defined(SUPPORT_PX)
	}
	sound_streamregist(&g_beep, (SOUNDCB)beep_getpcm);
}


// ----

void fmboard_fmrestore(OPN_T* pOpn, REG8 chbase, UINT bank)
{
	REG8 i;
	const UINT8 *reg;

	reg = pOpn->reg + (bank * 0x100);
	for (i = 0x30; i < 0xa0; i++)
	{
		opngen_setreg(&g_opngen, chbase, i, reg[i]);
	}
	for (i = 0xb7; i >= 0xa0; i--)
	{
		opngen_setreg(&g_opngen, chbase, i, reg[i]);
	}
	for (i = 0; i < 3; i++)
	{
		opngen_keyon(&g_opngen, chbase + i, g_opngen.opnch[chbase + i].keyreg);
	}
}

void fmboard_psgrestore(OPN_T* pOpn, PSGGEN psg, UINT bank)
{
	const UINT8 *reg;
	UINT i;

	reg = pOpn->reg + (bank * 0x100);
	for (i=0; i < 0x10; i++)
	{
		psggen_setreg(psg, i, reg[i]);
	}
}

void fmboard_rhyrestore(OPN_T* pOpn, RHYTHM rhy, UINT bank)
{
	const UINT8 *reg;

	reg = pOpn->reg + (bank * 0x100);
	rhythm_setreg(rhy, 0x11, reg[0x11]);
	rhythm_setreg(rhy, 0x18, reg[0x18]);
	rhythm_setreg(rhy, 0x19, reg[0x19]);
	rhythm_setreg(rhy, 0x1a, reg[0x1a]);
	rhythm_setreg(rhy, 0x1b, reg[0x1b]);
	rhythm_setreg(rhy, 0x1c, reg[0x1c]);
	rhythm_setreg(rhy, 0x1d, reg[0x1d]);
}

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
	_OPNA		g_opna[OPNA_MAX];

	_FMTIMER	g_fmtimer;
	_PCM86		pcm86;
	_CS4231		cs4231;

static void	(*extfn)(REG8 enable);


// ----

static	REG8	s_rapids = 0;

REG8 fmboard_getjoy(POPNA opna)
{
	REG8 ret;

	s_rapids ^= 0xf0;											// ver0.28
	ret = 0xff;
	if (!(opna->s.reg[15] & 0x40))
	{
		ret &= (joymng_getstat() | (s_rapids & 0x30));
		if (np2cfg.KEY_MODE == 1)
		{
			ret &= keystat_getjoy();
		}
	}
	else
	{
		if (np2cfg.KEY_MODE == 2)
		{
			ret &= keystat_getjoy();
		}
	}
	if (np2cfg.BTN_RAPID)
	{
		ret |= s_rapids;
	}

	// rapid‚Æ”ñrapid‚ð‡¬								// ver0.28
	ret &= ((ret >> 2) | (~0x30));

	if (np2cfg.BTN_MODE)
	{
		UINT8 bit1 = (ret & 0x20) >> 1;					// ver0.28
		UINT8 bit2 = (ret & 0x10) << 1;
		ret = (ret & (~0x30)) | bit1 | bit2;
	}

	// intr ”½‰f‚µ‚ÄI‚í‚è								// ver0.28
	ret &= 0x3f;
	ret |= g_fmtimer.intr;
	return ret;
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

/**
 * Constructor
 */
void fmboard_construct(void)
{
	UINT i;

	for (i = 0; i < NELEMENTS(g_opna); i++)
	{
		opna_construct(&g_opna[i]);
	}
}

/**
 * Destructor
 */
void fmboard_destruct(void)
{
	UINT i;

	for (i = 0; i < NELEMENTS(g_opna); i++)
	{
		opna_destruct(&g_opna[i]);
	}
}

/**
 * Reset
 */
void fmboard_reset(const NP2CFG *pConfig, UINT32 type) {

	UINT8 cross;
	UINT i;

	soundrom_reset();
	beep_reset();												// ver0.27a
	cross = pConfig->snd_x;										// ver0.30

	if (g_usesound != type)
	{
		for (i = 0; i < NELEMENTS(g_opna); i++)
		{
			opna_reset(&g_opna[i], 0);
		}
	}

	extfn = NULL;
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
			board86_reset(pConfig, FALSE);
			break;

		case 0x06:
			boardx2_reset(pConfig);
			break;

		case 0x08:
			board118_reset(pConfig);
			break;

		case 0x14:
			board86_reset(pConfig, TRUE);
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
	opngen_setVR(pConfig->spb_vrc, pConfig->spb_vrl);
}

void fmboard_bind(void) {

	keydisp_reset();
	switch (g_usesound)
	{
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
			board86_bind();
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

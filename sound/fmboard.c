#include	"compiler.h"
#include	"joymng.h"
#include	"soundmng.h"
// #include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cbuscore.h"
#include	"board14.h"
#include	"board26k.h"
#include	"board86.h"
#include	"boardx2.h"
#include	"board118.h"
#include	"boardspb.h"
#include	"amd98.h"
#include	"pcm86io.h"
#include	"cs4231io.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"beep.h"
#include	"keydisp.h"


	UINT32		usesound;
	OPN_T		opn;
	AMD98		amd98;
	MUSICGEN	musicgen;

	_TMS3631	tms3631;
	_FMTIMER	fmtimer;
	_OPNGEN		opngen;
	OPNCH		opnch[OPNCH_MAX];
	_PSGGEN		psg1;
	_PSGGEN		psg2;
	_PSGGEN		psg3;
	_RHYTHM		rhythm;
	_ADPCM		adpcm;
	_PCM86		pcm86;
	_CS4231		cs4231;


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
			ret &= keyext_getjoy();
		}
	}
	else {
		if (np2cfg.KEY_MODE == 2) {
			ret &= keyext_getjoy();
		}
	}
	if (np2cfg.BTN_RAPID) {
		ret |= rapids;
	}

	// rapid�Ɣ�rapid������								// ver0.28
	ret &= ((ret >> 2) | (~0x30));

	if (np2cfg.BTN_MODE) {
		BYTE bit1 = (ret & 0x20) >> 1;					// ver0.28
		BYTE bit2 = (ret & 0x10) << 1;
		ret = (ret & (~0x30)) | bit1 | bit2;
	}

	// intr ���f���ďI���								// ver0.28
	ret &= 0x3f;
	ret |= fmtimer.intr;
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

void fmboard_reset(UINT32 type) {

	BYTE	cross;

	soundrom_reset();
	beep_reset();												// ver0.27a
	cross = np2cfg.snd_x;										// ver0.30

	extfn = NULL;
	ZeroMemory(&opn, sizeof(opn));
	opn.channels = 3;
	opn.adpcmmask = (BYTE)~(0x1c);
	opn.reg[0xff] = 0x01;

	ZeroMemory(&musicgen, sizeof(musicgen));
	ZeroMemory(&amd98, sizeof(amd98));

	tms3631_reset(&tms3631);
	opngen_reset();
	psggen_reset(&psg1);
	psggen_reset(&psg2);
	psggen_reset(&psg3);
	rhythm_reset(&rhythm);
	adpcm_reset(&adpcm);
	pcm86_reset();
	cs4231_reset();

	switch(type) {
		case 0x01:
			board14_reset();
			break;

		case 0x02:
			board26k_reset();
			break;

		case 0x04:
			board86_reset();
			break;

		case 0x06:
			boardx2_reset();
			break;

		case 0x08:
			board118_reset();
			break;

		case 0x14:
			board86_reset();
			break;

		case 0x20:
			boardspb_reset();
			cross ^= np2cfg.spb_x;
			break;

		case 0x40:
			boardspr_reset();
			cross ^= np2cfg.spb_x;
			break;

		case 0x80:
//			amd98_reset();
			break;

		default:
			type = 0;
			break;
	}
	usesound = type;
	soundmng_setreverse(cross);
	keydisp_setfmboard(type);
	opngen_setVR(np2cfg.spb_vrc, np2cfg.spb_vrl);
}

void fmboard_bind(void) {

	switch(usesound) {
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
	}
	sound_streamregist(&beep, (SOUNDCB)beep_getpcm);
}


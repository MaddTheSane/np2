#include	"compiler.h"
#include	"soundmng.h"
#include	"pccore.h"
#include	"fdd_mtr.h"
#if defined(SUPPORT_WAVEMIX)
#include	"wavemix.h"
#endif


		int		fddmtr_biosbusy = 0;							// ver0.26
static	BYTE	mtr_curevent = 0;
static	UINT	nextevent = 0;
static	BYTE	FDC_HEAD[4] = {0, 0, 0, 0};
static	BYTE	curdrv = 0;

enum {
	MOVE1TCK_MS		= 15,
	MOVEMOTOR1_MS	= 25,
	DISK1ROL_MS		= 166
};

static void fddmtr_event(void) {

	switch(mtr_curevent) {
		case 100:
#if defined(SUPPORT_WAVEMIX)
			wavemix_stop(SOUND_PCMSEEK);
#else
			soundmng_pcmstop(SOUND_PCMSEEK);
#endif
			mtr_curevent = 0;
			break;

		default:
			mtr_curevent = 0;
			break;
	}
}

void fddmtr_init(void) {

	fddmtr_event();
	FillMemory(FDC_HEAD, sizeof(FDC_HEAD), 42);
}

void fddmtr_callback(UINT time) {

	if ((mtr_curevent) && (time >= nextevent)) {
		fddmtr_event();
	}
}

void fdbiosout(NEVENTITEM item) {

	fddmtr_biosbusy = 0;
	(void)item;
}

void fddmtr_seek(BYTE drv, BYTE c, UINT size) {

	int		regmove = 0;
	SINT32	waitms = 0;

	if (c != 0xff) {
		regmove = FDC_HEAD[curdrv] - c;
		FDC_HEAD[curdrv] = c;
	}
	if (!np2cfg.MOTOR) {
		SINT32 s = size * pccore.multiple;
		if (s) {													// ver0.28
			fddmtr_biosbusy = 1;
			nevent_set(NEVENT_FDBIOSBUSY, s, fdbiosout, NEVENT_ABSOLUTE);
		}
		return;
	}

	if (regmove < 0) {												// ver0.26
		regmove *= (-1);
	}
	if (regmove == 1) {
		if (mtr_curevent < 80) {
			fddmtr_event();
#if defined(SUPPORT_WAVEMIX)
			wavemix_play(SOUND_PCMSEEK1, FALSE);
#else
			soundmng_pcmplay(SOUND_PCMSEEK1, FALSE);
#endif
			mtr_curevent = 80;
			nextevent = GETTICK() + MOVEMOTOR1_MS;
		}
	}
	else if (regmove) {
		if (mtr_curevent < 100) {
			fddmtr_event();
#if defined(SUPPORT_WAVEMIX)
			wavemix_play(SOUND_PCMSEEK, TRUE);
#else
			soundmng_pcmplay(SOUND_PCMSEEK, TRUE);
#endif
			mtr_curevent = 100;
			nextevent = GETTICK() + (regmove * MOVE1TCK_MS);
		}
		if (regmove >= 32) {
			waitms += DISK1ROL_MS;
		}
	}
	waitms += (size * DISK1ROL_MS) / (1024 * 8);
	if (waitms) {
		fddmtr_biosbusy = 1;
		nevent_setbyms(NEVENT_FDBIOSBUSY, waitms, fdbiosout, NEVENT_ABSOLUTE);
	}
	(void)drv;
}


#include	"compiler.h"
#ifndef NP2GCC
#include	<fp.h>
#endif
#include	"parts.h"
#include	"soundmng.h"
#include	"sound.h"
#if defined(VERMOUTH_LIB)
#include	"vermouth.h"
#endif


#define	SOUNDBUFFERS	2

typedef struct {
	SndChannelPtr	hdl;
	SndCallBackUPP	cb;
	UINT			rate;
	UINT			samples;
	UINT			buffersize;
	ExtSoundHeader	*buf[SOUNDBUFFERS];
	SndCommand		cmd[SOUNDBUFFERS];
	SndCommand		cbcmd[SOUNDBUFFERS];
} _QSOUND, *QSOUND;

static	BOOL		QS_Avail = FALSE;
static	_QSOUND		QSound;
static	BOOL		QSound_Playing = FALSE;

#if defined(VERMOUTH_LIB)
		MIDIMOD		vermouth_module = NULL;
#endif


static pascal void QSoundCallback(SndChannelPtr inCh, SndCommand *inCmd) {

	QSOUND		qs;
	int			nextbuf;
	void		*dst;
const SINT32	*src;

	if (QS_Avail) {
		qs = &QSound;
		nextbuf = inCmd->param1;
		dst = qs->buf[nextbuf]->sampleArea;
		src = NULL;
		if (QSound_Playing) {
			src = sound_pcmlock();
		}
		if (src) {
			satuation_s16((SINT16 *)dst, src, qs->buffersize);
			sound_pcmunlock(src);
		}
		else {
			ZeroMemory(dst, qs->buffersize);
		}
		SndDoCommand(qs->hdl, &qs->cmd[nextbuf], TRUE);
		SndDoCommand(qs->hdl, &qs->cbcmd[nextbuf], TRUE);
	}
	(void)inCh;
}

static BOOL SoundChannel_Init(void) {

volatile QSOUND		qs;

	qs = &QSound;
	ZeroMemory(qs, sizeof(QSOUND));
#if TARGET_API_MAC_CARBON
	qs->cb = NewSndCallBackUPP(QSoundCallback);
#else
	qs->cb = NewSndCallBackProc(QSoundCallback);
#endif

	if (SndNewChannel(&qs->hdl, sampledSynth,
				initStereo + initNoInterp + initNoDrop, qs->cb) != noErr) {
		return(FAILURE);
	}
	return(SUCCESS);
}

static void SoundChannel_Term(void) {

volatile QSOUND		qs;
	SndChannelPtr	hdl;
	SndCallBackUPP	cb;

	qs = &QSound;
	hdl = qs->hdl;
	qs->hdl = NULL;
	cb = qs->cb;
	qs->cb = NULL;

#if TARGET_API_MAC_CARBON
	if (cb) {
		DisposeSndCallBackUPP(cb);
	}
#endif
	if (hdl) {
		SndDisposeChannel(hdl, TRUE);
	}
}

static BOOL SoundBuffer_Init(UINT rate, UINT samples) {

	QSOUND			qs;
	double			drate;
	extended80		extFreq;
	UINT			buffersize;
	int				i;
	ExtSoundHeader	*buf;

	qs = &QSound;
	qs->rate = rate;
	qs->samples = samples;
	buffersize = samples * 4;
	qs->buffersize = buffersize;
	drate = rate;
	dtox80(&drate, &extFreq);
	buffersize += sizeof(ExtSoundHeader);
	for (i=0; i<SOUNDBUFFERS; i++) {
		buf = (ExtSoundHeader *)_MALLOC(buffersize, "ExtSoundHeader");
		qs->buf[i] = buf;
		if (buf == NULL) {
			goto sbinit_err;
		}
		ZeroMemory(buf, buffersize);
		buf->numChannels = 2;
		buf->sampleRate = (UInt32)rate << 16;
		buf->encode = extSH;
		buf->numFrames = samples;
		buf->AIFFSampleRate = extFreq;
		buf->sampleSize = 16;

		qs->cmd[i].cmd = bufferCmd;
		qs->cmd[i].param2 = (SInt32)buf;
		qs->cbcmd[i].cmd = callBackCmd;
		qs->cbcmd[i].param1 = (i + 1) % SOUNDBUFFERS;
	}

	QS_Avail = TRUE;
	SndDoCommand(qs->hdl, &qs->cmd[0], TRUE);
	SndDoCommand(qs->hdl, &qs->cbcmd[0], TRUE);
	return(SUCCESS);

sbinit_err:
	return(FAILURE);
}

static void SoundBuffer_Term(void) {

	QSOUND			qs;
	ExtSoundHeader	**buf;
	int				i;

	qs = &QSound;
	buf = qs->buf;
	for (i=0; i<SOUNDBUFFERS; i++) {
		if (buf[i]) {
			_MFREE(buf[i]);
			buf[i] = NULL;
		}
	}
}

UINT soundmng_create(UINT rate, UINT ms) {

	UINT	samples;
#if defined(VERMOUTH_LIB)
	UINT	num;
#endif

	QSound_Playing = FALSE;

	if (rate < 11025) {
		rate = 11025;
	}
	else if (rate > 44100) {
		rate = 44100;
	}

	if (SoundChannel_Init()) {
		goto qsinit_err;
	}
	samples = (rate / 3) & (~3);
	if (SoundBuffer_Init(rate, samples)) {
		goto qsinit_err;
	}
#if defined(VERMOUTH_LIB)
	vermouth_module = midimod_create(rate);
	for (num=0; num<128; num++) {
		midimod_loadprogram(vermouth_module, num);
		midimod_loadrhythm(vermouth_module, num);
	}
#endif
	return(samples);

qsinit_err:
	soundmng_destroy();
	(void)ms;
	return(0);
}

void soundmng_destroy(void) {

	if (QS_Avail) {
#if defined(VERMOUTH_LIB)
		midimod_destroy(vermouth_module);
		vermouth_module = NULL;
#endif
		QS_Avail = FALSE;
		SoundBuffer_Term();
		SoundChannel_Term();
	}
}

void soundmng_play(void) {

	QSound_Playing = TRUE;
}

void soundmng_stop(void) {

	QSound_Playing = FALSE;
}


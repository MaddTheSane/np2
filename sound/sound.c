#include	"compiler.h"
#include	"soundmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"sndcsec.h"
#include	"beep.h"


	UINT32	opna_rate = 22050;

static int	writebytes = 0;
	UINT32	ratebase200 = 110;
	UINT32	dsound_lastclock = 0;




#define	STREAM_CBMAX	16

typedef struct {
	void	*hdl;
	SOUNDCB	cbfn;
} CBTBL;

typedef struct {
	SINT32	*buffer;
	SINT32	*ptr;
	UINT	samples;
	UINT	remain;
	CBTBL	*cbreg;
	CBTBL	cb[STREAM_CBMAX];
} SNDSTREAM;

static	SNDSTREAM	sndstream;


static void streamreset(void) {

	sndstream.ptr = sndstream.buffer;
	sndstream.remain = sndstream.samples;
	sndstream.cbreg = sndstream.cb;
}

static void streamprepare(UINT samples) {

	CBTBL	*cb;
	UINT	count;

	count = min(sndstream.remain, samples);
	if (count) {
		ZeroMemory(sndstream.ptr, count * 2 * sizeof(SINT32));
		cb = sndstream.cb;
		while(cb < sndstream.cbreg) {
			cb->cbfn(cb->hdl, sndstream.ptr, count);
			cb++;
		}
		sndstream.ptr += count * 2;
		sndstream.remain -= count;
	}
}


// ----

BOOL sound_create(UINT rate, UINT ms) {

	UINT	samples;

	ZeroMemory(&sndstream, sizeof(sndstream));
	if (rate == 0) {
		return(SUCCESS);
	}
	switch(rate) {
		case 11025:
			opna_rate = 11025;
			break;

		case 22050:
			opna_rate = 22050;
			break;

		case 44100:
			opna_rate = 44100;
			break;

		default:
			return(FAILURE);
	}
	samples = soundmng_create(rate, ms);
	if (samples == 0) {
		goto scre_err1;
	}
	soundmng_reset();

	sndstream.buffer = (SINT32 *)_MALLOC(samples * 2 * sizeof(SINT32),
																"stream");
	if (sndstream.buffer == NULL) {
		goto scre_err2;
	}
	sndstream.samples = samples;
	streamreset();
	ratebase200 = (opna_rate + 199) / 200;

	SNDCSEC_INIT;
	return(SUCCESS);

scre_err2:
	soundmng_destroy();

scre_err1:
	return(FAILURE);
}

void sound_destroy(void) {

	if (sndstream.buffer) {
		SNDCSEC_TERM;

		soundmng_stop();
		soundmng_destroy();
		_MFREE(sndstream.buffer);
		sndstream.buffer = NULL;
	}
}

void sound_reset(void) {

	if (sndstream.buffer) {
		soundmng_reset();
		streamreset();
		dsound_lastclock = nevent.clock;
		beep_eventreset();
	}
}

void sound_streamregist(void *hdl, SOUNDCB cbfn) {

	if (sndstream.buffer) {
		if ((cbfn) &&
			(sndstream.cbreg < (sndstream.cb + STREAM_CBMAX))) {
			sndstream.cbreg->hdl = hdl;
			sndstream.cbreg->cbfn = cbfn;
			sndstream.cbreg++;
		}
	}
}


// ----

void sound_sync(void) {

	UINT	length;

	if (sndstream.buffer == NULL) {
		return;
	}

	length = (nevent.clock + nevent.baseclock - nevent.remainclock
										- dsound_lastclock) * ratebase200;
	if (length < pc.dsoundclock2) {
		return;
	}
	length /= pc.dsoundclock;
	if (length) {
		SNDCSEC_ENTER;
		streamprepare(length);
		SNDCSEC_LEAVE;
		writebytes += length;
		dsound_lastclock += (length * pc.dsoundclock / ratebase200);
		beep_eventreset();
	}

	if (writebytes >= 100) {
		writebytes = 0;
		soundmng_sync();
	}
}

const SINT32 *sound_pcmlock(void) {

const SINT32 *ret;

	ret = sndstream.buffer;
	if (ret) {
		SNDCSEC_ENTER;
		if (sndstream.remain) {
			streamprepare(sndstream.remain);
			dsound_lastclock = nevent.clock;
			beep_eventreset();
		}
	}
	return(ret);
}

void sound_pcmunlock(const SINT32 *hdl) {

	if (hdl) {
		sndstream.ptr = sndstream.buffer;
		sndstream.remain = sndstream.samples;
		SNDCSEC_LEAVE;
	}
}


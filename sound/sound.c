#include	"compiler.h"
#include	"soundmng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"sndcsec.h"
#include	"beep.h"

	SOUNDCFG	soundcfg;


#define	STREAM_CBMAX	16

typedef struct {
	void	*hdl;
	SOUNDCB	cbfn;
} CBTBL;

typedef struct {
	SINT32	*buffer;
	SINT32	*ptr;
	UINT	samples;
	UINT	reserve;
	UINT	remain;
	CBTBL	*cbreg;
	CBTBL	cb[STREAM_CBMAX];
} SNDSTREAM;

static	SNDSTREAM	sndstream;


static void streamreset(void) {

	SNDCSEC_ENTER;
	sndstream.ptr = sndstream.buffer;
	sndstream.remain = sndstream.samples + sndstream.reserve;
	sndstream.cbreg = sndstream.cb;
	SNDCSEC_LEAVE;
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
	UINT	reserve;

	ZeroMemory(&sndstream, sizeof(sndstream));
	switch(rate) {
		case 11025:
		case 22050:
		case 44100:
			break;

		default:
			return(FAILURE);
	}
	samples = soundmng_create(rate, ms);
	if (samples == 0) {
		goto scre_err1;
	}
	soundmng_reset();

	soundcfg.rate = rate;
	sound_changeclock();

#if defined(SOUNDRESERVE)
	reserve = rate * SOUNDRESERVE / 1000;
#else
	reserve = 0;
#endif
	sndstream.buffer = (SINT32 *)_MALLOC((samples + reserve) * 2 
												* sizeof(SINT32), "stream");
	if (sndstream.buffer == NULL) {
		goto scre_err2;
	}
	sndstream.samples = samples;
	sndstream.reserve = reserve;

	SNDCSEC_INIT;
	streamreset();
	return(SUCCESS);

scre_err2:
	soundmng_destroy();

scre_err1:
	return(FAILURE);
}

void sound_destroy(void) {

	if (sndstream.buffer) {
		soundmng_stop();
		streamreset();
		soundmng_destroy();
		SNDCSEC_TERM;
		_MFREE(sndstream.buffer);
		sndstream.buffer = NULL;
	}
}

void sound_reset(void) {

	if (sndstream.buffer) {
		soundmng_reset();
		streamreset();
		soundcfg.lastclock = CPU_CLOCK;
		beep_eventreset();
	}
}

void sound_changeclock(void) {

	UINT32	clock;
	UINT	hz;
	UINT	hzmax;

	if (sndstream.buffer == NULL) {
		return;
	}

	// とりあえず 25で割り切れる。
	clock = pc.realclock / 25;
	hz = soundcfg.rate / 25;

	// で、クロック数に合せて調整。(64bit演算しろよな的)
	hzmax = (1 << (32 - 8)) / (clock >> 8);
	while(hzmax < hz) {
		clock = (clock + 1) >> 1;
		hz = (hz + 1) >> 1;
	}
	soundcfg.hzbase = hz;
	soundcfg.clockbase = clock;
	soundcfg.minclock = 2 * clock / hz;
	soundcfg.lastclock = CPU_CLOCK;
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

	UINT32	length;

	if (sndstream.buffer == NULL) {
		return;
	}

	length = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK - soundcfg.lastclock;
	if (length < soundcfg.minclock) {
		return;
	}
	length = (length * soundcfg.hzbase) / soundcfg.clockbase;
	if (length == 0) {
		return;
	}
	SNDCSEC_ENTER;
	streamprepare(length);
	soundcfg.lastclock += length * soundcfg.clockbase / soundcfg.hzbase;
	beep_eventreset();
	SNDCSEC_LEAVE;

	soundcfg.writecount += length;
	if (soundcfg.writecount >= 100) {
		soundcfg.writecount = 0;
		soundmng_sync();
	}
}

static volatile int locks = 0;

const SINT32 *sound_pcmlock(void) {

const SINT32 *ret;

	if (locks) {
		TRACEOUT(("sound pcm lock: already locked"));
		return(NULL);
	}
	locks++;
	ret = sndstream.buffer;
	if (ret) {
		SNDCSEC_ENTER;
		if (sndstream.remain > sndstream.reserve) {
			streamprepare(sndstream.remain - sndstream.reserve);
			soundcfg.lastclock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
			beep_eventreset();
		}
	}
	else {
		locks--;
	}
	return(ret);
}

void sound_pcmunlock(const SINT32 *hdl) {

	int		leng;

	if (hdl) {
		leng = sndstream.reserve - sndstream.remain;
		if (leng > 0) {
			CopyMemory(sndstream.buffer,
						sndstream.buffer + (sndstream.samples * 2),
												leng * 2 * sizeof(SINT32));
		}
		sndstream.ptr = sndstream.buffer + (leng * 2);
		sndstream.remain = sndstream.samples + sndstream.reserve - leng;
//		sndstream.remain += sndstream.samples;
		SNDCSEC_LEAVE;
		locks--;
	}
}


#include	"compiler.h"

#if defined(SUPPORT_WAVEMIX)

#include	"sound.h"
#include	"getsnd.h"
#include	"wavemix.res"

typedef struct {
const SINT16	*pcm;
	UINT		remain;
	SINT16		*sample;
	UINT		samples;
	BOOL		loop;
} _WMCH, *WMCH;

typedef struct {
	UINT32	playing;
	int		vol;
	_WMCH	ch[WAVEMIX_CHANNELS];
} _WAVEMIX, *WAVEMIX;

static	_WAVEMIX	wavemix;


static SINT16 *getpcmdata(void *datptr, UINT datsize,
										UINT rate, UINT *samples) {

	GETSND	gs;
	BYTE	tmp[256];
	UINT	size;
	UINT	r;
	SINT16	*ret;

	gs = getsnd_create(datptr, datsize);
	if (gs == NULL) {
		goto gpd_err1;
	}
	if (getsnd_setmixproc(gs, rate, 1) != SUCCESS) {
		goto gpd_err2;
	}
	size = 0;
	do {
		r = getsnd_getpcmbyleng(gs, tmp, sizeof(tmp));
		size += r;
	} while(r);
	getsnd_destroy(gs);
	if (size == 0) {
		goto gpd_err1;
	}

	ret = (SINT16 *)_MALLOC(size, "SEEKSND");
	if (ret == NULL) {
		goto gpd_err1;
	}
	gs = getsnd_create(datptr, datsize);
	if (gs == NULL) {
		goto gpd_err1;
	}
	if (getsnd_setmixproc(gs, rate, 1) != SUCCESS) {
		goto gpd_err2;
	}
	r = getsnd_getpcmbyleng(gs, ret, size);
	getsnd_destroy(gs);
	if (samples) {
		*samples = (r / 2);
	}
	return(ret);

gpd_err2:
	getsnd_destroy(gs);

gpd_err1:
	return(NULL);
}

static void SOUNDCALL wavemix_getpcm(WAVEMIX hdl, SINT32 *pcm, UINT count) {

	UINT		i;
	WMCH		c;
const SINT16	*s;
	UINT		srem;
	SINT32		*d;
	UINT		drem;
	UINT		r;
	UINT		j;

	if ((hdl->playing == 0) || (count == 0))  {
		return;
	}
	c = hdl->ch;
	for (i=0; i<WAVEMIX_CHANNELS; i++, c++) {
		if (hdl->playing & (1 << i)) {
			s = c->pcm;
			srem = c->remain;
			d = pcm;
			drem = count;
			do {
				r = min(srem, drem);
				for (j=0; j<r; j++) {
					d[j*2+0] += s[j];
					d[j*2+1] += s[j];
				}
				s += r;
				d += r*2;
				srem -= r;
				if (srem == 0) {
					if (c->loop) {
						s = c->sample;
						srem = c->samples;
					}
					else {
						hdl->playing &= ~(1 << i);
						break;
					}
				}
				drem -= r;
			} while(drem);
			c->pcm = s;
			c->remain = srem;
		}
	}
}


// ----

void wavemix_initialize(UINT rate) {

	SINT16	*sample;
	UINT	samples;

	ZeroMemory(&wavemix, sizeof(wavemix));

	sample = getpcmdata((void *)fddseek, sizeof(fddseek), rate, &samples);
	if (sample) {
		wavemix.ch[0].sample = sample;
		wavemix.ch[0].samples = samples;
	}
	sample = getpcmdata((void *)fddseek1, sizeof(fddseek1), rate, &samples);
	if (sample) {
		wavemix.ch[1].sample = sample;
		wavemix.ch[1].samples = samples;
	}
}

void wavemix_deinitialize(void) {

	WMCH	c;
	WMCH	cterm;

	c = wavemix.ch;
	cterm = c + WAVEMIX_CHANNELS;
	while(c < cterm) {
		if (c->sample) {
			_MFREE(c->sample);
		}
		c++;
	}
	ZeroMemory(&wavemix, sizeof(wavemix));
}

void wavemix_play(UINT num, BOOL loop) {

	WMCH	c;

	if (num < WAVEMIX_CHANNELS) {
		c = wavemix.ch + num;
		if ((c->sample) && (c->samples)) {
			sound_sync();
			c->pcm = c->sample;
			c->remain = c->samples;
			c->loop = loop;
			wavemix.playing |= (1 << num);
		}
	}
}

void wavemix_stop(UINT num) {

	if (num < WAVEMIX_CHANNELS) {
		sound_sync();
		wavemix.playing &= ~(1 << num);
	}
}

void wavemix_bind(void) {

	sound_streamregist(&wavemix, (SOUNDCB)wavemix_getpcm);
}

#endif


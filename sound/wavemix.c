#include	"compiler.h"

#if defined(SUPPORT_WAVEMIX)

#include	"pccore.h"
#include	"sound.h"
#include	"getsnd.h"
#include	"wavemix.res"

typedef struct {
const SINT16	*pcm;
	UINT		remain;
	SINT16		*sample;
	UINT		samples;
	UINT		flag;
	SINT32		volume;
} _WMTRK, *WMTRK;

typedef struct {
	UINT32	playing;
	_WMTRK	trk[WAVEMIX_MAXTRACK];
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

	UINT32		bitmap;
	WMTRK		t;
const SINT16	*s;
	UINT		srem;
	SINT32		*d;
	UINT		drem;
	UINT		r;
	UINT		j;
	UINT		flag;
	SINT32		vol;
	SINT32		samp;

	if ((hdl->playing == 0) || (count == 0))  {
		return;
	}
	t = hdl->trk;
	bitmap = 1;
	do {
		if (hdl->playing & bitmap) {
			s = t->pcm;
			srem = t->remain;
			d = pcm;
			drem = count;
			flag = t->flag;
			vol = t->volume;
			do {
				r = min(srem, drem);
				switch(flag & (WMFLAG_L | WMFLAG_R)) {
					case WMFLAG_L:
						for (j=0; j<r; j++) {
							d[j*2+0] += (s[j] * vol) >> 12;
						}
						break;

					case WMFLAG_R:
						for (j=0; j<r; j++) {
							d[j*2+1] += (s[j] * vol) >> 12;
						}
						break;

					case WMFLAG_L | WMFLAG_R:
						for (j=0; j<r; j++) {
							samp = (s[j] * vol) >> 12;
							d[j*2+0] += samp;
							d[j*2+1] += samp;
						}
						break;
				}
				s += r;
				d += r*2;
				srem -= r;
				if (srem == 0) {
					if (flag & WMFLAG_LOOP) {
						s = t->sample;
						srem = t->samples;
					}
					else {
						hdl->playing &= ~bitmap;
						break;
					}
				}
				drem -= r;
			} while(drem);
			t->pcm = s;
			t->remain = srem;
		}
		t++;
		bitmap <<= 1;
	} while(bitmap < (1 << WAVEMIX_MAXTRACK));
}


// ----

void wavemix_initialize(UINT rate) {

	SINT16	*sample;
	UINT	samples;

	ZeroMemory(&wavemix, sizeof(wavemix));

	sample = getpcmdata((void *)fddseek, sizeof(fddseek), rate, &samples);
	if (sample) {
		wavemix.trk[0].sample = sample;
		wavemix.trk[0].samples = samples;
		wavemix.trk[0].flag = WMFLAG_L | WMFLAG_R;
		wavemix.trk[0].volume = (np2cfg.MOTORVOL << 12) / 100;
	}
	sample = getpcmdata((void *)fddseek1, sizeof(fddseek1), rate, &samples);
	if (sample) {
		wavemix.trk[1].sample = sample;
		wavemix.trk[1].samples = samples;
		wavemix.trk[1].flag = WMFLAG_L | WMFLAG_R;
		wavemix.trk[1].volume = (np2cfg.MOTORVOL << 12) / 100;
	}
}

void wavemix_deinitialize(void) {

	WMTRK	t;
	WMTRK	tterm;

	t = wavemix.trk;
	tterm = t + WAVEMIX_MAXTRACK;
	while(t < tterm) {
		if (t->sample) {
			_MFREE(t->sample);
		}
		t++;
	}
	ZeroMemory(&wavemix, sizeof(wavemix));
}

void wavemix_play(UINT num, BOOL loop) {

	WMTRK	t;

	if (num < WAVEMIX_MAXTRACK) {
		t = wavemix.trk + num;
		if ((t->sample) && (t->samples)) {
			sound_sync();
			t->pcm = t->sample;
			t->remain = t->samples;
			if (loop) {
				t->flag |= WMFLAG_LOOP;
			}
			else {
				t->flag &= ~WMFLAG_LOOP;
			}
			wavemix.playing |= (1 << num);
		}
	}
}

void wavemix_stop(UINT num) {

	if (num < WAVEMIX_MAXTRACK) {
		sound_sync();
		wavemix.playing &= ~(1 << num);
	}
}

void wavemix_bind(void) {

	sound_streamregist(&wavemix, (SOUNDCB)wavemix_getpcm);
}

#endif


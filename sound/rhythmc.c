#include	"compiler.h"
#include	<math.h>
#include	"wavefile.h"
#include	"dosio.h"
#include	"sound.h"
#include	"rhythm.h"


static const char file_2608bd[] = "2608_bd.wav";
static const char file_2608sd[] = "2608_sd.wav";
static const char file_2608top[] = "2608_top.wav";
static const char file_2608hh[] = "2608_hh.wav";
static const char file_2608tom[] = "2608_tom.wav";
static const char file_2608rim[] = "2608_rim.wav";

static const char *rhythmfile[RHYTHM_MAX] = {
				file_2608bd,	file_2608sd,	file_2608top,
				file_2608hh,	file_2608tom,	file_2608rim};

	RHYTHMCFG	rhythmcfg;


static BOOL pcmload(RHYTHMPCM *pcm, const char *fname, UINT rate) {

	FILEH		fh;
	RIFF_HEADER	riff;
	BOOL		head;
	WAVE_HEADER	whead;
	UINT		size;
	WAVE_INFOS	info;
	UINT		step;
	UINT		samples;
	SINT16		*ptr;
	UINT		rsize;
	UINT		pos;
	BYTE		work[256];

	fh = file_open_c(fname);
	if (fh == FILEH_INVALID) {
		goto pld_err1;
	}
	if ((file_read(fh, &riff, sizeof(riff)) != sizeof(riff)) ||
		(riff.sig != WAVE_SIG('R','I','F','F')) ||
		(riff.fmt != WAVE_SIG('W','A','V','E'))) {
		goto pld_err2;
	}

	head = FALSE;
	while(1) {
		if (file_read(fh, &whead, sizeof(whead)) != sizeof(whead)) {
			goto pld_err2;
		}
		size = LOADINTELDWORD(whead.size);
		if (whead.sig == WAVE_SIG('f','m','t',' ')) {
			if (size >= sizeof(info)) {
				if (file_read(fh, &info, sizeof(info)) != sizeof(info)) {
					goto pld_err2;
				}
				size -= sizeof(info);
				head = TRUE;
			}
		}
		else if (whead.sig == WAVE_SIG('d','a','t','a')) {
			break;
		}
		if (size) {
			file_seek(fh, size, FSEEK_CUR);
		}
	}
	if (!head) {
		goto pld_err2;
	}
	if ((LOADINTELWORD(info.channel) != 1) ||
		(LOADINTELDWORD(info.rate) != 44100) ||
		(LOADINTELWORD(info.bit) != 16)) {
		goto pld_err2;
	}
	if (rate == 0) {
		goto pld_err2;
	}
	step = 44100 / rate;
	if (step == 0) {
		goto pld_err2;
	}
	samples = size / 2 / step;
	if (samples == 0) {
		goto pld_err2;
	}
	ptr = (SINT16 *)_MALLOC(samples * sizeof(SINT16), fname);
	if (ptr == NULL) {
		goto pld_err2;
	}
	ZeroMemory(ptr, samples * sizeof(SINT16));
	pcm->data = ptr;
	pcm->samples = samples;
	pos = 0;
	rsize = 0;
	do {
		while(pos >= rsize) {
			pos -= rsize;
			rsize = file_read(fh, work, sizeof(work));
			rsize &= (~1);
			if (rsize == 0) {
				break;
			}
		}
		*ptr++ = (SINT16)LOADINTELWORD(work + pos);
		pos += step * sizeof(SINT16);
	} while(--samples);
	file_close(fh);
	return(SUCCESS);

pld_err2:
	file_close(fh);

pld_err1:
	return(FAILURE);
}

void rhythm_initialize(UINT rate) {

	UINT	i;

	ZeroMemory(&rhythmcfg, sizeof(rhythmcfg));
	for (i=0; i<6; i++) {
		pcmload(rhythmcfg.pcm + i, rhythmfile[i], rate);
	}
	for (i=0; i<96; i++) {
		rhythmcfg.voltbl[i] = (UINT)(32768.0 *
										pow(2.0, (double)i * (-3.0) / 40.0));
	}
}

void rhythm_deinitialize(void) {

	UINT	i;
	SINT16	*ptr;

	for (i=0; i<RHYTHM_MAX; i++) {
		ptr = rhythmcfg.pcm[i].data;
		rhythmcfg.pcm[i].data = NULL;
		if (ptr) {
			_MFREE(ptr);
		}
	}
}

void rhythm_setvol(UINT vol) {

	rhythmcfg.vol = vol;
}

void rhythm_reset(RHYTHM rhy) {

	ZeroMemory(rhy, sizeof(_RHYTHM));
	rhythm_update(rhy);
}

void rhythm_update(RHYTHM rhy) {

	RHYTHMCH	*r;
	RHYTHMCH	*rterm;

	r = rhy->r;
	rterm = r + RHYTHM_MAX;
	do {
		r->volume = (rhythmcfg.voltbl[rhy->vol + r->volreg] *
														rhythmcfg.vol) >> 10;
	} while(++r < rterm);
}

void rhythm_setreg(RHYTHM rhy, BYTE reg, BYTE value) {

	RHYTHMCH	*r;
	BYTE		bit;
	int			i;

	if (reg == 0x10) {
		sound_sync();
		r = rhy->r;
		for (i=0, bit=1; i<6; i++, bit<<=1) {
			if (value & bit) {
				if (value & 0x80) {
					rhy->bitmap &= ~((UINT)bit);
				}
				else {
					if (rhythmcfg.pcm[i].data) {
						r->ptr = rhythmcfg.pcm[i].data;
						r->remain = rhythmcfg.pcm[i].samples;
						rhy->bitmap |= bit;
					}
				}
			}
			r++;
		}
	}
	else if (reg == 0x11) {										// ver0.28
		sound_sync();
		rhy->vol = (~value) & 0x3f;
		rhythm_update(rhy);
	}
	else if ((reg >= 0x18) && (reg < 0x1e)) {
		sound_sync();
		r = rhy->r + (reg - 0x18);
		r->volreg = (~value) & 0x1f;
		r->volume = (rhythmcfg.voltbl[rhy->vol + r->volreg] *
														rhythmcfg.vol) >> 10;
		r->lr = value & 0xc0;
	}
}


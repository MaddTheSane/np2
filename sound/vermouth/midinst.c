#include	"compiler.h"
#include	"dosio.h"
#include	"midiout.h"


// GUS format references:  http://www.onicos.com/staff/iz/formats/guspat.html

typedef struct {
	char	sig[12];
	char	id[10];
	char	description[60];

	BYTE	instruments;
	BYTE	voice;
	BYTE	channels;
	BYTE	waveforms[2];
	BYTE	volume[2];
	BYTE	datasize[4];
	BYTE	reserved1[36];

	BYTE	instrument[2];
	BYTE	instname[16];
	BYTE	instsize[4];
	BYTE	layers;
	BYTE	reserved2[40];

	BYTE	layerdupe;
	BYTE	layer;
	BYTE	layersize[4];
	BYTE	layersamples;
	BYTE	reserved3[40];
} GUSHEAD;

typedef struct {
	BYTE	wavename[7];
	BYTE	fractions;
	BYTE	datasize[4];
	BYTE	loopstart[4];
	BYTE	loopend[4];
	BYTE	samprate[2];
	BYTE	freqlow[4];
	BYTE	freqhigh[4];
	BYTE	freqroot[4];
	BYTE	tune[2];
	BYTE	balance;
	BYTE	env[6];
	BYTE	envpos[6];
	BYTE	tre_sweep;
	BYTE	tre_rate;
	BYTE	tre_depth;
	BYTE	vib_sweep;
	BYTE	vib_rate;
	BYTE	vib_depth;
	BYTE	mode;
	BYTE	scalefreq[2];
	BYTE	scalefactor[2];
	BYTE	reserved[36];
} GUSWAVE;


static void inst_destroy(INSTRUMENT inst);


// ---- resample

#define	BASEBITS	9
#define	MIXBASE		(1 << BASEBITS)

static SAMPLE downsamp(SAMPLE dst, SAMPLE src, int count, int mrate) {

	int		rem;
	SINT32	pcm;

	rem = MIXBASE;
	pcm = 0;
	do {
		if (rem > mrate) {
			rem -= mrate;
			pcm += (*src++) * mrate;
		}
		else {
			pcm += (*src) * rem;
			pcm >>= BASEBITS;
			*dst++ = (_SAMPLE)pcm;
			pcm = mrate - rem;
			rem = MIXBASE - pcm;
			pcm *= (*src++);
		}
	} while(--count);
	if (rem != MIXBASE) {
		*dst++ = (_SAMPLE)(pcm >> BASEBITS);
	}
	return(dst);
}

static SAMPLE upsamp(SAMPLE dst, SAMPLE src, int count, int mrate) {

	int		rem;
	SINT32	tmp;
	SINT32	pcm;
	SINT32	dat;

	rem = 0;
	pcm = 0;
	do {
		tmp = MIXBASE - rem;
		if (tmp >= 0) {
			dat = (pcm * rem);
			pcm = *src++;
			dat += (pcm * tmp);
			dat >>= BASEBITS;
			*dst++ = (_SAMPLE)dat;
			rem = mrate - tmp;
			count--;
		}
		while(rem >= MIXBASE) {
			rem -= MIXBASE;
			*dst++ = (_SAMPLE)pcm;
		}
	} while(count);
	if (rem) {
		*dst++ = rem >> BASEBITS;
	}
	return(dst);
}

static void resample(MIDIMOD mod, INSTLAYER inst, int freq) {

	int		mrate;
	int		orgcnt;
	int		newcnt;
	SAMPLE	dst;
	SAMPLE	dstterm;

	mrate = (int)((float)MIXBASE *
					(float)mod->samprate / (float)inst->samprate *
					(float)inst->freqroot / (float)freq);
	if (mrate != MIXBASE) {
		orgcnt = inst->datasize >> FREQ_SHIFT;
		newcnt = (orgcnt * mrate + MIXBASE - 1) >> BASEBITS;
		dst = (SAMPLE)_MALLOC((newcnt + 1) * sizeof(_SAMPLE), "resampled");
		if (dst == NULL) {
			return;
		}
		dst[newcnt] = 0;
		if (mrate > MIXBASE) {
			dstterm = upsamp(dst, inst->data, orgcnt, mrate);
		}
		else {
			dstterm = downsamp(dst, inst->data, orgcnt, mrate);
		}
#if 0
		if ((dstterm - dst) != newcnt) {
			TRACEOUT(("resample error %d %d", newcnt, dstterm - dst));
		}
#endif
		inst->datasize = newcnt << FREQ_SHIFT;
		inst->loopstart = 0;
		inst->loopend = newcnt << FREQ_SHIFT;
		_MFREE(inst->data);
		inst->data = dst;
	}
	inst->samprate = 0;
}


// ---- load

static const char ext_pat[] = ".pat";
static const char sig_GF1PATCH100[] = "GF1PATCH100";
static const char sig_GF1PATCH110[] = "GF1PATCH110";
static const char sig_ID000002[] = "ID#000002";
static const char str_question6[] = "??????";

static INSTRUMENT inst_create(MIDIMOD mod, TONECFG cfg) {

	char		filename[MAX_PATH];
	char		path[MAX_PATH];
	FILEH		fh;
	INSTRUMENT	ret;
	GUSHEAD		head;
	int			layers;
	int			size;
	INSTLAYER	layer;
	GUSWAVE		wave;
	int			i;
	SAMPLE		dat;
	_SAMPLE		tmp;
const BYTE		*d;
	SINT16		*p;
	SINT16		*q;
	int			cnt;

	if (cfg->name == NULL) {
		goto li_err1;
	}
	file_cpyname(filename, cfg->name, sizeof(filename));
	file_cutext(filename);
	file_catname(filename, ext_pat, sizeof(filename));
	if (cfgfile_getfile(mod, filename, path, sizeof(path)) != SUCCESS) {
		goto li_err1;
	}
	fh = file_open_rb(path);
	if (fh == FILEH_INVALID) {
//		TRACEOUT(("not found: %s", path));
		goto li_err1;
	}

	// head check
	if ((file_read(fh, &head, sizeof(head)) != sizeof(head)) &&
		(memcmp(head.sig, sig_GF1PATCH100, 12)) &&
		(memcmp(head.sig, sig_GF1PATCH110, 12)) &&
		(memcmp(head.id, sig_ID000002, 10)) &&
		(head.instruments != 0) && (head.instruments != 1) &&
		(head.layers != 0) && (head.layers != 1)) {
		goto li_err2;
	}

	layers = head.layersamples;
	if (!layers) {
		goto li_err2;
	}
	size = sizeof(_INSTRUMENT) + (layers * sizeof(_INSTLAYER));
	ret = (INSTRUMENT)_MALLOC(size, "instrument");
	if (ret == NULL) {
		goto li_err2;
	}
	ZeroMemory(ret, size);
	layer = (INSTLAYER)(ret + 1);
	ret->layers = layers;
	if (cfg->note != TONECFG_VARIABLE) {
		ret->freq = freq_table[cfg->note];
	}

	do {
		BYTE fractions;

		if (file_read(fh, &wave, sizeof(wave)) != sizeof(wave)) {
			goto li_err3;
		}
		fractions = wave.fractions;
		layer->datasize = LOADINTELDWORD(wave.datasize);
		layer->loopstart = LOADINTELDWORD(wave.loopstart);
		layer->loopend = LOADINTELDWORD(wave.loopend);
		layer->samprate = LOADINTELWORD(wave.samprate);
		layer->freqlow = LOADINTELDWORD(wave.freqlow);
		layer->freqhigh = LOADINTELDWORD(wave.freqhigh);
		layer->freqroot = LOADINTELDWORD(wave.freqroot);
		if (cfg->pan == TONECFG_VARIABLE) {
			layer->panpot = ((wave.balance * 8) + 4) & 0x7f;
		}
		else {
			layer->panpot = cfg->pan & 0x7f;
		}

		if (wave.mode & MODE_LOOPING) {
			wave.mode |= MODE_SUSTAIN;
		}
		if (cfg->flag & TONECFG_NOLOOP) {
			wave.mode &= ~(MODE_SUSTAIN | MODE_LOOPING | MODE_PINGPONG |
															MODE_REVERSE);
		}
		if (cfg->flag & TONECFG_NOENV) {
			wave.mode &= ~MODE_ENVELOPE;
		}
		else if (!(cfg->flag & TONECFG_KEEPENV)) {
			if (wave.mode & (MODE_LOOPING | MODE_PINGPONG | MODE_REVERSE)) {
				if ((!(wave.mode & MODE_SUSTAIN)) ||
					(!memcmp(wave.env, str_question6, 6)) ||
					(wave.envpos[5] >= 100)) {
					wave.mode &= ~MODE_ENVELOPE;
				}
			}
			else {
				wave.mode &= ~(MODE_SUSTAIN | MODE_ENVELOPE);
			}
		}

		for (i=0; i<6; i++) {
			int sft;
			sft = ((wave.env[i] >> 6) ^ 3) * 3;
			layer->envratetbl[i] = ((((wave.env[i] & 0x3f) << sft) * 44100
									/ mod->samprate) * ENV_RATE) << (3 + 1);
			layer->envpostbl[i] = wave.envpos[i] << (7 + 9);
		}
		if ((wave.tre_rate != 0) && (wave.tre_depth != 0)) {
			layer->tremolo_step = ((ENV_RATE * wave.tre_rate)
										<< (SINENT_BIT + TRERATE_SHIFT))
										/ (TRERATE_TUNE * mod->samprate);
			if (wave.tre_sweep) {
				layer->tremolo_sweep = ((ENV_RATE * TRESWEEP_TUNE)
										<< TRESWEEP_SHIFT)
										/ (wave.tre_sweep * mod->samprate);
			}
			layer->tremolo_depth = wave.tre_depth;
		}
		if ((wave.vib_rate != 0) && (wave.vib_depth != 0)) {
			layer->vibrate_rate = (VIBRATE_TUNE * mod->samprate) /
											(wave.vib_rate << VIBRATE_SHIFT);
			if (wave.vib_sweep) {
				layer->vibrate_sweep =
						((VIBRATE_TUNE * VIBSWEEP_TUNE) << VIBSWEEP_SHIFT) /
						(wave.vib_sweep * wave.vib_rate << VIBRATE_SHIFT);
			}
			layer->vibrate_depth = wave.vib_depth;
		}

		cnt = layer->datasize;
		if (!cnt) {
			goto li_err3;
		}
		if (wave.mode & MODE_16BIT) {
			cnt >>= 1;
		}
		dat = (SAMPLE)_MALLOC((cnt + 1) * sizeof(_SAMPLE), "data");
		if (dat == NULL) {
			goto li_err3;
		}
		layer->data = dat;
		if (file_read(fh, dat, layer->datasize) != (UINT)layer->datasize) {
			goto li_err3;
		}
		dat[cnt] = 0;
		if (wave.mode & MODE_16BIT) {
			layer->datasize >>= 1;
			layer->loopstart >>= 1;
			layer->loopend >>= 1;
#if defined(BYTESEX_LITTLE)
			if (sizeof(_SAMPLE) != 2) {				// Ara!?
#endif
				d = (BYTE *)dat;
				d += layer->datasize * 2;
				q = dat + layer->datasize;
				do {
					d -= 2;
					q--;
					*q = (_SAMPLE)LOADINTELWORD(d);
				} while(q > dat);
#if defined(BYTESEX_LITTLE)
			}
#endif
		}
		else {
			d = (BYTE *)dat;
			d += layer->datasize;
			q = dat + layer->datasize;
			do {
				d--;
				q--;
				*q = (_SAMPLE)((*d) * 257);
			} while(q > dat);
		}
		if (wave.mode & MODE_UNSIGNED) {
			q = dat + layer->datasize;
			do {
				q--;
				*q -= (_SAMPLE)0x8000;
			} while(q > dat);
		}
		if (wave.mode & MODE_REVERSE) {
			p = dat;
			q = dat + layer->datasize;
			do {
				q--;
				tmp = *p;
				*p = *q;
				*q = tmp;
				p++;
			} while(p < q);
		}

		if (cfg->amp == TONECFG_AUTOAMP) {
			int	sampdat;
			int sampmax;
			q = (SAMPLE)dat;
			cnt = layer->datasize;
			sampmax = 32768 / 4;
			do {
				sampdat = *q++;
				if (sampdat < 0) {
					sampdat *= -1;
				}
				sampmax = max(sampmax, sampdat);
			} while(--cnt);
			layer->volume = 32768 * 128 / sampmax;
		}
		else {
			layer->volume = cfg->amp * 128 / 100;
		}

		layer->datasize <<= FREQ_SHIFT;
		layer->loopstart <<= FREQ_SHIFT;
		layer->loopend <<= FREQ_SHIFT;
		layer->loopstart |= (fractions & 0x0F) << (FREQ_SHIFT - 4);
		layer->loopend |= ((fractions >> 4) & 0x0F) << (FREQ_SHIFT - 4);

		if (layer->loopstart > layer->datasize) {
			layer->loopstart = layer->datasize;
		}
		if (layer->loopend > layer->datasize) {
			layer->loopend = layer->datasize;
		}
		if (wave.mode & MODE_REVERSE) {
			cnt = layer->loopstart;
			layer->loopstart = layer->datasize - layer->loopend;
			layer->loopend = layer->datasize - cnt;
			wave.mode &= ~MODE_REVERSE;
			wave.mode |= MODE_LOOPING;
		}

		if ((ret->freq) && (!(wave.mode & MODE_LOOPING))) {
			resample(mod, layer, ret->freq);
		}
		if (cfg->flag & TONECFG_NOTAIL) {
			layer->datasize = layer->loopend;
		}
		layer->mode = wave.mode;

		layer++;
	} while(--layers);

	file_close(fh);
	return(ret);

li_err3:
	inst_destroy(ret);

li_err2:
	file_close(fh);

li_err1:
	return(NULL);
}

static void inst_destroy(INSTRUMENT inst) {

	int			layers;
	INSTLAYER	layer;

	if (inst) {
		layers = inst->layers;
		layer = (INSTLAYER)(inst + 1);
		while(layers--) {
			if (layer->data) {
				_MFREE(layer->data);
			}
			layer++;
		}
		_MFREE(inst);
	}
}

int inst_singleload(MIDIMOD mod, UINT bank, UINT num) {

	INSTRUMENT	*inst;
	INSTRUMENT	tone;
	TONECFG		cfg;

	if (bank >= (MIDI_BANKS * 2)) {
		return(MIDIOUT_FAILURE);
	}
	cfg = mod->tonecfg[bank];
	if (cfg == NULL) {
		return(MIDIOUT_FAILURE);
	}
	inst = mod->tone[bank];
	num &= 0x7f;
	if ((inst == NULL) || (inst[num] == NULL)) {
		tone = inst_create(mod, cfg + num);
		if (tone == NULL) {
			return(MIDIOUT_FAILURE);
		}
		if (inst == NULL) {
			inst = (INSTRUMENT *)_MALLOC(sizeof(INSTRUMENT) * 128, "INST");
			if (inst == NULL) {
				inst_destroy(tone);
				return(MIDIOUT_FAILURE);
			}
			mod->tone[bank] = inst;
			ZeroMemory(inst, sizeof(INSTRUMENT) * 128);
		}
		inst[num] = tone;
	}
	return(MIDIOUT_SUCCESS);
}

int inst_bankload(MIDIMOD mod, UINT bank) {

	INSTRUMENT	*inst;
	INSTRUMENT	tone;
	TONECFG		cfg;
	UINT		num;

	if (bank >= (MIDI_BANKS * 2)) {
		return(MIDIOUT_FAILURE);
	}
	cfg = mod->tonecfg[bank];
	if (cfg == NULL) {
		return(MIDIOUT_FAILURE);
	}
	inst = mod->tone[bank];
	for (num = 0; num<0x80; num++) {
		if ((inst == NULL) || (inst[num] == NULL)) {
			tone = inst_create(mod, cfg + num);
			if (tone) {
//				TRACEOUT(("load %d %d", bank, num));
				if (inst == NULL) {
					inst = (INSTRUMENT *)_MALLOC(
										sizeof(INSTRUMENT) * 128, "INST");
					if (inst == NULL) {
						inst_destroy(tone);
						return(MIDIOUT_FAILURE);
					}
					mod->tone[bank] = inst;
					ZeroMemory(inst, sizeof(INSTRUMENT) * 128);
				}
				inst[num] = tone;
			}
		}
	}
	return(MIDIOUT_SUCCESS);
}

void inst_bankfree(MIDIMOD mod, UINT bank) {

	INSTRUMENT	*inst;
	INSTRUMENT	*i;

	if (bank >= (MIDI_BANKS * 2)) {
		return;
	}
	inst = mod->tone[bank];
	if (inst == NULL) {
		return;
	}
	i = inst + 128;
	do {
		i--;
		inst_destroy(*i);
	} while(i > inst);
	if (bank >= 2) {
		mod->tone[bank] = NULL;
		_MFREE(inst);
	}
	else {
		ZeroMemory(inst, sizeof(INSTRUMENT) * 128);
	}
}


#include	"compiler.h"
#include	"midiout.h"


#define	MIDIOUT_VERSION		0x103
#define	MIDIOUT_VERSTRING	"VERMOUTH 1.03"


static const char vermouthver[] = MIDIOUT_VERSTRING;

static const int gaintbl[24+1] =
				{ 16,  19,  22,  26,  32,  38,  45,  53,
				  64,  76,  90, 107, 128, 152, 181, 215,
				 256, 304, 362, 430, 512, 608, 724, 861, 1024};


// ---- voice

static void voice_volupdate(VOICE v) {

	CHANNEL	ch;
	int		vol;

	ch = v->channel;
	vol = ch->level * v->velocity;
	vol >>= 7;
	vol *= v->sample->volume;
	vol >>= (21 - 16);
	v->volleft = vol;
	if ((v->flag & VOICE_MIXMASK) == VOICE_MIXNORMAL) {
		if (v->panpot < 64) {
			vol *= (v->panpot + 1);
			vol >>= 6;
			v->volright = vol;
		}
		else {
			v->volright = vol;
			vol *= (127 - v->panpot);
			vol >>= 6;
			v->volleft = vol;
		}
	}
}

static INSTLAYER selectlayer(VOICE v, INSTRUMENT inst) {

	int			layers;
	INSTLAYER	layer;
	INSTLAYER	layerterm;
	int			freq;
	int			diffmin;
	int			diff;
	INSTLAYER	layersel;

	layers = inst->layers;
	layer = (INSTLAYER)(inst + 1);

	if (layers == 1) {
		return(layer);
	}

	layerterm = layer + layers;
	freq = v->frequency;
	do {
		if ((freq >= layer->freqlow) && (freq <= layer->freqhigh)) {
			return(layer);
		}
		layer++;
	} while(layer < layerterm);

	layer = (INSTLAYER)(inst + 1);
	layersel = layer;
	diffmin = layer->freqroot - freq;
	if (diffmin < 0) {
		diffmin *= -1;
	}
	layer++;
	do {
		diff = layer->freqroot - freq;
		if (diff < 0) {
			diff *= -1;
		}
		if (diffmin > diff) {
			diffmin = diff;
			layersel = layer;
		}
		layer++;
	} while(layer < layerterm);
	return(layersel);
}

static void freq_update(VOICE v) {

	CHANNEL	ch;
	float	step;

	if (v->flag & VOICE_FIXPITCH) {
		return;
	}

	ch = v->channel;
	step = v->freq;
	if (ch->pitchbend != 0x2000) {
		step *= ch->pitchfactor;
	}
#if defined(ENABLE_VIRLATE)
	v->freqnow = step;
#endif
	step *= (float)(1 << FREQ_SHIFT);
	if (v->sampstep < 0) {
		step *= -1.0;
	}
	v->sampstep = (int)step;
}

static void voice_on(MIDIHDL midi, CHANNEL ch, VOICE v, int key, int vel) {

	INSTRUMENT	inst;
	INSTLAYER	layer;
	int			panpot;

	key &= 0x7f;
	if (!(ch->flag & CHANNEL_RHYTHM)) {
		inst = ch->inst;
		if (inst == NULL) {
			return;
		}
		if (inst->freq) {
			v->frequency = inst->freq;
		}
		else {
			v->frequency = freq_table[key];
		}
		layer = selectlayer(v, inst);
	}
	else {
#if !defined(MIDI_GMONLY)
		inst = ch->rhythm[key];
		if (inst == NULL) {
			inst = midi->bank0[1][key];
		}
#else
		inst = midi->bank0[1][key];
#endif
		if (inst == NULL) {
			return;
		}
		layer = (INSTLAYER)(inst + 1);
		if (inst->freq) {
			v->frequency = inst->freq;
		}
		else {
			v->frequency = freq_table[key];
		}
	}
	v->sample = layer;

	v->phase = VOICE_ON;
	v->channel = ch;
	v->note = key;
	v->velocity = vel;
	v->samppos = 0;
	v->sampstep = 0;

#if defined(ENABLE_TREMOLO)
	v->tremolo.count = 0;
	v->tremolo.step = layer->tremolo_step;
	v->tremolo.sweepstep = layer->tremolo_sweep;
	v->tremolo.sweepcount = 0;
#endif

#if defined(ENABLE_VIRLATE)
	v->vibrate.sweepstep = layer->vibrate_sweep;
	v->vibrate.sweepcount = 0;
	v->vibrate.rate = layer->vibrate_rate;
	v->vibrate.count = 0;
	v->vibrate.phase = 0;
#endif

	if (!(ch->flag & CHANNEL_RHYTHM)) {
		panpot = ch->panpot;
	}
	else {
		panpot = layer->panpot;
	}
	if ((panpot >= 60) && (panpot < 68)) {
		v->flag = VOICE_MIXCENTRE;
	}
	else if (panpot < 5) {
		v->flag = VOICE_MIXLEFT;
	}
	else if (panpot >= 123) {
		v->flag = VOICE_MIXRIGHT;
	}
	else {
		v->flag = VOICE_MIXNORMAL;
		v->panpot = panpot;
	}
	if (!layer->samprate) {
		v->flag |= VOICE_FIXPITCH;
	}
	else {
		v->freq = (float)layer->samprate / (float)midi->samprate *
					(float)v->frequency / (float)layer->freqroot;
	}
	voice_setphase(v, VOICE_ON);
	freq_update(v);
	voice_volupdate(v);
	v->envcount = 0;
	if (layer->mode & MODE_ENVELOPE) {
		v->envvol = 0;
		envlope_setphase(v, 0);
	}
	else {
		v->envstep = 0;
	}
	voice_setmix(v);
	envelope_updates(v);
}

static void voice_off(VOICE v) {

	voice_setphase(v, VOICE_OFF);
	if (v->sample->mode & MODE_ENVELOPE) {
		envlope_setphase(v, 3);
		voice_setmix(v);
		envelope_updates(v);
	}
}

static void allresetvoices(MIDIHDL midi) {

	VOICE	v;
	VOICE	vterm;

	v = midi->voice;
	vterm = v + VOICE_MAX;
	do {
		voice_setfree(v);
		v++;
	} while(v < vterm);
}


// ---- key

static void key_on(MIDIHDL midi, CHANNEL ch, int key, int vel) {

	VOICE	v;
	VOICE	v1;
	VOICE	v2;
	int		vol;
	int		volmin;

	v = NULL;
	v1 = midi->voice;
	v2 = v1 + VOICE_MAX;
	do {
		v2--;
		if (v2->phase == VOICE_FREE) {
			v = v2;
		}
		else if ((v2->channel == ch) &&
				((v2->note == key) || (ch->flag & CHANNEL_MONO))) {
			voice_setphase(v2, VOICE_REL);
			voice_setmix(v2);
		}
	} while(v1 < v2);

	if (v != NULL) {
		voice_on(midi, ch, v, key, vel);
		return;
	}

	volmin = 0x7fffffff;
	v2 = v1 + VOICE_MAX;
	do {
		v2--;
		if (!(v2->phase & (VOICE_ON | VOICE_REL))) {
			vol = v2->envleft;
			if ((v2->flag & VOICE_MIXMASK) == VOICE_MIXNORMAL) {
				vol = max(vol, v2->envright);
			}
			if (volmin > vol) {
				volmin = vol;
				v = v2;
			}
		}
	} while(v1 < v2);

	if (v != NULL) {
		voice_setfree(v);
		voice_on(midi, ch, v, key, vel);
	}
}

static void key_off(MIDIHDL midi, CHANNEL ch, int key) {

	VOICE	v;
	VOICE	vterm;

	v = midi->voice;
	vterm = v + VOICE_MAX;
	do {
		if ((v->phase & VOICE_ON) &&
			(v->channel == ch) && (v->note == key)) {
			if (ch->flag & CHANNEL_SUSTAIN) {
				voice_setphase(v, VOICE_SUSTAIN);
			}
			else {
				voice_off(v);
			}
			return;
		}
		v++;
	} while(v < vterm);
}

static void key_pressure(MIDIHDL midi, CHANNEL ch, int key, int vel) {

	VOICE	v;
	VOICE	vterm;

	v = midi->voice;
	vterm = v + VOICE_MAX;
	do {
		if ((v->phase & VOICE_ON) &&
			(v->channel == ch) && (v->note == key)) {
			v->velocity = vel;
			voice_volupdate(v);
			envelope_updates(v);
			break;
		}
		v++;
	} while(v < vterm);
}


// ---- control

static void volumeupdate(MIDIHDL midi, CHANNEL ch) {

	VOICE	v;
	VOICE	vterm;

	ch->level = (midi->level * ch->volume * ch->expression) >> 14;
	v = midi->voice;
	vterm = v + VOICE_MAX;
	do {
		if ((v->phase & (VOICE_ON | VOICE_SUSTAIN)) && (v->channel == ch)) {
			voice_volupdate(v);
			envelope_updates(v);
		}
		v++;
	} while(v < vterm);
}

static void pedaloff(MIDIHDL midi, CHANNEL ch) {

	VOICE	v;
	VOICE	vterm;

	v = midi->voice;
	vterm = v + VOICE_MAX;
	do {
		if ((v->phase & VOICE_SUSTAIN) && (v->channel == ch)) {
			voice_off(v);
		}
		v++;
	} while(v < vterm);
}

static void allsoundsoff(MIDIHDL midi, CHANNEL ch) {

	VOICE	v;
	VOICE	vterm;

	v = midi->voice;
	vterm = v + VOICE_MAX;
	do {
		if ((v->phase != VOICE_FREE) && (v->channel == ch)) {
			voice_setphase(v, VOICE_REL);
			voice_setmix(v);
		}
		v++;
	} while(v < vterm);
}

static void resetallcontrollers(CHANNEL ch) {

	ch->flag &= CHANNEL_MASK;
	if (ch->flag == 9) {
		ch->flag |= CHANNEL_RHYTHM;
	}
	ch->volume = 90;
	ch->expression = 127;
	ch->pitchbend = 0x2000;
	ch->pitchfactor = 1.0;
}

static void allnotesoff(MIDIHDL midi, CHANNEL ch) {

	VOICE	v;
	VOICE	vterm;

	v = midi->voice;
	vterm = v + VOICE_MAX;
	do {
#if 1
		if ((v->phase & (VOICE_ON | VOICE_SUSTAIN)) && (v->channel == ch)) {
			voice_off(v);
		}
#else
		if ((v->phase & VOICE_ON) && (v->channel == ch)) {
			if (ch->flag & CHANNEL_SUSTAIN) {
				voice_setphase(v, VOICE_SUSTAIN);
			}
			else {
				voice_off(v);
			}
		}
#endif
		v++;
	} while(v < vterm);
}

static void ctrlchange(MIDIHDL midi, CHANNEL ch, int ctrl, int val) {

	val &= 0x7f;
	switch(ctrl & 0x7f) {
		case CTRL_PGBANK:
			ch->bank = val;
			break;

		case CTRL_DATA_M:
//			TRACEOUT(("data: %x %x %x", c->rpn_l, c->rpn_m, val));
			if ((ch->rpn_l == 0) && (ch->rpn_m == 0)) {
				if (val >= 24) {
					val = 24;
				}
				ch->pitchsens = val;
			}
			break;

		case CTRL_VOLUME:
			ch->volume = val;
			volumeupdate(midi, ch);
			break;

		case CTRL_PANPOT:
			ch->panpot = val;
			break;

		case CTRL_EXPRESS:
			ch->expression = val;
			volumeupdate(midi, ch);
			break;

		case CTRL_PEDAL:
			if (val == 0) {
				ch->flag &= ~CHANNEL_SUSTAIN;
				pedaloff(midi, ch);
			}
			else {
				ch->flag |= CHANNEL_SUSTAIN;
			}
			break;

		case CTRL_RPN_L:
			ch->rpn_l = val;
			break;

		case CTRL_RPN_M:
			ch->rpn_m = val;
			break;

		case CTRL_SOUNDOFF:
			allsoundsoff(midi, ch);
			break;

		case CTRL_RESETCTRL:
			resetallcontrollers(ch);
			break;

		case CTRL_NOTEOFF:
			allnotesoff(midi, ch);
			break;

		case CTRL_MONOON:
			ch->flag |= CHANNEL_MONO;
			break;

		case CTRL_POLYON:
			ch->flag &= ~CHANNEL_MONO;
			break;

		default:
//			TRACEOUT(("ctrl: %x %x", ctrl, val);
			break;
	}
}

static void progchange(MIDIHDL midi, CHANNEL ch, int val) {

#if !defined(MIDI_GMONLY)
	MIDIMOD		module;
	INSTRUMENT	*bank;
	INSTRUMENT	inst;

	module = midi->module;
	inst = NULL;
	if (ch->bank < MIDI_BANKS) {
		bank = module->tone[ch->bank * 2];
		if (bank) {
			inst = bank[val];
		}
	}
	if (inst == NULL) {
		bank = midi->bank0[0];
		inst = bank[val];
	}
	ch->inst = inst;

	bank = NULL;
	if (ch->bank < MIDI_BANKS) {
		bank = module->tone[ch->bank * 2 + 1];
	}
	if (bank == NULL) {
		bank = midi->bank0[1];
	}
	ch->rhythm = bank;
#else
	ch->inst = midi->bank0[0][val];
#endif
	ch->program = val;
}

static void chpressure(MIDIHDL midi, CHANNEL ch, int vel) {

	VOICE	v;
	VOICE	vterm;

	v = midi->voice;
	vterm = v + VOICE_MAX;
	do {
		if ((v->phase & VOICE_ON) && (v->channel == ch)) {
			v->velocity = vel;
			voice_volupdate(v);
			envelope_updates(v);
			break;
		}
		v++;
	} while(v < vterm);
}

static void pitchbendor(MIDIHDL midi, CHANNEL ch, int val1, int val2) {

	VOICE	v;
	VOICE	vterm;

	val1 &= 0x7f;
	val1 += (val2 & 0x7f) << 7;
	if (1) {
		ch->pitchbend = val1;
		val1 -= 0x2000;
		if (!val1) {
			ch->pitchfactor = 1.0;
		}
		else {
			val1 *= ch->pitchsens;
			ch->pitchfactor = bendhtbl[(val1 >> (6 + 7)) + 24] *
												bendltbl[(val1 >> 7) & 0x3f];
		}
		v = midi->voice;
		vterm = v + VOICE_MAX;
		do {
			if ((v->phase != VOICE_FREE) && (v->channel == ch)) {
				freq_update(v);
			}
			v++;
		} while(v < vterm);
	}
}

static void allvolupdate(MIDIHDL midi) {

	int		level;
	CHANNEL	ch;
	CHANNEL	chterm;
	VOICE	v;
	VOICE	vterm;

	level = gaintbl[midi->gain + 16] >> 1;
	level *= midi->master;
	midi->level = level;
	ch = midi->channel;
	chterm = ch + 16;
	do {
		ch->level = (level * ch->volume * ch->expression) >> 14;
		ch++;
	} while(ch < chterm);
	v = midi->voice;
	vterm = v + VOICE_MAX;
	do {
		if (v->phase & (VOICE_ON | VOICE_SUSTAIN)) {
			voice_volupdate(v);
			envelope_updates(v);
		}
		v++;
	} while(v < vterm);
}

static void allresetmidi(MIDIHDL midi) {

	CHANNEL	ch;
	CHANNEL	chterm;
	UINT	flag;

	ch = midi->channel;
	chterm = ch + 16;
	ZeroMemory(ch, sizeof(_CHANNEL) * 16);
	flag = 0;
	do {
		ch->flag = flag++;
		ch->panpot = 64;
		ch->pitchsens = 2;
		ch->bank = 0;
		progchange(midi, ch, 0);
		resetallcontrollers(ch);
		ch++;
	} while(ch < chterm);
	allresetvoices(midi);
	allvolupdate(midi);
}


// ----

UINT midiout_getver(char *string, int leng) {

	milstr_ncpy(string, vermouthver, leng);
	return((MIDIOUT_VERSION << 8) | 0x00);
}

MIDIHDL midiout_create(MIDIMOD module, UINT worksize) {

	UINT	size;
	MIDIHDL	ret;

	if (module == NULL) {
		return(NULL);
	}
	worksize = min(worksize, 512);
	worksize = max(worksize, 16384);
	size = sizeof(_MIDIHDL);
	size += sizeof(SINT32) * 2 * worksize;
	size += sizeof(_SAMPLE) * worksize;
	ret = (MIDIHDL)_MALLOC(size, "MIDIHDL");
	if (ret) {
		ZeroMemory(ret, size);
		ret->samprate = module->samprate;
		ret->worksize = worksize;
		ret->module = module;
		ret->master = 127;
		ret->bank0[0] = module->tone[0];
		ret->bank0[1] = module->tone[1];
		ret->sampbuf = (SINT32 *)(ret + 1);
		ret->resampbuf = (SAMPLE)(ret->sampbuf + worksize * 2);
		allresetmidi(ret);
	}
	return(ret);
}

void midiout_destroy(MIDIHDL hdl) {

	if (hdl) {
		_MFREE(hdl);
	}
}

void midiout_shortmsg(MIDIHDL hdl, UINT32 msg) {

	UINT	cmd;
	CHANNEL	ch;

	if (hdl == NULL) {
		return;
	}
	cmd = msg & 0xff;
	if (cmd & 0x80) {
		hdl->status = cmd;
	}
	else {
		msg <<= 8;
		msg += hdl->status;
	}
	ch = hdl->channel + (cmd & 0x0f);
	switch((cmd >> 4) & 7) {
		case (MIDI_NOTE_OFF >> 4) & 7:
			key_off(hdl, ch, (msg >> 8) & 0x7f);
			break;

		case (MIDI_NOTE_ON >> 4) & 7:
			if (msg & (0x7f << 16)) {
				key_on(hdl, ch, (msg >> 8) & 0x7f, (msg >> 16) & 0x7f);
			}
			else {
				key_off(hdl, ch, (msg >> 8) & 0x7f);
			}
			break;

		case (MIDI_KEYPRESS >> 4) & 7:
			key_pressure(hdl, ch, (msg >> 8) & 0x7f, (msg >> 16) & 0x7f);
			break;

		case (MIDI_CTRLCHANGE >> 4) & 7:
			ctrlchange(hdl, ch, (msg >> 8) & 0x7f, (msg >> 16) & 0x7f);
			break;

		case (MIDI_PROGCHANGE >> 4) & 7:
			progchange(hdl, ch, (msg >> 8) & 0x7f);
			break;

		case (MIDI_CHPRESS >> 4) & 7:
			chpressure(hdl, ch, (msg >> 8) & 0x7f);
			break;

		case (MIDI_PITCHBEND >> 4) & 7:
			pitchbendor(hdl, ch, (msg >> 8) & 0x7f, (msg >> 16) & 0x7f);
			break;
	}
}

void midiout_longmsg(MIDIHDL hdl, const BYTE *msg, UINT size) {

	if ((hdl == NULL) || (msg == NULL)) {
		return;
	}
	if ((size > 5) && (msg[1] == 0x7e)) {			// GM
		if ((msg[2] == 0x7f) && (msg[3] == 0x09)) {
			allresetmidi(hdl);						// GM reset
		}
	}
}

const SINT32 *midiout_get(MIDIHDL hdl, UINT *samples) {

	UINT	size;
	VOICE	v;
	VOICE	vterm;
	BOOL	playing;
	SINT32	*buf;
	SAMPLE	src;
	SAMPLE	srcterm;
	UINT	cnt;
	UINT	pos;
	UINT	rem;

	if ((hdl == NULL) || (samples == NULL)) {
		goto moget_err;
	}
	size = min(*samples, hdl->worksize);
	if (size == 0) {
		goto moget_err;
	}
	buf = hdl->sampbuf;
	ZeroMemory(buf, size * 2 * sizeof(SINT32));
	v = hdl->voice;
	vterm = v + VOICE_MAX;
	playing = FALSE;
	do {
		if (v->phase != VOICE_FREE) {
			cnt = size;
			if (v->phase & VOICE_REL) {
				voice_setfree(v);
				if (cnt > REL_COUNT) {
					cnt = REL_COUNT;
				}
			}
			if (v->flag & VOICE_FIXPITCH) {
				pos = v->samppos >> FREQ_SHIFT;
				src = v->sample->data + pos;
				rem = (v->sample->datasize >> FREQ_SHIFT) - pos;
				if (cnt < rem) {
					v->samppos += cnt << FREQ_SHIFT;
					srcterm = src + cnt;
				}
				else {
					voice_setfree(v);
					srcterm = src + rem;
				}
			}
			else {
				src = hdl->resampbuf;
				srcterm = v->resamp(v, src, src + cnt);
			}
			if (src != srcterm) {
				v->mix(v, buf, src, srcterm);
			}
			playing = TRUE;
		}
		v++;
	} while(v < vterm);

	if (playing) {
		*samples = size;
		pos = 0;
		do {
			buf[pos*2+0] >>= (SAMP_SHIFT + 1);
			buf[pos*2+1] >>= (SAMP_SHIFT + 1);
		} while(++pos < size);
		return(buf);
	}

moget_err:
	return(NULL);
}

UINT midiout_get32(MIDIHDL hdl, SINT32 *pcm, UINT size) {

	UINT	step;
	VOICE	v;
	VOICE	vterm;
	SINT32	*buf;
	SAMPLE	src;
	SAMPLE	srcterm;
	UINT	cnt;
	UINT	pos;
	UINT	rem;

	if ((hdl != NULL) && (size)) {
		do {
			step = min(size, hdl->worksize);
			size -= step;
			buf = hdl->sampbuf;
			ZeroMemory(buf, step * 2 * sizeof(SINT32));
			v = hdl->voice;
			vterm = v + VOICE_MAX;
			do {
				if (v->phase != VOICE_FREE) {
					cnt = step;
					if (v->phase & VOICE_REL) {
						voice_setfree(v);
						if (cnt > REL_COUNT) {
							cnt = REL_COUNT;
						}
					}
					if (v->flag & VOICE_FIXPITCH) {
						pos = v->samppos >> FREQ_SHIFT;
						src = v->sample->data + pos;
						rem = (v->sample->datasize >> FREQ_SHIFT) - pos;
						if (cnt < rem) {
							v->samppos += cnt << FREQ_SHIFT;
							srcterm = src + cnt;
						}
						else {
							voice_setfree(v);
							srcterm = src + rem;
						}
					}
					else {
						src = hdl->resampbuf;
						srcterm = v->resamp(v, src, src + cnt);
					}
					if (src != srcterm) {
						v->mix(v, buf, src, srcterm);
					}
				}
				v++;
			} while(v < vterm);
			do {
				pcm[0] += buf[0] >> (SAMP_SHIFT + 1);
				pcm[1] += buf[1] >> (SAMP_SHIFT + 1);
				buf += 2;
				pcm += 2;
			} while(--step);
		} while(size);
	}
	return(0);
}

void midiout_setgain(MIDIHDL hdl, int gain) {

	if (hdl) {
		if (gain < -16) {
			gain = 16;
		}
		else if (gain > 8) {
			gain = 8;
		}
		hdl->gain = (SINT8)gain;
		allvolupdate(hdl);
	}
}


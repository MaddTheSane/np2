#include	"compiler.h"
#include	"np2.h"
#include	"commng.h"
#include	"cmmidi.h"

#if defined(VERMOUTH_LIB)

#include	"keydisp.h"
#include	"sound.h"
#include	"vermouth.h"

extern	MIDIMOD	vermouth_module;

#define MIDIOUTS(a, b, c)	(((c) << 16) + (b << 8) + (a))
#define MIDIOUTS2(a)		((a)[0] + ((a)[1] << 8))
#define MIDIOUTS3(a)		((a)[0] + ((a)[1] << 8) + ((a)[2] << 16))

static const BYTE EXCV_GMRESET[] = {
			0xf0, 0x7e, 0x7f, 0x09, 0x01, 0xf7};

enum {
	MIDI_EXCLUSIVE		= 0xf0,
	MIDI_TIMECODE		= 0xf1,
	MIDI_SONGPOS		= 0xf2,
	MIDI_SONGSELECT		= 0xf3,
	MIDI_TUNEREQUEST	= 0xf6,
	MIDI_EOX			= 0xf7,
	MIDI_TIMING			= 0xf8,
	MIDI_START			= 0xfa,
	MIDI_CONTINUE		= 0xfb,
	MIDI_STOP			= 0xfc,
	MIDI_ACTIVESENSE	= 0xfe,
	MIDI_SYSTEMRESET	= 0xff
};

enum {
	MIDI_BUFFER			= (1 << 10),
	MIDIIN_MAX			= 4,

	MIDICTRL_READY		= 0,
	MIDICTRL_2BYTES,
	MIDICTRL_3BYTES,
	MIDICTRL_EXCLUSIVE,
	MIDICTRL_TIMECODE,
	MIDICTRL_SYSTEM
};

typedef struct {
	MIDIHDL		vermouth;
	UINT		midictrl;
	UINT		midisyscnt;
	UINT		mpos;
	BYTE		midilast;
	BYTE		buffer[MIDI_BUFFER];
} _CMMIDI, *CMMIDI;


static void SOUNDCALL vermouth_getpcm(MIDIHDL hdl, SINT32 *pcm, UINT count) {

const SINT32	*ptr;
	UINT		size;

	while(count) {
		size = count;
		ptr = midiout_get(hdl, &size);
		if (ptr == NULL) {
			break;
		}
		count -= size;
		do {
			pcm[0] += ptr[0];
			pcm[1] += ptr[1];
			ptr += 2;
			pcm += 2;
		} while(--size);
	}
}

static void midireset(CMMIDI midi) {

	BYTE	work[4];

	midiout_longmsg(midi->vermouth, EXCV_GMRESET, sizeof(EXCV_GMRESET));

	work[1] = 0x7b;
	work[2] = 0x00;
	for (work[0]=0xb0; work[0]<0xc0; work[0]++) {
		keydisp_midi(work);
		sound_sync();
		midiout_shortmsg(midi->vermouth, MIDIOUTS3(work));
	}
}


// ----

static UINT	midiread(COMMNG self, BYTE *data) {

	(void)self;
	(void)data;
	return(0);
}

static UINT	midiwrite(COMMNG self, BYTE data) {

	CMMIDI	midi;

	midi = (CMMIDI)(self + 1);
	switch(data) {
		case MIDI_TIMING:
		case MIDI_START:
		case MIDI_CONTINUE:
		case MIDI_STOP:
		case MIDI_ACTIVESENSE:
		case MIDI_SYSTEMRESET:
			return(1);
	}
	if (midi->midictrl == MIDICTRL_READY) {
		if (data & 0x80) {
			midi->mpos = 0;
			switch(data & 0xf0) {
				case 0xc0:
				case 0xd0:
					midi->midictrl = MIDICTRL_2BYTES;
					break;

				case 0x80:
				case 0x90:
				case 0xa0:
				case 0xb0:
				case 0xe0:
					midi->midictrl = MIDICTRL_3BYTES;
					midi->midilast = data;
					break;

				default:
					switch(data) {
						case MIDI_EXCLUSIVE:
							midi->midictrl = MIDICTRL_EXCLUSIVE;
							break;

						case MIDI_TIMECODE:
							midi->midictrl = MIDICTRL_TIMECODE;
							break;

						case MIDI_SONGPOS:
							midi->midictrl = MIDICTRL_SYSTEM;
							midi->midisyscnt = 3;
							break;

						case MIDI_SONGSELECT:
							midi->midictrl = MIDICTRL_SYSTEM;
							midi->midisyscnt = 2;
							break;

						case MIDI_TUNEREQUEST:
							midi->midictrl = MIDICTRL_SYSTEM;
							midi->midisyscnt = 1;
							break;

//						case MIDI_EOX:
						default:
							return(1);
					}
					break;
			}
		}
		else {						// Key-onのみな気がしたんだけど忘れた…
			// running status
			midi->buffer[0] = midi->midilast;
			midi->mpos = 1;
			midi->midictrl = MIDICTRL_3BYTES;
		}
	}
	midi->buffer[midi->mpos] = data;
	midi->mpos++;

	switch(midi->midictrl) {
		case MIDICTRL_2BYTES:
			if (midi->mpos >= 2) {
				midi->buffer[1] &= 0x7f;
				keydisp_midi(midi->buffer);
				sound_sync();
				midiout_shortmsg(midi->vermouth, MIDIOUTS2(midi->buffer));
				midi->midictrl = MIDICTRL_READY;
				return(2);
			}
			break;

		case MIDICTRL_3BYTES:
			if (midi->mpos >= 3) {
				midi->buffer[1] &= 0x7f;
				midi->buffer[2] &= 0x7f;
				keydisp_midi(midi->buffer);
				sound_sync();
				midiout_shortmsg(midi->vermouth, MIDIOUTS3(midi->buffer));
				midi->midictrl = MIDICTRL_READY;
				return(3);
			}
			break;

		case MIDICTRL_EXCLUSIVE:
			if (data == MIDI_EOX) {
				midiout_longmsg(midi->vermouth, midi->buffer, midi->mpos);
				midi->midictrl = MIDICTRL_READY;
				return(midi->mpos);
			}
			else if (midi->mpos >= MIDI_BUFFER) {		// おーばーふろー
				midi->midictrl = MIDICTRL_READY;
			}
			break;

		case MIDICTRL_TIMECODE:
			if (midi->mpos >= 2) {
				if ((data == 0x7e) || (data == 0x7f)) {
					// exclusiveと同じでいい筈…
					midi->midictrl = MIDICTRL_EXCLUSIVE;
				}
				else {
					midi->midictrl = MIDICTRL_READY;
					return(2);
				}
			}
			break;

		case MIDICTRL_SYSTEM:
			if (midi->mpos >= midi->midisyscnt) {
				midi->midictrl = MIDICTRL_READY;
				return(midi->midisyscnt);
			}
			break;
	}
	return(0);
}

static BYTE	midigetstat(COMMNG self) {

	return(0x00);
}

static UINT	midimsg(COMMNG self, UINT msg, long param) {

	CMMIDI	midi;

	midi = (CMMIDI)(self + 1);
	switch(msg) {
		case COMMSG_MIDIRESET:
			midireset(midi);
			return(1);
	}
	return(0);
}

static void midirelease(COMMNG self) {

	CMMIDI	midi;

	midi = (CMMIDI)(self + 1);
	midiout_destroy(midi->vermouth);
	_MFREE(self);
}


// ----

COMMNG cmmidi_create(void) {

	MIDIHDL		vermouth;
	COMMNG		ret;
	CMMIDI		midi;

	vermouth = midiout_create(vermouth_module, 512);
	if (vermouth == NULL) {
		goto cmcre_err1;
	}

	ret = (COMMNG)_MALLOC(sizeof(_COMMNG) + sizeof(_CMMIDI), "MIDI");
	if (ret == NULL) {
		goto cmcre_err2;
	}
	ret->connect = COMCONNECT_MIDI;
	ret->read = midiread;
	ret->write = midiwrite;
	ret->getstat = midigetstat;
	ret->msg = midimsg;
	ret->release = midirelease;
	midi = (CMMIDI)(ret + 1);
	ZeroMemory(midi, sizeof(_CMMIDI));
	midi->vermouth = vermouth;
	sound_streamregist((void *)vermouth, (SOUNDCB)vermouth_getpcm);
	midi->midictrl = MIDICTRL_READY;
//	midi->midisyscnt = 0;
//	midi->mpos = 0;
	midi->midilast = 0x80;
	return(ret);

cmcre_err2:
	midiout_destroy(vermouth);

cmcre_err1:
	return(NULL);
}

#else

COMMNG cmmidi_create(void) {

	return(NULL);
}

#endif


#include	"compiler.h"
#include	"np2.h"
#include	"mimpidef.h"
#include	"commng.h"
#if defined(VERMOUTH_LIB)
#include	"sound.h"
#include	"vermouth.h"
#endif
#include	"keydisp.h"


#if defined(VERMOUTH_LIB)
extern	MIDIMOD	vermouth_module;
#endif


#define MIDIOUTS(a, b, c)	(((c) << 16) + (b << 8) + (a))
#define MIDIOUTS2(a)		(*(UINT16 *)(a))
#define MIDIOUTS3(a)		((*(UINT32 *)(a)) & 0xffffff)


const char cmmidi_midimapper[] = "MIDI MAPPER";
#if defined(VERMOUTH_LIB)
const char cmmidi_vermouth[] = "VERMOUTH";
#endif
const char *cmmidi_mdlname[12] = {
			"MT-32",		"CM-32L",		"CM-64",
			"CM-300",		"CM-500(LA)",	"CM-500(GS)",
			"SC-55",		"SC-88",		"LA",
			"GM",			"GS",			"XG"};

enum {		MIDI_MT32 = 0,	MIDI_CM32L,		MIDI_CM64,
			MIDI_CM300,		MIDI_CM500LA,	MIDI_CM500GS,
			MIDI_SC55,		MIDI_SC88,		MIDI_LA,
			MIDI_GM,		MIDI_GS,		MIDI_XG,	MIDI_OTHER};

static const BYTE EXCV_MTRESET[] = {
			0xfe, 0xfe, 0xfe};
static const BYTE EXCV_GMRESET[] = {
			0xf0, 0x7e, 0x7f, 0x09, 0x01, 0xf7};
static const BYTE EXCV_GSRESET[] = {
			0xf0, 0x41, 0x10, 0x42, 0x12, 0x40, 0x00, 0x7f, 0x00, 0x41, 0xf7};
static const BYTE EXCV_XGRESET[] = {
			0xf0, 0x43, 0x10, 0x4C, 0x00, 0x00, 0x7E, 0x00, 0xf7};
static const BYTE EXCV_GMVOLUME[] = {
			0xf0, 0x7F, 0x7F, 0x04, 0x01, 0x00, 0x00, 0xF7};

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

	CMMIDI_MIDIOUT		= 0x01,
	CMMIDI_MIDIIN		= 0x02,
	CMMIDI_MIDIINSTART	= 0x04,
#if defined(VERMOUTH_LIB)
	CMMIDI_VERMOUTH		= 0x08,
#endif

	MIDICTRL_READY		= 0,
	MIDICTRL_2BYTES,
	MIDICTRL_3BYTES,
	MIDICTRL_EXCLUSIVE,
	MIDICTRL_TIMECODE,
	MIDICTRL_SYSTEM
};

struct _cmmidi;
typedef struct _cmmidi	_CMMIDI;
typedef struct _cmmidi	*CMMIDI;

typedef struct {
	BYTE	prog;
	BYTE	press;
	UINT16	bend;
	BYTE	ctrl[28];
} _MIDICH, *MIDICH;

struct _cmmidi {
	UINT		opened;
	void		(*outfn)(CMMIDI self, UINT32 msg);
	HMIDIOUT	hmidiout;
	MIDIHDR		hmidiouthdr;
#if defined(VERMOUTH_LIB)
	MIDIHDL		vermouth;
#endif
	HMIDIIN		hmidiin;
	MIDIHDR		hmidiinhdr;
	UINT		midictrl;
	UINT		midisyscnt;
	UINT		mpos;

	BYTE		midilast;
	BYTE		midiexcvwait;
	BYTE		midimodule;

	BYTE		buffer[MIDI_BUFFER];
	_MIDICH		mch[16];
	BYTE		excvbuf[MIDI_BUFFER];

	BYTE		def_en;
	MIMPIDEF	def;

	UINT		recvpos;
	UINT		recvsize;
	BYTE		recvbuf[MIDI_BUFFER];
	BYTE		midiinbuf[MIDI_BUFFER];
};

typedef struct {
	HMIDIIN		hmidiin;
	CMMIDI		midi;
} MIDIINHDL;

static	UINT		midiinhdls;
static	MIDIINHDL	midiinhdl[MIDIIN_MAX];

static const BYTE midictrltbl[] = {	0, 1, 5, 7, 10, 11, 64,
									65, 66, 67, 84, 91, 93,
									94,						// for SC-88
									71, 72, 73, 74};		// for XG

static	BYTE	midictrlindex[128];


// ----

static BOOL getmidioutid(const char *midiout, UINT *ret) {

	UINT		num;
	UINT		i;
	MIDIOUTCAPS	moc;

	num = midiOutGetNumDevs();
	for (i=0; i<num; i++) {
		if ((midiOutGetDevCaps(i, &moc, sizeof(moc)) == MMSYSERR_NOERROR) &&
			(!milstr_cmp(midiout, moc.szPname))) {
			*ret = i;
			return(SUCCESS);
		}
	}
	if (!milstr_cmp(midiout, cmmidi_midimapper)) {
		*ret = MIDI_MAPPER;
		return(SUCCESS);
	}
	return(FAILURE);
}

static BOOL getmidiinid(const char *midiin, UINT *ret) {

	UINT		num;
	UINT		i;
	MIDIINCAPS	moc;

	num = midiInGetNumDevs();
	for (i=0; i<num; i++) {
		if ((midiInGetDevCaps(i, &moc, sizeof(moc)) == MMSYSERR_NOERROR) &&
			(!milstr_cmp(midiin, moc.szPname))) {
			*ret = i;
			return(SUCCESS);
		}
	}
	if (!milstr_cmp(midiin, cmmidi_midimapper)) {
		*ret = MIDI_MAPPER;
		return(SUCCESS);
	}
	return(FAILURE);
}

static UINT module2number(const char *module) {

	UINT	i;

	for (i=0; i<(sizeof(cmmidi_mdlname)/sizeof(char *)); i++) {
		if (milstr_extendcmp(module, cmmidi_mdlname[i])) {
			break;
		}
	}
	return(i);
}

static void waitlastexclusiveout(CMMIDI midi) {

	if (midi->midiexcvwait) {
		midi->midiexcvwait = 0;
		while(midiOutUnprepareHeader(midi->hmidiout, &midi->hmidiouthdr,
									sizeof(MIDIHDR)) == MIDIERR_STILLPLAYING);
	}
}

static void sendexclusive(CMMIDI midi, const BYTE *buf, UINT leng) {

	CopyMemory(midi->excvbuf, buf, leng);
	midi->hmidiouthdr.lpData = (char *)midi->excvbuf;
	midi->hmidiouthdr.dwFlags = 0;
	midi->hmidiouthdr.dwBufferLength = leng;
	midiOutPrepareHeader(midi->hmidiout, &midi->hmidiouthdr, sizeof(MIDIHDR));
	midiOutLongMsg(midi->hmidiout, &midi->hmidiouthdr, sizeof(MIDIHDR));
	midi->midiexcvwait = 1;
}

static void midiout_none(CMMIDI midi, UINT32 msg) {

	(void)midi;
	(void)msg;
}

static void midiout_win32(CMMIDI midi, UINT32 msg) {

	waitlastexclusiveout(midi);
	midiOutShortMsg(midi->hmidiout, (DWORD)msg);
}

#if defined(VERMOUTH_LIB)
static void midiout_vermouth(CMMIDI midi, UINT32 msg) {

	sound_sync();
	midiout_shortmsg(midi->vermouth, msg);
}

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
#endif

static void midireset(CMMIDI midi) {

const BYTE	*excv;
	UINT	excvsize;
	BYTE	work[4];

	switch(midi->midimodule) {
		case MIDI_GM:
			excv = EXCV_GMRESET;
			excvsize = sizeof(EXCV_GMRESET);
			break;

		case MIDI_CM300:
		case MIDI_CM500GS:
		case MIDI_SC55:
		case MIDI_SC88:
		case MIDI_GS:
			excv = EXCV_GSRESET;
			excvsize = sizeof(EXCV_GSRESET);
			break;

		case MIDI_XG:
			excv = EXCV_XGRESET;
			excvsize = sizeof(EXCV_XGRESET);
			break;

		case MIDI_MT32:
		case MIDI_CM32L:
		case MIDI_CM64:
		case MIDI_CM500LA:
		case MIDI_LA:
			excv = EXCV_MTRESET;
			excvsize = sizeof(EXCV_MTRESET);
			break;

		default:
			excv = NULL;
			excvsize = 0;
			break;
	}
	if (excv) {
		if (midi->opened & CMMIDI_MIDIOUT) {
			waitlastexclusiveout(midi);
			sendexclusive(midi, excv, excvsize);
		}
#if defined(VERMOUTH_LIB)
		else if (midi->opened & CMMIDI_VERMOUTH) {
			midiout_longmsg(midi->vermouth, excv, excvsize);
		}
#endif
	}

	work[1] = 0x7b;
	work[2] = 0x00;
	for (work[0]=0xb0; work[0]<0xc0; work[0]++) {
		keydisp_midi(work);
		midi->outfn(midi, MIDIOUTS3(work));
	}
}

static void midisetparam(CMMIDI midi) {

	BYTE	i;
	UINT	j;
	MIDICH	mch;

	mch = midi->mch;
	for (i=0; i<16; i++, mch++) {
		if (mch->press != 0xff) {
			midi->outfn(midi, MIDIOUTS(0xa0+i, mch->press, 0));
		}
		if (mch->bend != 0xffff) {
			midi->outfn(midi, (mch->bend << 8) + 0xe0+i);
		}
		for (j=0; j<sizeof(midictrltbl)/sizeof(BYTE); j++) {
			if (mch->ctrl[j+1] != 0xff) {
				midi->outfn(midi,
							MIDIOUTS(0xb0+i, midictrltbl[j], mch->ctrl[j+1]));
			}
		}
		if (mch->prog != 0xff) {
			midi->outfn(midi, MIDIOUTS(0xc0+i, mch->prog, 0));
		}
	}
}


// ----

static UINT midiread(COMMNG self, BYTE *data) {

	CMMIDI	midi;

	midi = (CMMIDI)(self + 1);
	if (midi->recvsize) {
		midi->recvsize--;
		*data = midi->recvbuf[midi->recvpos];
		midi->recvpos = (midi->recvpos + 1) & (MIDI_BUFFER - 1);
		return(1);
	}
	return(0);
}

static UINT midiwrite(COMMNG self, BYTE data) {

	CMMIDI	midi;
	MIDICH	mch;
	int		type;

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
				mch = midi->mch + (midi->buffer[0] & 0xf);
				switch(midi->buffer[0] & 0xf0) {
					case 0xa0:
						mch->press = midi->buffer[1];
						break;

					case 0xc0:
						if (midi->def_en) {
							type = midi->def.ch[midi->buffer[0] & 0x0f];
							if (type < MIMPI_RHYTHM) {
								midi->buffer[1] = 
										midi->def.map[type][midi->buffer[1]];
							}
						}
						mch->prog = midi->buffer[1];
						break;
				}
				keydisp_midi(midi->buffer);
				midi->outfn(midi, MIDIOUTS2(midi->buffer));
				midi->midictrl = MIDICTRL_READY;
				return(2);
			}
			break;

		case MIDICTRL_3BYTES:
			if (midi->mpos >= 3) {
				*(UINT16 *)(midi->buffer + 1) &= 0x7f7f;
				mch = midi->mch + (midi->buffer[0] & 0xf);
				switch(midi->buffer[0] & 0xf0) {
					case 0xb0:
						if (midi->buffer[1] == 123) {
							mch->press = 0;
							mch->bend = 0x4000;
							mch->ctrl[1+1] = 0;			// Modulation
							mch->ctrl[5+1] = 127;		// Explession
							mch->ctrl[6+1] = 0;			// Hold
							mch->ctrl[7+1] = 0;			// Portament
							mch->ctrl[8+1] = 0;			// Sostenute
							mch->ctrl[9+1] = 0;			// Soft
						}
						else {
							mch->ctrl[midictrlindex[midi->buffer[1]]]
															= midi->buffer[2];
						}
						break;

					case 0xe0:
						mch->bend = *(UINT16 *)(midi->buffer + 1);
						break;
				}
				keydisp_midi(midi->buffer);
				midi->outfn(midi, MIDIOUTS3(midi->buffer));
				midi->midictrl = MIDICTRL_READY;
				return(3);
			}
			break;

		case MIDICTRL_EXCLUSIVE:
			if (data == MIDI_EOX) {
				if (midi->opened & CMMIDI_MIDIOUT) {
					waitlastexclusiveout(midi);
					sendexclusive(midi, midi->buffer, midi->mpos);
				}
#if defined(VERMOUTH_LIB)
				else if (midi->opened & CMMIDI_VERMOUTH) {
					midiout_longmsg(midi->vermouth, midi->buffer, midi->mpos);
				}
#endif
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

static BYTE midigetstat(COMMNG self) {

	return(0x00);
}

static long midimsg(COMMNG self, UINT msg, long param) {

	CMMIDI	midi;
	COMFLAG	flag;

	midi = (CMMIDI)(self + 1);
	switch(msg) {
		case COMMSG_MIDIRESET:
			midireset(midi);
			return(1);

		case COMMSG_SETFLAG:
			flag = (COMFLAG)param;
			if ((flag) &&
				(flag->size == sizeof(_COMFLAG) + sizeof(midi->mch)) &&
				(flag->sig == COMSIG_MIDI)) {
				CopyMemory(midi->mch, flag + 1, sizeof(midi->mch));
				midisetparam(midi);
				return(1);
			}
			break;

		case COMMSG_GETFLAG:
			flag = (COMFLAG)_MALLOC(sizeof(_COMFLAG) + sizeof(midi->mch),
																"MIDI FLAG");
			if (flag) {
				flag->size = sizeof(_COMFLAG) + sizeof(midi->mch);
				flag->sig = COMSIG_MIDI;
				flag->ver = 0;
				flag->param = 0;
				CopyMemory(flag + 1, midi->mch, sizeof(midi->mch));
				return((long)flag);
			}
			break;

		case COMMSG_MIMPIDEFFILE:
			mimpidef_load(&midi->def, (char *)param);
			return(1);

		case COMMSG_MIMPIDEFEN:
			midi->def_en = (param)?TRUE:FALSE;
			return(1);
	}
	return(0);
}

static BOOL midiinhdlreg(CMMIDI midi, HMIDIIN hmidiin) {

	if (midiinhdls < MIDIIN_MAX) {
		midiinhdl[midiinhdls].hmidiin = hmidiin;
		midiinhdl[midiinhdls].midi = midi;
		midiinhdls++;
		return(SUCCESS);
	}
	else {
		return(FAILURE);
	}
}

static void midiinhdlunreg(CMMIDI midi) {

	UINT	i;

	for (i=0; i<midiinhdls; i++) {
		if (midiinhdl[i].midi == midi) {
			break;
		}
	}
	if (i < midiinhdls) {
		midiinhdls--;
		while(i < midiinhdls) {
			midiinhdl[i].hmidiin = midiinhdl[i+1].hmidiin;
			midiinhdl[i].midi = midiinhdl[i+1].midi;
		}
	}
}

static CMMIDI midiinhdlget(HMIDIIN hmidiin) {

	UINT	i;

	for (i=0; i<midiinhdls; i++) {
		if (midiinhdl[i].hmidiin == hmidiin) {
			return(midiinhdl[i].midi);
		}
	}
	return(NULL);
}

static void midirelease(COMMNG self) {

	CMMIDI	midi;

	midi = (CMMIDI)(self + 1);
	if (midi->opened & CMMIDI_MIDIOUT) {
		waitlastexclusiveout(midi);
		midiOutReset(midi->hmidiout);
		midiOutClose(midi->hmidiout);
	}
	if (midi->opened & CMMIDI_MIDIIN) {
		if (midi->opened & CMMIDI_MIDIINSTART) {
			midiInStop(midi->hmidiin);
			midiInUnprepareHeader(midi->hmidiin,
										&midi->hmidiinhdr, sizeof(MIDIHDR));
		}
		midiInReset(midi->hmidiin);
		midiInClose(midi->hmidiin);
		midiinhdlunreg(midi);
	}
#if defined(VERMOUTH_LIB)
	if (midi->opened & CMMIDI_VERMOUTH) {
		midiout_destroy(midi->vermouth);
	}
#endif
	_MFREE(self);
}


// ----

void cmmidi_initailize(void) {

	UINT	i;

	ZeroMemory(midictrlindex, sizeof(midictrlindex));
	for (i=0; i<sizeof(midictrltbl)/sizeof(BYTE); i++) {
		midictrlindex[midictrltbl[i]] = (BYTE)(i + 1);
	}
	midictrlindex[32] = 1;
}

COMMNG cmmidi_create(const char *midiout, const char *midiin,
														const char *module) {

	UINT		opened;
	UINT		id;
	void		(*outfn)(CMMIDI midi, UINT32 msg);
	HMIDIOUT	hmidiout = NULL;
	HMIDIIN		hmidiin = NULL;
#if defined(VERMOUTH_LIB)
	MIDIHDL		vermouth = NULL;
#endif
	COMMNG		ret;
	CMMIDI		midi;

	opened = 0;
	outfn = midiout_none;
	if (getmidioutid(midiout, &id) == SUCCESS) {
		if (midiOutOpen(&hmidiout, id, 0, 0, CALLBACK_NULL)
														== MMSYSERR_NOERROR) {
			midiOutReset(hmidiout);
			outfn = midiout_win32;
			opened |= CMMIDI_MIDIOUT;
		}
	}
	if (getmidiinid(midiin, &id) == SUCCESS) {
		if (midiInOpen(&hmidiin, id, (DWORD)hWndMain, 0, CALLBACK_WINDOW)
														== MMSYSERR_NOERROR) {
			midiInReset(hmidiin);
			opened |= CMMIDI_MIDIIN;
		}
	}
#if defined(VERMOUTH_LIB)
	else if (!milstr_cmp(midiout, cmmidi_vermouth)) {
		vermouth = midiout_create(vermouth_module, 512);
		if (vermouth != NULL) {
			outfn = midiout_vermouth;
			opened |= CMMIDI_VERMOUTH;
		}
	}
#endif
	if (!opened) {
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
	midi->opened = opened;
	midi->outfn = outfn;
	midi->midictrl = MIDICTRL_READY;
	midi->hmidiout = hmidiout;
#if 1
	midi->hmidiin = hmidiin;
	if (opened & CMMIDI_MIDIIN) {
		if (midiinhdlreg(midi, hmidiin) == SUCCESS) {
			midi->opened |= CMMIDI_MIDIINSTART;
			midi->hmidiinhdr.lpData = (char *)midi->midiinbuf;
			midi->hmidiinhdr.dwBufferLength = MIDI_BUFFER;
			midiInPrepareHeader(hmidiin, &midi->hmidiinhdr, sizeof(MIDIHDR));
			midiInAddBuffer(hmidiin, &midi->hmidiinhdr, sizeof(MIDIHDR));
			midiInStart(hmidiin);
		}
	}
#endif
#if defined(VERMOUTH_LIB)
	midi->vermouth = vermouth;
	if (opened & CMMIDI_VERMOUTH) {
		sound_streamregist((void *)vermouth, (SOUNDCB)vermouth_getpcm);
	}
#endif
//	midi->midisyscnt = 0;
//	midi->mpos = 0;

	midi->midilast = 0x80;
//	midi->midiexcvwait = 0;
	midi->midimodule = (BYTE)module2number(module);
	FillMemory(midi->mch, sizeof(midi->mch), 0xff);
	return(ret);

cmcre_err2:
	if (opened & CMMIDI_MIDIOUT) {
		midiOutReset(hmidiout);
		midiOutClose(hmidiout);
	}
#if defined(VERMOUTH_LIB)
	if (opened & CMMIDI_VERMOUTH) {
		midiout_destroy(vermouth);
	}
#endif

cmcre_err1:
	return(NULL);
}


// ---- midiin callback

static void midiinrecv(CMMIDI midi, const BYTE *data, UINT size) {

	UINT	wpos;
	UINT	wsize;

	size = min(size, MIDI_BUFFER - midi->recvsize);
	if (size) {
		wpos = (midi->recvpos + midi->recvsize) & (MIDI_BUFFER - 1);
		midi->recvsize += size;
		wsize = min(size, MIDI_BUFFER - wpos);
		CopyMemory(midi->recvbuf + wpos, data, wsize);
		size -= wsize;
		if (size) {
			CopyMemory(midi->recvbuf, data + wsize, size);
		}
	}
}

void cmmidi_recvdata(HMIDIIN hdr, UINT32 data) {

	CMMIDI	midi;
	UINT	databytes;

	midi = midiinhdlget(hdr);
	if (midi) {
		databytes = 0;
		switch(data & 0xf0) {
			case 0xc0:
			case 0xd0:
				databytes = 2;
				break;

			case 0x80:
			case 0x90:
			case 0xa0:
			case 0xb0:
			case 0xe0:
				databytes = 3;
				break;
#if 0
			case 0xf0:
				switch(data & 0xff) {
					case MIDI_TIMING:
					case MIDI_START:
					case MIDI_CONTINUE:
					case MIDI_STOP:
					case MIDI_ACTIVESENSE:
					case MIDI_SYSTEMRESET:
						databytes = 1;
						break;
				}
				break;
#endif
		}
		midiinrecv(midi, (BYTE *)&data, databytes);
	}
}

void cmmidi_recvexcv(HMIDIIN hdr, MIDIHDR *data) {

	CMMIDI	midi;

	midi = midiinhdlget(hdr);
	if (midi) {
		midiinrecv(midi, (BYTE *)data->lpData, data->dwBytesRecorded);
		midiInUnprepareHeader(midi->hmidiin,
										&midi->hmidiinhdr, sizeof(MIDIHDR));
		midiInPrepareHeader(midi->hmidiin,
										&midi->hmidiinhdr, sizeof(MIDIHDR));
		midiInAddBuffer(midi->hmidiin, &midi->hmidiinhdr, sizeof(MIDIHDR));
	}
}



struct _midivoice;
typedef	struct _midivoice	_VOICE;
typedef	struct _midivoice	*VOICE;

typedef void (*MIXPROC)(VOICE v, SINT32 *dst, SAMPLE src, SAMPLE srcterm);
typedef SAMPLE (*RESPROC)(VOICE v, SAMPLE dst, SAMPLE dstterm);

enum {
	CHANNEL_MASK	= 0x0f,
	CHANNEL_RHYTHM	= 0x10,
	CHANNEL_SUSTAIN	= 0x20,
	CHANNEL_MONO	= 0x40
};

enum {
	VOICE_FREE		= 0x00,
	VOICE_ON		= 0x01,
	VOICE_SUSTAIN	= 0x02,
	VOICE_OFF		= 0x04,
	VOICE_REL		= 0x08
};

enum {
	VOICE_MIXNORMAL	= 0x00,
	VOICE_MIXLEFT	= 0x01,
	VOICE_MIXRIGHT	= 0x02,
	VOICE_MIXCENTRE	= 0x03,
	VOICE_MIXMASK	= 0x03,
	VOICE_FIXPITCH	= 0x04
};

typedef struct {
	UINT		flag;
	int			level;
	int			pitchbend;
	int			pitchsens;
	float		pitchfactor;
	INSTRUMENT	inst;
#if !defined(MIDI_GMONLY)
	INSTRUMENT	*rhythm;
#endif

	BYTE		bank;
	BYTE		program;
	BYTE		volume;
	BYTE		expression;
	BYTE		panpot;
	BYTE		rpn_l;
	BYTE		rpn_m;
} _CHANNEL, *CHANNEL;

typedef struct {
	int			sweepstep;
	int			sweepcount;
	int			count;
	int			step;
	int			volume;
} VOICETRE;

typedef struct {
	int			sweepstep;
	int			sweepcount;
	int			phase;
	int			rate;
	int			count;
} VOICEVIB;

struct _midivoice {
	BYTE		phase;
	BYTE		flag;
	BYTE		note;
	BYTE		velocity;

	CHANNEL		channel;
	int			frequency;
	float		freq;
	int			panpot;

	MIXPROC		mix;
	RESPROC		resamp;
	INSTLAYER	sample;
	int			samppos;
	int			sampstep;
	int			envvol;
	int			envterm;
	int			envstep;
	int			envleft;
	int			envright;
	int			envphase;
	int			envcount;

	int			volleft;
	int			volright;

#if defined(ENABLE_TREMOLO)
	VOICETRE	tremolo;
#endif
#if defined(ENABLE_VIRLATE)
	float		freqnow;
	VOICEVIB	vibrate;
#endif
};


int envlope_setphase(VOICE v, int phase);
void envelope_updates(VOICE v);

void voice_setphase(VOICE v, BYTE phase);
void voice_setmix(VOICE v);


// ---- macro

#define voice_setfree(v)		(v)->phase = VOICE_FREE


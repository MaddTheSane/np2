
typedef SINT16	_SAMPLE;
typedef SINT16	*SAMPLE;

enum {
	MODE_16BIT		= 0x01,
	MODE_UNSIGNED	= 0x02,
	MODE_LOOPING	= 0x04,
	MODE_PINGPONG	= 0x08,
	MODE_REVERSE	= 0x10,
	MODE_SUSTAIN	= 0x20,
	MODE_ENVELOPE	= 0x40
};

typedef struct {
	int		sweep;
	int		step;
} INSTTRE;

typedef struct {
	int		sweep;
	int		rate;
} INSTVIB;

typedef struct {
	SAMPLE	data;
	int		loopstart;
	int		loopend;
	int		datasize;
	int		samprate;
	int		freqlow;
	int		freqhigh;
	int		freqroot;
	int		envratetbl[6];
	int		envpostbl[6];
	int		volume;
	int		tremolo_sweep;
	int		tremolo_step;
	int		vibrate_sweep;
	int		vibrate_rate;
	BYTE	tremolo_depth;
	BYTE	vibrate_depth;
	BYTE	mode;
	BYTE	panpot;
} _INSTLAYER, *INSTLAYER;

typedef struct {
	int			layers;
	int			freq;
} _INSTRUMENT, *INSTRUMENT;


int inst_singleload(MIDIMOD mod, UINT bank, UINT num);
int inst_bankload(MIDIMOD mod, UINT bank);
void inst_bankfree(MIDIMOD mod, UINT bank);


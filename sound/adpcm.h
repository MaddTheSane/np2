
enum {
	ADTIMING_BIT	= 11,
	ADTIMING		= (1 << ADTIMING_BIT),
	ADPCM_SHIFT		= 3
};

typedef struct {
	BYTE	ctrl1;		// 00
	BYTE	ctrl2;		// 01
	BYTE	start[2];	// 02
	BYTE	stop[2];	// 04
	BYTE	reg06;
	BYTE	reg07;
	BYTE	data;		// 08
	BYTE	delta[2];	// 09
	BYTE	level;		// 0b
	BYTE	limit[2];	// 0c
	BYTE	reg0e;
	BYTE	reg0f;
	BYTE	flag;		// 10
	BYTE	reg11;
	BYTE	reg12;
	BYTE	reg13;
} ADPCMREG;

typedef struct {
	ADPCMREG	reg;
	UINT32		pos;
	UINT32		start;
	UINT32		stop;
	UINT32		limit;
	SINT32		level;
	UINT32		base;
	SINT32		samp;
	SINT32		delta;
	SINT32		remain;
	SINT32		step;
	SINT32		out0;
	SINT32		out1;
	SINT32		fb;
	SINT32		pertim;
	BYTE		status;
	BYTE		play;
	BYTE		mask;
	BYTE		fifopos;
	BYTE		fifo[2];
	BYTE		padding[2];
	BYTE		buf[0x40000];
} _ADPCM, *ADPCM;

typedef struct {
	UINT	rate;
	UINT	vol;
} ADPCMCFG;


#ifdef __cplusplus
extern "C" {
#endif

void adpcm_initialize(UINT rate);
void adpcm_setvol(UINT vol);

void adpcm_reset(ADPCM ad);
void adpcm_update(ADPCM ad);
void adpcm_setreg(ADPCM ad, REG8 reg, REG8 value);
REG8 adpcm_status(ADPCM ad);

REG8 SOUNDCALL adpcm_readsample(ADPCM ad);
void SOUNDCALL adpcm_datawrite(ADPCM ad, REG8 data);
void SOUNDCALL adpcm_getpcm(ADPCM ad, SINT32 *buf, UINT count);

#ifdef __cplusplus
}
#endif


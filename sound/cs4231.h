
typedef REG8 (*CS4231DMA)(DMACH dmach);

enum {
	CS4231_BUFFERS	= (1 << 6),
	CS4231_BUFBYTES	= (CS4231_BUFFERS * 4)
};

typedef struct {
	BYTE	adc_l;				// 0
	BYTE	adc_r;				// 1
	BYTE	aux1_l;				// 2
	BYTE	aux1_r;				// 3
	BYTE	aux2_l;				// 4
	BYTE	aux2_r;				// 5
	BYTE	dac_l;				// 6
	BYTE	dac_r;				// 7
	BYTE	datafmt;			// 8
	BYTE	intconfig;			// 9
	BYTE	pinctrl;			// a
	BYTE	errorstatus;
	BYTE	mode_id;
	BYTE	loopctrl;
	BYTE	playcount[2];

	BYTE	featurefunc[2];
	BYTE	line_l;
	BYTE	line_r;
	BYTE	timer[2];
	BYTE	reserved1;
	BYTE	reserved2;
	BYTE	featurestatus;
	BYTE	chipid;
	BYTE	monoinput;
	BYTE	reserved3;
	BYTE	cap_datafmt;
	BYTE	reserved4;
	BYTE	cap_basecount[2];
} CS4231REG;

typedef struct {
	UINT		bufsize;
	UINT		readpos;
	UINT		writepos;
	UINT32		curtime;
	UINT32		step;
	SINT16		pcmdata[2];
	CS4231DMA	proc;

	BYTE		enable;
	BYTE		portctrl;
	UINT16		port;
	BYTE		dmairq;
	BYTE		dmach;
	BYTE		adrs;
	BYTE		index;
	BYTE		intflag;
	BYTE		outenable;
	BYTE		extfunc;
	BYTE		padding;

	CS4231REG	reg;
	BYTE		buffer[CS4231_BUFBYTES];
} _CS4231, *CS4231;

typedef struct {
	UINT	rate;
} CS4231CFG;


#ifdef __cplusplus
extern "C" {
#endif

extern const CS4231DMA cs4231dec[16];

REG8 cs4231_nodecode(DMACH dmach);
void cs4231_dma(NEVENTITEM item);
REG8 DMACCALL cs4231dmafunc(REG8 func);

void cs4231_initialize(UINT rate);
void cs4231_setvol(UINT vol);

void cs4231_reset(void);
void cs4231_update(void);
void cs4231_control(UINT index, REG8 value);

void SOUNDCALL cs4231_getpcm(CS4231 cs, SINT32 *pcm, UINT count);

#ifdef __cplusplus
}
#endif


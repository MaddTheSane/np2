
enum {
	CS4231_BUFFERS	= (1 << 9),
	CS4231_BUFMASK	= (CS4231_BUFFERS - 1)
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
	BYTE	iface;				// 9
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
	UINT		bufdatas;
	UINT		bufpos;
//	UINT32		curtime;
//	UINT32		step;
//	SINT32		pcmdata[2];
	UINT32		pos12;
	UINT32		step12;

	UINT8		enable;
	UINT8		portctrl;
	UINT16		port;
	UINT8		dmairq;
	UINT8		dmach;
	UINT8		adrs;
	UINT8		index;
	UINT8		intflag;
	UINT8		outenable;
	UINT8		extfunc;
	UINT8		padding;

	CS4231REG	reg;
	BYTE		buffer[CS4231_BUFFERS];
} _CS4231, *CS4231;

typedef struct {
	UINT	rate;
} CS4231CFG;


#ifdef __cplusplus
extern "C" {
#endif

void cs4231_dma(NEVENTITEM item);
REG8 DMACCALL cs4231dmafunc(REG8 func);
void cs4231_datasend(REG8 dat);

void cs4231_initialize(UINT rate);
void cs4231_setvol(UINT vol);

void cs4231_reset(void);
void cs4231_update(void);
void cs4231_control(UINT index, REG8 dat);

void SOUNDCALL cs4231_getpcm(CS4231 cs, SINT32 *pcm, UINT count);

#ifdef __cplusplus
}
#endif


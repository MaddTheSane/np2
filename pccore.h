
#include	"nevent.h"

typedef struct {
	UINT	baseclock;
	UINT	multiple;
	BYTE	uPD72020;

	BYTE	dipsw[3];
	BYTE	memsw[8];
	BYTE	vol14[6];
	BYTE	wait[6];

	BYTE	KEY_MODE;
	BYTE	SOUND_SW;
	BYTE	realpal;												// ver0.28
	UINT16	samplingrate;
	UINT16	delayms;
	BYTE	MIDIRESET;
	BYTE	DISPSYNC;
	BYTE	EXTMEM;
	BYTE	PROTECTMEM;												// qn

	BYTE	BTN_RAPID;
	BYTE	BTN_MODE;

	BYTE	snd_x;													// ver0.30
	BYTE	snd14opt[3];
	BYTE	snd26opt;
	BYTE	snd86opt;
	BYTE	spbopt;
	BYTE	spb_vrc;												// ver0.30
	BYTE	spb_vrl;												// ver0.30
	BYTE	spb_x;													// ver0.30
	BYTE	mpuopt;

	BYTE	MOTOR;
	BYTE	MOTORVOL;
	BYTE	BEEP_VOL;
	BYTE	ITF_WORK;
	BYTE	EMM_WORK;
	BYTE	LCD_MODE;
	UINT32	BG_COLOR;
	UINT32	FG_COLOR;

	BYTE	MOUSERAPID;
	BYTE	XSHIFT;
	BYTE	skipline;
	UINT16	skiplight;
	BYTE	RASTER;

	BYTE	vol_fm;													// ver0.27
	BYTE	vol_ssg;
	BYTE	vol_adpcm;
	BYTE	vol_pcm;
	BYTE	vol_rhythm;

	BYTE	pc9861enable;											// ver0.29
	BYTE	pc9861sw[3];
	BYTE	pc9861jmp[6];

	BYTE	grcg;
	BYTE	color16;
	BYTE	calendar;
	BYTE	usefd144;

	char	hddfile[2][MAX_PATH];									// ver0.30
	char	fontfile[MAX_PATH];
} NP2CFG;

typedef struct {
	UINT32	baseclock;
	UINT	multiple;
	UINT32	realclock;
	UINT32	dispclock;
	UINT32	vsyncclock;
	UINT32	mouseclock;
	UINT32	dsoundclock;
	UINT32	dsoundclock2;
	UINT32	keyboardclock;
	UINT32	midiclock;
	UINT32	sampleclock;
	UINT32	frame1000;
	UINT32	raster;
	UINT32	hsync;
	BYTE	cpumode;
} PCCORE;

enum {
	PCBASECLOCK25		= 2457600,
	PCBASECLOCK20		= 1996800
};

#define		CPUMODE_8MHz		0x20


#ifdef __cplusplus
extern "C" {
#endif

extern const char np2version[];

extern	NP2CFG	np2cfg;
extern	PCCORE	pc;
extern	BYTE	dip_default[3];
extern	BYTE	msw_default[8];
extern	BYTE	screenupdate;
extern	int		soundrenewal;
extern	BOOL	drawframe;
extern	UINT	drawcount;


void screendisp(NEVENTITEM item);
void screenvsync(NEVENTITEM item);


void pccore_cfgupdate(void);

void pccore_init(void);
void pccore_term(void);
void pccore_reset(void);
void pccore_exec(BOOL draw);

#ifdef __cplusplus
}
#endif


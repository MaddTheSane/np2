
#include	"nevent.h"

enum {
	PCBASECLOCK25		= 2457600,
	PCBASECLOCK20		= 1996800
};

enum {
	PCMODEL_VM			= 0,
	PCMODEL_VX			= 1
};

#define		CPUMODE_8MHz		0x20

typedef struct {
	// エミュレート中によく参照される奴
	BYTE	uPD72020;
	BYTE	DISPSYNC;
	BYTE	RASTER;
	BYTE	realpal;
	BYTE	LCD_MODE;
	BYTE	skipline;
	UINT16	skiplight;

	BYTE	KEY_MODE;
	BYTE	XSHIFT;
	BYTE	BTN_RAPID;
	BYTE	BTN_MODE;

	BYTE	dipsw[3];
	BYTE	MOUSERAPID;

	BYTE	calendar;
	BYTE	usefd144;
	BYTE	wait[6];


	// リセット時とかあんまり参照されない奴
	char	model[8];
	UINT	baseclock;
	UINT	multiple;

	BYTE	memsw[8];

	BYTE	ITF_WORK;
	BYTE	EXTMEM;
	BYTE	grcg;
	BYTE	color16;
	UINT32	BG_COLOR;
	UINT32	FG_COLOR;

	UINT16	samplingrate;
	UINT16	delayms;
	BYTE	SOUND_SW;
	BYTE	snd_x;

	BYTE	snd14opt[3];
	BYTE	snd26opt;
	BYTE	snd86opt;
	BYTE	spbopt;
	BYTE	spb_vrc;												// ver0.30
	BYTE	spb_vrl;												// ver0.30
	BYTE	spb_x;													// ver0.30

	BYTE	BEEP_VOL;
	BYTE	vol14[6];
	BYTE	vol_fm;
	BYTE	vol_ssg;
	BYTE	vol_adpcm;
	BYTE	vol_pcm;
	BYTE	vol_rhythm;

	BYTE	mpuenable;
	BYTE	mpuopt;

	BYTE	pc9861enable;
	BYTE	pc9861sw[3];
	BYTE	pc9861jmp[6];

	BYTE	MOTOR;
	BYTE	MOTORVOL;
	BYTE	PROTECTMEM;

	char	hddfile[2][MAX_PATH];									// ver0.30
	char	fontfile[MAX_PATH];
	char	biospath[MAX_PATH];
} NP2CFG;

typedef struct {
	UINT32	baseclock;
	UINT	multiple;
	UINT32	realclock;
	UINT32	dispclock;
	UINT32	vsyncclock;
	UINT32	mouseclock;
	UINT32	keyboardclock;
	UINT32	midiclock;
	UINT32	frame1000;
	UINT32	raster;
	UINT32	hsync;
	BYTE	cpumode;
	BYTE	model;
} PCCORE;


#ifdef __cplusplus
extern "C" {
#endif

extern const char np2version[];

extern	NP2CFG	np2cfg;
extern	PCCORE	pc;
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


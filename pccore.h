
#include	"nevent.h"
#include	"statsave.h"

enum {
	PCBASECLOCK25		= 2457600,
	PCBASECLOCK20		= 1996800
};

enum {
	PCMODEL_VM			= 0,
	PCMODEL_VX			= 1,
	PCMODELMASK			= 0x3f,
	PCMODEL_PC9821		= 0x40,
	PCMODEL_EPSON		= 0x80,

	PCHDD_SASI			= 0x01,
	PCHDD_SCSI			= 0x02,
	PCHDD_IDE			= 0x04,

	PCSOUND_NONE		= 0x00,

	PCCBUS_PC9861K		= 0x0001,
	PCCBUS_MPU98		= 0x0002
};

#define		CPUMODE_8MHz		0x20

typedef struct {
	// エミュレート中によく参照される奴
	UINT8	uPD72020;
	UINT8	DISPSYNC;
	UINT8	RASTER;
	UINT8	realpal;
	UINT8	LCD_MODE;
	UINT8	skipline;
	UINT16	skiplight;

	UINT8	KEY_MODE;
	UINT8	XSHIFT;
	UINT8	BTN_RAPID;
	UINT8	BTN_MODE;

	BYTE	dipsw[3];
	UINT8	MOUSERAPID;

	UINT8	calendar;
	UINT8	usefd144;
	BYTE	wait[6];


	// リセット時とかあんまり参照されない奴
	char	model[8];
	UINT	baseclock;
	UINT	multiple;

	BYTE	memsw[8];

	UINT8	ITF_WORK;
	UINT8	EXTMEM;
	UINT8	grcg;
	UINT8	color16;
	UINT32	BG_COLOR;
	UINT32	FG_COLOR;

	UINT16	samplingrate;
	UINT16	delayms;
	UINT8	SOUND_SW;
	UINT8	snd_x;

	BYTE	snd14opt[3];
	UINT8	snd26opt;
	UINT8	snd86opt;
	UINT8	spbopt;
	UINT8	spb_vrc;												// ver0.30
	UINT8	spb_vrl;												// ver0.30
	UINT8	spb_x;													// ver0.30

	UINT8	BEEP_VOL;
	BYTE	vol14[6];
	UINT8	vol_fm;
	UINT8	vol_ssg;
	UINT8	vol_adpcm;
	UINT8	vol_pcm;
	UINT8	vol_rhythm;

	UINT8	mpuenable;
	UINT8	mpuopt;

	UINT8	pc9861enable;
	BYTE	pc9861sw[3];
	BYTE	pc9861jmp[6];

	UINT8	fddequip;
	UINT8	MOTOR;
	UINT8	MOTORVOL;
	UINT8	PROTECTMEM;
	UINT8	hdrvacc;

	char	sasihdd[2][MAX_PATH];									// ver0.74
	char	scsihdd[4][MAX_PATH];									// ver0.74
	char	fontfile[MAX_PATH];
	char	biospath[MAX_PATH];
	char	hdrvroot[MAX_PATH];
} NP2CFG;

typedef struct {
	UINT32	baseclock;
	UINT	multiple;

	UINT8	cpumode;
	UINT8	model;
	UINT8	extmem;
	UINT8	hddif;

	UINT32	sound;
	UINT32	device;

	UINT32	realclock;
	UINT32	dispclock;
	UINT32	vsyncclock;
	UINT32	keyboardclock;
	UINT32	midiclock;
	UINT32	raster;
	UINT32	hsync;
} PCCORE;


#ifdef __cplusplus
extern "C" {
#endif

extern const char np2version[];

extern	NP2CFG	np2cfg;
extern	PCCORE	pccore;
extern	BYTE	screenupdate;
extern	int		soundrenewal;
extern	BOOL	drawframe;
extern	UINT	drawcount;
extern	BOOL	hardwarereset;

void getbiospath(char *path, const char *fname, int maxlen);
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


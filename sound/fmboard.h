
#include	"soundrom.h"
#include	"tms3631.h"
#include	"fmtimer.h"
#include	"opngen.h"
#include	"psggen.h"
#include	"rhythm.h"
#include	"adpcm.h"
#include	"pcm86.h"
#include	"cs4231.h"


typedef struct {
	BYTE	reg[0x400];
	BYTE	opnreg;
	BYTE	extreg;
	BYTE	opn2reg;
	BYTE	ext2reg;
	BYTE	adpcmmask;
	BYTE	channels;
	BYTE	extend;
	BYTE	padding;
	UINT16	base;
} OPN_T;

typedef struct {
	UINT16	port;
	BYTE	psg3reg;
	BYTE	rhythm;
} AMD98;

typedef struct {
	BYTE	porta;
	BYTE	portb;
	BYTE	portc;
	BYTE	mask;
	BYTE	key[8];
	int		sync;
	int		ch;
} MUSICGEN;


#ifdef __cplusplus
extern "C" {
#endif

extern	_TMS3631	tms3631;
extern	_FMTIMER	fmtimer;
extern	_OPNGEN		opngen;
extern	OPNCH		opnch[OPNCH_MAX];
extern	_PSGGEN		psg1;
extern	_PSGGEN		psg2;
extern	_PSGGEN		psg3;
extern	_RHYTHM		rhythm;
extern	_ADPCM		adpcm;
extern	_PCM86		pcm86;
extern	_CS4231		cs4231;

extern	int			usesound;
extern	OPN_T		opn;
extern	AMD98		amd98;
extern	MUSICGEN	musicgen;


BYTE fmboard_getjoy(PSGGEN psg);

void fmboard_extreg(void (*ext)(REG8 enable));
void fmboard_extenable(REG8 enable);

void fmboard_reset(BYTE num);
void fmboard_bind(void);

#ifdef __cplusplus
}
#endif



#if !defined(DISABLE_SOUND)

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
	UINT8	reg[0x400];
	UINT8	opnreg;
	UINT8	extreg;
	UINT8	opn2reg;
	UINT8	ext2reg;
	UINT8	adpcmmask;
	UINT8	channels;
	UINT8	extend;
	UINT8	padding;
	UINT16	base;
} OPN_T;

typedef struct {
	UINT16	port;
	UINT8	psg3reg;
	UINT8	rhythm;
} AMD98;

typedef struct {
	UINT8	porta;
	UINT8	portb;
	UINT8	portc;
	UINT8	mask;
	UINT8	key[8];
	int		sync;
	int		ch;
} MUSICGEN;


#ifdef __cplusplus
extern "C" {
#endif

extern	UINT32		usesound;
extern	OPN_T		opn;
extern	AMD98		amd98;
extern	MUSICGEN	musicgen;

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


REG8 fmboard_getjoy(PSGGEN psg);

void fmboard_extreg(void (*ext)(REG8 enable));
void fmboard_extenable(REG8 enable);

void fmboard_reset(UINT32 type);
void fmboard_bind(void);

void fmboard_fmrestore(REG8 chbase, UINT bank);
void fmboard_rhyrestore(RHYTHM rhy, UINT bank);

#ifdef __cplusplus
}
#endif

#else

#define	fmboard_reset(t)
#define	fmboard_bind()

#endif


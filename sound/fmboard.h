
#if !defined(DISABLE_SOUND)

#include	"fmtimer.h"
#include	"opngen.h"
#include	"psggen.h"
#include	"rhythm.h"
#include	"adpcm.h"
#include	"pcm86.h"
#include	"cs4231.h"


typedef struct {
	UINT8	addr1l;
	UINT8	addr1h;
	UINT8	addr2l;
	UINT8	addr2h;
	UINT8	data1;
	UINT8	data2;
	UINT16	base;
	UINT8	adpcmmask;
	UINT8	channels;
	UINT8	extend;
	UINT8	_padding2;
	UINT8	reg[0x400];
} OPN_T;

#ifdef __cplusplus
extern "C" {
#endif

extern	UINT32		g_usesound;
extern	OPN_T		g_opn;

extern	_FMTIMER	g_fmtimer;
extern	_OPNGEN		opngen;
extern	_PSGGEN		g_psg[3];
extern	_RHYTHM		g_rhythm;
extern	_ADPCM		g_adpcm;
extern	_PCM86		pcm86;
extern	_CS4231		cs4231;

#define	g_psg1	g_psg[0]
#define	g_psg2	g_psg[1]
#define	g_psg3	g_psg[2]

#if defined(SUPPORT_PX)
extern	OPN_T		g_opn2;
extern	OPN_T		g_opn3;
extern	_RHYTHM		g_rhythm2;
extern	_RHYTHM		g_rhythm3;
extern	_ADPCM		g_adpcm2;
extern	_ADPCM		g_adpcm3;
#endif	// defined(SUPPORT_PX)

REG8 fmboard_getjoy(PSGGEN psg);

void fmboard_extreg(void (*ext)(REG8 enable));
void fmboard_extenable(REG8 enable);

void fmboard_reset(const NP2CFG *pConfig, UINT32 type);
void fmboard_bind(void);

void fmboard_fmrestore(OPN_T* pOpn, REG8 chbase, UINT bank);
void fmboard_psgrestore(OPN_T* pOpn, PSGGEN psg, UINT bank);
void fmboard_rhyrestore(OPN_T* pOpn, RHYTHM rhy, UINT bank);

#ifdef __cplusplus
}
#endif

#else

#define	fmboard_reset(c, t)
#define	fmboard_bind()

#endif


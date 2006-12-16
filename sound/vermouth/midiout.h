
#if !defined(_WIN32_WCE) && !defined(SLZAURUS)
#define	ENABLE_TREMOLO
#define	ENABLE_VIRLATE
#else
#define	MIDI_GMONLY
#endif
#define	ENABLE_GSRX
#define	PANPOT_REVA
// #define	VOLUME_ACURVE
#define ENABLE_PORTB

struct _midimodule;
typedef	struct _midimodule	_MIDIMOD;
typedef	struct _midimodule	*MIDIMOD;

struct _midictrl;
typedef	struct _midictrl	_MIDIHDL;
typedef	struct _midictrl	*MIDIHDL;

enum {
	MIDIOUT_SUCCESS		= 0,
	MIDIOUT_FAILURE		= -1,
	MIDIOUT_ABORT		= -2
};

#define	VOICE_MAX		24

#define	SAMP_SHIFT		12
#define	SAMP_LIMIT		((1 << (SAMP_SHIFT + 1)) - 1)

#define	FREQ_SHIFT		12
#define	FREQ_MASK		((1 << FREQ_SHIFT) - 1)

#define	ENV_RATE		22
#define	ENVRATE_SHIFT	10

#define	TRESWEEP_SHIFT	16
#define	TRERATE_SHIFT	5
#define	TRESWEEP_TUNE	38
#define	TRERATE_TUNE	38

#define	VIBSWEEP_SHIFT	16
#define	VIBRATE_SHIFT	6
#define	VIBSWEEP_TUNE	38
#define	VIBRATE_TUNE	38

#define	REL_COUNT		20


#if defined(MIDI_GMONLY)
#define	MIDI_BANKS	1
#else
#define	MIDI_BANKS	128
#endif

#include	"midimod.h"
#include	"midinst.h"
#include	"midvoice.h"
#include	"midtable.h"

struct _midimodule {
	UINT		samprate;
	UINT		lockcount;
	INSTRUMENT	*tone[MIDI_BANKS * 2];
	TONECFG		tonecfg[MIDI_BANKS * 2];

	PATHLIST	pathlist;
	LISTARRAY	pathtbl;
	LISTARRAY	namelist;
};


struct _midictrl {
	UINT		samprate;
	UINT		worksize;
	int			level;
	UINT8		status;
	SINT8		gain;
	UINT8		master;
	UINT8		moduleid;

	MIDIMOD		module;
	INSTRUMENT	*bank0[2];

	SINT32		*sampbuf;
	SAMPLE		resampbuf;

#if defined(ENABLE_PORTB)
	MIDIHDL		portb;
#endif	// defined(ENABLE_PORTB);

	_CHANNEL	channel[16];
	_VOICE		voice[VOICE_MAX];
};

struct _miditoneloadparam {
	void	*userdata;
	UINT	totaltones;
	UINT	progress;
	UINT	bank;
	UINT	num;
};
typedef struct _miditoneloadparam	MIDIOUTLAEXPARAM;
typedef int (*FNMIDIOUTLAEXCB)(MIDIOUTLAEXPARAM *param);

#ifndef VERMOUTH_EXPORTS
#define	VEXTERN
#define	VEXPORT
#else
#define	VEXTERN		__declspec(dllexport)
#define	VEXPORT		WINAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

VEXTERN UINT VEXPORT midiout_getver(char *string, int leng);
VEXTERN _MIDIHDL * VEXPORT midiout_create(MIDIMOD module, UINT worksize);
VEXTERN void VEXPORT midiout_destroy(MIDIHDL hdl);
VEXTERN void VEXPORT midiout_shortmsg(MIDIHDL hdl, UINT32 msg);
VEXTERN void VEXPORT midiout_longmsg(MIDIHDL hdl, const UINT8 *msg, UINT size);
VEXTERN const SINT32 * VEXPORT midiout_get(MIDIHDL hdl, UINT *samples);
VEXTERN UINT VEXPORT midiout_get16(MIDIHDL hdl, SINT16 *pcm, UINT size);
VEXTERN UINT VEXPORT midiout_get32(MIDIHDL hdl, SINT32 *pcm, UINT size);
VEXTERN void VEXPORT midiout_setgain(MIDIHDL hdl, int gain);
VEXTERN void VEXPORT midiout_setmoduleid(MIDIHDL hdl, UINT8 moduleid);
VEXTERN void VEXPORT midiout_setportb(MIDIHDL hdl, MIDIHDL portb);

VEXTERN _MIDIMOD * VEXPORT midimod_create(UINT samprate);
VEXTERN void VEXPORT midimod_destroy(MIDIMOD hdl);
VEXTERN void VEXPORT midimod_loadprogram(MIDIMOD hdl, UINT num);
VEXTERN void VEXPORT midimod_loadrhythm(MIDIMOD hdl, UINT num);
VEXTERN void VEXPORT midimod_loadgm(MIDIMOD hdl);
VEXTERN void VEXPORT midimod_loadall(MIDIMOD hdl);
VEXTERN void VEXPORT midimod_loadallex(MIDIMOD hdl, FNMIDIOUTLAEXCB cb, void *userdata);

#ifdef __cplusplus
}
#endif


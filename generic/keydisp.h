
#if defined(SUPPORT_KEYDISP)

#include	"cmndraw.h"

enum {
	KEYDISP_MODENONE			= 0,
	KEYDISP_MODEFM,
	KEYDISP_MODEMIDI
};

enum {
	KEYDISP_NOTEMAX		= 16,
	KEYDISP_CHMAX		= 16,

	KEYDISP_KEYCX		= 28,
	KEYDISP_KEYCY		= 14,

	KEYDISP_LEVEL		= (1 << 4),

	KEYDISP_WIDTH		= 301,
	KEYDISP_HEIGHT		= (KEYDISP_KEYCY * KEYDISP_CHMAX) + 1,

	KEYDISP_DELAYEVENTS	= 2048,
	KEYDISP_FMMAX		= 12,
	KEYDISP_PSGMAX		= 3
};

enum {
	KEYDISP_PALBG		= 0,
	KEYDISP_PALFG,
	KEYDISP_PALHIT,

	KEYDISP_PALS
};

enum {
	KEYDISP_FLAGDRAW		= 0x01,
	KEYDISP_FLAGREDRAW		= 0x02,
	KEYDISP_FLAGSIZING		= 0x04
};


#ifdef __cplusplus
extern "C" {
#endif

void keydisp_initialize(void);
void keydisp_setmode(BYTE mode);
void keydisp_setpal(CMNPALFN *palfn);
void keydisp_setdelay(BYTE frames);
BYTE keydisp_process(BYTE framepast);
void keydisp_getsize(int *width, int *height);
BOOL keydisp_paint(CMNVRAM *vram, BOOL redraw);

void keydisp_setfmboard(BYTE board);
void keydisp_fmkeyon(BYTE ch, BYTE value);
void keydisp_psgmix(void *psg);
void keydisp_psgvol(void *psg, BYTE ch);
void keydisp_midi(const BYTE *msg);

#ifdef __cplusplus
}
#endif

#else

#define keydisp_draw(a)
#define keydisp_setfmboard(a)
#define keydisp_fmkeyon(a, b)
#define keydisp_psgmix(a)
#define keydisp_psgvol(a, b)
#define	keydisp_midi(a)

#endif


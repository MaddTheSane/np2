
// ‚Ü‚¾ƒeƒXƒg

#if defined(SUPPORT_SOFTKBD)

#include	"cmndraw.h"

enum {
	SOFTKBD_WIDTH	= 320,
	SOFTKBD_HEIGHT	= 40
};


#ifdef __cplusplus
extern "C" {
#endif

void softkbd_paint(CMNVRAM *vram, CMNPALCNV cnv);
BOOL softkbd_down(int x, int y);
void softkbd_up(void);

#ifdef __cplusplus
}
#endif

#endif


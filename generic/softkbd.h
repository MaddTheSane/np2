
#if defined(SUPPORT_SOFTKBD)

#include	"cmndraw.h"

#ifdef __cplusplus
extern "C" {
#endif

void softkbd_initialize(void);
void softkbd_deinitialize(void);
BOOL softkbd_getsize(int *width, int *height);
void softkbd_paint(CMNVRAM *vram, CMNPALCNV cnv);
BOOL softkbd_down(int x, int y);
void softkbd_up(void);

#ifdef __cplusplus
}
#endif

#endif


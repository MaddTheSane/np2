#include	"compiler.h"

#if defined(SUPPORT_SOFTKBD)

#include	"bmpdata.h"
#include	"softkbd.h"
#include	"keystat.h"

#define	SOFTKEY_MENU	0xfe
#define	SOFTKEY_NC		0xff

#include	"softkbd.res"

typedef struct {
	BYTE	*bmp;
	UINT	width;
	UINT	height;
	UINT8	key;
} SOFTKBD;

static	SOFTKBD	softkbd;


void softkbd_initialize(void) {

	BMPFILE	*bf;
	BMPDATA	inf;

	softkbd.bmp = NULL;
	softkbd.width = 0;
	softkbd.height = 0;
	softkbd.key = SOFTKEY_NC;
	bf = (BMPFILE *)bmpdata_solvedata(np2kbd_bmp);
	if (bf != NULL) {
		if ((bf->bfType[0] == 'B') && (bf->bfType[1] == 'M') &&
			(bmpdata_getinfo((BMPINFO *)(bf + 1), &inf) == SUCCESS) &&
			(inf.bpp == 4)) {
			softkbd.bmp = (BYTE *)bf;
			softkbd.width = inf.width;
			softkbd.height = inf.height;
		}
		else {
			_MFREE(bf);
		}
	}
}

void softkbd_deinitialize(void) {

	BYTE	*bmp;

	bmp = softkbd.bmp;
	softkbd.bmp = NULL;
	if (bmp) {
		_MFREE(bmp);
	}
}

BOOL softkbd_getsize(int *width, int *height) {

	if (softkbd.bmp == NULL) {
		return(FAILURE);
	}
	if (width) {
		*width = softkbd.width;
	}
	if (height) {
		*height = softkbd.height;
	}
	return(SUCCESS);
}

void softkbd_paint(CMNVRAM *vram, CMNPALCNV cnv) {

	cmddraw_bmp16(vram, softkbd.bmp, cnv, CMNBMP_LEFT | CMNBMP_TOP);
}

BOOL softkbd_down(int x, int y) {

	UINT8	key;

	softkbd_up();
	key = getsoftkbd(x, y);
	if (key == SOFTKEY_MENU) {
		return(TRUE);
	}
	else if (key != SOFTKEY_NC) {
		keystat_senddata((UINT8)key);
		softkbd.key = key;
	}
	return(FALSE);
}

void softkbd_up(void) {

	if (softkbd.key != SOFTKEY_NC) {
		keystat_senddata((REG8)(softkbd.key | 0x80));
		softkbd.key = SOFTKEY_NC;
	}
}
#endif


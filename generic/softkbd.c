#include	"compiler.h"

#if defined(SUPPORT_SOFTKBD)

#include	"bmpdata.h"
#include	"softkbd.h"
#include	"keystat.h"

#define	SOFTKEY_MENU	0xfe
#define	SOFTKEY_NC		0xff

#if (SUPPORT_SOFTKBD == 1)
#include	"softkbd1.res"
#elif (SUPPORT_SOFTKBD == 2)
#include	"softkbd2.res"
#else
#include	"softkbd.res"
#endif

#if 0
typedef struct {
	BMPDATA	inf;
	BYTE	*ptr;
	int		height;
	int		yalign;
} CMNBMP;

static BOOL cmnbmp_4(const BYTE *dat, CMNBMP *ret) {

	BMPDATA		inf;
const BMPFILE	*bf;

	bf = (BMPFILE *)dat;
	if ((bf == NULL) ||
		(bf->bfType[0] != 'B') || (bf->bfType[1] != 'M') ||
		(bmpdata_getinfo((BMPINFO *)(bf + 1), &inf) != SUCCESS) ||
		(inf.bpp == 4)) {
		return(FAILURE);
	}
	if (ret) {
		ret->inf = inf;
		ret->ptr = ((BYTE *)bf) + (LOADINTELDWORD(bf->bfOffBits));
		ret->yalign = bmpdata_getalign((BMPINFO *)(bf + 1));
		if (inf.height < 0) {
			ret->height = inf.height * -1;
		}
		else {
			ret->ptr += (inf.height - 1) * ret->yalign;
			ret->height = inf.height;
			ret->yalign *= -1;
		}
	}
	return(SUCCESS);
}
#endif


typedef struct {
	BYTE	*bmp;
	UINT	width;
	UINT	height;
	UINT8	key;
} SOFTKBD;

static	SOFTKBD	softkbd;



// ----

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
			if (inf.height > 0) {
				softkbd.height = inf.height;
			}
			else {
				softkbd.height = 0 - inf.height;
			}
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

BOOL softkbd_paint(CMNVRAM *vram, CMNPALCNV cnv) {

	cmddraw_bmp16(vram, softkbd.bmp, cnv, CMNBMP_LEFT | CMNBMP_TOP);
	return(TRUE);
}

BOOL softkbd_down(int x, int y) {

	UINT8	key;

	softkbd_up();
	key = getsoftkbd(x, y);
	if (key == SOFTKEY_MENU) {
		return(TRUE);
	}
	else if (key != SOFTKEY_NC) {
		keystat_down(&key, 1, NKEYREF_SOFTKBD);
		softkbd.key = key;
	}
	return(FALSE);
}

void softkbd_up(void) {

	if (softkbd.key != SOFTKEY_NC) {
		keystat_up(&softkbd.key, 1, NKEYREF_SOFTKBD);
		softkbd.key = SOFTKEY_NC;
	}
}
#endif


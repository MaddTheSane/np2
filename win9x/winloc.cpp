#include	<windows.h>

#define		SNAPDOTPULL		12
#define		SNAPDOTREL		16

static	int		wingx;
static	int		wingy;
static	int		winflg;

void winloc_movingstart(void) {

	wingx = 0;
	wingy = 0;
	winflg = 0;
}

void winloc_movingproc(RECT *prc) {

	RECT	workrc;
	int		winlx, winly;
	int		mv1, mv2;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrc, 0);
	winlx = prc->right - prc->left;
	winly = prc->bottom - prc->top;

	if ((winlx > (workrc.right - workrc.left)) ||
		(winly > (workrc.bottom - workrc.top))) {
		return;
	}
	mv1 = prc->left - workrc.left;
	mv2 = prc->right - workrc.right;
	if (winflg & 1) {
		wingx += mv1;
		prc->left = workrc.left;
	}
	else if (winflg & 2) {
		wingx += mv2;
		prc->left = workrc.right - winlx;
	}
	else if ((mv1 < SNAPDOTPULL) && (mv1 > -SNAPDOTPULL)) {
		wingx += mv1;
		prc->left = workrc.left;
		winflg |= 1;
	}
	else if ((mv2 < SNAPDOTPULL) && (mv2 > -SNAPDOTPULL)) {
		wingx += mv2;
		prc->left = workrc.right - winlx;
		winflg |= 2;
	}
	if ((wingx >= SNAPDOTREL) || (wingx <= -SNAPDOTREL)) {
		prc->left += wingx;
		wingx = 0;
		winflg &= (~3);
	}
	prc->right = prc->left + winlx;

	mv1 = prc->top - workrc.top;
	mv2 = prc->bottom - workrc.bottom;
	if (winflg & 4) {
		wingy += mv1;
		prc->top = workrc.top;
	}
	else if (winflg & 8) {
		wingy += mv2;
		prc->top = workrc.bottom - winly;
	}
	else if ((mv1 < SNAPDOTPULL) && (mv1 > -SNAPDOTPULL)) {
		wingy += mv1;
		prc->top = workrc.top;
		winflg |= 4;
	}
	else if ((mv2 < SNAPDOTPULL) && (mv2 > -SNAPDOTPULL)) {
		wingy += mv2;
		prc->top = workrc.bottom - winly;
		winflg |= 8;
	}
	if ((wingy >= SNAPDOTREL) || (wingy <= -SNAPDOTREL)) {
		prc->top += wingy;
		wingy = 0;
		winflg &= (~0xc);
	}
	prc->bottom = prc->top + winly;
}


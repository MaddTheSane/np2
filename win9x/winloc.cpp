#include	"compiler.h"
#include	"winloc.h"


enum {
	SNAPDOTPULL		= 12,
	SNAPDOTREL		= 16
};


void winloc_movingstart(WINLOC *wl) {

	ZeroMemory(wl, sizeof(WINLOC));
}

void winloc_movingproc(WINLOC *wl, RECT *rect) {

	RECT	workrc;
	int		winlx;
	int		winly;
	int		d;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrc, 0);
	winlx = rect->right - rect->left;
	winly = rect->bottom - rect->top;

	if ((winlx > (workrc.right - workrc.left)) ||
		(winly > (workrc.bottom - workrc.top))) {
		return;
	}

	if (wl->flag & 1) {
		wl->gx += rect->left - wl->tx;
		rect->left = wl->tx;
		if ((wl->gx >= SNAPDOTREL) || (wl->gx <= -SNAPDOTREL)) {
			wl->flag &= ~1;
			rect->left += wl->gx;
			wl->gx = 0;
		}
		rect->right = rect->left + winlx;
	}
	if (wl->flag & 2) {
		wl->gy += rect->top - wl->ty;
		rect->top = wl->ty;
		if ((wl->gy >= SNAPDOTREL) || (wl->gy <= -SNAPDOTREL)) {
			wl->flag &= ~2;
			rect->top += wl->gy;
			wl->gy = 0;
		}
		rect->bottom = rect->top + winly;
	}

	if (!(wl->flag & 1)) {
		do {
			d = rect->left - workrc.left;
			if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
				break;
			}
			d = rect->right - workrc.right;
			if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
				break;
			}
		} while(0);
		if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
			rect->left -= d;
			rect->right = rect->left + winlx;
			wl->flag |= 1;
			wl->gx = d;
			wl->tx = rect->left;
		}
	}
	if (!(wl->flag & 2)) {
		do {
			d = rect->top - workrc.top;
			if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
				break;
			}
			d = rect->bottom - workrc.bottom;
			if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
				break;
			}
		} while(0);
		if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
			rect->top -= d;
			rect->bottom = rect->top + winly;
			wl->flag |= 2;
			wl->gy = d;
			wl->ty = rect->top;
		}
	}
}


#include	"compiler.h"
#include	"np2.h"
#include	"mousemng.h"


static	BYTE	mouserunning = 0;
static	BYTE	lastmouse = 0;
static	short	mousex = 0;
static	short	mousey = 0;
static	BYTE	mouseb = 0xa0;


static void getmaincenter(POINT *cp) {

	RECT	rct;

	GetWindowRect(hWndMain, &rct);
	cp->x = (rct.right + rct.left) / 2;
	cp->y = (rct.bottom + rct.top) / 2;
}


static void mouseonoff(BYTE flg) {

	POINT	cp;
	RECT	rct;

	if ((lastmouse ^ flg) & 1) {
		lastmouse = (flg & 1);
		if (lastmouse & 1) {
			ShowCursor(FALSE);
			getmaincenter(&cp);
			rct.left = cp.x - 200;
			rct.right = cp.x + 200;
			rct.top = cp.y - 200;
			rct.bottom = cp.y + 200;
			SetCursorPos(cp.x, cp.y);
			ClipCursor(&rct);
		}
		else {
			ShowCursor(TRUE);
			ClipCursor(NULL);
		}
	}
}

// ---------------------------------------------------------------------------

BYTE mouse_flag(void) {

	return(mouserunning);
}


void mouse_running(BYTE flg) {

	BYTE	mf = mouserunning;

	switch(flg & 0xc0) {
		case 0x00:
			mf &= ~(1 << (flg & 7));
			break;
		case 0x40:
			mf ^= (1 << (flg & 7));
			break;
		default:
			mf |= (1 << (flg & 7));
			break;
	}
	if ((mf ^ mouserunning) & MOUSE_MASK) {
		mouserunning = (mf & MOUSE_MASK);
		if (mouserunning == 1) {
			mouseonoff(1);
		}
		else {
			mouseonoff(0);
		}
	}
}


void mouse_callback(void) {

	POINT	p, cp;

	if ((lastmouse & 1) && (GetCursorPos(&p))) {
		getmaincenter(&cp);
		mousex += (short)((p.x - cp.x) / 2);
		mousey += (short)((p.y - cp.y) / 2);
		SetCursorPos(cp.x, cp.y);
	}
}


BYTE mousemng_getstat(short *x, short *y, int clear) {

	*x = mousex;
	*y = mousey;
	if (clear) {
		mousex = 0;
		mousey = 0;
	}
	return(mouseb);
}



BYTE mouse_btn(BYTE btn) {

	if (!(lastmouse & 1)) {
		return(0);
	}
	switch(btn) {
		case MOUSE_LEFTDOWN:
			mouseb &= 0x7f;
			break;
		case MOUSE_LEFTUP:
			mouseb |= 0x80;
			break;
		case MOUSE_RIGHTDOWN:
			mouseb &= 0xdf;
			break;
		case MOUSE_RIGHTUP:
			mouseb |= 0x20;
			break;
	}
	return(1);
}


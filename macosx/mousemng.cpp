
#if defined(NP2GCC)

#include	"compiler.h"
#include	"np2.h"
#include	"mousemng.h"


static	BYTE	mouserunning = 0;
static	BYTE	lastmouse = 0;
static	short	mousex = 0;
static	short	mousey = 0;
static	BYTE	mouseb = 0xa0;


static void getmaincenter(Point *cp) {

	Rect	rct;

	GetWindowBounds(hWndMain, kWindowContentRgn, &rct);
	cp->h = (rct.right + rct.left) / 2;
	cp->v = (rct.bottom + rct.top) / 2;
}


void mouseonoff(BYTE flg) {

	Point	cp;

	if ((lastmouse ^ flg) & 1) {
		lastmouse = (flg & 1);
		if (lastmouse & 1) {
            HideCursor();
			getmaincenter(&cp);
            CGWarpMouseCursorPosition(CGPointMake(cp.h, cp.v));
		}
        else {
            ShowCursor();
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

	Point	cp;
    CGMouseDelta	x,y;
    static	CGMouseDelta	pastx=0,pasty=0;

	if (lastmouse & 1) {
        CGGetLastMouseDelta(&x, &y);
        if ((pastx!=x) && (pasty!=y)) {
            mousex += (short)x;
            mousey += (short)y;
            pastx = x;
            pasty = y;
            getmaincenter(&cp);
            CGWarpMouseCursorPosition(CGPointMake(cp.h, cp.v));
       }
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

#else

#include	"compiler.h"
#include	"mousemng.h"

BYTE mousemng_getstat(SINT16 *x, SINT16 *y, int clear) {

	*x = 0;
	*y = 0;
	(void)clear;
	return(0xa0);
}

#endif


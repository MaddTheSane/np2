#include	"compiler.h"
#include	"np2.h"
#include	"scrnmng.h"


typedef struct {
	BOOL			exist;
	WindowPtr		hWnd;
	GWorldPtr		gw;
	PixMapHandle	pm;
	Rect			rect;
} _QDRAW, *QDRAW;


static	_QDRAW		qdraw;
static	GDHandle	hgd;
static	GWorldPtr	gwp;
static	SCRNSURF	scrnsurf;

#ifdef NP2GCC
#define macosx_only
#endif

void scrnmng_initialize(void) {
}

BOOL scrnmng_create(BYTE scrnmode) {

	QDRAW	qd;

	qd = &qdraw;
	SetRect(&qd->rect, 0, 0, 640, 480);
#if defined(macosx_only)   
//GWorldの代わりに直接ウィンドウバッファを設定(tk800)
    GrafPtr		dstport;
    dstport = GetWindowPort(hWndMain);
    if (dstport) {
        LockPortBits(dstport);//こうしないと描画位置がおかしくなる
        qd->pm = GetPortPixMap(dstport);
        qd->exist = TRUE;
        qd->hWnd = hWndMain;
        return(SUCCESS);
    }
#else        
	if (NewGWorld(&qd->gw, 32, &qd->rect, NULL, NULL, useTempMem) == noErr) {
		qd->pm = GetGWorldPixMap(qd->gw);
		qd->exist = TRUE;
		qd->hWnd = hWndMain;
		return(SUCCESS);
	}
#endif
	else {
		(void)scrnmode;
		return(FAILURE);
	}
}

void scrnmng_destroy(void) {

	QDRAW	qd;

	qd = &qdraw;
	if (qd->exist) {
		qd->exist = FALSE;
		DisposeGWorld(qd->gw);
	}
#if defined(macosx_only)    
    UnlockPortBits(GetWindowPort(hWndMain));
#endif
}

void scrnmng_setwidth(int posx, int width) {

	(void)posx;
	(void)width;
}

void scrnmng_setextend(int extend) {

	(void)extend;
}

void scrnmng_setheight(int posy, int height) {

	(void)posy;
	(void)height;
}

const SCRNSURF *scrnmng_surflock(void) {

	QDRAW		qd;
	GDHandle	hgd;
	GWorldPtr	gwp;

#if 0 && defined(macosx_only)    
//色深度が32ビットじゃないときはさようなら(tk800)
    if (CGDisplayBitsPerPixel(kCGDirectMainDisplay)!=32) {
        return(NULL);
    }
#endif
	qd = &qdraw;
	if (!qd->exist) {
		return(NULL);
	}

#if defined(macosx_only)   
//描画位置をウィンドウバーの下に設定(tk800) 
	LockPixels(qd->pm);
	scrnsurf.ptr = (BYTE *)GetPixBaseAddr(qd->pm) + +640*4*22;
#else
	GetGWorld(&gwp, &hgd);
	LockPixels(qd->pm);
	SetGWorld(qd->gw, NULL);

	scrnsurf.ptr = (BYTE *)GetPixBaseAddr(qd->pm);
#endif
	scrnsurf.xalign = 4;
	scrnsurf.yalign = ((*qd->pm)->rowBytes) & 0x3fff;
	scrnsurf.width = 640;
	scrnsurf.height = 400;
	scrnsurf.extend = 0;
	return(&scrnsurf);
}

void scrnmng_surfunlock(const SCRNSURF *surf) {

	QDRAW	qd;

	if (surf) {
		qd = &qdraw;

#if defined(macosx_only)
//画面を更新するようWindow Serverに指示(tk800)
        GrafPtr		dstport;
        dstport = GetWindowPort(hWndMain);
        QDAddRectToDirtyRegion(dstport, &qd->rect);
        QDFlushPortBuffer(dstport, NULL);
		UnlockPixels(qd->pm);
#else

#if TARGET_API_MAC_CARBON
		{
			GrafPtr		thePort;
			RgnHandle	theVisibleRgn = NewRgn();

			theVisibleRgn = NewRgn();
			if (!EmptyRgn(GetPortVisibleRegion(GetWindowPort(qd->hWnd),
														theVisibleRgn))) {
				LockPortBits(GetWindowPort(qd->hWnd));
				LockPixels(qd->pm);
				GetPort(&thePort);
				SetPortWindowPort(qd->hWnd);
				CopyBits((BitMap*)(*qd->pm),
							GetPortBitMapForCopyBits(GetWindowPort(qd->hWnd)),
							&qd->rect, &qd->rect, srcCopy, theVisibleRgn);
				SetPort(thePort);
				UnlockPixels(qd->pm);
				UnlockPortBits(GetWindowPort(qd->hWnd));
			}
			DisposeRgn(theVisibleRgn);
		}
#else
		CopyBits((BitMap *)(*qd->pm), &(qd->hWnd->portBits), &qd->rect,
										&qd->rect, srcCopy, qd->hWnd->visRgn);
#endif

		UnlockPixels(qd->pm);
		SetGWorld(gwp, hgd);
#endif
	}
}


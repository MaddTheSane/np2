#include	"compiler.h"
#if defined(SUPPORT_KEYDISP)
#include	"resource.h"
#include	"np2.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"ini.h"
#include	"kdispwin.h"
#include	"keydisp.h"
#include	"cmndraw.h"
#include	"dosio.h"

#define KEYDISP_PALS	3
#define	FLOATINGWINDOWTITLEOFFSET 16

static const UINT32 kdwinpal[KEYDISP_PALS] =
									{0x00000000, 0xffffffff, 0xf9ff0000};

enum {
	KDISPCFG_FM		= 0x00,
	KDISPCFG_MIDI	= 0x80
};

enum {
	IDM_KDCLOSE		= 3000
};

typedef struct {
	WindowRef		hwnd;
	Point			wlex;
	PixMapHandle	dd2hdl;
	bool			drawed;
} KDWIN;

typedef struct {
	int		posx;
	int		posy;
	BYTE	mode;
	BYTE	type;
} KDISPCFG;

static	KDWIN		kdwin;
static	KDISPCFG	kdispcfg;


static BYTE getpal8(CMNPALFN *self, UINT num) {

	if (num < KEYDISP_PALS) {
		return(kdwinpal[num] >> 24);
	}
	return(0);
}

static UINT32 getpal32(CMNPALFN *self, UINT num) {

	if (num < KEYDISP_PALS) {
		return(kdwinpal[num] & 0xffffff);
	}
	return(0);
}

static UINT16 cnvpal16(CMNPALFN *self, RGB32 pal32) {
	RGB32   pal;
	
	pal.d = pal32.d & 0xF8F8F8;
	return((UINT16)((pal.p.g << 2) +
						(pal.p.r << 7) + (pal.p.b >> 3)));
}

static void drawwithcopybits(WindowPtr hWnd) {
	GWorldPtr		gw;
	PixMapHandle	pm;
	Rect			rect;
	GrafPtr			dst;
	GrafPtr			port;
	CMNVRAM			vram;
	RgnHandle		theVisibleRgn;
	bool			portchanged;

	port = GetWindowPort(hWnd);
	GetWindowBounds(hWnd, kWindowContentRgn, &rect);
	OffsetRect(&rect, -rect.left, -rect.top);
	
	if (NewGWorld(&gw, CGDisplayBitsPerPixel(kCGDirectMainDisplay), &rect, NULL, NULL, useTempMem) == noErr) {
		pm = GetGWorldPixMap(gw);
		LockPixels(pm);
		vram.ptr = (BYTE *)GetPixBaseAddr(pm);
		vram.width = rect.right - rect.left;
		vram.height = rect.bottom - rect.top;
		vram.bpp = CGDisplayBitsPerPixel(kCGDirectMainDisplay);
		vram.xalign = vram.bpp >> 3;
		vram.yalign = GetPixRowBytes(pm);
		if (vram.ptr) {
			if (keydisp_paint(&vram, TRUE)) {
				theVisibleRgn = NewRgn();
				if (!EmptyRgn(GetPortVisibleRegion(port, theVisibleRgn))) {
					LockPortBits(port);
					portchanged = QDSwapPort(port, &dst);
					CopyBits((BitMap*)(*pm),
							GetPortBitMapForCopyBits(port),
							&rect, &rect, srcCopy, theVisibleRgn);
					if (portchanged) QDSwapPort(dst, NULL);
					UnlockPortBits(port);
				}
				DisposeRgn(theVisibleRgn);
			}
		}
		UnlockPixels(pm);
		DisposeGWorld(gw);
	}
}

static void drawkeys(WindowPtr hWnd, BOOL redraw) {

	Rect	rect;
	Rect	draw;
	CMNVRAM vram;
	GrafPtr	port = NULL, dst;
	long	rowbyte;
	BYTE	drawit = FALSE;

	if (kdwin.drawed == false) {
		static int count = 5;
		drawwithcopybits(hWnd);
		count--;
		if (!count) {
			kdwin.drawed = true;
		}
		return;
	}
	
	port = GetWindowPort(hWnd);
	GetWindowBounds(hWnd, kWindowContentRgn, &rect);
	draw.left = 0;
	draw.top = 0;
	draw.right = min(KEYDISP_WIDTH, rect.right - rect.left);
	draw.bottom = min(KEYDISP_HEIGHT, rect.bottom - rect.top);
	if ((draw.right <= 0) || (draw.bottom <= 1)) {
		return;
	}
	
    bool portchanged = QDSwapPort(port, &dst);
    LockPortBits(port);
	LockPixels(kdwin.dd2hdl);
	
    rowbyte = GetPixRowBytes(kdwin.dd2hdl);
	vram.ptr = (BYTE *)GetPixBaseAddr(kdwin.dd2hdl) + rowbyte*FLOATINGWINDOWTITLEOFFSET;
	vram.width = rect.right - rect.left;
	vram.height = rect.bottom - rect.top;
	vram.bpp = CGDisplayBitsPerPixel(kCGDirectMainDisplay);
	vram.xalign = vram.bpp >> 3;
	vram.yalign = rowbyte;
	if (vram.ptr) {
		drawit = keydisp_paint(&vram, redraw);
		if (drawit) {
			QDAddRectToDirtyRegion(port, &draw);
		}
	}
	
	UnlockPixels(kdwin.dd2hdl);
	UnlockPortBits(port);
	if (portchanged) QDSwapPort(dst, NULL);
}

static void setkdwinsize(void) {

	int			width;
	int			height;
	Rect		bounds;

	keydisp_getsize(&width, &height);
	GetWindowBounds(kdwin.hwnd, kWindowContentRgn, &bounds);
	SetRect(&bounds, bounds.left, bounds.top, bounds.left+width, bounds.top+height);
	SetWindowBounds(kdwin.hwnd, kWindowContentRgn, &bounds);
}

static void setkeydispmode(BYTE mode) {

	CFStringRef title;
	
	keydisp_setmode(mode);
	if (mode == KEYDISP_MODEFM) {
		title = CFStringCreateWithCString(NULL, "Key Display: FM", kCFStringEncodingMacRoman);
	}
	else {
		title = CFStringCreateWithCString(NULL, "Key Display: MIDI", kCFStringEncodingMacRoman);
	}
	if (title) {
		SetWindowTitleWithCFString(kdwin.hwnd, title);
		CFRelease(title);
	}
}

static pascal OSStatus cfWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;

    if (GetEventClass(event)==kEventClassWindow && GetEventKind(event)==kEventWindowClickContentRgn) {
		if (kdispcfg.mode == KDISPCFG_FM) {
			kdispcfg.mode = KDISPCFG_MIDI;
			setkeydispmode(KEYDISP_MODEMIDI);
		}
		else {
			kdispcfg.mode = KDISPCFG_FM;
			setkeydispmode(KEYDISP_MODEFM);
		}
		kdispwin_draw(0);
		err = noErr;
    }
    else if (GetEventClass(event)==kEventClassWindow && GetEventKind(event)==kEventWindowClose) {
		menu_setkeydisp(np2oscfg.keydisp ^ 1);
		kdispwin_destroy();
		err = noErr;
	}
    else if (GetEventClass(event)==kEventClassWindow && GetEventKind(event)==kEventWindowShowing) {
		kdwin.drawed = false;
	}
	(void)myHandler;
	(void)userData;
    return err;
}

// ----

BOOL kdispwin_initialize(void) {

	keydisp_initialize();
	return(SUCCESS);
}

void kdispwin_create(void) {

	WindowRef   hwnd;
	BYTE		mode;
	CMNPALFN	palfn;
	GrafPtr		dstport = NULL;

	if (kdwin.hwnd != NULL) {
		return;
	}
	ZeroMemory(&kdwin, sizeof(kdwin));
    Rect wRect;
	if (kdispcfg.posx < 0) kdispcfg.posx = 8;
	if (kdispcfg.posy < 0) kdispcfg.posy = 30;
	SetRect(&wRect, kdispcfg.posx, kdispcfg.posy, kdispcfg.posx+100, kdispcfg.posy+100);
	CreateNewWindow(kFloatingWindowClass, kWindowStandardFloatingAttributes, &wRect, &hwnd);
	InstallStandardEventHandler(GetWindowEventTarget(hwnd));
	EventTypeSpec	list[]={ 
            { kEventClassWindow,	kEventWindowClickContentRgn }, 
            { kEventClassWindow,	kEventWindowClose }, 
            { kEventClassWindow,	kEventWindowShowing }, 
        };
	EventHandlerRef	ref;
	InstallWindowEventHandler (hwnd, NewEventHandlerUPP(cfWinproc), GetEventTypeCount(list), list, (void *)hwnd, &ref);
	kdwin.hwnd = hwnd;
	if (hwnd == NULL) {
		goto kdcre_err1;
	}
	switch(kdispcfg.mode) {
		case KDISPCFG_FM:
		default:
			mode = KEYDISP_MODEFM;
			break;

		case KDISPCFG_MIDI:
			mode = KEYDISP_MODEMIDI;
			break;
	}
    dstport = GetWindowPort(hwnd);
    if (!dstport) {
		goto kdcre_err2;
	}
	kdwin.dd2hdl = GetPortPixMap(dstport);
	kdwin.drawed = false;
	setkeydispmode(mode);
	setkdwinsize();
	ShowWindow(hwnd);
	palfn.get8 = getpal8;
	palfn.get32 = getpal32;
	palfn.cnv16 = cnvpal16;
	palfn.userdata = (long)kdwin.dd2hdl;
	keydisp_setpal(&palfn);
	return;

kdcre_err2:
	DisposeWindow(hwnd);

kdcre_err1:
	menu_setkeydisp(0);
	sysmng_update(SYS_UPDATEOSCFG);
}

void kdispwin_destroy(void) {

	if (kdwin.hwnd != NULL) {
		Rect rc;
		GetWindowBounds(kdwin.hwnd, kWindowContentRgn, &rc);
		kdispcfg.posx = rc.left;
		kdispcfg.posy = rc.top;
		sysmng_update(SYS_UPDATEOSCFG);
		DisposeWindow(kdwin.hwnd);
		kdwin.hwnd = NULL;
		kdwin.drawed = false;
	}
}

void kdispwin_draw(BYTE cnt) {

	BYTE	flag;

	if (kdwin.hwnd) {
		if (!cnt) {
			cnt = 1;
		}
		flag = keydisp_process(cnt);
		if (flag & KEYDISP_FLAGSIZING) {
			setkdwinsize();
		}
		drawkeys(kdwin.hwnd, FALSE);
	}
}


// ---- ini

static const char ini_title[] = "NP2 keydisp";
static const char inifile[] = "np2.cfg";			// same file name..

static const INITBL iniitem[] = {
	{"WindposX", INITYPE_SINT32,	&kdispcfg.posx,			0},
	{"WindposY", INITYPE_SINT32,	&kdispcfg.posy,			0},
	{"keydmode", INITYPE_UINT8,		&kdispcfg.mode,			0},
	{"windtype", INITYPE_BOOL,		&kdispcfg.type,			0}};

void kdispwin_readini(void) {

	char	path[MAX_PATH];

	ZeroMemory(&kdispcfg, sizeof(kdispcfg));
	kdispcfg.posx = 8;
	kdispcfg.posy = 30;
	file_cpyname(path, file_getcd(inifile), sizeof(path));
	ini_read(path, ini_title, iniitem, sizeof(iniitem)/sizeof(INITBL));
}

void kdispwin_writeini(void) {

	char	path[MAX_PATH];

	file_cpyname(path, file_getcd(inifile), sizeof(path));
	ini_write(path, ini_title, iniitem, sizeof(iniitem)/sizeof(INITBL), FALSE);
}
#endif


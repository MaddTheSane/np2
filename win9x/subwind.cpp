#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"winloc.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"ini.h"
#include	"dd2.h"
#include	"subwind.h"
#include	"np2class.h"
#include	"keydisp.h"
#include	"softkbd.h"


extern WINLOCEX np2_winlocexallwin(HWND base);

static void wintypechange(HWND hWnd, UINT8 type) {

	WINLOCEX	wlex;

	wlex = np2_winlocexallwin(hWndMain);
	winlocex_setholdwnd(wlex, hWnd);
	np2class_windowtype(hWnd, type);
	winlocex_move(wlex);
	winlocex_destroy(wlex);
}


// ---- key display

#if defined(SUPPORT_KEYDISP)
enum {
	KDISPCFG_FM		= 0x00,
	KDISPCFG_MIDI	= 0x80
};

enum {
	IDM_KDCLOSE		= 3000
};

typedef struct {
	HWND		hwnd;
	WINLOCEX	wlex;
	DD2HDL		dd2hdl;
} KDWIN;

typedef struct {
	int		posx;
	int		posy;
	BYTE	mode;
	BYTE	type;
} KDISPCFG;

static	KDWIN		kdwin;
static	KDISPCFG	kdispcfg;

static const char np2kdcaption[] = "Key Display";
static const char np2kdclass[] = "NP2-KeyDispWin";
static const char str_kdclose[] = "&Close";

static const UINT32 kdwinpal[KEYDISP_PALS] =
									{0x00000000, 0xffffffff, 0xf9ff0000};

static const char np2kdapp[] = "NP2 keydisp";

static const INITBL np2kdini[] = {
	{"WindposX", INITYPE_SINT32,	&kdispcfg.posx,			0},
	{"WindposY", INITYPE_SINT32,	&kdispcfg.posy,			0},
	{"keydmode", INITYPE_UINT8,		&kdispcfg.mode,			0},
	{"windtype", INITYPE_BOOL,		&kdispcfg.type,			0}};


static BYTE kdgetpal8(CMNPALFN *self, UINT num) {

	if (num < KEYDISP_PALS) {
		return(kdwinpal[num] >> 24);
	}
	return(0);
}

static UINT32 kdgetpal32(CMNPALFN *self, UINT num) {

	if (num < KEYDISP_PALS) {
		return(kdwinpal[num] & 0xffffff);
	}
	return(0);
}

static UINT16 kdcnvpal16(CMNPALFN *self, RGB32 pal32) {

	return(dd2_get16pal((DD2HDL)self->userdata, pal32));
}

static void kddrawkeys(HWND hWnd, BOOL redraw) {

	RECT	rect;
	RECT	draw;
	CMNVRAM	*vram;

	GetClientRect(hWnd, &rect);
	draw.left = 0;
	draw.top = 0;
	draw.right = min(KEYDISP_WIDTH, rect.right - rect.left);
	draw.bottom = min(KEYDISP_HEIGHT, rect.bottom - rect.top);
	if ((draw.right <= 0) || (draw.bottom <= 0)) {
		return;
	}
	vram = dd2_bsurflock(kdwin.dd2hdl);
	if (vram) {
		keydisp_paint(vram, redraw);
		dd2_bsurfunlock(kdwin.dd2hdl);
		dd2_blt(kdwin.dd2hdl, NULL, &draw);
	}
}

static void kdsetwinsize(void) {

	int			width;
	int			height;
	WINLOCEX	wlex;

	wlex = np2_winlocexallwin(hWndMain);
	winlocex_setholdwnd(wlex, kdwin.hwnd);
	keydisp_getsize(&width, &height);
	winloc_setclientsize(kdwin.hwnd, width, height);
	winlocex_move(wlex);
	winlocex_destroy(wlex);
}

static void kdsetdispmode(BYTE mode) {

	HMENU	hmenu;

	keydisp_setmode(mode);
	hmenu = np2class_gethmenu(kdwin.hwnd);
	CheckMenuItem(hmenu, IDM_KDISPFM,
					((mode == KEYDISP_MODEFM)?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(hmenu, IDM_KDISPMIDI,
					((mode == KEYDISP_MODEMIDI)?MF_CHECKED:MF_UNCHECKED));
}

static void kdpaintmsg(HWND hWnd) {

	HDC			hdc;
	PAINTSTRUCT	ps;

	hdc = BeginPaint(hWnd, &ps);
	kddrawkeys(hWnd, TRUE);
	EndPaint(hWnd, &ps);
}

static void kdopenpopup(HWND hWnd, LPARAM lp) {

	HMENU	hMenu;
	POINT	pt;

	hMenu = CreatePopupMenu();
	menu_addmenubar(hMenu, np2class_gethmenu(hWnd));
	AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hMenu, MF_STRING, IDM_KDCLOSE, str_kdclose);
	pt.x = LOWORD(lp);
	pt.y = HIWORD(lp);
	ClientToScreen(hWnd, &pt);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
	DestroyMenu(hMenu);
}

static LRESULT CALLBACK kdproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch(msg) {
		case WM_CREATE:
			np2class_wmcreate(hWnd);
			np2class_windowtype(hWnd, (kdispcfg.type & 1) << 1);
			break;

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDM_KDISPFM:
					kdispcfg.mode = KDISPCFG_FM;
					sysmng_update(SYS_UPDATEOSCFG);
					kdsetdispmode(KEYDISP_MODEFM);
					break;

				case IDM_KDISPMIDI:
					kdispcfg.mode = KDISPCFG_MIDI;
					sysmng_update(SYS_UPDATEOSCFG);
					kdsetdispmode(KEYDISP_MODEMIDI);
					break;

				case IDM_KDCLOSE:
					return(SendMessage(hWnd, WM_CLOSE, 0, 0));
			}
			break;

		case WM_PAINT:
			kdpaintmsg(hWnd);
			break;
#if 0
		case WM_ACTIVATE:
			if (LOWORD(wp) != WA_INACTIVE) {
				keydisps_reload();
				kddrawkeys(hWnd, TRUE);
			}
			break;
#endif
		case WM_LBUTTONDOWN:
			if (kdispcfg.type & 1) {
				return(SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0L));
			}
			break;

		case WM_RBUTTONDOWN:
			kdopenpopup(hWnd, lp);
			break;

		case WM_LBUTTONDBLCLK:
			kdispcfg.type ^= 1;
			wintypechange(hWnd, (kdispcfg.type & 1) << 1);
			sysmng_update(SYS_UPDATEOSCFG);
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
			SendMessage(hWndMain, msg, wp, lp);
			break;

		case WM_ENTERMENULOOP:
			soundmng_disable(SNDPROC_SUBWIND);
			break;

		case WM_EXITMENULOOP:
			soundmng_enable(SNDPROC_SUBWIND);
			break;

		case WM_ENTERSIZEMOVE:
			soundmng_disable(SNDPROC_SUBWIND);
			winlocex_destroy(kdwin.wlex);
			kdwin.wlex = np2_winlocexallwin(hWnd);
			break;

		case WM_MOVING:
			winlocex_moving(kdwin.wlex, (RECT *)lp);
			break;

		case WM_EXITSIZEMOVE:
			winlocex_destroy(kdwin.wlex);
			kdwin.wlex = NULL;
			soundmng_enable(SNDPROC_SUBWIND);
			break;

		case WM_MOVE:
			if (!(GetWindowLong(hWnd, GWL_STYLE) &
									(WS_MAXIMIZE | WS_MINIMIZE))) {
				RECT rc;
				GetWindowRect(hWnd, &rc);
				kdispcfg.posx = rc.left;
				kdispcfg.posy = rc.top;
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_CLOSE:
			sysmenu_setkeydisp(0);
			sysmng_update(SYS_UPDATEOSCFG);
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			np2class_wmdestroy(hWnd);
			dd2_release(kdwin.dd2hdl);
			kdwin.hwnd = NULL;
			kdsetdispmode(KEYDISP_MODENONE);
			break;

		default:
			return(DefWindowProc(hWnd, msg, wp, lp));
	}
	return(0L);
}


// ----

BOOL kdispwin_initialize(HINSTANCE hPreInst) {

	WNDCLASS	wc;

	if (!hPreInst) {
		ZeroMemory(&wc, sizeof(wc));
		wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wc.lpfnWndProc = kdproc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = NP2GWL_SIZE;
		wc.hInstance = hInst;
		wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2));
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = MAKEINTRESOURCE(IDR_KEYDISP);
		wc.lpszClassName = np2kdclass;
		if (!RegisterClass(&wc)) {
			return(FAILURE);
		}
	}
	keydisp_initialize();
	return(SUCCESS);
}

void kdispwin_create(void) {

	HWND		hwnd;
	BYTE		mode;
	CMNPALFN	palfn;

	if (kdwin.hwnd != NULL) {
		return;
	}
	ZeroMemory(&kdwin, sizeof(kdwin));
	hwnd = CreateWindow(np2kdclass, np2kdcaption,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						kdispcfg.posx, kdispcfg.posy,
						CW_USEDEFAULT, CW_USEDEFAULT,
						NULL, NULL, hInst, NULL);
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
	kdsetdispmode(mode);
	ShowWindow(hwnd, SW_SHOWNOACTIVATE);
	UpdateWindow(hwnd);
	kdwin.dd2hdl = dd2_create(hwnd, KEYDISP_WIDTH, KEYDISP_HEIGHT);
	if (kdwin.dd2hdl == NULL) {
		goto kdcre_err2;
	}
	palfn.get8 = kdgetpal8;
	palfn.get32 = kdgetpal32;
	palfn.cnv16 = kdcnvpal16;
	palfn.userdata = (long)kdwin.dd2hdl;
	keydisp_setpal(&palfn);
	kdispwin_draw(0);
	SetForegroundWindow(hWndMain);
	return;

kdcre_err2:
	DestroyWindow(hwnd);

kdcre_err1:
	sysmenu_setkeydisp(0);
	sysmng_update(SYS_UPDATEOSCFG);
}

void kdispwin_destroy(void) {

	if (kdwin.hwnd != NULL) {
		DestroyWindow(kdwin.hwnd);
	}
}

HWND kdispwin_gethwnd(void) {

	return(kdwin.hwnd);
}

void kdispwin_draw(BYTE cnt) {

	BYTE	flag;

	if (kdwin.hwnd) {
		if (!cnt) {
			cnt = 1;
		}
		flag = keydisp_process(cnt);
		if (flag & KEYDISP_FLAGSIZING) {
			kdsetwinsize();
		}
		kddrawkeys(kdwin.hwnd, FALSE);
	}
}

void kdispwin_readini(void) {

	char	path[MAX_PATH];

	ZeroMemory(&kdispcfg, sizeof(kdispcfg));
	kdispcfg.posx = CW_USEDEFAULT;
	kdispcfg.posy = CW_USEDEFAULT;
	initgetfile(path, sizeof(path));
	ini_read(path, np2kdapp, np2kdini, sizeof(np2kdini)/sizeof(INITBL));
}

void kdispwin_writeini(void) {

	char	path[MAX_PATH];

	initgetfile(path, sizeof(path));
	ini_write(path, np2kdapp, np2kdini, sizeof(np2kdini)/sizeof(INITBL));
}
#endif


// ---- soft keyboard

#if defined(SUPPORT_SOFTKBD)
typedef struct {
	HWND		hwnd;
	WINLOCEX	wlex;
	DD2HDL		dd2hdl;
	int			width;
	int			height;
} SKBDWIN;

typedef struct {
	int		posx;
	int		posy;
	UINT8	type;
} SKBDCFG;

static	SKBDWIN		skbdwin;
static	SKBDCFG		skbdcfg;

static const char np2skcaption[] = "Soft Keyboard";
static const char np2skclass[] = "NP2-SoftKBDWin";
static const char np2skapp[] = "NP2 softbkd";
static const INITBL np2skini[] = {
	{"WindposX", INITYPE_SINT32,	&skbdcfg.posx,			0},
	{"WindposY", INITYPE_SINT32,	&skbdcfg.posy,			0},
	{"windtype", INITYPE_BOOL,		&skbdcfg.type,			0}};

static void skpalcnv(CMNPAL *dst, const RGB32 *src, UINT pals, UINT bpp) {

	UINT	i;

	switch(bpp) {
#if defined(SUPPORT_8BPP)
		case 8:
			break;
#endif
#if defined(SUPPORT_16BPP)
		case 16:
			for (i=0; i<pals; i++) {
				dst[i].pal16 = dd2_get16pal(skbdwin.dd2hdl, src[i]);
			}
			break;
#endif
#if defined(SUPPORT_24BPP)
		case 24:
#endif
#if defined(SUPPORT_32BPP)
		case 32:
#endif
#if defined(SUPPORT_24BPP) || defined(SUPPORT_32BPP)
			for (i=0; i<pals; i++) {
				dst[i].pal32.d = src[i].d;
			}
			break;
#endif
	}
}

static void skdrawkeys(HWND hWnd, BOOL redraw) {

	RECT		rect;
	RECT		draw;
	CMNVRAM		*vram;

	GetClientRect(hWnd, &rect);
	draw.left = 0;
	draw.top = 0;
	draw.right = min(skbdwin.width, rect.right - rect.left);
	draw.bottom = min(skbdwin.height, rect.bottom - rect.top);
	vram = dd2_bsurflock(skbdwin.dd2hdl);
	if (vram) {
		softkbd_paint(vram, skpalcnv, redraw);
		dd2_bsurfunlock(skbdwin.dd2hdl);
		dd2_blt(skbdwin.dd2hdl, NULL, &draw);
	}
}

static void skpaintmsg(HWND hWnd) {

	HDC			hdc;
	PAINTSTRUCT	ps;

	hdc = BeginPaint(hWnd, &ps);
	skdrawkeys(hWnd, TRUE);
	EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK skproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch(msg) {
		case WM_CREATE:
			np2class_wmcreate(hWnd);
			winloc_setclientsize(hWnd, skbdwin.width, skbdwin.height);
			np2class_windowtype(hWnd, (skbdcfg.type & 1) + 1);
			break;

		case WM_PAINT:
			skpaintmsg(hWnd);
			break;

		case WM_LBUTTONDOWN:
			if ((softkbd_down(LOWORD(lp), HIWORD(lp))) &&
				(skbdcfg.type & 1)) {
				return(SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0L));
			}
			break;

		case WM_LBUTTONDBLCLK:
			if (softkbd_down(LOWORD(lp), HIWORD(lp))) {
				skbdcfg.type ^= 1;
				wintypechange(hWnd, (skbdcfg.type & 1) + 1);
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_LBUTTONUP:
			softkbd_up();
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
			SendMessage(hWndMain, msg, wp, lp);
			break;

		case WM_ENTERMENULOOP:
			soundmng_disable(SNDPROC_SUBWIND);
			break;

		case WM_EXITMENULOOP:
			soundmng_enable(SNDPROC_SUBWIND);
			break;

		case WM_ENTERSIZEMOVE:
			soundmng_disable(SNDPROC_SUBWIND);
			winlocex_destroy(skbdwin.wlex);
			skbdwin.wlex = np2_winlocexallwin(hWnd);
			break;

		case WM_MOVING:
			winlocex_moving(skbdwin.wlex, (RECT *)lp);
			break;

		case WM_EXITSIZEMOVE:
			winlocex_destroy(skbdwin.wlex);
			skbdwin.wlex = NULL;
			soundmng_enable(SNDPROC_SUBWIND);
			break;

		case WM_MOVE:
			if (!(GetWindowLong(hWnd, GWL_STYLE) &
									(WS_MAXIMIZE | WS_MINIMIZE))) {
				RECT rc;
				GetWindowRect(hWnd, &rc);
				skbdcfg.posx = rc.left;
				skbdcfg.posy = rc.top;
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			np2class_wmdestroy(hWnd);
			dd2_release(skbdwin.dd2hdl);
			skbdwin.hwnd = NULL;
			break;

		default:
			return(DefWindowProc(hWnd, msg, wp, lp));
	}
	return(0L);
}


// ----

BOOL skbdwin_initialize(HINSTANCE hPreInst) {

	WNDCLASS	wc;

	if (!hPreInst) {
		ZeroMemory(&wc, sizeof(wc));
		wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wc.lpfnWndProc = skproc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = NP2GWL_SIZE;
		wc.hInstance = hInst;
		wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2));
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = np2skclass;
		if (!RegisterClass(&wc)) {
			return(FAILURE);
		}
	}
	softkbd_initialize();
	return(SUCCESS);
}

void skbdwin_deinitialize(void) {

	softkbd_deinitialize();
}

void skbdwin_create(void) {

	HWND	hwnd;

	if (skbdwin.hwnd != NULL) {
		return;
	}
	ZeroMemory(&skbdwin, sizeof(skbdwin));
	if (softkbd_getsize(&skbdwin.width, &skbdwin.height) != SUCCESS) {
		return;
	}
	hwnd = CreateWindow(np2skclass, np2skcaption,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						skbdcfg.posx, skbdcfg.posy,
						skbdwin.width, skbdwin.height,
						NULL, NULL, hInst, NULL);
	skbdwin.hwnd = hwnd;
	if (hwnd == NULL) {
		goto skcre_err1;
	}
	ShowWindow(hwnd, SW_SHOWNOACTIVATE);
	UpdateWindow(hwnd);
	skbdwin.dd2hdl = dd2_create(hwnd, skbdwin.width, skbdwin.height);
	if (skbdwin.dd2hdl == NULL) {
		goto skcre_err2;
	}
	InvalidateRect(hwnd, NULL, TRUE);
	SetForegroundWindow(hWndMain);
	return;

skcre_err2:
	DestroyWindow(hwnd);

skcre_err1:
	return;
}

void skbdwin_destroy(void) {

	if (skbdwin.hwnd != NULL) {
		DestroyWindow(skbdwin.hwnd);
	}
}

HWND skbdwin_gethwnd(void) {

	return(skbdwin.hwnd);
}

void skbdwin_process(void) {

	if ((skbdwin.hwnd) && (softkbd_process())) {
		skdrawkeys(skbdwin.hwnd, FALSE);
	}
}

void skbdwin_readini(void) {

	char	path[MAX_PATH];

	skbdcfg.posx = CW_USEDEFAULT;
	skbdcfg.posy = CW_USEDEFAULT;
	initgetfile(path, sizeof(path));
	ini_read(path, np2skapp, np2skini, sizeof(np2skini)/sizeof(INITBL));
}

void skbdwin_writeini(void) {

	char	path[MAX_PATH];

	initgetfile(path, sizeof(path));
	ini_write(path, np2skapp, np2skini, sizeof(np2skini)/sizeof(INITBL));
}
#endif


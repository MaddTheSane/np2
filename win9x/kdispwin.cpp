#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"winloc.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"ini.h"
#include	"dd2.h"
#include	"kdispwin.h"
#include	"np2class.h"
#include	"keydisp.h"


extern WINLOCEX np2_winlocexallwin(HWND base);


static const char np2kdcaption[] = "Key Display";
static const char np2kdclass[] = "NP2-KeyDispWin";
static const char str_kdclose[] = "&Close";

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

	return(dd2_get16pal((DD2HDL)self->userdata, pal32));
}

static void drawkeys(HWND hWnd, BOOL redraw) {

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

static void setkdwinsize(void) {

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

static void setkeydispmode(BYTE mode) {

	HMENU	hmenu;

	keydisp_setmode(mode);
	hmenu = np2class_gethmenu(kdwin.hwnd);
	CheckMenuItem(hmenu, IDM_KDISPFM,
					((mode == KEYDISP_MODEFM)?MF_CHECKED:MF_UNCHECKED));
	CheckMenuItem(hmenu, IDM_KDISPMIDI,
					((mode == KEYDISP_MODEMIDI)?MF_CHECKED:MF_UNCHECKED));
}

static void kdtypechange(HWND hWnd, BYTE type) {

	WINLOCEX	wlex;

	wlex = np2_winlocexallwin(hWndMain);
	winlocex_setholdwnd(wlex, hWnd);
	np2class_windowtype(hWnd, (type & 1) << 1);
	winlocex_move(wlex);
	winlocex_destroy(wlex);
}

static void kdpaintmsg(HWND hWnd) {

	HDC			hdc;
	PAINTSTRUCT	ps;
//	RECT		rect;
//	HBRUSH		hbrush;

	hdc = BeginPaint(hWnd, &ps);
#if 0
	GetClientRect(hWnd, &rect);
	hbrush = (HBRUSH)SelectObject(hdc, GetStockObject(BLACK_BRUSH));
	PatBlt(hdc, 0, 0, rect.right, 1, PATCOPY);
	PatBlt(hdc, 0, 0, 1, rect.bottom, PATCOPY);
	SelectObject(hdc, hbrush);
#endif
	drawkeys(hWnd, TRUE);
	EndPaint(hWnd, &ps);
}

static void openpopup(HWND hWnd, LPARAM lp) {

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
					setkeydispmode(KEYDISP_MODEFM);
					break;

				case IDM_KDISPMIDI:
					kdispcfg.mode = KDISPCFG_MIDI;
					sysmng_update(SYS_UPDATEOSCFG);
					setkeydispmode(KEYDISP_MODEMIDI);
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
				drawkeys(hWnd, TRUE);
			}
			break;
#endif
		case WM_LBUTTONDOWN:
			if (kdispcfg.type & 1) {
				return(SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0L));
			}
			break;

		case WM_RBUTTONDOWN:
			openpopup(hWnd, lp);
			break;

		case WM_LBUTTONDBLCLK:
			kdispcfg.type ^= 1;
			kdtypechange(hWnd, kdispcfg.type);
			sysmng_update(SYS_UPDATEOSCFG);
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
			SendMessage(hWndMain, msg, wp, lp);
			break;

		case WM_ENTERMENULOOP:
			soundmng_disable(SNDPROC_KEYDISP);
			break;

		case WM_EXITMENULOOP:
			soundmng_enable(SNDPROC_KEYDISP);
			break;

		case WM_ENTERSIZEMOVE:
			soundmng_disable(SNDPROC_KEYDISP);
			winlocex_destroy(kdwin.wlex);
			kdwin.wlex = np2_winlocexallwin(hWnd);
			break;

		case WM_MOVING:
			winlocex_moving(kdwin.wlex, (RECT *)lp);
			break;

		case WM_EXITSIZEMOVE:
			winlocex_destroy(kdwin.wlex);
			kdwin.wlex = NULL;
			soundmng_enable(SNDPROC_KEYDISP);
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
			setkeydispmode(KEYDISP_MODENONE);
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
	setkeydispmode(mode);
	ShowWindow(hwnd, SW_SHOWNOACTIVATE);
	UpdateWindow(hwnd);
	kdwin.dd2hdl = dd2_create(hwnd, KEYDISP_WIDTH, KEYDISP_HEIGHT);
	if (kdwin.dd2hdl == NULL) {
		goto kdcre_err2;
	}
	palfn.get8 = getpal8;
	palfn.get32 = getpal32;
	palfn.cnv16 = cnvpal16;
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
			setkdwinsize();
		}
		drawkeys(kdwin.hwnd, FALSE);
	}
}


// ---- ini

static const char ini_title[] = "NP2 keydisp";

static const INITBL iniitem[] = {
	{"WindposX", INITYPE_SINT32,	&kdispcfg.posx,			0},
	{"WindposY", INITYPE_SINT32,	&kdispcfg.posy,			0},
	{"keydmode", INITYPE_UINT8,		&kdispcfg.mode,			0},
	{"windtype", INITYPE_BOOL,		&kdispcfg.type,			0}};

void kdispwin_readini(void) {

	char	path[MAX_PATH];

	ZeroMemory(&kdispcfg, sizeof(kdispcfg));
	kdispcfg.posx = CW_USEDEFAULT;
	kdispcfg.posy = CW_USEDEFAULT;
	initgetfile(path, sizeof(path));
	ini_read(path, ini_title, iniitem, sizeof(iniitem)/sizeof(INITBL));
}

void kdispwin_writeini(void) {

	char	path[MAX_PATH];

	initgetfile(path, sizeof(path));
	ini_write(path, ini_title, iniitem, sizeof(iniitem)/sizeof(INITBL));
}


#include "compiler.h"
#include "resource.h"
#include "kdispwnd.h"
#include "np2.h"
#include "misc\tstring.h"
#include "winloc.h"
#include "soundmng.h"
#include "sysmng.h"
#include "menu.h"
#include "ini.h"
#include "dd2.h"
#include "np2class.h"
#include "keydisp.h"

extern WINLOCEX np2_winlocexallwin(HWND base);

static void wintypechange(HWND hWnd, UINT8 type) {

	WINLOCEX	wlex;

	wlex = np2_winlocexallwin(g_hWndMain);
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

typedef struct {
	HWND		hwnd;
	WINLOCEX	wlex;
	DD2HDL		dd2hdl;
} KDISPWIN;

typedef struct {
	int		posx;
	int		posy;
	UINT8	mode;
	UINT8	type;
} KDISPCFG;

static	KDISPWIN	kdispwin;
static	KDISPCFG	kdispcfg;

static const TCHAR kdispclass[] = _T("NP2-KeyDispWin");

static const UINT32 kdisppal[KEYDISP_PALS] =
									{0x00000000, 0xffffffff, 0xf9ff0000};

static const OEMCHAR kdispapp[] = OEMTEXT("Key Display");
static const PFTBL kdispini[] = {
				PFVAL("WindposX", PFTYPE_SINT32,	&kdispcfg.posx),
				PFVAL("WindposY", PFTYPE_SINT32,	&kdispcfg.posy),
				PFVAL("keydmode", PFTYPE_UINT8,		&kdispcfg.mode),
				PFVAL("windtype", PFTYPE_BOOL,		&kdispcfg.type)};


static UINT8 kdgetpal8(CMNPALFN *self, UINT num) {

	if (num < KEYDISP_PALS) {
		return(kdisppal[num] >> 24);
	}
	return(0);
}

static UINT32 kdgetpal32(CMNPALFN *self, UINT num) {

	if (num < KEYDISP_PALS) {
		return(kdisppal[num] & 0xffffff);
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
	vram = dd2_bsurflock(kdispwin.dd2hdl);
	if (vram) {
		keydisp_paint(vram, redraw);
		dd2_bsurfunlock(kdispwin.dd2hdl);
		dd2_blt(kdispwin.dd2hdl, NULL, &draw);
	}
}

static void kdsetwinsize(void) {

	int			width;
	int			height;
	WINLOCEX	wlex;

	wlex = np2_winlocexallwin(g_hWndMain);
	winlocex_setholdwnd(wlex, kdispwin.hwnd);
	keydisp_getsize(&width, &height);
	winloc_setclientsize(kdispwin.hwnd, width, height);
	winlocex_move(wlex);
	winlocex_destroy(wlex);
}

static void kdsetdispmode(UINT8 mode) {

	HMENU	hmenu;

	keydisp_setmode(mode);
	hmenu = np2class_gethmenu(kdispwin.hwnd);
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
	menu_addmenu(hMenu, 0, np2class_gethmenu(hWnd), FALSE);
	menu_addmenures(hMenu, -1, IDR_CLOSE, TRUE);
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

				case IDM_CLOSE:
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
			SendMessage(g_hWndMain, msg, wp, lp);
			break;

		case WM_ENTERMENULOOP:
			soundmng_disable(SNDPROC_SUBWIND);
			break;

		case WM_EXITMENULOOP:
			soundmng_enable(SNDPROC_SUBWIND);
			break;

		case WM_ENTERSIZEMOVE:
			soundmng_disable(SNDPROC_SUBWIND);
			winlocex_destroy(kdispwin.wlex);
			kdispwin.wlex = np2_winlocexallwin(hWnd);
			break;

		case WM_MOVING:
			winlocex_moving(kdispwin.wlex, (RECT *)lp);
			break;

		case WM_EXITSIZEMOVE:
			winlocex_destroy(kdispwin.wlex);
			kdispwin.wlex = NULL;
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
			dd2_release(kdispwin.dd2hdl);
			kdispwin.hwnd = NULL;
			kdsetdispmode(KEYDISP_MODENONE);
			break;

		default:
			return(DefWindowProc(hWnd, msg, wp, lp));
	}
	return(0L);
}

BOOL kdispwin_initialize(HINSTANCE hInstance) {

	WNDCLASS	wc;

	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = kdproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = NP2GWLP_SIZE;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_KEYDISP);
	wc.lpszClassName = kdispclass;
	if (!RegisterClass(&wc)) {
		return(FAILURE);
	}
	keydisp_initialize();
	return(SUCCESS);
}

void kdispwin_create(HINSTANCE hInstance) {

	HWND		hwnd;
	UINT8		mode;
	CMNPALFN	palfn;

	if (kdispwin.hwnd != NULL) {
		return;
	}
	ZeroMemory(&kdispwin, sizeof(kdispwin));

	std::tstring rCaption(LoadTString(IDS_CAPTION_KEYDISP));
	hwnd = CreateWindow(kdispclass, rCaption.c_str(),
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						kdispcfg.posx, kdispcfg.posy,
						KEYDISP_WIDTH, KEYDISP_HEIGHT,
						NULL, NULL, hInstance, NULL);
	kdispwin.hwnd = hwnd;
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
	kdispwin.dd2hdl = dd2_create(hwnd, KEYDISP_WIDTH, KEYDISP_HEIGHT);
	if (kdispwin.dd2hdl == NULL) {
		goto kdcre_err2;
	}
	palfn.get8 = kdgetpal8;
	palfn.get32 = kdgetpal32;
	palfn.cnv16 = kdcnvpal16;
	palfn.userdata = (INTPTR)kdispwin.dd2hdl;
	keydisp_setpal(&palfn);
	kdispwin_draw(0);
	SetForegroundWindow(g_hWndMain);
	return;

kdcre_err2:
	DestroyWindow(hwnd);

kdcre_err1:
	sysmenu_setkeydisp(0);
	sysmng_update(SYS_UPDATEOSCFG);
}

void kdispwin_destroy(void) {

	if (kdispwin.hwnd != NULL) {
		DestroyWindow(kdispwin.hwnd);
	}
}

HWND kdispwin_gethwnd(void) {

	return(kdispwin.hwnd);
}

void kdispwin_draw(UINT8 cnt) {

	UINT8	flag;

	if (kdispwin.hwnd) {
		if (!cnt) {
			cnt = 1;
		}
		flag = keydisp_process(cnt);
		if (flag & KEYDISP_FLAGSIZING) {
			kdsetwinsize();
		}
		kddrawkeys(kdispwin.hwnd, FALSE);
	}
}

void kdispwin_readini(void) {

	OEMCHAR	path[MAX_PATH];

	ZeroMemory(&kdispcfg, sizeof(kdispcfg));
	kdispcfg.posx = CW_USEDEFAULT;
	kdispcfg.posy = CW_USEDEFAULT;
	initgetfile(path, NELEMENTS(path));
	ini_read(path, kdispapp, kdispini, NELEMENTS(kdispini));
}

void kdispwin_writeini(void) {

	OEMCHAR	path[MAX_PATH];

	initgetfile(path, NELEMENTS(path));
	ini_write(path, kdispapp, kdispini, NELEMENTS(kdispini));
}
#endif

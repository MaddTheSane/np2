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
#include "memdbg32.h"

extern WINLOCEX np2_winlocexallwin(HWND base);

static void wintypechange(HWND hWnd, UINT8 type) {

	WINLOCEX	wlex;

	wlex = np2_winlocexallwin(g_hWndMain);
	winlocex_setholdwnd(wlex, hWnd);
	np2class_windowtype(hWnd, type);
	winlocex_move(wlex);
	winlocex_destroy(wlex);
}


// ---- memdbg

#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)

typedef struct {
	HWND		hwnd;
	WINLOCEX	wlex;
	DD2HDL		dd2hdl;
	int			width;
	int			height;
} MDBGWIN;

typedef struct {
	int		posx;
	int		posy;
	UINT8	type;
} MDBGCFG;

static	MDBGWIN		mdbgwin;
static	MDBGCFG		mdbgcfg;

static const TCHAR mdbgtitle[] = _T("Memory Map");
static const TCHAR mdbgclass[] = _T("NP2-MemDbgWin");

static const PFTBL mdbgini[] = {
				PFVAL("WindposX", PFTYPE_SINT32,	&mdbgcfg.posx),
				PFVAL("WindposY", PFTYPE_SINT32,	&mdbgcfg.posy),
				PFVAL("windtype", PFTYPE_BOOL,		&mdbgcfg.type)};


static void mdpalcnv(CMNPAL *dst, const RGB32 *src, UINT pals, UINT bpp) {

	UINT	i;

	switch(bpp) {
#if defined(SUPPORT_16BPP)
		case 16:
			for (i=0; i<pals; i++) {
				dst[i].pal16 = dd2_get16pal(mdbgwin.dd2hdl, src[i]);
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

static void mdwincreate(HWND hWnd) {

	int			width;
	int			height;

	memdbg32_getsize(&width, &height);
}

static void mddrawwin(HWND hWnd, BOOL redraw) {

	RECT		rect;
	RECT		draw;
	CMNVRAM		*vram;

	GetClientRect(hWnd, &rect);
	draw.left = 0;
	draw.top = 0;
	draw.right = min(mdbgwin.width, rect.right - rect.left);
	draw.bottom = min(mdbgwin.height, rect.bottom - rect.top);
	vram = dd2_bsurflock(mdbgwin.dd2hdl);
	if (vram) {
		memdbg32_paint(vram, mdpalcnv, redraw);
		dd2_bsurfunlock(mdbgwin.dd2hdl);
		dd2_blt(mdbgwin.dd2hdl, NULL, &draw);
	}
}

static void mdpaintmsg(HWND hWnd) {

	HDC			hdc;
	PAINTSTRUCT	ps;

	hdc = BeginPaint(hWnd, &ps);
	mddrawwin(hWnd, TRUE);
	EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK mdproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch(msg) {
		case WM_CREATE:
			np2class_wmcreate(hWnd);
			winloc_setclientsize(hWnd, mdbgwin.width, mdbgwin.height);
			np2class_windowtype(hWnd, (mdbgcfg.type & 1) + 1);
			break;

		case WM_PAINT:
			mdpaintmsg(hWnd);
			break;

		case WM_LBUTTONDOWN:
			if (mdbgcfg.type & 1) {
				return(SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0L));
			}
			break;

		case WM_LBUTTONDBLCLK:
			mdbgcfg.type ^= 1;
			wintypechange(hWnd, (mdbgcfg.type & 1) + 1);
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
			winlocex_destroy(mdbgwin.wlex);
			mdbgwin.wlex = np2_winlocexallwin(hWnd);
			break;

		case WM_MOVING:
			winlocex_moving(mdbgwin.wlex, (RECT *)lp);
			break;

		case WM_EXITSIZEMOVE:
			winlocex_destroy(mdbgwin.wlex);
			mdbgwin.wlex = NULL;
			soundmng_enable(SNDPROC_SUBWIND);
			break;

		case WM_MOVE:
			if (!(GetWindowLong(hWnd, GWL_STYLE) &
									(WS_MAXIMIZE | WS_MINIMIZE))) {
				RECT rc;
				GetWindowRect(hWnd, &rc);
				mdbgcfg.posx = rc.left;
				mdbgcfg.posy = rc.top;
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			np2class_wmdestroy(hWnd);
			dd2_release(mdbgwin.dd2hdl);
			mdbgwin.hwnd = NULL;
			break;

		default:
			return(DefWindowProc(hWnd, msg, wp, lp));
	}
	return(0);
}

BOOL mdbgwin_initialize(HINSTANCE hInstance) {

	WNDCLASS	wc;

	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = mdproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = mdbgclass;
	if (!RegisterClass(&wc)) {
		return(FAILURE);
	}
	memdbg32_initialize();
	return(SUCCESS);
}

void mdbgwin_create(HINSTANCE hInstance) {

	HWND	hWnd;

	if (mdbgwin.hwnd != NULL) {
		return;
	}
	ZeroMemory(&mdbgwin, sizeof(mdbgwin));
	memdbg32_getsize(&mdbgwin.width, &mdbgwin.height);
	hWnd = CreateWindow(mdbgclass, mdbgtitle,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						mdbgcfg.posx, mdbgcfg.posy,
						mdbgwin.width, mdbgwin.height,
						NULL, NULL, hInstance, NULL);
	mdbgwin.hwnd = hWnd;
	if (hWnd == NULL) {
		goto mdcre_err1;
	}
	ShowWindow(hWnd, SW_SHOWNOACTIVATE);
	UpdateWindow(hWnd);
	mdbgwin.dd2hdl = dd2_create(hWnd, mdbgwin.width, mdbgwin.height);
	if (mdbgwin.dd2hdl == NULL) {
		goto mdcre_err2;
	}
	InvalidateRect(hWnd, NULL, TRUE);
	SetForegroundWindow(g_hWndMain);
	return;

mdcre_err2:
	DestroyWindow(hWnd);

mdcre_err1:
	return;
}

void mdbgwin_destroy(void) {

	if (mdbgwin.hwnd) {
		DestroyWindow(mdbgwin.hwnd);
	}
}

void mdbgwin_process(void) {

	if ((mdbgwin.hwnd) && (memdbg32_process())) {
		mddrawwin(mdbgwin.hwnd, FALSE);
	}
}

HWND mdbgwin_gethwnd(void) {

	return(mdbgwin.hwnd);
}

void mdbgwin_readini(void) {

	OEMCHAR	path[MAX_PATH];

	mdbgcfg.posx = CW_USEDEFAULT;
	mdbgcfg.posy = CW_USEDEFAULT;
	initgetfile(path, NELEMENTS(path));
	ini_read(path, mdbgtitle, mdbgini, NELEMENTS(mdbgini));
}

void mdbgwin_writeini(void) {

	OEMCHAR	path[MAX_PATH];

	initgetfile(path, NELEMENTS(path));
	ini_write(path, mdbgtitle, mdbgini, NELEMENTS(mdbgini));
}
#endif

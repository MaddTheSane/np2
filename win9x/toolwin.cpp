#include	"compiler.h"
#include	"strres.h"
#include	"resource.h"
#include	"np2.h"
#include	"dosio.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"toolwin.h"
#include	"ini.h"
#include	"dialog.h"
#include	"pccore.h"
#include	"diskdrv.h"


#ifdef STRICT
#define	SCPROC	WNDPROC
#else
#define	SCPROC	FARPROC
#endif


enum {
	IDC_TOOLHDDACC			= 0,
	IDC_TOOLFDD1ACC,
	IDC_TOOLFDD1LIST,
	IDC_TOOLFDD1BROWSE,
	IDC_TOOLFDD1EJECT,
	IDC_TOOLFDD2ACC,
	IDC_TOOLFDD2LIST,
	IDC_TOOLFDD2BROWSE,
	IDC_TOOLFDD2EJECT,
	IDC_MAXITEMS,

	IDC_BASE				= 3000
};

typedef struct {
	char	main[MAX_PATH];
	char	font[64];
	SINT32	fontsize;
	UINT32	color1;
	UINT32	color2;
} TOOLSKIN;

typedef struct {
const char	*cname;
const char	*text;
	DWORD	style;
	short	posx;
	short	posy;
	short	width;
	short	height;
	BOOL	tabstop;
} SUBITEM;

typedef struct {
	HWND	hwnd;
	HBITMAP	hbmp;
	BYTE	fddaccess[2];
	BYTE	hddaccess;
	BYTE	_padding;
	int		winflg;
	int		wingx;
	int		wingy;
	int		wintx;
	int		winty;
	int		parentcn;
	int		parentx;
	int		parenty;
	HFONT	hfont;
	HDC		hdcfont;
	HBRUSH	access[2];
	HWND	sub[IDC_MAXITEMS];
	SCPROC	subproc[IDC_MAXITEMS];
} TOOLWIN;


		NP2TOOL		np2tool;
static	TOOLSKIN	toolskin;
static	SUBITEM		subitem[IDC_MAXITEMS];
static	TOOLWIN		toolwin;

#include	"toolwin.res"


typedef struct {
	WORD	idc;
	BYTE	*counter;
} DISKACC;

static const BYTE fddlist[FDDLIST_DRV] = {
					IDC_TOOLFDD1LIST, IDC_TOOLFDD2LIST};

static const DISKACC diskacc[3] = {
					{IDC_TOOLFDD1ACC,	&toolwin.fddaccess[0]},
					{IDC_TOOLFDD2ACC,	&toolwin.fddaccess[1]},
					{IDC_TOOLHDDACC,	&toolwin.hddaccess}};


// ----

static void skinsetdef(void) {

	ZeroMemory(&toolskin, sizeof(toolskin));
	toolskin.fontsize = 12;
	milstr_ncpy(toolskin.font, str_deffont, sizeof(toolskin.font));
	toolskin.color1 = 0x600000;
	toolskin.color2 = 0xff0000;
	CopyMemory(subitem, defsubitem, sizeof(defsubitem));
}

static HBITMAP skinload(const char *path) {

	char	fname[MAX_PATH];

	if (path) {
		ini_read(path, skintitle, skinini, sizeof(skinini)/sizeof(INITBL));
	}
	if ((path) && (toolskin.main[0])) {
		milstr_ncpy(fname, path, sizeof(fname));
		file_cutname(fname);
		file_catname(fname, toolskin.main, sizeof(fname));
		return((HBITMAP)LoadImage(hInst, fname, IMAGE_BITMAP,
													0, 0, LR_LOADFROMFILE));
	}
	else {
		return(LoadBitmap(hInst, "NP2TOOL"));
	}
}


// ----

static void calctextsize(char *path, int leng, const char *p, int width) {

	HDC		hdc;
	SIZE	cur;
	char	work[MAX_PATH];
	int		l;
	SIZE	tail;
	int		pos;
	int		step;

	milstr_ncpy(path, p, leng);
	hdc = toolwin.hdcfont;
	GetTextExtentPoint32(hdc, p, strlen(p), &cur);
	if (cur.cx < width) {
		return;
	}

	file_cutname(path);
	file_cutseparator(path);
	file_cutname(path);
	l = strlen(path);
	if (l > 1) {
		l -= 1;
		if (milstr_kanji2nd(p, l - 1)) {
			l--;
		}
	}
	else {
		l = 0;
	}
	milstr_ncpy(work, str_browse, sizeof(work));
	milstr_ncat(work, p + l, sizeof(work));
	GetTextExtentPoint32(hdc, work, strlen(work), &tail);
	pos = 0;
	while(pos < l) {
		step = ((((p[pos] ^ 0x20) - 0xa1) & 0xff) < 0x3c)?2:1;
		GetTextExtentPoint32(hdc, p, pos + step, &cur);
		if (cur.cx + tail.cx >= width) {
			break;
		}
		pos += step;
	}
	if (pos < leng) {
		path[pos] = '\0';
	}
	milstr_ncat(path, work, leng);
}

static void setlist(UINT drv) {

	HWND	hwnd;
	RECT	rc;
	int		width;
	TOOLFDD	*fdd;
	char	*p;
	UINT	i;
	char	basedir[MAX_PATH];
	char	dir[MAX_PATH];
	char	*q;

	if (drv >= FDDLIST_DRV) {
		return;
	}
	hwnd = toolwin.sub[fddlist[drv]];
	GetClientRect(hwnd, &rc);
	width = rc.right - rc.left - 6;			// border size?
	SendMessage(hwnd, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	fdd = np2tool.fdd + drv;
	p = fdd->name[0];
	basedir[0] = '\0';
	if (fdd->insert) {
		milstr_ncpy(basedir, p, sizeof(basedir));
		file_cutname(basedir);
	}
	for (i=0; i<FDDLIST_MAX; i++) {
		if (p[0] == '\0') {
			break;
		}
		milstr_ncpy(dir, p, sizeof(dir));
		file_cutname(dir);
		if (!file_cmpname(basedir, dir)) {
			q = file_getname(p);
		}
		else {
			calctextsize(dir, sizeof(dir), p, width);
			q = dir;
		}
		SendMessage(hwnd, CB_INSERTSTRING, (WPARAM)i, (LPARAM)q);
		p += sizeof(fdd->name[0]);
	}
	if (fdd->insert) {
		SendMessage(hwnd, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	}
}

static void sellist(UINT drv) {

	HWND	hwnd;
	UINT	sel;
	char	path[MAX_PATH];

	if (drv >= FDDLIST_DRV) {
		return;
	}
	hwnd = toolwin.sub[fddlist[drv]];
	sel = (UINT)SendMessage(hwnd, CB_GETCURSEL, 0, 0);
	if (sel < FDDLIST_MAX) {
		milstr_ncpy(path, np2tool.fdd[drv].name[sel], sizeof(path));
		diskdrv_setfdd(drv, path, 0);
		toolwin_setfdd(drv, path);
	}
}

static void accdraw(HWND hWnd, BYTE count) {

	HDC			hdc;
	PAINTSTRUCT	ps;
	RECT		rc;
	HBRUSH		hbrush;

	hdc = BeginPaint(hWnd, &ps);
	GetClientRect(hWnd, &rc);
	hbrush = (HBRUSH)SelectObject(hdc, toolwin.access[(count)?1:0]);
	PatBlt(hdc, rc.left, rc.top, rc.right, rc.bottom, PATCOPY);
	SelectObject(hdc, hbrush);
	EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK twsub(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	UINT	idc;
	int		dir;
	UINT	newidc;
	int		files;
	char	fname[MAX_PATH];

	for (idc=0; idc<IDC_MAXITEMS; idc++) {
		if (toolwin.sub[idc] == hWnd) {
			break;
		}
	}
	if (idc >= IDC_MAXITEMS) {
		return(0);
	}
	if (msg == WM_KEYDOWN) {
		if ((TCHAR)wp == VK_TAB) {
			dir = (GetKeyState(VK_SHIFT) >= 0)?1:-1;
			newidc = idc;
			do {
				newidc += dir;
				if (newidc >= IDC_MAXITEMS) {
					newidc = (dir >= 0)?0:(IDC_MAXITEMS - 1);
				}
				if (subitem[newidc].tabstop) {
					SetFocus(toolwin.sub[newidc]);
					break;
				}
			} while(idc != newidc);
		}
		else if ((TCHAR)wp == VK_RETURN) {
			if (subitem[idc].cname == str_button) {
				return(CallWindowProc(toolwin.subproc[idc],
										hWnd, WM_KEYDOWN, VK_SPACE, 0));
			}
		}
	}
	else if (msg == WM_DROPFILES) {
   	    files = DragQueryFile((HDROP)wp, (UINT)-1, NULL, 0);
		if (files == 1) {
			DragQueryFile((HDROP)wp, 0, fname, sizeof(fname));
			if (idc == IDC_TOOLFDD1LIST) {
				diskdrv_setfdd(0, fname, 0);
				toolwin_setfdd(0, fname);
			}
			else if (idc == IDC_TOOLFDD2LIST) {
				diskdrv_setfdd(1, fname, 0);
				toolwin_setfdd(1, fname);
			}
		}
		DragFinish((HDROP)wp);
		return(FALSE);
	}
	else if (msg == WM_PAINT) {
		if (idc == IDC_TOOLHDDACC) {
			accdraw(hWnd, toolwin.hddaccess);
			return(FALSE);
		}
		else if (idc == IDC_TOOLFDD1ACC) {
			accdraw(hWnd, toolwin.fddaccess[0]);
			return(FALSE);
		}
		else if (idc == IDC_TOOLFDD2ACC) {
			accdraw(hWnd, toolwin.fddaccess[1]);
			return(FALSE);
		}
	}
	return(CallWindowProc(toolwin.subproc[idc], hWnd, msg, wp, lp));
}

static void toolwincreate(HWND hWnd, LPARAM lp) {

	HDC			hdc;
	HINSTANCE	hinstance;
const SUBITEM	*p;
	UINT		i;
	HWND		sub;

	toolwin.hfont = CreateFont(toolskin.fontsize, 0, 0, 0, 0, 0, 0, 0,
					SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, FIXED_PITCH, toolskin.font);
    hdc = GetDC(NULL);
	toolwin.hdcfont = CreateCompatibleDC(hdc);
	ReleaseDC(NULL, hdc);
	SelectObject(toolwin.hdcfont, toolwin.hfont);

	toolwin.access[0] = CreateSolidBrush(0x000060);
	toolwin.access[1] = CreateSolidBrush(0x0000ff);

	hinstance = ((LPCREATESTRUCT)lp)->hInstance;
	p = subitem;
	for (i=0; i<IDC_MAXITEMS; i++) {
		sub = CreateWindow(p->cname, p->text,
				WS_CHILD | p->style, p->posx, p->posy, p->width, p->height,
				hWnd, (HMENU)(i + IDC_BASE), hinstance, NULL);
		toolwin.sub[i] = sub;
		toolwin.subproc[i] = (SCPROC)GetWindowLong(sub, GWL_WNDPROC);
		SetWindowLong(sub, GWL_WNDPROC, (LONG)twsub);
		SendMessage(sub, WM_SETFONT, (WPARAM)toolwin.hfont,
														MAKELPARAM(TRUE, 0));
		p++;
	}
	for (i=0; i<FDDLIST_DRV; i++) {
		DragAcceptFiles(toolwin.sub[fddlist[i]], TRUE);
		setlist((BYTE)i);
	}
}

static void toolwindestroy(void) {

	UINT	i;

	for (i=0; i<IDC_MAXITEMS; i++) {
		DestroyWindow(toolwin.sub[i]);
	}
	DeleteObject(toolwin.access[0]);
	DeleteObject(toolwin.access[1]);
	DeleteObject(toolwin.hdcfont);
	DeleteObject(toolwin.hfont);
	DeleteObject(toolwin.hbmp);
}

static void toolwinpaint(HWND hWnd) {

	HDC			hdc;
	PAINTSTRUCT	ps;
	HINSTANCE	hinst;
	BITMAP		bmp;
	HDC			hmdc;

	hdc = BeginPaint(hWnd, &ps);
	hinst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	GetObject(toolwin.hbmp, sizeof(BITMAP), &bmp);
	hmdc = CreateCompatibleDC(hdc);
	SelectObject(hmdc, toolwin.hbmp);
	BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hmdc, 0, 0, SRCCOPY);
	DeleteDC(hmdc);
	EndPaint(hWnd, &ps);
}


// ---- moving

enum {
	SNAPDOTPULL		= 12,
	SNAPDOTREL		= 16
};

static void movingstart(void) {

	toolwin.winflg = 0;
	toolwin.wingx = 0;
	toolwin.wingy = 0;
}

static void movingproc(RECT *rect) {

	RECT	workrc;
	RECT	mainrc;
	int		winlx;
	int		winly;
	BOOL	changes;
	BOOL	connectx;
	BOOL	connecty;
	int		d;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrc, 0);
	GetWindowRect(hWndMain, &mainrc);
	winlx = rect->right - rect->left;
	winly = rect->bottom - rect->top;

	if ((winlx > (workrc.right - workrc.left)) ||
		(winly > (workrc.bottom - workrc.top))) {
		return;
	}

	changes = FALSE;
	do {
		if (toolwin.winflg & 1) {
			toolwin.wingx += rect->left - toolwin.wintx;
			rect->left = toolwin.wintx;
		}
		else {
			d = SNAPDOTPULL;
			do {
				connecty = ((rect->bottom >= mainrc.top) &&
							(rect->top <= mainrc.bottom));
				if (connecty) {
					d = rect->left - mainrc.right;
					if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
						break;
					}
					d = rect->right - mainrc.left;
					if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
						break;
					}
					if ((rect->bottom == mainrc.top) ||
						(rect->top == mainrc.bottom)) {
						d = rect->left - mainrc.left;
						if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
							break;
						}
						d = rect->right - mainrc.right;
						if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
							break;
						}
					}
				}
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
				toolwin.winflg |= 1;
				rect->left -= d;
				toolwin.wingx = d;
				toolwin.wintx = rect->left;
			}
		}
		if ((toolwin.wingx >= SNAPDOTREL) || (toolwin.wingx <= -SNAPDOTREL)) {
			toolwin.winflg &= ~1;
			rect->left += toolwin.wingx;
			toolwin.wingx = 0;
		}
		rect->right = rect->left + winlx;

		if (changes) {
			break;
		}

		if (toolwin.winflg & 2) {
			toolwin.wingy += rect->top - toolwin.winty;
			rect->top = toolwin.winty;
		}
		else {
			d = SNAPDOTPULL;
			do {
				connectx = ((rect->right >= mainrc.left) &&
							(rect->left <= mainrc.right));
				if (connectx) {
					d = rect->top - mainrc.bottom;
					if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
						break;
					}
					d = rect->bottom - mainrc.top;
					if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
						break;
					}
					if ((rect->right == mainrc.left) ||
						(rect->left == mainrc.right)) {
						d = rect->top - mainrc.top;
						if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
							break;
						}
						d = rect->bottom - mainrc.bottom;
						if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
							break;
						}
					}
				}
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
				toolwin.winflg |= 2;
				rect->top -= d;
				toolwin.wingy = d;
				toolwin.winty = rect->top;
				changes = TRUE;
			}
		}
		if ((toolwin.wingy >= SNAPDOTREL) || (toolwin.wingy <= -SNAPDOTREL)) {
			toolwin.winflg &= ~2;
			rect->top += toolwin.wingy;
			toolwin.wingy = 0;
			changes = TRUE;
		}
		rect->bottom = rect->top + winly;
	} while(changes);

	connectx = ((rect->right >= mainrc.left) && (rect->left <= mainrc.right));
	connecty = ((rect->bottom >= mainrc.top) && (rect->top <= mainrc.bottom));
	if ((toolwin.winflg & 3) && ((!connectx) || (!connecty))) {
		toolwin.winflg &= ~3;
		rect->left += toolwin.wingx;
		rect->top += toolwin.wingy;
		rect->right = rect->left + winlx;
		rect->bottom = rect->top + winly;
		toolwin.wingx = 0;
		toolwin.wingy = 0;
	}
}


// ----

static LRESULT CALLBACK twproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch(msg) {
		case WM_CREATE:
			SetWindowLong(hWnd, GWL_STYLE,
							GetWindowLong(hWnd, GWL_STYLE) & (~WS_CAPTION));
			SetWindowPos(hWnd, 0, 0, 0, 0, 0,
					SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
			toolwincreate(hWnd, lp);
			break;

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDC_BASE + IDC_TOOLFDD1LIST:
					if (HIWORD(wp) == CBN_SELCHANGE) {
						sellist(0);
					}
					break;

				case IDC_BASE + IDC_TOOLFDD1BROWSE:
					soundmng_stop();
					dialog_changefdd(hWnd, 0);
					soundmng_play();
					break;

				case IDC_BASE + IDC_TOOLFDD1EJECT:
					diskdrv_setfdd(0, NULL, 0);
					toolwin_setfdd(0, NULL);
					break;

				case IDC_BASE + IDC_TOOLFDD2LIST:
					if (HIWORD(wp) == CBN_SELCHANGE) {
						sellist(1);
					}
					break;

				case IDC_BASE + IDC_TOOLFDD2BROWSE:
					soundmng_stop();
					dialog_changefdd(hWnd, 1);
					soundmng_play();
					break;

				case IDC_BASE + IDC_TOOLFDD2EJECT:
					diskdrv_setfdd(1, NULL, 0);
					toolwin_setfdd(1, NULL);
					break;
			}
			break;

		case WM_PAINT:
			toolwinpaint(hWnd);
			break;

		case WM_ENTERMENULOOP:
			soundmng_stop();
			break;

		case WM_EXITMENULOOP:
			soundmng_play();
			break;

		case WM_ENTERSIZEMOVE:
			soundmng_stop();
			movingstart();
			break;

		case WM_EXITSIZEMOVE:
			soundmng_play();
			break;

		case WM_MOVING:
			movingproc((RECT *)lp);
			break;

		case WM_MOVE:
			if (!(GetWindowLong(hWnd, GWL_STYLE) &
									(WS_MAXIMIZE | WS_MINIMIZE))) {
				RECT rc;
				GetWindowRect(hWnd, &rc);
				np2tool.posx = rc.left;
				np2tool.posy = rc.top;
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_CLOSE:
			xmenu_settoolwin(0);
			sysmng_update(SYS_UPDATEOSCFG);
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			toolwindestroy();
			toolwin.hwnd = NULL;
			break;

		case WM_LBUTTONDOWN:
			SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0L);
			break;

		default:
			return(DefWindowProc(hWnd, msg, wp, lp));
	}
	return(0);
}

BOOL toolwin_initapp(HINSTANCE hInstance) {

	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = twproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = np2toolclass;
	return(RegisterClass(&wc));
}

void toolwin_open(void) {

	char	path[MAX_PATH];
	HBITMAP	hbmp;
	BITMAP	bmp;
	HWND	hWnd;

	if (toolwin.hwnd) {
		return;
	}
	ZeroMemory(&toolwin, sizeof(toolwin));
	skinsetdef();
	milstr_ncpy(path, modulefile, sizeof(path));
	file_cutname(path);
	file_catname(path, np2skin, sizeof(path));
	hbmp = skinload(path);
	if (hbmp == NULL) {
		goto twope_err1;
	}
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	toolwin.hbmp = hbmp;
	hWnd = CreateWindowEx(0, np2toolclass, np2tooltitle, WS_SYSMENU,
							np2tool.posx, np2tool.posy,
							bmp.bmWidth, bmp.bmHeight,
							NULL, NULL, hInst, NULL);
	toolwin.hwnd = hWnd;
	if (hWnd == NULL) {
		goto twope_err2;
	}
	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOW);
	return;

twope_err2:
	DeleteObject(hbmp);

twope_err1:
	xmenu_settoolwin(0);
	sysmng_update(SYS_UPDATEOSCFG);
	return;
}

void toolwin_close(void) {

	if (toolwin.hwnd) {
		DestroyWindow(toolwin.hwnd);
	}
}

void toolwin_movingstart(void) {

	RECT	mainrc;
	RECT	toolrc;

	if (toolwin.hwnd == NULL) {
		return;
	}
	GetWindowRect(hWndMain, &mainrc);
	GetWindowRect(toolwin.hwnd, &toolrc);
	if (((toolrc.right >= mainrc.left) && (toolrc.left <= mainrc.right) &&
		((toolrc.bottom == mainrc.top) || (toolrc.top == mainrc.bottom))) ||
		((toolrc.bottom >= mainrc.top) && (toolrc.top <= mainrc.bottom) &&
		((toolrc.right == mainrc.left) || (toolrc.left == mainrc.right)))) {
		toolwin.parentcn = 1;
		toolwin.parentx = mainrc.left;
		toolwin.parenty = mainrc.top;
	}
}

void toolwin_movingend(void) {

	RECT	mainrc;
	RECT	toolrc;
	int		dx;
	int		dy;

	if ((toolwin.hwnd) && (toolwin.parentcn)) {
		GetWindowRect(hWndMain, &mainrc);
		GetWindowRect(toolwin.hwnd, &toolrc);
		dx = mainrc.left - toolwin.parentx;
		dy = mainrc.top - toolwin.parenty;
		MoveWindow(toolwin.hwnd, toolrc.left + dx, toolrc.top + dy,
								toolrc.right - toolrc.left,
								toolrc.bottom - toolrc.top, TRUE);
	}
	toolwin.parentcn = 0;
}

void toolwin_setfdd(BYTE drv, const char *name) {

	TOOLFDD	*fdd;
	char	*q;
	char	*p;
	UINT	i;

	if (drv >= FDDLIST_DRV) {
		return;
	}
	fdd = np2tool.fdd + drv;
	if ((name == NULL) || (name[0] == '\0')) {
		fdd->insert = 0;
	}
	else {
		fdd->insert = 1;
		q = fdd->name[0];
		for (i=0; i<(FDDLIST_MAX - 1); i++) {
			if (!milstr_cmp(q, name)) {
				break;
			}
			q += sizeof(fdd->name[0]);
		}
		p = q - sizeof(fdd->name[0]);
		while(i > 0) {
			i--;
			CopyMemory(q, p, sizeof(fdd->name[0]));
			p -= sizeof(fdd->name[0]);
			q -= sizeof(fdd->name[0]);
		}
		milstr_ncpy(fdd->name[0], name, sizeof(fdd->name[0]));
	}
	sysmng_update(SYS_UPDATEOSCFG);
	if (toolwin.hwnd != NULL) {
		setlist(drv);
		SetForegroundWindow(hWndMain);
	}
}


static void setdiskacc(UINT num, BYTE count) {

const DISKACC	*acc;
	BYTE		counter;

	if (toolwin.hwnd == NULL) {
		return;
	}
	if (num < (sizeof(diskacc)/sizeof(DISKACC))) {
		acc = diskacc + num;
		counter = *(acc->counter);
		*(acc->counter) = count;
		if (counter) {
			InvalidateRect(toolwin.sub[acc->idc], NULL, TRUE);
		}
	}
}

void toolwin_fddaccess(BYTE drv) {

	if (drv < 2) {
		setdiskacc(drv, 20);
	}
}

void toolwin_hddaccess(BYTE drv) {

	setdiskacc(2, 10);
}

void toolwin_draw(BYTE frame) {

const DISKACC	*acc;
const DISKACC	*accterm;
	BYTE		counter;

	if (toolwin.hwnd == NULL) {
		return;
	}
	if (!frame) {
		frame = 1;
	}
	acc = diskacc;
	accterm = acc + (sizeof(diskacc)/sizeof(DISKACC));
	while(acc < accterm) {
		counter = *acc->counter;
		if (counter) {
			if (counter <= frame) {
				*(acc->counter) = 0;
				InvalidateRect(toolwin.sub[acc->idc], NULL, TRUE);
			}
			else {
				*(acc->counter) -= frame;
			}
		}
		acc++;
	}
}


// ----

static const char np2toolini[] = "np2tool.ini";
static const char ini_title[] = "NP2 tool";

static const INITBL iniitem[] = {
	{"WindposX", INITYPE_SINT32,	&np2tool.posx,			0},
	{"WindposY", INITYPE_SINT32,	&np2tool.posy,			0},
	{"FD1NAME0", INITYPE_STR,		np2tool.fdd[0].name[0],	MAX_PATH},
	{"FD1NAME1", INITYPE_STR,		np2tool.fdd[0].name[1],	MAX_PATH},
	{"FD1NAME2", INITYPE_STR,		np2tool.fdd[0].name[2],	MAX_PATH},
	{"FD1NAME3", INITYPE_STR,		np2tool.fdd[0].name[3],	MAX_PATH},
	{"FD1NAME4", INITYPE_STR,		np2tool.fdd[0].name[4],	MAX_PATH},
	{"FD1NAME5", INITYPE_STR,		np2tool.fdd[0].name[5],	MAX_PATH},
	{"FD1NAME6", INITYPE_STR,		np2tool.fdd[0].name[6],	MAX_PATH},
	{"FD1NAME7", INITYPE_STR,		np2tool.fdd[0].name[7],	MAX_PATH},
	{"FD2NAME0", INITYPE_STR,		np2tool.fdd[1].name[0],	MAX_PATH},
	{"FD2NAME1", INITYPE_STR,		np2tool.fdd[1].name[1],	MAX_PATH},
	{"FD2NAME2", INITYPE_STR,		np2tool.fdd[1].name[2],	MAX_PATH},
	{"FD2NAME3", INITYPE_STR,		np2tool.fdd[1].name[3],	MAX_PATH},
	{"FD2NAME4", INITYPE_STR,		np2tool.fdd[1].name[4],	MAX_PATH},
	{"FD2NAME5", INITYPE_STR,		np2tool.fdd[1].name[5],	MAX_PATH},
	{"FD2NAME6", INITYPE_STR,		np2tool.fdd[1].name[6],	MAX_PATH},
	{"FD2NAME7", INITYPE_STR,		np2tool.fdd[1].name[7],	MAX_PATH}};

static void getinifile(char *path, int leng) {

	milstr_ncpy(path, modulefile, leng);
	file_cutname(path);
	file_catname(path, np2toolini, leng);
}

void toolwin_readini(void) {

	char	path[MAX_PATH];

	ZeroMemory(&np2tool, sizeof(np2tool));
	np2tool.posx = CW_USEDEFAULT;
	np2tool.posy = CW_USEDEFAULT;
	getinifile(path, sizeof(path));
	ini_read(path, ini_title, iniitem, sizeof(iniitem)/sizeof(INITBL));
}

void toolwin_writeini(void) {

	char	path[MAX_PATH];

	getinifile(path, sizeof(path));
	ini_write(path, ini_title, iniitem, sizeof(iniitem)/sizeof(INITBL));
}


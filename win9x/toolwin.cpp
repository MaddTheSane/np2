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
#include	"dialogs.h"
#include	"pccore.h"
#include	"diskdrv.h"


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
	IDC_TOOLRESET,
	IDC_TOOLPOWER,
	IDC_MAXITEMS,

	IDC_BASE				= 3000,

	IDC_SKINDEF				= 3100,
	IDC_SKINSEL				= 3101,
	IDC_TOOLCLOSE			= 3102
};

enum {
	TCTL_STATIC				= 0,
	TCTL_BUTTON				= 1,
	TCTL_DDLIST				= 2
};

typedef struct {
	char	main[MAX_PATH];
	char	font[64];
	SINT32	fontsize;
	UINT32	color1;
	UINT32	color2;
} TOOLSKIN;

typedef struct {
	UINT	tctl;
const char	*text;
	short	posx;
	short	posy;
	short	width;
	short	height;
	short	extend;
	short	padding;
} SUBITEM;

typedef struct {
	HWND			hwnd;
	HBITMAP			hbmp;
	BYTE			fddaccess[2];
	BYTE			hddaccess;
	BYTE			_padding;
	int				winflg;
	int				wingx;
	int				wingy;
	int				wintx;
	int				winty;
	UINT			parentcn;
	int				parentx;
	int				parenty;
	HFONT			hfont;
	HDC				hdcfont;
	HBRUSH			access[2];
	HWND			sub[IDC_MAXITEMS];
	SUBCLASSPROC	subproc[IDC_MAXITEMS];
} TOOLWIN;

enum {
	GTWL_FOCUS		= 0,
	GTWL_SIZE		= 4
};

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

static HBITMAP skinload(const char *path) {

	char	fname[MAX_PATH];
	HBITMAP	ret;

	ZeroMemory(&toolskin, sizeof(toolskin));
	toolskin.fontsize = 12;
	milstr_ncpy(toolskin.font, str_deffont, sizeof(toolskin.font));
	toolskin.color1 = 0x600000;
	toolskin.color2 = 0xff0000;
	CopyMemory(subitem, defsubitem, sizeof(defsubitem));

	if (path) {
		ini_read(path, skintitle, skinini, sizeof(skinini)/sizeof(INITBL));
	}
	if ((path) && (toolskin.main[0])) {
		milstr_ncpy(fname, path, sizeof(fname));
		file_cutname(fname);
		file_catname(fname, toolskin.main, sizeof(fname));
		ret = (HBITMAP)LoadImage(hInst, fname, IMAGE_BITMAP,
													0, 0, LR_LOADFROMFILE);
		if (ret != NULL) {
			return(ret);
		}
	}
	return(LoadBitmap(hInst, "NP2TOOL"));
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

static void setlist(HWND hwnd, const TOOLFDD *fdd, UINT sel) {

	RECT	rc;
	int		width;
	char	basedir[MAX_PATH];
	UINT	i;
const char	*p;
	char	dir[MAX_PATH];
const char	*q;

	SendMessage(hwnd, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	GetClientRect(hwnd, &rc);
	width = rc.right - rc.left - 6;			// border size?
	basedir[0] = '\0';
	if (sel < fdd->cnt) {
		milstr_ncpy(basedir, fdd->name[fdd->pos[sel]], sizeof(basedir));
		file_cutname(basedir);
	}
	for (i=0; i<fdd->cnt; i++) {
		p = fdd->name[fdd->pos[i]];
		milstr_ncpy(dir, p, sizeof(dir));
		file_cutname(dir);
		if (!file_cmpname(basedir, dir)) {
			q = file_getname((char *)p);
		}
		else {
			calctextsize(dir, sizeof(dir), p, width);
			q = dir;
		}
		SendMessage(hwnd, CB_INSERTSTRING, (WPARAM)i, (LPARAM)q);
		p += sizeof(fdd->name[0]);
	}
	if (sel < fdd->cnt) {
		SendMessage(hwnd, CB_SETCURSEL, (WPARAM)sel, (LPARAM)0);
	}
}

static void sellist(UINT drv) {

	HWND	hwnd;
	TOOLFDD	*fdd;
	UINT	sel;

	if (drv >= FDDLIST_DRV) {
		return;
	}
	hwnd = toolwin.sub[fddlist[drv]];
	fdd = np2tool.fdd + drv;
	sel = (UINT)SendMessage(hwnd, CB_GETCURSEL, 0, 0);
	if (sel < fdd->cnt) {
		diskdrv_setfdd(drv, fdd->name[fdd->pos[sel]], 0);
		fdd->insert = 1;
		setlist(hwnd, fdd, sel);
	}
}

static void remakefddlist(HWND hwnd, TOOLFDD *fdd) {

	char	*p;
	UINT	cnt;
	char	*q;
	char	*fname[FDDLIST_MAX];
	UINT	i;
	UINT	j;
	UINT	sel;

	p = fdd->name[0];
	for (cnt=0; cnt<FDDLIST_MAX; cnt++) {
		if (p[0] == '\0') {
			break;
		}
		q = file_getname(p);
		fname[cnt] = q;
		for (i=0; i<cnt; i++) {
			if (file_cmpname(q, fname[fdd->pos[i]]) < 0) {
				break;
			}
		}
		for (j=cnt; j>i; j--) {
			fdd->pos[j] = fdd->pos[j-1];
		}
		fdd->pos[i] = cnt;
		p += sizeof(fdd->name[0]);
	}
	fdd->cnt = cnt;
	sel = (UINT)-1;
	if (fdd->insert) {
		for (i=0; i<cnt; i++) {
			if (fdd->pos[i] == 0) {
				sel = i;
				break;
			}
		}
	}
	setlist(hwnd, fdd, sel);
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

	idc = GetWindowLong(hWnd, GWL_ID) - IDC_BASE;
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
				if ((toolwin.sub[newidc] != NULL) &&
					(subitem[newidc].tctl != TCTL_STATIC)) {
					SetFocus(toolwin.sub[newidc]);
					break;
				}
			} while(idc != newidc);
		}
		else if ((TCHAR)wp == VK_RETURN) {
			if (subitem[idc].tctl == TCTL_BUTTON) {
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
	else if (msg == WM_SETFOCUS) {
		SetWindowLong(GetParent(hWnd), GTWL_FOCUS, idc);
	}
	return(CallWindowProc(toolwin.subproc[idc], hWnd, msg, wp, lp));
}

static void toolwincreate(HWND hWnd) {

	HDC			hdc;
const SUBITEM	*p;
	UINT		i;
	HWND		sub;
const char		*cls;
	DWORD		style;

	toolwin.hfont = CreateFont(toolskin.fontsize, 0, 0, 0, 0, 0, 0, 0,
					SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, FIXED_PITCH, toolskin.font);
    hdc = GetDC(NULL);
	toolwin.hdcfont = CreateCompatibleDC(hdc);
	ReleaseDC(NULL, hdc);
	SelectObject(toolwin.hdcfont, toolwin.hfont);

	toolwin.access[0] = CreateSolidBrush(0x000060);
	toolwin.access[1] = CreateSolidBrush(0x0000ff);

	p = subitem;
	for (i=0; i<IDC_MAXITEMS; i++) {
		sub = NULL;
		cls = NULL;
		switch(p->tctl) {
			case TCTL_STATIC:
				cls = str_static;
				style = 0;
				break;

			case TCTL_BUTTON:
				if (p->extend == 0) {
					cls = str_button;
					style = BS_PUSHBUTTON;
				}
				else if (p->extend == 1) {
					cls = str_button;
					style = BS_OWNERDRAW;
				}
				break;

			case TCTL_DDLIST:
				cls = str_combobox;
				style = CBS_DROPDOWNLIST | WS_VSCROLL;
				break;
		}
		if ((cls) && (p->width > 0) && (p->height > 0)) {
			sub = CreateWindow(cls, p->text, WS_CHILD | WS_VISIBLE | style,
							p->posx, p->posy, p->width, p->height,
							hWnd, (HMENU)(i + IDC_BASE), hInst, NULL);
		}
		toolwin.sub[i] = sub;
		toolwin.subproc[i] = NULL;
		if (sub) {
			toolwin.subproc[i] = (SUBCLASSPROC)GetWindowLong(sub, GWL_WNDPROC);
			SetWindowLong(sub, GWL_WNDPROC, (LONG)twsub);
			SendMessage(sub, WM_SETFONT, (WPARAM)toolwin.hfont,
														MAKELPARAM(TRUE, 0));
		}
		p++;
	}
	for (i=0; i<FDDLIST_DRV; i++) {
		sub = toolwin.sub[fddlist[i]];
		if (sub) {
			DragAcceptFiles(sub, TRUE);
			remakefddlist(sub, np2tool.fdd + i);
		}
	}
	for (i=0; i<IDC_MAXITEMS; i++) {
		if ((toolwin.sub[i]) && (subitem[i].tctl != TCTL_STATIC)) {
			break;
		}
	}
	SetWindowLong(hWnd, GTWL_FOCUS, i);
}

static void toolwindestroy(void) {

	UINT	i;
	HWND	sub;

	if (toolwin.hbmp) {
		for (i=0; i<IDC_MAXITEMS; i++) {
			sub = toolwin.sub[i];
			if (sub) {
				DestroyWindow(sub);
			}
		}
		DeleteObject(toolwin.access[0]);
		DeleteObject(toolwin.access[1]);
		DeleteObject(toolwin.hdcfont);
		DeleteObject(toolwin.hfont);
		DeleteObject(toolwin.hbmp);
		toolwin.hbmp = NULL;
	}
}

static void toolwinpaint(HWND hWnd) {

	HDC			hdc;
	PAINTSTRUCT	ps;
	BITMAP		bmp;
	HDC			hmdc;

	hdc = BeginPaint(hWnd, &ps);
	if (toolwin.hbmp) {
		GetObject(toolwin.hbmp, sizeof(BITMAP), &bmp);
		hmdc = CreateCompatibleDC(hdc);
		SelectObject(hmdc, toolwin.hbmp);
		BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hmdc, 0, 0, SRCCOPY);
		DeleteDC(hmdc);
	}
	EndPaint(hWnd, &ps);
}

static void tooldrawbutton(HWND hWnd, HDC hdc, UINT idc) {

	POINT	pt;
	HWND	sub;
	RECT	rect;
	HDC		hmdc;
	RECT	btn;

	idc -= IDC_BASE;
	if (idc >= IDC_MAXITEMS) {
		return;
	}
	pt.x = 0;
	pt.y = 0;
	ClientToScreen(hWnd, &pt);
	sub = toolwin.sub[idc];
	GetWindowRect(sub, &rect);
	btn.left = 0;
	btn.top = 0;
	btn.right = rect.right - rect.left;
	btn.bottom = rect.bottom - rect.top;
	if (toolwin.hbmp) {
		hmdc = CreateCompatibleDC(hdc);
		SelectObject(hmdc, toolwin.hbmp);
		BitBlt(hdc, 0, 0, btn.right, btn.bottom,
					hmdc, rect.left - pt.x, rect.top - pt.y, SRCCOPY);
		DeleteDC(hmdc);
		if (GetFocus() == sub) {
			DrawFocusRect(hdc, &btn);
		}
	}
}

static void changeskin(HWND hWnd) {

	HBITMAP	hbmp;
	BITMAP	bmp;

	toolwin_movingstart();
	toolwindestroy();
	hbmp = skinload(np2tool.skin);
	if (hbmp == NULL) {
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		return;
	}
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	toolwin.hbmp = hbmp;
	MoveWindow(hWnd, np2tool.posx, np2tool.posy,
											bmp.bmWidth, bmp.bmHeight, TRUE);
	toolwincreate(hWnd);
	toolwin_movingend();
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

	if (toolwin.winflg & 0x03) {
		toolwin.wingx += rect->left - toolwin.wintx;
		rect->left = toolwin.wintx;
		if ((toolwin.wingx >= SNAPDOTREL) || (toolwin.wingx <= -SNAPDOTREL)) {
			toolwin.winflg &= ~0x03;
			rect->left += toolwin.wingx;
			toolwin.wingx = 0;
		}
		rect->right = rect->left + winlx;
	}
	if (toolwin.winflg & 0x0c) {
		toolwin.wingy += rect->top - toolwin.winty;
		rect->top = toolwin.winty;
		if ((toolwin.wingy >= SNAPDOTREL) || (toolwin.wingy <= -SNAPDOTREL)) {
			toolwin.winflg &= ~0x0c;
			rect->top += toolwin.wingy;
			toolwin.wingy = 0;
		}
		rect->bottom = rect->top + winly;
	}

	connectx = ((rect->right >= mainrc.left) && (rect->left <= mainrc.right));
	connecty = ((rect->bottom >= mainrc.top) && (rect->top <= mainrc.bottom));
	if ((!connectx) || (!connecty)) {
		if (toolwin.winflg & 0x01) {
			toolwin.winflg &= ~0x01;
			rect->left += toolwin.wingx;
			rect->right = rect->left + winlx;
			toolwin.wingx = 0;
		}
		if (toolwin.winflg & 0x04) {
			toolwin.winflg &= ~0x04;
			rect->top += toolwin.wingy;
			rect->bottom = rect->top + winly;
			toolwin.wingy = 0;
		}
	}

	do {
		changes = FALSE;
		if ((!(toolwin.winflg & 0x01)) &&
			(rect->bottom >= mainrc.top) && (rect->top <= mainrc.bottom)) {
			do {
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
			} while(0);
			if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
				toolwin.winflg |= 0x01;
				rect->left -= d;
				rect->right = rect->left + winlx;
				toolwin.wingx = d;
				toolwin.wintx = rect->left;
				changes = TRUE;
			}
		}
		if (!(toolwin.winflg & 0x03)) {
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
				toolwin.winflg |= 0x02;
				rect->left -= d;
				rect->right = rect->left + winlx;
				toolwin.wingx = d;
				toolwin.wintx = rect->left;
				changes = TRUE;
			}
		}

		if ((!(toolwin.winflg & 0x04)) &&
			(rect->right >= mainrc.left) && (rect->left <= mainrc.right)) {
			do {
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
			} while(0);
			if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
				toolwin.winflg |= 0x04;
				rect->top -= d;
				rect->bottom = rect->top + winly;
				toolwin.wingy = d;
				toolwin.winty = rect->top;
				changes = TRUE;
			}
		}
		if (!(toolwin.winflg & 0x0c)) {
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
				toolwin.winflg |= 0x08;
				rect->top -= d;
				rect->bottom = rect->top + winly;
				toolwin.wingy = d;
				toolwin.winty = rect->top;
				changes = TRUE;
			}
		}
	} while(changes);
}


// ----

static void open_popup(HWND hWnd, LPARAM lp) {

	HMENU		hMenu;
	POINT		pt;

	hMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING, IDC_SKINDEF, str_skindef);
	AppendMenu(hMenu, MF_STRING, IDC_SKINSEL, str_skinsel);
	AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hMenu, MF_STRING, IDC_TOOLCLOSE, str_toolclose);
	pt.x = LOWORD(lp);
	pt.y = HIWORD(lp);
	ClientToScreen(hWnd, &pt);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
	DestroyMenu(hMenu);
}

static LRESULT CALLBACK twproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	BOOL	r;
	UINT	idc;

	switch(msg) {
		case WM_CREATE:
			SetWindowLong(hWnd, GWL_STYLE,
							GetWindowLong(hWnd, GWL_STYLE) & (~WS_CAPTION));
			SetWindowPos(hWnd, 0, 0, 0, 0, 0,
					SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
			toolwincreate(hWnd);
			break;

		case WM_SYSCOMMAND:
			switch(wp) {
				case IDC_SKINDEF:
					np2tool.skin[0] = '\0';
					changeskin(hWnd);
					sysmng_update(SYS_UPDATEOSCFG);
					break;

				case IDC_SKINSEL:
					soundmng_disable(SNDPROC_TOOL);
					r = dlgs_selectfile(hWnd, &skinui, np2tool.skin,
											sizeof(np2tool.skin), NULL);
					soundmng_enable(SNDPROC_TOOL);
					if (r) {
						changeskin(hWnd);
						sysmng_update(SYS_UPDATEOSCFG);
					}
					break;

				default:
					return(DefWindowProc(hWnd, msg, wp, lp));
			}
			break;

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDC_BASE + IDC_TOOLFDD1LIST:
					if (HIWORD(wp) == CBN_SELCHANGE) {
						sellist(0);
					}
					break;

				case IDC_BASE + IDC_TOOLFDD1BROWSE:
					soundmng_disable(SNDPROC_TOOL);
					dialog_changefdd(hWnd, 0);
					soundmng_enable(SNDPROC_TOOL);
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
					soundmng_disable(SNDPROC_TOOL);
					dialog_changefdd(hWnd, 1);
					soundmng_enable(SNDPROC_TOOL);
					break;

				case IDC_BASE + IDC_TOOLFDD2EJECT:
					diskdrv_setfdd(1, NULL, 0);
					toolwin_setfdd(1, NULL);
					break;

				case IDC_BASE + IDC_TOOLRESET:
					SendMessage(hWndMain, WM_COMMAND, IDM_RESET, 0);
					SetForegroundWindow(hWndMain);
					break;

				case IDC_BASE + IDC_TOOLPOWER:
					SendMessage(hWndMain, WM_CLOSE, 0, 0L);
					break;

				case IDC_SKINDEF:
				case IDC_SKINSEL:
					SendMessage(hWnd, WM_SYSCOMMAND, wp, lp);
					break;

				case IDC_TOOLCLOSE:
					SendMessage(hWnd, WM_CLOSE, 0, 0);
					break;
			}
			break;

		case WM_KEYDOWN:						// TAB‚ð‰Ÿ‚µ‚½Žž‚É•œ‹A
			if ((TCHAR)wp == VK_TAB) {
				idc = GetWindowLong(hWnd, GTWL_FOCUS);
				if (idc < IDC_MAXITEMS) {
					SetFocus(toolwin.sub[idc]);
				}
				return(0);
			}
			else if ((TCHAR)wp == VK_ESCAPE) {
				
				return(0);
			}
			break;

		case WM_PAINT:
			toolwinpaint(hWnd);
			break;

		case WM_DRAWITEM:
			tooldrawbutton(hWnd, ((LPDRAWITEMSTRUCT)lp)->hDC, wp);
			break;

		case WM_ENTERMENULOOP:
			soundmng_disable(SNDPROC_TOOL);
			break;

		case WM_EXITMENULOOP:
			soundmng_enable(SNDPROC_TOOL);
			break;

		case WM_ENTERSIZEMOVE:
			soundmng_disable(SNDPROC_TOOL);
			movingstart();
			break;

		case WM_EXITSIZEMOVE:
			soundmng_enable(SNDPROC_TOOL);
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

		case WM_RBUTTONDOWN:
			open_popup(hWnd, lp);
			break;

		default:
			return(DefWindowProc(hWnd, msg, wp, lp));
	}
	return(0);
}

BOOL toolwin_initapp(HINSTANCE hInstance) {

	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = twproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = GTWL_SIZE;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = np2toolclass;
	return(RegisterClass(&wc));
}

void toolwin_open(void) {

	HBITMAP	hbmp;
	BITMAP	bmp;
	HWND	hWnd;
	HMENU	hMenu;

	if (toolwin.hwnd) {
		return;
	}
	ZeroMemory(&toolwin, sizeof(toolwin));
	hbmp = skinload(np2tool.skin);
	if (hbmp == NULL) {
		goto twope_err1;
	}
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	toolwin.hbmp = hbmp;
	hWnd = CreateWindow(np2toolclass, np2tooltitle, WS_SYSMENU,
							np2tool.posx, np2tool.posy,
							bmp.bmWidth, bmp.bmHeight,
							NULL, NULL, hInst, NULL);
	toolwin.hwnd = hWnd;
	if (hWnd == NULL) {
		goto twope_err2;
	}
	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOW);

	hMenu = GetSystemMenu(hWnd, FALSE);
	InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, IDC_SKINDEF, str_skindef);
	InsertMenu(hMenu, 1, MF_BYPOSITION | MF_STRING, IDC_SKINSEL, str_skinsel);
	InsertMenu(hMenu, 2, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
	SetForegroundWindow(hWndMain);
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
	UINT	connect;

	if (toolwin.hwnd == NULL) {
		return;
	}
	GetWindowRect(hWndMain, &mainrc);
	GetWindowRect(toolwin.hwnd, &toolrc);
	connect = 0;
	if ((toolrc.bottom >= mainrc.top) && (toolrc.top <= mainrc.bottom)) {
		if (toolrc.right == mainrc.left) {
			connect += 0x01;
		}
		else if (toolrc.left == mainrc.right) {
			connect += 0x02;
		}
		else if (toolrc.left == mainrc.left) {
			connect += 0x03;
		}
		else if (toolrc.right == mainrc.right) {
			connect += 0x04;
		}
	}
	if ((toolrc.right >= mainrc.left) && (toolrc.left <= mainrc.right)) {
		if (toolrc.bottom == mainrc.top) {
			connect += 1 << 4;
		}
		else if (toolrc.top == mainrc.bottom) {
			connect += 2 << 4;
		}
		else if (toolrc.top == mainrc.top) {
			connect += 3 << 4;
		}
		else if (toolrc.bottom == mainrc.bottom) {
			connect += 4 << 4;
		}
	}
	toolwin.parentcn = connect;
	toolwin.parentx = mainrc.left;
	toolwin.parenty = mainrc.top;
}

void toolwin_movingend(void) {

	UINT	connect;
	RECT	mainrc;
	RECT	toolrc;
	int		cx;
	int		cy;

	connect = toolwin.parentcn;
	toolwin.parentcn = 0;
	if ((toolwin.hwnd == NULL) || (!connect)) {
		return;
	}
	GetWindowRect(hWndMain, &mainrc);
	GetWindowRect(toolwin.hwnd, &toolrc);
	cx = toolrc.right - toolrc.left;
	cy = toolrc.bottom - toolrc.top;
	toolrc.left += mainrc.left - toolwin.parentx;
	toolrc.top += mainrc.top - toolwin.parenty;
	switch(connect & 0x0f) {
		case 1:
			toolrc.left = mainrc.left - cx;
			break;

		case 2:
			toolrc.left = mainrc.right;
			break;

		case 3:
			toolrc.left = mainrc.left;
			break;

		case 4:
			toolrc.left = mainrc.right - cx;
			break;
	}
	switch((connect >> 4) & 0x0f) {
		case 1:
			toolrc.top = mainrc.top - cy;
			break;

		case 2:
			toolrc.top = mainrc.bottom;
			break;

		case 3:
			toolrc.top = mainrc.top;
			break;

		case 4:
			toolrc.top = mainrc.bottom - cy;
			break;
	}
	MoveWindow(toolwin.hwnd, toolrc.left, toolrc.top, cx, cy, TRUE);
}

void toolwin_setfdd(BYTE drv, const char *name) {

	TOOLFDD	*fdd;
	char	*q;
	char	*p;
	UINT	i;
	HWND	sub;

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
		sub = toolwin.sub[fddlist[drv]];
		if (sub) {
			remakefddlist(sub, fdd);
			SetForegroundWindow(hWndMain);
		}
	}
}

static void setdiskacc(UINT num, BYTE count) {

const DISKACC	*acc;
	HWND		sub;

	if (toolwin.hwnd == NULL) {
		return;
	}
	if (num < (sizeof(diskacc)/sizeof(DISKACC))) {
		acc = diskacc + num;
		sub = NULL;
		if (*(acc->counter) == 0) {
			sub = toolwin.sub[acc->idc];
		}
		*(acc->counter) = count;
		if (sub) {
			InvalidateRect(sub, NULL, TRUE);
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
	HWND		sub;

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
				sub = toolwin.sub[acc->idc];
				if (sub) {
					InvalidateRect(sub, NULL, TRUE);
				}
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
	{"SkinFile", INITYPE_STR,		np2tool.skin,			MAX_PATH},
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


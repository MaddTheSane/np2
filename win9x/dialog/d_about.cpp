#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"np2class.h"
#include	"dialog.h"
#include	"dialogs.h"
#include	"np2ver.h"
#include	"pccore.h"
#include	"np2info.h"


static	SIZE	aboutsize;

#if !defined(SUPPORT_PC9821)
static const OEMCHAR str_np2title[] = OEMTEXT("Neko Project II  ");
#else
static const OEMCHAR str_np2title[] = OEMTEXT("Neko Project 21  ");
#endif
static const OEMCHAR np2infostr[] = OEMTEXT("CPU: %CPU% %CLOCK%\nMEM: %MEM1%\nGDC: %GDC%\n     %GDC2%\nTEXT: %TEXT%\nGRPH: %GRPH%\nSOUND: %EXSND%\n\nBIOS: %BIOS%\nRHYTHM: %RHYTHM%\n\nSCREEN: %DISP%");


static void about_init(HWND hWnd) {

	OEMCHAR	work[128];
	RECT	rectwindow;
	RECT	rectclient;
	POINT	pt;
	RECT	parent;

	milstr_ncpy(work, str_np2title, NELEMENTS(work));
	milstr_ncat(work, np2version, NELEMENTS(work));
#if defined(NP2VER_WIN9X)
	milstr_ncat(work, NP2VER_WIN9X, NELEMENTS(work));
#endif
	SetDlgItemText(hWnd, IDC_NP2VER, work);
	GetWindowRect(hWnd, &rectwindow);
	GetClientRect(hWnd, &rectclient);
	aboutsize.cx = rectwindow.right - rectwindow.left;
	aboutsize.cy = rectwindow.bottom - rectwindow.top;
	pt.x = 0;
	pt.y = 0;
	ClientToScreen(GetParent(hWnd), &pt);
	GetClientRect(GetParent(hWnd), &parent);
	np2class_move(hWnd,
					pt.x + ((parent.right - parent.left - aboutsize.cx) / 2),
					pt.y + ((parent.bottom - parent.top - aboutsize.cy) / 2),
					aboutsize.cx,
					aboutsize.cy + 60 - (rectclient.bottom - rectclient.top));

	SetFocus(GetDlgItem(hWnd, IDOK));
}

static void about_more(HWND hWnd) {

	OEMCHAR	infostr[1024];
	RECT	rect;

	np2info(infostr, np2infostr, NELEMENTS(infostr), NULL);
	SetDlgItemText(hWnd, IDC_NP2INFO, infostr);
	EnableWindow(GetDlgItem(hWnd, IDC_MORE), FALSE);
	GetWindowRect(hWnd, &rect);
	np2class_move(hWnd, rect.left, rect.top, aboutsize.cx, aboutsize.cy);
	SetFocus(GetDlgItem(hWnd, IDOK));
}

LRESULT CALLBACK AboutDialogProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_INITDIALOG:
			about_init(hWnd);
			return(FALSE);

		case WM_COMMAND:
			switch (LOWORD(wp)) {
				case IDOK:
					EndDialog(hWnd, IDOK);
					break;

				case IDC_MORE:
					about_more(hWnd);
					break;

				default:
					return(FALSE);
			}
			break;

		case WM_CLOSE:
			PostMessage(hWnd, WM_COMMAND, IDOK, 0);
			break;

		default:
			return(FALSE);
	}
	return(TRUE);
}


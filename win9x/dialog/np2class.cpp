#include	"compiler.h"
#include	<prsht.h>
#include	"resource.h"
#include	"winloc.h"
#include	"np2class.h"


const char np2dlgclass[] = "np2dialog";


void np2class_initialize(HINSTANCE hinst) {

	WNDCLASS	wc;

	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_SAVEBITS | CS_DBLCLKS;
	wc.lpfnWndProc = DefDlgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.hInstance = hinst;
	wc.hIcon = LoadIcon(hinst, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = np2dlgclass;
	RegisterClass(&wc);
}

void np2class_deinitialize(HINSTANCE hinst) {

	UnregisterClass(np2dlgclass, hinst);
}


// ---- 

void np2class_move(HWND hWnd, int posx, int posy, int cx, int cy) {

	RECT	workrc;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrc, 0);

	if (workrc.right < (posx + cx)) {
		posx = workrc.right - cx;
	}
	if (workrc.left > posx) {
		posx = workrc.left;
	}
	if (workrc.bottom < (posy + cy)) {
		posy = workrc.bottom - cy;
	}
	if (workrc.top > posy) {
		posy = workrc.top;
	}
	MoveWindow(hWnd, posx, posy, cx, cy, TRUE);
}

// ----

int CALLBACK np2class_propetysheet(HWND hWndDlg, UINT uMsg, LPARAM lParam) {

	if (uMsg == PSCB_INITIALIZED) {
		SetWindowLong(hWndDlg, GWL_EXSTYLE,
				GetWindowLong(hWndDlg, GWL_EXSTYLE) & (~WS_EX_CONTEXTHELP));
	}
	return(0);
}


// ----

void np2class_windowtype(HWND hWnd, BYTE type) {

	RECT		rect;
	DWORD		style;
	HMENU		hmenu;

	GetClientRect(hWnd, &rect);
	style = GetWindowLong(hWnd, GWL_STYLE);
	hmenu = (HMENU)GetWindowLong(hWnd, NP2GWL_HMENU);
	if (!(type & 1)) {
		style |= WS_CAPTION;
		if (hmenu) {
			SetMenu(hWnd, hmenu);
			hmenu = NULL;
		}
	}
	else {
		style &= ~WS_CAPTION;
		if (hmenu == NULL) {
			hmenu = GetMenu(hWnd);
			SetMenu(hWnd, NULL);
		}
	}
	SetWindowLong(hWnd, GWL_STYLE, style);
	SetWindowLong(hWnd, NP2GWL_HMENU, (LONG)hmenu);
	SetWindowPos(hWnd, 0, 0, 0, 0, 0,
					SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
	winloc_setclientsize(hWnd,
							rect.right - rect.left, rect.bottom - rect.top);
}

HMENU np2class_gethmenu(HWND hWnd) {

	HMENU	ret;

	ret = (HMENU)GetWindowLong(hWnd, NP2GWL_HMENU);
	if (ret == NULL) {
		ret = GetMenu(hWnd);
	}
	return(ret);
}

void np2class_destroymenu(HWND hWnd) {

	HMENU	hmenu;

	hmenu = (HMENU)GetWindowLong(hWnd, NP2GWL_HMENU);
	if (hmenu != NULL) {
		DestroyMenu(hmenu);
		SetWindowLong(hWnd, NP2GWL_HMENU, 0);
	}
}


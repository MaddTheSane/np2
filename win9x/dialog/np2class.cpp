#include	"compiler.h"
#include	"resource.h"


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


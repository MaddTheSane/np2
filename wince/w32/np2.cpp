#include	"compiler.h"
#include	<time.h>
#include	"resource.h"
#include	"strres.h"
#include	"np2.h"
#include	"dosio.h"
#include	"commng.h"
#include	"joymng.h"
#include	"mousemng.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"winkbd.h"
#include	"ini.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"pc9861k.h"
#include	"mpu98ii.h"
#include	"bios.h"
#include	"scrndraw.h"
#include	"sound.h"
#include	"beep.h"
#include	"s98.h"
#include	"diskdrv.h"
#include	"fddfile.h"
#include	"font.h"
#include	"timing.h"
#include	"debugsub.h"


static	const char	szAppCaption[] = STRLITERAL("Neko Project II");
static	const char	szClassName[] = STRLITERAL("NP2-MainWindow");


		NP2OSCFG	np2oscfg = {0, 2};
		HWND		hWndMain;
		HINSTANCE	hInst;
		HINSTANCE	hPrev;
		char		modulefile[MAX_PATH];

static	UINT		framecnt;
static	UINT		waitcnt;
static	UINT		framemax = 1;


// ---- proc

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT	ps;
	HDC			hdc;

	switch (msg) {
		case WM_CREATE:
//			WINNLSEnableIME(hWnd, FALSE);
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			scrndraw_redraw();
			EndPaint(hWnd, &ps);
			break;

		case WM_KEYDOWN:
			if (wParam == VK_F11) {
				return(DefWindowProc(hWnd, WM_SYSKEYDOWN, VK_F10, lParam));
			}
			winkeydown106(wParam, lParam);
			break;

		case WM_KEYUP:
			winkeyup106(wParam, lParam);
			break;

		case WM_SYSKEYDOWN:
			winkeydown106(wParam, lParam);
			break;

		case WM_SYSKEYUP:
			winkeyup106(wParam, lParam);
			break;

		case WM_ENTERSIZEMOVE:
			soundmng_stop();
			break;

		case WM_EXITSIZEMOVE:
			soundmng_play();
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_NP2CMD:
			switch(LOWORD(lParam)) {
				case NP2CMD_EXIT:
				case NP2CMD_EXIT2:
					PostQuitMessage(0);
					break;

				case NP2CMD_RESET:
					pccore_cfgupdate();
					pccore_reset();
					break;
			}
			break;

		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return(0L);
}


static void processwait(UINT waitcnt) {

	if (timing_getcount() >= waitcnt) {
		timing_setcount(0);
		framecnt = 0;
	}
	else {
		Sleep(1);
	}
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
										LPSTR lpszCmdLine, int nCmdShow) {
	WNDCLASS	np2;
	MSG			msg;
	HWND		hwndorg;

	GetModuleFileName(NULL, modulefile, sizeof(modulefile));
	dosio_init();
	file_setcd(modulefile);
	initload();

	srand((unsigned)time(NULL));

	if ((hwndorg = FindWindow(szClassName, NULL)) != NULL) {
		ShowWindow(hwndorg, SW_RESTORE);
		SetForegroundWindow(hwndorg);
		dosio_term();
		return(FALSE);
	}

	hInst = hInstance;
	hPrev = hPreInst;
	TRACEINIT();

	keystat_reset();

	if (!hPreInst) {
		np2.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
		np2.lpfnWndProc = WndProc;
		np2.cbClsExtra = 0;
		np2.cbWndExtra = 0;
		np2.hInstance = hInstance;
		np2.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
		np2.hCursor = LoadCursor(NULL, IDC_ARROW);
		np2.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		np2.lpszMenuName = NULL;
		np2.lpszClassName = szClassName;
		if (!RegisterClass(&np2)) {
			return(FALSE);
		}
	}

	hWndMain = CreateWindowEx(0, szClassName, szAppCaption,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						0, 0, 320, 240,
						NULL, NULL, hInstance, NULL);

	scrnmng_initialize();

	ShowWindow(hWndMain, nCmdShow);
	UpdateWindow(hWndMain);

	if (scrnmng_create(hWndMain, 320, 240) != SUCCESS) {
		MessageBox(hWndMain, "Couldn't create DirectDraw Object",
									szAppCaption, MB_OK | MB_ICONSTOP);
		return(FALSE);
	}

	commng_initialize();
	sysmng_initialize();
	pccore_init();
	S98_init();

	scrndraw_redraw();
	pccore_reset();

	while(1) {
		if (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)) {
			if (!GetMessage(&msg, NULL, 0, 0)) {
				break;
			}
			if ((msg.message != WM_SYSKEYDOWN) &&
				(msg.message != WM_SYSKEYUP)) {
				TranslateMessage(&msg);
			}
			DispatchMessage(&msg);
		}
		else {
			if (np2oscfg.NOWAIT) {
				pccore_exec(framecnt == 0);
				if (np2oscfg.DRAW_SKIP) {			// nowait frame skip
					framecnt++;
					if (framecnt >= np2oscfg.DRAW_SKIP) {
						processwait(0);
					}
				}
				else {							// nowait auto skip
					framecnt = 1;
					if (timing_getcount()) {
						processwait(0);
					}
				}
			}
			else if (np2oscfg.DRAW_SKIP) {		// frame skip
				if (framecnt < np2oscfg.DRAW_SKIP) {
					pccore_exec(framecnt == 0);
					framecnt++;
				}
				else {
					processwait(np2oscfg.DRAW_SKIP);
				}
			}
			else {								// auto skip
				if (!waitcnt) {
					UINT cnt;
					pccore_exec(framecnt == 0);
					framecnt++;
					cnt = timing_getcount();
					if (framecnt > cnt) {
						waitcnt = framecnt;
						if (framemax > 1) {
							framemax--;
						}
					}
					else if (framecnt >= framemax) {
						if (framemax < 12) {
							framemax++;
						}
						if (cnt >= 12) {
							timing_reset();
						}
						else {
							timing_setcount(cnt - framecnt);
						}
						framecnt = 0;
					}
				}
				else {
					processwait(waitcnt);
					waitcnt = framecnt;
				}
			}
		}
	}

	pccore_cfgupdate();
	pccore_term();
	S98_trash();

	scrnmng_destroy();

	if (sys_updates	& (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
		initsave();
	}
	TRACETERM();
	dosio_term();

	return(msg.wParam);
}


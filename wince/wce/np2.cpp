#include	"compiler.h"
#include	<gx.h>
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
#include	"taskmng.h"
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
#include	"statsave.h"
#include	"vramhdl.h"
#include	"menubase.h"
#include	"sysmenu.h"
#include	"debugsub.h"


static const TCHAR szAppCaption[] = STRLITERAL("Neko Project II");
static const TCHAR szClassName[] = STRLITERAL("NP2-MainWindow");


		NP2OSCFG	np2oscfg = {0, 2, 0, 0};
		HWND		hWndMain;
		HINSTANCE	hInst;
		HINSTANCE	hPrev;
		char		modulefile[MAX_PATH];

static	BOOL		sysrunning;
static	UINT		framecnt;
static	UINT		waitcnt;
static	UINT		framemax = 1;


// ---- resume

static void getstatfilename(char *path, const char *ext, int size) {

	file_cpyname(path, modulefile, size);
	file_cutext(path);
	file_catname(path, str_dot, size);
	file_catname(path, ext, size);
}

static int flagsave(const char *ext) {

	int		ret;
	char	path[MAX_PATH];

	getstatfilename(path, ext, sizeof(path));
	ret = statsave_save(path);
	if (ret) {
		file_delete(path);
	}
	return(ret);
}

static void flagdelete(const char *ext) {

	char	path[MAX_PATH];

	getstatfilename(path, ext, sizeof(path));
	file_delete(path);
}

static int flagload(const char *ext, const char *title, BOOL force) {

	int		ret;
	int		id;
	char	path[MAX_PATH];
	char	buf[1024];
	char	buf2[1024 + 256];

	getstatfilename(path, ext, sizeof(path));
	id = DID_YES;
	ret = statsave_check(path, buf, sizeof(buf));
	if (ret & (~NP2FLAG_DISKCHG)) {
		menumbox("Couldn't restart", title, MBOX_OK | MBOX_ICONSTOP);
		id = DID_NO;
	}
	else if ((!force) && (ret & NP2FLAG_DISKCHG)) {
		SPRINTF(buf2, "Conflict!\n\n%s\nContinue?", buf);
		id = menumbox(buf2, title, MBOX_YESNOCAN | MBOX_ICONQUESTION);
	}
	if (id == DID_YES) {
		statsave_load(path);
	}
	return(id);
}


// ---- proc

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT	ps;
	HDC			hdc;

	switch (msg) {
		case WM_CREATE:
			ImmAssociateContext(hWnd, NULL);
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			if (!sysrunning) {
				scrnmng_clear(TRUE);
			}
			else {
				scrndraw_redraw();
			}
			EndPaint(hWnd, &ps);
			break;

		case WM_KEYDOWN:
			if (wParam == VK_F11) {
				if (menuvram == NULL) {
					sysmenu_menuopen(0, 0, 0);
				}
				else {
					menubase_close();
				}
			}
			else {
				winkbd_keydown(wParam, lParam);
			}
			break;

		case WM_KEYUP:
			winkbd_keyup(wParam, lParam);
			break;
#if 0
		case WM_SYSKEYDOWN:
			winkbd_keydown(wParam, lParam);
			break;

		case WM_SYSKEYUP:
			winkbd_keyup(wParam, lParam);
			break;
#endif
		case WM_MOUSEMOVE:
			if (scrnmng_mousepos(&lParam) == SUCCESS) {
				if (menuvram == NULL) {
				}
				else {
					menubase_moving(LOWORD(lParam), HIWORD(lParam), 0);
				}
			}
			break;

		case WM_LBUTTONDOWN:
			if (scrnmng_mousepos(&lParam) == SUCCESS) {
				if (menuvram == NULL) {
				}
				else {
					menubase_moving(LOWORD(lParam), HIWORD(lParam), 1);
				}
			}
			break;

		case WM_LBUTTONUP:
			if (scrnmng_mousepos(&lParam) == SUCCESS) {
				if (menuvram == NULL) {
					sysmenu_menuopen(0, 0, 0);
				}
				else {
					menubase_moving(LOWORD(lParam), HIWORD(lParam), 2);
				}
			}
			break;

		case WM_CLOSE:
			break;

		case WM_DESTROY:
//			PostQuitMessage(0);
			break;

		case WM_ACTIVATE:
			if (sysrunning) {
				if (LOWORD(wParam) != WA_INACTIVE) {
					GXResume();
					scrnmng_enable(TRUE);
					scrndraw_redraw();
					soundmng_enable(SNDPROC_MAIN);
				}
				else {
					soundmng_disable(SNDPROC_MAIN);
					scrnmng_enable(FALSE);
					GXSuspend();
				}
			}
			break;

#if defined(WAVEMNG_CBMAIN)
		case MM_WOM_DONE:
			soundmng_cb(MM_WOM_DONE, (HWAVEOUT)wParam, (WAVEHDR *)lParam);
			break;
#endif

		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return(0L);
}


#define	framereset(cnt)		framecnt = 0

static void processwait(UINT cnt) {

	if (timing_getcount() >= cnt) {
		timing_setcount(0);
		framereset(cnt);
	}
	else {
		Sleep(1);
	}
}

static DWORD GetModuleFileName_A(HMODULE hModule,
								LPSTR lpFileName, DWORD nSize) {

	TCHAR	*FileNameW;
	DWORD	len;

	if (nSize) {
		FileNameW = (TCHAR *)malloc(nSize * sizeof(TCHAR));
		if (FileNameW) {
			len = GetModuleFileName(hModule, FileNameW, nSize);
			nSize = WideCharToMultiByte(CP_ACP, 0, FileNameW, -1,
										lpFileName, nSize, NULL, NULL);
			if (nSize) {
				nSize--;
			}
			free(FileNameW);
		}
		else {
			nSize = 0;
		}
	}
	return(nSize);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
										LPTSTR lpszCmdLine, int nCmdShow) {

	HWND		hWnd;
	WNDCLASS	np2;
	int			id;
	MSG			msg;

	hWnd = FindWindow(szClassName, NULL);
	if (hWnd != NULL) {
		ShowWindow(hWnd, SW_SHOW);
#if defined(WIN32_PLATFORM_PSPC)
		SetForegroundWindow((HWND)((ULONG)hWnd | 1));
#else
		SetForegroundWindow(hWnd);
#endif
		return(0);
	}

	GetModuleFileName_A(NULL, modulefile, MAX_PATH);
	dosio_init();
	file_setcd(modulefile);
	initload();

//	srand((unsigned)time(NULL));

	hInst = hInstance;
	hPrev = hPreInst;
	TRACEINIT();

	keystat_reset();

//	if (!hPreInst) {
		np2.style = CS_HREDRAW | CS_VREDRAW;
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
//	}

#if !defined(WIN32_PLATFORM_PSPC)
	hWnd = CreateWindow(szClassName, szAppCaption,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						0, 0, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT,
						NULL, NULL, hInstance, NULL);
#else
	hWnd = CreateWindow(szClassName, szAppCaption,
						WS_VISIBLE,
						0, 0, 
		      			GetSystemMetrics(SM_CXSCREEN),
		      			GetSystemMetrics(SM_CYSCREEN),
						NULL, NULL, hInstance, NULL);
#endif
	hWndMain = hWnd;
	if (hWnd == NULL) {
		goto np2main_err1;
	}

	scrnmng_initialize();

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	if (sysmenu_create() != SUCCESS) {
		DestroyWindow(hWnd);
		goto np2main_err1;
	}
	if (scrnmng_create(hWnd, FULLSCREEN_WIDTH, FULLSCREEN_HEIGHT)
																!= SUCCESS) {
		MessageBox(hWnd, STRLITERAL("Couldn't create DirectDraw Object"),
									szAppCaption, MB_OK | MB_ICONSTOP);
		DestroyWindow(hWnd);
		goto np2main_err2;
	}

	if (GXOpenInput() == 0) {
		DestroyWindow(hWnd);
		goto np2main_err3;
	}

	soundmng_initialize();
	commng_initialize();
	sysmng_initialize();
	taskmng_initialize();
	pccore_init();
	S98_init();

	pccore_reset();
	scrndraw_redraw();

	sysrunning = TRUE;

	if (np2oscfg.resume) {
		id = flagload(str_sav, str_resume, FALSE);
		if (id == DID_CANCEL) {
			DestroyWindow(hWnd);
			goto np2main_err4;
		}
	}

	while(taskmng_isavail()) {
		if (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)) {
			if (!GetMessage(&msg, NULL, 0, 0)) {
				break;
			}
#if 1
			TranslateMessage(&msg);
#else
			if ((msg.hwnd != hWnd) ||
				((msg.message != WM_SYSKEYDOWN) &&
				(msg.message != WM_SYSKEYUP))) {
				TranslateMessage(&msg);
			}
#endif
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
						framereset(0);
					}
				}
				else {
					processwait(waitcnt);
					waitcnt = framecnt;
				}
			}
		}
	}

	soundmng_disable(SNDPROC_MAIN);
	sysrunning = FALSE;

	DestroyWindow(hWnd);

	pccore_cfgupdate();
	if (np2oscfg.resume) {
		flagsave(str_sav);
	}
	else {
		flagdelete(str_sav);
	}
	pccore_term();
	S98_trash();
	soundmng_deinitialize();

	GXCloseInput();
	scrnmng_destroy();
	sysmenu_destroy();

	if (sys_updates	& (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
		initsave();
	}
	TRACETERM();
	dosio_term();

	return(msg.wParam);

np2main_err4:
	pccore_cfgupdate();
	pccore_term();
	S98_trash();
	soundmng_deinitialize();
	GXCloseInput();

np2main_err3:
	scrnmng_destroy();

np2main_err2:
	sysmenu_destroy();

np2main_err1:
	TRACETERM();
	dosio_term();
	return(FALSE);
}


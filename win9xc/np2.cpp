#include	"compiler.h"
#include	<time.h>
#ifndef __GNUC__
#include	<winnls32.h>
#endif
#include	"strres.h"
#include	"parts.h"
#include	"resource.h"
#include	"np2.h"
#include	"np2arg.h"
#include	"dosio.h"
#include	"commng.h"
#include	"joymng.h"
#include	"mousemng.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"

#include	"timing.h"

#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"

#include	"scrndraw.h"
#include	"sound.h"

#include	"ini.h"
#include	"menu.h"
#include	"dialog.h"
#include	"mpu98ii.h"
#include	"fddfile.h"
#include	"diskdrv.h"
#include	"s98.h"
#include	"font.h"
#include	"pc9861k.h"
#include	"winkbd.h"
#include	"debugsub.h"
#include	"beep.h"


#ifdef BETA_RELEASE
#define	OPENING_WAIT	1500
#endif

#define	STATSAVEMAX		10

static	char		np2help[] = "np2.hlp";
// static char		np2resume[] = "sav";

static	const char	szAppCaption[] = "Neko Project II - C version";
static	const char	szClassName[] = "NP2-MainWindow";


		NP2OSCFG	np2oscfg = {
						CW_USEDEFAULT, CW_USEDEFAULT, 0, 2,
						KEY_UNKNOWN, 0,
						0, 0, 0, {1, 2, 2, 1},
						0};

		HWND		hWndMain;
		HINSTANCE	hInst;
		HINSTANCE	hPrev;
		char		modulefile[MAX_PATH];
		char		fddfolder[MAX_PATH];
		char		hddfolder[MAX_PATH];
		char		bmpfilefolder[MAX_PATH];

static	UINT		framecnt;
static	UINT		waitcnt;
static	UINT		framemax = 1;
static	int			np2opening = 1;
static	int			np2quitmsg = 0;


static	BYTE	scrnmode;

static void changescreen(BYTE newmode) {

	BYTE	change;
	BYTE	renewal;

	change = scrnmode ^ newmode;
	renewal = (change & SCRNMODE_FULLSCREEN);
	if (newmode & SCRNMODE_FULLSCREEN) {
		renewal |= (change & SCRNMODE_HIGHCOLOR);
	}
	else {
		renewal |= (change & SCRNMODE_ROTATEMASK);
	}
	if (renewal) {
		soundmng_stop();
		mouse_running(MOUSE_STOP);
		scrnmng_destroy();
		if (scrnmng_create(newmode) == SUCCESS) {
			scrnmode = newmode;
		}
		else {
			if (scrnmng_create(scrnmode) != SUCCESS) {
				PostQuitMessage(0);
				return;
			}
		}
		scrndraw_redraw();
		mouse_running(MOUSE_CONT);
		soundmng_play();
	}
	else {
		scrnmode = newmode;
	}
}


// ---- proc

static void winuienter(void) {

	soundmng_stop();
	scrnmng_topwinui();
}

static void winuileave(void) {

	scrnmng_clearwinui();
	soundmng_play();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT	ps;
	RECT		rc;
	HDC			hdc;
	UINT		update;

	switch (msg) {
		case WM_CREATE:
#ifndef __GNUC__
			WINNLSEnableIME(hWnd, FALSE);
#endif
			break;

		case WM_SYSCOMMAND:
			update = 0;
			switch(wParam) {
				case IDM_MEMORYDUMP:
					debugsub_memorydump();
					break;

				default:
					return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			sysmng_update(update);
			break;

		case WM_COMMAND:
			update = 0;
			switch(LOWORD(wParam)) {
				case IDM_RESET:
					pccore_cfgupdate();
					pccore_reset();
					break;

				case IDM_CONFIG:
					winuienter();
					DialogBox(hInst, MAKEINTRESOURCE(IDD_CONFIG),
									hWnd, (DLGPROC)CfgDialogProc);
					winuileave();
					break;

				case IDM_NEWDISK:
					winuienter();
					dialog_newdisk(hWnd);
					winuileave();
					break;

				case IDM_CHANGEFONT:
					winuienter();
					dialog_font(hWnd);
					winuileave();
					break;

				case IDM_EXIT:
					SendMessage(hWnd, WM_CLOSE, 0, 0L);
					break;

				case IDM_FDD1OPEN:
					winuienter();
					dialog_changefdd(hWnd, 0);
					winuileave();
					break;

				case IDM_FDD1EJECT:
					diskdrv_setfdd(0, NULL, 0);
					break;

				case IDM_FDD2OPEN:
					winuienter();
					dialog_changefdd(hWnd, 1);
					winuileave();
					break;

				case IDM_FDD2EJECT:
					diskdrv_setfdd(1, NULL, 0);
					break;

				case IDM_SASI1OPEN:
					winuienter();
					dialog_changehdd(hWnd, 0);
					winuileave();
					break;

				case IDM_SASI1EJECT:
					diskdrv_sethdd(0, NULL);
					break;

				case IDM_SASI2OPEN:
					winuienter();
					dialog_changehdd(hWnd, 1);
					winuileave();
					break;

				case IDM_SASI2EJECT:
					diskdrv_sethdd(1, NULL);
					break;

				case IDM_WINDOW:
					changescreen(scrnmode & (~SCRNMODE_FULLSCREEN));
					break;

				case IDM_FULLSCREEN:
					changescreen(scrnmode | SCRNMODE_FULLSCREEN);
					break;

				case IDM_ROLNORMAL:
					xmenu_setroltate(0);
					changescreen(scrnmode & (~SCRNMODE_ROTATEMASK));
					break;

				case IDM_ROLLEFT:
					xmenu_setroltate(1);
					changescreen((scrnmode & (~SCRNMODE_ROTATEMASK)) |
														SCRNMODE_ROTATELEFT);
					break;

				case IDM_ROLRIGHT:
					xmenu_setroltate(2);
					changescreen((scrnmode & (~SCRNMODE_ROTATEMASK)) |
														SCRNMODE_ROTATERIGHT);
					break;

				case IDM_DISPSYNC:
					xmenu_setdispmode(np2cfg.DISPSYNC ^ 1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_RASTER:
					xmenu_setraster(np2cfg.RASTER ^ 1);
					if (np2cfg.RASTER) {
						changescreen(scrnmode | SCRNMODE_HIGHCOLOR);
					}
					else {
						changescreen(scrnmode & (~SCRNMODE_HIGHCOLOR));
					}
					update |= SYS_UPDATECFG;
					break;

				case IDM_NOWAIT:
					xmenu_setwaitflg(np2oscfg.NOWAIT ^ 1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_AUTOFPS:
					xmenu_setframe(0);
					update |= SYS_UPDATECFG;
					break;

				case IDM_60FPS:
					xmenu_setframe(1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_30FPS:
					xmenu_setframe(2);
					update |= SYS_UPDATECFG;
					break;

				case IDM_20FPS:
					xmenu_setframe(3);
					update |= SYS_UPDATECFG;
					break;

				case IDM_15FPS:
					xmenu_setframe(4);
					update |= SYS_UPDATECFG;
					break;

				case IDM_SCREENOPT:
					winuienter();
					dialog_scropt(hWnd);
					winuileave();
					break;

				case IDM_KEY:
					xmenu_setkey(0);
					update |= SYS_UPDATECFG;
					break;

				case IDM_JOY1:
					xmenu_setkey(1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_JOY2:
					xmenu_setkey(2);
					update |= SYS_UPDATECFG;
					break;

				case IDM_XSHIFT:
					xmenu_setxshift(np2cfg.XSHIFT ^ 1);
					keystat_forcerelease(0x70);
					update |= SYS_UPDATECFG;
					break;

				case IDM_XCTRL:
					xmenu_setxshift(np2cfg.XSHIFT ^ 2);
					keystat_forcerelease(0x74);
					update |= SYS_UPDATECFG;
					break;

				case IDM_XGRPH:
					xmenu_setxshift(np2cfg.XSHIFT ^ 4);
					keystat_forcerelease(0x73);
					update |= SYS_UPDATECFG;
					break;

				case IDM_F12MOUSE:
					xmenu_setf12copy(0);
					keystat_resetcopyhelp();
					update |= SYS_UPDATEOSCFG;
					break;

				case IDM_F12COPY:
					xmenu_setf12copy(1);
					keystat_resetcopyhelp();
					update |= SYS_UPDATEOSCFG;
					break;

				case IDM_F12STOP:
					xmenu_setf12copy(2);
					keystat_resetcopyhelp();
					update |= SYS_UPDATEOSCFG;
					break;

				case IDM_BEEPOFF:
					xmenu_setbeepvol(0);
					beep_setvol(0);
					update |= SYS_UPDATECFG;
					break;

				case IDM_BEEPLOW:
					xmenu_setbeepvol(1);
					beep_setvol(1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_BEEPMID:
					xmenu_setbeepvol(2);
					beep_setvol(2);
					update |= SYS_UPDATECFG;
					break;

				case IDM_BEEPHIGH:
					xmenu_setbeepvol(3);
					beep_setvol(3);
					update |= SYS_UPDATECFG;
					break;

				case IDM_NOSOUND:
					xmenu_setsound(0);
					update |= SYS_UPDATECFG;
					break;

				case IDM_PC9801_14:
					xmenu_setsound(1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_PC9801_26K:
					{
						BYTE	sw;
						sw = np2cfg.SOUND_SW;
						if (sw & 4) {
							sw &= 6;
							sw ^= 2;
						}
						else {
							sw = 2;
						}
						xmenu_setsound(sw);
						update |= SYS_UPDATECFG;
					}
					break;

				case IDM_PC9801_86:
					{
						BYTE	sw;
						sw = np2cfg.SOUND_SW;
						if (sw & 2) {
							sw &= 6;
							sw ^= 4;
						}
						else if (!(sw & 4)) {
							sw = 4;
						}
						xmenu_setsound(sw);
						update |= SYS_UPDATECFG;
					}
					break;

				case IDM_PC9801_118:
					xmenu_setsound(8);
					update |= SYS_UPDATECFG;
					break;

				case IDM_CHIBIOTO:
					{
						BYTE	sw;
						sw = np2cfg.SOUND_SW;
						if (sw & 4) {
							sw &= 0x14;
							sw ^= 0x10;
						}
						else {
							sw = 0x14;
						}
						xmenu_setsound(sw);
						update |= SYS_UPDATECFG;
					}
					break;

				case IDM_SPEAKBOARD:
					xmenu_setsound(0x20);
					update |= SYS_UPDATECFG;
					break;

				case IDM_SPARKBOARD:
					xmenu_setsound(0x40);
					update |= SYS_UPDATECFG;
					break;

				case IDM_AMD98:
					xmenu_setsound(0x80);
					update |= SYS_UPDATECFG;
					break;

				case IDM_SNDOPT:
					winuienter();
					dialog_sndopt(hWnd);
					winuileave();
					break;

				case IDM_SEEKSND:
					xmenu_setmotorflg(np2cfg.MOTOR ^ 1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_MEM640:
					xmenu_setextmem(0);
					update |= SYS_UPDATECFG;
					break;

				case IDM_MEM16:
					xmenu_setextmem(1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_MEM36:
					xmenu_setextmem(3);
					update |= SYS_UPDATECFG;
					break;

				case IDM_MEM76:
					xmenu_setextmem(7);
					update |= SYS_UPDATECFG;
					break;

				case IDM_MOUSE:
					mouse_running(MOUSE_XOR);
					xmenu_setmouse(np2oscfg.MOUSE_SW ^ 1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_MPUPC98:
					winuienter();
					DialogBox(hInst, MAKEINTRESOURCE(IDD_MPUPC98),
											hWnd, (DLGPROC)MidiDialogProc);
					winuileave();
					break;

				case IDM_MIDIPANIC:									// ver0.29
					rs232c_midipanic();
					mpu98ii_midipanic();
					pc9861k_midipanic();
					break;

				case IDM_BMPSAVE:
					winuienter();
					dialog_writebmp(hWnd);
					winuileave();
					break;

				case IDM_S98LOGGING:
					winuienter();
					dialog_s98(hWnd);
					winuileave();
					break;

				case IDM_CALENDAR:
					winuienter();
					DialogBox(hInst, MAKEINTRESOURCE(IDD_CALENDAR),
											hWnd, (DLGPROC)ClndDialogProc);
					winuileave();
					break;

				case IDM_JOYX:
					xmenu_setbtnmode(np2cfg.BTN_MODE ^ 1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_RAPID:
					xmenu_setbtnrapid(np2cfg.BTN_RAPID ^ 1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_MSRAPID:
					xmenu_setmsrapid(np2cfg.MOUSERAPID ^ 1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_I286SAVE:
					debugsub_status();
					break;

				case IDM_HELP:
					ShellExecute(hWnd, NULL, file_getcd(np2help),
											NULL, NULL, SW_SHOWNORMAL);
					break;

				case IDM_ABOUT:
					winuienter();
					DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT),
									hWnd, (DLGPROC)AboutDialogProc);
					winuileave();
					break;
			}
			sysmng_update(update);
			break;

		case WM_ACTIVATE:
			if (LOWORD(wParam) != WA_INACTIVE) {
				scrnmng_update();
				mouse_running(MOUSE_CONT_M);
			}
			else {
				mouse_running(MOUSE_STOP_M);
			}
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			if (np2opening) {

			    HDC			hmdc;
			    HBITMAP		hbmp;
			    BITMAP		bmp;
			    HINSTANCE	hinst;
				int			sx = 640;
				int			sy = 400;

			    hinst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
#if 1
    			hbmp = LoadBitmap(hinst, "NP2BMP");
			    GetObject(hbmp, sizeof(BITMAP), &bmp);
			    hmdc = CreateCompatibleDC(hdc);
			    SelectObject(hmdc, hbmp);
			    BitBlt(hdc, (sx - 252) / 2, (sy - 28) / 2,
							bmp.bmWidth, bmp.bmHeight, hmdc, 0, 0, SRCCOPY);
			    DeleteDC(hmdc);
	    		DeleteObject(hbmp);
#else
    			hbmp = LoadBitmap(hinst, "NP2EXT");
			    GetObject(hbmp, sizeof(BITMAP), &bmp);
			    hmdc = CreateCompatibleDC(hdc);
			    SelectObject(hmdc, hbmp);
			    BitBlt(hdc, (sx - 160) / 2, (sy - 140) / 2,
							bmp.bmWidth, bmp.bmHeight, hmdc, 0, 0, SRCCOPY);
			    DeleteDC(hmdc);
	    		DeleteObject(hbmp);
#endif
			}
			else {
//				scrnmng_update();
				scrndraw_redraw();
			}
			EndPaint(hWnd, &ps);
			break;

		case WM_QUERYNEWPALETTE:
			scrnmng_querypalette();
			break;

		case WM_MOVE:
			if ((!scrnmng_isfullscreen()) &&
				(!(GetWindowLong(hWndMain, GWL_STYLE) &
									(WS_MAXIMIZE | WS_MINIMIZE)))) {
				GetWindowRect(hWnd, &rc);
				np2oscfg.winx = rc.left;
				np2oscfg.winy = rc.top;
				sysmng_update(SYS_UPDATEOSCFG);
			}
			break;

		case WM_ENTERMENULOOP:
			winuienter();
			if (scrnmng_isfullscreen()) {
				DrawMenuBar(hWnd);
			}
			break;

		case WM_EXITMENULOOP:
			winuileave();
			break;

		case WM_ENTERSIZEMOVE:
			soundmng_stop();
			mouse_running(MOUSE_STOP);
			break;

		case WM_EXITSIZEMOVE:
			mouse_running(MOUSE_CONT);
			soundmng_play();
			break;

		case WM_KEYDOWN:
			if (wParam == VK_F11) {
				return(DefWindowProc(hWnd, WM_SYSKEYDOWN, VK_F10, lParam));
			}
			if ((wParam == VK_F12) && (!np2oscfg.F12COPY)) {
				mouse_running(MOUSE_XOR);
				xmenu_setmouse(np2oscfg.MOUSE_SW ^ 1);
				sysmng_update(SYS_UPDATECFG);
			}
			else {
				winkeydown106(wParam, lParam);
			}
			break;
		case WM_KEYUP:
			if (wParam == VK_F11) {
				return(DefWindowProc(hWnd, WM_SYSKEYUP, VK_F10, lParam));
			}
			if ((wParam != VK_F12) || (np2oscfg.F12COPY)) {
				winkeyup106(wParam, lParam);
			}
			break;

		case WM_SYSKEYDOWN:
			winkeydown106(wParam, lParam);
			break;

		case WM_SYSKEYUP:
			winkeyup106(wParam, lParam);
			break;

		case WM_LBUTTONDOWN:
			if (!mouse_btn(MOUSE_LEFTDOWN)) {
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;
		case WM_LBUTTONUP:
			if (!mouse_btn(MOUSE_LEFTUP)) {
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_MBUTTONDOWN:									// ver0.26
			mouse_running(MOUSE_XOR);
			xmenu_setmouse(np2oscfg.MOUSE_SW ^ 1);
			sysmng_update(SYS_UPDATECFG);
			break;

		case WM_RBUTTONDOWN:
			if (!mouse_btn(MOUSE_RIGHTDOWN)) {
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_RBUTTONUP:
			if (!mouse_btn(MOUSE_RIGHTUP)) {
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
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
					np2quitmsg = 1;
					PostQuitMessage(0);
					break;

				case NP2CMD_EXIT2:
					np2quitmsg = 2;
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

static void framereset(void) {

	framecnt = 0;
	sysmng_updatecaption();
}

static void processwait(UINT waitcnt) {

	if (timing_getcount() >= waitcnt) {
		timing_setcount(0);
		framereset();
	}
	else {
		Sleep(1);
	}
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
										LPSTR lpszCmdLine, int nCmdShow) {
	WNDCLASS	np2;
	MSG			msg;
	HMENU		hMenu;
	HWND		hwndorg;
	int			i;
#ifdef OPENING_WAIT
	UINT32		tick;
#endif

	GetModuleFileName(NULL, modulefile, sizeof(modulefile));
	dosio_init();
	file_setcd(modulefile);
	np2arg_analize(lpszCmdLine);				// タイミング修正	// ver0.29
	initload();

	rand_setseed((unsigned)time(NULL));

	if ((hwndorg = FindWindow(szClassName, NULL)) != NULL) {
		ShowWindow(hwndorg, SW_RESTORE);
		SetForegroundWindow(hwndorg);
		dosio_term();
		return(FALSE);
	}

	hInst = hInstance;
	hPrev = hPreInst;
	TRACEINIT();

	if (np2oscfg.KEYBOARD >= KEY_TYPEMAX) {							// ver0.28
		int keytype = GetKeyboardType(1);
		if ((keytype & 0xff00) == 0x0d00) {
			np2oscfg.KEYBOARD = KEY_PC98;
		}
		else if (!keytype) {
			np2oscfg.KEYBOARD = KEY_KEY101;
		}
		else {
			np2oscfg.KEYBOARD = KEY_KEY106;
		}
	}
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
		np2.lpszMenuName = MAKEINTRESOURCE(IDR_MAIN);
		np2.lpszClassName = szClassName;
		if (!RegisterClass(&np2)) {
			return(FALSE);
		}
	}

	hWndMain = CreateWindowEx(0, szClassName, szAppCaption,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						np2oscfg.winx, np2oscfg.winy, 640, 400,
						NULL, NULL, hInstance, NULL);

	scrnmng_initialize();

	xmenu_setsound(np2cfg.SOUND_SW);
	xmenu_setbeepvol(np2cfg.BEEP_VOL);
	xmenu_setkey(0);
	xmenu_setbtnrapid(np2cfg.BTN_RAPID);
	xmenu_setbtnmode(np2cfg.BTN_MODE);
	xmenu_setmsrapid(np2cfg.MOUSERAPID);
	xmenu_setwaitflg(np2oscfg.NOWAIT);
	xmenu_setframe(np2oscfg.DRAW_SKIP);
	xmenu_setdispmode(np2cfg.DISPSYNC);
	xmenu_seti286save(np2oscfg.I286SAVE);
	xmenu_setmotorflg(np2cfg.MOTOR);
	xmenu_setroltate(0);
	xmenu_setxshift(0);
	xmenu_setf12copy(np2oscfg.F12COPY);
	xmenu_setextmem(np2cfg.EXTMEM);
	xmenu_setraster(np2cfg.RASTER);
	xmenu_setmouse(np2oscfg.MOUSE_SW);

	ShowWindow(hWndMain, nCmdShow);
	UpdateWindow(hWndMain);

#ifdef OPENING_WAIT
	tick = GetTickCount();
#endif

	if (np2oscfg.I286SAVE) {
		hMenu = GetSystemMenu(hWndMain, FALSE);
		InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, IDM_MEMORYDUMP,
							"&Memory Dump");
		InsertMenu(hMenu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
	}
	DrawMenuBar(hWndMain);

	// ver0.30
	if (file_attr_c(np2help) == (short)-1) {
		EnableMenuItem(GetMenu(hWndMain), IDM_HELP, MF_GRAYED);
	}

	scrnmode = 0;
	if (np2arg.fullscreen) {
		scrnmode |= SCRNMODE_FULLSCREEN;
	}
	if (np2cfg.RASTER) {
		scrnmode |= SCRNMODE_HIGHCOLOR;
	}
	if (scrnmng_create(scrnmode) != SUCCESS) {
		scrnmode ^= SCRNMODE_FULLSCREEN;
		if (scrnmng_create(scrnmode) != SUCCESS) {
			MessageBox(hWndMain, "Couldn't create DirectDraw Object",
										szAppCaption, MB_OK | MB_ICONSTOP);
			return(FALSE);
		}
	}

	soundmng_initialize();
	commng_initialize();
	sysmng_initialize();
	joy_init();
	pccore_init();
	S98_init();

#ifdef OPENING_WAIT
	while((GetTickCount() - tick) < OPENING_WAIT);
#endif

	if (np2oscfg.MOUSE_SW) {										// ver0.30
		mouse_running(MOUSE_ON);
	}
	scrndraw_redraw();
	pccore_reset();

	np2opening = 0;

//	リセットしてから… コマンドラインのディスク挿入。				// ver0.29
	for (i=0; i<4; i++) {
		if (np2arg.disk[i]) {
			milstr_ncpy(diskdrv_fname[i], np2arg.disk[i], MAX_PATH);
			diskdrv_delay[i] = 1;
		}
	}

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
				joy_flash();
				mouse_callback();
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
					joy_flash();
					mouse_callback();
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
					joy_flash();
					mouse_callback();
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
						framereset();
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

	mouse_running(MOUSE_OFF);
	soundmng_deinitialize();
	scrnmng_destroy();

	if (sys_updates	& (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
		initsave();
	}
	TRACETERM();
	dosio_term();

	return(msg.wParam);
}


#include	"compiler.h"
#include	<time.h>
#include	<winnls32.h>
#include	"resource.h"
#include	"strres.h"
#include	"np2.h"
#include	"np2arg.h"
#include	"dosio.h"
#include	"extromio.h"
#include	"commng.h"
#include	"joymng.h"
#include	"mousemng.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"winkbd.h"
#include	"ini.h"
#include	"menu.h"
#include	"dialog.h"
#include	"winloc.h"
#include	"sstp.h"
#include	"sstpmsg.h"
#include	"dclock.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"pc9861k.h"
#include	"mpu98ii.h"
#include	"timing.h"
#include	"debugsub.h"
#include	"bios.h"
#include	"scrndraw.h"
#include	"sound.h"
#include	"beep.h"
#include	"s98.h"
#include	"diskdrv.h"
#include	"fddfile.h"

#ifdef USE_ROMEO
#include	"juliet.h"
#endif

#include	"keydisp.h"
#include	"viewer.h"
#include	"statsave.h"


#ifdef BETA_RELEASE
#define		OPENING_WAIT		1500
#endif

#define	STATSAVEMAX		10

static	char		szClassName[] = "NP2-MainWindow";
		HWND		hWndMain;
		HINSTANCE	hInst;
		HINSTANCE	hPrev;

		NP2OSCFG	np2oscfg = {
						"Neko Project II", "NP2",
						CW_USEDEFAULT, CW_USEDEFAULT, 1, 1, 0, 1, 0, 2,
						0, 0, KEY_UNKNOWN, 0,
						0, 0, 0, {1, 2, 2, 1},
						{5, 0, 0x3e, 19200, "", "", "", ""},		// ver0.34
						{0, 0, 0x3e, 19200, "", "", "", ""},		// ver0.34
						{0, 0, 0x3e, 19200, "", "", "", ""},		// ver0.34
						{0, 0, 0x3e, 19200, "", "", "", ""},		// ver0.34
						0xffffff, 0xffbf6a, 0, 0,
						0, 1, 0, 9801, 0, 0, 0, 0};					// ver0.34

		char	fddfolder[MAX_PATH];
		char	hddfolder[MAX_PATH];
		char	bmpfilefolder[MAX_PATH];
		char	mimpideffile[MAX_PATH];
		char	modulefile[MAX_PATH];

static	UINT	framecnt = 0;
static	UINT	waitcnt = 0;
static	UINT	framemax = 1;
		BYTE	np2break = 0;									// ver0.30
static	BYTE	np2stopemulate = 0;
static	int		np2opening = 1;
static	int		np2quitmsg = 0;
		HMENU	hStat = NULL;

static const char np2help[] = "np2.hlp";
static const char np2resume[] = "sav";


typedef struct {
const char	*str;
	int		id;
} SYSMENU_ITEM;

static SYSMENU_ITEM smenu_item[] = {
			{"&Centering",			IDM_SCREENCENTER},
			{"&Window Snap",		IDM_SNAPENABLE},
			{"&Background",			IDM_BACKGROUND},
			{"Background &Sound",	IDM_BGSOUND},
			{"&Key display",		IDM_KEYDISP},
			{NULL,					0},
			{" 320x200",			IDM_SCRNMUL4},
			{" 480x300",			IDM_SCRNMUL6},
			{" 640x400",			IDM_SCRNMUL8},
			{" 800x500",			IDM_SCRNMUL10},
			{" 960x600",			IDM_SCRNMUL12},
			{"1280x800",			IDM_SCRNMUL16},
			{NULL,					0},
		};


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
		keydisp_destroy();
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

static void wincentering(HWND hWnd) {

	RECT	rc;
	int		width;
	int		height;

	GetWindowRect(hWnd, &rc);
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;
	np2oscfg.winx = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	np2oscfg.winy = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
	if (np2oscfg.winx < 0) {
		np2oscfg.winx = 0;
	}
	if (np2oscfg.winy < 0) {
		np2oscfg.winy = 0;
	}
	sysmng_update(SYS_UPDATEOSCFG);
	MoveWindow(hWndMain, np2oscfg.winx, np2oscfg.winy, width, height, TRUE);
}

void np2active_renewal(void) {									// ver0.30

	if (np2break & (~NP2BREAK_MAIN)) {
		np2stopemulate = 2;
		soundmng_disable();
	}
	else if (np2break & NP2BREAK_MAIN) {
		if (np2oscfg.background & 1) {
			np2stopemulate = 1;
		}
		else {
			np2stopemulate = 0;
		}
		if (np2oscfg.background) {
			soundmng_disable();
		}
		else {
			soundmng_enable();
		}
	}
	else {
		np2stopemulate = 0;
		soundmng_enable();
	}
}

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
	soundmng_stop();
	ret = statsave_save(path);
	if (ret) {
		file_delete(path);
	}
	soundmng_play();
	return(ret);
}

static int flagload(const char *ext, const char *title, BOOL force) {

	int		ret;
	int		id;
	char	path[MAX_PATH];
	char	buf[1024];

	getstatfilename(path, ext, sizeof(path));
	soundmng_stop();
	scrnmng_topwinui();
	id = IDYES;
	ret = statsave_check(path, buf, sizeof(buf));
	if (ret & (~NP2FLAG_DISKCHG)) {
		MessageBox(hWndMain, "Couldn't restart", title, MB_OK | MB_ICONSTOP);
		id = IDNO;
	}
	else if ((!force) && (ret & NP2FLAG_DISKCHG)) {
		char buf2[1024 + 256];
		wsprintf(buf2, "Conflict!\n\n%s\nContinue?", buf);
		id = MessageBox(hWndMain, buf2, title,
										MB_YESNOCANCEL | MB_ICONQUESTION);
	}
	if (id == IDYES) {
		statsave_load(path);
	}
	sysmng_workclockreset();
	sysmng_updatecaption(1);
	scrnmng_clearwinui();
	soundmng_play();
	return(id);
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
	BOOL		b;
	UINT		update;

	switch (msg) {
		case WM_CREATE:
			sstp_construct(hWnd);
			WINNLSEnableIME(hWnd, FALSE);
//			DragAcceptFiles(hWnd, TRUE);
			break;

		case WM_SYSCOMMAND:
			update = 0;
			switch(wParam) {
				case IDM_SCREENCENTER:							// ver0.26
					if ((!scrnmng_isfullscreen()) &&
						(!(GetWindowLong(hWnd, GWL_STYLE) &
											(WS_MAXIMIZE | WS_MINIMIZE)))) {
						wincentering(hWnd);
					}
					break;

				case IDM_SNAPENABLE:
					xmenu_setwinsnap(np2oscfg.WINSNAP ^ 1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_BACKGROUND:
					xmenu_setbackground(np2oscfg.background ^ 1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_BGSOUND:
					xmenu_setbgsound(np2oscfg.background ^ 2);
					update |= SYS_UPDATECFG;
					break;

				case IDM_KEYDISP:
					keydisp_create();
					break;

				case IDM_MEMORYDUMP:
					debugsub_memorydump();
					break;

				case IDM_DEBUGUTY:
					viewer_open();
					break;

				case IDM_SCRNMUL4:								// ver0.26
				case IDM_SCRNMUL6:
				case IDM_SCRNMUL8:
				case IDM_SCRNMUL10:
				case IDM_SCRNMUL12:
				case IDM_SCRNMUL16:
#if 0
					if ((SCREENMODE & SCMD_WINDOWED) &&
						!(GetWindowLong(hWndMain, GWL_STYLE) & WS_MINIMIZE)) {
						xmenu_setscrnmul(wParam - IDM_SCRNMUL);
						ddraws_setmul(wParam - IDM_SCRNMUL);
					}
#endif
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
					b = FALSE;
					if (!np2oscfg.comfirm) {
						b = TRUE;
					}
					else if (sstpconfirm_reset()) {
						winuienter();
						if (MessageBox(hWnd, "Sure?", "Reset",
									MB_ICONQUESTION | MB_YESNO) == IDYES) {
							b = TRUE;
						}
						winuileave();
					}
					if (b) {
						sstpmsg_reset();
						pccore_cfgupdate();
						pccore_reset();
					}
					break;

				case IDM_CONFIG:
					winuienter();
					sstpmsg_config();
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
					keystat_resetjoykey();
					break;

				case IDM_JOY1:
					xmenu_setkey(1);
					keystat_resetjoykey();
					break;

				case IDM_JOY2:
					xmenu_setkey(2);
					keystat_resetjoykey();
					break;

				case IDM_XSHIFT:
					xmenu_setxshift(np2cfg.XSHIFT ^ 1);
					keystat_forcerelease(0x70);
					break;

				case IDM_XCTRL:
					xmenu_setxshift(np2cfg.XSHIFT ^ 2);
					keystat_forcerelease(0x74);
					break;

				case IDM_XGRPH:
					xmenu_setxshift(np2cfg.XSHIFT ^ 4);
					keystat_forcerelease(0x73);
					break;

				case IDM_F12MOUSE:
					xmenu_setf12copy(0);
					keystat_resetcopyhelp();
					update |= SYS_UPDATECFG;
					break;

				case IDM_F12COPY:
					xmenu_setf12copy(1);
					keystat_resetcopyhelp();
					update |= SYS_UPDATECFG;
					break;

				case IDM_F12STOP:
					xmenu_setf12copy(2);
					keystat_resetcopyhelp();
					update |= SYS_UPDATECFG;
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

				case IDM_SERIAL1:
					winuienter();
					dialog_serial(hWnd);
					winuileave();
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

				case IDM_DISPCLOCK:
					xmenu_setdispclk(np2oscfg.DISPCLK ^ 1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_DISPFRAME:
					xmenu_setdispclk(np2oscfg.DISPCLK ^ 2);
					update |= SYS_UPDATECFG;
					break;

				case IDM_CALENDAR:
					winuienter();
					DialogBox(hInst, MAKEINTRESOURCE(IDD_CALENDAR),
											hWnd, (DLGPROC)ClndDialogProc);
					winuileave();
					break;

				case IDM_ALTENTER:
					xmenu_setshortcut(np2oscfg.shortcut ^ 1);
					update |= SYS_UPDATECFG;
					break;

				case IDM_ALTF4:
					xmenu_setshortcut(np2oscfg.shortcut ^ 2);
					update |= SYS_UPDATECFG;
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

				case IDM_SSTP:
					xmenu_setsstp(np2oscfg.sstp ^ 1);
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
					sstpmsg_about();
					if (sstp_result() != SSTP_SENDING) {
						winuienter();
						DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT),
										hWnd, (DLGPROC)AboutDialogProc);
						winuileave();
					}
					break;

				default:
					if ((LOWORD(wParam) >= IDM_FLAGSAVE) &&
						(LOWORD(wParam) < IDM_FLAGSAVE + STATSAVEMAX)) {
						char ext[4];
						wsprintf(ext, "S%02d",
									(LOWORD(wParam) - IDM_FLAGSAVE) % 100);
						flagsave(ext);
					}
					if ((LOWORD(wParam) >= IDM_FLAGLOAD) &&
						(LOWORD(wParam) < IDM_FLAGLOAD + STATSAVEMAX)) {
						char ext[4];
						wsprintf(ext, "S%02d",
									(LOWORD(wParam) - IDM_FLAGLOAD) % 100);
						flagload(ext, "Status Load", TRUE);
					}
					break;
			}
			sysmng_update(update);
			break;

		case WM_ACTIVATE:
			if (LOWORD(wParam) != WA_INACTIVE) {
				np2break &= ~NP2BREAK_MAIN;
				scrnmng_update();
				mouse_running(MOUSE_CONT_M);
			}
			else {
				np2break |= NP2BREAK_MAIN;
				mouse_running(MOUSE_STOP_M);
			}
			np2active_renewal();
			break;

		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			if (np2opening) {

			    HDC			hmdc;
			    HBITMAP		hbmp;
			    BITMAP		bmp;
			    HINSTANCE	hinst;
				int			sx = 640 + np2oscfg.paddingx;
				int			sy = 400 + np2oscfg.paddingy;

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
			winloc_movingstart();
			break;

		case WM_EXITSIZEMOVE:
			mouse_running(MOUSE_CONT);
			soundmng_play();
			break;

		case WM_MOVING:
			if (np2oscfg.WINSNAP) {
				winloc_movingproc((RECT *)lParam);
			}
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
			if (lParam & 0x20000000) {								// ver0.30
				if ((np2oscfg.shortcut & 1) && (wParam == VK_RETURN)) {
					changescreen(scrnmode ^ SCRNMODE_FULLSCREEN);
					break;
				}
				if ((np2oscfg.shortcut & 2) && (wParam == VK_F4)) {
					SendMessage(hWnd, WM_CLOSE, 0, 0L);
					break;
				}
			}
			winkeydown106(wParam, lParam);
			break;

		case WM_SYSKEYUP:
			winkeyup106(wParam, lParam);
			break;

		case WM_LBUTTONDOWN:
			if (!mouse_btn(MOUSE_LEFTDOWN)) {
				if (scrnmng_isfullscreen()) {
					POINT	p;
					if ((GetCursorPos(&p)) && (p.y >= 466)) {
						np2oscfg.clk_x++;
						sysmng_update(SYS_UPDATEOSCFG);
						dclock_reset();
					}
				}
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
				if (scrnmng_isfullscreen()) {
					POINT	p;
					if ((GetCursorPos(&p)) && (p.y >= 466) &&
												(np2oscfg.clk_x)) {
						np2oscfg.clk_fnt++;
						sysmng_update(SYS_UPDATEOSCFG);
						dclock_reset();
					}
				}
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_RBUTTONUP:
			if (!mouse_btn(MOUSE_RIGHTUP)) {
				return(DefWindowProc(hWnd, msg, wParam, lParam));
			}
			break;

		case WM_CLOSE:
			b = FALSE;
			if (!np2oscfg.comfirm) {
				b = TRUE;
			}
			else if (sstpconfirm_exit()) {
				winuienter();
				if (MessageBox(hWnd, "Sure?", "Exit",
									MB_ICONQUESTION | MB_YESNO) == IDYES) {
					b = TRUE;
				}
				winuileave();
			}
			if (b) {
				keydisp_destroy();
				viewer_allclose();
				DestroyWindow(hWnd);
			}
			break;

		case WM_DESTROY:
			DestroyMenu(hStat);
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

		case WM_SSTP:
			switch(LOWORD(lParam)) {
				case FD_CONNECT:
					if (!HIWORD(lParam)) {
						sstp_connect();
					}
					break;
				case FD_READ:
					if (!HIWORD(lParam)) {
						sstp_readSocket();
					}
					break;
				case FD_WRITE:
					if (!HIWORD(lParam)) {
//						sstp_writeSokect();
					}
					break;
				case FD_CLOSE:
					if (!HIWORD(lParam)) {
						sstp_disconnect();
					}
					break;
			}
			break;

		case MM_MIM_DATA:
			cmmidi_recvdata((HMIDIIN)wParam, (DWORD)lParam);
			break;

		case MM_MIM_LONGDATA:
			cmmidi_recvexcv((HMIDIIN)wParam, (MIDIHDR *)lParam);
			break;

		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return(0L);
}


static void processwait(WORD cnt) {

	if (timing_getcount() >= cnt) {
		timing_setcount(0);
		framecnt = 0;
		scrnmng_dispclock();
		keydisp_draw(np2oscfg.DRAW_SKIP);
		viewer_allreload(FALSE);
		if (np2oscfg.DISPCLK & 3) {
			if (sysmng_workclockrenewal()) {
				sysmng_updatecaption(3);
			}
		}
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
	np2arg_analize(lpszCmdLine);				// �^�C�~���O�C��	// ver0.29
	initload();

	srand((unsigned)time(NULL));

//	np2arg_analize(lpszCmdLine);

	CopyMemory(szClassName, np2oscfg.winid, 3);

	if ((hwndorg = FindWindow(szClassName, NULL)) != NULL) {
		sstpmsg_running();
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
		np2.lpszMenuName = MAKEINTRESOURCE(IDM_MAIN);
		np2.lpszClassName = szClassName;
		if (!RegisterClass(&np2)) {
			return(FALSE);
		}
	}
	keydisp_initialize(hPreInst);
	viewer_init(hPreInst);										// ver0.30

	hWndMain = CreateWindowEx(0, szClassName, np2oscfg.titles,
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
	xmenu_setdispclk(np2oscfg.DISPCLK);
	xmenu_seti286save(np2oscfg.I286SAVE);
	xmenu_setmotorflg(np2cfg.MOTOR);
	xmenu_setroltate(0);
	xmenu_setxshift(0);
	xmenu_setf12copy(np2oscfg.F12COPY);
	xmenu_setextmem(np2cfg.EXTMEM);
	xmenu_setraster(np2cfg.RASTER);
	xmenu_setshortcut(np2oscfg.shortcut);						// ver0.30
	xmenu_setsstp(np2oscfg.sstp);
	xmenu_setmouse(np2oscfg.MOUSE_SW);

	ShowWindow(hWndMain, nCmdShow);
	UpdateWindow(hWndMain);

#ifdef OPENING_WAIT
	tick = GetTickCount();
#endif

	// �߂ɂ�[�ǉ�
	if (np2oscfg.statsave) {
		char buf[16];
		hMenu = GetMenu(hWndMain);
		hStat = CreatePopupMenu();
		for (i=0; i<STATSAVEMAX; i++) {
			wsprintf(buf, "Save %d", i);
			AppendMenu(hStat, MF_STRING, IDM_FLAGSAVE + i, buf);
		}
		AppendMenu(hStat, MF_MENUBARBREAK, 0, NULL);
		for (i=0; i<STATSAVEMAX; i++) {
			wsprintf(buf, "Load %d", i);
			AppendMenu(hStat, MF_STRING, IDM_FLAGLOAD + i, buf);
		}
		InsertMenu(hMenu, 1, MF_BYPOSITION | MF_POPUP, (UINT)hStat, "S&tat");
	}

	hMenu = GetSystemMenu(hWndMain, FALSE);						// ver0.26
	for (i=0; i<(sizeof(smenu_item)/sizeof(SYSMENU_ITEM)); i++) {
		if (smenu_item[i].str) {
			InsertMenu(hMenu, i, MF_BYPOSITION | MF_STRING,
							smenu_item[i].id, smenu_item[i].str);
		}
		else {
			InsertMenu(hMenu, i, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
		}
	}

	if (np2oscfg.I286SAVE) {
		InsertMenu(hMenu, 5, MF_BYPOSITION | MF_STRING, IDM_MEMORYDUMP,
							"&Memory Dump");
		InsertMenu(hMenu, 6, MF_BYPOSITION | MF_STRING, IDM_DEBUGUTY,
							"&Debug Utility");
	}
	DrawMenuBar(hWndMain);

	// ver0.30
	if (file_attr_c(np2help) == (short)-1) {
		EnableMenuItem(GetMenu(hWndMain), IDM_HELP, MF_GRAYED);
	}

	xmenu_setwinsnap(np2oscfg.WINSNAP);
	xmenu_setbackground(np2oscfg.background);
	xmenu_setbgsound(np2oscfg.background);
	xmenu_setscrnmul(8);										// ver0.26

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
			if (sstpmsg_dxerror()) {
				MessageBox(hWndMain, "Couldn't create DirectDraw Object",
										np2oscfg.titles, MB_OK | MB_ICONSTOP);
			}
			return(FALSE);
		}
	}

	if (soundmng_initialize() == SUCCESS) {
		soundmng_pcmload(SOUND_PCMSEEK, "SEEKWAV", EXTROMIO_RES);
		soundmng_pcmload(SOUND_PCMSEEK1, "SEEK1WAV", EXTROMIO_RES);
		soundmng_pcmvolume(SOUND_PCMSEEK, np2cfg.MOTORVOL);
		soundmng_pcmvolume(SOUND_PCMSEEK1, np2cfg.MOTORVOL);
	}

#ifdef USE_ROMEO
	juliet_load();
	juliet_prepare();
#endif

	if (np2oscfg.MOUSE_SW) {										// ver0.30
		mouse_running(MOUSE_ON);
	}

	commng_initialize();
	sysmng_initialize();

	joy_init();
	pccore_init();
	S98_init();


	sstpmsg_welcome();

#ifdef OPENING_WAIT
	while((GetTickCount() - tick) < OPENING_WAIT);
#endif

	scrndraw_redraw();

	pccore_reset();

	np2opening = 0;

	// �ꂶ����
	if (np2oscfg.resume) {
		int		id;

		id = flagload(np2resume, "Resume", FALSE);
		if (id == IDYES) {
			for (i=0; i<4; i++) np2arg.disk[i] = NULL;
		}
		else if (id == IDCANCEL) {
			DestroyWindow(hWndMain);
			mouse_running(MOUSE_OFF);
			S98_trash();
			pccore_term();
			sstp_destruct();
			soundmng_deinitialize();
			scrnmng_destroy();
			TRACETERM();
			dosio_term();
			viewer_term();
			return(0);
		}
	}

//	���Z�b�g���Ă���c �R�}���h���C���̃f�B�X�N�}���B				// ver0.29
	for (i=0; i<4; i++) {
		if (np2arg.disk[i]) {
			milstr_ncpy(diskdrv_fname[i], np2arg.disk[i], MAX_PATH);
			diskdrv_delay[i] = 1;
		}
	}

	while(1) {
		if (!np2stopemulate) {
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
					dclock_callback();
					if (np2oscfg.DRAW_SKIP) {		// nowait frame skip
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
						dclock_callback();
						framecnt++;
					}
					else {
						processwait(np2oscfg.DRAW_SKIP);
					}
				}
				else {								// auto skip
					if (!waitcnt) {
						joy_flash();
						mouse_callback();
						pccore_exec(framecnt == 0);
						dclock_callback();
						framecnt++;
						if (timing_getcount() < framecnt) {
							waitcnt = framecnt;
							if (framemax > 1) {
								framemax--;
							}
						}
						else if (framecnt >= framemax) {
							waitcnt = framecnt;
							if (framemax < 12) {
								framemax++;
							}
						}
					}
					else {
						processwait(waitcnt);
						waitcnt = framecnt;
					}
				}
			}
		}
		else if ((np2stopemulate == 1) ||				// background sleep
				(PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))) {
			if (!GetMessage(&msg, NULL, 0, 0)) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	pccore_cfgupdate();

	mouse_running(MOUSE_OFF);
	S98_trash();

	if (np2oscfg.resume) {
		flagsave(np2resume);
	}
	else {
//		DeleteFile(file_getcd(np2resume));
	}

#ifdef USE_ROMEO
	juliet_YMF288Reset();
#endif
	pccore_term();

	sstp_destruct();

#ifdef USE_ROMEO
	juliet_unload();
#endif

	soundmng_deinitialize();
	scrnmng_destroy();

	if (sys_updates	& (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
		initsave();
	}

	TRACETERM();
	dosio_term();

	viewer_term();												// ver0.30

	return(msg.wParam);
}


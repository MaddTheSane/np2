#include	"compiler.h"
#include	"resource.h"
#include	"strres.h"
#include	"np2.h"
#include	"dosio.h"
#include	"scrnmng.h"
#include	"soundmng.h"
#include	"sysmng.h"
#include	"taskmng.h"
#include	"mackbd.h"
#include	"ini.h"
#include	"menu.h"
#include	"dialog.h"
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
#include	"statsave.h"

#if defined(NP2GCC)
#include	"mousemng.h"
#endif

#define	USE_RESUME


#if defined(NP2GCC)
		NP2OSCFG	np2oscfg = {0, 2, 0, 0, 0, 0};
#else
		NP2OSCFG	np2oscfg = {0, 2, 0, 0, 0};
#endif

		WindowPtr	hWndMain;
		BOOL		np2running;
static	UINT		framecnt = 0;
static	UINT		waitcnt = 0;
static	UINT		framemax = 1;
static	BYTE		scrnmode;


#define DRAG_THRESHOLD		5

#ifndef NP2GCC
#define	DEFAULTPATH		":"
#else
#define	DEFAULTPATH		"::::"
#endif
static	char	target[MAX_PATH] = DEFAULTPATH;


static const char np2resume[] = "sav";


// ---- ‚¨‚Ü‚¶‚È‚¢

#if TARGET_CARBON
#define	AEProc(fn)	NewAEEventHandlerUPP((AEEventHandlerProcPtr)(fn))
#else
#define	AEProc(fn)	NewAEEventHandlerProc(fn)
#endif

#ifdef TARGET_API_MAC_CARBON
static pascal OSErr handleQuitApp(const AppleEvent *event, AppleEvent *reply,
															long refcon) {

	taskmng_exit();

	(void)event, (void)reply, (void)refcon;
	return(noErr);
}
#endif

static void InitToolBox(void) {

#if !TARGET_CARBON
	MaxApplZone();
	InitGraf(&qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(0);
#endif
	FlushEvents(everyEvent, 0);
	InitCursor();

#ifdef TARGET_API_MAC_CARBON
	AEInstallEventHandler(kCoreEventClass, kAEQuitApplication,
						AEProc(handleQuitApp), 0L, false);
#endif
}

static void MenuBarInit(void) {

	Handle		hMenu;
	MenuHandle	happlemenu;

	hMenu = GetNewMBar(IDM_MAINMENU);
	if (!hMenu) {
		ExitToShell();
	}
	SetMenuBar(hMenu);
	happlemenu = GetMenuHandle(IDM_APPLE);
	if (happlemenu) {
		AppendResMenu(happlemenu, 'DRVR');
	}
	InsertMenu(GetMenu(IDM_SASI1), -1);
	InsertMenu(GetMenu(IDM_SASI2), -1);
	InsertMenu(GetMenu(IDM_KEYBOARD), -1);
	InsertMenu(GetMenu(IDM_SOUND), -1);
	InsertMenu(GetMenu(IDM_MEMORY), -1);
	DrawMenuBar();
}

static void changescreen(BYTE mode) {

	(void)mode;
}

static void HandleMenuChoice(long wParam) {

	UINT	update;
	Str255	applname;

	update = 0;
	switch(wParam) {
		case IDM_ABOUT:
			AboutDialogProc();
			break;

		case IDM_RESET:
			pccore_cfgupdate();
			pccore_reset();
			break;

#if 0
		case IDM_NEWFDD:
			newdisk();
			break;
#endif
#if 0
		case IDM_NEWHDD:
			newhdddisk();
			break;
#endif
		case IDM_EXIT:
			np2running = FALSE;
			break;

		case IDM_FDD1OPEN:
			dialog_changefdd(0);
			break;

		case IDM_FDD1EJECT:
			diskdrv_setfdd(0, NULL, 0);
			break;

		case IDM_FDD2OPEN:
			dialog_changefdd(1);
			break;

		case IDM_FDD2EJECT:
			diskdrv_setfdd(1, NULL, 0);
			break;

		case IDM_SASI1OPEN:
			dialog_changehdd(0);
			break;

		case IDM_SASI1REMOVE:
			diskdrv_sethdd(0, NULL);
			break;

		case IDM_SASI2OPEN:
			dialog_changehdd(1);
			break;

		case IDM_SASI2REMOVE:
			diskdrv_sethdd(1, NULL);
			break;

		case IDM_ROLNORMAL:
			menu_setrotate(0);
			changescreen(scrnmode & (~SCRNMODE_ROTATEMASK));
			break;

		case IDM_ROLLEFT:
			menu_setrotate(1);
			changescreen((scrnmode & (~SCRNMODE_ROTATEMASK)) |
														SCRNMODE_ROTATELEFT);
			break;

		case IDM_ROLRIGHT:
			menu_setrotate(2);
			changescreen((scrnmode & (~SCRNMODE_ROTATEMASK)) |
														SCRNMODE_ROTATERIGHT);
			break;

		case IDM_DISPSYNC:
			menu_setdispmode(np2cfg.DISPSYNC ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_RASTER:
			menu_setraster(np2cfg.RASTER ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_NOWAIT:
			menu_setwaitflg(np2oscfg.NOWAIT ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_AUTOFPS:
			menu_setframe(0);
			update |= SYS_UPDATECFG;
			break;

		case IDM_60FPS:
			menu_setframe(1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_30FPS:
			menu_setframe(2);
			update |= SYS_UPDATECFG;
			break;

		case IDM_20FPS:
			menu_setframe(3);
			update |= SYS_UPDATECFG;
			break;

		case IDM_15FPS:
			menu_setframe(4);
			update |= SYS_UPDATECFG;
			break;

#if defined(NP2GCC)
        case IDM_MOUSE:
            mouse_running(MOUSE_XOR);
            menu_setmouse(np2oscfg.MOUSE_SW ^ 1);
            sysmng_update(SYS_UPDATECFG);
			break;
#endif

		case IDM_KEY:
			menu_setkey(0);
			keystat_resetjoykey();
			break;

		case IDM_JOY1:
			menu_setkey(1);
			keystat_resetjoykey();
			break;

		case IDM_JOY2:
			menu_setkey(2);
			keystat_resetjoykey();
			break;

		case IDM_MOUSEKEY:
			menu_setkey(3);
			keystat_resetjoykey();
			break;

		case IDM_XSHIFT:
			menu_setxshift(np2cfg.XSHIFT ^ 1);
			keystat_forcerelease(0x70);
			break;

		case IDM_XCTRL:
			menu_setxshift(np2cfg.XSHIFT ^ 2);
			keystat_forcerelease(0x74);
			break;

		case IDM_XGRPH:
			menu_setxshift(np2cfg.XSHIFT ^ 4);
			keystat_forcerelease(0x73);
			break;

		case IDM_F12MOUSE:
			menu_setf12copy(0);
			keystat_resetcopyhelp();
			update |= SYS_UPDATECFG;
			break;

		case IDM_F12COPY:
			menu_setf12copy(1);
			keystat_resetcopyhelp();
			update |= SYS_UPDATECFG;
			break;

		case IDM_F12STOP:
			menu_setf12copy(2);
			keystat_resetcopyhelp();
			update |= SYS_UPDATECFG;
			break;

		case IDM_BEEPOFF:
			menu_setbeepvol(0);
			beep_setvol(0);
			update |= SYS_UPDATECFG;
			break;

		case IDM_BEEPLOW:
			menu_setbeepvol(1);
			beep_setvol(1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_BEEPMID:
			menu_setbeepvol(2);
			beep_setvol(2);
			update |= SYS_UPDATECFG;
			break;

		case IDM_BEEPHIGH:
			menu_setbeepvol(3);
			beep_setvol(3);
			update |= SYS_UPDATECFG;
			break;

		case IDM_NOSOUND:
			menu_setsound(0);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_14:
			menu_setsound(0x01);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_26K:
			menu_setsound(0x02);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_86:
			menu_setsound(0x04);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_26_86:
			menu_setsound(0x06);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_86_CB:
			menu_setsound(0x14);
			update |= SYS_UPDATECFG;
			break;

		case IDM_PC9801_118:
			menu_setsound(0x08);
			update |= SYS_UPDATECFG;
			break;

		case IDM_SPEAKBOARD:
			menu_setsound(0x20);
			update |= SYS_UPDATECFG;
			break;

		case IDM_SPARKBOARD:
			menu_setsound(0x40);
			update |= SYS_UPDATECFG;
			break;

		case IDM_SEEKSND:
			menu_setmotorflg(np2cfg.MOTOR ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM640:
			menu_setextmem(0);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM16:
			menu_setextmem(1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM36:
			menu_setextmem(3);
			update |= SYS_UPDATECFG;
			break;

		case IDM_MEM76:
			menu_setextmem(7);
			update |= SYS_UPDATECFG;
			break;

		case IDM_DISPCLOCK:
			menu_setdispclk(np2oscfg.DISPCLK ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_DISPFLAME:
			menu_setdispclk(np2oscfg.DISPCLK ^ 2);
			update |= SYS_UPDATECFG;
			break;

		case IDM_JOYX:
			menu_setbtnmode(np2cfg.BTN_MODE ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_RAPID:
			menu_setbtnrapid(np2cfg.BTN_RAPID ^ 1);
			update |= SYS_UPDATECFG;
			break;

		case IDM_I286SAVE:
			debugsub_status();
			break;

		default:
			if (HiWord(wParam) == IDM_APPLE) {
				GetMenuItemText(GetMenuHandle(IDM_APPLE), 
											LoWord(wParam), applname);
#if !TARGET_API_MAC_CARBON
				(void)OpenDeskAcc(applname);
#endif
			}
			break;
	}
	sysmng_update(update);
	HiliteMenu(0);
}

static void HandleUpdateEvent(EventRecord *pevent) {

	WindowPtr	hWnd;

	hWnd = (WindowPtr)pevent->message;
	BeginUpdate(hWnd);
	scrndraw_redraw();
	EndUpdate(hWnd);
}

static void HandleMouseDown(EventRecord *pevent) {

	WindowPtr	hWnd;
	Rect		rDrag;

	switch(FindWindow(pevent->where, &hWnd)) {
		case inMenuBar:
			HandleMenuChoice(MenuSelect(pevent->where));
			break;

		case inDrag:
#if TARGET_API_MAC_CARBON
		{
			BitMap	gscreenBits;
			GetQDGlobalsScreenBits(&gscreenBits);
			rDrag = gscreenBits.bounds;
			InsetRect(&rDrag, DRAG_THRESHOLD, DRAG_THRESHOLD);
			DragWindow(hWnd, pevent->where, &rDrag);
		}
#else
			rDrag = qd.screenBits.bounds;
			InsetRect(&rDrag, DRAG_THRESHOLD, DRAG_THRESHOLD);
			DragWindow(hWnd, pevent->where, &rDrag);
#endif
			break;

		case inContent:
#if defined(NP2GCC)
            if (controlKey & GetCurrentKeyModifiers() ) {
                mouse_btn(MOUSE_RIGHTDOWN);
            }
            else {
                mouse_btn(MOUSE_LEFTDOWN);
            }
#endif
			break;

		case inGoAway:
			if (TrackGoAway(hWnd, pevent->where)) { }
			np2running = FALSE;
			break;
	}
}

static void eventproc(EventRecord *event) {

	switch(event->what) {
		case mouseDown:
			HandleMouseDown(event);
			break;

		case updateEvt:
			HandleUpdateEvent(event);
			break;

		case keyDown:
		case autoKey:
			mackbd_f12down(((event->message) & keyCodeMask) >> 8);
			if (event->modifiers & cmdKey) {
				HandleMenuChoice(MenuKey(event->message & charCodeMask));
			}
			break;

		case keyUp:
			mackbd_f12up(((event->message) & keyCodeMask) >> 8);
			break;

#if defined(NP2GCC)
        case mouseUp:
            if (controlKey & GetCurrentKeyModifiers()) {
                mouse_btn(MOUSE_RIGHTUP);
            }
            else {
                mouse_btn(MOUSE_LEFTUP);
            }
			break;
#endif
	}
}


// ----

static void processwait(UINT waitcnt) {

	if (timing_getcount() >= waitcnt) {
		timing_setcount(0);
		framecnt = 0;
		if (np2oscfg.DISPCLK & 3) {
			if (sysmng_workclockrenewal()) {
				sysmng_updatecaption(3);
			}
		}
	}
}

static void getstatfilename(char *path, const char *ext, int size) {

	file_cpyname(path, file_getcd("np2"), size);
	file_catname(path, str_dot, size);
	file_catname(path, ext, size);
}

static void flagsave(const char *ext) {

	char	path[MAX_PATH];

	getstatfilename(path, ext, sizeof(path));
	statsave_save(path);
}

static void flagload(const char *ext) {

	char	path[MAX_PATH];
	char	buf[1024];
	int		ret;

	getstatfilename(path, ext, sizeof(path));
	ret = statsave_check(path, buf, sizeof(buf));
	if (ret == NP2FLAG_SUCCESS) {
		statsave_load(path);
	}
}

int main(int argc, char *argv[]) {

	Rect		wRect;
	EventRecord	event;

	dosio_init();
	file_setcd(target);

	InitToolBox();
	macossub_init();
	MenuBarInit();

	initload();

	TRACEINIT();

	SetRect(&wRect, 100, 100, 100, 100);
	hWndMain = NewWindow(0, &wRect, "\pNeko Project II", FALSE,
								noGrowDocProc, (WindowPtr)-1, TRUE, 0);
	if (!hWndMain) {
		TRACETERM();
		macossub_term();
		dosio_term();
		return(0);
	}
	scrnmng_initialize();
	SizeWindow(hWndMain, 640, 400, TRUE);
	ShowWindow(hWndMain);

	menu_setrotate(0);
	menu_setdispmode(np2cfg.DISPSYNC);
	menu_setwaitflg(np2oscfg.NOWAIT);
	menu_setframe(np2oscfg.DRAW_SKIP);
	menu_setkey(0);
	menu_setxshift(0);
	menu_setf12copy(np2oscfg.F12COPY);
	menu_setbeepvol(np2cfg.BEEP_VOL);
	menu_setsound(np2cfg.SOUND_SW);
	menu_setmotorflg(np2cfg.MOTOR);
	menu_setextmem(np2cfg.EXTMEM);
	menu_setdispclk(np2oscfg.DISPCLK);
	menu_setbtnrapid(np2cfg.BTN_RAPID);
	menu_setbtnmode(np2cfg.BTN_MODE);

	scrnmode = 0;
	if (scrnmng_create(scrnmode) != SUCCESS) {
		TRACETERM();
		macossub_term();
		dosio_term();
		DisposeWindow(hWndMain);
		return(0);
	}

	sysmng_initialize();
	mackbd_initialize();
	pccore_init();
	S98_init();

#if defined(NP2GCC)
	if (np2oscfg.MOUSE_SW) {										// ver0.30
		mouse_running(MOUSE_ON);
	}
#endif
	scrndraw_redraw();
	pccore_reset();

#if defined(USE_RESUME)
	flagload(np2resume);
#endif

	SetEventMask(everyEvent);

	np2running = TRUE;
	while(np2running) {
		if (WaitNextEvent(everyEvent, &event, 0, 0)) {
			eventproc(&event);
		}
		else {
			if (np2oscfg.NOWAIT) {
#if defined(NP2GCC)
				mouse_callback();
#endif
				mackbd_callback();
				pccore_exec(framecnt == 0);
				if (np2oscfg.DRAW_SKIP) {			// nowait frame skip
					framecnt++;
					if (framecnt >= np2oscfg.DRAW_SKIP) {
						processwait(0);
					}
				}
				else {								// nowait auto skip
					framecnt = 1;
					if (timing_getcount()) {
						processwait(0);
					}
				}
			}
			else if (np2oscfg.DRAW_SKIP) {			// frame skip
				if (framecnt < np2oscfg.DRAW_SKIP) {
#if defined(NP2GCC)
                    mouse_callback();
#endif
					mackbd_callback();
					pccore_exec(framecnt == 0);
					framecnt++;
				}
				else {
					processwait(np2oscfg.DRAW_SKIP);
				}
			}
			else {								// auto skip
				if (!waitcnt) {
#if defined(NP2GCC)
                    mouse_callback();
#endif
					mackbd_callback();
					pccore_exec(framecnt == 0);
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
	np2running = FALSE;

	pccore_cfgupdate();

#if defined(USE_RESUME)
	flagsave(np2resume);
#endif

	pccore_term();
	S98_trash();

#if defined(NP2GCC)
	mouse_running(MOUSE_OFF);
#endif

	scrnmng_destroy();

	if (sys_updates	& (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
		initsave();
	}
	TRACETERM();
	macossub_term();
	dosio_term();

	DisposeWindow(hWndMain);

	(void)argc;
	(void)argv;
	return(0);
}


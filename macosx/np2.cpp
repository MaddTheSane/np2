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
#include	"mousemng.h"
#include	"configure.h"
#include	"screenopt.h"


#define	USE_RESUME
#define	NP2OPENING

#ifdef		NP2OPENING
#include	<QuickTime/QuickTime.h>
#define		OPENING_WAIT		1500
#endif



		NP2OSCFG	np2oscfg = {0, 2, 0, 0, 0, 0, 1, 0};

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


// ---- おまじない

#if TARGET_CARBON
#define	AEProc(fn)	NewAEEventHandlerUPP((AEEventHandlerProcPtr)(fn))
#else
#define	AEProc(fn)	NewAEEventHandlerProc(fn)
#endif

static void setUpCarbonEvent(void);

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
#if 0
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
#endif
    OSStatus	err;
    IBNibRef	nibRef;    
    err = CreateNibReference(CFSTR("np2"), &nibRef);
    if (err!=noErr) return;
    err = SetMenuBarFromNib(nibRef, CFSTR("MainMenu"));
    DisposeNibReference(nibRef);
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
            
		case IDM_CONFIGURE:
			initConfig();
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

        case IDM_FONT:
            dialog_font();
            break;
            
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
		case IDM_SCREENOPT:
			initScreenOpt();
			break;

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

#if 0
static void HandleUpdateEvent(EventRecord *pevent) {

	WindowPtr	hWnd;

	hWnd = (WindowPtr)pevent->message;
	BeginUpdate(hWnd);
	scrndraw_redraw();
	EndUpdate(hWnd);
}
#endif

static void HandleMouseDown(EventRecord *pevent) {

	WindowPtr	hWnd;
	Rect		rDrag;

    soundmng_stop();
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

#ifndef NP2GCC
		case inGoAway:
			if (TrackGoAway(hWnd, pevent->where)) { }
			np2running = FALSE;
			break;
#endif
	}
    soundmng_play();
}

#if 0
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
#endif

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

#ifdef		NP2OPENING
static void openingNP2(void) {
    Rect		srt, bounds;
    GrafPtr		port;
    CFURLRef	openingURL;
    CFStringRef	path;
    char		buffer[1024];
    FSRef		fsr;
    FSSpec		fsc;
    PicHandle	pict;
    GraphicsImportComponent	gi;
    
    GetPort(&port);
    SetPortWindowPort(hWndMain);
    const RGBColor col = {0, 0, 0};
    SetRect(&bounds, 0, 0, 640, 400);
    RGBBackColor(&col);
    EraseRect(&bounds);
    
    openingURL=CFURLCopyAbsoluteURL(CFBundleCopyResourcesDirectoryURL(CFBundleGetMainBundle()));
    if (openingURL) {
        path = CFURLCopyFileSystemPath(openingURL, kCFURLPOSIXPathStyle);
        if (path) {
            if (CFStringGetCString(path, buffer, 1024, CFStringGetSystemEncoding())) {
                strcat(buffer, "/nekop2.bmp");
                FSPathMakeRef((const UInt8*)buffer, &fsr, NULL);
                FSGetCatalogInfo(&fsr, kFSCatInfoNone, NULL, NULL, &fsc, NULL);
                if (!GetGraphicsImporterForFile(&fsc, &gi)) {
                    if (!GraphicsImportGetNaturalBounds(gi, &srt)) {
                        OffsetRect( &srt, -srt.left, -srt.top);
                        GraphicsImportSetBoundsRect(gi, &srt);
                        GraphicsImportGetAsPicture(gi, &pict);
                        OffsetRect(&srt, (640-srt.right)/2, (400-srt.bottom)/2);
                        DrawPicture(pict,&srt);
                        QDFlushPortBuffer(GetWindowPort(hWndMain), NULL);
                        KillPicture(pict);
                    }
                    CloseComponent(gi);
                }
            }
            if (path) CFRelease(path);
        }
        if (openingURL) CFRelease(openingURL);
    }
    SetPort(port);
}
#endif


int main(int argc, char *argv[]) {

	Rect		wRect;
#if 0
	EventRecord	event;
#endif
    EventRef		theEvent;
    EventTargetRef	theTarget;
#ifdef OPENING_WAIT
	UINT32		tick;
#endif

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
    setUpCarbonEvent();
	ShowWindow(hWndMain);
#ifdef    NP2OPENING
    openingNP2();
#endif
#ifdef OPENING_WAIT
	tick = GETTICK();
#endif

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
#ifdef OPENING_WAIT
	while((GETTICK() - tick) < OPENING_WAIT);
#endif
	scrndraw_redraw();
	pccore_reset();

#if defined(USE_RESUME)
    if (np2oscfg.resume) {
        flagload(np2resume);
    }
#endif

#if 0
	SetEventMask(everyEvent);
#endif
    theTarget = GetEventDispatcherTarget();
    
	np2running = TRUE;
	while(np2running) {
        if (ReceiveNextEvent(0, NULL,kEventDurationNoWait,true, &theEvent)== noErr)
        {
            SendEventToEventTarget (theEvent, theTarget);
            ReleaseEvent(theEvent);
        }
#if 0
		if (WaitNextEvent(everyEvent, &event, 0, 0)) {
			eventproc(&event);
		}
#endif
		else {
			if (np2oscfg.NOWAIT) {
#if 0
				mackbd_callback();
#endif
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
#if 0
                    mackbd_callback();
#endif
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
#if 0
                    mackbd_callback();
#endif
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
	np2running = FALSE;

	pccore_cfgupdate();

#if defined(USE_RESUME)
    if (np2oscfg.resume) {
        flagsave(np2resume);
    }
#endif

	pccore_term();
	S98_trash();

#if defined(NP2GCC)
	mouse_running(MOUSE_OFF);
#endif

	scrnmng_destroy();

	if (sys_updates & (SYS_UPDATECFG | SYS_UPDATEOSCFG)) {
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

//以下、ごっそりIIxからマージ
static pascal OSStatus np2appevent (EventHandlerCallRef myHandlerChain, EventRef event, void* userData)
{
    UInt32          whatHappened;
    OSStatus        result = eventNotHandledErr;
    
    long		eventClass;
    
    eventClass = GetEventClass(event);
    whatHappened = GetEventKind(event);

    EventRecord         eve;
    ConvertEventRefToEventRecord( event,&eve );

    if (IsDialogEvent(&eve)) return result;

    UInt32 modif;
    GetEventParameter (event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modif);

#if defined(NP2GCC)
    HIPoint delta;
    EventMouseButton buttonKind;
    GetEventParameter (event, kEventParamMouseButton, typeMouseButton, NULL, sizeof(EventMouseButton), NULL, &buttonKind);

	BYTE ret;
#endif
        
    switch (eventClass)
        {
                case kEventClassAppleEvent:  
                    if (whatHappened == kEventAppleEvent) {
                        AEProcessAppleEvent(&eve);
                    }
                    break;

                case kEventClassMouse: 
#if defined(NP2GCC)
                    switch (whatHappened)
                        {
                        case kEventMouseMoved:
                            GetEventParameter (event, kEventParamMouseDelta, typeHIPoint, NULL, sizeof(HIPoint), NULL, &delta);
                            mouse_callback(delta);
                            result = noErr;
                            break;
                       case kEventMouseDown:
                            if (buttonKind == kEventMouseButtonSecondary | modif & controlKey) {
                                ret=mouse_btn(MOUSE_RIGHTDOWN);
                            }
                            else {
                                HandleMouseDown(&eve);
                            }
                            result=noErr;
                            break;
                        case kEventMouseUp:
                            if (buttonKind == kEventMouseButtonSecondary | modif & controlKey) {
                                ret=mouse_btn(MOUSE_RIGHTUP);
                            }
                            else if (buttonKind == kEventMouseButtonTertiary) {
                                mouse_running(MOUSE_XOR);
                                menu_setmouse(np2oscfg.MOUSE_SW ^ 1);
                                sysmng_update(SYS_UPDATECFG);
                            }
                            else {
                                ret=mouse_btn(MOUSE_LEFTUP);
                            }
                            result=noErr;
                            break;    
                        }
#else
						if (whatHappened == kEventMouseDown) {
							HandleMouseDown(&eve);
						}
#endif
                        break;
            default:
                    break; 
        }

	(void)myHandlerChain;
	(void)userData;
    return result; 
}

static pascal OSStatus np2windowevent(EventHandlerCallRef myHandler,  EventRef event, void* userData)
{
    WindowRef	window;
    UInt32		whatHappened;
    OSStatus	result = eventNotHandledErr;    
    long		eventClass;
    static UInt32 backup = 0;
    
    GetEventParameter(event, kEventParamDirectObject, typeWindowRef, NULL,
                         sizeof(window), NULL, &window);
    eventClass = GetEventClass(event);
    whatHappened = GetEventKind(event);
        
    switch (eventClass)
        {        
            case kEventClassWindow:            
                if (whatHappened == kEventWindowClose) {
                    np2running = FALSE;
                    result = noErr;
                }
                break;
            case kEventClassKeyboard:
                UInt32 key;
                GetEventParameter (event, kEventParamKeyCode, typeUInt32, NULL, sizeof(UInt32), NULL, &key);
                UInt32 modif;
                GetEventParameter (event, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(UInt32), NULL, &modif);
                switch (whatHappened)
                {
                    case kEventRawKeyUp:
                        mackbd_f12up(key);
                        result = noErr;
                        break;
                    case kEventRawKeyRepeat:
                        mackbd_f12down(key);
                        result = noErr;
                        break;
                    case kEventRawKeyDown:
                        if (modif & cmdKey) {
                            EventRecord	eve;
                            ConvertEventRefToEventRecord( event,&eve );
                            HandleMenuChoice(MenuEvent(&eve));
                        }
                        else {
                            mackbd_f12down(key);
                        }
                        result = noErr;
                        break;
                    case kEventRawKeyModifiersChanged:
                        if (modif & shiftKey) keystat_senddata(0x70);
                        else keystat_senddata(0x70 | 0x80);
                        if (modif & optionKey) keystat_senddata(0x73);
                        else keystat_senddata(0x73 | 0x80);
                        if (modif & controlKey) keystat_senddata(0x74);
                        else keystat_senddata(0x74 | 0x80);
                        if ((modif & alphaLock) != (backup & alphaLock)) {
                            keystat_senddata(0x71);
                            backup = modif;
                        }
                        result = noErr;
                        break;
                    default: 
                        break;             
                }
            default: 
                break;                
        }

	(void)myHandler;
	(void)userData;
    return result;
}

static const EventTypeSpec appEventList[] = {
				{kEventClassAppleEvent,	kEventAppleEvent},
				{kEventClassMouse,		kEventMouseDown},
#if defined(NP2GCC)
				{kEventClassMouse,		kEventMouseMoved},
				{kEventClassMouse,		kEventMouseUp},
#endif
			};

static const EventTypeSpec windEventList[] = {
				{kEventClassWindow,		kEventWindowClose},
				{kEventClassKeyboard,	kEventRawKeyDown},
				{kEventClassKeyboard,	kEventRawKeyUp},
				{kEventClassKeyboard,	kEventRawKeyRepeat},
				{kEventClassKeyboard,	kEventRawKeyModifiersChanged},
			};


static void setUpCarbonEvent(void) {

	InstallStandardEventHandler(GetWindowEventTarget(hWndMain));
	InstallApplicationEventHandler(NewEventHandlerUPP(np2appevent),
								GetEventTypeCount(appEventList),
								appEventList, 0, NULL);
	InstallWindowEventHandler(hWndMain, NewEventHandlerUPP(np2windowevent),
								GetEventTypeCount(windEventList),
								windEventList, 0, NULL);
}


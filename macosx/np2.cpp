#include	"compiler.h"
#include	"resource.h"
#include	"strres.h"
#include	"np2.h"
#include	"dosio.h"
#include	"commng.h"
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
#include	"soundopt.h"
#include	"macnewdisk.h"
#include	"fdefine.h"
#include	"hid.h"
#include	"midiopt.h"
#include	"macalert.h"
#include	"np2opening.h"

#include	<QuickTime/QuickTime.h>
#define	USE_RESUME
#define	NP2OPENING
// #define	OPENING_WAIT	1500


		NP2OSCFG	np2oscfg = {0, 2, 0, 0, 0, 0, 1, 0};

		WindowPtr	hWndMain;
		BOOL		np2running;
static	UINT		framecnt = 0;
static	UINT		waitcnt = 0;
static	UINT		framemax = 1;
        BYTE		scrnmode;


#define DRAG_THRESHOLD		5

#ifndef NP2GCC
#define	DEFAULTPATH		":"
#else
#define	DEFAULTPATH		"::::"
#endif
static	char	target[MAX_PATH] = DEFAULTPATH;


static const char np2resume[] = "sav";


// ---- おまじない

#define	AEProc(fn)	NewAEEventHandlerUPP((AEEventHandlerProcPtr)(fn))

static void setUpCarbonEvent(void);
static bool setupMainWindow(void);
static void toggleFullscreen(void);

static pascal OSErr handleQuitApp(const AppleEvent *event, AppleEvent *reply,
															long refcon) {

	taskmng_exit();

	(void)event, (void)reply, (void)refcon;
	return(noErr);
}

pascal OSErr OpenAppleEventHandler(const AppleEvent *event, AppleEvent *reply,long print)
{
	long		i,ct,len;
	FSSpec		fsc,*pp;
	DescType	rtype;
	AEKeyword	key;
	AEDescList	dlist;
        
	if( ! AEGetParamDesc( event,keyDirectObject,typeAEList,&dlist ) )	{
		AECountItems( &dlist,&ct );
		for( i=1;i<=ct;i++ )	{
            pp=&fsc;
			if( ! AEGetNthPtr( &dlist,i,typeFSS,&key,&rtype,(Ptr)pp,(long)sizeof(FSSpec),&len ) )	{
                char		fname[MAX_PATH];
                int			ftype;
                fsspec2path(&fsc, fname, MAX_PATH);
                ftype = file_getftype(fname);
                if ((ftype != FTYPE_D88) && (ftype != FTYPE_BETA)) {
                    diskdrv_sethdd(0, fname);
                }
                else {
                    diskdrv_setfdd(i-1, fname, 0);
                }
			}
		}
		AEDisposeDesc( &dlist );
	}
	return( 0 );
}


static void InitToolBox(void) {

	FlushEvents(everyEvent, 0);
	InitCursor();

	AEInstallEventHandler(kCoreEventClass, kAEQuitApplication,
						AEProc(handleQuitApp), 0L, false);
	AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments,
						AEProc(OpenAppleEventHandler), 0L, false);
}

static void MenuBarInit(void) {
#if 1
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
    SetMenuItemModifiers(GetMenuRef(IDM_FDD2), IDM_FDD2OPEN, kMenuOptionModifier);
    SetMenuItemModifiers(GetMenuRef(IDM_FDD2), IDM_FDD2EJECT, kMenuOptionModifier);
    SetMenuItemModifiers(GetMenuRef(IDM_SASI2), IDM_SASI2OPEN, kMenuOptionModifier);
	DrawMenuBar();
#else
    OSStatus	err;
    IBNibRef	nibRef;    
    err = CreateNibReference(CFSTR("np2"), &nibRef);
    if (err!=noErr) return;
    err = SetMenuBarFromNib(nibRef, CFSTR("MainMenu"));
    DisposeNibReference(nibRef);
#endif
}

static void changescreen(BYTE mode) {

	BYTE	change;
	BYTE	renewal;

	change = scrnmode ^ mode;
	renewal = (change & SCRNMODE_FULLSCREEN);
	if (mode & SCRNMODE_FULLSCREEN) {
		renewal |= (change & SCRNMODE_HIGHCOLOR);
	}
	else {
		renewal |= (change & SCRNMODE_ROTATEMASK);
	}
	if (renewal) {
		soundmng_stop();
		mouse_running(MOUSE_STOP);
		scrnmng_destroy();
		if (scrnmng_create(mode) == SUCCESS) {
			scrnmode = mode;
		}
		scrndraw_redraw();
		mouse_running(MOUSE_CONT);
		soundmng_play();
	}
	else {
		scrnmode = mode;
	}
}

static void HandleMenuChoice(long wParam) {

	UINT	update;

    soundmng_stop();
	update = 0;
	switch(wParam) {
		case IDM_ABOUT:
			AboutDialogProc();
			break;

		case IDM_RESET:
            if (ResetWarningDialogProc()) {
                pccore_cfgupdate();
                pccore_reset();
            }
			break;
            
		case IDM_CONFIGURE:
			initConfig();
			break;

		case IDM_NEWDISK:
            newdisk();
			break;
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

		case IDM_FULLSCREEN:
            toggleFullscreen();
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

		case IDM_SCREENOPT:
			initScreenOpt();
			break;

        case IDM_MOUSE:
            mouse_running(MOUSE_XOR);
            menu_setmouse(np2oscfg.MOUSE_SW ^ 1);
            sysmng_update(SYS_UPDATECFG);
			break;
            
        case IDM_MIDIOPT:
            initMidiOpt();
            break;
            
		case IDM_MIDIPANIC:
			rs232c_midipanic();
			mpu98ii_midipanic();
			pc9861k_midipanic();
			break;

        case IDM_SNDOPT:
            initSoundOpt();
            break;

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

		case IDM_BMPSAVE:
			dialog_writebmp();
			break;

        case IDM_S98LOGGING:
            dialog_s98();
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

        case IDM_MSRAPID:
            menu_setmsrapid(np2cfg.MOUSERAPID ^ 1);
            update |= SYS_UPDATECFG;
            break;

		case IDM_I286SAVE:
			debugsub_status();
			break;

        case IDM_RECORDING:
            menu_setrecording(false);
            break;

		default:
			break;
	}
	sysmng_update(update);
	HiliteMenu(0);
}

static void HandleMouseDown(EventRecord *pevent) {

	WindowPtr	hWnd;

    soundmng_stop();
	switch(FindWindow(pevent->where, &hWnd)) {
		case inMenuBar:
			HandleMenuChoice(MenuSelect(pevent->where));
			break;
	}
}


// ----

static void processwait(UINT waitcnt) {

	if (timing_getcount() >= waitcnt) {
		framecnt = 0;
		timing_setcount(0);
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

static void flagdelete(const char *ext) {

	char	path[MAX_PATH];

	getstatfilename(path, ext, sizeof(path));
	file_delete(path);
}

static void flagload(const char *ext) {

	char	path[MAX_PATH];
	char	buf[1024];
	int		ret, r;

    ret = IDOK;
	getstatfilename(path, ext, sizeof(path));
	r = statsave_check(path, buf, sizeof(buf));
	if (r & (~NP2FLAG_DISKCHG)) {
		ResumeErrorDialogProc();
		ret = IDCANCEL;
	}
	else if (r & NP2FLAG_DISKCHG) {
		ret = ResumeWarningDialogProc(buf);
	}
	if (ret == IDOK) {
		statsave_load(path);
	}
	return;
}

int main(int argc, char *argv[]) {

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

    if (!(setupMainWindow())) {
        return(0);
    }

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

	commng_initialize();
	sysmng_initialize();
	mackbd_initialize();
	pccore_init();
	S98_init();

    hid_init();
	if (soundmng_initialize() == SUCCESS) {
		soundmng_pcmvolume(SOUND_PCMSEEK, np2cfg.MOTORVOL);
		soundmng_pcmvolume(SOUND_PCMSEEK1, np2cfg.MOTORVOL);
	}

#if defined(NP2GCC)
	if (np2oscfg.MOUSE_SW) {										// ver0.30
		mouse_running(MOUSE_ON);
	}
#endif
#ifdef OPENING_WAIT
	while((GETTICK() - tick) < OPENING_WAIT);
#endif
	pccore_reset();

#if defined(USE_RESUME)
    if (np2oscfg.resume) {
        flagload(np2resume);
    }
#endif

    theTarget = GetEventDispatcherTarget();
    
	np2running = TRUE;
	while(np2running) {
        if (ReceiveNextEvent(0, NULL,kEventDurationNoWait,true, &theEvent)== noErr)
        {
            SendEventToEventTarget (theEvent, theTarget);
            ReleaseEvent(theEvent);
        }
		else {
            soundmng_play();
			if (np2oscfg.NOWAIT) {
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
						processwait(0);
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
    menu_setrecording(true);

    if (scrnmode & SCRNMODE_FULLSCREEN) {
        toggleFullscreen();
    }
    
	pccore_cfgupdate();

#if defined(USE_RESUME)
    if (np2oscfg.resume) {
        flagsave(np2resume);
    }
	else {
		flagdelete(np2resume);
	}
#endif

	pccore_term();
	S98_trash();

    hid_clear();
#if defined(NP2GCC)
	mouse_running(MOUSE_OFF);
#endif

	soundmng_deinitialize();
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
                switch (whatHappened)
                {
                    case kEventWindowClose:
                        np2running = FALSE;
                        result = noErr;
                        break;
                    case kEventWindowShowing:
                        scrndraw_redraw();
                       break;
                    case kEventWindowActivated:
                        DisableAllMenuItems(GetMenuHandle(IDM_EDIT));
                        break;
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
				{kEventClassWindow,		kEventWindowShowing},
				{kEventClassWindow,		kEventWindowActivated},
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
    InstallStandardEventHandler(GetWindowEventTarget(hWndMain));
}

bool setupMainWindow(void) {
#if defined(NP2GCC)
    OSStatus	err;
    IBNibRef	nibRef;

    err = CreateNibReference(CFSTR("np2"), &nibRef);
    if (err ==noErr ) {
        CreateWindowFromNib(nibRef, CFSTR("MainWindow"), &hWndMain);
        DisposeNibReference ( nibRef);
    }
    else {
         return(false);
    }
    
#else
    Rect wRect;
    
	SetRect(&wRect, 100, 100, 100, 100);
	hWndMain = NewWindow(0, &wRect, "\pNeko Project II", FALSE,
								noGrowDocProc, (WindowPtr)-1, TRUE, 0);
	if (!hWndMain) {
		TRACETERM();
		macossub_term();
		dosio_term();
		return(false);
	}
	SizeWindow(hWndMain, 640, 400, TRUE);
#endif
	scrnmng_initialize();
    setUpCarbonEvent();
	ShowWindow(hWndMain);
    return(true);
}

void toggleFullscreen(void) {
    static Ptr 	bkfullscreen;
    static BYTE mouse = 0;

    soundmng_stop();
    if (!scrnmode & SCRNMODE_FULLSCREEN) {
        RGBColor col = {0, 0, 0};
        short	w=640, h=480;
        DisposeWindow(hWndMain);
        BeginFullScreen(&bkfullscreen,0,&w,&h,&hWndMain,&col,(fullScreenAllowEvents | fullScreenDontChangeMenuBar));	
        HideMenuBar();
        setUpCarbonEvent();
        if (!np2oscfg.MOUSE_SW) {
            mouse = np2oscfg.MOUSE_SW;
            mouse_running(MOUSE_ON);
            menu_setmouse(1);
        }
        changescreen(scrnmode | SCRNMODE_FULLSCREEN);
    }
    else {
        scrnmng_destroy();
        EndFullScreen(bkfullscreen, 0);
        setupMainWindow();
        changescreen(scrnmode & (~SCRNMODE_FULLSCREEN));
        if (!mouse) {
            mouse_running(MOUSE_OFF);
            menu_setmouse(0);
        }
        ShowMenuBar();
    }
    CheckMenuItem(GetMenuHandle(IDM_SCREEN), LoWord(IDM_FULLSCREEN), scrnmode & SCRNMODE_FULLSCREEN);
    soundmng_play();
}

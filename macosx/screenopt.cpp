/*
 *  screenopt.cpp
 *  np2
 *
 *  Created by tk800 on Fri Oct 24 2003.
 *
 */

#include	"compiler.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"scrndraw.h"
#include	"palettes.h"
#include	"configure.h"
#include	"dialogutils.h"

#define	getControlValue(a)		GetControl32BitValue(getControlRefByID(a,0,screenWin))
#define	setControlValue(a,b)	SetControl32BitValue(getControlRefByID(a,0,screenWin),b)

#define	kMaxNumTabs 3
enum {kTabMasterSig = 'ScrT',kTabMasterID = 1000,kTabPaneSig= 'ScTb'};

static WindowRef	screenWin;
static UInt16 lastPaneSelected = 1;	// static, to keep track of it long term (in a more complex application
                                        // you might store this in a data structure in the window refCon)                                            


static void setFieldValue(UInt32 type) {
    ControlRef	conRef;
    Str255		title;
    SInt32		value;
    
    value = getControlValue(type); 
    if (type == 'RPAd') {
        value -= 31;
    }
    NumToString(value, title);
    conRef = getControlRefByID(type, 1, screenWin);
	SetControlData(conRef, kControlNoPart, kControlStaticTextTextTag, *title, title+1);
    Draw1Control(conRef);
}

static void setReverseStatus(void) {
    if (getControlValue('LCDy')) {
        ActivateControl(getControlRefByID('rvrs', 0, screenWin));
    }
    else {
        DeactivateControl(getControlRefByID('rvrs', 0, screenWin));
    }
}

static void initScreenWindow(void) {
    setControlValue('LCDy', np2cfg.LCD_MODE & 1);
    setReverseStatus();
    setControlValue('rvrs', np2cfg.LCD_MODE & 2?1:0);
    setControlValue('uskr', np2cfg.skipline);
    setControlValue('rati', np2cfg.skiplight);
    setFieldValue('rati');
    
    setControlValue('GDCl', np2cfg.uPD72020 + 1);
    setControlValue('GrCh', (np2cfg.grcg & 3) + 1);
    setControlValue('16cl', np2cfg.color16);
    
    setControlValue('tram', np2cfg.wait[0]);
    setFieldValue('tram');
    setControlValue('vram', np2cfg.wait[2]);
    setFieldValue('vram');
    setControlValue('crgc', np2cfg.wait[4]);
    setFieldValue('crgc');
    setControlValue('RPAd', np2cfg.realpal);
    setFieldValue('RPAd');
    
}

static pascal OSStatus cfWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
    HICommand	cmd;
    SINT32	val;
	UInt8	value[6];
	UInt8	b;
	UInt8	update;
	int		renewal;

    if (GetEventClass(event)==kEventClassCommand && GetEventKind(event)==kEventCommandProcess ) {
        GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
        switch (cmd.commandID)
        {
            case 'LCDy':
                setReverseStatus();
                break;
                
            case 'rati':
            case 'tram':
            case 'vram':
            case 'crgc':
            case 'RPAd':
                setFieldValue(cmd.commandID);
                break;

            case kHICommandOK:
                renewal = 0;
                val=getControlValue('uskr');
				if (np2cfg.skipline != val) {
					np2cfg.skipline = val;
					renewal = 1;
				}
                val=getControlValue('rati');
                if (val != np2cfg.skiplight);
				if (renewal) {
					pal_makeskiptable();
				}
                val=getControlValue('LCDy') | getControlValue('rvrs') << 1;
				if (np2cfg.LCD_MODE != val) {
					np2cfg.LCD_MODE = val;
					pal_makelcdpal();
					renewal = 1;
				}
				if (renewal) {
					sysmng_update(SYS_UPDATECFG);
					scrndraw_redraw();
				}

                update = 0;
                val=getControlValue('GDCl')-1;
				if (np2cfg.uPD72020 != val) {
					np2cfg.uPD72020 = val;
					update |= SYS_UPDATECFG;
					gdc_restorekacmode();
					gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
				}
                val=getControlValue('GrCh')-1;
				if (np2cfg.grcg != val) {
					np2cfg.grcg = val;
					update |= SYS_UPDATECFG;
					gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
				}
                val=getControlValue('16cl');
				if (np2cfg.color16 != val) {
					np2cfg.color16 = val;
					update |= SYS_UPDATECFG;
				}
    
                value[0]=getControlValue('tram');
				if (value[0]) {
					value[1] = 1;
				}
                value[2]=getControlValue('vram');
				if (value[0]) {
					value[3] = 1;
				}
                value[4]=getControlValue('crgc');
				if (value[0]) {
					value[5] = 1;
				}
				for (b=0; b<6; b++) {
					if (np2cfg.wait[b] != value[b]) {
						np2cfg.wait[b] = value[b];
						update |= SYS_UPDATECFG;
					}
				}
                val=getControlValue('RPAd');
                if (val != np2cfg.realpal) {
                    np2cfg.realpal = val;
					update |= SYS_UPDATECFG;
                }
                
                sysmng_update(update);
                endLoop(screenWin);
                err=noErr;
                break;
                
            case kHICommandCancel:
                endLoop(screenWin);
                err=noErr;
                break;
        }
    }

	(void)myHandler;
	(void)userData;
    return err;
}

static pascal OSStatus PrefsTabEventHandlerProc( EventHandlerCallRef inCallRef, EventRef inEvent, void* inUserData )
{
    WindowRef theWindow = (WindowRef)inUserData;  // get the windowRef, passed around as userData    
    short ret;
    ret = changeTab(theWindow, lastPaneSelected);
    if (ret) {
        lastPaneSelected = ret;
    }
    return( eventNotHandledErr );
}

static void makeNibWindow (IBNibRef nibRef) {
    OSStatus	err;
    
    err = CreateWindowFromNib(nibRef, CFSTR("ScreenDialog"), &screenWin);
    if (err == noErr) {
        initScreenWindow();
        SetInitialTabState(screenWin, lastPaneSelected, kMaxNumTabs);
        EventTypeSpec	tabControlEvents[] ={ { kEventClassControl, kEventControlHit }};
        InstallControlEventHandler( getControlRefByID(kTabMasterSig,kTabMasterID,screenWin),  PrefsTabEventHandlerProc , GetEventTypeCount(tabControlEvents), tabControlEvents, screenWin, NULL );
        EventTypeSpec	list[]={ { kEventClassCommand, kEventCommandProcess },};
        EventHandlerRef	ref;
        InstallWindowEventHandler (screenWin, NewEventHandlerUPP(cfWinproc), GetEventTypeCount(list), list, (void *)screenWin, &ref);
        ShowSheetWindow(screenWin, hWndMain);
        
        err=RunAppModalLoopForWindow(screenWin);
    }
    return;
}

void initScreenOpt( void ) {
    OSStatus	err;
    IBNibRef	nibRef;

    err = CreateNibReference(CFSTR("np2"), &nibRef);
    if (err ==noErr ) {
        makeNibWindow (nibRef);
        DisposeNibReference ( nibRef);
    }
    else {
         return;
    }
}
                             
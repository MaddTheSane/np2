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

static WindowRef	screenWin;
static void SetInitialTabState(WindowRef theWindow);
static pascal OSStatus PrefsTabEventHandlerProc( EventHandlerCallRef inCallRef, EventRef inEvent, void* inUserData );

enum {kTabMasterSig = 'ScrT',kTabMasterID = 1000,kTabPaneSig= 'ScTb',kMaxNumTabs= 3};

static void endLoop(void) {
    OSStatus	err;
    HideSheetWindow(screenWin);
    DisposeWindow(screenWin);
    err=QuitAppModalLoopForWindow(screenWin);
}

static void setFieldValue(UInt32 type) {
    ControlRef	conRef;
    Str255		title;
    SInt32		value;
    
    value = GetControl32BitValue(getControlRefByID(type, 0, screenWin)); 
    if (type == 'RPAd') {
        value -= 31;
    }
    NumToString(value, title);
    conRef = getControlRefByID(type, 1, screenWin);
	SetControlData(conRef, kControlNoPart, kControlStaticTextTextTag, *title, title+1);
    Draw1Control(conRef);
}

static void setReverseStatus(void) {
    if (GetControl32BitValue(getControlRefByID('LCDy', 0, screenWin))) {
        ActivateControl(getControlRefByID('rvrs', 0, screenWin));
    }
    else {
        DeactivateControl(getControlRefByID('rvrs', 0, screenWin));
    }
}

static void initScreenWindow(void) {
    SetControl32BitValue(getControlRefByID('LCDy', 0, screenWin), np2cfg.LCD_MODE & 1);
    setReverseStatus();
    SetControl32BitValue(getControlRefByID('rvrs', 0, screenWin), np2cfg.LCD_MODE & 2?1:0);
    SetControl32BitValue(getControlRefByID('uskr', 0, screenWin), np2cfg.skipline);
    SetControl32BitValue(getControlRefByID('rati', 0, screenWin), np2cfg.skiplight);
    setFieldValue('rati');
    
    SetControl32BitValue(getControlRefByID('GDCl', 0, screenWin), np2cfg.uPD72020 + 1);
    SetControl32BitValue(getControlRefByID('GrCh', 0, screenWin), (np2cfg.grcg & 3) + 1);
    SetControl32BitValue(getControlRefByID('16cl', 0, screenWin), np2cfg.color16);
    
    SetControl32BitValue(getControlRefByID('tram', 0, screenWin), np2cfg.wait[0]);
    setFieldValue('tram');
    SetControl32BitValue(getControlRefByID('vram', 0, screenWin), np2cfg.wait[2]);
    setFieldValue('vram');
    SetControl32BitValue(getControlRefByID('crgc', 0, screenWin), np2cfg.wait[4]);
    setFieldValue('crgc');
    SetControl32BitValue(getControlRefByID('RPAd', 0, screenWin), np2cfg.realpal);
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
                val=GetControl32BitValue(getControlRefByID('uskr', 0, screenWin));
				if (np2cfg.skipline != val) {
					np2cfg.skipline = val;
					renewal = 1;
				}
                val=GetControl32BitValue(getControlRefByID('rati', 0, screenWin));
                if (val != np2cfg.skiplight);
				if (renewal) {
					pal_makeskiptable();
				}
                val=GetControl32BitValue(getControlRefByID('LCDy', 0, screenWin)) |
                    GetControl32BitValue(getControlRefByID('rvrs', 0, screenWin)) << 1;
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
                val=GetControl32BitValue(getControlRefByID('GDCl', 0, screenWin))-1;
				if (np2cfg.uPD72020 != val) {
					np2cfg.uPD72020 = val;
					update |= SYS_UPDATECFG;
					gdc_restorekacmode();
					gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
				}
                val=GetControl32BitValue(getControlRefByID('GrCh', 0, screenWin))-1;
				if (np2cfg.grcg != val) {
					np2cfg.grcg = val;
					update |= SYS_UPDATECFG;
					gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
				}
                val=GetControl32BitValue(getControlRefByID('16cl', 0, screenWin));
				if (np2cfg.color16 != val) {
					np2cfg.color16 = val;
					update |= SYS_UPDATECFG;
				}
    
                value[0]=GetControl32BitValue(getControlRefByID('tram', 0, screenWin));
				if (value[0]) {
					value[1] = 1;
				}
                value[2]=GetControl32BitValue(getControlRefByID('vram', 0, screenWin));
				if (value[0]) {
					value[3] = 1;
				}
                value[4]=GetControl32BitValue(getControlRefByID('crgc', 0, screenWin));
				if (value[0]) {
					value[5] = 1;
				}
				for (b=0; b<6; b++) {
					if (np2cfg.wait[b] != value[b]) {
						np2cfg.wait[b] = value[b];
						update |= SYS_UPDATECFG;
					}
				}
                val=GetControl32BitValue(getControlRefByID('RPAd', 0, screenWin));
                if (val != np2cfg.realpal) {
                    np2cfg.realpal = val;
					update |= SYS_UPDATECFG;
                }
                
                sysmng_update(update);
                endLoop();
                err=noErr;
                break;
                
            case kHICommandCancel:
                endLoop();
                err=noErr;
                break;
        }
    }

	(void)myHandler;
	(void)userData;
    return err;
}

static void makeNibWindow (IBNibRef nibRef) {
    OSStatus	err;
    
    err = CreateWindowFromNib(nibRef, CFSTR("ScreenDialog"), &screenWin);
    if (err == noErr) {
        initScreenWindow();
        SetInitialTabState(screenWin);
        EventTypeSpec	tabControlEvents[] ={ { kEventClassControl, kEventControlHit }};
        InstallControlEventHandler( getControlRefByID(kTabMasterSig,kTabMasterID,screenWin),  PrefsTabEventHandlerProc , GetEventTypeCount(tabControlEvents), tabControlEvents, screenWin, NULL );
        EventTypeSpec	list[]={
        { kEventClassCommand, kEventCommandProcess },
        };
        EventHandlerRef	ref;
        InstallWindowEventHandler (screenWin, NewEventHandlerUPP(cfWinproc), sizeof(list)/sizeof(EventTypeSpec), list, (void *)screenWin, &ref);
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

static UInt16 lastPaneSelected = 1;	// static, to keep track of it long term (in a more complex application
                                        // you might store this in a data structure in the window refCon)                                            

static void SetInitialTabState(WindowRef theWindow)
{
    short qq;

    for(qq=0;qq<kMaxNumTabs+1;qq++)
    SetControlVisibility( getControlRefByID(  kTabPaneSig,  kTabMasterID+qq, theWindow), false, true );  
    
    SetControlValue(getControlRefByID(kTabMasterSig,kTabMasterID,theWindow),lastPaneSelected );
    SetControlVisibility( getControlRefByID(  kTabPaneSig,  kTabMasterID+lastPaneSelected, theWindow), true, true );
}

static pascal OSStatus PrefsTabEventHandlerProc( EventHandlerCallRef inCallRef, EventRef inEvent, void* inUserData )
{
    WindowRef theWindow = (WindowRef)inUserData;  // get the windowRef, passed around as userData    
    short controlValue = 2;
    controlValue = GetControlValue( getControlRefByID(kTabMasterSig,kTabMasterID,theWindow) );
    if ( controlValue != lastPaneSelected )
    {
        SetControlVisibility( getControlRefByID(  kTabPaneSig,  kTabMasterID+lastPaneSelected, theWindow), false, true );
        SetControlVisibility( getControlRefByID(  kTabPaneSig,  kTabMasterID+controlValue, theWindow), true, true );    

        Draw1Control( getControlRefByID(kTabMasterSig,kTabMasterID,theWindow) );		
        lastPaneSelected= controlValue;    
    }
    
    return( eventNotHandledErr );
}

                             
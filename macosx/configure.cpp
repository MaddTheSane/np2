/*
 *  configure.cpp
 *  drom Neko Project IIx 0.3
 *
 *  Created by tk800 on Mon Sep 23 2002.
 *
 */

#include	"compiler.h"
#include	"np2.h"
#include	"pccore.h"
#include	"ini.h"
#include	"sysmng.h"
#include	"configure.h"

        int			sound_renewals = 0;
static	WindowRef	configWin;

#define	AVE(a, b)					\
			(((a) + (b)) / 2)

static void endLoop(void) {
    OSStatus	err;
    HideSheetWindow(configWin);
    DisposeWindow(configWin);
    err=QuitAppModalLoopForWindow(configWin);
}

ControlRef getControlRefByID(OSType sign, int id, WindowRef win) {
    ControlRef	conRef;
    ControlID	conID;

    conID.signature=sign;
    conID.id=id;
    GetControlByID(win, &conID, &conRef);
    return conRef;
}    

static SInt16 getSelectedValue(OSType sign, int id) {
    SInt16	value;
    
    value=GetControlValue(getControlRefByID(sign, id, configWin));

    return value;
}

static int getMultiple(void) {
    int multi;
    switch (getSelectedValue('Mltp', 5))
    {
        case 1:
            multi=1;
            break;
        case 2:
            multi=2;
            break;
        case 3:
            multi=4;
            break;
        case 4:
            multi=5;
            break;
        case 5:
            multi=6;
            break;
        case 6:
            multi=8;
            break;
        case 7:
            multi=10;
            break;
        case 8:
            multi=12;
            break;
        case 9:
            multi=16;
            break;
        case 10:
            multi=20;
            break;
        default:
            multi=4;
            break;
    }
    return multi;
}

static void setCPUClock(void) {
    unsigned int	base, clock;
    ControlRef	conRef;
    char	outstr[1024+64+2];
    int	length;

    if (getSelectedValue('CPU ', 3)==1) {
        base=24576;
    }
    else {
        base=19968;
    }
    clock = base * getMultiple();
    conRef=getControlRefByID('CKtx', 6, configWin);
    
    sprintf(outstr, " %2u.%04u", clock / 10000, clock % 10000);
    length=strlen(outstr);
    SetControlData(conRef,kControlNoPart,kControlStaticTextTextTag,length,outstr);
}

static void initConfigWindow(void) {
        SInt16	i;
        Str255	title;
    
        if (np2cfg.baseclock >= AVE(PCBASECLOCK25, PCBASECLOCK20)) {
            i=1;
        }
        else {
            i=2;
        }
        SetControlValue(getControlRefByID('CPU ', 3, configWin), i);

        switch (np2cfg.multiple)
        {
            case 1:
                i=1;
                break;
            case 2:
                i=2;
                break;
            case 4:
                i=3;
                break;
            case 5:
                i=4;
                break;
            case 6:
                i=5;
                break;
            case 8:
                i=6;
                break;
            case 10:
                i=7;
                break;
            case 12:
                i=8;
                break;
            case 16:
                i=9;
                break;
            case 20:
                i=10;
                break;
            default:
                i=4;
                break;
        }
        SetControlValue(getControlRefByID('Mltp', 5, configWin), i);
        setCPUClock();
        NumToString(np2cfg.delayms, title);
        if (np2cfg.samplingrate < AVE(11025, 22050)) {
            i=1;
        }
        else if (np2cfg.samplingrate < AVE(22050, 44100)) {
            i=2;
        }
        else {
            i=3;
        }
        SetControlValue(getControlRefByID('Rate', 8, configWin), i);
        NumToString(np2cfg.delayms, title);
        SetControlData(getControlRefByID('Bufr', 7, configWin), kControlNoPart, kControlStaticTextTextTag, *title, title+1);

}

static pascal OSStatus cfWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
    HICommand	cmd;
    UINT32	dval;
    UINT16	wval;
	UINT	update;

    if (GetEventClass(event)==kEventClassCommand && GetEventKind(event)==kEventCommandProcess ) {
        GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
        switch (cmd.commandID)
        {
            case 'base':
                setCPUClock();
                break;

            case 'mult':
                setCPUClock();
                break;

            case kHICommandOK:
                update = 0;
                if (getSelectedValue('CPU ', 3)!=1) {
                    dval=PCBASECLOCK20;
                }
                else {
                    dval=PCBASECLOCK25;
                }
                if (dval != np2cfg.baseclock) {
                    np2cfg.baseclock = dval;
                    update |= SYS_UPDATECFG | SYS_UPDATECLOCK;
                }
                dval=getMultiple();
                if (dval != np2cfg.multiple) {
                    np2cfg.multiple = dval;
                    update |= SYS_UPDATECFG | SYS_UPDATECLOCK;
                }
                
                dval=getSelectedValue('Rate', 8);
                if (dval==1) {
                    wval = 11025;
                }
                else if (dval==2) {
                    wval = 22050;
                }
                else {
                    wval = 44100;
                }
                if (wval != np2cfg.samplingrate) {
                    np2cfg.samplingrate = wval;
                    update |= SYS_UPDATECFG | SYS_UPDATERATE;
                    soundrenewal = 1;
                }
                

                {
                    Size	size, outSize;
                    ControlRef	cRef;
                    char	buffer[255];
                    char*	pt;
                    char*	retPtr;

                    pt=&buffer[0];
                    cRef=getControlRefByID('Bufr', 7, configWin);
                    GetControlDataSize(cRef, kControlNoPart, kControlStaticTextTextTag, &size);
                    GetControlData(cRef, kControlNoPart, kControlStaticTextTextTag, size, pt, &outSize);
                    *(pt+outSize)=NULL;
                    dval=strtoul(pt, &retPtr, 10);
					if (dval < 100) {
						dval = 100;
					}
					else if (dval > 1000) {
						dval = 1000;
					}
                    if (dval!= np2cfg.delayms) {
						soundrenewal = 1;
						np2cfg.delayms = dval;
						update |= SYS_UPDATECFG | SYS_UPDATESBUF;
                    }
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

    return err;
}


static void makeNibWindow (IBNibRef nibRef) {
    OSStatus	err;
    
    err = CreateWindowFromNib(nibRef, CFSTR("ConfigDialog"), &configWin);
    if (err == noErr) {
        initConfigWindow();
        EventTypeSpec	list[]={ { kEventClassCommand, kEventCommandProcess },
        { kEventClassWindow, kEventWindowActivated } };
        EventHandlerRef	ref;
        
        InstallWindowEventHandler (configWin, NewEventHandlerUPP(cfWinproc), 2, list, (void *)configWin, &ref);
        ShowSheetWindow(configWin, hWndMain);
        
        err=RunAppModalLoopForWindow(configWin);
    }
    return;
}

void initConfig( void ) {
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



                             
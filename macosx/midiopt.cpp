/*
 *  midiopt.cpp
 *  np2
 *
 *  Created by tk800 on Fri Oct 31 2003.
 *
 */

#include "compiler.h"
#include "np2.h"
#include "pccore.h"
#include "sysmng.h"
#include "dialogutils.h"
#include "midiopt.h"

#define	setControlValue(a,b,c)		SetControl32BitValue(getControlRefByID(a,b,midiWin),c)
#define getMenuValue				(GetControl32BitValue(getControlRefByID(cmd.commandID,0,midiWin))-1)

static WindowRef	midiWin;
static BYTE			mpu = 0;

static void setMPUs(void) {
    setControlValue('MPio', 0, ((mpu >> 4) & 15)+1);
    setControlValue('MPin', 0, (mpu & 3)+1);
}

static pascal OSStatus cfWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
    HICommand	cmd;
	UInt8	update;

    if (GetEventClass(event)==kEventClassCommand && GetEventKind(event)==kEventCommandProcess ) {
        GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
        switch (cmd.commandID)
        {
            case 'MPDF':
                mpu = 0x82;
                setMPUs();
                break;
                
            case 'MPio':
                setjmper(&mpu, getMenuValue << 4, 0xf0);
                break;

            case 'MPin':
                setjmper(&mpu, getMenuValue, 0x03);
                break;

            case kHICommandOK:
                update = 0;
                if (np2cfg.mpuopt != mpu) {
                    np2cfg.mpuopt = mpu;
                    update |= SYS_UPDATECFG | SYS_UPDATEMIDI;
                }
                sysmng_update(update);
                                
                endLoop(midiWin);
                err=noErr;
                break;
                
            case kHICommandCancel:
                endLoop(midiWin);
                err=noErr;
                break;
                
            default:
                break;
        }
    }

	(void)myHandler;
	(void)userData;
    return err;
}

static void initMidiWindow(void) {
    mpu = np2cfg.mpuopt;
    uncheckAllPopupMenuItems('MPio', 16, midiWin);
    uncheckAllPopupMenuItems('MPin', 4, midiWin);
    setMPUs();
}

static void makeNibWindow (IBNibRef nibRef) {
    OSStatus	err;
    EventHandlerRef	ref;
    
    err = CreateWindowFromNib(nibRef, CFSTR("MidiDialog"), &midiWin);
    if (err == noErr) {
    
        initMidiWindow();
        EventTypeSpec	list[]={ { kEventClassCommand, kEventCommandProcess },};
        InstallWindowEventHandler (midiWin, NewEventHandlerUPP(cfWinproc), GetEventTypeCount(list), list, (void *)midiWin, &ref);
        ShowSheetWindow(midiWin, hWndMain);
        
        err=RunAppModalLoopForWindow(midiWin);
    }
    return;
}

void initMidiOpt( void ) {
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

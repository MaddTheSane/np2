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
#include "dialog.h"
#include "dialogutils.h"
#include "midiopt.h"
#include "dipswbmp.h"

#define	setControlValue(a,b,c)		SetControl32BitValue(getControlRefByID(a,b,midiWin),c)
#define getMenuValue				(GetControl32BitValue(getControlRefByID(cmd.commandID,0,midiWin))-1)

static WindowRef	midiWin;
static BYTE			mpu = 0;
static COMCFG		mpucfg;

extern const char cmmidi_midimapper[];
extern char cmmidi_vermouth[];
extern const char *cmmidi_mdlname[];

static const char *midiout_name[3] = {"", cmmidi_midimapper, cmmidi_vermouth};

static UINT module2number(const char *module) {

	UINT	i;

	for (i=0; i<12; i++) {
		if (!milstr_extendcmp(module, cmmidi_mdlname[i])) {
			break;
		}
	}
	return(i);
}

static void setMIMPIFilename(void) {
	ControlRef  conRef = NULL;
	
	conRef = getControlRefByID('mnam', 0, midiWin);
	SetControlData(conRef, kControlNoPart, kControlStaticTextTextTag, strlen(mpucfg.def), mpucfg.def);
	Draw1Control(conRef);
}


static void setMPUs(void) {
	int		data = 1;
	UINT	i;
	
    setControlValue('MPio', 0, ((mpu >> 4) & 15)+1);
    setControlValue('MPin', 0, (mpu & 3)+1);

	if (!milstr_cmp(mpucfg.mout, cmmidi_midimapper)) {
		data = 2;
	}
#if defined(VERMOUTH_LIB)
	else if (!milstr_cmp(mpucfg.mout, cmmidi_vermouth)) {
		data = 3;
	}
#endif
    setControlValue('mido', 0, data);
    setControlValue('midi', 0, 1);//Not available
	i = module2number(mpucfg.mdl);
    setControlValue('midm', 0, i+1);
	
	setControlValue('usem', 0, mpucfg.def_en);
	setMIMPIFilename();
	
	PicHandle   pict;
	ControlRef  disp = getControlRefByID('BMP ', 0, midiWin);
	setbmp(dipswbmp_getmpu(mpu), &pict);
	SetControlData(disp, kControlNoPart, kControlPictureHandleTag, sizeof(PicHandle), &pict);
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
				strcpy(mpucfg.mout, "");
				strcpy(mpucfg.min, "");
				strcpy(mpucfg.mdl, "GM");
				strcpy(mpucfg.def, "");
				mpucfg.def_en = 0;
                setMPUs();
                break;
                
            case 'MPio':
                setjmper(&mpu, getMenuValue << 4, 0xf0);
                break;

            case 'MPin':
                setjmper(&mpu, getMenuValue, 0x03);
                break;

            case 'mido':
				strcpy(mpucfg.mout, midiout_name[getMenuValue]);
                break;

            case 'midi':
				strcpy(mpucfg.min, midiout_name[getMenuValue]);
                break;

            case 'midm':
				strcpy(mpucfg.mdl, cmmidi_mdlname[getMenuValue]);
                break;

            case 'usem':
				mpucfg.def_en = GetControl32BitValue(getControlRefByID(cmd.commandID, 0, midiWin));
                break;

            case 'opnm':
				dialog_fileselect(mpucfg.def, sizeof(mpucfg.def), NULL, OPEN_MIMPI);
				setMIMPIFilename();
                break;

            case kHICommandOK:
                update = 0;
                if (np2cfg.mpuopt != mpu) {
                    np2cfg.mpuopt = mpu;
                    update |= SYS_UPDATECFG | SYS_UPDATEMIDI;
                }
                if (milstr_extendcmp(np2oscfg.mpu.mout, mpucfg.mout) ||
					milstr_extendcmp(np2oscfg.mpu.min, mpucfg.min) ||
					milstr_extendcmp(np2oscfg.mpu.mdl, mpucfg.mdl) ||
					milstr_extendcmp(np2oscfg.mpu.def, mpucfg.def) ||
					(np2oscfg.mpu.def_en != mpucfg.def_en)) {
                    np2oscfg.mpu = mpucfg;
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
	else if (GetEventClass(event)==kEventClassWindow && GetEventKind(event)==kEventWindowShowing ) {
		//setMPUs();
		//SysBeep(0);
	}

	(void)myHandler;
	(void)userData;
    return err;
}

static void initMidiWindow(void) {
    mpu = np2cfg.mpuopt;
	mpucfg = np2oscfg.mpu;
    uncheckAllPopupMenuItems('MPio', 16, midiWin);
    uncheckAllPopupMenuItems('MPin', 4, midiWin);
    uncheckAllPopupMenuItems('mido', 0, midiWin);
    uncheckAllPopupMenuItems('midi', 0, midiWin);
    uncheckAllPopupMenuItems('midm', 0, midiWin);
    setMPUs();
}

static void makeNibWindow (IBNibRef nibRef) {
    OSStatus	err;
    EventHandlerRef	ref;
    
    err = CreateWindowFromNib(nibRef, CFSTR("MidiDialog"), &midiWin);
    if (err == noErr) {
    
        initMidiWindow();
        EventTypeSpec	list[]={ { kEventClassCommand, kEventCommandProcess },
								 { kEventClassWindow,  kEventWindowShowing} };
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

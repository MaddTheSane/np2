/*
 *  soundopt.cpp
 *  np2
 *
 *  Created by tk800 on Sun Oct 26 2003.
 *
 */


#include	"compiler.h"
#include	"np2.h"
#include	"pccore.h"
#include	"sysmng.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"dialogutils.h"

#define	getControlValue(a)		GetControl32BitValue(getControlRefByID(a,0,soundWin))
#define	setControlValue(a,b)	SetControl32BitValue(getControlRefByID(a,0,soundWin),b)

#define	kMaxNumTabs 6
enum {kTabMasterSig = 'ScrT',kTabMasterID = 1000,kTabPaneSig= 'ScTb'};

static WindowRef	soundWin;
static UInt16 lastPaneSelected = 1;	// static, to keep track of it long term (in a more complex application
                                        // you might store this in a data structure in the window refCon)                                            


static void setFieldValue(UInt32 type) {
    ControlRef	conRef;
    Str255		title;
    SInt32		value;
    
    value = getControlValue(type); 
    NumToString(value, title);
    conRef = getControlRefByID(type, 1, soundWin);
	SetControlData(conRef, kControlNoPart, kControlStaticTextTextTag, *title, title+1);
    Draw1Control(conRef);
}

static	BYTE			snd26 = 0;
static	BYTE			snd86 = 0;
static	BYTE			spb = 0;
static	BYTE			spbvrc = 0;

static void set26jmp(BYTE value, BYTE bit) {

	if ((snd26 ^ value) & bit) {
		snd26 &= ~bit;
		snd26 |= value;
	}
}

static void initSoundWindow(void) {
    setControlValue('vFM ', np2cfg.vol_fm);
    setFieldValue('vFM ');
    setControlValue('vPSG', np2cfg.vol_ssg);
    setFieldValue('vPSG');
    setControlValue('vADP', np2cfg.vol_adpcm);
    setFieldValue('vADP');
    setControlValue('vPCM', np2cfg.vol_pcm);
    setFieldValue('vPCM');
    setControlValue('vRtm', np2cfg.vol_rhythm);
    setFieldValue('vRtm');
    
    setControlValue('vLft', np2cfg.vol14[0]);
    setFieldValue('vLft');
    setControlValue('vRit', np2cfg.vol14[1]);
    setFieldValue('vRit');
    setControlValue('vf2 ', np2cfg.vol14[2]);
    setFieldValue('vf2 ');
    setControlValue('vf4 ', np2cfg.vol14[3]);
    setFieldValue('vf4 ');
    setControlValue('vf8 ', np2cfg.vol14[4]);
    setFieldValue('vf8 ');
    setControlValue('vf16', np2cfg.vol14[5]);
    setFieldValue('vf16');
    
    snd26 = np2cfg.snd26opt;
    {
        BYTE para;
        static short paranum[4] = {0, 3, 1, 2};
        setControlValue('26io', ((snd26 >> 4) & 1)+1);
        setControlValue('26in', paranum[(snd26 >> 6) & 3]+1);
        para = snd26 & 7;
        if (para > 4) {
            para = 4;
        }
        setControlValue('26rm', para+1);
    }
    snd86 = np2cfg.snd86opt;
    {
        setControlValue('86io', ((~snd86) & 1)+1);
        static short paranum[4] = {0, 1, 3, 2};
        setControlValue('86in', paranum[(snd86 >> 2) & 3]+1);
        setControlValue('86id', (((~snd86) >> 5) & 7)+1);
        SetControl32BitValue(getControlRefByID('86in',1,soundWin),(snd86 & 0x10)?1:0);
        SetControl32BitValue(getControlRefByID('86rm',1,soundWin),(snd86 & 0x02)?1:0);
   }
    spb = np2cfg.spbopt;
    {
        BYTE para;
        static short paranum[4] = {0, 3, 1, 2};
        setControlValue('spio', ((spb >> 4) & 1)+1);
        setControlValue('spin', paranum[(spb >> 6) & 3]+1);
        para = spb & 7;
        if (para > 4) {
            para = 4;
        }
        setControlValue('sprm', para+1);
        
        spbvrc = np2cfg.spb_vrc;								// ver0.30
        setControlValue('spvl',(spbvrc & 1)?1:0);
        setControlValue('spvr',(spbvrc & 2)?1:0);
        setControlValue('splv',np2cfg.spb_vrl);
        setControlValue('sprv',np2cfg.spb_x);
    }
    spbvrc = np2cfg.spb_vrc;								// ver0.30

    
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
            case 'vFM ':
            case 'vPSG':
            case 'vADP':
            case 'vPCM':
            case 'vRtm':
            case 'vLft':
            case 'vRit':
            case 'vf2 ':
            case 'vf4 ':
            case 'vf8 ':
            case 'vf16':
                setFieldValue(cmd.commandID);
                break;

            case '26io':
                set26jmp(getControlValue('26io'), 0x10);
                break;

            case kHICommandOK:
                renewal = 0;
                val = getControlValue('vFM ');
                if (val != np2cfg.vol_fm) {
                    np2cfg.vol_fm = val;
                    renewal = 1;
                }
                val = getControlValue('vPSG');
                if (val != np2cfg.vol_ssg) {
                    np2cfg.vol_ssg = val;
                    renewal = 1;
                }
                val = getControlValue('vADP');
                if (val != np2cfg.vol_adpcm) {
                    np2cfg.vol_adpcm = val;
                    renewal = 1;
                }
                val = getControlValue('vPCM');
                if (val != np2cfg.vol_pcm) {
                    np2cfg.vol_pcm = val;
                    renewal = 1;
                }
                val = getControlValue('vRtm');
                if (val != np2cfg.vol_rhythm) {
                    np2cfg.vol_rhythm = val;
                    renewal = 1;
                }
                if (renewal) {
                    sysmng_update(SYS_UPDATECFG);
                }
				opngen_setvol(np2cfg.vol_fm);
				psggen_setvol(np2cfg.vol_ssg);
				rhythm_setvol(np2cfg.vol_rhythm);
				rhythm_update(&rhythm);
				adpcm_setvol(np2cfg.vol_adpcm);
				adpcm_update(&adpcm);
				pcm86gen_setvol(np2cfg.vol_pcm);
				pcm86gen_update();
                
                renewal = 0;
                val = getControlValue('vLft');
                if (val != np2cfg.vol14[0]) {
                    np2cfg.vol14[0] = val;
                    renewal = 1;
                }
                val = getControlValue('vRit');
                if (val != np2cfg.vol14[1]) {
                    np2cfg.vol14[1] = val;
                    renewal = 1;
                }
                val = getControlValue('vf2 ');
                if (val != np2cfg.vol14[2]) {
                    np2cfg.vol14[2] = val;
                    renewal = 1;
                }
                val = getControlValue('vf4 ');
                if (val != np2cfg.vol14[3]) {
                    np2cfg.vol14[3] = val;
                    renewal = 1;
                }
                val = getControlValue('vf8 ');
                if (val != np2cfg.vol14[4]) {
                    np2cfg.vol14[4] = val;
                    renewal = 1;
                }
                val = getControlValue('vf16');
                if (val != np2cfg.vol14[5]) {
                    np2cfg.vol14[5] = val;
                    renewal = 1;
                }
                if (renewal) {
                    sysmng_update(SYS_UPDATECFG);
                    tms3631_setvol(np2cfg.vol14);
                }

				if (np2cfg.snd26opt != snd26) {
					np2cfg.snd26opt = snd26;
					sysmng_update(SYS_UPDATECFG);
				}
				if (np2cfg.snd86opt != snd86) {
					np2cfg.snd86opt = snd86;
					sysmng_update(SYS_UPDATECFG);
				}
                
				update = 0;
				if (np2cfg.spbopt != spb) {
					np2cfg.spbopt = spb;
					update |= SYS_UPDATECFG;
				}
				if (np2cfg.spb_vrc != spbvrc) {
					np2cfg.spb_vrc = spbvrc;
					update |= SYS_UPDATECFG;
				}
				val = getControlValue('spvl');
				if (np2cfg.spb_vrl != val) {
					np2cfg.spb_vrl = val;
					update |= SYS_UPDATECFG;
				}
				opngen_setVR(np2cfg.spb_vrc, np2cfg.spb_vrl);
				val = getControlValue('sprv');
				if (np2cfg.spb_x != val) {
					np2cfg.spb_x = val;
					update |= SYS_UPDATECFG;
				}
				sysmng_update(update);
                
                endLoop(soundWin);
                err=noErr;
                break;
                
            case kHICommandCancel:
                endLoop(soundWin);
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
    
    err = CreateWindowFromNib(nibRef, CFSTR("SoundDialog"), &soundWin);
    if (err == noErr) {
        initSoundWindow();
        SetInitialTabState(soundWin, lastPaneSelected, kMaxNumTabs);
        EventTypeSpec	tabControlEvents[] ={ { kEventClassControl, kEventControlHit }};
        InstallControlEventHandler( getControlRefByID(kTabMasterSig,kTabMasterID,soundWin),  PrefsTabEventHandlerProc , GetEventTypeCount(tabControlEvents), tabControlEvents, soundWin, NULL );
        EventTypeSpec	list[]={ { kEventClassCommand, kEventCommandProcess },};
        EventHandlerRef	ref;
        InstallWindowEventHandler (soundWin, NewEventHandlerUPP(cfWinproc), GetEventTypeCount(list), list, (void *)soundWin, &ref);
        ShowSheetWindow(soundWin, hWndMain);
        
        err=RunAppModalLoopForWindow(soundWin);
    }
    return;
}

void initSoundOpt( void ) {
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


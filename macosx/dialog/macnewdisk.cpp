#include	"compiler.h"

#include	"resource.h"
#include	"np2.h"
#include	"newdisk.h"
#include	"dialog.h"
#include	"dialogutils.h"
#include	"macnewdisk.h"

const int defaultsize[5] = {20, 41, 65, 80, 128};
static	WindowRef	diskWin;
static	SInt32	targetDisk, media, hdsize;
static	char	disklabel[256];
enum {kTabMasterSig = 'ScrT',kTabMasterID = 1000,kTabPaneSig= 'ScTb'};
#define	kMaxNumTabs 2
static UInt16		lastPaneSelected = 1;
#define	getControlValue(a,b)		GetControl32BitValue(getControlRefByID(a,b,diskWin))

static pascal OSStatus cfWinproc(EventHandlerCallRef myHandler, EventRef event, void* userData) {
    OSStatus	err = eventNotHandledErr;
    HICommand	cmd;
	SINT32		data;
    char		outstr[16];

    if (GetEventClass(event)==kEventClassCommand && GetEventKind(event)==kEventCommandProcess ) {
        GetEventParameter(event, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &cmd);
        switch (cmd.commandID)
        {
            case 'hdsz':
                data = getControlValue(cmd.commandID,1)-1;
                sprintf(outstr, "%d", defaultsize[data]);
                SetControlData(getControlRefByID(cmd.commandID,0,diskWin),kControlNoPart,kControlStaticTextTextTag,strlen(outstr),outstr);
                Draw1Control(getControlRefByID(cmd.commandID,0,diskWin));
                break;

            case kHICommandOK:
                targetDisk = getControlValue(kTabMasterSig, kTabMasterID);
                getFieldText(getControlRefByID('fdlb', 0, diskWin), disklabel);
                media = getControlValue('fdty', 0);
                data = getFieldValue(getControlRefByID('hdsz', 0, diskWin));
                if (data < 0) {
                    data = 0;
                }
                else if (data > 512) {
                    data = 512;
                }
                hdsize = data;
                QuitAppModalLoopForWindow(diskWin);
                err=noErr;
                break;
                
            case kHICommandCancel:
                QuitAppModalLoopForWindow(diskWin);
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
    EventHandlerRef	ref;
    
    err = CreateWindowFromNib(nibRef, CFSTR("NewDiskDialog"), &diskWin);
    if (err == noErr) {
    
        SetInitialTabState(diskWin, lastPaneSelected, kMaxNumTabs);
        EventTypeSpec	tabControlEvents[] ={ { kEventClassControl, kEventControlHit }};
        InstallControlEventHandler( getControlRefByID(kTabMasterSig,kTabMasterID,diskWin),  PrefsTabEventHandlerProc , GetEventTypeCount(tabControlEvents), tabControlEvents, diskWin, NULL );
        EventTypeSpec	list[]={ { kEventClassCommand, kEventCommandProcess },};
        InstallWindowEventHandler (diskWin, NewEventHandlerUPP(cfWinproc), GetEventTypeCount(list), list, (void *)diskWin, &ref);
        ShowWindow(diskWin);
        RunAppModalLoopForWindow(diskWin);

    }
    return;
}

static SInt32 initNewDisk( void ) {
    OSStatus	err;
    IBNibRef	nibRef;

    err = CreateNibReference(CFSTR("np2"), &nibRef);
    if (err ==noErr ) {
        makeNibWindow (nibRef);
        DisposeNibReference ( nibRef);
    }
    return(err);
}

// ‚Æ‚è‚ ‚¦‚¸ƒ‚[ƒ_ƒ‹‚Å
void newdisk(void) {

    FSSpec	fss;
    char	fname[MAX_PATH];

    initNewDisk();
    
	if (targetDisk == 1) {
        if (saveFile('.D88', "Newdisk.d88", &fss)) {
            fsspec2path(&fss, fname, sizeof(fname));
            newdisk_fdd(fname, media, disklabel);
        }
	}
    else if (targetDisk == 2) {
        if (saveFile('.THD', "Newdisk.thd", &fss)) {
            fsspec2path(&fss, fname, sizeof(fname));
            newdisk_hdd(fname, hdsize);
        }
    }
    HideWindow(diskWin);
    DisposeWindow(diskWin);
}

static pascal void navEventProc( NavEventCallbackMessage sel,NavCBRecPtr parm,NavCallBackUserData ud )
{
	switch( sel )
	{
		case kNavCBEvent:
		{
			switch( parm->eventData.eventDataParms.event->what )
			{
			}
			break;
		}
	}
}

Boolean saveFile(OSType type, char *title, FSSpec *fsc)
{	
	OSType				sign='SMil';
	NavEventUPP			eventUPP;
	NavReplyRecord		reply;
	DescType			rtype;
	short				ret;
	AEKeyword			key;
	Size				len;
	NavDialogOptions	opt;
		
	InitCursor();
	NavGetDefaultDialogOptions( &opt );
    mkstr255(opt.savedFileName, title);
	opt.dialogOptionFlags+=kNavNoTypePopup;
	
	eventUPP=NewNavEventUPP( navEventProc );
	ret=NavPutFile( NULL,&reply,&opt,eventUPP,type,sign,NULL );
	DisposeNavEventUPP( eventUPP );

	if( reply.validRecord && ret==0 )
	{
		ret=AEGetNthPtr( &(reply.selection),1,typeFSS,&key,&rtype,(Ptr)fsc,(long)sizeof(FSSpec),&len );
		NavDisposeReply( &reply );
	}
        if (ret == noErr) {
            return true;
        }
	return( false );
}

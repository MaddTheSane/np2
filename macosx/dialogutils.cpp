/*
 *  dialogutils.cpp
 *  np2
 *
 *  Created by tk800 on Sat Oct 25 2003.
 *
 */

#include	"compiler.h"
#include	"dialogutils.h"

enum {kTabMasterSig = 'ScrT',kTabMasterID = 1000,kTabPaneSig= 'ScTb'};

void endLoop(WindowRef window) {

    HideSheetWindow(window);
    DisposeWindow(window);
    QuitAppModalLoopForWindow(window);
}

ControlRef getControlRefByID(OSType sign, int id, WindowRef win) {
    ControlRef	conRef;
    ControlID	conID;

    conID.signature=sign;
    conID.id=id;
    GetControlByID(win, &conID, &conRef);
    return conRef;
}    


void SetInitialTabState(WindowRef theWindow, UInt16 pane, int max)
{
    short qq;

    for(qq=0;qq<max+1;qq++)
    SetControlVisibility( getControlRefByID(  kTabPaneSig,  kTabMasterID+qq, theWindow), false, true );  
    
    SetControlValue(getControlRefByID(kTabMasterSig,kTabMasterID,theWindow),pane );
    SetControlVisibility( getControlRefByID(  kTabPaneSig,  kTabMasterID+pane, theWindow), true, true );
}

short changeTab(WindowRef window, UInt16 pane) {
    short controlValue;
    controlValue = GetControlValue( getControlRefByID(kTabMasterSig,kTabMasterID,window) );
    if ( controlValue != pane )
    {
        SetControlVisibility( getControlRefByID(  kTabPaneSig,  kTabMasterID+pane, window), false, true );
        SetControlVisibility( getControlRefByID(  kTabPaneSig,  kTabMasterID+controlValue, window), true, true );    

        Draw1Control( getControlRefByID(kTabMasterSig,kTabMasterID,window) );		
        return(controlValue);    
    }
    
    return(0);
}

pascal OSStatus changeSlider(ControlRef theControl, WindowRef theWindow, short base) {
    ControlRef	conRef;
    ControlID	conID;
    Str255		title;
    SInt32		value;
    
    value = GetControl32BitValue(theControl) - base; 
    NumToString(value, title);
    if (GetControlID(theControl, &conID) == noErr) {
        conRef = getControlRefByID(conID.signature, conID.id+1000, theWindow);
        SetControlData(conRef, kControlNoPart, kControlStaticTextTextTag, *title, title+1);
        Draw1Control(conRef);
    }
    return( eventNotHandledErr );
}


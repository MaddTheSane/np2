/*
 *  macalert.cpp
 *  np2
 *
 *  Created by tk800 on Fri Oct 31 2003.
 *
 */

#include "compiler.h"
#include "resource.h"
#include "np2.h"
#include "macalert.h"

static SInt16 showCautionAlert(CFStringRef title, CFStringRef string, CFStringRef button) {
    DialogRef	ret;
    AlertStdCFStringAlertParamRec	param;
    DialogItemIndex	hit;
    
    GetStandardAlertDefaultParams(&param, kStdCFStringAlertVersionOne);
    param.movable = true;
    param.cancelButton = kAlertStdAlertCancelButton;
    param.cancelText = param.defaultText;
    if (button) param.defaultText = button;
   
    CreateStandardAlert(kAlertCautionAlert, title, string, &param, &ret);
    RunStandardAlert(ret, NULL, &hit);
    return(hit);
}

void ResumeErrorDialogProc(void) {
    DialogRef	ret;
    AlertStdCFStringAlertParamRec	param;
    DialogItemIndex	hit;
    
    GetStandardAlertDefaultParams(&param, kStdCFStringAlertVersionOne);   
    CreateStandardAlert(kAlertStopAlert, CFCopyLocalizedString(CFSTR("Couldn't restart"), "Resume Error Message"), 
                                         CFCopyLocalizedString(CFSTR("A resume error occured when loading the np2.sav file. So Neko Project II couldn't restart."), "Resume Error Description"),
                                         &param, &ret);
    RunStandardAlert(ret, NULL, &hit);
}

int ResumeWarningDialogProc(const char *string) {
    SInt16	ret;
    
    ret = showCautionAlert(	CFCopyLocalizedString(CFSTR("Save Data Conflict"), "bad data"), 
                            CFStringCreateWithCString(NULL, string, CFStringGetSystemEncoding()), 
                            CFCopyLocalizedString(CFSTR("Continue"), "OK"));
    if (ret = kAlertStdAlertOKButton) {
        return(IDOK);
    }
    return(IDCANCEL);
}

bool ResetWarningDialogProc(void) {
    SInt16	ret;
    
    if (np2oscfg.comfirm) {
        ret = showCautionAlert(	CFCopyLocalizedString(CFSTR("Reset"), "Reset title"), 
                                CFCopyLocalizedString(CFSTR("Are you sure you want to reset?"), "Reset causion string"), 
                                NULL);
        if (ret == kAlertStdAlertCancelButton) {
            return(false);
        }
    }
    return(true);
}

bool QuitWarningDialogProc(void) {
    SInt16	ret;
    
    if (np2oscfg.comfirm) {
        ret = showCautionAlert(	CFCopyLocalizedString(CFSTR("Quit"), "Quit title"), 
                                CFCopyLocalizedString(CFSTR("Are you sure you want to quit?"), "Quit causion string"), 
                                NULL);
        if (ret == kAlertStdAlertCancelButton) {
            return(false);
        }
    }
    return(true);
}
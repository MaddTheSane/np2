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

static AlertStdAlertParamRec makeDefaultParam(void) {
    AlertStdAlertParamRec	param;
    
    param.movable = true;
    param.helpButton = false;
    param.filterProc = NULL;
    param.defaultText = (ConstStringPtr)kAlertDefaultOKText;
    param.cancelText = NULL;
    param.otherText = NULL;
    param.defaultButton = kAlertStdAlertOKButton;
    param.cancelButton = NULL;
    param.position = kWindowDefaultPosition;
    
    return(param);
}

static SInt16 showCautionAlert(const Str255 title, Str255 string, const Str255 button) {
    SInt16	ret;
    AlertStdAlertParamRec	param = makeDefaultParam();
    
    param.defaultText = button;
    param.cancelText = (ConstStringPtr)kAlertDefaultCancelText;
    param.cancelButton = kAlertStdAlertCancelButton;
    
    StandardAlert(kAlertCautionAlert, title, string, &param, &ret);
    return(ret);
}

void ResumeErrorDialogProc(void) {
    SInt16	ret;
    AlertStdAlertParamRec	param = makeDefaultParam();
    StandardAlert(kAlertStopAlert, "\pCouldn't restart", NULL, &param, &ret);
}

int ResumeWarningDialogProc(const char *string) {
    SInt16	ret;
    Str255	str;
    
    mkstr255(str, string);
    ret = showCautionAlert("\pConflict", str, "\pContinue");
    if (ret = kAlertStdAlertOKButton) {
        return(IDOK);
    }
    return(IDCANCEL);
}

bool ResetWarningDialogProc(void) {
    SInt16	ret;
    
    if (np2oscfg.comfirm) {
        ret = showCautionAlert("\pReset", NULL, "\pReset");
        if (ret == kAlertStdAlertCancelButton) {
            return(false);
        }
    }
    return(true);
}
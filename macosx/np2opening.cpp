/*
 *  np2opening.h
 *  np2
 *
 *  Created by tk800 on Fri Oct 31 2003.
 *
 */

#include <QuickTime/QuickTime.h>
#include "compiler.h"
#include "np2.h"
#include "np2opening.h"



static PicHandle getbmp(FSSpec fsc, Rect* srt) {
    PicHandle	pict = NULL;
    GraphicsImportComponent	gi;
    
    if (!GetGraphicsImporterForFile(&fsc, &gi)) {
        if (!GraphicsImportGetNaturalBounds(gi, srt)) {
            OffsetRect( srt, -(*srt).left, -(*srt).top);
            GraphicsImportSetBoundsRect(gi, srt);
            GraphicsImportGetAsPicture(gi, &pict);
        }
        CloseComponent(gi);
    }
    return pict;
}

PicHandle getBMPfromPath(char* path, Rect* srt) {
    
    Str255		fname;
    FSSpec		fsc;
    
	mkstr255(fname, path);
	FSMakeFSSpec(0, 0, fname, &fsc);
    return(getbmp(fsc, srt));
}

PicHandle getBMPfromResource(const char* name, Rect* srt, const CFStringRef	type) {
    CFURLRef	url = NULL;
    FSRef		fsr;
    FSSpec		fsc;
    PicHandle	pict = NULL;
    
    url=CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTRj(name), type, NULL);
    if (url) {
        if (CFURLGetFSRef(url, &fsr)) {
            FSGetCatalogInfo(&fsr, kFSCatInfoNone, NULL, NULL, &fsc, NULL);
            pict = getbmp(fsc, srt);
        }
        CFRelease(url);
    }
    return(pict);
}

void openingNP2(void) {
    Rect		srt;
    GrafPtr		port;
    PicHandle	pict = NULL;

    pict = getBMPfromResource("nekop2", &srt, CFSTR("bmp"));
    if (pict) {
        GetPort(&port);
        SetPortWindowPort(hWndMain);
        OffsetRect(&srt, (640-srt.right)/2, (400-srt.bottom)/2);
        DrawPicture(pict,&srt);
        QDFlushPortBuffer(GetWindowPort(hWndMain), NULL);
        SetPort(port);
        KillPicture(pict);
    }
}

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

void openingNP2(void) {
    Rect		srt;
    GrafPtr		port;
    CFURLRef	openingURL;
    char		buffer[1024];
    FSRef		fsr;
    FSSpec		fsc;
    PicHandle	pict;
    GraphicsImportComponent	gi;
    
    GetPort(&port);
    SetPortWindowPort(hWndMain);

    openingURL=CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("nekop2"), CFSTR("bmp"), NULL);
    if (openingURL) {
        if (CFURLGetFSRef(openingURL, &fsr)) {
            FSPathMakeRef((const UInt8*)buffer, &fsr, NULL);
            FSGetCatalogInfo(&fsr, kFSCatInfoNone, NULL, NULL, &fsc, NULL);
            if (!GetGraphicsImporterForFile(&fsc, &gi)) {
                if (!GraphicsImportGetNaturalBounds(gi, &srt)) {
                    OffsetRect( &srt, -srt.left, -srt.top);
                    GraphicsImportSetBoundsRect(gi, &srt);
                    GraphicsImportGetAsPicture(gi, &pict);
                    OffsetRect(&srt, (640-srt.right)/2, (400-srt.bottom)/2);
                    DrawPicture(pict,&srt);
                    QDFlushPortBuffer(GetWindowPort(hWndMain), NULL);
                    KillPicture(pict);
                }
                CloseComponent(gi);
            }
        }
        CFRelease(openingURL);
    }
    SetPort(port);
}

/*
 *  np2opening.h
 *  np2
 *
 *  Created by tk800 on Fri Oct 31 2003.
 *
 */

#define	CFSTRj(name)	CFStringCreateWithCString(NULL,name,CFStringGetSystemEncoding())

#ifdef __cplusplus
extern "C" {
#endif

void openingNP2(void);
PicHandle getBMPfromPath(char* path, Rect* srt);
PicHandle getBMPfromResource(const char* name, Rect* srt);

#ifdef __cplusplus
}
#endif



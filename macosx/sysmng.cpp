#include	"compiler.h"
#include	"np2.h"
#include	"dosio.h"
#include	"i286.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"fddfile.h"
#include	"diskdrv.h"

static bool getLongFileName(char* dst, const char* path);

	UINT	sys_updates;


static	char	strtitle[256];
static	char	strclock[64];

static struct {
	UINT32	tick;
	UINT32	clock;
	UINT32	draws;
	SINT32	fps;
	SINT32	khz;
} workclock;

void sysmng_workclockreset(void) {

	workclock.tick = GETTICK();
	workclock.clock = I286_CLOCK;
	workclock.draws = drawcount;
}

BOOL sysmng_workclockrenewal(void) {

	SINT32	tick;

	tick = GETTICK() - workclock.tick;
	if (tick < 2000) {
		return(FALSE);
	}
	workclock.tick += tick;
	workclock.fps = ((drawcount - workclock.draws) * 10000) / tick;
	workclock.draws = drawcount;
	workclock.khz = (I286_CLOCK - workclock.clock) / tick;
	workclock.clock = I286_CLOCK;
	return(TRUE);
}

void sysmng_updatecaption(BYTE flag) {

    char	name1[255], name2[255];
	char	work[256];
#ifndef NP2GCC
	Str255	str;
#endif

	if (flag & 1) {
		strtitle[0] = '\0';
		if (fdd_diskready(0)) {
			milstr_ncat(strtitle, "  FDD1:", sizeof(strtitle));
            if (getLongFileName(name1, fdd_diskname(0))) {
                milstr_ncat(strtitle, name1, sizeof(strtitle));
            }
            else {
                milstr_ncat(strtitle, file_getname((char *)fdd_diskname(0)),
															sizeof(strtitle));
            }
		}
		if (fdd_diskready(1)) {
			milstr_ncat(strtitle, "  FDD2:", sizeof(strtitle));
            if (getLongFileName(name2, fdd_diskname(1))) {
                milstr_ncat(strtitle, name2, sizeof(strtitle));
            }
            else {
                milstr_ncat(strtitle, file_getname((char *)fdd_diskname(1)),
															sizeof(strtitle));
            }
		}
	}
	if (flag & 2) {
		strclock[0] = '\0';
		if (np2oscfg.DISPCLK & 2) {
			if (workclock.fps) {
				SPRINTF(strclock, " - %u.%1uFPS",
								workclock.fps / 10, workclock.fps % 10);
			}
			else {
				milstr_ncpy(strclock, " - 0FPS", sizeof(strclock));
			}
		}
		if (np2oscfg.DISPCLK & 1) {
			SPRINTF(work, " %2u.%03uMHz",
								workclock.khz / 1000, workclock.khz % 1000);
			if (strclock[0] == '\0') {
				milstr_ncpy(strclock, " -", sizeof(strclock));
			}
			milstr_ncat(strclock, work, sizeof(strclock));
		}
	}
	milstr_ncpy(work, "Neko Project II", sizeof(work));
	milstr_ncat(work, strtitle, sizeof(work));
	milstr_ncat(work, strclock, sizeof(work));

#if defined(NP2GCC)
    SetWindowTitleWithCFString(hWndMain, CFStringCreateWithCString(NULL, work, kCFStringEncodingUTF8));
#else
	mkstr255(str, work);
	SetWTitle(hWndMain, str);
#endif
}

static bool getLongFileName(char* dst, const char* path) {
	FSSpec	fss;
	Str255	fname;
    FSRef	fref;
    char	buffer[1024];
	char 	*ret, *val;

    if (*path == '\0') {
        return(false);
    }
	mkstr255(fname, path);
	if (FSMakeFSSpec(0, 0, fname, &fss) != noErr) {
        return(false);
    }
    if (FSpMakeFSRef(&fss, &fref) != noErr) {
        return(false);
    }
    if (FSRefMakePath(&fref, (UInt8*)buffer, 1024) != noErr) {
        return(false);
    }
	val = buffer;
    ret = val;
	while(*val != '\0') {
 		if (*val++ == '/') {
			ret = val;
		}
	}
    strcpy(dst, ret);
    return(true);
}
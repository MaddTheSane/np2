#include	"compiler.h"
#include	"np2.h"
#include	"dosio.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"fddfile.h"


	UINT	sys_updates;


// ----

static	char	title[512];
static	char	clock[64];

static struct {
	UINT32	tick;
	UINT32	clock;
	UINT32	draws;
	SINT32	fps;
	SINT32	khz;
} workclock;

void sysmng_workclockreset(void) {

	workclock.tick = GETTICK();
	workclock.clock = nevent.clock;
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
	workclock.khz = (nevent.clock - workclock.clock) / tick;
	workclock.clock = nevent.clock;
	return(TRUE);
}

void sysmng_updatecaption(BYTE flag) {

	char	work[512];

	if (flag & 1) {
		title[0] = '\0';
		if (fdd_diskready(0)) {
			milstr_ncat(title, "  FDD1:", sizeof(title));
			milstr_ncat(title, file_getname((char *)fdd_diskname(0)),
															sizeof(title));
		}
		if (fdd_diskready(1)) {
			milstr_ncat(title, "  FDD2:", sizeof(title));
			milstr_ncat(title, file_getname((char *)fdd_diskname(1)),
															sizeof(title));
		}
	}
	if (flag & 2) {
		clock[0] = '\0';
		if (np2oscfg.DISPCLK & 2) {
			if (workclock.fps) {
				SPRINTF(clock, " - %u.%1uFPS",
									workclock.fps / 10, workclock.fps % 10);
			}
			else {
				milstr_ncpy(clock, " - 0FPS", sizeof(clock));
			}
		}
		if (np2oscfg.DISPCLK & 1) {
			SPRINTF(work, " %2u.%03uMHz",
								workclock.khz / 1000, workclock.khz % 1000);
			if (clock[0] == '\0') {
				milstr_ncpy(clock, " -", sizeof(clock));
			}
			milstr_ncat(clock, work, sizeof(clock));
		}
	}
	milstr_ncpy(work, np2oscfg.titles, sizeof(work));
	milstr_ncat(work, title, sizeof(work));
	milstr_ncat(work, clock, sizeof(work));
	SetWindowText(hWndMain, work);
}


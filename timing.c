#include	"compiler.h"
#include	"pccore.h"
#include	"diskdrv.h"
#include	"fdd_mtr.h"
#include	"timing.h"


static	UINT	timercnt = 0;
static	UINT32	tick = 0;
static	UINT32	cnt = 0;


void timing_init(void) {

	cnt = 0;
	timercnt = 0;
	tick = GETTICK();
}

void timing_setcount(UINT value) {

	timercnt = value;
}

UINT timing_getcount(void) {			// ver0.12 60.0Hz Å® 56.4Hz

	UINT32	ticknow;
	UINT32	span;
	UINT	steps;

	ticknow = GETTICK();
	span = ticknow - tick;
	if (span) {
		fddmtr_callback(ticknow);

		tick = ticknow;
		if (span < 1000) {
			cnt += (span * 388);
			steps = cnt / 6875;
			timercnt += steps;
			cnt -= (steps * 6875);
		}
		else {
			cnt = 0;
			timercnt += 56;
		}
	}
	return(timercnt);
}

void timing_term(void) {
}


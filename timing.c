#include	"compiler.h"
#include	"pccore.h"
#include	"diskdrv.h"
#include	"fdd_mtr.h"
#include	"timing.h"


typedef struct {
	UINT32	tick;
	UINT	cnt;
	UINT32	fraction;
} TIMING;

static	TIMING	timing;


void timing_reset(void) {

	timing.tick = GETTICK();
	timing.cnt = 0;
	timing.fraction = 0;
}

void timing_setcount(UINT value) {

	timing.cnt = value;
}

UINT timing_getcount(void) {

	UINT32	ticknow;
	UINT32	span;
	UINT	steps;

	ticknow = GETTICK();
	span = ticknow - timing.tick;
	if (span) {
		fddmtr_callback(ticknow);

		timing.tick = ticknow;
		if (span < 1000) {
			timing.fraction += (span * 388);
			steps = timing.fraction / 6875;
			timing.cnt += steps;
			timing.fraction -= (steps * 6875);
		}
		else {
			timing.fraction = 0;
			timing.cnt += 56;
		}
	}
	return(timing.cnt);
}

void timing_term(void) {
}


#include	"compiler.h"
#include	"sound.h"
#include	"beep.h"


extern	BEEPCFG		beepcfg;

static void oneshot(BEEP bp, SINT32 *pcm, UINT count) {

	SINT32		vol;
const BPEVENT	*bev;
	SINT32		clock;
	int			event;
	SINT32		remain;
	SINT32		samp;

	vol = beepcfg.vol;
	bev = bp->event;
	if (bp->events) {
		bp->events--;
		clock = bev->clock;
		event = bev->enable;
		bev++;
	}
	else {
		clock = 0x40000000;
		event = bp->lastenable;
	}
	do {
		remain = (1 << 16);
		samp = 0;
		while(remain >= clock) {
			remain -= clock;
			if (bp->lastenable) {
				samp += clock;
			}
			bp->lastenable = event;
			if (bp->events) {
				bp->events--;
				clock = bev->clock;
				event = bev->enable;
				bev++;
			}
			else {
				clock = 0x40000000;
			}
		}
		clock -= remain;
		if (bp->lastenable) {
			samp += remain;
		}
		samp *= vol;
		samp >>= (16 - 10);
		pcm[0] += samp;
		pcm[1] += samp;
		pcm += 2;
	} while(--count);
	bp->lastenable = event;
	bp->events = 0;
}

static void rategenerator(BEEP bp, SINT32 *pcm, UINT count) {

	SINT32		vol;
const BPEVENT	*bev;
	SINT32		samp;
	SINT32		remain;
	SINT32		clock;
	int			event;
	UINT		r;

	vol = beepcfg.vol;
	bev = bp->event;
	if (bp->events) {
		bp->events--;
		clock = bev->clock;
		event = bev->enable;
		bev++;
	}
	else {
		clock = 0x40000000;
		event = bp->lastenable;
	}
	do {
		if (clock >= (1 << 16)) {
			r = clock >> 16;
			r = min(r, count);
			clock -= r << 16;
			count -= r;
			if (bp->lastenable) {
				do {
					samp = (bp->cnt & 0x8000)?1:-1;
					bp->cnt += bp->hz;
					samp += (bp->cnt & 0x8000)?1:-1;
					bp->cnt += bp->hz;
					samp += (bp->cnt & 0x8000)?1:-1;
					bp->cnt += bp->hz;
					samp += (bp->cnt & 0x8000)?1:-1;
					bp->cnt += bp->hz;
					samp *= vol;
					samp <<= (10 - 2);
					pcm[0] += samp;
					pcm[1] += samp;
					pcm += 2;
				} while(--r);
			}
			else {
				pcm += 2 * r;
			}
		}
		else {
			remain = (1 << 16);
			samp = 0;
			while(remain >= clock) {
				remain -= clock;
				if (bp->lastenable) {
					samp += clock;
				}
				bp->lastenable = event;
				bp->cnt = 0;
				if (bp->events) {
					bp->events--;
					clock = bev->clock;
					event = bev->enable;
					bev++;
				}
				else {
					clock = 0x40000000;
				}
			}
			clock -= remain;
			if (bp->lastenable) {
				samp += remain;
			}
			samp *= vol;
			samp >>= (16 - 10);
			pcm[0] += samp;
			pcm[1] += samp;
			pcm += 2;
			count--;
		}
	} while(count);
	bp->lastenable = event;
	bp->events = 0;
}

static void rategenerate(BEEP bp, SINT32 *pcm, UINT count) {

	UINT	vol;
	SINT32	samp;
	UINT	r;

	vol = beepcfg.vol << 8;
	do {
		samp = 0;
		r = 4;
		do {
			bp->cnt += bp->hz;
			samp += vol * ((bp->cnt & 0x8000)?1:-1);
		} while(--r);
		pcm[0] += samp;
		pcm[1] += samp;
		pcm += 2;
	} while(--count);
}

void SOUNDCALL beep_getpcm(BEEP bp, SINT32 *pcm, UINT count) {

	if ((count) && (beepcfg.vol)) {
		if (bp->mode == 0) {
			if (bp->events) {
				oneshot(bp, pcm, count);
			}
		}
		else if (bp->mode == 1) {
			rategenerator(bp, pcm, count);
		}
	}
}


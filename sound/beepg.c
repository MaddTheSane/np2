#include	"compiler.h"
#include	"pccore.h"
#include	"sound.h"
#include	"beep.h"


extern	BEEPCFG		beepcfg;


static void oneshot(BEEP bp, SINT32 *pcm, UINT count) {

	SINT32	vol;
	SINT32	samp;
	SINT32	remain;
	BPEVENT	*bev;

	vol = beepcfg.vol << 11;
	bev = bp->event;

	do {
		remain = pc.sampleclock;
		samp = 0;
		while(remain >= bev->clock) {
			if (--bp->events) {
				remain -= bev->clock;
				if (bp->lastenable) {
					samp += bev->clock;
				}
				bp->lastenable = bev->enable;
				bev++;
			}
			else {								// �T���v�����ۂȂ��Ȃ�
				if (bp->lastenable) {
					samp += remain;
					samp *= vol;
					samp /= pc.sampleclock;
					do {
						pcm[0] += samp;
						pcm[1] += samp;
						pcm += 2;
						samp >>= 1;
					} while(--count);
				}
				else {
					samp *= vol;
					samp /= pc.sampleclock;
					pcm[0] += samp;
					pcm[1] += samp;
				}
				return;
			}
		}
		bev->clock -= remain;
		if (bp->lastenable) {
			samp += remain;
		}
		samp *= vol;
		samp /= pc.sampleclock;
		pcm[0] += samp;
		pcm[1] += samp;
		pcm += 2;
	} while(--count);
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
			if ((bp->events) && (pc.sampleclock)) {
				oneshot(bp, pcm, count);
			}
		}
		else if (bp->mode == 1) {
			if (bp->buz) {
				if (bp->hz) {
					rategenerate(bp, pcm, count);
				}
				else {
				}
			}
		}
	}
}


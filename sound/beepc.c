#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"
#include	"beep.h"


	_BEEP		beep;
	BEEPCFG		beepcfg;


void beep_initialize(UINT rate) {

	beepcfg.rate = rate;
	beepcfg.vol = 2;
	beepcfg.puchibase = (rate * 3) / (11025 * 2);
}

void beep_setvol(UINT vol) {

	beepcfg.vol = vol & 3;
}

void beep_changeclock(void) {

	UINT32	hz;
	UINT	rate;

	hz = pccore.realclock / 25;
	rate = beepcfg.rate / 25;
	beepcfg.samplebase = (1 << 16) * rate / hz;
}

void beep_reset(void) {

	beep_changeclock();
	ZeroMemory(&beep, sizeof(beep));
	beep.mode = 1;
}

void beep_hzset(UINT16 cnt) {

	double	hz;

	sound_sync();
	beep.hz = 0;
	if ((cnt & 0xff80) && (beepcfg.rate)) {
		hz = 65536.0 / 4.0 * pccore.baseclock / beepcfg.rate / cnt;
		if (hz < 0x8000) {
			beep.hz = (UINT16)hz;
			return;
		}
	}
	beep.puchi = beepcfg.puchibase;
}

void beep_modeset(void) {

	BYTE	newmode;

	newmode = (pit.mode[1] >> 2) & 3;
	beep.puchi = beepcfg.puchibase;
	if (beep.mode != newmode) {
		sound_sync();
		beep.mode = newmode;
		if (!newmode) {					// mode:#0, #1
			beep_eventinit();
		}
	}
}

static void beep_eventset(void) {

	BPEVENT	*evt;
	int		enable;
	SINT32	clock;

	enable = beep.low & beep.buz;
	if (beep.enable != enable) {
		beep.enable = enable;
		if (beep.events < BEEPEVENT_MAX) {
			clock = CPU_CLOCK + CPU_BASECLOCK - CPU_REMCLOCK;
			evt = beep.event + beep.events;
			beep.events++;
			evt->clock = (clock - beep.clock) * beepcfg.samplebase;
			evt->enable = enable;
			beep.clock = clock;
		}
	}
}

void beep_eventinit(void) {

	beep.low = 0;
	beep.enable = 0;
	beep.lastenable = 0;
	beep.clock = soundcfg.lastclock;
					// nevent.clock + nevent.baseclock - nevent.remainclock;
	beep.events = 0;
}

void beep_eventreset(void) {

	beep.lastenable = beep.enable;
	beep.clock = soundcfg.lastclock;
	beep.events = 0;
}


void beep_lheventset(int low) {

	if (beep.low != low) {
		beep.low = low;
		if (!beep.mode) {
			if (beep.events >= (BEEPEVENT_MAX / 2)) {
				sound_sync();
			}
			beep_eventset();
		}
	}
}

void beep_oneventset(void) {

	int		buz;

	buz = (sysport.c & 8)?0:1;

	if (beep.buz != buz) {
		sound_sync();
		beep.buz = buz;
		if (buz) {
			beep.puchi = beepcfg.puchibase;
		}
		if (!beep.mode) {
			beep_eventset();
		}
		else {
			beep.cnt = 0;
		}
	}
}


#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"


	CS4231CFG	cs4231cfg;


static const UINT16 cs4231samprate[] = {
					 8000,	 5510,	16000,	11025,
					27420,	18900,	32000,	22050,
					54840,	37800,	64000,	44100,
					48000,	33075,	 9600,	 6620};


void cs4231_initialize(UINT rate) {

	cs4231cfg.rate = rate;
}

void cs4231_setvol(UINT vol) {

	(void)vol;
}


void cs4231_dma(NEVENTITEM item) {

	DMACH	dmach;
	REG8	ret;
	SINT32	cnt;

	if (item->flag & NEVENT_SETEVENT) {
		if (cs4231.dmach != 0xff) {
			dmach = dmac.dmach + cs4231.dmach;
			if (dmach->leng.w) {
				sound_sync();
				ret = cs4231.proc(dmach);
				if ((ret) && (cs4231.reg.pinctrl & 2)) {
					dmach->leng.w = 0;
					if (cs4231.dmairq != 0xff) {
						cs4231.intflag = 1;
						pic_setirq(cs4231.dmairq);
					}
				}
			}
		}
		if (cs4231cfg.rate) {
			cnt = pc.realclock * 16 / cs4231cfg.rate;
			nevent_set(NEVENT_CS4231, cnt, cs4231_dma, NEVENT_RELATIVE);
		}
	}
	(void)item;
}

REG8 DMACCALL cs4231dmafunc(REG8 func) {

	SINT32	cnt;

	switch(func) {
		case DMAEXT_START:
			if (cs4231cfg.rate) {
				cnt = pc.realclock * 16 / cs4231cfg.rate;
				nevent_set(NEVENT_CS4231, cnt, cs4231_dma, NEVENT_ABSOLUTE);
			}
			break;

		case DMAEXT_BREAK:
			nevent_reset(NEVENT_CS4231);
			break;
	}
	return(0);
}

void cs4231_reset(void) {

	ZeroMemory(&cs4231, sizeof(cs4231));
	cs4231.proc = cs4231_nodecode;
	cs4231.port = 0xffff;
	cs4231.dmach = 0xff;
	cs4231.dmairq = 0xff;
}

void cs4231_update(void) {
}


void cs4231_control(UINT index, REG8 value) {

	DMACH	dmach;

	*(((BYTE *)(&cs4231.reg)) + index) = value;
	switch(index) {
		case 8:					// playback data format
			cs4231.proc = cs4231dec[value >> 4];
			cs4231.step = cs4231samprate[value & 0x0f];
			break;

		case 9:
			if (cs4231.dmach != 0xff) {
				dmach = dmac.dmach + cs4231.dmach;
				if ((value & 0x5) == 0x5) {
					dmach->ready = 1;
				}
				else {
					dmach->ready = 0;
				}
				dmac_check();
			}
			break;
	}
}


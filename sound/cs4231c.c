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

static const BYTE dmach[] =  {0xff, 0x00, 0x01, 0x03, 0xff, 0xff, 0xff, 0xff};
static const BYTE dmairq[] = {0xff, 0x03, 0x06, 0x0a, 0x0c, 0xff, 0xff, 0xff};


void cs4231_initialize(UINT rate) {

	cs4231cfg.rate = rate;
}

void cs4231_setvol(UINT vol) {

	(void)vol;
}


void cs4231_dma(NEVENTITEM item) {

	BYTE	ret;
	SINT32	cnt;

	if (item->flag & NEVENT_SETEVENT) {
		if (dmac.dmach[0].leng.w) {
			sound_sync();
			ret = cs4231.proc();
			if ((ret) && (cs4231.reg.pinctrl & 2)) {
				dmac.dmach[0].leng.w = 0;
				cs4231.intflag = 1;
				pic_setirq(0x0c);
			}
		}
		if (cs4231cfg.rate) {
			cnt = pc.realclock * 16 / cs4231cfg.rate;
			nevent_set(NEVENT_CS4231, cnt, cs4231_dma, NEVENT_RELATIVE);
		}
	}
	(void)item;
}

BYTE DMACCALL cs4231dmafunc(BYTE func) {

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


void cs4231_control(UINT index, BYTE value) {

	*(((BYTE *)(&cs4231.reg)) + index) = value;
	switch(index) {
		case 8:					// playback data format
			cs4231.proc = cs4231dec[value >> 4];
			cs4231.step = cs4231samprate[value & 0x0f];
			break;

		case 9:
			if ((value & 0x5) == 0x5) {
				dmac.dmach[0].ready = 1;
			}
			else {
				dmac.dmach[0].ready = 0;
			}
			dmac_check();
			break;
	}
}


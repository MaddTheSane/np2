#include	"compiler.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cs4231io.h"
#include	"sound.h"
#include	"fmboard.h"


static const UINT8 cs4231dma[] = {0xff,0x00,0x01,0x03,0xff,0xff,0xff,0xff};
static const UINT8 cs4231irq[] = {0xff,0x03,0x06,0x0a,0x0c,0xff,0xff,0xff};


static void IOOUTCALL csctrl_oc24(UINT port, REG8 dat) {

	cs4231.portctrl = dat;
	(void)port;
}

static void IOOUTCALL csctrl_oc2b(UINT port, REG8 dat) {

	if ((cs4231.portctrl & 0x2f) == 0x20) {
		cs4231.port &= 0xff00;
		cs4231.port |= (dat & 0xf0);
	}
	(void)port;
}

static void IOOUTCALL csctrl_oc2d(UINT port, REG8 dat) {

	if ((cs4231.portctrl & 0x2f) == 0x20) {
		cs4231.port &= 0x00ff;
		cs4231.port |= (dat << 8);
	}
	(void)port;
}

static REG8 IOINPCALL csctrl_ic24(UINT port) {

	(void)port;
	return(0x80 | cs4231.portctrl);
}

static REG8 IOINPCALL csctrl_ic2b(UINT port) {

	switch(cs4231.portctrl & 0x0f) {
		case 0x00:
			return(cs4231.port & 0xff);

		case 0x01:
			return(0x60);

		case 0x04:
			return(0x88);
	}
	(void)port;
	return(0xff);
}

static REG8 IOINPCALL csctrl_ic2d(UINT port) {

	switch(cs4231.portctrl & 0x0f) {
		case 0x00:
			return(cs4231.port >> 8);

		case 0x01:
			return(0xa4);

		case 0x04:
			return(0x01);
	}
	(void)port;
	return(0xff);
}


// ----

void cs4231io_reset(void) {

	cs4231.enable = 1;
	cs4231.port = 0xf40;
	cs4231.adrs = 0x21;
	cs4231.dmairq = cs4231irq[(cs4231.adrs >> 3) & 3];
	cs4231.dmach = cs4231dma[cs4231.adrs & 7];
	cs4231.step = 22050;
	if (cs4231.dmach != 0xff) {
		dmac_attach(DMADEV_CS4231, cs4231.dmach);
	}
}

void cs4231io_bind(void) {

	sound_streamregist(&cs4231, (SOUNDCB)cs4231_getpcm);
	iocore_attachout(0xc24, csctrl_oc24);
	iocore_attachout(0xc2b, csctrl_oc2b);
	iocore_attachout(0xc2d, csctrl_oc2d);
	iocore_attachinp(0xc24, csctrl_ic24);
	iocore_attachinp(0xc2b, csctrl_ic2b);
	iocore_attachinp(0xc2d, csctrl_ic2d);
}

void IOOUTCALL cs4231io_w8(UINT port, REG8 value) {

	switch(port & 0x0f) {
		case 0:
			cs4231.adrs = value;
			cs4231.dmairq = cs4231irq[(value >> 3) & 3];
			cs4231.dmach = cs4231dma[value & 7];
			dmac_detach(DMADEV_CS4231);
			if (cs4231.dmach != 0xff) {
				dmac_attach(DMADEV_CS4231, cs4231.dmach);
			}
			break;

		case 4:
			cs4231.index = value;
			break;

		case 5:
			cs4231_control(cs4231.index & 0x1f, value);
			break;

		case 6:
			cs4231.intflag = 0;
			break;
	}
}

REG8 IOINPCALL cs4231io_r8(UINT port) {

	switch(port & 0x0f) {
		case 0:
			return(cs4231.adrs);

		case 3:
			return(0x04);

		case 4:
			return(cs4231.index & 0x7f);

		case 5:
			return(*(((BYTE *)(&cs4231.reg)) + (cs4231.index & 0x1f)));

		case 6:
			return(cs4231.intflag);

	}
	return(0);
}


#include	"compiler.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"cs4231io.h"
#include	"sound.h"
#include	"fmboard.h"


static void IOOUTCALL csctrl_oc24(UINT port, BYTE dat) {

	cs4231.portctrl = dat;
	(void)port;
}

static void IOOUTCALL csctrl_oc2b(UINT port, BYTE dat) {

	if ((cs4231.portctrl & 0x2f) == 0x20) {
		cs4231.port &= 0xff00;
		cs4231.port |= (dat & 0xf0);
	}
	(void)port;
}

static void IOOUTCALL csctrl_oc2d(UINT port, BYTE dat) {

	if ((cs4231.portctrl & 0x2f) == 0x20) {
		cs4231.port &= 0x00ff;
		cs4231.port |= (dat << 8);
	}
	(void)port;
}

static BYTE IOINPCALL csctrl_ic24(UINT port) {

	(void)port;
	return(0x80 | cs4231.portctrl);
}

static BYTE IOINPCALL csctrl_ic2b(UINT port) {

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

static BYTE IOINPCALL csctrl_ic2d(UINT port) {

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

	cs4231.enable = 0;
	cs4231.port = 0xffff;
}

void cs4231io_bind(void) {

	cs4231.enable = 1;
	cs4231.port = 0xf40;
	cs4231.adrs = 0x21;
	cs4231.dmach = 0;
	cs4231.dmairq = 0x0c;
	cs4231.step = 22050;
	sound_streamregist(&cs4231, (SOUNDCB)cs4231_getpcm);

	iocore_attachout(0xc24, csctrl_oc24);
	iocore_attachout(0xc2b, csctrl_oc2b);
	iocore_attachout(0xc2d, csctrl_oc2d);
	iocore_attachinp(0xc24, csctrl_ic24);
	iocore_attachinp(0xc2b, csctrl_ic2b);
	iocore_attachinp(0xc2d, csctrl_ic2d);
}

void IOOUTCALL cs4231io_w8(UINT port, BYTE value) {

	switch(port & 0x0f) {
		case 0:
#if 0
			cs4231.adrs = value;
			cs4231.dmairq = dmairq[(value >> 3) & 3];
			cs4231.dmach = dmach[value & 7];
#endif
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

BYTE IOINPCALL cs4231io_r8(UINT port) {

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


#include	"compiler.h"
#include	"i286.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"lio.h"


BYTE lio_gcolor1(void) {

	LIOGCOLOR1	dat;
	LIOGCOLOR1	gcolor1;

	i286_memstr_read(I286_DS, I286_BX, &dat, sizeof(dat));
	if (dat.palmode < 2) {
		gcolor1.palmax = 8;
		gcolor1.palmode = dat.palmode;
	}
	else if (dat.palmode == 2) {
		gcolor1.palmax = 16;
		gcolor1.palmode = 2;
	}
	else {
		return(5);
	}

	if (dat.bgcolor == 0xff) {
		gcolor1.bgcolor = lio.gcolor1.bgcolor;
	}
	else if (dat.bgcolor < gcolor1.palmax) {
		gcolor1.bgcolor = dat.bgcolor;
	}
	else {
		return(5);
	}

	if (dat.bdcolor == 0xff) {
		gcolor1.bdcolor = lio.gcolor1.bdcolor;
	}
	else if (dat.bdcolor < gcolor1.palmax) {
		gcolor1.bdcolor = dat.bdcolor;
	}
	else {
		return(5);
	}

	if (dat.fgcolor == 0xff) {
		gcolor1.fgcolor = lio.gcolor1.fgcolor;
	}
	else if (dat.fgcolor < gcolor1.palmax) {
		gcolor1.fgcolor = dat.fgcolor;
	}
	else {
		return(5);
	}

	if (!gcolor1.palmode) {
		iocore_out8(0x006a, 0);
	}
	else {
		iocore_out8(0x006a, 1);
	}
	lio.gcolor1 = gcolor1;
	return(0);
}


#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"biosmem.h"
#include	"lio.h"


typedef struct {
	UINT8	pal;
	UINT8	color1;
	UINT8	color2;
} LIOGCOLOR2;


// ----

REG8 lio_gcolor1(void) {

	LIOGCOLOR1	dat;

	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	if (dat.bgcolor == 0xff) {
		dat.bgcolor = lio.gcolor1.bgcolor;
	}
	if (dat.bdcolor == 0xff) {
		dat.bdcolor = lio.gcolor1.bdcolor;
	}
	if (dat.fgcolor == 0xff) {
		dat.fgcolor = lio.gcolor1.fgcolor;
	}
	if (dat.palmode == 0xff) {
		dat.palmode = lio.gcolor1.palmode;
	}
	else if (!(mem[MEMB_PRXCRT] & 1)) {				// 16color lio
		dat.palmode = 0;
	}
	else {
		if (!(mem[MEMB_PRXCRT] & 4)) {				// have e-plane?
			goto gcolor1_err5;
		}
		dat.palmax = (dat.palmode == 2)?16:8;
		if (!dat.palmode) {
			iocore_out8(0x006a, 0);
		}
		else {
			iocore_out8(0x006a, 1);
		}
	}
	lio.gcolor1 = dat;
	return(LIO_SUCCESS);

gcolor1_err5:
	return(LIO_ILLEGALFUNC);
}


// ----

REG8 lio_gcolor2(void) {

	LIOGCOLOR2	dat;

	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	if (dat.pal >= lio.gcolor1.palmax) {
		goto gcolor2_err5;
	}
	if (!lio.gcolor1.palmode) {
		dat.color1 &= 7;
		lio.degcol[dat.pal] = dat.color1;
		gdc_setdegitalpal(dat.pal, dat.color1);
	}
	else {
		gdc_setanalogpal(dat.pal, offsetof(RGB32, p.b),
												(UINT8)(dat.color1 & 0x0f));
		gdc_setanalogpal(dat.pal, offsetof(RGB32, p.r),
												(UINT8)(dat.color1 >> 4));
		gdc_setanalogpal(dat.pal, offsetof(RGB32, p.g),
												(UINT8)(dat.color2 & 0x0f));
	}
	return(LIO_SUCCESS);

gcolor2_err5:
	return(LIO_ILLEGALFUNC);
}


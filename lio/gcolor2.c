#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"lio.h"


typedef struct {
	BYTE	pal;
	BYTE	color1;
	BYTE	color2;
} MEMGCOLOR2;


BYTE lio_gcolor2(void) {

	MEMGCOLOR2	dat;

	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));

	if (dat.pal >= lio.gcolor1.palmax) {
		return(5);
	}
	if (!lio.gcolor1.palmode) {
		dat.color1 &= 7;
		lio.degcol[dat.pal] = dat.color1;
		gdc_setdegitalpal(dat.pal, dat.color1);
	}
	else {
		gdc_setanalogpal(dat.pal, offsetof(RGB32, p.b),
												(BYTE)(dat.color1 & 0x0f));
		gdc_setanalogpal(dat.pal, offsetof(RGB32, p.r),
												(BYTE)(dat.color1 >> 4));
		gdc_setanalogpal(dat.pal, offsetof(RGB32, p.g),
												(BYTE)(dat.color2 & 0x0f));
	}
	return(0);
}


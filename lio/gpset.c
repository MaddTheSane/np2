#include	"compiler.h"
#include	"cpucore.h"
#include	"lio.h"


typedef struct {
	BYTE	x[2];
	BYTE	y[2];
	BYTE	pal;
} GPSET;


// ---- CLS

REG8 lio_gcls(LIOWORK lio) {

	SINT16	y;

	lio_updaterange(lio);
	for (y=lio->range.y1; y<=lio->range.y2; y++) {
		lio_line(lio, lio->range.x1, lio->range.x2, y, lio->gcolor1.bgcolor);
	}
	return(LIO_SUCCESS);
}


// ---- PSET

REG8 lio_gpset(LIOWORK lio) {

	GPSET	dat;
	SINT16	x;
	SINT16	y;

	lio_updaterange(lio);
	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	if (dat.pal >= lio->gcolor1.palmax) {
		if (CPU_AH == 2) {
			dat.pal = lio->gcolor1.bgcolor;
		}
		else {
			dat.pal = lio->gcolor1.fgcolor;
		}
	}
	x = (SINT16)LOADINTELWORD(dat.x);
	y = (SINT16)LOADINTELWORD(dat.y);
	lio_pset(lio, x, y, dat.pal);
	return(LIO_SUCCESS);
}


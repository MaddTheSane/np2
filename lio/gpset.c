#include	"compiler.h"
#include	"cpucore.h"
#include	"lio.h"


typedef struct {
	BYTE	x[2];
	BYTE	y[2];
	BYTE	pal;
} GPSET;

typedef struct {
	BYTE	x[2];
	BYTE	y[2];
} GPOINT2;


// ---- CLS

REG8 lio_gcls(LIOWORK lio) {

	SINT16	y;

	lio_updatedraw(lio);
	for (y=lio->draw.y1; y<=lio->draw.y2; y++) {
		lio_line(lio, lio->draw.x1, lio->draw.x2, y, lio->gcolor1.bgcolor);
	}
	return(LIO_SUCCESS);
}


// ---- PSET

REG8 lio_gpset(LIOWORK lio) {

	GPSET	dat;
	SINT16	x;
	SINT16	y;

	lio_updatedraw(lio);
	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	x = (SINT16)LOADINTELWORD(dat.x);
	y = (SINT16)LOADINTELWORD(dat.y);
	if (dat.pal == 0xff) {
		if (CPU_AH == 1) {
			dat.pal = lio->gcolor1.fgcolor;
		}
		else {
			dat.pal = lio->gcolor1.bgcolor;
		}
	}
	lio_pset(lio, x, y, dat.pal);
	return(LIO_SUCCESS);
}


// ---- GPOINT2

REG8 lio_gpoint2(LIOWORK lio) {

	GPOINT2	dat;
	SINT16	x;
	SINT16	y;
	REG8	ret;
	UINT32	addr;
	UINT	sft;
	UINT	pl;
const BYTE	*ptr;

	lio_updatedraw(lio);
	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	x = (SINT16)LOADINTELWORD(dat.x);
	y = (SINT16)LOADINTELWORD(dat.y);
	if ((lio->draw.x1 > x) || (lio->draw.x2 < x) ||
		(lio->draw.y1 > y) || (lio->draw.y2 < y)) {
		ret = 0xff;
	}
	else {
		ret = 0;
		addr = (x >> 3) + (y * 80);
		if (lio->scrn.top) {
			addr += 16000;
		}
		addr += lio->draw.base;
		sft = (~x) & 7;
		if (lio->scrn.plane & 0x80) {
			for (pl=0; pl<3; pl++) {
				ptr = mem + addr + lioplaneadrs[pl];
				ret += (((*ptr) >> sft) & 1) << pl;
			}
			if (lio->gcolor1.palmode == 2) {
				ptr = mem + addr + lioplaneadrs[3];
				ret += (((*ptr) >> sft) & 1) << 3;
			}
		}
		else {
			ptr = mem + addr + lioplaneadrs[lio->scrn.plane & 3];
			ret = ((*ptr) >> sft) & 1;
		}
	}
	CPU_AL = ret;
	return(LIO_SUCCESS);
}


#include	"compiler.h"
#include	"i286.h"
#include	"memory.h"
#include	"lio.h"


typedef struct {
	BYTE	x1[2];
	BYTE	y1[2];
	BYTE	x2[2];
	BYTE	y2[2];
	BYTE	pal;
	BYTE	type;
	BYTE	sw;
	BYTE	style_l;
	BYTE	style_h;
	BYTE	patleng;
	BYTE	off[2];
	BYTE	seg[2];
} MEMGLINE;


static void nor_linebox(SINT16 x1, SINT16 y1,
											SINT16 x2, SINT16 y2, BYTE pal) {

	lio_line(x1, x2, y1, pal);
	if (y1 != y2) {
		lio_line(x1, x2, y2, pal);
	}
	for (; y1<y2; y1++) {
		lio_pset(x1, y1, pal);
		lio_pset(x2, y1, pal);
	}
}

static void nor_lineboxfill(SINT16 x1, SINT16 y1,
											SINT16 x2, SINT16 y2, BYTE pal) {

	for (; y1<=y2; y1++) {
		lio_line(x1, x2, y1, pal);
	}
}

BYTE lio_gline(void) {

	MEMGLINE	dat;
	SINT16		x1;
	SINT16		y1;
	SINT16		x2;
	SINT16		y2;

	i286_memstr_read(I286_DS, I286_BX, &dat, sizeof(dat));
	if (dat.pal == 0xff) {
		dat.pal = lio.gcolor1.fgcolor;
	}
	else if (dat.pal >= lio.gcolor1.palmax) {
		return(5);
	}
	x1 = (SINT16)LOADINTELWORD(dat.x1);
	y1 = (SINT16)LOADINTELWORD(dat.y1);
	x2 = (SINT16)LOADINTELWORD(dat.x2);
	y2 = (SINT16)LOADINTELWORD(dat.y2);
	switch(dat.type) {
		case 0:
			break;

		case 1:
			nor_linebox(x1, y1, x2, y2, dat.pal);
			break;

		case 2:
			nor_lineboxfill(x1, y1, x2, y2, dat.pal);
			break;

		default:
			return(5);
	}
	return(0);
}


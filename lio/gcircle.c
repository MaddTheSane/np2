#include	"compiler.h"
#include	"cpucore.h"
#include	"lio.h"

typedef struct {
	BYTE	cx[2];
	BYTE	cy[2];
	BYTE	rx[2];
	BYTE	ry[2];
	BYTE	pal;
	BYTE	flag;
	BYTE	sx[2];
	BYTE	sy[2];
	BYTE	ex[2];
	BYTE	ey[2];
	BYTE	pat;
	BYTE	off[2];
	BYTE	seg[2];
} GCIRCLE;


// ---- GCIRCLE

static void draw4(const _GLIO *lio, SINT16 x, SINT16 y,
											SINT16 d1, SINT16 d2, REG8 pal) {

	SINT16	x1;
	SINT16	x2;
	SINT16	y1;
	SINT16	y2;

	x1 = x - d1;
	x2 = x + d1;
	y1 = y - d2;
	y2 = y + d2;
	lio_pset(lio, x1, y1, pal);
	lio_pset(lio, x1, y2, pal);
	lio_pset(lio, x2, y1, pal);
	lio_pset(lio, x2, y2, pal);
}

REG8 lio_gcircle(GLIO lio) {

	GCIRCLE	dat;
	SINT16	cx;
	SINT16	cy;
	SINT16	rx;
	SINT16	ry;
	REG8	pal;
	SINT16	d1;
	SINT16	d2;
	SINT16	d3;

	lio_updatedraw(lio);
	MEML_READSTR(CPU_DS, CPU_BX, &dat, sizeof(dat));

	// チェック
	if (dat.flag & 0x7f) {
		TRACEOUT(("LIO GCIRCLE not support flags: %.2x", dat.flag));
	}
	cx = (SINT16)LOADINTELWORD(dat.cx);
	cy = (SINT16)LOADINTELWORD(dat.cy);
	rx = (SINT16)LOADINTELWORD(dat.rx);
	ry = (SINT16)LOADINTELWORD(dat.ry);
	pal = dat.pal;
	if (pal == 0xff) {
		pal = lio->work.fgcolor;
	}

	if (rx != ry) {
		TRACEOUT(("LIO GCIRCLE not support ellipse"));
		return(LIO_SUCCESS);
	}

	// 単純な円の描画
	d1 = 0;
	d2 = ry;
	d3 = 0 - ry;
	while(d1 <= d2) {
		draw4(lio, cx, cy, d1, d2, pal);
		draw4(lio, cx, cy, d2, d1, pal);
		lio->wait += 8 * (10 + 10 + 10);
		d1++;
		d3 += (d1 * 2) - 1;
		if (d3 >= 0) {
			d2--;
			d3 -= d2 * 2;
		}
	}
	return(LIO_SUCCESS);
}


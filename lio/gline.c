#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"gdc_sub.h"
#include	"lio.h"


typedef struct {
	BYTE	x1[2];
	BYTE	y1[2];
	BYTE	x2[2];
	BYTE	y2[2];
	BYTE	pal;
	BYTE	type;
	BYTE	sw;
	BYTE	style[2];
	BYTE	patleng;
	BYTE	off[2];
	BYTE	seg[2];
} GLINE;

typedef struct {
	int		x1;
	int		y1;
	int		x2;
	int		y2;
	UINT16	pat;
	UINT8	pal;
} LINEPT;


static void gline(const _LIOWORK *lio, const LINEPT *lp) {

	int		x1;
	int		y1;
	int		x2;
	int		y2;
	int		swap;
	int		tmp;
	int		width;
	int		height;
	int		d1;
	int		d2;
	UINT16	pat;
	UINT32	csrw;
	GDCVECT	vect;

	x1 = lp->x1;
	y1 = lp->y1;
	x2 = lp->x2;
	y2 = lp->y2;

	// ‚Ñ‚ã[‚Û‚¢‚ñ‚Æ
	swap = 0;
	if (x1 > x2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
		tmp = y1;
		y1 = y2;
		y2 = tmp;
		swap = 1;
	}
	if ((x1 > lio->draw.x2) || (x2 < lio->draw.x1)) {
		return;
	}
	width = x2 - x1;
	height = y2 - y1;
	d1 = lio->draw.x1 - x1;
	d2 = x2 - lio->draw.x2;
	if (d1 > 0) {
		x1 = lio->draw.x1;
		y1 += (((height * d1 * 2) / width) + 1) >> 1;
	}
	if (d2 > 0) {
		x2 = lio->draw.x2;
		y2 -= (((height * d2 * 2) / width) + 1) >> 1;
	}
	if (swap) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	swap = 0;
	if (y1 > y2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	if ((y1 > lio->draw.y2) || (y2 < lio->draw.y1)) {
		return;
	}
	width = x2 - x1;
	height = y2 - y1;
	d1 = lio->draw.y1 - y1;
	d2 = y2 - lio->draw.y2;
	if (d1 > 0) {
		y1 = lio->draw.y1;
		x1 += (((width * d1 * 2) / height) + 1) >> 1;
	}
	if (d2 > 0) {
		y2 = lio->draw.y2;
		x2 -= (((width * d2 * 2) / height) + 1) >> 1;
	}
	if (swap) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	// i‚ñ‚¾‹——£ŒvŽZ
	d1 = x1 - lp->x1;
	if (d1 < 0) {
		d1 = 0 - d1;
	}
	d2 = y1 - lp->y1;
	if (d2 < 0) {
		d2 = 0 - d2;
	}
	d1 = max(d1, d2) & 15;
	pat = (UINT16)((lp->pat >> d1) | (lp->pat << (16 - d1)));

	csrw = (y1 * 40) + (x1 >> 4) + ((x1 & 0xf) << 20);
	gdcsub_setvectl(&vect, x1, y1, x2, y2);
	if (lio->scrn.plane & 0x80) {
		gdcsub_vectl(csrw + 0x4000, &vect, pat,
									(lp->pal & 1)?GDCOPE_SET:GDCOPE_CLEAR);
		gdcsub_vectl(csrw + 0x8000, &vect, pat,
									(lp->pal & 2)?GDCOPE_SET:GDCOPE_CLEAR);
		gdcsub_vectl(csrw + 0xc000, &vect, pat,
									(lp->pal & 4)?GDCOPE_SET:GDCOPE_CLEAR);
		if (lio->gcolor1.palmode == 2) {
			gdcsub_vectl(csrw, &vect, pat,
									(lp->pal & 8)?GDCOPE_SET:GDCOPE_CLEAR);
		}
	}
	else {
		csrw += ((lio->scrn.plane + 1) & 3) << 12;
		gdcsub_vectl(csrw, &vect, pat, (lp->pal)?GDCOPE_SET:GDCOPE_CLEAR);
	}
}


// ----

static void nor_linebox(const _LIOWORK *lio, SINT16 x1, SINT16 y1,
											SINT16 x2, SINT16 y2, REG8 pal) {

	lio_line(lio, x1, x2, y1, pal);
	if (y1 != y2) {
		lio_line(lio, x1, x2, y2, pal);
	}
	for (; y1<y2; y1++) {
		lio_pset(lio, x1, y1, pal);
		lio_pset(lio, x2, y1, pal);
	}
}

static void nor_lineboxfill(const _LIOWORK *lio, SINT16 x1, SINT16 y1,
											SINT16 x2, SINT16 y2, REG8 pal) {

	for (; y1<=y2; y1++) {
		lio_line(lio, x1, x2, y1, pal);
	}
}


// ----

REG8 lio_gline(LIOWORK lio) {

	GLINE	dat;
	LINEPT	lp;
	SINT16	x1;
	SINT16	y1;
	SINT16	x2;
	SINT16	y2;

	lio_updatedraw(lio);
	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	if (dat.pal == 0xff) {
		lp.pal = lio->gcolor1.fgcolor;
	}
	else {
		lp.pal = dat.pal;
	}
	if (lp.pal >= lio->gcolor1.palmax) {
		return(LIO_ILLEGALFUNC);
	}
	lp.x1 = (SINT16)LOADINTELWORD(dat.x1);
	lp.y1 = (SINT16)LOADINTELWORD(dat.y1);
	lp.x2 = (SINT16)LOADINTELWORD(dat.x2);
	lp.y2 = (SINT16)LOADINTELWORD(dat.y2);
	if (dat.sw) {
		lp.pat = (GDCPATREVERSE(dat.style[0]) << 8) +
											GDCPATREVERSE(dat.style[1]);
	}
	else {
		lp.pat = 0xffff;
	}


	if (dat.pal == 0xff) {
		dat.pal = lio->gcolor1.fgcolor;
	}
	else if (dat.pal >= lio->gcolor1.palmax) {
		return(5);
	}
	x1 = (SINT16)LOADINTELWORD(dat.x1);
	y1 = (SINT16)LOADINTELWORD(dat.y1);
	x2 = (SINT16)LOADINTELWORD(dat.x2);
	y2 = (SINT16)LOADINTELWORD(dat.y2);
	switch(dat.type) {
		case 0:
//			gline0();
			break;

		case 1:
			nor_linebox(lio, x1, y1, x2, y2, dat.pal);
			break;

		case 2:
			nor_lineboxfill(lio, x1, y1, x2, y2, dat.pal);
			break;

		default:
			return(5);
	}
	return(0);
}


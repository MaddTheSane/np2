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
	UINT8	pal;
} LINEPT;


static void gline(const _GLIO *lio, const LINEPT *lp, UINT16 pat) {

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
	pat = (UINT16)((pat >> d1) | (pat << (16 - d1)));

	csrw = (y1 * 40) + (x1 >> 4) + ((x1 & 0xf) << 20);
	if (lio->draw.flag & LIODRAW_UPPER) {
		csrw += 16000 >> 1;
	}
	gdcsub_setvectl(&vect, x1, y1, x2, y2);
	if (!(lio->draw.flag & LIODRAW_MONO)) {
		gdcsub_vectl(csrw + 0x4000, &vect, pat,
							(REG8)((lp->pal & 1)?GDCOPE_SET:GDCOPE_CLEAR));
		gdcsub_vectl(csrw + 0x8000, &vect, pat,
							(REG8)((lp->pal & 2)?GDCOPE_SET:GDCOPE_CLEAR));
		gdcsub_vectl(csrw + 0xc000, &vect, pat,
							(REG8)((lp->pal & 4)?GDCOPE_SET:GDCOPE_CLEAR));
		if (lio->draw.flag & LIODRAW_4BPP) {
			gdcsub_vectl(csrw, &vect, pat,
							(REG8)((lp->pal & 8)?GDCOPE_SET:GDCOPE_CLEAR));
		}
	}
	else {
		csrw += ((lio->draw.flag + 1) & LIODRAW_PMASK) << 12;
		gdcsub_vectl(csrw, &vect, pat,
								(REG8)((lp->pal)?GDCOPE_SET:GDCOPE_CLEAR));
	}
}

static void glineb(const _GLIO *lio, const LINEPT *lp, UINT16 pat) {

	LINEPT	lpt;

	lpt = *lp;
	lpt.y2 = lp->y1;
	gline(lio, &lpt, pat);
	lpt.y2 = lp->y2;

	lpt.x2 = lp->x1;
	gline(lio, &lpt, pat);
	lpt.x2 = lp->x2;

	lpt.x1 = lp->x2;
	gline(lio, &lpt, pat);
	lpt.x1 = lp->x1;

	lpt.y1 = lp->y2;
	gline(lio, &lpt, pat);
	lpt.y1 = lp->y1;
}


// ----

static void gbox(const _GLIO *lio, const LINEPT *lp, BYTE *tile, UINT leng) {

	int		x1;
	int		y1;
	int		x2;
	int		y2;
	int		tmp;
	UINT32	csrw;
	GDCVECT	vect;
	UINT16	pat[4];

	x1 = lp->x1;
	y1 = lp->y1;
	x2 = lp->x2;
	y2 = lp->y2;

	if (x1 > x2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 > y2) {
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	if ((x1 > lio->draw.x2) || (x2 < lio->draw.x1) ||
		(y1 > lio->draw.y2) || (y2 < lio->draw.y1)) {
		return;
	}
	x1 = max(x1, lio->draw.x1);
	y1 = max(y1, lio->draw.y1);
	x2 = min(x2, lio->draw.x2);
	y2 = min(y2, lio->draw.y2);
	pat[0] = 0xffff;
	pat[1] = 0xffff;
	pat[2] = 0xffff;
	pat[3] = 0xffff;
	if (leng != 0) {
	}
	if (!(lio->draw.flag & LIODRAW_MONO)) {
		while(y1 <= y2) {
			gdcsub_setvectl(&vect, x1, y1, x2, y1);
			csrw = (y1 * 40) + (x1 >> 4) + ((x1 & 0xf) << 20);
			if (lio->draw.flag & LIODRAW_UPPER) {
				csrw += 16000 >> 1;
			}
			gdcsub_vectl(csrw + 0x4000, &vect, pat[0],
							(REG8)((lp->pal & 1)?GDCOPE_SET:GDCOPE_CLEAR));
			gdcsub_vectl(csrw + 0x8000, &vect, pat[1],
							(REG8)((lp->pal & 2)?GDCOPE_SET:GDCOPE_CLEAR));
			gdcsub_vectl(csrw + 0xc000, &vect, pat[2],
							(REG8)((lp->pal & 4)?GDCOPE_SET:GDCOPE_CLEAR));
			if (lio->draw.flag & LIODRAW_4BPP) {
				gdcsub_vectl(csrw, &vect, pat[3],
							(REG8)((lp->pal & 8)?GDCOPE_SET:GDCOPE_CLEAR));
			}
			y1++;
		}
	}
	else {
		while(y1 <= y2) {
			gdcsub_setvectl(&vect, x1, y1, x2, y1);
			csrw = (y1 * 40) + (x1 >> 4) + ((x1 & 0xf) << 20);
			if (lio->draw.flag & LIODRAW_UPPER) {
				csrw += 16000 >> 1;
			}
			csrw += ((lio->draw.flag + 1) & LIODRAW_PMASK) << 12;
			gdcsub_vectl(csrw, &vect, pat[0],
								(REG8)((lp->pal)?GDCOPE_SET:GDCOPE_CLEAR));
			y1++;
		}
	}
}


// ---- CLS

REG8 lio_gcls(GLIO lio) {

	LINEPT	lp;

	lio_updatedraw(lio);
	lp.x1 = lio->draw.x1;
	lp.y1 = lio->draw.y1;
	lp.x2 = lio->draw.x2;
	lp.y2 = lio->draw.y2;
	lp.pal = lio->work.bgcolor;
	gbox(lio, &lp, NULL, 0);
	return(LIO_SUCCESS);
}


// ----

REG8 lio_gline(GLIO lio) {

	GLINE	dat;
	LINEPT	lp;
	UINT16	pat;
	UINT	leng;
	UINT	lengmin;
	BYTE	tile[256];

	lio_updatedraw(lio);
	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	lp.x1 = (SINT16)LOADINTELWORD(dat.x1);
	lp.y1 = (SINT16)LOADINTELWORD(dat.y1);
	lp.x2 = (SINT16)LOADINTELWORD(dat.x2);
	lp.y2 = (SINT16)LOADINTELWORD(dat.y2);
	if (dat.pal == 0xff) {
		lp.pal = lio->work.fgcolor;
	}
	else {
		lp.pal = dat.pal;
	}
	if (lp.pal >= lio->draw.palmax) {
		goto gline_err;
	}
	pat = 0xffff;
	if (dat.type < 2) {
		if (dat.sw) {
			pat = (GDCPATREVERSE(dat.style[0]) << 8) +
											GDCPATREVERSE(dat.style[1]);
		}
		if (dat.type == 0) {
			gline(lio, &lp, pat);
		}
		else {
			glineb(lio, &lp, pat);
		}
	}
	else if (dat.type == 2) {
		leng = 0;
		if (dat.sw != 2) {
			leng = dat.patleng;
			if (leng == 0) {
				goto gline_err;
			}
			i286_memstr_read(LOADINTELWORD(dat.seg), LOADINTELWORD(dat.off),
																tile, leng);
		}
		gbox(lio, &lp, tile, leng);
	}
	else {
		goto gline_err;
	}
	return(LIO_SUCCESS);

gline_err:
	return(LIO_ILLEGALFUNC);
}


#include	"compiler.h"
#include	<math.h>
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"gdc_pset.h"
#include	"vram.h"


enum {
	RT_MULBIT		= 15,
	RT_TABLEBIT		= 12,
	RT_TABLEMAX		= (1 << RT_TABLEBIT)
};

typedef struct {
	SINT16	x;
	SINT16	y;
	SINT16	x2;
	SINT16	y2;
} VECTDIR;

static	UINT16	gdc_rt[RT_TABLEMAX+1];

static const VECTDIR vectdir[16] = {
					{ 0, 1, 1, 0}, { 1, 1, 1,-1},
					{ 1, 0, 0,-1}, { 1,-1,-1,-1},
					{ 0,-1,-1, 0}, {-1,-1,-1, 1},
					{-1, 0, 0, 1}, {-1, 1, 1, 1},

					{ 0, 1, 1, 1}, { 1, 1, 1, 0},		// SL
					{ 1, 0, 1,-1}, { 1,-1, 0,-1},
					{ 0,-1,-1,-1}, {-1,-1,-1, 0},
					{-1, 0,-1, 1}, {-1, 1, 0, 1}};


#if !defined(MEMOPTIMIZE) || (MEMOPTIMIZE < 2)
const UINT8 gdcbitreverse[0x100] = {
				0x00, 0x01, 0x01, 0x03, 0x01, 0x05, 0x03, 0x07,
				0x01, 0x09, 0x05, 0x0d, 0x03, 0x0b, 0x07, 0x0f,
				0x01, 0x11, 0x09, 0x19, 0x05, 0x15, 0x0d, 0x1d,
				0x03, 0x13, 0x0b, 0x1b, 0x07, 0x17, 0x0f, 0x1f,
				0x01, 0x21, 0x11, 0x31, 0x09, 0x29, 0x19, 0x39,
				0x05, 0x25, 0x15, 0x35, 0x0d, 0x2d, 0x1d, 0x3d,
				0x03, 0x23, 0x13, 0x33, 0x0b, 0x2b, 0x1b, 0x3b,
				0x07, 0x27, 0x17, 0x37, 0x0f, 0x2f, 0x1f, 0x3f,
				0x01, 0x41, 0x21, 0x61, 0x11, 0x51, 0x31, 0x71,
				0x09, 0x49, 0x29, 0x69, 0x19, 0x59, 0x39, 0x79,
				0x05, 0x45, 0x25, 0x65, 0x15, 0x55, 0x35, 0x75,
				0x0d, 0x4d, 0x2d, 0x6d, 0x1d, 0x5d, 0x3d, 0x7d,
				0x03, 0x43, 0x23, 0x63, 0x13, 0x53, 0x33, 0x73,
				0x0b, 0x4b, 0x2b, 0x6b, 0x1b, 0x5b, 0x3b, 0x7b,
				0x07, 0x47, 0x27, 0x67, 0x17, 0x57, 0x37, 0x77,
				0x0f, 0x4f, 0x2f, 0x6f, 0x1f, 0x5f, 0x3f, 0x7f,
				0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
				0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
				0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
				0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
				0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
				0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
				0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
				0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
				0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
				0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
				0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
				0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
				0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
				0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
				0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
				0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff};
#else
REG8 gdcbitreverse(REG8 data) {

	REG8	ret;
	UINT	cnt;

	ret = 0;
	cnt = 8;
	do {
		ret = (ret * 2) + (data & 1);
		data >>= 1;
	} while(--cnt);
	return(ret);
}
#endif


void gdcsub_initialize(void) {

	int		i;

	for (i=0; i<=RT_TABLEMAX; i++) {
		gdc_rt[i] = (UINT16)((double)(1 << RT_MULBIT) *
				(1 - sqrt(1 - pow((0.70710678118654 * i) / RT_TABLEMAX, 2))));
	}
}

void gdcslavewait(NEVENTITEM item) {

	if (item->flag & NEVENT_SETEVENT) {
		gdc.s_drawing = 0;							// clear DRAWING
	}
}

static void calc_gdcslavewait(UINT dots) {

	SINT32	clk;

	clk = dots;
	if (pccore.baseclock != PCBASECLOCK20) {
		clk *= 27648;
	}
	else {
		clk *= 22464;
	}
	clk *= pccore.multiple;
	clk /= 15625;
	clk += 30 * pccore.multiple;
	gdc.s_drawing = 8;							// GDC DRAWING!
	nevent_set(NEVENT_GDCSLAVE, clk, gdcslavewait, NEVENT_ABSOLUTE);
}

void gdcsub_vect0(UINT32 csrw, const GDCVECT *vect, REG16 pat, REG8 ope) {

	(void)csrw;
	(void)vect;
	(void)pat;
	(void)ope;
}

void gdcsub_vectl(UINT32 csrw, const GDCVECT *vect, REG16 pat, REG8 ope) {

	_GDCPSET	pset;
	UINT		dc;
	REG16		x;
	REG16		y;
	UINT		i;
	UINT16		step;
	UINT		d1;

	gdcpset_prepare(&pset, csrw, pat, ope);
	dc = (LOADINTELWORD(vect->DC)) & 0x3fff;
	if (dc == 0) {
		gdcpset(&pset, pset.x, pset.y);
	}
	else {
		x = pset.x;
		y = pset.y;
		d1 = LOADINTELWORD(vect->D1);
		switch(vect->ope & 7) {
			case 0:
				for (i=0; i<=dc; i++) {
					step = (UINT16)((((d1 * i) / dc) + 1) >> 1);
					gdcpset(&pset, (REG16)(x + step), y++);
				}
				break;

			case 1:
				for (i=0; i<=dc; i++) {
					step = (UINT16)((((d1 * i) / dc) + 1) >> 1);
					gdcpset(&pset, x++, (REG16)(y + step));
				}
				break;

			case 2:
				for (i=0; i<=dc; i++) {
					step = (UINT16)((((d1 * i) / dc) + 1) >> 1);
					gdcpset(&pset, x++, (REG16)(y - step));
				}
				break;

			case 3:
				for (i=0; i<=dc; i++) {
					step = (UINT16)((((d1 * i) / dc) + 1) >> 1);
					gdcpset(&pset, (REG16)(x + step), y--);
				}
				break;

			case 4:
				for (i=0; i<=dc; i++) {
					step = (UINT16)((((d1 * i) / dc) + 1) >> 1);
					gdcpset(&pset, (REG16)(x - step), y--);
				}
				break;

			case 5:
				for (i=0; i<=dc; i++) {
					step = (UINT16)((((d1 * i) / dc) + 1) >> 1);
					gdcpset(&pset, x--, (REG16)(y - step));
				}
				break;

			case 6:
				for (i=0; i<=dc; i++) {
					step = (UINT16)((((d1 * i) / dc) + 1) >> 1);
					gdcpset(&pset, x--, (REG16)(y + step));
				}
				break;

			case 7:
				for (i=0; i<=dc; i++) {
					step = (UINT16)((((d1 * i) / dc) + 1) >> 1);
					gdcpset(&pset, (REG16)(x - step), y++);
				}
				break;
		}
	}
	calc_gdcslavewait(pset.dots);
}

void gdcsub_vectt(UINT32 csrw, const GDCVECT *vect, REG16 pat, REG8 ope) {

	_GDCPSET	pset;
	BYTE		multiple;
	UINT		sx;
const VECTDIR	*dir;
	BYTE		muly;
	REG16		cx;
	REG16		cy;
	UINT		xrem;
	BYTE		mulx;

	if (vect->ope & 0x80) {		// SL
		pat = (REG16)((GDCPATREVERSE(pat) << 8) + GDCPATREVERSE(pat >> 8));
	}
	gdcpset_prepare(&pset, csrw, 0xffff, ope);
	multiple = (gdc.s.para[GDC_ZOOM] & 15) + 1;

	sx = LOADINTELWORD(vect->D);
	sx = ((sx - 1) & 0x3fff) + 1;
	if (sx >= 768) {
		sx = 768;
	}
	dir = vectdir + ((vect->ope) & 7);

	muly = multiple;
	while(muly--) {
		cx = pset.x;
		cy = pset.y;
		xrem = sx;
		while(xrem--) {
			mulx = multiple;
			if (pat & 1) {
				pat >>= 1;
				pat |= 0x8000;
				while(mulx--) {
					gdcpset(&pset, cx, cy);
					cx += dir->x;
					cy += dir->y;
				}
			}
			else {
				pat >>= 1;
				while(mulx--) {
					cx += dir->x;
					cy += dir->y;
				}
			}
		}
		pset.x += dir->x2;
		pset.y += dir->y2;
	}
	calc_gdcslavewait(pset.dots);
}

void gdcsub_vectc(UINT32 csrw, const GDCVECT *vect, REG16 pat, REG8 ope) {

	_GDCPSET	pset;
	UINT		r;
	UINT		m;
	UINT		i;
	UINT		t;
	UINT		x;
	UINT		y;
	UINT		s;

	gdcpset_prepare(&pset, csrw, pat, ope);
	r = (LOADINTELWORD(vect->D)) & 0x3fff;
	m = (r * 10000 + 14141) / 14142;
	if (!m) {
		gdcpset(&pset, pset.x, pset.y);
	}
	else {
		i = (LOADINTELWORD(vect->DM)) & 0x3fff;
		t = (LOADINTELWORD(vect->DC)) & 0x3fff;
		x = pset.x;
		y = pset.y;
		if (t > m) {
			t = m;
		}
		switch((vect->ope) & 7) {
			case 0:
				for (; i<=t; i++) {
					s = (gdc_rt[(i << RT_TABLEBIT) / m] * r);
					s = (s + (1 << (RT_MULBIT - 1))) >> RT_MULBIT;
					gdcpset(&pset, (REG16)(x + s), (REG16)(y + i));
				}
				break;

			case 1:
				for (; i<=t; i++) {
					s = (gdc_rt[(i << RT_TABLEBIT) / m] * r);
					s = (s + (1 << (RT_MULBIT - 1))) >> RT_MULBIT;
					gdcpset(&pset, (REG16)(x + i), (REG16)(y + s));
				}
				break;

			case 2:
				for (; i<=t; i++) {
					s = (gdc_rt[(i << RT_TABLEBIT) / m] * r);
					s = (s + (1 << (RT_MULBIT - 1))) >> RT_MULBIT;
					gdcpset(&pset, (REG16)(x + i), (REG16)(y - s));
				}
				break;

			case 3:
				for (; i<=t; i++) {
					s = (gdc_rt[(i << RT_TABLEBIT) / m] * r);
					s = (s + (1 << (RT_MULBIT - 1))) >> RT_MULBIT;
					gdcpset(&pset, (REG16)(x + s), (REG16)(y - i));
				}
				break;

			case 4:
				for (; i<=t; i++) {
					s = (gdc_rt[(i << RT_TABLEBIT) / m] * r);
					s = (s + (1 << (RT_MULBIT - 1))) >> RT_MULBIT;
					gdcpset(&pset, (REG16)(x - s), (REG16)(y - i));
				}
				break;

			case 5:
				for (; i<=t; i++) {
					s = (gdc_rt[(i << RT_TABLEBIT) / m] * r);
					s = (s + (1 << (RT_MULBIT - 1))) >> RT_MULBIT;
					gdcpset(&pset, (REG16)(x - i), (REG16)(y - s));
				}
				break;

			case 6:
				for (; i<=t; i++) {
					s = (gdc_rt[(i << RT_TABLEBIT) / m] * r);
					s = (s + (1 << (RT_MULBIT - 1))) >> RT_MULBIT;
					gdcpset(&pset, (REG16)(x - i), (REG16)(y + s));
				}
				break;

			case 7:
				for (; i<=t; i++) {
					s = (gdc_rt[(i << RT_TABLEBIT) / m] * r);
					s = (s + (1 << (RT_MULBIT - 1))) >> RT_MULBIT;
					gdcpset(&pset, (REG16)(x - s), (REG16)(y + i));
				}
				break;
		}
	}
	calc_gdcslavewait(pset.dots);
}

void gdcsub_vectr(UINT32 csrw, const GDCVECT *vect, REG16 pat, REG8 ope) {

	_GDCPSET	pset;
	UINT		d;
	UINT		d2;
	REG16		x;
	REG16		y;
	UINT		i;
const VECTDIR	*dir;

	gdcpset_prepare(&pset, csrw, pat, ope);
	d = (LOADINTELWORD(vect->D)) & 0x3fff;
	d2 = (LOADINTELWORD(vect->D2)) & 0x3fff;
	x = pset.x;
	y = pset.y;
	dir = vectdir + ((vect->ope) & 7);
	for (i=0; i<d; i++) {
		gdcpset(&pset, x, y);
		x += dir->x;
		y += dir->y;
	}
	for (i=0; i<d2; i++) {
		gdcpset(&pset, x, y);
		x += dir->x2;
		y += dir->y2;
	}
	for (i=0; i<d; i++) {
		gdcpset(&pset, x, y);
		x -= dir->x;
		y -= dir->y;
	}
	for (i=0; i<d2; i++) {
		gdcpset(&pset, x, y);
		x -= dir->x2;
		y -= dir->y2;
	}
	calc_gdcslavewait(pset.dots);
}

void gdcsub_text(UINT32 csrw, const GDCVECT *vect, const BYTE *pat,
																REG8 ope) {

	_GDCPSET	pset;
	BYTE		multiple;
	UINT		sx;
	UINT		sy;
const VECTDIR	*dir;
	UINT		patnum;
	BYTE		muly;
	REG16		cx;
	REG16		cy;
	UINT		xrem;
	BYTE		bit;
	BYTE		mulx;

	gdcpset_prepare(&pset, csrw, 0xffff, ope);
	multiple = (gdc.s.para[GDC_ZOOM] & 15) + 1;
#if 1
	sy = LOADINTELWORD(vect->DC);
	sy = (sy & 0x3fff) + 1;
	sx = LOADINTELWORD(vect->D);
	sx = ((sx - 1) & 0x3fff) + 1;

	// てきとーにリミット
	if (sx >= 768) {
		sx = 768;
	}
	if (sy >= 768) {
		sy = 768;
	}
#else
	sx = 8;
	sy = 8;
#endif
	dir = vectdir + (((vect->ope & 0x80) >> 4) + ((vect->ope) & 7));
	patnum = 0;

	while(sy--) {
		muly = multiple;
		patnum--;
		while(muly--) {
			cx = pset.x;
			cy = pset.y;
			bit = pat[patnum & 7];
			xrem = sx;
			while(xrem--) {
				mulx = multiple;
				if (bit & 1) {
					bit >>= 1;
					bit |= 0x80;
					while(mulx--) {
						gdcpset(&pset, cx, cy);
						cx += dir->x;
						cy += dir->y;
					}
				}
				else {
					bit >>= 1;
					while(mulx--) {
						cx += dir->x;
						cy += dir->y;
					}
				}
			}
			pset.x += dir->x2;
			pset.y += dir->y2;
		}
	}
	calc_gdcslavewait(pset.dots);
}


// ----

void gdcsub_write(void) {

	UINT16	mask;
	UINT16	data;
	UINT32	adrs;
	UINT	leng;
	BYTE	*ptr;
	UINT16	updatebit;

#if 0
	TRACEOUT(("gdcsub_write"));
	if (grcg.gdcwithgrcg & 0x8) {
		MessageBox(NULL, "!!! grcg working", "?", MB_OK);
	}
#endif

	mask = LOADINTELWORD(gdc.s.para + GDC_MASK);
#if defined(BYTESEX_LITTLE)
	switch(gdc.s.cmd & 0x18) {
		case 0x00:
			data = LOADINTELWORD(gdc.s.para + GDC_CODE);
			break;

		case 0x10:
			mask &= 0x00ff;
			data = gdc.s.para[GDC_CODE];
			break;

		case 0x18:
			mask &= 0xff00;
			data = gdc.s.para[GDC_CODE] << 8;
			break;

		default:
			return;
	}
#else
	switch(gdc.s.cmd & 0x18) {
		case 0x00:
			data = (gdc.s.para[GDC_CODE] << 8) + gdc.s.para[GDC_CODE + 1];
			break;

		case 0x10:
			mask &= 0xff00;
			data = gdc.s.para[GDC_CODE] << 8;
			break;

		case 0x18:
			mask &= 0x00ff;
			data = gdc.s.para[GDC_CODE];
			break;

		default:
			return;
	}
#endif
#if 0									// これって uPD7220でも有効？
	if (data != 0xffff) {
		data = 0;
	}
#endif

	adrs = LOADINTELDWORD(gdc.s.para + GDC_CSRW);
	leng = LOADINTELWORD(gdc.s.para + GDC_VECTW + 1);
	leng++;
	ptr = mem;
	if (!gdcs.access) {
		updatebit = 0x0101;
	}
	else {
		ptr += VRAM_STEP;
		updatebit = 0x0202;
	}
	gdcs.grphdisp |= (BYTE)updatebit;

	ptr += vramplaneseg[(adrs >> 14) & 3];
	adrs = (adrs & 0x3fff) << 1;
	calc_gdcslavewait(leng);

	switch(gdc.s.cmd & 0x03) {
		case 0x00:					// replace
			data &= mask;
			mask = ~mask;
			do {
				*(UINT16 *)(ptr + adrs) &= mask;
				*(UINT16 *)(ptr + adrs) |= data;
				*(UINT16 *)(vramupdate + adrs) |= updatebit;
				adrs = (adrs + 2) & 0x7ffe;
			} while(--leng);
			break;

		case 0x01:					// complement
			data &= mask;
			do {
				*(UINT16 *)(ptr + adrs) ^= data;
				*(UINT16 *)(vramupdate + adrs) |= updatebit;
				adrs = (adrs + 2) & 0x7ffe;
			} while(--leng);
			break;

		case 0x02:					// clear
			data &= mask;
			do {
				*(UINT16 *)(ptr + adrs) &= data;
				*(UINT16 *)(vramupdate + adrs) |= updatebit;
				adrs = (adrs + 2) & 0x7ffe;
			} while(--leng);
			break;

		case 0x03:					// set
			data &= mask;
			do {
				*(UINT16 *)(ptr + adrs) |= data;
				*(UINT16 *)(vramupdate + adrs) |= updatebit;
				adrs = (adrs + 2) & 0x7ffe;
			} while(--leng);
			break;
	}
}


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
	SINT16	sx;
	SINT16	sy;
} TEXTDIR;

static	UINT16	gdc_rt[RT_TABLEMAX+1];

static const TEXTDIR textdir[8] = {
					{ 0, 1, 1, 0}, { 1, 1, 1,-1},
					{ 1, 0, 0,-1}, { 1,-1,-1,-1},
					{ 0,-1,-1, 0}, {-1,-1,-1, 1},
					{-1, 0, 0, 1}, {-1, 1, 1, 1}};


void gdcsub_init(void) {

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

void gdcsub_line(UINT32 csrw, const GDCVECT *vect, UINT16 pat, BYTE ope) {

	_GDCPSET	pset;
	UINT		dc;
	UINT16		x;
	UINT16		y;
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
					gdcpset(&pset, (UINT16)(x + step), y++);
				}
				break;

			case 1:
				for (i=0; i<=dc; i++) {
					step = (UINT16)((((d1 * i) / dc) + 1) >> 1);
					gdcpset(&pset, x++, (UINT16)(y + step));
				}
				break;

			case 2:
				for (i=0; i<=dc; i++) {
					step = (UINT16)((((d1 * i) / dc) + 1) >> 1);
					gdcpset(&pset, x++, (UINT16)(y - step));
				}
				break;

			case 3:
				for (i=0; i<=dc; i++) {
					step = (UINT16)((((d1 * i) / dc) + 1) >> 1);
					gdcpset(&pset, (UINT16)(x + step), y--);
				}
				break;

			case 4:
				for (i=0; i<=dc; i++) {
					step = (UINT16)((((d1 * i) / dc) + 1) >> 1);
					gdcpset(&pset, (UINT16)(x - step), y--);
				}
				break;

			case 5:
				for (i=0; i<=dc; i++) {
					step = (UINT16)((((d1 * i) / dc) + 1) >> 1);
					gdcpset(&pset, x--, (UINT16)(y - step));
				}
				break;

			case 6:
				for (i=0; i<=dc; i++) {
					step = (UINT16)((((d1 * i) / dc) + 1) >> 1);
					gdcpset(&pset, x--, (UINT16)(y + step));
				}
				break;

			case 7:
				for (i=0; i<=dc; i++) {
					step = (UINT16)((((d1 * i) / dc) + 1) >> 1);
					gdcpset(&pset, (UINT16)(x - step), y++);
				}
				break;
		}
	}
	calc_gdcslavewait(pset.dots);
}

void gdcsub_box(UINT32 csrw, const GDCVECT *vect, UINT16 pat, BYTE ope) {

	_GDCPSET	pset;
	UINT		d;
	UINT		d2;
	UINT16		x;
	UINT16		y;
	UINT		i;

	gdcpset_prepare(&pset, csrw, pat, ope);
	d = (LOADINTELWORD(vect->D)) & 0x3fff;
	d2 = (LOADINTELWORD(vect->D2)) & 0x3fff;
	x = pset.x;
	y = pset.y;
	// ‰ñ“]‚Í‚È‚µ‚æ(Žè”²‚«)
	switch((vect->ope) & 7) {
		case 0:
			for (i=0; i<d; i++) {
				gdcpset(&pset, x, y++);
			}
			for (i=0; i<d2; i++) {
				gdcpset(&pset, x++, y);
			}
			for (i=0; i<d; i++) {
				gdcpset(&pset, x, y--);
			}
			for (i=0; i<d2; i++) {
				gdcpset(&pset, x--, y);
			}
			break;

		case 2:
			for (i=0; i<d; i++) {
				gdcpset(&pset, x++, y);
			}
			for (i=0; i<d2; i++) {
				gdcpset(&pset, x, y--);
			}
			for (i=0; i<d; i++) {
				gdcpset(&pset, x--, y);
			}
			for (i=0; i<d2; i++) {
				gdcpset(&pset, x, y++);
			}
			break;

		case 4:
			for (i=0; i<d; i++) {
				gdcpset(&pset, x, y--);
			}
			for (i=0; i<d2; i++) {
				gdcpset(&pset, x--, y);
			}
			for (i=0; i<d; i++) {
				gdcpset(&pset, x, y++);
			}
			for (i=0; i<d2; i++) {
				gdcpset(&pset, x++, y);
			}
			break;

		case 6:
			for (i=0; i<d; i++) {
				gdcpset(&pset, x--, y);
			}
			for (i=0; i<d2; i++) {
				gdcpset(&pset, x, y++);
			}
			for (i=0; i<d; i++) {
				gdcpset(&pset, x++, y);
			}
			for (i=0; i<d2; i++) {
				gdcpset(&pset, x, y--);
			}
			break;
	}
	calc_gdcslavewait(pset.dots);
}

void gdcsub_circle(UINT32 csrw, const GDCVECT *vect, UINT16 pat, BYTE ope) {

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
					gdcpset(&pset, (UINT16)(x + s), (UINT16)(y + i));
				}
				break;

			case 1:
				for (; i<=t; i++) {
					s = (gdc_rt[(i << RT_TABLEBIT) / m] * r);
					s = (s + (1 << (RT_MULBIT - 1))) >> RT_MULBIT;
					gdcpset(&pset, (UINT16)(x + i), (UINT16)(y + s));
				}
				break;

			case 2:
				for (; i<=t; i++) {
					s = (gdc_rt[(i << RT_TABLEBIT) / m] * r);
					s = (s + (1 << (RT_MULBIT - 1))) >> RT_MULBIT;
					gdcpset(&pset, (UINT16)(x + i), (UINT16)(y - s));
				}
				break;

			case 3:
				for (; i<=t; i++) {
					s = (gdc_rt[(i << RT_TABLEBIT) / m] * r);
					s = (s + (1 << (RT_MULBIT - 1))) >> RT_MULBIT;
					gdcpset(&pset, (UINT16)(x + s), (UINT16)(y - i));
				}
				break;

			case 4:
				for (; i<=t; i++) {
					s = (gdc_rt[(i << RT_TABLEBIT) / m] * r);
					s = (s + (1 << (RT_MULBIT - 1))) >> RT_MULBIT;
					gdcpset(&pset, (UINT16)(x - s), (UINT16)(y - i));
				}
				break;

			case 5:
				for (; i<=t; i++) {
					s = (gdc_rt[(i << RT_TABLEBIT) / m] * r);
					s = (s + (1 << (RT_MULBIT - 1))) >> RT_MULBIT;
					gdcpset(&pset, (UINT16)(x - i), (UINT16)(y - s));
				}
				break;

			case 6:
				for (; i<=t; i++) {
					s = (gdc_rt[(i << RT_TABLEBIT) / m] * r);
					s = (s + (1 << (RT_MULBIT - 1))) >> RT_MULBIT;
					gdcpset(&pset, (UINT16)(x - i), (UINT16)(y + s));
				}
				break;

			case 7:
				for (; i<=t; i++) {
					s = (gdc_rt[(i << RT_TABLEBIT) / m] * r);
					s = (s + (1 << (RT_MULBIT - 1))) >> RT_MULBIT;
					gdcpset(&pset, (UINT16)(x - s), (UINT16)(y + i));
				}
				break;
		}
	}
	calc_gdcslavewait(pset.dots);
}

void gdcsub_text(UINT32 csrw, const GDCVECT *vect, UINT16 pat, BYTE ope) {

	_GDCPSET	pset;
	BYTE		multiple;
	UINT		sx;
	UINT		sy;
const TEXTDIR	*dir;
	UINT		patnum;
	BYTE		muly;
	UINT16		cx;
	UINT16		cy;
	UINT		xrem;
	BYTE		bit;
	BYTE		mulx;

	gdcpset_prepare(&pset, csrw, pat, ope);
	multiple = (gdc.s.para[GDC_ZOOM] & 15) + 1;
#if 0
	sx = (LOADINTELWORD(vect->DC)) & 0x3fff) + 1;
	sy = (LOADINTELWORD(vect->D)) & 0x3fff) + 1;
#else
	sx = 8;
	sy = 8;
#endif
	dir = textdir + ((vect->ope) & 7);
	patnum = 0;

	while(sy--) {
		muly = multiple;
		patnum--;
		while(muly--) {
			cx = pset.x;
			cy = pset.y;
			bit = gdc.s.para[GDC_TEXTW + (patnum & 7)];
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
			pset.x += dir->sx;
			pset.y += dir->sy;
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
#if 0									// ‚±‚ê‚Á‚Ä uPD7220‚Å‚à—LŒøH
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


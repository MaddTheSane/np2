#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"lio.h"
#include	"vram.h"


typedef struct {
	BYTE	x1[2];
	BYTE	y1[2];
	BYTE	x2[2];
	BYTE	y2[2];
	BYTE	off[2];
	BYTE	seg[2];
	BYTE	leng[2];
} GGET;

typedef struct {
	BYTE	x[2];
	BYTE	y[2];
	BYTE	off[2];
	BYTE	seg[2];
	BYTE	leng[2];
	BYTE	mode;
	BYTE	colorsw;
	BYTE	fg;
	BYTE	bg;
} GPUT1;

typedef struct {
	BYTE	x[2];
	BYTE	y[2];
	BYTE	chr[2];
	BYTE	mode;
	BYTE	colorsw;
	BYTE	fg;
	BYTE	bg;
} GPUT2;

typedef struct {
	SINT16	x;
	SINT16	y;
	UINT16	width;
	UINT16	height;
	UINT16	off;
	UINT16	seg;
	UINT8	mode;
	UINT8	sw;
	UINT8	fg;
	UINT8	bg;
} LIOPUT;

typedef struct {
	BYTE	*baseptr;
	UINT	addr;
	UINT	sft;
	UINT	width;
	UINT8	maskl;
	UINT8	maskr;
	UINT8	masklr;
	UINT8	mask;
	BYTE	pat[84];
} PUTCNTX;

#if 0
static void getvram(const PUTCNTX *pt) {

	BYTE	*baseptr;
	UINT	addr;
	BYTE	*dst;
	UINT	width;
	UINT	dat;
	UINT	sft;

	baseptr = pt->baseptr;
	addr = pt->addr;
	src = pt->pat;
	width = pt->width;
	sft = 8 - pt->sft;
	dat = baseptr[LOW15(addr)];
	addr++;
	while(width > 8) {
		width -= 8;
		dat = (dat << 8) + baseptr[LOW15(addr)];
		addr++;
		*dst = (UINT8)(dat >> sft);
		dst++
	}
	dat = (dat << 8) + baseptr[LOW15(addr)];
	*dst = (UINT8)((dat >> sft) & pt->mask);
}
#endif

static void setdirty(UINT addr, UINT width, UINT height, REG8 bit) {

	UINT	r;

	gdcs.grphdisp |= bit;
	width = (width + 7) >> 3;
	while(height--) {
		r = 0;
		while(r < width) {
			vramupdate[LOW15(addr + r)] |= bit;
			r++;
		}
		addr += 80;
	}
}

static void putor(const PUTCNTX *pt) {

	BYTE	*baseptr;
	UINT	addr;
const BYTE	*src;
	UINT	width;
	UINT	dat;

	baseptr = pt->baseptr;
	addr = pt->addr;
	src = pt->pat;
	width = pt->width;
	dat = *src++;
	if ((pt->sft + width) < 8) {
		baseptr[LOW15(addr)] |= (UINT8)((dat >> pt->sft) & pt->masklr);
	}
	else {
		baseptr[LOW15(addr)] |= (UINT8)(dat >> pt->sft) & pt->maskl;
		addr++;
		width -= (8 - pt->sft);
		while(width > 8) {
			width -= 8;
			dat = (dat << 8) + (*src);
			src++;
			baseptr[LOW15(addr)] |= (UINT8)(dat >> pt->sft);
			addr++;
		}
		if (width) {
			dat = (dat << 8) + (*src);
			baseptr[LOW15(addr)] |= (UINT8)(dat >> pt->sft) & pt->maskr;
		}
	}
}

static void putorn(const PUTCNTX *pt) {

	BYTE	*baseptr;
	UINT	addr;
const BYTE	*src;
	UINT	width;
	UINT	dat;

	baseptr = pt->baseptr;
	addr = pt->addr;
	src = pt->pat;
	width = pt->width;
	dat = *src++;
	if ((pt->sft + width) < 8) {
		baseptr[LOW15(addr)] |= (UINT8)(((~dat) >> pt->sft) & pt->masklr);
	}
	else {
		baseptr[LOW15(addr)] |= (UINT8)(((~dat) >> pt->sft) & pt->maskl);
		addr++;
		width -= (8 - pt->sft);
		while(width > 8) {
			width -= 8;
			dat = (dat << 8) + (*src);
			src++;
			baseptr[LOW15(addr)] |= (UINT8)((~dat) >> pt->sft);
			addr++;
		}
		if (width) {
			dat = (dat << 8) + (*src);
			baseptr[LOW15(addr)] |= (UINT8)(((~dat) >> pt->sft) & pt->maskr);
		}
	}
}

static void putand(const PUTCNTX *pt) {

	BYTE	*baseptr;
	UINT	addr;
const BYTE	*src;
	UINT	width;
	UINT	dat;

	baseptr = pt->baseptr;
	addr = pt->addr;
	src = pt->pat;
	width = pt->width;
	dat = *src++;
	if ((pt->sft + width) < 8) {
		baseptr[LOW15(addr)] &= (UINT8)((dat >> pt->sft) | (~pt->masklr));
	}
	else {
		baseptr[LOW15(addr)] &= (UINT8)((dat >> pt->sft) | (~pt->maskl));
		addr++;
		width -= (8 - pt->sft);
		while(width > 8) {
			width -= 8;
			dat = (dat << 8) + (*src);
			src++;
			baseptr[LOW15(addr)] &= (UINT8)(dat >> pt->sft);
			addr++;
		}
		if (width) {
			dat = (dat << 8) + (*src);
			baseptr[LOW15(addr)] &= (UINT8)((dat >> pt->sft) | (~pt->maskr));
		}
	}
}

static void putandn(const PUTCNTX *pt) {

	BYTE	*baseptr;
	UINT	addr;
const BYTE	*src;
	UINT	width;
	UINT	dat;

	baseptr = pt->baseptr;
	addr = pt->addr;
	src = pt->pat;
	width = pt->width;
	dat = *src++;
	if ((pt->sft + width) < 8) {
		baseptr[LOW15(addr)] &= (UINT8)(~((dat >> pt->sft) & pt->masklr));
	}
	else {
		baseptr[LOW15(addr)] &= (UINT8)(~((dat >> pt->sft) & pt->maskl));
		addr++;
		width -= (8 - pt->sft);
		while(width > 8) {
			width -= 8;
			dat = (dat << 8) + (*src);
			src++;
			baseptr[LOW15(addr)] &= (UINT8)((~dat) >> pt->sft);
			addr++;
		}
		if (width) {
			dat = (dat << 8) + (*src);
			baseptr[LOW15(addr)] &= (UINT8)(~((dat >> pt->sft) & pt->maskr));
		}
	}
}

static void putxor(const PUTCNTX *pt) {

	BYTE	*baseptr;
	UINT	addr;
const BYTE	*src;
	UINT	width;
	UINT	dat;

	baseptr = pt->baseptr;
	addr = pt->addr;
	src = pt->pat;
	width = pt->width;
	dat = *src++;
	if ((pt->sft + width) < 8) {
		baseptr[LOW15(addr)] ^= (UINT8)((dat >> pt->sft) & pt->masklr);
	}
	else {
		baseptr[LOW15(addr)] ^= (UINT8)(dat >> pt->sft) & pt->maskl;
		addr++;
		width -= (8 - pt->sft);
		while(width > 8) {
			width -= 8;
			dat = (dat << 8) + (*src);
			src++;
			baseptr[LOW15(addr)] ^= (UINT8)(dat >> pt->sft);
			addr++;
		}
		if (width) {
			dat = (dat << 8) + (*src);
			baseptr[LOW15(addr)] ^= (UINT8)(dat >> pt->sft) & pt->maskr;
		}
	}
}

static void putxorn(const PUTCNTX *pt) {

	BYTE	*baseptr;
	UINT	addr;
const BYTE	*src;
	UINT	width;
	UINT	dat;

	baseptr = pt->baseptr;
	addr = pt->addr;
	src = pt->pat;
	width = pt->width;
	dat = *src++;
	if ((pt->sft + width) < 8) {
		baseptr[LOW15(addr)] ^= (UINT8)(((~dat) >> pt->sft) & pt->masklr);
	}
	else {
		baseptr[LOW15(addr)] ^= (UINT8)(((~dat) >> pt->sft) & pt->maskl);
		addr++;
		width -= (8 - pt->sft);
		while(width > 8) {
			width -= 8;
			dat = (dat << 8) + (*src);
			src++;
			baseptr[LOW15(addr)] ^= (UINT8)((~dat) >> pt->sft);
			addr++;
		}
		if (width) {
			dat = (dat << 8) + (*src);
			baseptr[LOW15(addr)] ^= (UINT8)(((~dat) >> pt->sft) & pt->maskr);
		}
	}
}


// ----

static REG8 putsub(LIOWORK lio, const LIOPUT *lput) {

	UINT	addr;
	UINT8	sbit;
	PUTCNTX	pt;
	UINT	datacnt;
	UINT	off;
	UINT	height;
	UINT	flag;
	UINT	pl;
	UINT	writecnt;

	if ((lput->x < lio->range.x1) ||
		(lput->y < lio->range.y1) ||
		((lput->x + lput->width) > lio->range.x2) ||
		((lput->y + lput->height) > lio->range.y2)) {
		return(LIO_ILLEGALFUNC);
	}
	if ((lput->width <= 0) || (lput->height <= 0)) {
		return(LIO_SUCCESS);
	}

	addr = (lput->x >> 3) + (lput->y * 80);
	if (lio->scrn.top) {
		addr += 16000;
	}
	if (!lio->scrn.bank) {
		sbit = 1;
	}
	else {
		addr += VRAM_STEP;
		sbit = 2;
	}
	setdirty(addr, (lput->x & 7) + lput->width, lput->height, sbit);

	pt.sft = lput->x & 7;
	pt.width = lput->width;
	pt.maskl = (UINT8)(0xff >> pt.sft);
	pt.maskr = (UINT8)((~0x7f) >> ((pt.width + pt.sft - 1) & 7));
	pt.masklr = (UINT8)(pt.maskl >> pt.sft);

	datacnt = (lput->width + 7) >> 3;
	off = lput->off;

	flag = (lio->gcolor1.palmode == 2)?0x0f:0x07;
	flag |= (lput->fg & 15) << 4;
	flag |= (lput->bg & 15) << 8;

	TRACEOUT(("mode = %d [%.x]", lput->mode, flag));

	// ���ĕ\���B
	writecnt = 0;
	for (pl=0; pl<4; pl++) {
		if (flag & 1) {
			pt.baseptr = mem + lioplaneadrs[pl];
			pt.addr = addr;
			height = lput->height;
			do {
				i286_memstr_read(lput->seg, off, pt.pat, datacnt);
				off += datacnt;
				switch(lput->mode) {
					case 0:		// PSET
						if (flag & (1 << 4)) {
							putor(&pt);
						}
						else {
							putandn(&pt);
						}
						if (flag & (1 << 8)) {
							putorn(&pt);
						}
						else {
							putand(&pt);
						}
						writecnt += 2;
						break;

					case 1:		// NOT
						if (!(flag & (1 << 4))) {
							putor(&pt);
						}
						else {
							putandn(&pt);
						}
						if (!(flag & (1 << 8))) {
							putorn(&pt);
						}
						else {
							putand(&pt);
						}
						writecnt += 2;
						break;

					case 2:		// OR
						if (flag & (1 << 4)) {
							putor(&pt);
							writecnt++;
						}
						if (flag & (1 << 8)) {
							putorn(&pt);
							writecnt++;
						}
						break;

					case 3:		// AND
						if (!(flag & (1 << 4))) {
							putandn(&pt);
							writecnt++;
						}
						if (!(flag & (1 << 8))) {
							putand(&pt);
							writecnt++;
						}
						break;

					case 4:		// XOR
						if (flag & (1 << 4)) {
							putxor(&pt);
							writecnt++;
						}
						if (flag & (1 << 8)) {
							putxorn(&pt);
							writecnt++;
						}
						break;
				}
				pt.addr += 80;
			} while(--height);
			if (!lput->sw) {
				off = lput->off;
			}
		}
		flag >>= 1;
	}
	lio->wait += writecnt * datacnt * (10 + 10 + 10);
	return(LIO_SUCCESS);
}


// ---- GGET

REG8 lio_gget(LIOWORK lio) {

	GGET	dat;

	lio_updaterange(lio);
	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));

	return(0);
}


// ---- GPUT1

REG8 lio_gput1(LIOWORK lio) {

	GPUT1	dat;
	LIOPUT	lput;
	UINT	leng;
	UINT	size;

	lio_updaterange(lio);
	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	lput.x = (SINT16)LOADINTELWORD(dat.x);
	lput.y = (SINT16)LOADINTELWORD(dat.y);
	lput.off = (UINT16)(LOADINTELWORD(dat.off) + 4);
	lput.seg = LOADINTELWORD(dat.seg);
	lput.mode = dat.mode;
	leng = LOADINTELWORD(dat.leng);
	lput.width = i286_memword_read(lput.seg, lput.off - 4);
	lput.height = i286_memword_read(lput.seg, lput.off - 2);
	size = ((lput.width + 7) >> 3) * lput.height;
	if (leng < (size + 4)) {
		return(LIO_ILLEGALFUNC);
	}
	if (leng < ((size * 3) + 4)) {
		lput.sw = 0;
		if (dat.colorsw) {
			lput.fg = dat.fg;
			lput.bg = dat.bg;
		}
		else {
			lput.fg = lio->gcolor1.fgcolor;
			lput.bg = lio->gcolor1.bgcolor;
		}
	}
	else {
		if (dat.colorsw) {
			lput.sw = 0;
			lput.fg = dat.fg;
			lput.bg = dat.bg;
		}
		else {
			lput.sw = 1;
			lput.fg = 0x0f;
			lput.bg = 0x0f;
		}
	}
	return(putsub(lio, &lput));
}


// ---- GPUT2

REG8 lio_gput2(LIOWORK lio) {

	GPUT2	dat;
	LIOPUT	lput;
	UINT16	jis;
	REG16	size;

	lio_updaterange(lio);
	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	lput.x = (SINT16)LOADINTELWORD(dat.x);
	lput.y = (SINT16)LOADINTELWORD(dat.y);
	jis = LOADINTELWORD(dat.chr);
	if (jis & 0xff00) {
		lput.off = 0x104e;
		lput.seg = CPU_DS;
		if (jis < 0x200) {			// 1/4ANK
			jis &= 0xff;
		}
		size = bios0x18_14(lput.seg, 0x104c, jis);
		lput.width = (size & 0xff00) >> (8 - 3);
		lput.height = (size & 0xff) << 3;
	}
	else {
		return(0);
	}
	lput.mode = dat.mode;
	lput.sw = 0;
	if (dat.colorsw) {
		lput.fg = dat.fg;
		lput.bg = dat.bg;
	}
	else {
		lput.fg = lio->gcolor1.fgcolor;
		lput.bg = lio->gcolor1.bgcolor;
	}
	return(putsub(lio, &lput));
}


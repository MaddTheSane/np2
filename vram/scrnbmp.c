#include	"compiler.h"
#include	"bmpdata.h"
#include	"scrnmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"scrndraw.h"
#include	"palettes.h"
#include	"dispsync.h"
#include	"scrnbmp.h"


static void screenmix(BYTE *dest, const BYTE *src1, const BYTE *src2) {

	int		i;

	for (i=0; i<(SURFACE_WIDTH * SURFACE_HEIGHT); i++) {
		*dest++ = (*src1++) + (*src2++) + NP2PAL_GRPH;
	}
}

static void screenmix2(BYTE *dest, const BYTE *src1, const BYTE *src2) {

	int		x, y;

	for (y=0; y<(SURFACE_HEIGHT/2); y++) {
		for (x=0; x<SURFACE_WIDTH; x++) {
			*dest++ = (*src1++) + (*src2++) + NP2PAL_GRPH;
		}
		for (x=0; x<SURFACE_WIDTH; x++) {
			*dest++ = ((*src1++) >> 4) + NP2PAL_TEXT;
		}
		src2 += SURFACE_WIDTH;
	}
}

static void screenmix3(BYTE *dest, const BYTE *src1, const BYTE *src2) {

	BYTE	c;
	int		x, y;

	for (y=0; y<(SURFACE_HEIGHT/2); y++) {
		// dest == src1, dest == src2 の時があるので…
		for (x=0; x<SURFACE_WIDTH; x++) {
			c = (*(src1 + SURFACE_WIDTH)) >> 4;
			if (!c) {
				c = (*src2) + NP2PAL_SKIP;
			}
			*(dest + SURFACE_WIDTH) = c;
			*dest++ = (*src1++) + (*src2++) + NP2PAL_GRPH;
		}
		dest += SURFACE_WIDTH;
		src1 += SURFACE_WIDTH;
		src2 += SURFACE_WIDTH;
	}
}

SCRNBMP scrnbmp(void) {

	BMPDATA	bd;
	UINT	allocsize;
	BYTE	*scrn;
	BYTE	*p;
	BYTE	*q;
	UINT	pals;
	UINT32	pal[256];
	BYTE	remap[256];
	BYTE	remapflg[256];
	int		x;
	int		y;
	UINT	col;
	UINT32	curpal;
	UINT	pos;
	BMPINFO	bi;
	UINT	type;
	UINT	palsize;
	UINT	align;
	SCRNBMP	ret;
	BMPFILE	*bf;
	int		r;

	bd.width = dsync.scrnxmax;
	bd.height = dsync.scrnymax;
	if ((bd.width <= 0) || (bd.height <= 0)) {
		goto sb_err1;
	}
	allocsize = SURFACE_WIDTH * SURFACE_HEIGHT;
	scrn = (BYTE *)_MALLOC(allocsize, "screen data");
	if (scrn == NULL) {
		goto sb_err1;
	}
	ZeroMemory(scrn, allocsize);

	p = scrn;
	q = scrn;
	if (gdcs.textdisp & 0x80) {
		p = np2_tram;
	}
	if (gdcs.grphdisp & 0x80) {
		q = np2_vram[gdcs.disp];
	}
	if (!(gdc.mode1 & 0x10)) {
		screenmix(scrn, p, q);
	}
	else if (!np2cfg.skipline) {
		screenmix2(scrn, p, q);
	}
	else {
		screenmix3(scrn, p, q);
	}

	// パレット最適化
	p = scrn;
	pals = 0;
	ZeroMemory(pal, sizeof(pal));
	ZeroMemory(remap, sizeof(remap));
	ZeroMemory(remapflg, sizeof(remapflg));
	for (y=0; y<bd.height; y++) {
		for (x=0; x<bd.width; x++) {
			col = *p++;
			if (!remapflg[col]) {
				remapflg[col] = 1;
				curpal = np2_pal32[col].d & 0x00ffffff;		// !!!!
				for (pos=0; pos<pals; pos++) {
					if (pal[pos] == curpal) {
						break;
					}
				}
				if (pos >= pals) {
					pal[pos] = curpal;
					pals++;
				}
				remap[col] = (BYTE)pos;
			}
		}
		p += SURFACE_WIDTH - bd.width;
	}

	allocsize = sizeof(BMPFILE) + sizeof(BMPINFO);
	if (pals < 2) {
		type = SCRNBMP_1BIT;
		bd.bpp = 1;
	}
	else if (pals < 16) {
		type = SCRNBMP_4BIT;
		bd.bpp = 4;
	}
	else {
		type = SCRNBMP_8BIT;
		bd.bpp = 8;
	}
	palsize = 4 << bd.bpp;
	allocsize += palsize;
	bmpdata_setinfo(&bi, &bd);
	allocsize += bmpdata_getdatasize(&bi);
	align = bmpdata_getalign(&bi);

	ret = (SCRNBMP)_MALLOC(sizeof(_SCRNBMP) + allocsize, "scrnbmp");
	if (ret == NULL) {
		goto sb_err2;
	}

	bf = (BMPFILE *)(ret + 1);
	ZeroMemory(bf, allocsize);
	bf->bfType[0] = 'B';
	bf->bfType[1] = 'M';
	pos = sizeof(BMPFILE) + sizeof(BMPINFO) + palsize;
	STOREINTELDWORD(bf->bfOffBits, pos);
	q = (BYTE *)(bf + 1);
	STOREINTELDWORD(bi.biClrImportant, pals);
	CopyMemory(q, &bi, sizeof(bi));
	q += sizeof(bi);
	CopyMemory(q, pal, palsize);
	q += palsize;

	p = scrn + (SURFACE_WIDTH * bd.height);
	do {
		p -= SURFACE_WIDTH;
		switch(type) {
			case SCRNBMP_1BIT:
				for (x=0; x<bd.width; x++) {
					if (remap[p[x*2+0]]) {
						q[x >> 3] |= 0x80 >> (x & 7);
					}
				}
				break;

			case SCRNBMP_4BIT:
				r = bd.width / 2;
				for (x=0; x<r; x++) {
					q[x] = (remap[p[x*2+0]] << 4) + remap[p[x*2+1]];
				}
				if (bd.width & 1) {
					q[x] = remap[p[x*2+0]] << 4;
				}
				break;

			case SCRNBMP_8BIT:
				for (x=0; x<bd.width; x++) {
					q[x] = remap[p[x]];
				}
				break;
		}
		q += align;
	} while(scrn < p);
	_MFREE(scrn);
	ret->type = type;
	ret->ptr = (BYTE *)(ret + 1);
	ret->size = allocsize;
	return(ret);

sb_err2:
	_MFREE(scrn);

sb_err1:
	return(NULL);
}


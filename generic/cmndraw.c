#include	"compiler.h"
#include	"bmpdata.h"
#include	"scrnmng.h"
#include	"cpucore.h"
#include	"font.h"
#include	"cmndraw.h"
#include	"minifont.res"


void cmndraw_getpals(CMNPALFN *fn, CMNPALS *pal, UINT pals) {

	UINT	i;

	if (fn == NULL) {
		return;
	}
	if (fn->get8) {
		for (i=0; i<pals; i++) {
			pal[i].pal8 = (*fn->get8)(fn, i);
		}
	}
	if (fn->get32) {
		for (i=0; i<pals; i++) {
			pal[i].pal32.d = (*fn->get32)(fn, i);
		}
		if (fn->cnv16) {
			for (i=0; i<pals; i++) {
				pal[i].pal16 = (*fn->cnv16)(fn, pal[i].pal32);
			}
		}
	}
}

void cmndraw_makegrad(RGB32 *pal, int pals, RGB32 bg, RGB32 fg) {

	int		i;

	if (pals <= 0) {
		return;
	}
	pals--;
	for (i=0; i<pals; i++) {
		pal[i].p.b = bg.p.b + ((int)(fg.p.b - bg.p.b) * i) / pals;
		pal[i].p.r = bg.p.r + ((int)(fg.p.r - bg.p.r) * i) / pals;
		pal[i].p.g = bg.p.g + ((int)(fg.p.g - bg.p.g) * i) / pals;
		pal[i].p.e = bg.p.e + ((int)(fg.p.e - bg.p.e) * i) / pals;
	}
	pal[i].d = fg.d;
}

#if defined(SUPPORT_8BPP)
void cmndraw8_fill(const CMNVRAM *vram, int x, int y,
										int cx, int cy, BYTE fg) {

	BYTE	*p;
	int		dalign;
	int		r;

	p = vram->ptr + (x * vram->xalign) + (y * vram->yalign);
	dalign = vram->yalign - (vram->xalign * cx);
	do {
		r = cx;
		do {
			*p = fg;
			p += vram->xalign;
		} while(--r);
		p += dalign;
	} while(--cy);
}

void cmndraw8_setfg(const CMNVRAM *vram, const BYTE *src,
										int x, int y, BYTE fg) {

const BYTE	*p;
	BYTE	*q;
	BYTE	cy;
	int		dalign;
	BYTE	c;
	BYTE	bit;
	BYTE	cx;

	p = src + 2;
	q = vram->ptr + (x * vram->xalign) + (y * vram->yalign);
	dalign = vram->yalign - (vram->xalign * src[0]);
	cy = src[1];
	do {
		cx = src[0];
		bit = 0;
		c = 0;
		do {
			if (!bit) {
				bit = 0x80;
				c = *p++;
			}
			if (c & bit) {
				*q = fg;
			}
			bit >>= 1;
			q += vram->xalign;
		} while(--cx);
		q += dalign;
	} while(--cy);
}

void cmndraw8_setpat(const CMNVRAM *vram, const BYTE *src,
										int x, int y, BYTE bg, BYTE fg) {

const BYTE	*p;
	BYTE	*q;
	BYTE	cy;
	int		dalign;
	BYTE	c;
	BYTE	bit;
	BYTE	cx;

	p = src + 2;
	q = vram->ptr + (x * vram->xalign) + (y * vram->yalign);
	dalign = vram->yalign - (vram->xalign * src[0]);
	cy = src[1];
	do {
		cx = src[0];
		bit = 0;
		c = 0;
		do {
			if (!bit) {
				bit = 0x80;
				c = *p++;
			}
			if (c & bit) {
				*q = fg;
			}
			else {
				*q = bg;
			}
			bit >>= 1;
			q += vram->xalign;
		} while(--cx);
		q += dalign;
	} while(--cy);
}
#endif

#if defined(SUPPORT_16BPP)
void cmndraw16_fill(const CMNVRAM *vram, int x, int y,
										int cx, int cy, UINT16 fg) {

	BYTE	*p;
	int		dalign;
	int		r;

	p = vram->ptr + (x * vram->xalign) + (y * vram->yalign);
	dalign = vram->yalign - (vram->xalign * cx);
	do {
		r = cx;
		do {
			*(UINT16 *)p = fg;
			p += vram->xalign;
		} while(--r);
		p += dalign;
	} while(--cy);
}

void cmndraw16_setfg(const CMNVRAM *vram, const BYTE *src,
										int x, int y, UINT16 fg) {

const BYTE	*p;
	BYTE	*q;
	BYTE	cy;
	int		dalign;
	BYTE	c;
	BYTE	bit;
	BYTE	cx;

	p = src + 2;
	q = vram->ptr + (x * vram->xalign) + (y * vram->yalign);
	dalign = vram->yalign - (vram->xalign * src[0]);
	cy = src[1];
	do {
		cx = src[0];
		bit = 0;
		c = 0;
		do {
			if (!bit) {
				bit = 0x80;
				c = *p++;
			}
			if (c & bit) {
				*(UINT16 *)q = fg;
			}
			bit >>= 1;
			q += vram->xalign;
		} while(--cx);
		q += dalign;
	} while(--cy);
}

void cmndraw16_setpat(const CMNVRAM *vram, const BYTE *src,
										int x, int y, UINT16 bg, UINT16 fg) {

const BYTE	*p;
	BYTE	*q;
	BYTE	cy;
	int		dalign;
	BYTE	c;
	BYTE	bit;
	BYTE	cx;

	p = src + 2;
	q = vram->ptr + (x * vram->xalign) + (y * vram->yalign);
	dalign = vram->yalign - (vram->xalign * src[0]);
	cy = src[1];
	do {
		cx = src[0];
		bit = 0;
		c = 0;
		do {
			if (!bit) {
				bit = 0x80;
				c = *p++;
			}
			if (c & bit) {
				*(UINT16 *)q = fg;
			}
			else {
				*(UINT16 *)q = bg;
			}
			bit >>= 1;
			q += vram->xalign;
		} while(--cx);
		q += dalign;
	} while(--cy);
}
#endif

#if defined(SUPPORT_24BPP)
void cmndraw24_fill(const CMNVRAM *vram, int x, int y,
										int cx, int cy, RGB32 fg) {

	BYTE	*p;
	int		dalign;
	int		r;

	p = vram->ptr + (x * vram->xalign) + (y * vram->yalign);
	dalign = vram->yalign - (vram->xalign * cx);
	do {
		r = cx;
		do {
			p[RGB24_R] = fg.p.r;
			p[RGB24_G] = fg.p.g;
			p[RGB24_B] = fg.p.b;
			p += vram->xalign;
		} while(--r);
		p += dalign;
	} while(--cy);
}

void cmndraw24_setfg(const CMNVRAM *vram, const BYTE *src,
										int x, int y, RGB32 fg) {

const BYTE	*p;
	BYTE	*q;
	BYTE	cy;
	int		dalign;
	BYTE	c;
	BYTE	bit;
	BYTE	cx;

	p = src + 2;
	q = vram->ptr + (x * vram->xalign) + (y * vram->yalign);
	dalign = vram->yalign - (vram->xalign * src[0]);
	cy = src[1];
	do {
		cx = src[0];
		bit = 0;
		c = 0;
		do {
			if (!bit) {
				bit = 0x80;
				c = *p++;
			}
			if (c & bit) {
				q[RGB24_R] = fg.p.r;
				q[RGB24_G] = fg.p.g;
				q[RGB24_B] = fg.p.b;
			}
			bit >>= 1;
			q += vram->xalign;
		} while(--cx);
		q += dalign;
	} while(--cy);
}

void cmndraw24_setpat(const CMNVRAM *vram, const BYTE *src,
										int x, int y, RGB32 bg, RGB32 fg) {

const BYTE	*p;
	BYTE	*q;
	BYTE	cy;
	int		dalign;
	BYTE	c;
	BYTE	bit;
	BYTE	cx;

	p = src + 2;
	q = vram->ptr + (x * vram->xalign) + (y * vram->yalign);
	dalign = vram->yalign - (vram->xalign * src[0]);
	cy = src[1];
	do {
		cx = src[0];
		bit = 0;
		c = 0;
		do {
			if (!bit) {
				bit = 0x80;
				c = *p++;
			}
			if (c & bit) {
				q[RGB24_R] = fg.p.r;
				q[RGB24_G] = fg.p.g;
				q[RGB24_B] = fg.p.b;
			}
			else {
				q[RGB24_R] = bg.p.r;
				q[RGB24_G] = bg.p.g;
				q[RGB24_B] = bg.p.b;
			}
			bit >>= 1;
			q += vram->xalign;
		} while(--cx);
		q += dalign;
	} while(--cy);
}
#endif

#if defined(SUPPORT_32BPP)
void cmndraw32_fill(const CMNVRAM *vram, int x, int y,
										int cx, int cy, RGB32 fg) {

	BYTE	*p;
	int		dalign;
	int		r;

	p = vram->ptr + (x * vram->xalign) + (y * vram->yalign);
	dalign = vram->yalign - (vram->xalign * cx);
	do {
		r = cx;
		do {
			*(UINT32 *)p = fg.d;
			p += vram->xalign;
		} while(--r);
		p += dalign;
	} while(--cy);
}

void cmndraw32_setfg(const CMNVRAM *vram, const BYTE *src,
										int x, int y, RGB32 fg) {

const BYTE	*p;
	BYTE	*q;
	BYTE	cy;
	int		dalign;
	BYTE	c;
	BYTE	bit;
	BYTE	cx;

	p = src + 2;
	q = vram->ptr + (x * vram->xalign) + (y * vram->yalign);
	dalign = vram->yalign - (vram->xalign * src[0]);
	cy = src[1];
	do {
		cx = src[0];
		bit = 0;
		c = 0;
		do {
			if (!bit) {
				bit = 0x80;
				c = *p++;
			}
			if (c & bit) {
				*(UINT32 *)q = fg.d;
			}
			bit >>= 1;
			q += vram->xalign;
		} while(--cx);
		q += dalign;
	} while(--cy);
}

void cmndraw32_setpat(const CMNVRAM *vram, const BYTE *src,
										int x, int y, RGB32 bg, RGB32 fg) {

const BYTE	*p;
	BYTE	*q;
	BYTE	cy;
	int		dalign;
	BYTE	c;
	BYTE	bit;
	BYTE	cx;

	p = src + 2;
	q = vram->ptr + (x * vram->xalign) + (y * vram->yalign);
	dalign = vram->yalign - (vram->xalign * src[0]);
	cy = src[1];
	do {
		cx = src[0];
		bit = 0;
		c = 0;
		do {
			if (!bit) {
				bit = 0x80;
				c = *p++;
			}
			if (c & bit) {
				*(UINT32 *)q = fg.d;
			}
			else {
				*(UINT32 *)q = bg.d;
			}
			bit >>= 1;
			q += vram->xalign;
		} while(--cx);
		q += dalign;
	} while(--cy);
}
#endif



// ----

void cmddraw_fill(CMNVRAM *vram, int x, int y, int cx, int cy, CMNPALS *pal) {

	if ((vram == NULL) || (pal == NULL)) {
		return;
	}
	switch(vram->bpp) {
#if defined(SUPPORT_8BPP)
		case 8:
			cmndraw8_fill(vram, x, y, cx, cy, pal->pal8);
			break;
#endif
#if defined(SUPPORT_16BPP)
		case 16:
			cmndraw16_fill(vram, x, y, cx, cy, pal->pal16);
			break;
#endif
#if defined(SUPPORT_24BPP)
		case 24:
			cmndraw24_fill(vram, x, y, cx, cy, pal->pal32);
			break;
#endif
#if defined(SUPPORT_32BPP)
		case 32:
			cmndraw32_fill(vram, x, y, cx, cy, pal->pal32);
			break;
#endif
	}
}

void cmddraw_text8(CMNVRAM *vram, int x, int y, const char *str, CMNPALS *pal) {

	UINT	s;
const BYTE	*ptr;
	BYTE	src[10];

	if ((vram == NULL) || (str == NULL) || (pal == NULL)) {
		return;
	}
	src[0] = 0;
	src[1] = 7;
	while(*str) {
		s = (UINT)(*str++);
		ptr = NULL;
		if ((s >= 0x20) && (s < 0x80)) {
			ptr = minifont + (s - 0x20) * 8;
		}
		else if ((s >= 0xa0) && (s < 0xe0)) {
			ptr = minifont + (s - 0xa0 + 0x60) * 8;
		}
		if (ptr == NULL) {
			continue;
		}
		src[0] = ptr[0];
		CopyMemory(src + 2, ptr + 1, 7);
		switch(vram->bpp) {
#if defined(SUPPORT_8BPP)
			case 8:
				cmndraw8_setfg(vram, src, x, y, pal->pal8);
				break;
#endif
#if defined(SUPPORT_16BPP)
			case 16:
				cmndraw16_setfg(vram, src, x, y, pal->pal16);
				break;
#endif
#if defined(SUPPORT_24BPP)
			case 24:
				cmndraw24_setfg(vram, src, x, y, pal->pal32);
				break;
#endif
#if defined(SUPPORT_32BPP)
			case 32:
				cmndraw32_setfg(vram, src, x, y, pal->pal32);
				break;
#endif
		}
		x += ptr[0] + 1;
	}
}


// ---- bmp

void cmddraw_bmp16(CMNVRAM *vram, const void *bmp, CMNPALCNV cnv, UINT flag) {

const BMPFILE	*bf;
const BMPINFO	*bi;
const BYTE		*palptr;
	BMPDATA		inf;
	BYTE		*src;
	int			bmpalign;
	UINT		pals;
	UINT		c;
	RGB32		paltbl[16];
	CMNPAL		pal[16];
	BYTE		*dst;
	int			yalign;
	int			x;
	int			y;

	if ((vram == NULL) || (bmp == NULL) || (cnv == NULL)) {
		return;
	}
	bf = (BMPFILE *)bmp;
	bi = (BMPINFO *)(bf + 1);
	palptr = (BYTE *)(bi + 1);
	if (((bf->bfType[0] != 'B') && (bf->bfType[1] != 'M')) ||
		(bmpdata_getinfo(bi, &inf) != SUCCESS) || (inf.bpp != 4)) {
		return;
	}
	src = (BYTE *)bf + (LOADINTELDWORD(bf->bfOffBits));
	bmpalign = bmpdata_getalign(bi);
	if (inf.height > 0) {
		src += (inf.height - 1) * bmpalign;
		bmpalign *= -1;
	}
	else {
		inf.height *= -1;
	}
	if ((vram->width < inf.width) || (vram->height < inf.height)) {
		return;
	}
	pals = LOADINTELDWORD(bi->biClrUsed);
	pals = min(pals, 16);
	ZeroMemory(paltbl, sizeof(paltbl));
	for (c=0; c<pals; c++) {
		paltbl[c].p.b = palptr[c*4+0];
		paltbl[c].p.g = palptr[c*4+1];
		paltbl[c].p.r = palptr[c*4+2];
	}
	(*cnv)(pal, paltbl, pals, vram->bpp);
	dst = vram->ptr;
	switch(flag & 0x03) {
		case CMNBMP_CENTER:
			dst += ((vram->width - inf.width) / 2) * vram->xalign;
			break;

		case CMNBMP_RIGHT:
			dst += (vram->width - inf.width) * vram->xalign;
			break;
	}
	switch(flag & 0x0c) {
		case CMNBMP_MIDDLE:
			dst += ((vram->height - inf.height) / 2) * vram->yalign;
			break;

		case CMNBMP_BOTTOM:
			dst += (vram->height - inf.height) * vram->yalign;
			break;
	}
	yalign = vram->yalign - (inf.width * vram->xalign);
	switch(vram->bpp) {
#if defined(SUPPORT_8BPP)
		case 8:
			for (y=0; y<inf.height; y++) {
				for (x=0; x<inf.width; x++) {
					if (!(x & 1)) {
						c = src[x >> 1] >> 4;
					}
					else {
						c = src[x >> 1] & 15;
					}
					*dst = pal[c].pal8;
					dst += vram->xalign;
				}
				src += bmpalign;
				dst += yalign;
			}
			break;
#endif
#if defined(SUPPORT_16BPP)
		case 16:
			for (y=0; y<inf.height; y++) {
				for (x=0; x<inf.width; x++) {
					if (!(x & 1)) {
						c = src[x >> 1] >> 4;
					}
					else {
						c = src[x >> 1] & 15;
					}
					*(UINT16 *)dst = pal[c].pal16;
					dst += vram->xalign;
				}
				src += bmpalign;
				dst += yalign;
			}
			break;
#endif
#if defined(SUPPORT_24BPP)
		case 24:
			for (y=0; y<inf.height; y++) {
				for (x=0; x<inf.width; x++) {
					if (!(x & 1)) {
						c = src[x >> 1] >> 4;
					}
					else {
						c = src[x >> 1] & 15;
					}
					dst[RGB24_R] = pal[c].pal32.p.r;
					dst[RGB24_G] = pal[c].pal32.p.g;
					dst[RGB24_B] = pal[c].pal32.p.b;
					dst += vram->xalign;
				}
				src += bmpalign;
				dst += yalign;
			}
			break;
#endif
#if defined(SUPPORT_32BPP)
		case 32:
			for (y=0; y<inf.height; y++) {
				for (x=0; x<inf.width; x++) {
					if (!(x & 1)) {
						c = src[x >> 1] >> 4;
					}
					else {
						c = src[x >> 1] & 15;
					}
					*(UINT32 *)dst = pal[c].pal32.d;
					dst += vram->xalign;
				}
				src += bmpalign;
				dst += yalign;
			}
			break;
#endif
	}
}


#include	"compiler.h"
#include	"scrnmng.h"
#include	"cmndraw.h"


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


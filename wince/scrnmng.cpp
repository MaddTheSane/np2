#include	"compiler.h"
#include	<gx.h>
#include	"bmpdata.h"
#include	"scrnmng.h"
#include	"scrndraw.h"
#include	"vramhdl.h"
#include	"menubase.h"
#include	"nekop2.res"


typedef struct {
	BOOL	rotate;
	VRAMHDL	vram;
	UINT	posx;
	UINT	posy;
	int		width;
	int		height;
	long	start;
	long	xalign;
	long	yalign;
} SCRNMNG;

typedef struct {
	int		width;
	int		height;
} SCRNSTAT;

	GXDisplayProperties		gx_dp;

static	SCRNMNG		scrnmng;
static	SCRNSTAT	scrnstat;
static	SCRNSURF	scrnsurf;

static	BYTE		gx_disable = 1;

static const TCHAR	errmsg[] = STRLITERAL("Error");


typedef struct {
	int		xalign;
	int		yalign;
	int		width;
	int		height;
	int		srcpos;
	int		dstpos;
} DRAWRECT;

static BOOL calcdrawrect(DRAWRECT *dr, VRAMHDL s, const RECT_T *rt) {

	int		pos;

	dr->xalign = scrnmng.xalign;
	dr->yalign = scrnmng.yalign;
	dr->srcpos = 0;
	dr->dstpos = scrnmng.start;
	dr->width = min(scrnmng.width, s->width);
	dr->height = min(scrnmng.height, s->height);
	if (rt) {
		pos = max(rt->left, 0);
		dr->srcpos += pos;
		dr->dstpos += pos * dr->xalign;
		dr->width = min(rt->right, dr->width) - pos;

		pos = max(rt->top, 0);
		dr->srcpos += pos * s->width;
		dr->dstpos += pos * dr->yalign;
		dr->height = min(rt->bottom, dr->height) - pos;
	}
	if ((dr->width <= 0) || (dr->height <= 0)) {
		return(FAILURE);
	}
	return(SUCCESS);
}

void scrnmng_keybinds(struct GXKeyList *gxkey) {

	short	tmp;

	if (scrnmng.rotate) {
		tmp = gxkey->vkLeft;
		gxkey->vkLeft = gxkey->vkDown;
		gxkey->vkDown = gxkey->vkRight;
		gxkey->vkRight = gxkey->vkUp;
		gxkey->vkUp = tmp;
	}
}


// ----

void scrnmng_initialize(void) {

	scrnstat.width = 640;
	scrnstat.height = 400;
}

BOOL scrnmng_create(HWND hWnd, LONG width, LONG height) {

	TCHAR	msg[32];

	if ((width <= 0) || (height <= 0)) {
		return(FAILURE);
	}
	if (GXOpenDisplay(hWnd, GX_FULLSCREEN) == 0) {
		MessageBox(hWnd, STRLITERAL("Couldn't GameX Object"),
											errmsg, MB_OK | MB_ICONSTOP);
		return(FAILURE);
	}
	gx_dp = GXGetDisplayProperties();
	if (gx_dp.cBPP != 16) {
		MessageBox(hWnd, STRLITERAL("Only 16bit color support..."),
											errmsg, MB_OK | MB_ICONSTOP);
		return(FAILURE);
	}
	if (!(gx_dp.ffFormat & kfDirect565)) {
		MessageBox(hWnd, STRLITERAL("Only 16bit(565) support..."),
											errmsg, MB_OK | MB_ICONSTOP);
		return(FAILURE);
	}

	if ((gx_dp.cxWidth >= (DWORD)width) &&
		(gx_dp.cyHeight >= (DWORD)height)) {
		scrnmng.rotate = FALSE;
	}
	else if ((gx_dp.cxWidth >= (DWORD)height) &&
			(gx_dp.cyHeight >= (DWORD)width)) {
		scrnmng.rotate = TRUE;
	}
	else {
		wsprintf(msg, STRLITERAL("Required %dx%d..."), width, height);
		MessageBox(hWnd, msg, errmsg, MB_OK | MB_ICONSTOP);
		return(FAILURE);
	}

	if (!scrnmng.rotate) {
		scrnmng.posx = ((gx_dp.cxWidth - width) / 2);
		scrnmng.posy = ((gx_dp.cyHeight - height) / 2);
		scrnmng.start = scrnmng.posx * gx_dp.cbxPitch +
						scrnmng.posy * gx_dp.cbyPitch;
		scrnmng.width = width;
		scrnmng.height = height;
		scrnmng.xalign = gx_dp.cbxPitch;
		scrnmng.yalign = gx_dp.cbyPitch;
	}
	else {
		scrnmng.posx = ((gx_dp.cxWidth - height) / 2);
		scrnmng.posy = ((gx_dp.cyHeight - width) / 2);
		scrnmng.start = ((scrnmng.posy + width - 1) * gx_dp.cbyPitch) +
						(scrnmng.posx * gx_dp.cbxPitch);
		scrnmng.width = width;
		scrnmng.height = height;
		scrnmng.xalign = -gx_dp.cbyPitch;
		scrnmng.yalign = gx_dp.cbxPitch;
	}
	gx_disable = 0;
	scrnmng_clear(TRUE);
	return(SUCCESS);
}

void scrnmng_destroy(void) {

	gx_disable = 1;
	GXCloseDisplay();
}

RGB16 scrnmng_makepal16(RGB32 pal32) {

	RGB16	ret;

	ret = (pal32.p.r & 0xf8) << 8;
#if defined(SIZE_QVGA)
	ret += (pal32.p.g & 0xfc) << (3 + 16);
#else
	ret += (pal32.p.g & 0xfc) << 3;
#endif
	ret += pal32.p.b >> 3;
	return(ret);
}

void scrnmng_setwidth(int posx, int width) {

	scrnstat.width = width;
}

void scrnmng_setheight(int posy, int height) {

	scrnstat.height = height;
}

const SCRNSURF *scrnmng_surflock(void) {

	if (gx_disable) {
		return(NULL);
	}
	if (scrnmng.vram == NULL) {
		scrnsurf.ptr = (BYTE *)GXBeginDraw() + scrnmng.start;
		scrnsurf.xalign = scrnmng.xalign;
		scrnsurf.yalign = scrnmng.yalign;
	}
	else {
		scrnsurf.ptr = scrnmng.vram->ptr;
		scrnsurf.xalign = 2;
		scrnsurf.yalign = scrnmng.vram->yalign;
	}
	scrnsurf.width = min(scrnstat.width, 640);
	scrnsurf.height = min(scrnstat.height, 400);
	scrnsurf.bpp = 16;
	scrnsurf.extend = 0;
	return(&scrnsurf);
}

static void draw_onmenu(void) {

	RECT_T		rt;
	DRAWRECT	dr;
const BYTE		*p;
	BYTE		*q;
const BYTE		*a;
	int			salign;
	int			dalign;
	int			r;

	rt.left = 0;
	rt.top = 0;
	rt.right = min(scrnstat.width, 640);
	rt.bottom = min(scrnstat.height, 400);
#if defined(SIZE_QVGA)
	rt.right >>= 1;
	rt.bottom >>= 1;
#endif
	if (calcdrawrect(&dr, menuvram, &rt) != SUCCESS) {
		return;
	}
	p = scrnmng.vram->ptr + (dr.srcpos * 2);
	q = (BYTE *)GXBeginDraw() + dr.dstpos;
	a = menuvram->alpha + dr.srcpos;
	salign = menuvram->width - dr.width;
	dalign = dr.yalign - (dr.width * dr.xalign);
	do {
		r = dr.width;
		do {
			if (a[0] == 0) {
				*(UINT16 *)q = *(UINT16 *)p;
			}
			p += 2;
			q += dr.xalign;
			a += 1;
		} while(--r);
		p += salign * 2;
		q += dalign;
		a += salign;
	} while(--dr.height);
	GXEndDraw();
}

void scrnmng_surfunlock(const SCRNSURF *surf) {

	if (surf) {
		if (scrnmng.vram == NULL) {
			GXEndDraw();
		}
		else {
			if (menuvram) {
				draw_onmenu();
			}
		}
	}
}

void scrnmng_update(void) {
}


// ---- for wince

void scrnmng_enable(BOOL enable) {

	if (enable) {
		gx_disable &= ~2;
	}
	else {
		gx_disable |= 2;
	}
}

BOOL scrnmng_mousepos(LPARAM *lp) {

	UINT	x;
	UINT	y;

	if (!scrnmng.rotate) {
		x = LOWORD(*lp) - scrnmng.posx;
		y = HIWORD(*lp) - scrnmng.posy;
	}
	else {
		x = scrnmng.width - HIWORD(*lp) - 1 - scrnmng.posy;
		y = LOWORD(*lp) - scrnmng.posx;
	}
	if ((x >= (UINT)scrnmng.width) || (y >= (UINT)scrnmng.height)) {
		return(FAILURE);
	}
	*lp = (x & 0xffff) | ((y << 16) & 0xffff0000);
	return(SUCCESS);
}


// ---- for menubase

BOOL scrnmng_entermenu(SCRNMENU *smenu) {

	if (smenu == NULL) {
		goto smem_err;
	}
	vram_destroy(scrnmng.vram);
	scrnmng.vram = vram_create(scrnmng.width, scrnmng.height, FALSE, 16);
	if (scrnmng.vram == NULL) {
		goto smem_err;
	}
	scrndraw_redraw();
	smenu->width = scrnmng.width;
	smenu->height = scrnmng.height;
	smenu->bpp = 16;
	return(SUCCESS);

smem_err:
	return(FAILURE);
}

void scrnmng_leavemenu(void) {

	VRAM_RELEASE(scrnmng.vram);
}

void scrnmng_menudraw(const RECT_T *rct) {

	DRAWRECT	dr;
const BYTE		*p;
const BYTE		*q;
	BYTE		*r;
	BYTE		*a;
	int			salign;
	int			dalign;
	int			x;

	if ((gx_disable) || (menuvram == NULL)) {
		return;
	}
	if (calcdrawrect(&dr, menuvram, rct) != SUCCESS) {
		return;
	}
	p = scrnmng.vram->ptr + (dr.srcpos * 2);
	q = menuvram->ptr + (dr.srcpos * 2);
	r = (BYTE *)GXBeginDraw() + dr.dstpos;
	a = menuvram->alpha + dr.srcpos;
	salign = menuvram->width;
	dalign = dr.yalign - (dr.width * dr.xalign);
	do {
		x = 0;
		do {
			if (a[x]) {
				if (a[x] & 2) {
					*(UINT16 *)r = *(UINT16 *)(q + x * 2);
				}
				else {
					a[x] = 0;
					*(UINT16 *)r = *(UINT16 *)(p + x * 2);
				}
			}
			r += dr.xalign;
		} while(++x < dr.width);
		p += salign * 2;
		q += salign * 2;
		r += dalign;
		a += salign;
	} while(--dr.height);
	GXEndDraw();
}


// ---- clear

static void np2open_draw(BYTE *dst, BMPFILE *bf) {

const BMPINFO	*bi;
const BYTE		*pal;
	BMPDATA		inf;
	BYTE		*src;
	int			bmpalign;
	long		yalign;
	UINT		pals;
	UINT16		paltbl[16];
	UINT		c;
	int			x;
	int			y;

	if (bf == NULL) {
		goto nod_err;
	}
	bi = (BMPINFO *)(bf + 1);
	pal = (BYTE *)(bi + 1);
	if (((bf->bfType[0] != 'B') && (bf->bfType[1] != 'M')) ||
		(bmpdata_getinfo(bi, &inf) != SUCCESS) || (inf.bpp != 4)) {
		goto nod_err;
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
	if ((scrnmng.width < inf.width) || (scrnmng.height < inf.height)) {
		goto nod_err;
	}
	dst += scrnmng.start;
	dst += ((scrnmng.width - inf.width) / 2) * scrnmng.xalign;
	dst += ((scrnmng.height - inf.height) / 2) * scrnmng.yalign;
	yalign = scrnmng.yalign - (inf.width * scrnmng.xalign);

	pals = LOADINTELDWORD(bi->biClrUsed);
	pals = min(pals, 16);
	ZeroMemory(paltbl, sizeof(paltbl));
	for (c=0; c<pals; c++) {
		paltbl[c] = ((pal[c*4+2] & 0xf8) << 8) +
					((pal[c*4+1] & 0xfc) << 3) + (pal[c*4+0] >> 3);
	}

	for (y=0; y<inf.height; y++) {
		for (x=0; x<inf.width; x++) {
			if (!(x & 1)) {
				c = src[x >> 1] >> 4;
			}
			else {
				c = src[x >> 1] & 15;
			}
			*(UINT16 *)dst = paltbl[c];
			dst += scrnmng.xalign;
		}
		src += bmpalign;
		dst += yalign;
	}

nod_err:
	return;
}

void scrnmng_clear(BOOL logo) {

	BMPFILE *bf;
	BYTE	*p;
	BYTE	*q;
	int		y;
	int		x;
	long	yalign;

	if (gx_disable) {
		return;
	}
	bf = NULL;
	if (logo) {
		bf = (BMPFILE *)bmpdata_solvedata(nekop2_bmp);
	}
	p = (BYTE *)GXBeginDraw();
	q = p;
	y = gx_dp.cyHeight;
	yalign = gx_dp.cbyPitch - (gx_dp.cbxPitch * gx_dp.cxWidth);
	do {
		x = gx_dp.cxWidth;
		do {
			*(UINT16 *)q = 0;
			q += gx_dp.cbxPitch;
		} while(--x);
		q += yalign;
	} while(--y);
	if (bf) {
		np2open_draw(p, bf);
	}
	GXEndDraw();
	if (bf) {
		_MFREE(bf);
	}
}


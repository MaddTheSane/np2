#include	"compiler.h"
#include	"fontmng.h"


#define	FONTMNG_CACHE		64

#if defined(FONTMNG_CACHE)
typedef struct {
	WORD		str;
	WORD		next;
} FNTCTBL;
#endif

typedef struct {
	int			fontsize;
	UINT		fonttype;
	int			fontwidth;
	int			fontheight;
	int			fontalign;

// Ç†Ç∆ÇÕägí£Å`
	HDC			hdcimage;
	HBITMAP		hBitmap;
	BYTE		*image;
	HFONT		hfont;
	RECT		rect;
	int			bmpwidth;
	int			bmpheight;
	int			bmpalign;

#if defined(FONTMNG_CACHE)
	DWORD		caches;
	DWORD		cachehead;
	FNTCTBL		cache[FONTMNG_CACHE];
#endif
} _FNTMNG, *FNTMNG;


static const TCHAR deffontface[] = STRLITERAL("ÇlÇr ÉSÉVÉbÉN");
static const TCHAR deffontface2[] = STRLITERAL("ÇlÇr ÇoÉSÉVÉbÉN");

#ifndef _WIN32_WCE
#define	TEXTALPHABASE	(FDAT_DEPTH * 0x60 / 256)
#else
#define	TEXTALPHABASE	(FDAT_DEPTH * 0x40 / 256)
#endif


void *fontmng_create(int size, UINT type, const TCHAR *fontface) {

	int			i;
	int			fontalign;
	int			fontwork;
	int			allocsize;
	FNTMNG		ret;
	BITMAPINFO	*bi;
	HDC			hdc;
	int			bmpwidth;
	int			bmpheight;
	int			fontsize;
	int			fontwidth;
	int			fontheight;
	LOGFONT		lf;

	if (size < 0) {
		size *= -1;
	}
	if (size < 6) {
		size = 6;
	}
	else if (size > 128) {
		size = 128;
	}

	if (size < 10) {
		type |= FDAT_ALIAS;
	}
	else if (size < 16) {
		type &= ~FDAT_BOLD;
	}

	if (type & FDAT_ALIAS) {
		fontsize = size * 2;
		fontwidth = fontsize;
		fontheight = fontsize;
		if (type & FDAT_BOLD) {
			fontwidth++;
		}
		fontwidth = (fontwidth + 1) >> 1;
		fontheight = (fontheight + 1) >> 1;
		bmpwidth = fontwidth * 2;
		bmpheight = fontheight * 2;
	}
	else {
		fontsize = size;
		fontwidth = fontsize;
		fontheight = fontsize;
		if (type & FDAT_BOLD) {
			fontwidth++;
		}
		bmpwidth = fontwidth;
		bmpheight = fontheight;
	}

	fontalign = sizeof(_FNTDAT) + (fontwidth * fontheight);
	fontalign = (fontalign + 3) & (~3);
#if defined(FONTMNG_CACHE)
	fontwork = fontalign * FONTMNG_CACHE;
#else
	fontwork = fontalign;
#endif

	allocsize = sizeof(_FNTMNG);
	allocsize += fontwork;
	allocsize += sizeof(BITMAPINFOHEADER) + (4 * 2);

	ret = (FNTMNG)_MALLOC(allocsize, "font mng");
	if (ret == NULL) {
		return(NULL);
	}
	ZeroMemory(ret, allocsize);
	ret->fontsize = size;
	ret->fonttype = type;
	ret->fontalign = fontalign;
	ret->fontwidth = fontwidth;
	ret->fontheight = fontheight;
	ret->bmpwidth = bmpwidth;
	ret->bmpheight = bmpheight;
	ret->bmpalign = (((bmpwidth + 31) / 8) & (~3));

	bi = (BITMAPINFO *)(((BYTE *)(ret + 1)) + fontwork);
	bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi->bmiHeader.biWidth = ret->bmpwidth;
	bi->bmiHeader.biHeight = ret->bmpheight;
	bi->bmiHeader.biPlanes = 1;
	bi->bmiHeader.biBitCount = 1;
	bi->bmiHeader.biCompression = BI_RGB;
	bi->bmiHeader.biSizeImage = ret->bmpalign * ret->bmpheight;
	bi->bmiHeader.biXPelsPerMeter = 0;
	bi->bmiHeader.biYPelsPerMeter = 0;
	bi->bmiHeader.biClrUsed = 2;
	bi->bmiHeader.biClrImportant = 2;
	for (i=0; i<2; i++) {
		bi->bmiColors[i].rgbRed = (i ^ 1) - 1;
		bi->bmiColors[i].rgbGreen = (i ^ 1) - 1;
		bi->bmiColors[i].rgbBlue = (i ^ 1) - 1;
		bi->bmiColors[i].rgbReserved = PC_RESERVED;
	}

    hdc = GetDC(NULL);
	ret->hBitmap = CreateDIBSection(hdc, bi, DIB_RGB_COLORS,
											(void **)&ret->image, NULL, 0);
	ret->hdcimage = CreateCompatibleDC(hdc);
	ReleaseDC(NULL, hdc);
	ret->hBitmap = (HBITMAP)SelectObject(ret->hdcimage, ret->hBitmap);
#ifndef _WIN32_WCE
	SetDIBColorTable(ret->hdcimage, 0, 2, bi->bmiColors);
#endif

	lf.lfHeight = fontsize;
	lf.lfWidth = 0;
	lf.lfEscapement = FW_DONTCARE;
	lf.lfOrientation = FW_DONTCARE;
#ifdef _WIN32_WCE
	lf.lfWeight = (type & FDAT_BOLD)?FW_BOLD:FW_THIN;
#else
	lf.lfWeight = (type & FDAT_BOLD)?FW_BOLD:FW_REGULAR;
#endif
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = SHIFTJIS_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = (type & FDAT_PROPORTIONAL)?
												VARIABLE_PITCH:FIXED_PITCH;
	if (fontface == NULL) {
		fontface = (type & FDAT_PROPORTIONAL)?deffontface2:deffontface;
	}
	lstrcpy(lf.lfFaceName, fontface);
	ret->hfont = CreateFontIndirect(&lf);

	ret->hfont = (HFONT)SelectObject(ret->hdcimage, ret->hfont);
	SetTextColor(ret->hdcimage, RGB(255, 255, 255));
	SetBkColor(ret->hdcimage, RGB(0, 0, 0));
	SetRect(&ret->rect, 0, 0, bmpwidth, bmpheight);
	return(ret);
}

void fontmng_destroy(void *hdl) {

	FNTMNG	fhdl;

	fhdl = (FNTMNG)hdl;
	if (fhdl) {
		DeleteObject(SelectObject(fhdl->hdcimage, fhdl->hBitmap));
		DeleteObject(SelectObject(fhdl->hdcimage, fhdl->hfont));
		DeleteDC(fhdl->hdcimage);
		_MFREE(hdl);
	}
}

int fontmng_getpoint(void *hdl) {

	if (hdl) {
	}
	return(0);
}

#ifdef _WIN32_WCE
static void getlength1(FNTMNG fhdl, FNTDAT fdat, LPCTSTR string,
													int tleng, int length) {

	SIZE	fntsize;

	if ((fhdl->fonttype & FDAT_PROPORTIONAL) &&
		(GetTextExtentPoint32(fhdl->hdcimage, string, tleng, &fntsize))) {
		if (fhdl->fonttype & FDAT_ALIAS) {
			fntsize.cx = min(fntsize.cx, fhdl->bmpwidth);
			fdat->width = (fntsize.cx + 1) >> 1;
			fdat->pitch = fntsize.cx >> 1;
			fdat->height = fhdl->bmpheight >> 1;
		}
		else {
			fntsize.cx = min(fntsize.cx, fhdl->bmpwidth);
			fdat->width = fntsize.cx;
			fdat->pitch = fntsize.cx;
			fdat->height = fhdl->bmpheight;
		}
	}
	else {
		if (length < 2) {
			fdat->pitch = fhdl->fontsize >> 1;
		}
		else {
			fdat->pitch = fhdl->fontsize;
		}
		if (fhdl->fonttype & FDAT_ALIAS) {
			fdat->width = fhdl->bmpwidth >> 1;
			fdat->height = fhdl->bmpheight >> 1;
		}
		else {
			fdat->width = fhdl->bmpwidth;
			fdat->height = fhdl->bmpheight;
		}
	}
}

static void fontmng_getchar(FNTMNG fhdl, FNTDAT fdat, const char *string) {

	TCHAR	txtwork[4];
	DWORD	len;
	int		leng;

	FillRect(fhdl->hdcimage, &fhdl->rect,
										(HBRUSH)GetStockObject(BLACK_BRUSH));
	leng = strlen(string);
	len = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, string, -1,
								txtwork, sizeof(txtwork)/sizeof(TCHAR)) - 1;
	ExtTextOut(fhdl->hdcimage, 0, 0, ETO_OPAQUE, NULL,
													txtwork, len, NULL);
	getlength1(fhdl, fdat, txtwork, len, leng);
}

BOOL fontmng_getsize(void *hdl, const char *string, POINT_T *pt) {

	TCHAR	txtwork[4];
	char	buf[4];
	_FNTDAT	fdat;
	int		width;
	DWORD	len;
	int		leng;

	if ((hdl == NULL) || (string == NULL)) {
		goto fmgs_exit;
	}
	width = 0;
	buf[2] = '\0';
	do {
		buf[0] = *string++;
		if ((((buf[0] ^ 0x20) - 0xa1) & 0xff) < 0x3c) {
			buf[1] = *string++;
			if (buf[1] == '\0') {
				break;
			}
			leng = 2;
		}
		else if (buf[0]) {
			buf[1] = '\0';
			leng = 1;
		}
		else {
			break;
		}
		len = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buf, -1,
								txtwork, sizeof(txtwork)/sizeof(TCHAR)) - 1;
		getlength1((FNTMNG)hdl, &fdat, txtwork, len, leng);
		width += fdat.pitch;
	} while(1);

	if (pt) {
		pt->x = width;
		pt->y = ((FNTMNG)hdl)->fontsize;
	}
	return(SUCCESS);

fmgs_exit:
	return(FAILURE);
}

BOOL fontmng_getdrawsize(void *hdl, const char *string, POINT_T *pt) {

	TCHAR	txtwork[4];
	char	buf[4];
	_FNTDAT	fdat;
	int		width;
	int		posx;
	DWORD	len;
	int		leng;

	if ((hdl == NULL) || (string == NULL)) {
		goto fmgds_exit;
	}

	width = 0;
	posx = 0;
	buf[2] = '\0';
	do {
		buf[0] = *string++;
		if ((((buf[0] ^ 0x20) - 0xa1) & 0xff) < 0x3c) {
			buf[1] = *string++;
			if (buf[1] == '\0') {
				break;
			}
			leng = 2;
		}
		else if (buf[0]) {
			buf[1] = '\0';
			leng = 1;
		}
		else {
			break;
		}
		len = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buf, -1,
								txtwork, sizeof(txtwork)/sizeof(TCHAR)) - 1;
		getlength1((FNTMNG)hdl, &fdat, txtwork, len, leng);
		width = posx + max(fdat.width, fdat.pitch);
		posx += fdat.pitch;
	} while(1);

	if (pt) {
		pt->x = width;
		pt->y = ((FNTMNG)hdl)->fontsize;
	}
	return(SUCCESS);

fmgds_exit:
	return(FAILURE);
}

#else

static void getlength1(FNTMNG fhdl, FNTDAT fdat,
											const char *string, int length) {

	SIZE	fntsize;

	if ((fhdl->fonttype & FDAT_PROPORTIONAL) &&
		(GetTextExtentPoint32(fhdl->hdcimage, string, length, &fntsize))) {
		if (fhdl->fonttype & FDAT_ALIAS) {
			fntsize.cx = min(fntsize.cx, fhdl->bmpwidth);
			fdat->width = (fntsize.cx + 1) >> 1;
			fdat->pitch = fntsize.cx >> 1;
			fdat->height = fhdl->bmpheight >> 1;
		}
		else {
			fntsize.cx = min(fntsize.cx, fhdl->bmpwidth);
			fdat->width = fntsize.cx;
			fdat->pitch = fntsize.cx;
			fdat->height = fhdl->bmpheight;
		}
	}
	else {
		if (length < 2) {
			fdat->pitch = fhdl->fontsize >> 1;
		}
		else {
			fdat->pitch = fhdl->fontsize;
		}
		if (fhdl->fonttype & FDAT_ALIAS) {
			fdat->width = fhdl->bmpwidth >> 1;
			fdat->height = fhdl->bmpheight >> 1;
		}
		else {
			fdat->width = fhdl->bmpwidth;
			fdat->height = fhdl->bmpheight;
		}
	}
}

static void fontmng_getchar(FNTMNG fhdl, FNTDAT fdat, const char *string) {

	int		leng;

	FillRect(fhdl->hdcimage, &fhdl->rect,
										(HBRUSH)GetStockObject(BLACK_BRUSH));
	leng = strlen(string);
	TextOut(fhdl->hdcimage, 0, 0, string, leng);
	getlength1(fhdl, fdat, string, leng);
}

BOOL fontmng_getsize(void *hdl, const char *string, POINT_T *pt) {

	char	buf[4];
	_FNTDAT	fdat;
	int		width;
	int		leng;

	width = 0;
	if ((hdl == NULL) || (string == NULL)) {
		goto fmgs_exit;
	}

	buf[2] = '\0';
	do {
		buf[0] = *string++;
		if ((((buf[0] ^ 0x20) - 0xa1) & 0xff) < 0x3c) {
			buf[1] = *string++;
			if (buf[1] == '\0') {
				break;
			}
			leng = 2;
		}
		else if (buf[0]) {
			buf[1] = '\0';
			leng = 1;
		}
		else {
			break;
		}
		getlength1((FNTMNG)hdl, &fdat, buf, leng);
		width += fdat.pitch;
	} while(1);

	if (pt) {
		pt->x = width;
		pt->y = ((FNTMNG)hdl)->fontsize;
	}
	return(SUCCESS);

fmgs_exit:
	return(FAILURE);
}

BOOL fontmng_getdrawsize(void *hdl, const char *string, POINT_T *pt) {

	char	buf[4];
	_FNTDAT	fdat;
	int		width;
	int		posx;
	int		leng;

	if ((hdl == NULL) || (string == NULL)) {
		goto fmgds_exit;
	}

	width = 0;
	posx = 0;
	buf[2] = '\0';
	do {
		buf[0] = *string++;
		if ((((buf[0] ^ 0x20) - 0xa1) & 0xff) < 0x3c) {
			buf[1] = *string++;
			if (buf[1] == '\0') {
				break;
			}
			leng = 2;
		}
		else if (buf[0]) {
			buf[1] = '\0';
			leng = 1;
		}
		else {
			break;
		}
		getlength1((FNTMNG)hdl, &fdat, buf, leng);
		width = posx + max(fdat.width, fdat.pitch);
		posx += fdat.pitch;
	} while(1);

	if (pt) {
		pt->x = width;
		pt->y = ((FNTMNG)hdl)->fontsize;
	}
	return(SUCCESS);

fmgds_exit:
	return(FAILURE);
}
#endif

static void fontmng_setpat(FNTMNG fhdl, FNTDAT fdat) {

	DWORD	remx;
	DWORD	remy;
	BYTE	*src;
	BYTE	*dst;
	BYTE	*s1, *s2;
	BYTE	bit;
	BYTE	b1, b2;
	int		align;
	int		c;

	align = fhdl->bmpalign;
	src = fhdl->image + (fhdl->rect.bottom * align);
	if (fdat->width <= 0) {
		goto fmsp_end;
	}

	dst = (BYTE *)(fdat + 1);
	align *= -1;

	if (fhdl->fonttype & FDAT_ALIAS) {
		remy = fdat->height;
		do {
			src += align;
			s1 = src;
			src += align;
			s2 = src;
			remx = fdat->width;
			bit = 0;
			do {
				if (bit == 0) {
					bit = 4;
					b1 = *s1++;
					b2 = *s2++;
				}
				c = 0;
				if (b1 & 0x80) {
					c += (FDAT_DEPTH - TEXTALPHABASE);
				}
				if (b1 & 0x40) {
					c += (FDAT_DEPTH - TEXTALPHABASE);
				}
				if (b2 & 0x80) {
					c += (FDAT_DEPTH - TEXTALPHABASE);
				}
				if (b2 & 0x40) {
					c += (FDAT_DEPTH - TEXTALPHABASE);
				}
				if (c) {
					*dst++ = (BYTE)(TEXTALPHABASE + (c / 4));
				}
				else {
					*dst++ = 0;
				}
				b1 <<= 2;
				b2 <<= 2;
				bit--;
			} while(--remx);
		} while(--remy);
	}
	else {
		remy = fdat->height;
		do {
			src += align;
			s1 = src;
			remx = fdat->width;
			bit = 0;
			do {
				if (bit == 0) {
					bit = 0x80;
					b1 = *s1++;
				}
				*dst++ = (b1 & bit)?FDAT_DEPTH:0x00;
				bit >>= 1;
			} while(--remx);
		} while(--remy);
	}

fmsp_end:
	return;
}

FNTDAT fontmng_get(void *hdl, const char *string) {

	FNTMNG	fhdl;
	FNTDAT	fdat;

	if ((hdl == NULL) || (string == NULL)) {
		goto ftmggt_err;
	}
	fhdl = (FNTMNG)hdl;

#if defined(FONTMNG_CACHE)
{
	FNTCTBL	*fct;
	DWORD	str;
	DWORD	pos;
	DWORD	prev;
	DWORD	cnt;

	str = string[0] & 0xff;
	str |= (string[1] & 0xff) << 8;

	fct = fhdl->cache;
	cnt = fhdl->caches;
	pos = fhdl->cachehead;
	prev = FONTMNG_CACHE;
	while(cnt--) {
		if (fct[pos].str != str) {
			prev = pos;
			pos = fct[pos].next;
			continue;
		}
		if (prev < FONTMNG_CACHE) {
			fct[prev].next = fct[pos].next;
			fct[pos].next = (WORD)fhdl->cachehead;
			fhdl->cachehead = pos;
		}
		return((FNTDAT)(((BYTE *)(fhdl + 1)) + (pos * fhdl->fontalign)));
	}
	if (fhdl->caches < FONTMNG_CACHE) {
		prev = fhdl->caches;
		fhdl->caches++;
	}
	fct[prev].str = (WORD)str;
	fct[prev].next = (WORD)fhdl->cachehead;
	fhdl->cachehead = prev;
	fdat = (FNTDAT)(((BYTE *)(fhdl + 1)) + (prev * fhdl->fontalign));
}
#else
	fdat = (FNTDAT)(fhdl + 1);
#endif

	fontmng_getchar(fhdl, fdat, string);
	fontmng_setpat(fhdl, fdat);
	return(fdat);

ftmggt_err:
	return(NULL);
}


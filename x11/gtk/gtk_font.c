/*
 * Copyright (c) 2003 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "compiler.h"

#include "np2.h"
#include "codecnv.h"

#include "fontmng.h"

#include "gtk/xnp2.h"


typedef struct {
	int		fontsize;
	UINT		fonttype;
	int		fontwidth;
	int		fontheight;

	GdkFont		*fontset;
	GdkPixmap	*pixmap;
	unsigned long	black_pixel;
} _FNTMNG, *FNTMNG;

void
fontmng_setdeffontname(const char *name)
{

	/* これじゃ無理 */
	milstr_ncpy(fontname, name, sizeof(fontname));
}

void *
fontmng_create(int size, UINT type, const TCHAR *fontface)
{
	char work[1024];
	_FNTMNG fnt;
	FNTMNG fntp;
	char *ftype;
	int fontalign;
	int allocsize;
	GdkColormap *colormap;
	GdkColor color;

	UNUSED(fontface);

	if (size < 0) {
		size = -size;
	}
	if (size < 6) {
		size = 6;
	} else if (size > 128) {
		size = 128;
	}

	fnt.fontsize = size;
	fnt.fontwidth = size + 1;
	fnt.fontheight = size + 1;
	fnt.fonttype = type;
	if (type & FDAT_BOLD) {
		fnt.fontwidth++;
		ftype = "bold";
	} else {
		ftype = "medium";
	}

	fontalign = sizeof(_FNTDAT) + (fnt.fontwidth * fnt.fontheight);
	fontalign = roundup(fontalign, 4);

	allocsize = sizeof(_FNTMNG);
	allocsize += fontalign;

	/* GTK */
	colormap = gtk_widget_get_colormap(GTK_WIDGET(window));
	color.red = color.green = color.blue = 0;
	if (gdk_color_alloc(colormap, &color) == 0) {
		fnt.black_pixel = 0;
	} else {
		fnt.black_pixel = color.pixel;
	}

	g_snprintf(work, sizeof(work), fontname, ftype, size);
	fnt.fontset = gdk_fontset_load(work);
	if (fnt.fontset == NULL) {
		fprintf(stderr, "Warning: Can't create font-set.\n");
		return NULL;
	}
	fnt.pixmap = gdk_pixmap_new(window->window, fnt.fontwidth, fnt.fontheight, -1);

	fntp = _MALLOC(allocsize, "font mng");
	if (fntp) {
		memset(fntp, 0, allocsize);
		memcpy(fntp, &fnt, sizeof(fnt));
	} else {
		gdk_pixmap_unref(fnt.pixmap);
		gdk_font_unref(fnt.fontset);
	}
	return fntp;
}

void
fontmng_destroy(void *hdl)
{
	FNTMNG fnt = (FNTMNG)hdl;

	if (fnt) {
		gdk_pixmap_unref(fnt->pixmap);
		gdk_font_unref(fnt->fontset);
		_MFREE(fnt);
	}
}

static void
setfdathead(FNTMNG fhdl, FNTDAT fdat, const BYTE *str, int len)
{

	UNUSED(str);

	fdat->width = fhdl->fontwidth;
	fdat->height = fhdl->fontheight;
	fdat->pitch = fhdl->fontsize;
	if (len < 2) {
		fdat->pitch = (fdat->pitch + 1) >> 1;
	}
}

static void
getlength1(FNTMNG fhdl, FNTDAT fdat, const BYTE *str, int len)
{

	setfdathead(fhdl, fdat, str, len);
}

static void
getfont1(FNTMNG fhdl, FNTDAT fdat, const BYTE *str, int len)
{
	GdkImage *img;

	getlength1(fhdl, fdat, str, len);

	gdk_draw_rectangle(fhdl->pixmap, window->style->white_gc, TRUE,
	    0, 0, fhdl->fontwidth, fhdl->fontheight);
	gdk_draw_text(fhdl->pixmap, fhdl->fontset, window->style->black_gc,
	    0, 14, str, strlen(str));
	img = gdk_image_get(fhdl->pixmap, 0, 0, fhdl->fontwidth,
	    fhdl->fontheight);
	if (img) {
		BYTE *p = (BYTE *)(fdat + 1);
		unsigned long black = fhdl->black_pixel;
		unsigned long pixel;
		int x, y;

		for (y = 0; y < fdat->height; y++) {
			for (x = 0; x < fdat->width; x++) {
				pixel = gdk_image_get_pixel(img, x, y);
				if (pixel == black) {
					*p = FDAT_DEPTH;
				} else {
					*p = 0x00;
				}
				p++;
			}
		}
		gdk_image_destroy(img);
	} else {
		memset(fdat + 1, 0, fdat->width * fdat->height);
	}
}

BOOL
fontmng_getsize(void *hdl, const char *str, POINT_T *pt)
{
	FNTMNG fhdl = (FNTMNG)hdl;
	_FNTDAT fdat;
	BYTE buf[4];
	int width;
	int len;

	if ((fhdl == NULL) || (str == NULL))
		return FAILURE;

	width = 0;
	buf[2] = '\0';
	for (;;) {
		buf[0] = *str++;
		if ((((buf[0] ^ 0x20) - 0xa1) & 0xff) < 0x3c && *str != '\0') {
			buf[1] = *str++;
			len = 2;
		} else if (buf[0] >= 0xa1 && buf[1] <= 0xdf) {
			buf[1] = buf[0];
			buf[0] = 0x8e;
			len = 1;
		} else if (buf[0]) {
			buf[1] = '\0';
			len = 1;
		} else {
			break;
		}
		getlength1(fhdl, &fdat, buf, len);
		width += fdat.pitch;
	}
	if (pt) {
		pt->x = width;
		pt->y = fhdl->fontsize;
	}
	return SUCCESS;
}

BOOL
fontmng_getdrawsize(void *hdl, const char *str, POINT_T *pt)
{
	FNTMNG fhdl = (FNTMNG)hdl;
	_FNTDAT fdat;
	BYTE buf[4];
	int width;
	int len;
	int posx;

	if ((hdl == NULL) || (str == NULL))
		return FAILURE;

	width = 0;
	posx = 0;
	buf[2] = '\0';
	for (;;) {
		buf[0] = *str++;
		if ((((buf[0] ^ 0x20) - 0xa1) & 0xff) < 0x3c && *str != '\0') {
			buf[1] = *str++;
			len = 2;
		} else if (buf[0] >= 0xa1 && buf[0] <= 0xdf) {
			buf[1] = buf[0];
			buf[0] = 0x8e;
			len = 1;
		} else if (buf[0]) {
			buf[1] = '\0';
			len = 1;
		} else {
			break;
		}
		getlength1(fhdl, &fdat, buf, len);
		width = posx + max(fdat.width, fdat.pitch);
		posx += fdat.pitch;
	}
	if (pt) {
		pt->x = width;
		pt->y = fhdl->fontsize;
	}
	return SUCCESS;
}

FNTDAT
fontmng_get(void *hdl, const char *str)
{
	FNTMNG fhdl = (FNTMNG)hdl;
	FNTDAT fdat = (FNTDAT)(fhdl + 1);
	BYTE buf[4];
	int len;

	if ((fhdl == NULL) || (str == NULL)) {
		return NULL;
	}

	if (((((str[0] ^ 0x20) - 0xa1) & 0xff) < 0x3c) && (str[1] != '\0')) {
		codecnv_sjis2euc(buf, 4, str, 2);
		len = 2;
	} else if ((BYTE)str[0] >= 0xa1 && (BYTE)str[0] <= 0xdf) {
		buf[0] = 0x8e;
		buf[1] = str[0];
		buf[2] = '\0';
		len = 1;
	} else {
		buf[0] = str[0];
		buf[1] = '\0';
		len = 1;
	}
	getfont1(fhdl, fdat, buf, len);
	return fdat;
}

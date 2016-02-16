/*
 * Copyright (c) 2004 NONAKA Kimihiro
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
#include "codecnv/codecnv.h"

#include "fontmng.h"

#include "gtk2/xnp2.h"


struct tagFontMng {
	int			size;
	UINT			type;
	GdkRectangle		rect;

	PangoFontDescription	*desc;
	PangoLayout		*layout;
	GdkPixmap		*backsurf;
	unsigned long		black_pixel;
};


BRESULT
fontmng_init(void)
{

	fontmng_setdeffontname("fixed");

	return SUCCESS;
}

void
fontmng_terminate(void)
{

	/* Nothing to do */
}

void
fontmng_setdeffontname(const OEMCHAR *fontface)
{

	milstr_ncpy(fontname, fontface, sizeof(fontname));
}

FONTMNGH
fontmng_create(int size, UINT type, const OEMCHAR *fontface)
{
	char buf[256];
	struct tagFontMng fnt;
	FONTMNGH fntp;
	gchar *fontname_utf8;
	int fontalign;
	int allocsize;
	GdkColormap *cmap;
	GdkColor color;
	gboolean rv;

	if (size < 0) {
		size = -size;
	}
	if (size < 6) {
		size = 6;
	} else if (size > 128) {
		size = 128;
	}

	g_snprintf(buf, sizeof(buf), "%s %s %s %d",
	    fontface ? (const char *)fontface : fontname,
	    (type & FDAT_BOLD) ? "bold" : "medium",
	    (type & FDAT_PROPORTIONAL) ? "" : "",
	    size);
	fontname_utf8 = g_locale_to_utf8(buf, -1, NULL, NULL, NULL);
	if (fontname_utf8 == NULL) {
		return NULL;
	}
	fnt.desc = pango_font_description_from_string(fontname_utf8);
	g_free(fontname_utf8);
	if (fnt.desc == NULL) {
		return NULL;
	}

	cmap = gtk_widget_get_colormap(GTK_WIDGET(main_window));
	memset(&color, 0, sizeof(color));
	rv = gdk_colormap_alloc_color(cmap, &color, FALSE, FALSE);
	if (rv) {
		fnt.black_pixel = color.pixel;
	} else {
		fnt.black_pixel = 0;
	}

	fnt.layout = gtk_widget_create_pango_layout(main_window, NULL);
	pango_layout_set_font_description(fnt.layout, fnt.desc);
	pango_layout_context_changed(fnt.layout);

	fnt.size = size;
	fnt.type = type;
	fnt.rect.x = fnt.rect.y = 0;
	fnt.rect.width = size + 1;
	fnt.rect.height = size + 1;

	fontalign = sizeof(_FNTDAT) + (fnt.rect.width * fnt.rect.height);
	fontalign = roundup(fontalign, 4);

	allocsize = sizeof(fnt);
	allocsize += fontalign;

	fnt.backsurf = gdk_pixmap_new(main_window->window,
	    fnt.rect.width, fnt.rect.height, -1);

	fntp = _MALLOC(allocsize, "fontmng");
	if (fntp) {
		memset(fntp, 0, allocsize);
		*fntp = fnt;
	} else {
		pango_font_description_free(fnt.desc);
	}
	return fntp;
}

void
fontmng_destroy(FONTMNGH fhdl)
{
	if (fhdl) {
		if (fhdl->backsurf)
			g_object_unref(fhdl->backsurf);
		if (fhdl->layout)
			g_object_unref(fhdl->layout);
		if (fhdl->desc)
			pango_font_description_free(fhdl->desc);
		_MFREE(fhdl);
	}
}

static void
setfdathead(FONTMNGH fhdl, FNTDAT fdat, const char *str, int len)
{

	fdat->width = fhdl->rect.width;
	fdat->height = fhdl->rect.height;
	fdat->pitch = fhdl->size;
	if (len < 2) {
		fdat->pitch = (fdat->pitch + 1) >> 1;
	}
}

static void
getlength1(FONTMNGH fhdl, FNTDAT fdat, const char *str, int len)
{

	setfdathead(fhdl, fdat, str, len);
}

static void
getfont1(FONTMNGH fhdl, FNTDAT fdat, const char *str, int len)
{
	GdkImage *img;

	getlength1(fhdl, fdat, str, len);

	gdk_draw_rectangle(fhdl->backsurf, main_window->style->white_gc, TRUE,
	    0, 0, fhdl->rect.width, fhdl->rect.height);
	pango_layout_set_text(fhdl->layout, str, -1);
	gdk_draw_layout(fhdl->backsurf, main_window->style->black_gc,
	    0, 0, fhdl->layout);
	img = gdk_drawable_get_image(fhdl->backsurf,
	    0, 0, fhdl->rect.width, fhdl->rect.height);
	if (img) {
		UINT8 *p = (UINT8 *)(fdat + 1);
		unsigned long pixel;
		int x, y;

		for (y = 0; y < fdat->height; y++) {
			for (x = 0; x < fdat->width; x++) {
				pixel = gdk_image_get_pixel(img, x, y);
				if (pixel == fhdl->black_pixel) {
					*p = FDAT_DEPTH;
				} else {
					*p = 0x00;
				}
				p++;
			}
		}
		g_object_unref(img);
	} else {
		memset(fdat + 1, 0, fdat->width * fdat->height);
	}
}

BRESULT
fontmng_getsize(FONTMNGH fhdl, const char *str, POINT_T *pt)
{
	_FNTDAT fdat;
	char buf[4];
	int width;
	int len;

	if ((fhdl == NULL) || (str == NULL)) {
		return FAILURE;
	}

	width = 0;
	for (;;) {
		while ((len = milstr_charsize(str)) != 0) {
			memcpy(buf, str, len * sizeof(char));
			buf[len] = '\0';
			getlength1(fhdl, &fdat, buf, len);
			width += fdat.pitch;
		}
	}
	if (pt) {
		pt->x = width;
		pt->y = fhdl->size;
	}
	return SUCCESS;
}

BRESULT
fontmng_getdrawsize(FONTMNGH fhdl, const char *str, POINT_T *pt)
{
	_FNTDAT fdat;
	char buf[4];
	int width;
	int len;
	int posx;

	if ((fhdl == NULL) || (str == NULL)) {
		return FAILURE;
	}

	width = 0;
	posx = 0;
	for (;;) {
		while ((len = milstr_charsize(str)) != 0) {
			memcpy(buf, str, len * sizeof(char));
			buf[len] = '\0';
			getlength1(fhdl, &fdat, buf, len);
			width = posx + max(fdat.width, fdat.pitch);
			posx += fdat.pitch;
		}
	}
	if (pt) {
		pt->x = width;
		pt->y = fhdl->size;
	}
	return SUCCESS;
}

FNTDAT
fontmng_get(FONTMNGH fhdl, const char *str)
{
	FNTDAT fdat = (FNTDAT)(fhdl + 1);
	char buf[4];
	gchar *utf8;
	int len;

	if ((fhdl == NULL) || (str == NULL)) {
		return NULL;
	}

	while ((len = milstr_charsize(str)) != 0) {
		memcpy(buf, str, len * sizeof(char));
		buf[len] = '\0';
		utf8 = g_locale_to_utf8(buf, -1, NULL, NULL, NULL);
		if (utf8) {
			getfont1(fhdl, fdat, utf8, len);
			g_free(utf8);
		}
		str += len;
	}
	return fdat;
}

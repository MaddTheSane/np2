/*	$Id: gtk_screen.c,v 1.4 2005/03/12 12:36:57 monaka Exp $	*/

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
#include "palettes.h"
#include "scrndraw.h"

#include "scrnmng.h"

#include "gtk2/xnp2.h"
#include "gtk2/gtk_drawmng.h"


typedef struct {
	UINT8		scrnmode;
	UINT8		drawing;
	int		width;
	int		height;
	int		extend;
	int		clipping;

	PAL16MASK	pal16mask;

	RECT_T		scrn;
	RECT_T		rect;

	int		lpitch;
	int		scrw;
	int		scrh;

	/* toolkit depend */
	GdkImage	*surface;
	GdkPixmap	*backsurf;

	unsigned long	pixel[24];	/* pallete */
} DRAWMNG;

typedef struct {
	int	width;
	int	height;
	int	extend;
} SCRNSTAT;

typedef struct {
	SCRNSURF	ss;

	UINT8		renewal[SURFACE_HEIGHT];
} X11SCRNSURF;

static SCRNMNG scrnmng;
static DRAWMNG drawmng;
static SCRNSTAT scrnstat;
static X11SCRNSURF scrnsurf;

SCRNMNG *scrnmngp = &scrnmng;

GtkWidget *main_window;
GtkWidget *drawarea;


static void
set_window_size(int width, int height)
{

	drawmng.scrw = width + np2oscfg.paddingx * 2;
	drawmng.scrh = height + np2oscfg.paddingy * 2;

	gtk_widget_set_size_request(drawarea, drawmng.scrw, drawmng.scrh);
}

static void
renewal_client_size(void)
{
	int width;
	int height;
	int extend;
	int scrnwidth;
	int scrnheight;

	width = min(scrnstat.width, drawmng.width);
	height = min(scrnstat.height, drawmng.height);
	extend = 0;

#if notyet
	if (drawmng.scrnmode & SCRNMODE_FULLSCREEN) {
	} else
#endif
	{
		if (!(drawmng.scrnmode & SCRNMODE_ROTATE)) {
			if (np2oscfg.paddingx > 0) {
				extend = min(scrnstat.extend, drawmng.extend);
			}
			scrnwidth = width;
			scrnheight = height;
			drawmng.rect.right = width + extend;
			drawmng.rect.bottom = height;
			drawmng.scrn.left = np2oscfg.paddingx - extend;
			drawmng.scrn.top = np2oscfg.paddingy;
		} else {
			if (np2oscfg.paddingy > 0) {
				extend = min(scrnstat.extend, drawmng.extend);
			}
			scrnwidth = height;
			scrnheight = width;
			drawmng.rect.right = height;
			drawmng.rect.bottom = width + extend;
			drawmng.scrn.left = np2oscfg.paddingx;
			drawmng.scrn.top = np2oscfg.paddingy - extend;
		}
		drawmng.scrn.right = np2oscfg.paddingx + scrnwidth;
		drawmng.scrn.bottom = np2oscfg.paddingy + scrnheight;
		set_window_size(scrnwidth, scrnheight);
	}
	scrnsurf.ss.width = width;
	scrnsurf.ss.height = height;
	scrnsurf.ss.extend = extend;
}

static void
clear_out_of_rect(const RECT_T *target, const RECT_T *base)
{
	GdkDrawable *d = drawarea->window;
	GdkGC *gc = drawarea->style->black_gc;
	RECT_T rect;

	rect.left = base->left;
	rect.right = base->right;
	rect.top = base->top;
	rect.bottom = target->top;
	if (rect.top < rect.bottom) {
		gdk_draw_rectangle(d, gc, TRUE,
		    rect.left, rect.top, rect.right, rect.bottom);
	}
	rect.top = target->bottom;
	rect.bottom = base->bottom;
	if (rect.top < rect.bottom) {
		gdk_draw_rectangle(d, gc, TRUE,
		    rect.left, rect.top, rect.right, rect.bottom);
	}

	rect.top = max(base->top, target->top);
	rect.bottom = min(base->bottom, target->bottom);
	if (rect.top < rect.bottom) {
		rect.left = base->left;
		rect.right = target->left;
		if (rect.left < rect.right) {
			gdk_draw_rectangle(d, gc, TRUE,
			    rect.left, rect.top, rect.right, rect.bottom);
		}
		rect.left = target->right;
		rect.right = base->right;
		if (rect.left < rect.right) {
			gdk_draw_rectangle(d, gc, TRUE,
			    rect.left, rect.top, rect.right, rect.bottom);
		}
	}
}

static void
clear_outscreen(void)
{
	RECT_T target;
	RECT_T base;

	base.left = base.top = 0;
	base.right = drawmng.scrw;
	base.bottom = drawmng.scrh;
	target = drawmng.scrn;
	clear_out_of_rect(&target, &base);
}

static void
palette_init(void)
{
	GdkColor color;
	GdkColormap *cmap;
	int rv;
	int i;

	cmap = gdk_colormap_get_system();
	rv = gdk_colors_alloc(cmap, TRUE, NULL, 0, drawmng.pixel, 24);
	if (rv == 0) {
		g_error("Can't allocate enough color.\n");
		return;
	}

	for (i = 0; i < 8; i++) {
		color.pixel = drawmng.pixel[NP2PAL_TEXT + i + 1];
		color.red = np2_pal32[NP2PAL_TEXT + i + 1].p.r << 8;
		color.green = np2_pal32[NP2PAL_TEXT + i + 1].p.g << 8;
		color.blue = np2_pal32[NP2PAL_TEXT + i + 1].p.b << 8;
		gdk_colormap_alloc_color(cmap, &color, TRUE, FALSE);
	}
}

static void
palette_set(void)
{
	GdkColor color;
	GdkColormap *cmap;
	int i;

	cmap = gdk_colormap_get_system();
	for (i = 0; i < NP2PALS_GRPH; i++) {
		color.pixel = drawmng.pixel[NP2PAL_GRPH + i];
		color.red = np2_pal32[NP2PAL_GRPH + i].p.r << 8;
		color.green = np2_pal32[NP2PAL_GRPH + i].p.g << 8;
		color.blue = np2_pal32[NP2PAL_GRPH + i].p.b << 8;
		gdk_colormap_alloc_color(cmap, &color, TRUE, FALSE);
	}
}

void
scrnmng_initialize(void)
{

	drawmng.drawing = FALSE;
	scrnstat.width = 640;
	scrnstat.height = 400;
	scrnstat.extend = 1;
	set_window_size(scrnstat.width, scrnstat.height);
}

BOOL
scrnmng_create(UINT8 mode)
{
	GdkVisual *visual;
	RECT_T rect;
	int height;
	UINT lpitch;
	UINT8 bytes_per_pixel;
	pixmap_format_t fmt;
	int padding;

	while (drawmng.drawing)
		gtk_main_iteration_do(FALSE);
	drawmng.drawing = TRUE;

#if notyet
	if (mode & SCRNMODE_FULLSCREEN) {
		mode &= ~SCRNMODE_ROTATEMASK;
		drawmng.extend = 0;
		return FAILURE;
	} else
#endif
	{
		scrnmng.flag = SCRNFLAG_HAVEEXTEND;

		visual = gtk_widget_get_visual(drawarea);
		if (!gtkdrawmng_getformat(drawarea, main_window, &fmt))
			return FAILURE;

		switch (fmt.bits_per_pixel) {
		case 16:
			drawmng_make16mask(&drawmng.pal16mask, visual->blue_mask, visual->red_mask, visual->green_mask);
			break;

		case 8:
			palette_init();
			break;
		}
		drawmng.extend = 1;
		bytes_per_pixel = (UINT8)(fmt.bits_per_pixel / 8);

		if (!(mode & SCRNMODE_ROTATE)) {
			rect.right = 641;
			rect.bottom = 480;
		} else {
			rect.right = 480;
			rect.bottom = 641;
		}
		lpitch = rect.right * bytes_per_pixel;
		padding = lpitch % (fmt.scanline_pad / 8);
		if (padding > 0) {
			rect.right += padding / bytes_per_pixel;
			lpitch = rect.right * bytes_per_pixel;
		}
		height = 480;

		drawmng.surface = gdk_image_new(GDK_IMAGE_FASTEST, visual,
		    rect.right, rect.bottom);
		if (drawmng.surface == NULL) {
			g_message("can't create surface.");
			return FAILURE;
		}

		drawmng.backsurf = gdk_pixmap_new(drawarea->window,
		    rect.right, rect.bottom, visual->depth);
		if (drawmng.backsurf == NULL) {
			g_message("can't create pixmap.");
			return FAILURE;
		}
		gdk_draw_rectangle(drawmng.backsurf, drawarea->style->black_gc,
		    TRUE, 0, 0, rect.right, rect.bottom);
	}
	scrnmng.bpp = (UINT8)fmt.bits_per_pixel;
	drawmng.lpitch = lpitch;
	scrnsurf.ss.bpp = fmt.bits_per_pixel;
	drawmng.scrnmode = mode;
	drawmng.width = 640;
	drawmng.height = height;
	drawmng.clipping = 0;
	renewal_client_size();

	drawmng.drawing = FALSE;

	return SUCCESS;
}

void
scrnmng_destroy(void)
{

	if (drawmng.backsurf) {
		gdk_pixmap_unref(drawmng.backsurf);
		drawmng.backsurf = NULL;
	}
	if (drawmng.surface) {
		gdk_image_destroy(drawmng.surface);
		drawmng.surface = NULL;
	}
}

RGB16
scrnmng_makepal16(RGB32 pal32)
{

	return drawmng_makepal16(&drawmng.pal16mask, pal32);
}

void
scrnmng_setwidth(int posx, int width)
{

	UNUSED(posx);

	scrnstat.width = width;
	renewal_client_size();
}

void
scrnmng_setheight(int posy, int height)
{

	UNUSED(posy);

	scrnstat.height = height;
	renewal_client_size();
}

void
scrnmng_setextend(int extend)
{

	scrnstat.extend = extend;
	renewal_client_size();
}

const SCRNSURF *
scrnmng_surflock(void)
{
	int lpitch = drawmng.lpitch;
	int bytes_per_pixel = scrnsurf.ss.bpp >> 3;

#if !defined(USE_SCREEN_INVALIDATE)
	memcpy(scrnsurf.renewal, renewal_line, sizeof(scrnsurf.renewal));
#endif

	scrnsurf.ss.ptr = (UINT8 *)drawmng.surface->mem;
	if (!(drawmng.scrnmode & SCRNMODE_ROTATE)) {
		scrnsurf.ss.xalign = bytes_per_pixel;
		scrnsurf.ss.yalign = lpitch;
	} else if (!(drawmng.scrnmode & SCRNMODE_ROTATEDIR)) {
		/* rotate left */
		scrnsurf.ss.ptr += (scrnsurf.ss.width + scrnsurf.ss.extend - 1) * lpitch;
		scrnsurf.ss.xalign = -lpitch;
		scrnsurf.ss.yalign = bytes_per_pixel;
	} else {
		/* rotate right */
		scrnsurf.ss.ptr += (drawmng.rect.right - 1) * bytes_per_pixel;
		scrnsurf.ss.xalign = lpitch;
		scrnsurf.ss.yalign = -bytes_per_pixel;
	}
	return &scrnsurf.ss;
}

void
scrnmng_surfunlock(const SCRNSURF *surf)
{
	GdkDrawable *d = drawarea->window;
	GdkGC *gc = drawarea->style->fg_gc[GTK_WIDGET_STATE(drawarea)];
	X11SCRNSURF *ss = (X11SCRNSURF *)surf;
	UINT8 *delta = ss->renewal;
	RECT_T r;
	gint h, s;

	r.left = drawmng.scrn.left;
	r.top = drawmng.scrn.top;
	r.right = drawmng.rect.right;
	r.bottom = drawmng.rect.bottom;

#if !defined(USE_SCREEN_INVALIDATE)
	if (!(drawmng.scrnmode & SCRNMODE_ROTATE)) {
		/* normal */
		for (s = h = 0; h < r.bottom; h++) {
			if ((renewal_line[h] != delta[h]) || (s == h))
				continue;

			gdk_draw_image(drawmng.backsurf, gc, drawmng.surface,
			    0, s, r.left, r.top + s, r.right, h - s);
			s = h + 1;
		}
		if (s != h) {
			gdk_draw_image(drawmng.backsurf, gc, drawmng.surface,
			    0, s, r.left, r.top + s, r.right, h - s);
		}
	} else if (!(drawmng.scrnmode & SCRNMODE_ROTATEDIR)) {
		/* rotate left */
		for (s = h = 0; h < r.right; h++) {
			if ((renewal_line[h] != delta[h]) || (s == h))
				continue;

			gdk_draw_image(drawmng.backsurf, gc, drawmng.surface,
			    s, 0, r.left + s, r.top, h - s, r.bottom);
			s = h + 1;
		}
		if (s != h) {
			gdk_draw_image(drawmng.backsurf, gc, drawmng.surface,
			    s, 0, r.left + s, r.top, h - s, r.bottom);
		}
	} else {
		/* rotate right */
		for (s = h = 0; h < r.right; h++) {
			if ((renewal_line[h] != delta[h]) || (s == h))
				continue;

			gdk_draw_image(drawmng.backsurf, gc, drawmng.surface,
			    r.right - h, 0,
			    drawmng.scrn.right - h, r.top, h - s, r.bottom);
			s = h + 1;
		}
		if (s != h) {
			gdk_draw_image(drawmng.backsurf, gc, drawmng.surface,
			    r.right - h, 0,
			    drawmng.scrn.right - h, r.top, h - s, r.bottom);
		}
	}
#else
	gtk_widget_queue_draw_area(drawarea, r.left, r.top, r.right, r.bottom);
#endif

	if (scrnmng.palchanged) {
		scrnmng.palchanged = FALSE;
		palette_set();
	}

#if !defined(USE_SCREEN_INVALIDATE)
	if (drawmng.drawing)
		return;

	drawmng.drawing = TRUE;
#if notyet
	if (drawmng.scrnmode & SCRNMODE_FULLSCREEN) {
	} else
#endif
	{
		if (scrnmng.allflash) {
			scrnmng.allflash = 0;
			if (np2oscfg.paddingx || np2oscfg.paddingy) {
				clear_outscreen();
			}
		}
		gdk_draw_pixmap(d, gc, drawmng.backsurf,
		    0, 0,                                     /* src  */
		    drawmng.scrn.left, drawmng.scrn.top,      /* dest */
		    drawmng.rect.right, drawmng.rect.bottom); /* w/h  */
	}
	drawmng.drawing = FALSE;
#endif
}

void
scrnmng_draw(RECT_T *r)
{
	GdkDrawable *d = drawarea->window;
	GdkGC *gc = drawarea->style->fg_gc[GTK_WIDGET_STATE(drawarea)];

	if (drawmng.drawing)
		return;

	drawmng.drawing = TRUE;

	if (r->right > drawmng.rect.right)
		r->right = drawmng.rect.right;
	if (r->bottom > drawmng.rect.bottom)
		r->bottom = drawmng.rect.bottom;

#if notyet
	if (drawmng.scrnmode & SCRNMODE_FULLSCREEN) {
	} else
#endif
	{
		if (scrnmng.allflash) {
			scrnmng.allflash = 0;
			if (np2oscfg.paddingx || np2oscfg.paddingy) {
				clear_outscreen();
			}
		}
#if !defined(USE_SCREEN_INVALIDATE)
		gdk_draw_pixmap(d, gc, drawmng.backsurf,
		    r->left, r->top,
		    r->left, r->top, r->right, r->bottom);
#else
		gdk_draw_image(d, gc, drawmng.surface,
		    r->left, r->top,
		    r->left, r->top, r->right, r->bottom);
#endif
	}
	drawmng.drawing = FALSE;
}

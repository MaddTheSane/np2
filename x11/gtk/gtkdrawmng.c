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
#include "palettes.h"
#include "scrndraw.h"

#include "scrnmng.h"

#include "gtk/xnp2.h"
#include "gtk/gtkdrawmng.h"


GTKDRAWMNG_HDL
gtkdrawmng_create(GtkWidget *parent_window, int width, int height)
{
	GTKDRAWMNG_HDL hdl = NULL;
	GdkVisual *visual;
	int bitcolor;
	int bytes_per_pixel;

	if (parent_window == NULL)
		return NULL;

	hdl = (GTKDRAWMNG_HDL)_MALLOC(sizeof(_GTKDRAWMNG_HDL), "drawmng hdl");
	if (hdl == NULL)
		return NULL;
	memset(hdl, 0, sizeof(_GTKDRAWMNG_HDL));

	hdl->width = width;
	hdl->height = height;

	hdl->drawarea = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(hdl->drawarea), width, height);

	visual = gtk_widget_get_visual(hdl->drawarea);
	bitcolor = gtkdrawmng_getbpp(hdl->drawarea, parent_window);
	if (bitcolor == 0)
		goto destroy;
	bytes_per_pixel = bitcolor / 8;

	if (bitcolor == 16) {
		drawmng_make16mask(&hdl->pal16mask, visual->blue_mask,
		    visual->red_mask, visual->green_mask);
	}

	hdl->dest.x = hdl->dest.x = 0;
	hdl->src.left = hdl->src.top = 0;
	hdl->src.right = width;
	hdl->src.bottom = height;
	hdl->lpitch = hdl->src.right * bytes_per_pixel;
	if (hdl->lpitch % 4) {
		hdl->src.right += (hdl->lpitch % 4) / bytes_per_pixel;
		hdl->lpitch = hdl->src.right * bytes_per_pixel;
	}

	/* image */
	hdl->surface = gdk_image_new(GDK_IMAGE_FASTEST, visual,
	    hdl->src.right, hdl->src.bottom);
	if (hdl->surface == NULL)
		goto destroy;

	hdl->vram.width = hdl->src.right;
	hdl->vram.height = hdl->src.bottom;
	hdl->vram.xalign = bytes_per_pixel;
	hdl->vram.yalign = hdl->lpitch;
	hdl->vram.bpp = bitcolor;

	/* pixmap */
	hdl->shared_pixmap = use_shared_pixmap;
	hdl->backsurf = NULL;
	if (use_shared_pixmap) {
		hdl->backsurf = gdk_pixmap_shpix_new(parent_window->window,
		    hdl->surface, hdl->vram.width, hdl->vram.height,
		    visual->depth);
	}
	if (hdl->backsurf == NULL) {
		hdl->shared_pixmap = NULL;
		hdl->backsurf = gdk_pixmap_new(parent_window->window,
		    hdl->vram.width, hdl->vram.height, visual->depth);
	}
	if (hdl->backsurf == NULL)
		goto destroy;

	return hdl;

destroy:
	if (hdl) {
		gtkdrawmng_release(hdl);
		if (hdl->drawarea) {
			gtk_widget_unref(hdl->drawarea);
		}
	}
	return NULL;
}

void
gtkdrawmng_release(GTKDRAWMNG_HDL hdl)
{

	if (hdl) {
		if (hdl->backsurf) {
			gdk_pixmap_unref(hdl->backsurf);
		}
		if (hdl->surface) {
			gdk_image_destroy(hdl->surface);
		}
		_MFREE(hdl);
	}
}

CMNVRAM *
gtkdrawmng_surflock(GTKDRAWMNG_HDL hdl)
{

	hdl->vram.ptr = (BYTE *)hdl->surface->mem;

	return &hdl->vram;
}

void
gtkdrawmng_surfunlock(GTKDRAWMNG_HDL hdl)
{
	GdkGC *gc =hdl->drawarea->style->fg_gc[GTK_WIDGET_STATE(hdl->drawarea)];

	if (!hdl->shared_pixmap) {
		gdk_draw_image(hdl->backsurf, gc, hdl->surface,
		    0, 0, 0, 0, hdl->width, hdl->height);
	}
}

void
gtkdrawmng_blt(GTKDRAWMNG_HDL hdl, RECT_T *sr, POINT_T *dp)
{
	GdkGC *gc =hdl->drawarea->style->fg_gc[GTK_WIDGET_STATE(hdl->drawarea)];

	if (sr || dp) {
		POINT_T p;
		RECT_T r;
		int width, height;

		if (sr) {
			r = *sr;
		} else {
			r.left = r.top = 0;
			r.right = hdl->width;
			r.bottom = hdl->height;
		}
		if (dp) {
			p = *dp;
		} else {
			p.x = p.y = 0;
		}
		width = r.right - p.x;
		height = r.bottom - p.y;

		gdk_draw_pixmap(hdl->drawarea->window, gc, hdl->backsurf,
		    r.left, r.top, p.x, p.y, width, height);
	} else {
		gdk_draw_pixmap(hdl->drawarea->window, gc, hdl->backsurf,
		    0, 0, 0, 0, hdl->width, hdl->height);
	}
}

int
gtkdrawmng_getbpp(GtkWidget *w, GtkWidget *parent_window)
{
	GdkVisual *visual;
	int bitcolor;

	visual = gtk_widget_get_visual(w);
	switch (visual->type) {
	case GDK_VISUAL_TRUE_COLOR:
	case GDK_VISUAL_PSEUDO_COLOR:
	case GDK_VISUAL_DIRECT_COLOR:
		if (visual->depth > 8) {
			break;
		}
		/* FALLTHROUGH */
	default:
		fprintf(stderr, "No support visual class.\n");
		return 0;
	}

	if (visual->depth == 32) {
		bitcolor = 32;
	} else if (visual->depth == 24) {
		if (is_32bpp(parent_window->window)) {
			bitcolor = 32;
		} else {
			bitcolor = 24;
		}
	} else if (visual->depth == 15 || visual->depth == 16) {
		bitcolor = 16;
	} else if (visual->depth == 8) {
		bitcolor = 8;
	} else if (visual->depth < 8) {
		fprintf(stderr, "Too few allocable color.\n");
		return 0;
	} else {
		fprintf(stderr, "No support depth.\n");
		return 0;
	}

	return bitcolor;
}


void
gtkdrawmng_set_size(GTKDRAWMNG_HDL hdl, int width, int height)
{

	hdl->width = width;
	hdl->height = height;
	gtk_widget_set_usize(hdl->drawarea, width, height);
}

void
drawmng_make16mask(PAL16MASK *pal16, UINT32 bmask, UINT32 rmask, UINT32 gmask)
{
	BYTE sft;

	if (pal16 == NULL)
		return;

	sft = 0;
	while ((!(bmask & 0x80)) && (sft < 32)) {
		bmask <<= 1;
		sft++;
	}
	pal16->mask.p.b = (BYTE)bmask;
	pal16->r16b = sft;

	sft = 0;
	while ((rmask & 0xffffff00) && (sft < 32)) {
		rmask >>= 1;
		sft++;
	}
	pal16->mask.p.r = (BYTE)rmask;
	pal16->l16r = sft;

	sft = 0;
	while ((gmask & 0xffffff00) && (sft < 32)) {
		gmask >>= 1;
		sft++;
	}
	pal16->mask.p.g = (BYTE)gmask;
	pal16->l16g = sft;
}

RGB16
drawmng_makepal16(PAL16MASK *pal16, RGB32 pal32)
{
	RGB32 pal;

	pal.d = pal32.d & pal16->mask.d;
	return (RGB16)((pal.p.g << pal16->l16g) + (pal.p.r << pal16->l16r) + (pal.p.b >> pal16->r16b));
}

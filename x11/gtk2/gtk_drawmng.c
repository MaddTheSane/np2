/*	$Id: gtk_drawmng.c,v 1.1 2004/07/14 16:01:40 monaka Exp $	*/

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

#include "gtk2/xnp2.h"
#include "gtk2/gtk_drawmng.h"


DRAWMNG_HDL
drawmng_create(void *parent, int width, int height)
{
	GTKDRAWMNG_HDL hdl = NULL;
	GtkWidget *parent_window;
	GdkVisual *visual;
	int bitcolor;
	int bytes_per_pixel;

	if (parent == NULL)
		return NULL;
	parent_window = GTK_WIDGET(parent);

	hdl = (GTKDRAWMNG_HDL)_MALLOC(sizeof(_GTKDRAWMNG_HDL), "drawmng hdl");
	if (hdl == NULL)
		return NULL;
	memset(hdl, 0, sizeof(_GTKDRAWMNG_HDL));

	hdl->d.width = width;
	hdl->d.height = height;
	hdl->d.drawing = FALSE;

	hdl->drawarea = gtk_drawing_area_new();
	gtk_widget_set_size_request(GTK_WIDGET(hdl->drawarea), width, height);

	visual = gtk_widget_get_visual(hdl->drawarea);
	bitcolor = gtkdrawmng_getbpp(hdl->drawarea, parent_window);
	if (bitcolor == 0)
		goto destroy;
	bytes_per_pixel = bitcolor / 8;

	if (bitcolor == 16) {
		drawmng_make16mask(&hdl->d.pal16mask, visual->blue_mask,
		    visual->red_mask, visual->green_mask);
	}

	hdl->d.dest.x = hdl->d.dest.x = 0;
	hdl->d.src.left = hdl->d.src.top = 0;
	hdl->d.src.right = width;
	hdl->d.src.bottom = height;
	hdl->d.lpitch = hdl->d.src.right * bytes_per_pixel;
	if (hdl->d.lpitch % 4) {
		hdl->d.src.right += (hdl->d.lpitch % 4) / bytes_per_pixel;
		hdl->d.lpitch = hdl->d.src.right * bytes_per_pixel;
	}

	/* image */
	hdl->surface = gdk_image_new(GDK_IMAGE_FASTEST, visual,
	    hdl->d.src.right, hdl->d.src.bottom);
	if (hdl->surface == NULL)
		goto destroy;

	hdl->d.vram.width = hdl->d.src.right;
	hdl->d.vram.height = hdl->d.src.bottom;
	hdl->d.vram.xalign = bytes_per_pixel;
	hdl->d.vram.yalign = hdl->d.lpitch;
	hdl->d.vram.bpp = bitcolor;

	/* pixmap */
	hdl->backsurf = gdk_pixmap_new(parent_window->window,
	    hdl->d.vram.width, hdl->d.vram.height, visual->depth);
	if (hdl->backsurf == NULL)
		goto destroy;

	return (DRAWMNG_HDL)hdl;

destroy:
	if (hdl) {
		drawmng_release((DRAWMNG_HDL)hdl);
		if (hdl->drawarea) {
			gtk_widget_unref(hdl->drawarea);
		}
	}
	return NULL;
}

void
drawmng_release(DRAWMNG_HDL dhdl)
{
	GTKDRAWMNG_HDL hdl = (GTKDRAWMNG_HDL)dhdl;

	if (hdl) {
		while (hdl->d.drawing)
			usleep(1);
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
drawmng_surflock(DRAWMNG_HDL dhdl)
{
	GTKDRAWMNG_HDL hdl = (GTKDRAWMNG_HDL)dhdl;

	if (hdl) {
		hdl->d.vram.ptr = (BYTE *)hdl->surface->mem;
		if (hdl->d.vram.ptr) {
			hdl->d.drawing = TRUE;
			return &hdl->d.vram;
		}
	}
	return NULL;
}

void
drawmng_surfunlock(DRAWMNG_HDL dhdl)
{
	GTKDRAWMNG_HDL hdl = (GTKDRAWMNG_HDL)dhdl;
	GdkGC *gc;

	if (hdl) {
		gc = hdl->drawarea->style->fg_gc[GTK_WIDGET_STATE(hdl->drawarea)];
		gdk_draw_image(hdl->backsurf, gc, hdl->surface,
		    0, 0, 0, 0, hdl->d.width, hdl->d.height);
		hdl->d.drawing = FALSE;
	}
}

void
drawmng_blt(DRAWMNG_HDL dhdl, RECT_T *sr, POINT_T *dp)
{
	GTKDRAWMNG_HDL hdl = (GTKDRAWMNG_HDL)dhdl;
	RECT_T r;
	POINT_T p;
	GdkGC *gc;
	int width, height;

	if (hdl) {
		gc = hdl->drawarea->style->fg_gc[GTK_WIDGET_STATE(hdl->drawarea)];
		if (sr || dp) {

			if (sr) {
				r = *sr;
			} else {
				r.left = r.top = 0;
				r.right = hdl->d.width;
				r.bottom = hdl->d.height;
			}
			if (dp) {
				p = *dp;
			} else {
				p.x = p.y = 0;
			}
			width = r.right - p.x;
			height = r.bottom - p.y;

			gdk_draw_pixmap(hdl->drawarea->window, gc,
			    hdl->backsurf,
			    r.left, r.top, p.x, p.y, width, height);
		} else {
			gdk_draw_pixmap(hdl->drawarea->window, gc,
			    hdl->backsurf,
			    0, 0, 0, 0, hdl->d.width, hdl->d.height);
		}
	}
}

void
drawmng_set_size(DRAWMNG_HDL dhdl, int width, int height)
{
	GTKDRAWMNG_HDL hdl = (GTKDRAWMNG_HDL)dhdl;

	hdl->d.width = width;
	hdl->d.height = height;
	gtk_widget_set_size_request(hdl->drawarea, width, height);
}

void
drawmng_invalidate(DRAWMNG_HDL dhdl, RECT_T *r)
{
	GTKDRAWMNG_HDL hdl = (GTKDRAWMNG_HDL)dhdl;
	gint x, y, w, h;

	if (r == NULL) {
		gtk_widget_queue_draw(hdl->drawarea);
	} else {
		x = r->left;
		y = r->top;
		w = r->right - r->left;
		h = r->bottom - r->top;
		gtk_widget_queue_draw_area(hdl->drawarea, x, y, w, h);
	}
}

void *
drawmng_get_widget_handle(DRAWMNG_HDL dhdl)
{
	GTKDRAWMNG_HDL hdl = (GTKDRAWMNG_HDL)dhdl;

	return hdl->drawarea;
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
		if (visual->depth >= 8) {
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

/*
 * Copyright (c) 2002-2003 NONAKA Kimihiro
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <gdk/gdkprivate.h>


void
gdk_window_set_pointer(GdkWindow *w, gint x, gint y)
{ 
	GdkWindowPrivate *private;

	if (w == NULL)
		w = (GdkWindow *)&gdk_root_parent;
	private = (GdkWindowPrivate *)w;
	if (!private->destroyed) {
		XWarpPointer(private->xdisplay, None, private->xwindow,
		    0, 0, 0, 0, x, y);
	}
}

void
gtk_scale_set_default_values(GtkScale *scale)
{

	gtk_range_set_update_policy(GTK_RANGE(scale), GTK_UPDATE_CONTINUOUS);
	gtk_scale_set_digits(scale, 1);
	gtk_scale_set_value_pos(scale, GTK_POS_RIGHT);
	gtk_scale_set_draw_value(scale, TRUE);
}

int
is_32bpp(GdkWindow *w)
{
	GdkWindowPrivate *private;
	XPixmapFormatValues *format;
	int nbit = 0;
	int count;
	int i;

	if (w == NULL)
		w = (GdkWindow *)&gdk_root_parent;
	private = (GdkWindowPrivate *)w;
	if (private->destroyed)
		return 0;

	format = XListPixmapFormats(private->xdisplay, &count);
	if (format != 0) {
		for (i = 0; i < count; i++) {
			if (format[i].depth != 24)
				continue;

			if (format[i].bits_per_pixel == 32) {
				nbit = 32;
			} else {
				nbit = 24;
			}
			break;
		}
		XFree(format);

		if (i == count) {
			fprintf(stderr, "24bpp depth not support?\n");
			return 0;
		}
	} else {
		fprintf(stderr, "Can't get PixmapFormats.\n");
		return 0;
	}

	return (nbit == 32) ? 1 : 0;
}

#if defined(MITSHM)

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

/* 
 * Desc: query the server for support for the MIT_SHM extension
 * Return:  0 = not available
 *          1 = shared XImage support available
 *          2 = shared Pixmap support available also
 */
static int
gdk_image_check_xshm(Display *display)
{
	int major, minor, ignore;
	Bool pixmaps;

	if (XQueryExtension(display, "MIT-SHM", &ignore, &ignore, &ignore)) {
		if (XShmQueryVersion(display, &major, &minor, &pixmaps) == True)
			return (pixmaps == True) ? 2 : 1;
	}
	return 0;
}

GdkPixmap *
gdk_pixmap_shpix_new(GdkWindow *w, GdkImage *image, gint width, gint height, gint depth)
{
	GdkPixmap *pixmap;
	GdkImagePrivate *private_image;
	GdkWindowPrivate *private;
	GdkWindowPrivate *window_private;

	g_return_val_if_fail((w != NULL) || (depth != -1), NULL);
	g_return_val_if_fail((width != 0) && (height != 0), NULL);

	if (w == NULL)
		w = (GdkWindow *)&gdk_root_parent;
	window_private = (GdkWindowPrivate *)w;
	if (window_private->destroyed)
		return NULL;

	if (depth == -1)
		depth = gdk_window_get_visual(w)->depth;

	if (gdk_image_check_xshm(window_private->xdisplay) != 2)
		return NULL;

	private = g_new0(GdkWindowPrivate, 1);
	pixmap = (GdkPixmap *)private;

	private->xdisplay = window_private->xdisplay;
	private->window_type = GDK_WINDOW_PIXMAP;

	private_image = (GdkImagePrivate *)image;
	private->xwindow = XShmCreatePixmap(private->xdisplay,
	    window_private->xwindow, private_image->ximage->data,
	    private_image->x_shm_info, width, height, depth);

	private->colormap = NULL;
	private->parent = NULL;
	private->x = 0;
	private->y = 0;
	private->width = width;
	private->height = height;
	private->resize_count = 0;
	private->ref_count = 1;
	private->destroyed = 0;

	gdk_xid_table_insert(&private->xwindow, pixmap);

	return pixmap;
}

#else	/* !MITSHM */

GdkPixmap *
gdk_pixmap_shpix_new(GdkWindow *w, GdkImage *image, gint width, gint height, gint depth)
{

	return NULL;
}

#endif	/* MITSHM */

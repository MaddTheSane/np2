/*	$Id: gtk_wrapper.c,v 1.5 2004/07/27 17:07:49 monaka Exp $	*/

/*
 * Copyright (c) 2002-2004 NONAKA Kimihiro
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

#include "gtk/xnp2.h"

#if (GTK_MAJOR_VERSION == 1)
#include <gdk/gdkprivate.h>
#elif (GTK_MAJOR_VERSION == 2)
#include <gdk/gdkx.h>
#else
#error	GTK+ version error
#endif


void
gtk_scale_set_default_values(GtkScale *scale)
{

	g_return_if_fail(scale != NULL);

	gtk_range_set_update_policy(GTK_RANGE(scale), GTK_UPDATE_CONTINUOUS);
	gtk_scale_set_digits(scale, 1);
	gtk_scale_set_value_pos(scale, GTK_POS_RIGHT);
	gtk_scale_set_draw_value(scale, TRUE);
}

void
gdk_window_set_pointer(GdkWindow *w, gint x, gint y)
{ 
	Display *xdisplay;
	Window xwindow;

	g_return_if_fail(w != NULL);

#if (GTK_MAJOR_VERSION == 1)
	xdisplay = ((GdkWindowPrivate *)w)->xdisplay;
	xwindow = ((GdkWindowPrivate *)w)->xwindow;
#elif (GTK_MAJOR_VERSION == 2)
	xdisplay = GDK_WINDOW_XDISPLAY(w);
	xwindow = GDK_WINDOW_XWINDOW(w);
#endif
	XWarpPointer(xdisplay, None, xwindow, 0, 0, 0, 0, x, y);
}

BOOL
gdk_window_get_pixmap_format(GdkWindow *w, GdkVisual *visual, pixmap_format_t *fmtp)
{
	Display *xdisplay;
	XPixmapFormatValues *format;
	int count;
	int i;

	g_return_val_if_fail(w != NULL, FALSE);
	g_return_val_if_fail(visual != NULL, FALSE);
	g_return_val_if_fail(fmtp != NULL, FALSE);

#if (GTK_MAJOR_VERSION == 1)
	xdisplay = ((GdkWindowPrivate *)w)->xdisplay;
#elif (GTK_MAJOR_VERSION == 2)
	xdisplay = GDK_WINDOW_XDISPLAY(w);
#endif
	format = XListPixmapFormats(xdisplay, &count);
	if (format) {
		for (i = 0; i < count; i++) {
			if (visual->depth == format[i].depth) {
				fmtp->depth = format[i].depth;
				fmtp->bits_per_pixel = format[i].bits_per_pixel;
				fmtp->scanline_pad = format[i].scanline_pad;
				XFree(format);
				return TRUE;
			}
		}
		XFree(format);
	}
	return FALSE;
}

#if defined(MITSHM) && (GTK_MAJOR_VERSION == 1)

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>

/* 
 * Description: query the server for support for the MIT_SHM extension
 * Return:      0: not available
 *              1: shared XImage support available
 *              2: shared Pixmap support available also
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

#else	/* !MITSHM || GTK_MAJOR_VERSION != 1 */

GdkPixmap *
gdk_pixmap_shpix_new(GdkWindow *w, GdkImage *image, gint width, gint height, gint depth)
{

	(void)w;
	(void)image;
	(void)width;
	(void)height;
	(void)depth;

	return NULL;
}

#endif	/* MITSHM && GTK_MAJOR_VERSION == 1 */

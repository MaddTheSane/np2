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

#include "gtk2/xnp2.h"

#include <gdk/gdkx.h>


void
gtk_scale_set_default_values(GtkScale *scale)
{

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

	xdisplay = GDK_WINDOW_XDISPLAY(w);
	xwindow = GDK_WINDOW_XWINDOW(w);
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

	xdisplay = GDK_WINDOW_XDISPLAY(w);
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

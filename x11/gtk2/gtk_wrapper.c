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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

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

	if (w) {
		xdisplay = GDK_WINDOW_XDISPLAY(w);
		xwindow = GDK_WINDOW_XWINDOW(w);
		XWarpPointer(xdisplay, None, xwindow, 0, 0, 0, 0, x, y);
	}
}

int
is_32bpp(GdkWindow *w)
{
	Display *xdisplay;
	XPixmapFormatValues *format;
	int nbit = 0;
	int count;
	int i;

	if (w == NULL)
		return 0;

	xdisplay = GDK_WINDOW_XDISPLAY(w);
	format = XListPixmapFormats(xdisplay, &count);
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

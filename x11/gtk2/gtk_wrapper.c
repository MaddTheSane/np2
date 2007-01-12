/*	$Id: gtk_wrapper.c,v 1.4 2007/01/12 19:09:58 monaka Exp $	*/

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

#include <string.h>

#include "gtk2/xnp2.h"

#include <gdk/gdkx.h>

#ifdef HAVE_XF86VIDMODE
#include <X11/extensions/xf86vmode.h>
#endif

extern int verbose;

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

gboolean
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

#ifdef HAVE_XF86VIDMODE
static XF86VidModeModeInfo **modes = NULL;
static int modeidx = -1;
static XF86VidModeModeInfo *saved_modes;
static XF86VidModeModeInfo orig_mode;
static int view_x, view_y;
static gint orig_x, orig_y;

static inline Bool
XF86VidModeGetModeInfo(Display *d, int s, XF86VidModeModeInfo *info)
{
	XF86VidModeModeLine *line;

	memset(info, 0, sizeof(*info));
	line = (void *)((char *)info + sizeof(info->dotclock));

	return XF86VidModeGetModeLine(d, s, (int *)&info->dotclock, line);
}
#endif

gboolean
gtk_window_init_fullscreen(GtkWidget *widget)
{
	gboolean ret = FALSE;
#ifdef HAVE_XF86VIDMODE
	GtkWindow *window;
	GdkWindow *w;
	Display *xdisplay;
	XF86VidModeModeInfo mode;
	int xscreen;
	int event_base, error_base;
	int major_ver, minor_ver;
	int nmodes;
	int i;
	Bool rv;

	g_return_val_if_fail(widget != NULL, FALSE);

	window = GTK_WINDOW(widget);
	w = widget->window;
	xdisplay = GDK_WINDOW_XDISPLAY(w);
	xscreen = XDefaultScreen(xdisplay);

	XLockDisplay(xdisplay);

	rv = XF86VidModeQueryExtension(xdisplay, &event_base, &error_base);
	if (!rv) {
		goto out;
	}

	rv = XF86VidModeQueryVersion(xdisplay, &major_ver, &minor_ver);
	if (!rv) {
		goto out;
	}
	if (verbose) {
		printf("XF86VidMode Extension: ver.%d.%d detected\n",
		    major_ver, minor_ver);
	}

	rv = XF86VidModeGetModeInfo(xdisplay, xscreen, &mode);
	if (rv) {
		if ((mode.hdisplay == 640) && (mode.vdisplay == 480)) {
			orig_mode = mode;
			saved_modes = &orig_mode;
			modes = &saved_modes;
			modeidx = 0;
			ret = TRUE;
			goto out;
		}
	}

	rv = XF86VidModeGetAllModeLines(xdisplay, xscreen, &nmodes, &modes);
	if (!rv) {
		goto out;
	}
	if (verbose) {
		printf("XF86VidMode Extension: %d modess\n", nmodes);
	}

	for (i = 0; i < nmodes; i++) {
		if (verbose) {
			printf("XF86VidMode Extension: %d: %dx%d\n", i,
			    modes[i]->hdisplay, modes[i]->vdisplay);
		}

		rv = XF86VidModeValidateModeLine(xdisplay, xscreen, modes[i]);
		if (rv) {
			if ((modes[i]->hdisplay == 640)
			 && (modes[i]->vdisplay == 480)) {
				rv = XF86VidModeGetModeInfo(xdisplay, xscreen,
				    &orig_mode);
				if (rv) {
					if (verbose) {
						printf("found\n");
					}
					modeidx = i;
					ret = TRUE;
					break;
				}
			}
		}
	}
	XFree(modes);

out:
	XUnlockDisplay(xdisplay);
#endif	/* HAVE_XF86VIDMODE */

	return ret;
}

void
gtk_window_fullscreen_mode(GtkWidget *widget)
{
#ifdef HAVE_XF86VIDMODE
	GtkWindow *window;
	GdkWindow *w;
	Display *xdisplay;
	int xscreen;

	g_return_if_fail(widget != NULL);

	if (modeidx < 0)
		return;

	window = GTK_WINDOW(widget);
	w = widget->window;
	xdisplay = GDK_WINDOW_XDISPLAY(w);
	xscreen = XDefaultScreen(xdisplay);

	XLockDisplay(xdisplay);

	XF86VidModeLockModeSwitch(xdisplay, xscreen, True);
	XF86VidModeGetViewPort(xdisplay, xscreen, &view_x, &view_y);
	gdk_window_get_origin(w, &orig_x, &orig_y);
	gtk_window_move(window, 0, 0);
	XF86VidModeSwitchToMode(xdisplay, xscreen, modes[modeidx]);

	XUnlockDisplay(xdisplay);
#endif	/* HAVE_XF86VIDMODE */
}

void
gtk_window_restore_mode(GtkWidget *widget)
{
#ifdef HAVE_XF86VIDMODE
	GtkWindow *window;
	GdkWindow *w;
	Display *xdisplay;
	int xscreen;
	XF86VidModeModeInfo mode;
	int rv;

	g_return_if_fail(widget != NULL);

	if ((orig_mode.hdisplay == 0) || (orig_mode.vdisplay == 0))
		return;

	window = GTK_WINDOW(widget);
	w = widget->window;
	xdisplay = GDK_WINDOW_XDISPLAY(w);
	xscreen = XDefaultScreen(xdisplay);

	XLockDisplay(xdisplay);

	rv = XF86VidModeGetModeInfo(xdisplay, xscreen, &mode);
	if (rv) {
		if ((orig_mode.hdisplay != mode.hdisplay)
		 || (orig_mode.vdisplay != mode.vdisplay)) {
			XF86VidModeSwitchToMode(xdisplay, xscreen, &orig_mode);
			XF86VidModeLockModeSwitch(xdisplay, xscreen, False);
		}
		if ((view_x != 0) || (view_y != 0)) {
			XF86VidModeSetViewPort(xdisplay,xscreen,view_x,view_y);
		}
	}

	gtk_window_move(window, orig_x, orig_y);

	XUnlockDisplay(xdisplay);
#endif	/* HAVE_XF86VIDMODE */
}

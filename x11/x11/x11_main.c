/*	$Id: x11_main.c,v 1.2 2004/07/15 16:10:03 monaka Exp $	*/

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
#include "pccore.h"
#include "scrndraw.h"
#include "timing.h"

#include "toolkit.h"

#include "mousemng.h"
#include "scrnmng.h"
#include "taskmng.h"

#include "x11/xnp2.h"
#include <X11/keysym.h>

#include "resources/np2.xbm"


#define	APP_NAME	"NP2"

#define	EVENT_MASK	\
			(Button1MotionMask	\
			 | Button2MotionMask	\
			 | Button3MotionMask	\
			 | PointerMotionMask	\
			 | KeyPressMask		\
			 | KeyReleaseMask	\
			 | ButtonPressMask	\
			 | ButtonReleaseMask	\
			 | EnterWindowMask	\
			 | LeaveWindowMask	\
			 | ExposureMask)

Display *display;
int screen;
Window window;
GC gc;
u_long black, white;

static Atom proto, delwin;
static char *icon_name = "np2";
static Pixmap icon_pixmap;

/*
 * Toolkit
 */
const char *
gui_x11_get_toolkit(void)
{

	return "x11";
}

BOOL
gui_x11_arginit(int *argcp, char ***argvp)
{

	UNUSED(argcp);
	UNUSED(argvp);

	setlocale(LC_ALL, "");

	return SUCCESS;
}

void
gui_x11_widget_create(void)
{
	XTextProperty iconName;
	XSizeHints size;
	Window root;

	display = XOpenDisplay(NULL);
	if (display == NULL) {
		fprintf(stderr, "Can't open display.\n");
		return;
	}
	root = DefaultRootWindow(display);
	screen = DefaultScreen(display);
	black = BlackPixel(display, screen);
	white = WhitePixel(display, screen);

	window = XCreateSimpleWindow(display, root,
	                             0, 0, 640, 400,
				     0, black, black);
	if (window == 0) {
		fprintf(stderr, "Can't create window.\n");
		XCloseDisplay(display);
		display = NULL;
		return;
	}
	XStoreName(display, window, np2oscfg.titles);

	gc = XCreateGC(display, root, 0, NULL);
	XSetForeground(display, gc, black);
	XSetBackground(display, gc, black);

	/* Set window size */
	size.flags = PMinSize | PMaxSize;
	size.min_width = 640;
	size.min_height = 400;
	size.max_width = 640;
	size.max_height = 400;
	XSetNormalHints(display, window, &size);

	/* Set icon name & bitmap */
	if (XStringListToTextProperty(&icon_name, 1, &iconName) != 0) {
		XWMHints wmhints;

		icon_pixmap = XCreateBitmapFromData(display, window,
				(char *)np2_bits, np2_width, np2_height);
		if (icon_pixmap != None) {
			wmhints.icon_pixmap = icon_pixmap;
			wmhints.flags = IconPixmapHint;
			XSetWMProperties(display, window, NULL, &iconName,
						0, 0, 0, &wmhints, NULL);
		}
	}

	proto = XInternAtom(display, "WM_PROTOCOLS", 0);
	delwin = XInternAtom(display, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(display, window, &delwin, 1);

	XSelectInput(display, window, EVENT_MASK);
}

static void
gui_x11_terminate(void)
{

	if (display) {
		XCloseDisplay(display);
	}
}

void
gui_x11_widget_show(void)
{

	if (display) {
		XMapRaised(display, window);
		XSync(display, False);
	}
}

void
gui_x11_widget_mainloop(void)
{

	if (display == NULL || window == 0)
		return;

	while (taskmng_isavail()) {
		if (XPending(display) > 0) {
			gui_x11_event_process();
		} else {
			mainloop(NULL);
		}
	}
}

void
gui_x11_widget_quit(void)
{

	taskmng_exit();
}

void
gui_x11_event_process(void)
{
	XEvent ev;
	KeySym ksym;

	if (!taskmng_isavail() || (XPending(display) <= 0))
		return;

	XNextEvent(display, &ev);
	switch (ev.type) {
	case Expose:
		if (ev.xexpose.count == 0) {
			scrndraw_redraw();
		}
		break;

	case KeyPress:
		ksym = XLookupKeysym(&ev.xkey, 0);
		if ((ksym == XK_F12) && (np2oscfg.F12KEY == 0)) {
			mouse_running(M_XOR);
		} else {
			x11kbd_keydown(ksym);
		}
		break;

	case KeyRelease:
		ksym = XLookupKeysym(&ev.xkey, 0);
		x11kbd_keyup(ksym);
		break;

	case ButtonPress:
		switch (ev.xbutton.button) {
		case Button1:
			mouse_btn(MOUSE_LEFTDOWN);
			break;

		case Button2:
			mouse_running(M_XOR);
			break;

		case Button3:
			mouse_btn(MOUSE_RIGHTDOWN);
			break;
		}
		break;

	case ButtonRelease:
		switch (ev.xbutton.button) {
		case Button1:
			mouse_btn(MOUSE_LEFTUP);
			break;

		case Button2:
			break;

		case Button3:
			mouse_btn(MOUSE_RIGHTUP);
			break;
		}
		break;

	case MotionNotify:
		break;

	case EnterNotify:
		break;

	case LeaveNotify:
		break;

	case ClientMessage:
		if ((ev.xclient.message_type == proto) &&
		    (ev.xclient.data.l[0] == delwin)) {
			taskmng_exit();
		}
		break;
	}
}

void
gui_x11_set_window_title(const char* str)
{

	XStoreName(display, window, str);
}

void
gui_x11_messagebox(const char *title, const char *msg)
{

	printf("%s\n", title);
	printf(msg);
}

/* toolkit data */
gui_toolkit_t x11_toolkit = {
	gui_x11_get_toolkit,
	gui_x11_arginit,
	gui_x11_terminate,
	gui_x11_widget_create,
	gui_x11_widget_show,
	gui_x11_widget_mainloop,
	gui_x11_widget_quit,
	gui_x11_event_process,
	gui_x11_set_window_title,
	gui_x11_messagebox,
};

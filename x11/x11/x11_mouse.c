/*	$Id: x11_mouse.c,v 1.1 2004/02/06 16:52:48 monaka Exp $	*/

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

#include "mousemng.h"

#include "x11/xnp2.h"

typedef struct {
	int	mouserunning;	// showing
	int	lastmouse;	// working
	short	mousex;
	short	mousey;
	BYTE	mouseb;

	BYTE	mouse_move_ratio;
	BYTE	mouse_move_mul;
	BYTE	mouse_move_div;

	Pixmap	cursor_pixmap;
	Cursor	cursor;
} mouse_stat_t;

static mouse_stat_t ms_default = {
	0, 0, 0, 0, 0xa0,
	MOUSE_RATIO_100, 1, 1,
	None,	/* cursor_pixmap */
	None	/* cursor */
};
static mouse_stat_t ms;

static void getmaincenter(int *, int *);
static void mouseonoff(int);


BOOL
mousemng_initialize(void)
{
	static char hide_cursor[16*16/8] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};
	Colormap cmap;
	XColor xc_black;

	ms = ms_default;

	cmap = XDefaultColormap(display, screen);
	ms.cursor_pixmap = XCreatePixmapFromBitmapData(display, window,
					hide_cursor, 16, 16, black, black, 1);
	if (ms.cursor_pixmap == None) {
		fprintf(stderr, "Can't create cursor pixmap.\n");
		return FAILURE;
	}
	XParseColor(display, cmap, "black", &xc_black);
	ms.cursor = XCreatePixmapCursor(display,
	                                ms.cursor_pixmap, ms.cursor_pixmap,
	                                &xc_black, &xc_black, 0, 0);
	if (ms.cursor == None) {
		fprintf(stderr, "Can't create cursor.\n");
		return FAILURE;
	}
	return SUCCESS;
}

void
mousemng_term(void)
{

	XUndefineCursor(display, window);
	if (ms.cursor_pixmap != None) {
		XFreePixmap(display, ms.cursor_pixmap);
	}
}

static void
getmaincenter(int *cx, int *cy)
{
	XWindowAttributes xw_attrib;

	XGetWindowAttributes(display, window, &xw_attrib);
	*cx = xw_attrib.x + xw_attrib.width / 2;
	*cy = xw_attrib.y + xw_attrib.height / 2;
}

static void
mouseonoff(int flag)
{
	int curx, cury;

	if ((ms.lastmouse ^ flag) & 1) {
		ms.lastmouse = flag & 1;
		if (ms.lastmouse) {
			XGrabPointer(display, window, True, None,
			    GrabModeAsync, GrabModeAsync, window, ms.cursor,
			    CurrentTime);
			getmaincenter(&curx, &cury);
			XWarpPointer(display, None, window, 0, 0, 0, 0,
			             curx, cury);
		} else {
			XUngrabPointer(display, CurrentTime);
		}
	}
}

BYTE
mouse_flag(void)
{

	return ms.mouserunning;
}

void
mouse_running(BYTE flg)
{
	BYTE mf = ms.mouserunning;

	switch (flg & 0xc0) {
	case M_RES:
		mf &= ~(1 << (flg & 7));
		break;

	case M_XOR:
		mf ^= (1 << (flg & 7));
		break;

	case M_SET:
		mf |= (1 << (flg & 7));
		break;
	}

	if ((mf ^ ms.mouserunning) & MOUSE_MASK) {
		ms.mouserunning = (mf & MOUSE_MASK);
		mouseonoff((ms.mouserunning == 1) ? 1 : 0);
	}
}

void
mousemng_callback(void)
{
	Window root_window, child;
	u_int mask;
	int rootx, rooty;
	int wx, wy;
	int cx, cy;

	if (ms.lastmouse & 1) {
		if (XQueryPointer(display, window, &root_window, &child,
		                  &rootx, &rooty, &wx, &wy, &mask)) {
			getmaincenter(&cx, &cy);
			ms.mousex += (short)((wx - cx) / 2);
			ms.mousey += (short)((wy - cy) / 2);
			XWarpPointer(display, None, window, 0, 0, 0, 0, cx, cy);
		}
	}
}

BYTE
mouse_btn(BYTE button)
{

	if ((ms.lastmouse & 1) == 0)
		return 0;

	switch (button) {
	case MOUSE_LEFTDOWN:
		ms.mouseb &= 0x7f;
		break;

	case MOUSE_LEFTUP:
		ms.mouseb |= 0x80;
		break;

	case MOUSE_RIGHTDOWN:
		ms.mouseb &= 0xdf;
		break;

	case MOUSE_RIGHTUP:
		ms.mouseb |= 0x20;
		break;
	}
	return 1;
}

BYTE
mousemng_getstat(short *x, short *y, int clear)
{

	if (ms.mouse_move_ratio == MOUSE_RATIO_100) {
		*x = ms.mousex;
		*y = ms.mousey;
	} else if (ms.mouse_move_div == 1) {
		*x = ms.mousex * ms.mouse_move_mul;
		*y = ms.mousey * ms.mouse_move_mul;
	} else {
		*x = (ms.mousex * ms.mouse_move_mul) / ms.mouse_move_div;
		*y = (ms.mousey * ms.mouse_move_mul) / ms.mouse_move_div;
	}
	if (clear) {
		ms.mousex = 0;
		ms.mousey = 0;
	}
	return ms.mouseb;
}

void
mousemng_set_ratio(BYTE new_ratio)
{

	np2oscfg.mouse_move_ratio = new_ratio;
	ms.mouse_move_ratio = np2oscfg.mouse_move_ratio;
	ms.mouse_move_mul = (ms.mouse_move_ratio >> 4) & 0xf;
	ms.mouse_move_div = ms.mouse_move_ratio & 0xf;
}

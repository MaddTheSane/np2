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

#include "np2.h"

#include "mousemng.h"

#include "gtk/xnp2.h"

static BYTE mouse_move_ratio = MOUSE_RATIO_100;
static BYTE mouse_move_mul;
static BYTE mouse_move_div;

static GdkPixmap *cursor_pixmap;
static GdkCursor *cursor;

static int mouserunning = 0;	// showing
static int lastmouse = 0;	// working
static short mousex = 0;
static short mousey = 0;
static BYTE mouseb = 0xa0;

static void getmaincenter(int *, int *);
static void mouseonoff(int);

int
mousemng_initialize(void)
{
	static gchar hide_cursor[16*16/8] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	cursor_pixmap = gdk_pixmap_create_from_data(
	    drawarea->window, hide_cursor, 16, 16, 1,
	    &drawarea->style->black, &drawarea->style->black);
	cursor = gdk_cursor_new_from_pixmap(cursor_pixmap, cursor_pixmap,
	    &drawarea->style->black, &drawarea->style->black, 0, 0);

	return SUCCESS;
}

void
mousemng_term(void)
{

	if (cursor_pixmap)
		gdk_pixmap_unref(cursor_pixmap);
}

static void
getmaincenter(int *cx, int *cy)
{

	*cx = drawarea->allocation.x + drawarea->allocation.width / 2;
	*cy = drawarea->allocation.y + drawarea->allocation.height / 2;
}

static void
mouseonoff(int flag)
{
	int curx, cury;

	if ((lastmouse ^ flag) & 1) {
		lastmouse = flag & 1;
		if (lastmouse) {
			gdk_pointer_grab(drawarea->window, TRUE, 0,
			    drawarea->window, cursor, 0);
			getmaincenter(&curx, &cury);
			gdk_window_set_pointer(drawarea->window, curx, cury);
		} else {
			gdk_pointer_ungrab(0);
		}
	}
}

BYTE
mouse_flag(void)
{

	return mouserunning;
}

void
mouse_running(BYTE flg)
{
	BYTE mf = mouserunning;

	switch (flg & 0xc0) {
	case 0x00:
		mf &= ~(1 << (flg & 7));
		break;

	case 0x40:
		mf ^= (1 << (flg & 7));
		break;

	default:
		mf |= (1 << (flg & 7));
		break;
	}

	if ((mf ^ mouserunning) & MOUSE_MASK) {
		mouserunning = (mf & MOUSE_MASK);
		mouseonoff((mouserunning == 1) ? 1 : 0);
	}
}

void
mousemng_callback(void)
{
	int wx, wy;
	int cx, cy;

	if (lastmouse & 1) {
		gdk_window_get_pointer(drawarea->window, &wx, &wy, NULL);
		getmaincenter(&cx, &cy);
		mousex += (short)((wx - cx) / 2);
		mousey += (short)((wy - cy) / 2);
		gdk_window_set_pointer(drawarea->window, cx, cy);
	}
}

BYTE
mouse_btn(BYTE button)
{

	if ((lastmouse & 1) == 0)
		return 0;

	switch (button) {
	case MOUSE_LEFTDOWN:
		mouseb &= 0x7f;
		break;

	case MOUSE_LEFTUP:
		mouseb |= 0x80;
		break;

	case MOUSE_RIGHTDOWN:
		mouseb &= 0xdf;
		break;

	case MOUSE_RIGHTUP:
		mouseb |= 0x20;
		break;
	}
	return 1;
}

BYTE
mousemng_getstat(short *x, short *y, int clear)
{

	if (mouse_move_ratio == MOUSE_RATIO_100) {
		*x = mousex;
		*y = mousey;
	} else if (mouse_move_div == 1) {
		*x = mousex * mouse_move_mul;
		*y = mousey * mouse_move_mul;
	} else {
		*x = (mousex * mouse_move_mul) / mouse_move_div;
		*y = (mousey * mouse_move_mul) / mouse_move_div;
	}
	if (clear) {
		mousex = 0;
		mousey = 0;
	}
	return mouseb;
}

void
mousemng_set_ratio(BYTE new_ratio)
{

	np2oscfg.mouse_move_ratio = new_ratio;
	mouse_move_ratio = np2oscfg.mouse_move_ratio;
	mouse_move_mul = (mouse_move_ratio >> 4) & 0xf;
	mouse_move_div = mouse_move_ratio & 0xf;
}

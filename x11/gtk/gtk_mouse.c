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


typedef struct {
	int mouserunning;	// showing
	int lastmouse;		// working
	short mousex;
	short mousey;
	BYTE mouseb;

	BYTE mouse_move_ratio;
	BYTE mouse_move_mul;
	BYTE mouse_move_div;

	GdkPixmap *cursor_pixmap;
	GdkCursor *cursor;
} mouse_stat_t;

static mouse_stat_t ms_default = {
	0, 0, 0, 0, 0xa0,
	MOUSE_RATIO_100, 1, 1,
	NULL, NULL,
};
static mouse_stat_t ms;

static void getmaincenter(GtkWidget *w, int *cx, int *cy);
static void mouseonoff(int onoff);


int
mousemng_initialize(void)
{
	static gchar hide_cursor[16*16/8] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	ms = ms_default;

	ms.cursor_pixmap = gdk_pixmap_create_from_data(drawarea->window,
	    hide_cursor, 16, 16, 1,
	    &drawarea->style->black, &drawarea->style->black);
	ms.cursor = gdk_cursor_new_from_pixmap(ms.cursor_pixmap,
	    ms.cursor_pixmap, &drawarea->style->black, &drawarea->style->black,
	    0, 0);

	return SUCCESS;
}

void
mousemng_term(void)
{

	if (ms.cursor_pixmap) {
		gdk_pixmap_unref(ms.cursor_pixmap);
		ms.cursor_pixmap = NULL;
	}
}

static void
getmaincenter(GtkWidget *w, int *cx, int *cy)
{

	*cx = w->allocation.x + w->allocation.width / 2;
	*cy = w->allocation.y + w->allocation.height / 2;
}

static void
mouseonoff(int flag)
{
	int curx, cury;

	if ((ms.lastmouse ^ flag) & 1) {
		ms.lastmouse = flag & 1;
		if (ms.lastmouse) {
			gdk_pointer_grab(drawarea->window, TRUE, 0,
			    drawarea->window, ms.cursor, 0);
			getmaincenter(drawarea, &curx, &cury);
			gdk_window_set_pointer(drawarea->window, curx, cury);
		} else {
			gdk_pointer_ungrab(0);
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

	if ((mf ^ ms.mouserunning) & MOUSE_MASK) {
		ms.mouserunning = (mf & MOUSE_MASK);
		mouseonoff((ms.mouserunning == 1) ? 1 : 0);
	}
}

void
mousemng_callback(void)
{
	int wx, wy;
	int cx, cy;

	if (ms.lastmouse & 1) {
		gdk_window_get_pointer(drawarea->window, &wx, &wy, NULL);
		getmaincenter(drawarea, &cx, &cy);
		ms.mousex += (short)((wx - cx) / 2);
		ms.mousey += (short)((wy - cy) / 2);
		gdk_window_set_pointer(drawarea->window, cx, cy);
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

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

#include "mousemng.h"

#include "qt/xnp2.h"

#include <qcursor.h>

static QWidget *pEmulationScreen = 0;
static QCursor blankCursor(Qt::BlankCursor);

static BYTE mouse_move_ratio = MOUSE_RATIO_100;
static BYTE mouse_move_mul;
static BYTE mouse_move_div;

static int mouserunning = 0;	// showing
static int lastmouse = 0;	// working
static short mousex = 0;
static short mousey = 0;
static BYTE mouseb = 0xa0;


//
// Event
//
void
emulationWindow::mousePressEvent(QMouseEvent *ev)
{

	switch (ev->button() & MouseButtonMask) {
	case LeftButton:
		mouse_btn(MOUSE_LEFTDOWN);
		break;

	case MidButton:
		mouse_running(M_XOR);
		break;

	case RightButton:
		mouse_btn(MOUSE_RIGHTDOWN);
		break;

	case NoButton:
	case MouseButtonMask:
	case ShiftButton:
	case ControlButton:
	case MetaButton:
	case KeyButtonMask:
	case Keypad:
	default:
		break;
	}
}

void
emulationWindow::mouseReleaseEvent(QMouseEvent *ev)
{

	switch (ev->button() & MouseButtonMask) {
	case LeftButton:
		mouse_btn(MOUSE_LEFTUP);
		break;

	case MidButton:
		break;

	case RightButton:
		mouse_btn(MOUSE_RIGHTUP);
		break;

	case NoButton:
	case MouseButtonMask:
	case ShiftButton:
	case ControlButton:
	case MetaButton:
	case KeyButtonMask:
	case Keypad:
	default:
		break;
	}
}


//
// mousemng
//
int
mousemng_initialize(void)
{

	pEmulationScreen = (QWidget *)np2EmulationWindow->getEmulationScreen();

	return SUCCESS;
}

void
mousemng_term(void)
{
}

static void
getmaincenter(int *cx, int *cy)
{

	*cx = pEmulationScreen->width() / 2;
	*cy = pEmulationScreen->height() / 2;
}

static void
mouseonoff(int flag)
{
	int curx, cury;

	if ((lastmouse ^ flag) & 1) {
		lastmouse = flag & 1;
		if (lastmouse) {
			pEmulationScreen->grabMouse(blankCursor);
			getmaincenter(&curx, &cury);
			qt_setPointer(pEmulationScreen, curx, cury);
		} else {
			pEmulationScreen->releaseMouse();
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
		qt_getPointer(pEmulationScreen, &wx, &wy);
		getmaincenter(&cx, &cy);
		mousex += (short)((wx - cx) / 2);
		mousey += (short)((wy - cy) / 2);
		qt_setPointer(pEmulationScreen, cx, cy);
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

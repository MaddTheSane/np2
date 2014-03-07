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
#include "qt/xnp2.h"

#if defined(Q_WS_X11)
#include <X11/Xlib.h>
#include <qcursor.h>

const ScreenInfo
qt_getScreenInfo()
{
	ScreenInfo info;
	Visual *visual;
	Display *display;
	int depth;
	int bitcolor;

	visual = (Visual *)QPaintDevice::x11AppVisual();
	display = QPaintDevice::x11AppDisplay();
	depth = QPaintDevice::x11AppDepth();

	info.bpp = 0;

	switch (visual->c_class) {
	case TrueColor:
	case PseudoColor:
	case DirectColor:
		if (depth >= 8) {
			break;
		}
		/*FALLTHROUGH*/
	default:
		return info;
	}

	bitcolor = 0;
	if (depth == 32) {
		bitcolor = 32;
	} else if (depth == 24) {
		XPixmapFormatValues *format;
		int count;
		int i;

		format = ::XListPixmapFormats(display, &count);
		if (format) {
			for (i = 0; i < count; i++) {
				if (format[i].depth == 24)
					continue;

				if (format[i].bits_per_pixel == 32)
					bitcolor = 32;
				else
					bitcolor = 24;
				break;
			}
			::XFree(format);
		}
	} else if (depth == 15 || depth == 16) {
		bitcolor = 16;
	} else if (depth == 8) {
		bitcolor = 8;
	}

	info.bpp = bitcolor;
	info.mask.red = visual->red_mask;
	info.mask.green = visual->green_mask;
	info.mask.blue = visual->blue_mask;

	return info;
}

bool
hasPendingEvents()
{

	return XPending(QPaintDevice::x11AppDisplay());
}
#endif	/* Q_WS_X11 */

#if defined(Q_WS_QWS)
#include <qpixmap.h>

const ScreenInfo
qt_getScreenInfo()
{
	ScreenInfo info;

	info.bpp = 0;

	// 16bpp only...
	if (QPixmap::defaultDepth() == 16) {
		info.bpp = 16;
		info.mask.red = 0xf800;
		info.mask.green = 0x07e0;
		info.mask.blue = 0x001f;
	}

	return info;
}

bool
hasPendingEvents()
{

	return false;
}
#endif

void
qt_setPointer(QWidget *w, int x, int y)
{
	QPoint gp;

	gp = w->mapToGlobal(QPoint(x, y));
	QCursor::setPos(gp);
}

void
qt_getPointer(QWidget *w, int *x, int *y)
{
	QPoint gp;
	QPoint wp;

	gp = QCursor::pos();
	wp = w->mapFromGlobal(gp);
	*x = wp.x();
	*y = wp.y();
}

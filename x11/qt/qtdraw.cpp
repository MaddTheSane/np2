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
#include "palettes.h"
#include "scrndraw.h"

#include "scrnmng.h"

#include "qt/xnp2.h"
#include "qt/qtdraw.h"

static emulationScreen *pEmulationScreen = 0;

static SCRNSURF scrnsurf;
SCRNMNG scrnmng;


void
scrnmng_initialize(void)
{

	pEmulationScreen = np2EmulationWindow->getEmulationScreen();

	pEmulationScreen->setScreen(640, 400, 1);
}

BOOL
scrnmng_create(BYTE mode)
{

	return pEmulationScreen->createScreen(mode);
}

void
scrnmng_destroy(void)
{

	pEmulationScreen->destroyScreen();
}

RGB16
scrnmng_makepal16(RGB32 pal32)
{

	return pEmulationScreen->makePal16(pal32);
}

void
scrnmng_setwidth(int posx, int width)
{

	UNUSED(posx);

	pEmulationScreen->setScreenWidth(width);
}

void
scrnmng_setheight(int posy, int height)
{

	UNUSED(posy);

	pEmulationScreen->setScreenHeight(height);
}

void
scrnmng_setextend(int extend)
{

	pEmulationScreen->setScreenExtend(extend);
}

const SCRNSURF *
scrnmng_surflock(void)
{

	return pEmulationScreen->lockSurface();
}

void
scrnmng_surfunlock(const SCRNSURF *surf)
{

	pEmulationScreen->unlockSurface(surf);
}


//
// Event
//
void
emulationScreen::paintEvent(QPaintEvent *ev)
{

	UNUSED(ev);

	if (m_Painter == 0) {
		m_Painter = new QPainter(this);
		scrndraw_redraw();
		DELETE(m_Painter);
	}
}

void
emulationScreen::make16mask(UINT32 bmask, UINT32 rmask, UINT32 gmask)
{
	BYTE sft;

	sft = 0;
	while ((!(bmask & 0x80)) && (sft < 32)) {
		bmask <<= 1;
		sft++;
	}
	m_pal16mask.p.b = (BYTE)bmask;
	m_r16b = sft;

	sft = 0;
	while ((rmask & 0xffffff00) && (sft < 32)) {
		rmask >>= 1;
		sft++;
	}
	m_pal16mask.p.r = (BYTE)rmask;
	m_l16r = sft;

	sft = 0;
	while ((gmask & 0xffffff00) && (sft < 32)) {
		gmask >>= 1;
		sft++;
	}
	m_pal16mask.p.g = (BYTE)gmask;
	m_l16g = sft;
}

BOOL
emulationScreen::createScreen(BYTE mode)
{
	ScreenInfo inf;
	QSize size;
	int height;

#if notyet
	if (mode & SCRNMODE_FULLSCREEN) {
	} else
#endif
	{
		scrnmng.flag = SCRNFLAG_HAVEEXTEND;

		inf = qt_getScreenInfo();
		if (inf.bpp == 0) {
			return FAILURE;
		}

		if (!(mode & SCRNMODE_ROTATE)) {
			size.setWidth(641);
			size.setHeight(480);
		} else {
			size.setWidth(480);
			size.setHeight(641);
		}
		height = 480;

		/* create surface image */
		m_Surface = new QImage();
		if (m_Surface == NULL) {
			goto failure;
		}
		if (!m_Surface->create(size, inf.bpp)) {
			if (((inf.bpp == 16) || (inf.bpp == 24))
			  && m_Surface->create(size, 32)) {
				inf.bpp = 32;
			}
		}
		if (m_Surface->bits() == 0) {
			goto failure;
		}

#if !defined(Q_WS_QWS)
		/* create off-screen pixmap */
		m_Offscreen = new QPixmap(size);
		if (m_Offscreen == NULL) {
			goto failure;
		}
		m_Offscreen->fill(Qt::black);
#endif
		m_DefaultExtend = 1;

		if (inf.bpp == 16) {
			make16mask(inf.mask.red, inf.mask.green, inf.mask.blue);
		} else if (inf.bpp == 8) {
		}
	}
	scrnmng.bpp = (BYTE)inf.bpp;
	scrnsurf.bpp = inf.bpp;
	m_bytesPerPixel = inf.bpp / 8;
	m_ScreenMode = mode;
	m_DefaultSize.setWidth(640);
	m_DefaultSize.setHeight(height);
	renewalWindowSize();
	return SUCCESS;

failure:
	destroyScreen();
	return FAILURE;
}

void
emulationScreen::destroyScreen()
{

	DELETE(m_Offscreen);
	DELETE(m_Surface);
}

void
emulationScreen::setWindowSize(int width, int height)
{

	width += np2oscfg.paddingx * 2;
	height += np2oscfg.paddingy * 2;
	setFixedSize(width, height);
	setMinimumSize(width, height);
	setMaximumSize(width, height);
}

void
emulationScreen::renewalWindowSize()
{
	int width;
	int height;
	int extend;
	int scrnwidth;
	int scrnheight;

	width = QMIN(m_ScreenSize.width(), m_DefaultSize.width());
	height = QMIN(m_ScreenSize.height(), m_DefaultSize.height());
	extend = 0;

#if notyet
	if (m_ScreenMode & SCRNMODE_FULLSCREEN) {
	} else
#endif
	{
		if (!(m_ScreenMode & SCRNMODE_ROTATE)) {
			if (np2oscfg.paddingx) {
				extend = QMIN(m_ScreenExtend, m_DefaultExtend);
			}
			scrnwidth = width;
			scrnheight = height;
			m_DestPoint.setX(np2oscfg.paddingx - extend);
			m_DestPoint.setY(np2oscfg.paddingy);
			m_SrcRect.setRight(width + extend);
			m_SrcRect.setBottom(height);
		} else {
			if (np2oscfg.paddingy) {
				extend = QMIN(m_ScreenExtend, m_DefaultExtend);
			}
			scrnwidth = height;
			scrnheight = width;
			m_DestPoint.setX(np2oscfg.paddingx);
			m_DestPoint.setY(np2oscfg.paddingy - extend);
			m_SrcRect.setRight(height);
			m_SrcRect.setBottom(width + extend);
		}
		setWindowSize(scrnwidth, scrnheight);
	}
	scrnsurf.width = width;
	scrnsurf.height = height;
	scrnsurf.extend = extend;
}

void
emulationScreen::clearOutOfRect(const QRect& target, const QRect& base)
{
	QRect rect;

	if (m_Painter) {
		QBrush brush(Qt::black);

		rect = base;
		if (base.top() < target.top()) {
			rect.setBottom(target.top());
			m_Painter->fillRect(rect, brush);
		}
		if (target.bottom() < base.bottom()) {
			rect.setTop(target.bottom());
			rect.setBottom(base.bottom());
			m_Painter->fillRect(rect, brush);
		}

		rect.setTop(QMAX(base.top(), target.top()));
		rect.setBottom(QMIN(base.bottom(), target.bottom()));
		if (rect.top() < rect.bottom()) {
			if (base.left() < target.left()) {
				rect.setLeft(base.left());
				rect.setRight(target.left());
				m_Painter->fillRect(rect, brush);
			}
			if (target.right() < base.right()) {
				rect.setLeft(target.right());
				rect.setRight(base.right());
				m_Painter->fillRect(rect, brush);
			}
		}
	}
}

void
emulationScreen::clearOutOfScreen()
{
	QRect target;
	QRect base;

	base.setRect(0, 0, width(), height());
	target.setLeft(m_DestPoint.x() + m_SrcRect.left());
	target.setTop(m_DestPoint.y() + m_SrcRect.top());
	target.setRight(m_DestPoint.x() + m_SrcRect.right());
	target.setBottom(m_DestPoint.y() + m_SrcRect.bottom());
	clearOutOfRect(target, base);
}

const SCRNSURF *
emulationScreen::lockSurface()
{

	scrnsurf.ptr = m_Surface->bits();
	if (scrnsurf.ptr) {
		if (!(m_ScreenMode & SCRNMODE_ROTATE)) {
			scrnsurf.xalign = m_bytesPerPixel;
			scrnsurf.yalign = m_Surface->bytesPerLine();
		} else if (!(m_ScreenMode & SCRNMODE_ROTATEDIR)) {
			scrnsurf.ptr += (scrnsurf.width - 1) * m_Surface->bytesPerLine();
			scrnsurf.xalign = 0 - m_Surface->bytesPerLine();
			scrnsurf.yalign = m_bytesPerPixel;
		} else {
			scrnsurf.ptr += (scrnsurf.height - 1) * m_bytesPerPixel;
			scrnsurf.xalign = m_Surface->bytesPerLine();
			scrnsurf.yalign = 0 - m_bytesPerPixel;
		}
		return &scrnsurf;
	}
	return 0;
}

void
emulationScreen::unlockSurface(const SCRNSURF *surf)
{

	if (surf) {
		*m_Offscreen = *m_Surface;

		if (scrnmng.palchanged) {
			scrnmng.palchanged = FALSE;
		}

#if notyet
		if (m_ScreenMode & SCRNMODE_FULLSCREEN) {
		} else
#endif
		{
			if (scrnmng.allflash) {
				scrnmng.allflash = 0;
				if (np2oscfg.paddingx || np2oscfg.paddingy) {
					clearOutOfScreen();
				}
			}
			bitBlt(this, m_DestPoint, m_Offscreen, m_SrcRect, Qt::CopyROP);
		}
	}
}

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

#ifndef	NP2_QT_QTDRAW_H__
#define	NP2_QT_QTDRAW_H__

#include "compiler.h"

#include <qwidget.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qpainter.h>

#include "scrnmng.h"


//
// emulationScreen
//
class emulationScreen : public QWidget
{
	Q_OBJECT

protected:
	void paintEvent(QPaintEvent *ev);

private:
	void setWindowSize(int width, int height);
	void renewalWindowSize();
	void clearOutOfRect(const QRect& target, const QRect& base);
	void clearOutOfScreen();

	void make16mask(UINT32 bmask, UINT32 rmask, UINT32 gmask);

public:
	emulationScreen(QWidget *parent, const char *name, WFlags f = 0);
	~emulationScreen();

	BOOL createScreen(BYTE mode);
	void destroyScreen();

	RGB16 makePal16(RGB32 pal32) {
		RGB32 pal;

		pal.d = pal32.d & m_pal16mask.d;
		return (RGB16)((pal.p.g << m_l16g) + (pal.p.r << m_l16r) + (pal.p.b >> m_r16b));
	}

	void setScreenWidth(int width) {
		m_ScreenSize.setWidth(width);
		renewalWindowSize();
	}
	void setScreenHeight(int height) {
		m_ScreenSize.setHeight(height);
		renewalWindowSize();
	}
	void setScreenExtend(int extend) {
		m_ScreenExtend = extend;
		renewalWindowSize();
	}
	void setScreen(int width, int height, int extend) {
		m_ScreenSize.setWidth(width);
		m_ScreenSize.setHeight(height);
		m_ScreenExtend = extend;
		setWindowSize(width, height);
	}

	const SCRNSURF *lockSurface();
	void unlockSurface(const SCRNSURF *surf);

private:
	QImage		*m_Surface;
#if !defined(Q_WS_QWS)
	QPixmap		*m_Offscreen;
#endif
	int		m_bytesPerPixel;
	QPainter	*m_Painter;

	BYTE		m_ScreenMode;

	QRect		m_SrcRect;
	QPoint		m_DestPoint;

	QSize		m_ScreenSize;	/* scrnstat.width, height */
	int		m_ScreenExtend;	/* scrnstat.extend */

	QSize		m_DefaultSize;
	int		m_DefaultExtend;

	RGB32		m_pal16mask;
	BYTE		m_r16b;
	BYTE		m_l16r;
	BYTE		m_l16g;
};

#endif	/* NP2_QT_QTDRAW_H__ */

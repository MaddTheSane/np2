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

#ifndef	NP2_QT_XNP2_H__
#define	NP2_QT_XNP2_H__

#include "compiler.h"

#include <qapplication.h>
#include <qmainwindow.h>
#include <qtimer.h>

#define	DELETE(v)	if (v) { delete v; v = 0; }

//
// emlationWindow
//
class emulationScreen;
class emulationWindow : public QMainWindow
{
	Q_OBJECT

protected:
	void closeEvent(QCloseEvent *);
	void mousePressEvent(QMouseEvent *ev);
	void mouseReleaseEvent(QMouseEvent *ev);
	void keyPressEvent(QKeyEvent *ev);
	void keyReleaseEvent(QKeyEvent *ev);

private slots:
	void mainLoop();	// main loop
	void reset();
	void about();

	// user function
private:
	void createMenu();

public:
	emulationWindow();
	~emulationWindow();

	void startTimer();
	void stopTimer();

	emulationScreen *getEmulationScreen() { return m_emulationScreen; }

	// variable
private:
	emulationScreen *m_emulationScreen;
	QTimer          *m_mainLoopTimer;
};


//
// wrapper
//
struct ScreenInfo {
	int bpp;

	struct {
		UINT32	red;
		UINT32	green;
		UINT32	blue;
	} mask;
};

#ifdef __cplusplus
extern "C" {
#endif

const ScreenInfo qt_getScreenInfo();
void qt_setPointer(QWidget *w, int x, int y);
void qt_getPointer(QWidget *w, int *x, int *y);

extern emulationWindow *np2EmulationWindow;

#ifdef __cplusplus
}
#endif

#endif	/* NP2_QT_XNP2_H__ */

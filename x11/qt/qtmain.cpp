#include "compiler.h"

#include "np2.h"
#include "pccore.h"
#include "iocore.h"
#include "toolkit.h"

#include "np2ver.h"
#include "../resources/np2.xpm"

#include "qt/xnp2.h"
#include "qt/qtdraw.h"

#if QT_VERSION >= 300
#include <qeventloop.h>
#endif
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qpopupmenu.h>
#include <qtranslator.h>
#include <qtextcodec.h>

static QApplication *np2App = 0;
emulationWindow *np2EmulationWindow = 0;


//
// Toolkit
//
const char *
gui_qt_get_toolkit(void)
{

	return "qt";
}

BOOL
gui_qt_arginit(int *argcp, char ***argvp)
{

	// new application
	np2App = new QApplication(*argcp, *argvp);
	if (np2App == 0)
		goto failure;

	// transtlator
	QTranslator *translator;
	translator = new QTranslator(0);
	if (translator == 0)
		goto failure;
	translator->load(QString( "xnp2_" ) + QTextCodec::locale(), ".");
	np2App->installTranslator(translator);

	np2App->connect(np2App,SIGNAL(lastWindowClosed()),np2App,SLOT(quit()));

	return TRUE;

failure:
	DELETE(np2App);
	return FALSE;
}

void
gui_qt_terminate(void)
{

	DELETE(np2App);
}

void
gui_qt_widget_create(void)
{

	np2EmulationWindow = new emulationWindow();
	np2App->setMainWidget(np2EmulationWindow);
}

void
gui_qt_widget_show(void)
{

	np2EmulationWindow->show();
}

void
gui_qt_widget_mainloop(void)
{

	np2EmulationWindow->startTimer();
	np2App->exec();
}

void
gui_qt_widget_quit(void)
{

	if (np2App) {
		np2EmulationWindow->stopTimer();
		np2App->quit();
	}
}

void
gui_qt_event_process(void)
{

	// XXX Nothing to do
}

void
gui_qt_set_window_title(const char* str)
{

	np2EmulationWindow->setCaption(str);
}

// toolkit data
gui_toolkit_t qt_toolkit = {
	gui_qt_get_toolkit,
	gui_qt_arginit,
	gui_qt_terminate,
	gui_qt_widget_create,
	gui_qt_widget_show,
	gui_qt_widget_mainloop,
	gui_qt_widget_quit,
	gui_qt_event_process,
	gui_qt_set_window_title,
};


//
// emulationWindow
//
emulationWindow::emulationWindow()
	: QMainWindow(0, "NP2 main window", WType_TopLevel|WDestructiveClose)
	, m_emulationScreen(0)
	, m_mainLoopTimer(0)
{
	// set title caption
	setCaption(np2oscfg.titles);

	// icon
	QPixmap icon(np2_icon);
	setIcon(icon);

	// menu
	createMenu();

	// emulation window
	m_emulationScreen = new emulationScreen(this, "NP2 emulation widget");
	setCentralWidget(m_emulationScreen);

	// main loop timer
	m_mainLoopTimer = new QTimer(this);
	connect(m_mainLoopTimer, SIGNAL(timeout()), this, SLOT(mainLoop()));
}

emulationWindow::~emulationWindow()
{

	if (m_mainLoopTimer) {
		m_mainLoopTimer->stop();
		delete m_mainLoopTimer;
		m_mainLoopTimer = 0;
	}
	DELETE(m_emulationScreen);
}

void
emulationWindow::startTimer()
{

	m_mainLoopTimer->start(0);
}

void
emulationWindow::stopTimer()
{

	m_mainLoopTimer->stop();
}

void
emulationWindow::mainLoop()
{

#if QT_VERSION >= 300
	if (!QApplication::eventLoop()->hasPendingEvents())
#else
	if (!hasPendingEvents())
#endif
	{
		mainloop(0);
	}
}

void
emulationWindow::createMenu()
{
	// "Emulate"
	QPopupMenu *emulate = new QPopupMenu(this);
	menuBar()->insertItem("&Emulate", emulate);
	emulate->insertItem("&Reset", this, SLOT(reset()));
	emulate->insertSeparator();
	emulate->insertItem("E&xit", qApp, SLOT(closeAllWindows()));

	// "Other"
	QPopupMenu *other = new QPopupMenu(this);
	menuBar()->insertItem("&Other", other);
	other->insertSeparator();
	other->insertItem("&About", this, SLOT(about()));
}

void
emulationWindow::closeEvent(QCloseEvent *ev)
{
	int rv;

	if (np2oscfg.confirm) {
		rv = QMessageBox::information(this, np2oscfg.titles,
		    tr("Are you sure?"), tr("Yes"), tr("No"), QString::null,
		    0, 1);
	} else {
		rv = 0;
	}
	if (rv == 0) {
		ev->accept();
	} else {
		ev->ignore();
	}
}

//
// menu item
//
void
emulationWindow::reset()
{
	int rv;

	if (np2oscfg.confirm) {
		rv = QMessageBox::information(this, np2oscfg.titles,
		    tr("Are you sure?"), tr("Yes"), tr("No"), QString::null,
		    0, 1);
	} else {
		rv = 0;
	}
	if (rv == 0) {
		pccore_cfgupdate();
		pccore_reset();
	}
}

void
emulationWindow::about()
{
	QString str = "Neko Project II\n";
	str += NP2VER_CORE;
#if defined(NP2VER_X11)
	str += NP2VER_X11;
#endif

	QMessageBox::about(this, tr("About Neko Project II"), str);
}


//
// Emulation screen
//
emulationScreen::emulationScreen(QWidget *parent, const char *name, WFlags f)
	: QWidget(parent, name, f)
	, m_Surface(0)
	, m_Offscreen(0)
	, m_Painter(0)
{

	m_SrcRect.setRect(0, 0, 0, 0);
}

emulationScreen::~emulationScreen()
{

	destroyScreen();
}

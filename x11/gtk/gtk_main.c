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
#include "pccore.h"
#include "scrndraw.h"
#include "timing.h"

#include "toolkit.h"

#include "joymng.h"
#include "mousemng.h"
#include "scrnmng.h"
#include "soundmng.h"

#include "gtk/xnp2.h"
#include "gtk/gtk_keyboard.h"
#include "gtk/gtk_menu.h"

#include <gdk/gdkkeysyms.h>

#include "resources/np2.xbm"


#define	APPNAME		"NP2"

#define	EVENT_MASK \
			(GDK_BUTTON1_MOTION_MASK	\
			 | GDK_BUTTON2_MOTION_MASK	\
			 | GDK_BUTTON3_MOTION_MASK	\
			 | GDK_POINTER_MOTION_MASK	\
			 | GDK_KEY_PRESS_MASK		\
			 | GDK_KEY_RELEASE_MASK		\
			 | GDK_BUTTON_PRESS_MASK	\
			 | GDK_BUTTON_RELEASE_MASK	\
			 | GDK_ENTER_NOTIFY_MASK	\
			 | GDK_LEAVE_NOTIFY_MASK	\
			 | GDK_EXPOSURE_MASK)



/*
 - Signal: gboolean GtkWidget::expose_event(GtkWidget *widget,
          GdkEventExpose *event, gpointer user_data)
*/
static gboolean
expose(GtkWidget *w, GdkEventExpose *ev, gpointer p)
{

	UNUSED(w);
	UNUSED(p);

	if (ev->type == GDK_EXPOSE) {
		if (ev->count == 0) {
			scrndraw_redraw();
		}
		return TRUE;
	}
	return FALSE;
}

/*
 - Signal: gboolean GtkWidget::key_press_event (GtkWidget *widget,
          GdkEventKey *event, gpointer user_data)
*/
static gboolean
key_press(GtkWidget *w, GdkEventKey *ev, gpointer p)
{

	UNUSED(w);
	UNUSED(p);

	if (ev->type == GDK_KEY_PRESS) {
		if ((ev->keyval == GDK_F12) && (np2oscfg.F12KEY == 0))
			xmenu_toggle_item(MOUSE_MODE, !np2oscfg.MOUSE_SW, TRUE);
		else
			gtkkbd_keydown(ev->keyval);
		return TRUE;
	}
	return FALSE;
}

/*
 - Signal: gboolean GtkWidget::key_release_event (GtkWidget *widget,
          GdkEventKey *event, gpointer user_data)
*/
static gboolean
key_release(GtkWidget *w, GdkEventKey *ev, gpointer p)
{

	UNUSED(w);
	UNUSED(p);

	if (ev->type == GDK_KEY_RELEASE) {
		if ((ev->keyval != GDK_F12) || (np2oscfg.F12KEY != 0))
			gtkkbd_keyup(ev->keyval);
		return TRUE;
	}
	return FALSE;
}

/*
 - Signal: gboolean GtkWidget::button_press_event (GtkWidget *widget,
          GdkEventButton *event, gpointer user_data)
*/
static gboolean
button_press(GtkWidget *w, GdkEventButton *ev, gpointer p)
{

	UNUSED(w);
	UNUSED(p);

	if (ev->type == GDK_BUTTON_PRESS) {
		switch (ev->button) {
		case 1:
			mouse_btn(MOUSE_LEFTDOWN);
			break;

		case 2:
			xmenu_toggle_item(MOUSE_MODE, !np2oscfg.MOUSE_SW, TRUE);
			break;

		case 3:
			mouse_btn(MOUSE_RIGHTDOWN);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

/*
 - Signal: gboolean GtkWidget::button_release_event (GtkWidget *widget,
          GdkEventButton *event, gpointer user_data)
*/
static gboolean
button_release(GtkWidget *w, GdkEventButton *ev, gpointer p)
{

	UNUSED(w);
	UNUSED(p);

	if (ev->type == GDK_BUTTON_RELEASE) {
		switch (ev->button) {
		case 1:
			mouse_btn(MOUSE_LEFTUP);
			break;

		case 2:
			break;

		case 3:
			mouse_btn(MOUSE_RIGHTUP);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

/*
 - Signal: gboolean GtkWidget::enter_notify_event (GtkWidget *widget,
          GdkEventCrossing *event, gpointer user_data)
*/
static gboolean
enter_notify(GtkWidget *w, GdkEventCrossing *ev, gpointer p)
{

	UNUSED(w);
	UNUSED(p);

	if (ev->type == GDK_ENTER_NOTIFY) {
		scrndraw_redraw();
		return TRUE;
	}
	return FALSE;
}


/*
 * misc
 */
static void
set_icon_bitmap(GtkWidget *w)
{
	GdkPixmap *icon_pixmap;

	gdk_window_set_icon_name(w->window, APPNAME);
	icon_pixmap = gdk_bitmap_create_from_data(
	    w->window, np2_bits, np2_width, np2_height);
	gdk_window_set_icon(w->window, NULL, icon_pixmap, NULL);
}

#if 0
static GtkWidget*
create_pixmap(GtkWidget *widget, gchar **data)
{
	GtkWidget *pixmap;
	GdkColormap *colormap;
	GdkPixmap *gdkpixmap;
	GdkBitmap *mask;

	colormap = gtk_widget_get_colormap(widget);
	gdkpixmap = gdk_pixmap_colormap_create_from_xpm_d(
	    NULL, colormap, &mask, NULL, data);
	if (gdkpixmap == NULL) {
		g_warning("Couldn't create pixmap.");
		return NULL;
	}

	pixmap = gtk_pixmap_new(gdkpixmap, mask);
	gdk_pixmap_unref(gdkpixmap);
	gdk_bitmap_unref(mask);

	return pixmap;
}
#endif


/*
 * idle process
 */
static int install_count = 0;
static int idle_id;

void
install_idle_process(void)
{

	if (install_count++ == 0) {
		idle_id = gtk_idle_add((GtkFunction)mainloop, drawarea);
		soundmng_play();
	}
}

void
uninstall_idle_process(void)
{

	if (--install_count == 0) {
		soundmng_stop();
		gtk_idle_remove(idle_id);
	}
}


/*
 * toolkit
 */
const char *
gui_gtk_get_toolkit(void)
{

	return "gtk";
}

BOOL
gui_gtk_arginit(int *argcp, char ***argvp)
{

	gtk_set_locale();
	gtk_rc_add_default_file(".np2rc");
	gtk_init(argcp, argvp);

	return SUCCESS;
}

void
gui_gtk_widget_create(void)
{
	GtkWidget *main_vbox;
	GtkWidget *menubar;

	/* ウィンドウ作成 */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_policy(GTK_WINDOW(window), FALSE, FALSE, TRUE);
	gtk_window_set_title(GTK_WINDOW(window), np2oscfg.titles);
	gtk_widget_add_events(window, EVENT_MASK);

	main_vbox = gtk_vbox_new(FALSE, 2);
	gtk_container_border_width(GTK_CONTAINER(main_vbox), 1);
	gtk_container_add(GTK_CONTAINER(window), main_vbox);
	gtk_widget_show(main_vbox);

	/* メニューバー	*/
	menubar = create_menu(window);
	gtk_box_pack_start(GTK_BOX(main_vbox), menubar, FALSE, TRUE, 0);
	gtk_widget_show(menubar);

	/* 画面領域 */
	drawarea = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(drawarea), 640, 400);
	gtk_box_pack_start(GTK_BOX(main_vbox), drawarea, FALSE, TRUE, 0);
	gtk_widget_show(drawarea);

	gtk_widget_realize(window);
	set_icon_bitmap(window);

	/* setup signal */
	gtk_signal_connect(GTK_OBJECT(window), "destroy", 
	    GTK_SIGNAL_FUNC(gtk_main_quit), "WM destroy");
	gtk_signal_connect(GTK_OBJECT(window), "key_press_event",
	    GTK_SIGNAL_FUNC(key_press), NULL);
	gtk_signal_connect(GTK_OBJECT(window), "key_release_event",
	    GTK_SIGNAL_FUNC(key_release), NULL);
	gtk_signal_connect(GTK_OBJECT(window), "button_press_event",
	    GTK_SIGNAL_FUNC(button_press), NULL);
	gtk_signal_connect(GTK_OBJECT(window), "button_release_event",
	    GTK_SIGNAL_FUNC(button_release), NULL);

	/* setup drawarea signal */
	gtk_signal_connect(GTK_OBJECT(drawarea), "expose_event",
	    GTK_SIGNAL_FUNC(expose), NULL);
	gtk_signal_connect(GTK_OBJECT(drawarea), "enter_notify_event",
	    GTK_SIGNAL_FUNC(enter_notify), NULL);
}

static void
gui_gtk_terminate(void)
{

	/* Nothing to do */
}

void
gui_gtk_widget_show(void)
{

	gtk_widget_show(window);
}

void
gui_gtk_widget_mainloop(void)
{

	install_idle_process();
	gtk_main();
	uninstall_idle_process();
}

void
gui_gtk_widget_quit(void)
{

	gtk_main_quit();
}

void
gui_gtk_event_process(void)
{

	/* XXX: Nothing to do */
}

void
gui_gtk_set_window_title(const char* str)
{

	gtk_window_set_title(GTK_WINDOW(window), str);
}

/* toolkit data */
gui_toolkit_t gtk_toolkit = {
	gui_gtk_get_toolkit,
	gui_gtk_arginit,
	gui_gtk_terminate,
	gui_gtk_widget_create,
	gui_gtk_widget_show,
	gui_gtk_widget_mainloop,
	gui_gtk_widget_quit,
	gui_gtk_event_process,
	gui_gtk_set_window_title,
};

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
#include "taskmng.h"

#include "gtk/xnp2.h"
#include "gtk/gtk_keyboard.h"
#include "gtk/gtk_menu.h"

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
 - Signal: gboolean GtkWidget::configure_event(GtkWidget *widget,
          GdkEventConfigure *event, gpointer user_data)
*/
static gboolean
configure(GtkWidget *w, GdkEventConfigure *ev, gpointer p)
{

	UNUSED(ev);
	UNUSED(p);

	gdk_draw_rectangle(w->window, w->style->black_gc, TRUE,
	    0, 0, w->allocation.width, w->allocation.height);

	return TRUE;
}

/*
 - Signal: gboolean GtkWidget::expose_event(GtkWidget *widget,
          GdkEventExpose *event, gpointer user_data)
*/
static gboolean
expose(GtkWidget *w, GdkEventExpose *ev, gpointer p)
{

	UNUSED(w);
	UNUSED(p);

	if (ev->count == 0) {
		scrndraw_redraw();
	}
	return TRUE;
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

	if ((ev->keyval == GDK_F12) && (np2oscfg.F12KEY == 0))
		xmenu_toggle_item(MOUSE_MODE, !np2oscfg.MOUSE_SW, TRUE);
	else
		gtkkbd_keydown(ev->keyval);
	return TRUE;
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

	if ((ev->keyval != GDK_F12) || (np2oscfg.F12KEY != 0))
		gtkkbd_keyup(ev->keyval);
	return TRUE;
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

/*
 - Signal: gboolean GtkWidget::button_release_event (GtkWidget *widget,
          GdkEventButton *event, gpointer user_data)
*/
static gboolean
button_release(GtkWidget *w, GdkEventButton *ev, gpointer p)
{

	UNUSED(w);
	UNUSED(p);

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


/*
 * idle process
 */
static int install_count = 0;
static int idle_id;
#if defined(SUPPORT_JOYSTICK)
static int joymng_task_id;
#endif

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
	char tmp[MAX_PATH];
	char *homeenv;

	homeenv = getenv("HOME");

	gtk_set_locale();
	gtk_init(argcp, argvp);
	if (homeenv) {
		g_snprintf(tmp, sizeof(tmp), "%s/.np2/gtkrc", homeenv);
		gtk_rc_add_default_file(tmp);
	}

	return SUCCESS;
}

void
gui_gtk_widget_create(void)
{
	GtkWidget *main_vbox;
	GtkWidget *menubar;
#if (GTK_MAJOR_VERSION == 2)
	gchar *accel = NULL;
#endif

	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_policy(GTK_WINDOW(main_window), FALSE, FALSE, TRUE);
	gtk_window_set_title(GTK_WINDOW(main_window), np2oscfg.titles);
	gtk_widget_add_events(main_window, EVENT_MASK);

	main_vbox = gtk_vbox_new(FALSE, 2);
	gtk_container_border_width(GTK_CONTAINER(main_vbox), 1);
	gtk_container_add(GTK_CONTAINER(main_window), main_vbox);
	gtk_widget_show(main_vbox);

	menubar = create_menu();
	gtk_box_pack_start(GTK_BOX(main_vbox), menubar, FALSE, TRUE, 0);
	gtk_widget_show(menubar);

	drawarea = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(drawarea), 640, 400);
	gtk_box_pack_start(GTK_BOX(main_vbox), drawarea, FALSE, TRUE, 0);
	gtk_widget_show(drawarea);

#if (GTK_MAJOR_VERSION == 2)
	g_object_get(gtk_widget_get_settings(main_window),
	    "gtk-menu-bar-accel", &accel, NULL);
	if (accel) {
		g_object_set(gtk_widget_get_settings(main_window),
		    "gtk-menu-bar-accel", "Menu", NULL);
		g_free(accel);
	}
#endif

	gtk_widget_realize(main_window);
	set_icon_bitmap(main_window);

	gtk_signal_connect(GTK_OBJECT(main_window), "destroy", 
	    GTK_SIGNAL_FUNC(gtk_main_quit), "WM destroy");
	gtk_signal_connect(GTK_OBJECT(main_window), "key_press_event",
	    GTK_SIGNAL_FUNC(key_press), NULL);
	gtk_signal_connect(GTK_OBJECT(main_window), "key_release_event",
	    GTK_SIGNAL_FUNC(key_release), NULL);
	gtk_signal_connect(GTK_OBJECT(main_window), "button_press_event",
	    GTK_SIGNAL_FUNC(button_press), NULL);
	gtk_signal_connect(GTK_OBJECT(main_window), "button_release_event",
	    GTK_SIGNAL_FUNC(button_release), NULL);

	gtk_signal_connect(GTK_OBJECT(drawarea), "configure_event",
	    GTK_SIGNAL_FUNC(configure), NULL);
	gtk_signal_connect(GTK_OBJECT(drawarea), "expose_event",
	    GTK_SIGNAL_FUNC(expose), NULL);
}

static void
gui_gtk_terminate(void)
{

	/* Nothing to do */
}

void
gui_gtk_widget_show(void)
{

	gtk_widget_show(main_window);
}

void
gui_gtk_widget_mainloop(void)
{

#if defined(SUPPORT_JOYSTICK)
	joymng_task_id = gtk_idle_add((GtkFunction)joymng_update_task, NULL);
#endif
	install_idle_process();
	gtk_main();
	uninstall_idle_process();
}

void
gui_gtk_widget_quit(void)
{

#if defined(SUPPORT_JOYSTICK)
	gtk_idle_remove(joymng_task_id);
#endif
	taskmng_exit();
	gtk_main_quit();
}

void
gui_gtk_event_process(void)
{

	if (taskmng_isavail() && gdk_events_pending()) {
		gtk_main_iteration_do(FALSE);
	}
}

void
gui_gtk_set_window_title(const char* str)
{

	gtk_window_set_title(GTK_WINDOW(main_window), str);
}

void
gui_gtk_messagebox(const char *title, const char *msg)
{

	UNUSED(title);
	g_message(msg);
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
	gui_gtk_messagebox,
};

/*	$Id: window_softkbd.c,v 1.1 2004/06/16 12:50:49 monaka Exp $	*/

#include "compiler.h"

#if defined(SUPPORT_SOFTKBD)

#include "np2.h"

#include "skbdwin.h"

#include "gtk/xnp2.h"
#include "gtk/gtk_drawmng.h"
#include "gtk/gtk_keyboard.h"
#include "gtk/gtk_menu.h"


/*
 * software keyboard
 */

typedef struct {
	GtkWidget	*window;
	GTKDRAWMNG_HDL	hdl;

	BOOL		drawing;
} SKBDWIN;

static SKBDWIN skwin;


static void
skpalcnv(CMNPAL *dst, const RGB32 *src, UINT pals, UINT bpp)
{
	UINT i;

	switch (bpp) {
#if defined(SUPPORT_16BPP)
	case 16:
		for (i = 0; i < pals; i++) {
			dst[i].pal16 = drawmng_makepal16(&skwin.hdl->pal16mask, src[i]);
		}
		break;
#endif
#if defined(SUPPORT_24BPP)
	case 24:
#endif
#if defined(SUPPORT_32BPP)
	case 32:
#endif
#if defined(SUPPORT_24BPP) || defined(SUPPORT_32BPP)
		for (i = 0; i < pals; i++) {
			dst[i].pal32.d = src[i].d;
		}
		break;
#endif
	}
}

static void
skbdwin_draw(BOOL redraw)
{
	CMNVRAM *vram;

	skwin.drawing = TRUE;
	vram = gtkdrawmng_surflock(skwin.hdl);
	if (vram) {
		softkbd_paint(vram, skpalcnv, redraw);
		gtkdrawmng_surfunlock(skwin.hdl);
		gtkdrawmng_blt(skwin.hdl, NULL, NULL);
	}
	skwin.drawing = FALSE;
}

/*
 * signal
 */
static void
skbdwin_window_destroy(GtkWidget *w, gpointer p)
{

	UNUSED(w);
	UNUSED(p);

	if (skwin.window)
		skwin.window = NULL;
	while (skwin.drawing)
		usleep(1);
	gtkdrawmng_release(skwin.hdl);
	skwin.hdl = NULL;
}

static gboolean
skbdwin_expose(GtkWidget *w, GdkEventExpose *ev)
{

	UNUSED(w);

	if (ev->type == GDK_EXPOSE) {
		if (ev->count == 0) {
			skbdwin_draw(TRUE);
			return TRUE;
		}
	}
	return FALSE;
}

static gboolean
skbdwin_key_press(GtkWidget *w, GdkEventKey *ev, gpointer p)
{

	UNUSED(w);
	UNUSED(p);

	if (ev->type == GDK_KEY_PRESS) {
		if ((ev->keyval != GDK_F12) || (np2oscfg.F12KEY != 0))
			gtkkbd_keydown(ev->keyval);
		return TRUE;
	}
	return FALSE;
}
static gboolean
skbdwin_key_release(GtkWidget *w, GdkEventKey *ev, gpointer p)
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

static gboolean
skbdwin_button_press(GtkWidget *w, GdkEventButton *ev, gpointer p)
{

	UNUSED(w);
	UNUSED(p);

	if (ev->type == GDK_BUTTON_PRESS) {
		switch (ev->button) {
		case 1:
			softkbd_down((int)ev->x, (int)ev->y);
			break;
		}
		return TRUE;
	}
	return FALSE;
}

static gboolean
skbdwin_button_release(GtkWidget *w, GdkEventButton *ev, gpointer p)
{

	UNUSED(w);
	UNUSED(p);

	if (ev->type == GDK_BUTTON_RELEASE) {
		switch (ev->button) {
		case 1:
			softkbd_up();
			break;
		}
		return TRUE;
	}
	return FALSE;
}

/*
 * software keyboard interface
 */
#define	EVENT_MASK \
			 ( GDK_KEY_PRESS_MASK		\
			 | GDK_KEY_RELEASE_MASK		\
			 | GDK_BUTTON_PRESS_MASK	\
			 | GDK_BUTTON_RELEASE_MASK	\
			 | GDK_EXPOSURE_MASK)

void
skbdwin_create(void)
{
	GtkWidget *main_widget;
	int width, height;

	if (skwin.window)
		return;

	if (softkbd_getsize(&width, &height) != SUCCESS)
		return;

	skwin.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(skwin.window), "Software Keyboard");
	gtk_window_set_policy(GTK_WINDOW(skwin.window), FALSE, FALSE, TRUE);
	gtk_widget_add_events(skwin.window, EVENT_MASK);
	gtk_signal_connect(GTK_OBJECT(skwin.window), "destroy",
	    GTK_SIGNAL_FUNC(skbdwin_window_destroy), NULL);

	main_widget = gtk_vbox_new(FALSE, 2);
	gtk_widget_show(main_widget);
	gtk_container_add(GTK_CONTAINER(skwin.window), main_widget);

	skwin.hdl = gtkdrawmng_create(skwin.window, width, height);
	if (skwin.hdl == NULL) {
		goto destroy;
	}
	gtk_box_pack_start(GTK_BOX(main_widget), skwin.hdl->drawarea,
	    FALSE, TRUE, 0);
	gtk_widget_show(skwin.hdl->drawarea);
	gtk_widget_realize(skwin.window);

	gtk_signal_connect(GTK_OBJECT(skwin.window), "key_press_event",
	    GTK_SIGNAL_FUNC(skbdwin_key_press), NULL);
	gtk_signal_connect(GTK_OBJECT(skwin.window), "key_release_event",
	    GTK_SIGNAL_FUNC(skbdwin_key_release), NULL);
	gtk_signal_connect(GTK_OBJECT(skwin.window), "button_press_event",
	    GTK_SIGNAL_FUNC(skbdwin_button_press), NULL);
	gtk_signal_connect(GTK_OBJECT(skwin.window), "button_release_event",
	    GTK_SIGNAL_FUNC(skbdwin_button_release), NULL);

	gtk_signal_connect(GTK_OBJECT(skwin.hdl->drawarea),
	    "expose_event", GTK_SIGNAL_FUNC(skbdwin_expose), NULL);

	skwin.drawing = FALSE;
	gtk_widget_show(skwin.window);

	gtkdrawmng_set_size(skwin.hdl, width, height);
	skbdwin_draw(TRUE);
	return;

destroy:
	gtk_widget_destroy(skwin.window);
	skwin.window = NULL;
}

void
skbdwin_destroy(void)
{

	if (skwin.window) {
		gtk_widget_destroy(skwin.window);
		skwin.window = NULL;
	}
}

void
skbdwin_process(void)
{

	if (skwin.window && softkbd_process()) {
		skbdwin_draw(FALSE);
	}
}
#endif	/* SUPPORT_SOFTKBD */

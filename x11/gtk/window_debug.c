/*	$Id: window_debug.c,v 1.2 2004/06/17 14:36:34 monaka Exp $	*/

#include "compiler.h"

#if defined(SUPPORT_MEMDBG32)

#include "np2.h"

#include "debugwin.h"
#include "memdbg32.h"

#include "gtk/xnp2.h"
#include "gtk/gtk_drawmng.h"


#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
/*
 * memory map
 */

typedef struct {
	DRAWMNG_HDL	hdl;

	GtkWidget	*window;
} MEMDBG;

static MEMDBG memdbg;


/*
 * color managements
 */
static const UINT32 mdwinpal[MEMDBG32_PALS] = {
	0x00333333, 0x00000000, 0x00ffaa00, 0x00ff0000, 0x00118811, 0x0000ff00,
	0xffffffff
};

static BYTE
getpal8(CMNPALFN *self, UINT num)
{

	UNUSED(self);

	if (num < MEMDBG32_PALS)
		return mdwinpal[num] >> 24;
	return 0;
}

static UINT32
getpal32(CMNPALFN *self, UINT num)
{

	UNUSED(self);

	if (num < MEMDBG32_PALS)
		return mdwinpal[num] & 0xffffff;
	return 0;
}

static UINT16
cnvpal16(CMNPALFN *self, RGB32 pal32)
{
	MEMDBG *d = (MEMDBG *)self->userdata;

	return (UINT16)drawmng_makepal16(&d->hdl->pal16mask, pal32);
}

static void
memdbg_draw(BOOL redraw)
{
	CMNVRAM *vram;

	vram = drawmng_surflock(memdbg.hdl);
	if (vram) {
		memdbg32_paint(vram, redraw);
		drawmng_surfunlock(memdbg.hdl);
		drawmng_blt(memdbg.hdl, NULL, NULL);
	}
}

static void
setclientsize(void)
{
	int width, height;

	memdbg32_getsize(&width, &height);
	drawmng_set_size(memdbg.hdl, width, height);
}

/*
 * signal
 */
static void
memdbg_window_destroy(GtkWidget *w, gpointer p)
{

	UNUSED(w);
	UNUSED(p);

	if (memdbg.window)
		memdbg.window = NULL;
	drawmng_release(memdbg.hdl);
	memdbg.hdl = NULL;
}

static gint
memdbg_expose(GtkWidget *w, GdkEventExpose *ev)
{

	UNUSED(w);

	if (ev->type == GDK_EXPOSE) {
		if (ev->count == 0) {
			memdbg_draw(TRUE);
			return TRUE;
		}
	}
	return FALSE;
}

/*
 * memdbg Interface
 */
void
memdbg_create(void)
{
	GtkWidget *main_widget;
	CMNPALFN palfn;
	int width, height;

	if (memdbg.window)
		return;

	memdbg32_initialize();
	memdbg32_getsize(&width, &height);

	memdbg.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(memdbg.window), "Memory Map");
	gtk_window_set_policy(GTK_WINDOW(memdbg.window), FALSE, FALSE, TRUE);
	gtk_signal_connect(GTK_OBJECT(memdbg.window), "destroy",
	    GTK_SIGNAL_FUNC(memdbg_window_destroy), NULL);

	main_widget = gtk_vbox_new(FALSE, 2);
	gtk_widget_show(main_widget);
	gtk_container_add(GTK_CONTAINER(memdbg.window), main_widget);

	memdbg.hdl = drawmng_create(memdbg.window, width, height);
	if (memdbg.hdl == NULL) {
		goto destroy;
	}
	gtk_box_pack_start(GTK_BOX(main_widget), memdbg.hdl->drawarea,
	    FALSE, TRUE, 0);
	gtk_widget_show(memdbg.hdl->drawarea);
	gtk_widget_realize(memdbg.window);
	gtk_signal_connect(GTK_OBJECT(memdbg.hdl->drawarea), "expose_event",
	    GTK_SIGNAL_FUNC(memdbg_expose), NULL);

	gtk_widget_show(memdbg.window);

	palfn.get8 = getpal8;
	palfn.get32 = getpal32;
	palfn.cnv16 = cnvpal16;
	palfn.userdata = &memdbg;
	memdbg32_setpal(&palfn);
	setclientsize();
	memdbg_draw(TRUE);
	return;

destroy:
	gtk_widget_destroy(memdbg.window);
	memdbg.window = NULL;
}

void
memdbg_destroy(void)
{

	if (memdbg.window) {
		gtk_widget_destroy(memdbg.window);
		memdbg.window = NULL;
	}
}

void
memdbg_process(void)
{

	if (memdbg.window) {
		memdbg_draw(FALSE);
	}
}
#endif	/* CPUCORE_IA32 && SUPPORT_MEMDBG32 */


/*
 * Interface
 */
void
debugwin_create(void)
{

#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
	memdbg_create();
#endif
}

void
debugwin_destroy(void)
{

#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
	memdbg_destroy();
#endif
}

void
debugwin_process(void)
{

#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
	memdbg_process();
#endif
}

#endif	/* SUPPORT_MEMDBG32 */

#include "compiler.h"

#if defined(SUPPORT_KEYDISP)

#include "np2.h"

#include "kdispwin.h"

#include "scrnmng.h"
#include "sysmng.h"

#include "gtk/xnp2.h"
#include "gtk/gtk_menu.h"
#include "gtk/gtk_drawmng.h"


static UINT32 kdwinpal[KEYDISP_PALS] = {
	0x00000000, 0xffffffff, 0xf9ff0000
};

typedef struct {
	DRAWMNG_HDL	hdl;

	GtkWidget	*window;
	_MENU_HDL	menuhdl;
} KDWIN;

static KDWIN kdwin;

static void drawkeys(void);
static void setkeydispmode(BYTE mode);
static BYTE kdispwin_getmode(BYTE cfgmode);


/*
 * keydisp widget
 */

/*
 * Menu
 */
static void
kdispwin_window_destroy(GtkWidget *w, gpointer p)
{

	UNUSED(w);
	UNUSED(p);

	if (kdwin.window)
		kdwin.window = NULL;
	drawmng_release(kdwin.hdl);
	kdwin.hdl = NULL;
}

static void
close_window(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(action);
	UNUSED(w);

	xmenu_toggle_item(KEY_DISPLAY, 0, TRUE);
}

static void
change_module(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(w);

	if (kdispcfg.mode != action) {
		kdispcfg.mode = action;
		sysmng_update(SYS_UPDATEOSCFG);
		setkeydispmode(kdispwin_getmode(kdispcfg.mode));
	}
}

static void
xmenu_select_module(BYTE  mode)
{
	static const char *name[] = {
		NULL,
		"/Module/FM",
		"/Module/MIDI",
	};

	if (mode < NELEMENTS(name) && name[mode]) {
		xmenu_select_item(&kdwin.menuhdl, name[mode]);
	}
}

#define f(f)	((GtkItemFactoryCallback)(f))
static GtkItemFactoryEntry menu_items[] = {
{ "/_Module",		NULL, NULL,             0,             "<Branch>"    },
{ "/Module/_FM",	NULL, f(change_module), KDISPCFG_FM,   "<RadioItem>" },
{ "/Module/_MIDI",	NULL, f(change_module), KDISPCFG_MIDI, "/Module/FM"  },
{ "/Module/sep",	NULL, NULL,             0,             "<Separator>" },
{ "/Module/_Close",	NULL, f(close_window),  0,             NULL          },
};
#undef	f

static GtkWidget *
create_kdispwin_menu(GtkWidget *parent)
{
	GtkAccelGroup *accel_group;
	GtkWidget *menubar;

	(void)parent;

	accel_group = gtk_accel_group_new();
	kdwin.menuhdl.item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", accel_group);
	gtk_item_factory_create_items(kdwin.menuhdl.item_factory, NELEMENTS(menu_items), menu_items, NULL);

	menubar = gtk_item_factory_get_widget(kdwin.menuhdl.item_factory, "<main>");

	xmenu_select_module(kdispwin_getmode(kdispcfg.mode));

	return menubar;
}

/*
 * signal
 */
static gint
kdispwin_expose(GtkWidget *w, GdkEventExpose *ev)
{

	UNUSED(w);

	if (ev->type == GDK_EXPOSE) {
		if (ev->count == 0) {
			drawkeys();
			return TRUE;
		}
	}
	return FALSE;
}


/*
 * keydisp local function
 */
static BYTE
getpal8(CMNPALFN *self, UINT num)
{

	UNUSED(self);

	if (num < KEYDISP_PALS) {
		return kdwinpal[num] >> 24;
	}
	return 0;
}

static UINT32
getpal32(CMNPALFN *self, UINT num)
{

	UNUSED(self);

	if (num < KEYDISP_PALS) {
		return kdwinpal[num] & 0xffffff;
	}
	return 0;
}

static UINT16
cnvpal16(CMNPALFN *self, RGB32 pal32)
{

	UNUSED(self);

	return (UINT16)drawmng_makepal16(&kdwin.hdl->pal16mask, pal32);
}

static BYTE
kdispwin_getmode(BYTE cfgmode)
{

	switch (cfgmode) {
	case KDISPCFG_FM:
	default:
		return KEYDISP_MODEFM;

	case KDISPCFG_MIDI:
		return KEYDISP_MODEMIDI;
	}
}

static void
drawkeys(void)
{
	CMNVRAM *vram;

	vram = drawmng_surflock(kdwin.hdl);
	if (vram) {
		keydisp_paint(vram, TRUE);
		drawmng_surfunlock(kdwin.hdl);
		drawmng_blt(kdwin.hdl, NULL, NULL);
	}
}

static void
setkdwinsize(void)
{
	int width, height;

	keydisp_getsize(&width, &height);
	drawmng_set_size(kdwin.hdl, width, height);
}

static void
setkeydispmode(BYTE mode)
{

	keydisp_setmode(mode);
}

/*
 * Interface
 */
void
kdispwin_create(void)
{
	GtkWidget *main_widget;
	GtkWidget *kdispwin_menu;
	GtkWidget *da;
	CMNPALFN palfn;
	BYTE mode;

	if (kdwin.window)
		return;

	/* keydisp ウィンドウ作成 */
	kdwin.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(kdwin.window), "Key Display");
	gtk_window_set_policy(GTK_WINDOW(kdwin.window), FALSE, FALSE, TRUE);
	gtk_signal_connect(GTK_OBJECT(kdwin.window), "destroy",
	    GTK_SIGNAL_FUNC(kdispwin_window_destroy), NULL);

	main_widget = gtk_vbox_new(FALSE, 2);
	gtk_widget_show(main_widget);
	gtk_container_add(GTK_CONTAINER(kdwin.window), main_widget);

	/* keydisp メニューバー */
	kdispwin_menu = create_kdispwin_menu(kdwin.window);
	gtk_box_pack_start(GTK_BOX(main_widget), kdispwin_menu, FALSE, TRUE, 0);
	gtk_widget_show(kdispwin_menu);

	/* keydisp 画面領域 */
	kdwin.hdl = drawmng_create(kdwin.window, KEYDISP_WIDTH, KEYDISP_HEIGHT);
	if (kdwin.hdl == NULL) {
		goto destroy;
	}

	da = GTK_WIDGET(drawmng_get_widget_handle(kdwin.hdl));
	gtk_box_pack_start(GTK_BOX(main_widget), da, FALSE, TRUE, 0);
	gtk_widget_show(da);
	gtk_widget_realize(kdwin.window);
	gtk_signal_connect(GTK_OBJECT(da), "expose_event",
	    GTK_SIGNAL_FUNC(kdispwin_expose), NULL);

	mode = kdispwin_getmode(kdispcfg.mode);
	setkeydispmode(mode);
	setkdwinsize();
	gtk_widget_show(kdwin.window);

	palfn.get8 = getpal8;
	palfn.get32 = getpal32;
	palfn.cnv16 = cnvpal16;
	keydisp_setpal(&palfn);
	drawmng_invalidate(kdwin.hdl, NULL);
	return;

destroy:
	gtk_widget_destroy(kdwin.window);
	kdwin.window = NULL;
	xmenu_toggle_item(KEY_DISPLAY, 0, TRUE);
	sysmng_update(SYS_UPDATEOSCFG);
}

void
kdispwin_destroy(void)
{

	if (kdwin.window) {
		gtk_widget_destroy(kdwin.window);
		kdwin.window = NULL;
	}
}

void
kdispwin_draw(BYTE cnt)
{
	BYTE flag;

	if (kdwin.window) {
		if (cnt == 0) {
			cnt = 1;
		}
		flag = keydisp_process(cnt);
		if (flag & KEYDISP_FLAGSIZING) {
			setkdwinsize();
		}
		drawmng_invalidate(kdwin.hdl, NULL);
	}
}

#endif	/* SUPPORT_KEYDISP */

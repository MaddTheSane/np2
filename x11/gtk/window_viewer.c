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

#if defined(SUPPORT_VIEWER)

#include "np2.h"
#include "cpucore.h"
#include "pccore.h"
#include "iocore.h"

#include "bios.h"
#include "debugsub.h"
#include "viewer.h"
#include "viewmem.h"

#include "gtk/xnp2.h"
#include "gtk/gtk_menu.h"

#define	DEFAULT_FONTSIZE	16

static NP2VIEW_T np2view[NP2VIEW_MAX];
static const char viewcmn_hex[16] = "0123456789abcdef";

/* init */
static void viewreg_init(NP2VIEW_T *dst, NP2VIEW_T *src);
static void viewseg_init(NP2VIEW_T *dst, NP2VIEW_T *src);

/* expose event */
static void viewreg_expose(NP2VIEW_T *view);
static void viewseg_expose(NP2VIEW_T *view);

/* misc */
static void invalidate(NP2VIEW_T *view);


/*-----------------------------------------------------------------------------
 * viewer common
 */
static void
viewcmn_setvscroll(NP2VIEW_T *view)
{

	UNUSED(view);
}

static void
viewcmn_caption(NP2VIEW_T *view, char *buf, size_t size)
{
	const char *p;

	if (view->lock) {
		p = "Locked";
	} else {
		p = "RealTime";
	}
	g_snprintf(buf, size, "%d.%s - NP2 Debug Utility", view->index + 1, p);
}

void
viewcmn_putcaption(NP2VIEW_T *view)
{
	char buf[256];

	viewcmn_caption(view, buf, sizeof(buf));
	gtk_window_set_title(GTK_WINDOW(view->window), buf);
}

BOOL
viewcmn_alloc(VIEWMEMBUF *buf, UINT32 size)
{

	if (buf->type != ALOOCTYPE_ERROR) {
		if (buf->size < size) {
			if (buf->ptr) {
				free(buf->ptr);
			}
			memset(buf, 0, sizeof(*buf));
			buf->ptr = malloc(size);
			if (buf->ptr == NULL) {
				buf->type = ALOOCTYPE_ERROR;
				return FAILURE;
			}
			memset(buf->ptr, 0, size);
			buf->size = size;
		}
		return SUCCESS;
	}
	return FAILURE;
}

static void
viewcmn_free(VIEWMEMBUF *buf)
{

	if (buf->ptr) {
		free(buf->ptr);
	}
	memset(buf, 0, sizeof(*buf));
}

static void
viewcmn_setmode(NP2VIEW_T *dst, NP2VIEW_T *src, BYTE type)
{

	switch (type) {
	case VIEWMODE_REG:
		viewreg_init(dst, src);
		break;

	case VIEWMODE_SEG:
		viewseg_init(dst, src);
		break;
	}
}

static void
viewcmn_setbank(NP2VIEW_T *view)
{
	VIEWMEM_T *dmem;

	dmem = &view->dmem;
	dmem->vram = gdcs.disp;
	dmem->itf = i286core.s.itfbank;
	dmem->A20 = (BYTE)((i286core.s.adrsmask >> 20) & 1);
}

static void
viewer_segmode(NP2VIEW_T *view, BYTE type)
{

	if (view->type != type) {
		viewcmn_setmode(view, view, type);
		viewcmn_setbank(view);
		viewcmn_setvscroll(view);
		invalidate(view);
	}
}

static void
viewcmn_paint(NP2VIEW_T *view, void (*callback)(NP2VIEW_T *, RECT_T *))
{
	GtkWidget *w = (GtkWidget *)view->widget;
	RECT_T r;

	r.left = w->allocation.x;
	r.top = w->allocation.y;
	r.right = w->allocation.x + w->allocation.width;
	r.bottom = w->allocation.y + w->allocation.height;
	(*callback)(view, &r);
}

/*-----------------------------------------------------------------------------
 * viewer register
 */
static void
viewreg_init(NP2VIEW_T *dst, NP2VIEW_T *src)
{

	UNUSED(src);

	dst->type = VIEWMODE_REG;
	dst->maxline = 4;
	dst->mul = 1;
	dst->pos = 0;
}

static void
viewreg_paint(NP2VIEW_T *view, RECT_T *r)
{
	GtkWidget *w = (GtkWidget *)view->widget;
	char buf[128];
	I286STAT *reg;
	UINT32 pos;
	int y;

	UNUSED(r);

	if (view->lock) {
		if (view->buf1.type != ALLOCTYPE_REG) {
			if (viewcmn_alloc(&view->buf1, sizeof(i286core.s))) {
				xmenu_select_item(view->menu, "/Mode/Lock");
			} else {
				view->buf1.type = ALLOCTYPE_REG;
				memcpy(view->buf1.ptr, &i286core.s, sizeof(i286core.s));
			}
			viewcmn_putcaption(view);
		}
	}

	pos = view->pos;
	if (view->lock) {
		reg = (I286STAT *)view->buf1.ptr;
	} else {
		reg = &i286core.s;
	}

	for (y = 0; y < r->bottom && pos < 4; y += view->fontsize, pos++) {
		switch (pos) {
		case 0:
			g_snprintf(buf, sizeof(buf),
			    "AX=%04x  BX=%04x  CX=%04x  DX=%04x",
			    reg->r.w.ax, reg->r.w.bx, reg->r.w.cx, reg->r.w.dx);
			break;

		case 1:
			g_snprintf(buf, sizeof(buf),
			    "SP=%04x  BP=%04x  SI=%04x  DI=%04x",
			    reg->r.w.sp, reg->r.w.bp, reg->r.w.si, reg->r.w.di);
			break;

		case 2:
			g_snprintf(buf, sizeof(buf),
			    "DS=%04x  ES=%04x  SS=%04x  CS=%04x",
			    reg->r.w.ds, reg->r.w.es, reg->r.w.ss, reg->r.w.cs);
			break;

		case 3:
			g_snprintf(buf, sizeof(buf), "IP=%04x   %s",
			    reg->r.w.ip, debugsub_flags(reg->r.w.flag));
			break;

		default:
			return;
		}
		gdk_draw_string(w->window, view->font,
		    w->style->fg_gc[GTK_WIDGET_STATE(w)],
		    r->left, r->top + y, buf);
	}
}

static void
viewreg_expose(NP2VIEW_T *view)
{

	viewcmn_paint(view, viewreg_paint);
}

/*-----------------------------------------------------------------------------
 * viewer segment memory
 */
static void
viewseg_init(NP2VIEW_T *dst, NP2VIEW_T *src)
{

	if (src) {
		switch (src->type) {
		case VIEWMODE_SEG:
			dst->seg = dst->seg;
			break;

		case VIEWMODE_1MB:
			if (dst->pos < 0x10000) {
				dst->seg = (UINT16)dst->pos;
			} else {
				dst->seg = 0xffff;
			}
			break;

		case VIEWMODE_ASM:
			dst->seg = src->seg;
			break;

		default:
			src = NULL;
			break;
		}
	}
	if (src == NULL) {
		dst->seg = 0;
	}
	dst->type = VIEWMODE_SEG;
	dst->maxline = 0x1000;
	dst->mul = 1;
	dst->pos = 0;
}

static void
viewseg_paint(NP2VIEW_T *view, RECT_T *r)
{
	GtkWidget *w = (GtkWidget *)view->widget;
	char str[256];
	BYTE buf[16];
	char tmp[] = "   ";
	BYTE *p;
	UINT32 off;
	UINT32 madr;
	int x, y;

	off = (view->pos) >> 4;
	madr = (((UINT32)view->seg) << 4) + off;

	if (view->lock) {
		if ((view->buf1.type != ALLOCTYPE_SEG)
		 || (view->buf1.arg != view->seg)) {
			if (viewcmn_alloc(&view->buf1, 0x10000)) {
				xmenu_select_item(view->menu, "/Mode/Lock");
			} else {
				view->buf1.type = ALLOCTYPE_SEG;
				view->buf1.arg = view->seg;
				viewmem_read(&view->dmem, view->buf1.arg << 4,
				    (BYTE *)view->buf1.ptr, 0x10000);
			}
			viewcmn_putcaption(view);
		}
	}

	for (y = 0; y < r->bottom && off < 0x10000; y += view->fontsize, off += 16) {
		g_snprintf(str, sizeof(str), "%04x:%04x", view->seg, off);
		if (view->lock) {
			p = (BYTE *)view->buf1.ptr;
			p += off;
		} else {
			p = buf;
			viewmem_read(&view->dmem, madr, buf, 16);
			madr += 16;
		}
		for (x = 0; x < 16; x++) {
			tmp[1] = viewcmn_hex[*p >> 4];
			tmp[2] = viewcmn_hex[*p & 15];
			p++;
			milstr_ncat(str, tmp, sizeof(str));
		}
		gdk_draw_string(w->window, view->font,
		    w->style->fg_gc[GTK_WIDGET_STATE(w)],
		    r->left, r->top + y, str);
	}
}


static void
viewseg_expose(NP2VIEW_T *view)
{

	viewcmn_paint(view, viewseg_paint);
}

/*-----------------------------------------------------------------------------
 * viewer
 */
static GtkWidget *viewer_create_menu(NP2VIEW_T *view, GtkWidget *parent);

static void
viewer_close(NP2VIEW_T *view)
{

	view->lock = TRUE;
	viewcmn_free(&view->buf1);
	viewcmn_free(&view->buf2);
	view->lock = FALSE;
	view->widget = NULL;
	view->window = NULL;
	if (view->enter) {
		install_idle_process();
		view->enter = FALSE;
	}
	view->alive = FALSE;
}

static void
viewer_destroy(GtkWidget *w, gpointer p)
{

	UNUSED(w);

	viewer_close((NP2VIEW_T *)p);
}

static gboolean
viewer_expose(GtkWidget *w, GdkEventExpose *ev, gpointer p)
{
	NP2VIEW_T *view;

	UNUSED(w);

	if (ev->type == GDK_EXPOSE) {
		if (ev->count == 0) {
			gdk_window_clear_area(w->window, ev->area.x, ev->area.y,
			    ev->area.width, ev->area.height);
			gdk_gc_set_clip_rectangle(w->style->fg_gc[GTK_WIDGET_STATE(w)], &ev->area);
			view = (NP2VIEW_T *)p;
			switch (view->type) {
			case VIEWMODE_REG:
				viewreg_expose(view);
				break;

			case VIEWMODE_SEG:
				viewseg_expose(view);
				break;
			}
			gdk_gc_set_clip_rectangle(w->style->fg_gc[GTK_WIDGET_STATE(w)], NULL);
		}
		return TRUE;
	}
	return FALSE;
}

static gboolean
viewer_focus_in(GtkWidget *w, GdkEventFocus *ev, gpointer p)
{
	NP2VIEW_T *view;

	UNUSED(w);

	if (ev->type == GDK_FOCUS_CHANGE) {
		view = (NP2VIEW_T *)p;
		view->enter = TRUE;
		uninstall_idle_process();
		return TRUE;
	}
	return FALSE;
}

static gboolean
viewer_focus_out(GtkWidget *w, GdkEventFocus *ev, gpointer p)
{
	NP2VIEW_T *view;

	UNUSED(w);

	if (ev->type == GDK_FOCUS_CHANGE) {
		view = (NP2VIEW_T *)p;
		view->enter = FALSE;
		install_idle_process();
		return TRUE;
	}
	return FALSE;
}

static gboolean
viewer_changed(GtkAdjustment *adj, gpointer p)
{
	NP2VIEW_T *view = (NP2VIEW_T *)p;

	UNUSED(view);
	UNUSED(adj);

	return TRUE;
}

static gboolean
viewer_value_changed(GtkAdjustment *adj, gpointer p)
{
	NP2VIEW_T *view = (NP2VIEW_T *)p;

	UNUSED(view);
	UNUSED(adj);

	return TRUE;
}

static gboolean
view_fontload(NP2VIEW_T *view, UINT8 size)
{
	char work[1024];
	GdkFont *font;

	if (view->fontsize != size) {
		if (view->font) {
			gdk_font_unref(view->font);
			view->font = NULL;
		}

		g_snprintf(work, sizeof(work), fontname, "medium", size);
		font = gdk_fontset_load(work);
		if (font == NULL) {
			fprintf(stderr, "Warning: Can't create font-set.\n");
			return FAILURE;
		}
		view->font = font;
		view->fontsize = size;
	}

	return SUCCESS;
}

/*
 * interface
 */
BOOL
viewer_init(void)
{
	NP2VIEW_T *view;
	int i;

	view = np2view;
	for (i = 0; i < NP2VIEW_MAX; i++, view++) {
		view->index = i;
		view->enter = FALSE;
		view->font = NULL;
	}

	return SUCCESS;
}

void
viewer_term(void)
{
	NP2VIEW_T *view;
	int i;

	view = np2view;
	for (i = 0; i < NP2VIEW_MAX; i++, view++) {
		if (view->font) {
			gdk_font_unref(view->font);
			view->font = NULL;
		}
	}
}

void
viewer_open(void)
{
	GtkWidget *w;
	GtkWidget *menu;
	GtkWidget *rw;
	GtkWidget *bw;
	GtkWidget *cw;
	GtkWidget *vscr;
	GtkObject *adj;
	NP2VIEW_T *view;
	int i;

	view = np2view;
	for (i = 0; i < NP2VIEW_MAX; i++, view++) {
		if (!view->alive) {
			char buf[256];
			viewcmn_caption(view, buf, sizeof(buf));

			view->alive = TRUE;
			view->lock = TRUE;

			w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
			view->window = w;
			gtk_window_set_title(GTK_WINDOW(w), buf);
			gtk_signal_connect(GTK_OBJECT(w), "destroy",
			    GTK_SIGNAL_FUNC(viewer_destroy), (gpointer)view);
			gtk_signal_connect(GTK_OBJECT(w), "focus_in_event",
			    GTK_SIGNAL_FUNC(viewer_focus_in), (gpointer)view);
			gtk_signal_connect(GTK_OBJECT(w), "focus_out_event",
			    GTK_SIGNAL_FUNC(viewer_focus_out), (gpointer)view);

			rw = gtk_vbox_new(FALSE, 0);
			gtk_widget_show(rw);
			gtk_container_add(GTK_CONTAINER(w), rw);

			menu = viewer_create_menu(view, w);
			gtk_widget_show(menu);
			gtk_box_pack_start(GTK_BOX(rw), menu, FALSE, TRUE, 0);

			bw = gtk_hbox_new(FALSE, 0);
			gtk_widget_show(bw);
			gtk_box_pack_start(GTK_BOX(rw), bw, TRUE, TRUE, 0);

			cw = gtk_drawing_area_new();
			view->widget = cw;
			gtk_widget_show(cw);
			gtk_box_pack_start(GTK_BOX(bw), cw, TRUE, TRUE, 0);
			gtk_signal_connect(GTK_OBJECT(cw), "expose_event",
			    GTK_SIGNAL_FUNC(viewer_expose), (gpointer)view);

			adj = gtk_adjustment_new(0.0, 0.0, 3.0, 1.0, 1.0, 1.0);
			gtk_signal_connect_object(adj, "changed",
			    GTK_SIGNAL_FUNC(viewer_changed), (gpointer)view);
			gtk_signal_connect_object(adj, "value_changed",
			    GTK_SIGNAL_FUNC(viewer_value_changed),
			    (gpointer)view);
			vscr = gtk_vscrollbar_new(GTK_ADJUSTMENT(adj));
			gtk_widget_show(vscr);
			gtk_box_pack_start(GTK_BOX(bw), vscr, FALSE, TRUE, 0);

			view_fontload(view, DEFAULT_FONTSIZE);

			viewcmn_setmode(view, NULL, VIEWMODE_REG);

			gtk_widget_realize(view->window);
			gtk_widget_show_all(view->window);

			view->lock = FALSE;
			break;
		}
	}
}

void
viewer_allclose(void)
{
	NP2VIEW_T *view;
	int i;

	view = np2view;
	for (i = 0; i < NP2VIEW_MAX; i++, view++) {
		if (view->alive) {
			gtk_widget_destroy(view->window);
		}
	}
}

void
viewer_allreload(BOOL force)
{
	NP2VIEW_T *view;
	UINT32 now;
	int i;

	view = np2view;
	now = gettick();
	for (i = 0; i < NP2VIEW_MAX; i++, view++) {
		if (view->alive && (force || (now - view->last) >= 200)) {
			view->last = now;
			if (!view->lock) {
				if (view->type == VIEWMODE_ASM) {
					view->seg = CPU_CS;
					view->off = CPU_IP;
					view->pos = 0;
					viewcmn_setvscroll(view);
				}
				viewcmn_setbank(view);
			}
			invalidate(view);
		}
	}
}

/* misc --------------------------------------------------------------------- */
static void
invalidate(NP2VIEW_T *view)
{
	GtkWidget *w;
	GdkRectangle r;

	w = (GtkWidget *)view->widget;
	r.x = r.y = 0;
	r.width = w->allocation.x + w->allocation.width;
	r.height = w->allocation.y + w->allocation.height;
	gtk_widget_draw(w, &r);
}

/* menu --------------------------------------------------------------------- */
#define	FIXUP_BITS		24
#define	FIXUP_GET_NUM(v)	(((v) >> FIXUP_BITS) & 0xff)

static void
_viewer_open(gpointer p, guint action, GtkWidget *w)
{
	UNUSED(p);
	UNUSED(action);
	UNUSED(w);

	viewer_open();
}

static void
_viewer_close(gpointer p, guint action, GtkWidget *w)
{
	BYTE num;

	UNUSED(p);
	UNUSED(w);

	num = FIXUP_GET_NUM(action);
	if (num < NP2VIEW_MAX) {
		gtk_widget_destroy(np2view[num].window);
	}
}

static void
_viewer_allclose(gpointer p, guint action, GtkWidget *w)
{

	UNUSED(p);
	UNUSED(action);
	UNUSED(w);

	viewer_allclose();
}

static void
change_mode(gpointer p, guint action, GtkWidget *w)
{
	BYTE num;

	UNUSED(p);
	UNUSED(w);

	switch (action & 0xff) {
	case VIEWMODE_REG:
	case VIEWMODE_SEG:
	case VIEWMODE_1MB:
	case VIEWMODE_ASM:
	case VIEWMODE_SND:
		num = FIXUP_GET_NUM(action);
		if (num < NP2VIEW_MAX) {
			viewer_segmode(&np2view[num], action);
		}
		break;
	}
}

static void
toggle(gpointer p, guint action, GtkWidget *w)
{
	NP2VIEW_T *view;
	BYTE num;

	UNUSED(p);
	UNUSED(w);

	num = FIXUP_GET_NUM(action);
	if (num >= NP2VIEW_MAX)
		return;

	view = &np2view[num];
	switch (action & 0xff) {
	case 0:
		/* Lock */
		view->lock = !view->lock;
		viewcmn_putcaption(view);
		invalidate(view);
		break;
	}
}

#define	f(f)	((GtkItemFactoryCallback)(f))
static GtkItemFactoryEntry menu_items[] = {
{ "/_System",			NULL, NULL,                0, "<Branch>"      },
{ "/System/_New window",	NULL, f(_viewer_open),     0, NULL            },
{ "/System/sep",		NULL, NULL,                0, "<Separator>"   },
{ "/System/_Close",		NULL, f(_viewer_close),    0, NULL            },
{ "/System/_All Close",		NULL, f(_viewer_allclose), 0, NULL            },
{ "/_Mode/",			NULL, NULL,                0, "<Branch>"      },
{ "/_Mode/_Register",		NULL, f(change_mode),      VIEWMODE_REG, "<RadioItem>"},
{ "/_Mode/_Data segment",	NULL, f(change_mode),      VIEWMODE_SEG, "/Mode/Register"},
{ "/_Mode/_Main memory dump",	NULL, f(change_mode),      VIEWMODE_1MB, "/Mode/Data segment"},
{ "/_Mode/Dis_assemble",	NULL, f(change_mode),      VIEWMODE_ASM, "/Mode/Main memory dump"},
{ "/_Mode/_Sound register",	NULL, f(change_mode),      VIEWMODE_SND, "/Mode/Disassemble"},
{ "/Mode/sep",			NULL, NULL,                0, "<Separator>"   },
{ "/Mode/_Lock",		NULL, f(toggle),           0, "<ToggleItem>"  },
};
#undef	f

static const int fixup_index[] = {
	3,
	6,
	7,
	8,
	9,
	10,
	12,
};

static GtkWidget *
viewer_create_menu(NP2VIEW_T *view, GtkWidget *parent)
{
	GtkAccelGroup *accel_group;
	MENU_HDL hdl;
	int num;
	int i;

	hdl = (MENU_HDL)malloc(sizeof(_MENU_HDL));
	if (hdl == NULL) {
		fprintf(stderr, "Couldn't create viewer menu\n");
		return NULL;
	}
	memset(hdl, 0, sizeof(_MENU_HDL));
	view->menu = hdl;

	/* XXX: FIXME!!! start */
	num = (view->index & 0xff) << FIXUP_BITS;
	for (i = 0; i < NELEMENTS(fixup_index); i++) {
		menu_items[fixup_index[i]].callback_action &= 0x000000ff;
		menu_items[fixup_index[i]].callback_action |= num;
	}
	/* XXX: FIXME!!! end */

	accel_group = gtk_accel_group_new();
	hdl->item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR,
	    "<main>", accel_group);
	gtk_item_factory_create_items(hdl->item_factory, NELEMENTS(menu_items),
	    menu_items, NULL);
	gtk_accel_group_attach(accel_group, GTK_OBJECT(parent));

	return gtk_item_factory_get_widget(hdl->item_factory, "<main>");
}

#endif	/* SUPPORT_VIEWER */

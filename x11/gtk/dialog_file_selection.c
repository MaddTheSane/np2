/*
 * Copyright (c) 2002-2003 NONAKA Kimihiro
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

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "gtk/xnp2.h"
#include "gtk/gtk_menu.h"

#include "np2.h"
#include "dosio.h"
#include "pccore.h"
#include "iocore.h"

#include "diskdrv.h"
#include "font.h"
#include "ini.h"

#include "sysmng.h"


typedef struct {
	GtkWidget *w;
	void *arg;
	BOOL result;

	BOOL (*callback_ok)(void *arg, const char *path);
	void (*callback_destroy)(void *arg, BOOL result);
} file_selection_t;


static void
file_selection_ok(GtkButton *b, gpointer p)
{
	file_selection_t *fsp = (file_selection_t *)p;
	const char *path;
	BOOL result = FALSE;

	UNUSED(b);

	path = gtk_file_selection_get_filename(GTK_FILE_SELECTION(fsp->w));
	if (path != NULL) {
		if (fsp->callback_ok) {
			result = (*fsp->callback_ok)(fsp->arg, path);
		}
	}
	fsp->result = result;

	gtk_widget_destroy(GTK_WIDGET(fsp->w));
}

static void
dialog_destroy(GtkWidget *w, gpointer p)
{
	file_selection_t *fsp = (file_selection_t *)p;
	GtkWidget *fs = fsp->w;
	BOOL result = fsp->result;

	UNUSED(w);

	if (fsp->callback_destroy) {
		(*fsp->callback_destroy)(fsp->arg, result);
	}

	_MFREE(fsp);
	install_idle_process();
	gtk_widget_destroy(fs);
}

void
create_file_selection(const char *title, const char *defstr, void *arg,
                         file_selection_ok_callback *ok_cb,
			 file_selection_destrroy_callback *destroy_cb)
{
	GtkWidget *file_dialog;
	file_selection_t *fsp;

	fsp = _MALLOC(sizeof(*fsp), "file selection dialog param");
	if (fsp == NULL) {
		return;
	}
	memset(fsp, 0, sizeof(*fsp));

	uninstall_idle_process();

	file_dialog = gtk_file_selection_new(title);
	if (defstr != NULL) {
		gtk_file_selection_set_filename(
		    GTK_FILE_SELECTION(file_dialog), defstr);
	}
	gtk_window_set_position(GTK_WINDOW(file_dialog), GTK_WIN_POS_CENTER);
	gtk_window_set_modal(GTK_WINDOW(file_dialog), TRUE);
	gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(file_dialog));
	gtk_signal_connect(GTK_OBJECT(file_dialog),
	    "destroy", GTK_SIGNAL_FUNC(dialog_destroy), (gpointer)fsp);
	gtk_signal_connect(
	    GTK_OBJECT(GTK_FILE_SELECTION(file_dialog)->ok_button),
	    "clicked", GTK_SIGNAL_FUNC(file_selection_ok), (gpointer)fsp);
	gtk_signal_connect_object(
	    GTK_OBJECT(GTK_FILE_SELECTION(file_dialog)->cancel_button),
	    "clicked", GTK_SIGNAL_FUNC(gtk_widget_destroy),
	    GTK_OBJECT(file_dialog));

	fsp->w = file_dialog;
	fsp->arg = arg;
	fsp->result = FALSE;
	fsp->callback_ok = ok_cb;
	fsp->callback_destroy = destroy_cb;

	gtk_widget_show(file_dialog);
}

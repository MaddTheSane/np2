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

#include "gtk/xnp2.h"
#include "gtk/gtkmenu.h"

#include "np2.h"
#include "dosio.h"
#include "ini.h"

#include "fddfile.h"
#include "newdisk.h"

#define	DISKNAME_LEN	16

typedef struct {
	char      filename[MAX_PATH];
	GtkWidget *dialog;
} newdisk_common_t;

static void
dialog_destroy(GtkWidget *w, gpointer p)
{

	if (p) {
		_MFREE(p);
	}

	install_idle_process();
	gtk_widget_destroy(w);
}


/*
 * create floppy disk image
 */

static const struct {
	const char *str;
	BYTE fdtype;
} disktype[] = {
	{ "2DD",   DISKTYPE_2DD << 4      },
	{ "2HD",   DISKTYPE_2HD << 4      },
	{ "1.44", (DISKTYPE_2HD << 4) + 1 },
};

static BYTE makefdtype = DISKTYPE_2HD << 4;

typedef struct {
	newdisk_common_t com;

	GtkWidget        *diskname_entry;
} newdisk_fd_t;

static void
newdisk_fd_disktype_button_clicked(GtkButton *b, gpointer d)
{

	UNUSED(b);
	makefdtype = (BYTE)(guint)d;
}

static void
newdisk_fd_ok_button_clicked(GtkButton *b, gpointer d)
{
	newdisk_fd_t *data = (newdisk_fd_t *)d;
	gchar *p;

	UNUSED(b);

	p = gtk_entry_get_text(GTK_ENTRY(data->diskname_entry));
	if (p != 0) {
		newdisk_fdd(data->com.filename, makefdtype, p);
	}
	gtk_widget_destroy(GTK_WIDGET(data->com.dialog));
}

static void
newdisk_fd(newdisk_fd_t *datap)
{
	GtkWidget *dialog;
	GtkWidget *dialog_table;
	GtkWidget *label;
	GtkWidget *entry;
	GtkWidget *hbox;
	GtkWidget *button[NELEMENTS(disktype)];
	GtkWidget *ok_button;
	GtkWidget *cancel_button;
	GSList *gslist;
	int i;

	uninstall_idle_process();

	/* dialog */
	datap->com.dialog = dialog = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_title(GTK_WINDOW(dialog),"Create new floppy disk image");
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	gtk_window_set_policy(GTK_WINDOW(dialog), TRUE, TRUE, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);

	gtk_signal_connect(GTK_OBJECT(dialog), "destroy",
	    GTK_SIGNAL_FUNC(dialog_destroy), (gpointer)datap);

	/* dialog table */
	dialog_table = gtk_table_new(2, 3, FALSE);
	gtk_container_add(GTK_CONTAINER(dialog), dialog_table);
	gtk_table_set_col_spacings(GTK_TABLE(dialog_table), 5);
	gtk_widget_show(dialog_table);

	/* "Disk name" label */
	label = gtk_label_new(" Disk name");
	gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
	gtk_table_attach_defaults(GTK_TABLE(dialog_table), label, 0, 1, 0, 1);
	gtk_widget_show(label);

	/* "Disk name" text entry */
	datap->diskname_entry = entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), DISKNAME_LEN);
	gtk_entry_set_editable(GTK_ENTRY(entry), TRUE);
	gtk_table_attach_defaults(GTK_TABLE(dialog_table), entry, 1, 2, 0, 1);
	gtk_widget_show(entry);

	/* "Disk type" label */
	label = gtk_label_new(" Disk type");
	gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
	gtk_table_attach_defaults(GTK_TABLE(dialog_table), label, 0, 1, 1, 2);
	gtk_widget_show(label);

	/* "o 2DD  o 2HD  o 1.44" radio button */
	hbox = gtk_hbox_new(FALSE, 3);
	gtk_table_attach_defaults(GTK_TABLE(dialog_table), hbox, 1, 2, 1, 2);
	gtk_widget_show(hbox);

	for (gslist = 0, i = 0; i < NELEMENTS(disktype); ++i) {
		button[i] = gtk_radio_button_new_with_label(gslist,
		    disktype[i].str);
		GTK_WIDGET_UNSET_FLAGS(button[i], GTK_CAN_FOCUS);
		gslist = gtk_radio_button_group(GTK_RADIO_BUTTON(button[i]));
		gtk_box_pack_start(GTK_BOX(hbox), button[i], FALSE, FALSE, 1);
		gtk_signal_connect(GTK_OBJECT(button[i]), "clicked",
		    GTK_SIGNAL_FUNC(newdisk_fd_disktype_button_clicked),
		    (gpointer)(guint)disktype[i].fdtype);
		gtk_widget_show(button[i]);
	}
	for (i = 0; i < NELEMENTS(disktype); ++i) {
		if (disktype[i].fdtype == makefdtype)
			break;
	}
	if (i == NELEMENTS(disktype)) {
		i = (i <= 1) ? 0 : 1;	/* 2HD */
	}
	gtk_signal_emit_by_name(GTK_OBJECT(button[i]), "clicked");

	/* "OK" button */
	ok_button = gtk_button_new_with_label("OK");
	gtk_container_set_border_width(GTK_CONTAINER(ok_button), 2);
	gtk_widget_set_usize(ok_button, 80, 0);
	gtk_table_attach_defaults(GTK_TABLE(dialog_table), ok_button,
	    2, 3, 0, 1);
	gtk_signal_connect(GTK_OBJECT(ok_button), "clicked",
	    GTK_SIGNAL_FUNC(newdisk_fd_ok_button_clicked), (gpointer)datap);
	GTK_WIDGET_SET_FLAGS(ok_button, GTK_CAN_DEFAULT);
	GTK_WIDGET_SET_FLAGS(ok_button, GTK_HAS_DEFAULT);
	gtk_widget_grab_default(ok_button);
	gtk_widget_show(ok_button);

	/* "Cancel" button */
	cancel_button = gtk_button_new_with_label("Cancel");
	gtk_container_set_border_width(GTK_CONTAINER(cancel_button), 4);
	gtk_widget_set_usize(cancel_button, 80, 0);
	gtk_table_attach_defaults(GTK_TABLE(dialog_table), cancel_button,
	    2, 3, 1, 2);
	gtk_signal_connect_object(GTK_OBJECT(cancel_button), "clicked",
	    GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(dialog));
	GTK_WIDGET_SET_FLAGS(cancel_button, GTK_CAN_DEFAULT);
	gtk_widget_show(cancel_button);

	gtk_widget_show(dialog);
}


/*
 * create hard disk image
 */

typedef struct {
	newdisk_common_t com;

	GtkWidget        *hdsize_combo;
} newdisk_hd_t;

static void
newdisk_hd_ok_button_clicked(GtkButton *b, gpointer d)
{
	newdisk_hd_t *data = (newdisk_hd_t *)d;
	gchar *p;
	int hdsize;

	UNUSED(b);

	p = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(data->hdsize_combo)->entry));
	if (p != 0 && strlen(p) != 0) {
		hdsize = milstr_solveINT(p);
		if (hdsize >= 5 && hdsize <= 256) {
			newdisk_hdd(data->com.filename, hdsize);
		}
	}
	gtk_widget_destroy(GTK_WIDGET(data->com.dialog));
}

static void
newdisk_hd(newdisk_hd_t *datap)
{
	static const char *hddsizestr[] = {
		"20", "41", "65", "80", "128",
	};
	GtkWidget *dialog;
	GtkWidget *dialog_table;
	GtkWidget *label;
	GtkWidget *hbox;
	GtkWidget *combo;
	GtkWidget *entry;
	GtkWidget *ok_button;
	GtkWidget *cancel_button;
	GList *items;
	int i;

	uninstall_idle_process();

	/* dialog */
	datap->com.dialog = dialog = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_window_set_title(GTK_WINDOW(dialog), "Create new hard disk image");
	gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	gtk_window_set_policy(GTK_WINDOW(dialog), TRUE, TRUE, TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);

	gtk_signal_connect(GTK_OBJECT(dialog), "destroy",
	    GTK_SIGNAL_FUNC(dialog_destroy), (gpointer)datap);

	/* dialog table */
	dialog_table = gtk_table_new(2, 3, FALSE);
	gtk_table_set_col_spacings(GTK_TABLE(dialog_table), 5);
	gtk_container_add(GTK_CONTAINER(dialog), dialog_table);
	gtk_widget_show(dialog_table);

	/* "HDD size" label */
	label = gtk_label_new(" HDD size");
	gtk_misc_set_alignment(GTK_MISC(label), 0.5, 0.5);
	gtk_table_attach_defaults(GTK_TABLE(dialog_table), label, 0, 1, 0, 1);
	gtk_widget_show(label);

	/* HDD size */
	hbox = gtk_hbox_new(FALSE, 2);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 0);
	gtk_table_attach_defaults(GTK_TABLE(dialog_table), hbox, 1, 2, 0, 1);
	gtk_widget_show(hbox);

	datap->hdsize_combo = combo = gtk_combo_new();
	gtk_combo_set_value_in_list(GTK_COMBO(combo), TRUE, TRUE);
	gtk_combo_set_use_arrows_always(GTK_COMBO(combo), TRUE);
	for (items = 0, i = 0; i < NELEMENTS(hddsizestr); ++i)
		items = g_list_append(items, (gpointer)hddsizestr[i]);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo), items);
	g_list_free(items);
	gtk_widget_set_usize(combo, 60, 0);
	gtk_box_pack_start(GTK_BOX(hbox), combo, FALSE, FALSE, 5);
	gtk_widget_show(combo);

	entry = GTK_COMBO(combo)->entry;
	gtk_entry_set_editable(GTK_ENTRY(entry), TRUE);
	gtk_entry_set_max_length(GTK_ENTRY(entry), 3);
	gtk_entry_set_text(GTK_ENTRY(entry), "");
	gtk_widget_show(entry);

	/* "MB" label */
	label = gtk_label_new("MB");
	gtk_misc_set_alignment(GTK_MISC(label), 0.1, 0.5);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);

	/* "(5-256MB)" label */
	label = gtk_label_new("(5-256MB)");
	gtk_misc_set_alignment(GTK_MISC(label), 0.9, 0.5);
	gtk_table_attach_defaults(GTK_TABLE(dialog_table), label, 1, 2, 1, 2);
	gtk_widget_show(label);

	/* "OK" button */
	ok_button = gtk_button_new_with_label("OK");
	gtk_container_set_border_width(GTK_CONTAINER(ok_button), 2);
	gtk_widget_set_usize(ok_button, 80, 0);
	gtk_table_attach_defaults(GTK_TABLE(dialog_table), ok_button,
	    2, 3, 0, 1);
	gtk_signal_connect(GTK_OBJECT(ok_button), "clicked",
	    GTK_SIGNAL_FUNC(newdisk_hd_ok_button_clicked), (gpointer)datap);
	GTK_WIDGET_SET_FLAGS(ok_button, GTK_CAN_DEFAULT);
	GTK_WIDGET_SET_FLAGS(ok_button, GTK_HAS_DEFAULT);
	gtk_widget_grab_default(ok_button);
	gtk_widget_show(ok_button);

	/* "Cancel" button */
	cancel_button = gtk_button_new_with_label("Cancel");
	gtk_container_set_border_width(GTK_CONTAINER(cancel_button), 4);
	gtk_widget_set_usize(cancel_button, 80, 0);
	gtk_table_attach_defaults(GTK_TABLE(dialog_table), cancel_button,
	    2, 3, 1, 2);
	gtk_signal_connect_object(GTK_OBJECT(cancel_button), "clicked",
	    GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(dialog));
	GTK_WIDGET_SET_FLAGS(cancel_button, GTK_CAN_DEFAULT);
	gtk_widget_show(cancel_button);

	gtk_widget_show(dialog);
}

/*
 * newdisk
 */
typedef union {
	newdisk_common_t com;
	newdisk_fd_t     fd;
	newdisk_hd_t     hd;
} newdisk_t;

void
create_newdisk_dialog(const char *filebasename, const char *fileextname)
{
	newdisk_t *disk;
	char *extName;
	size_t len;

	disk = _MALLOC(sizeof(*disk), "newdisk work");
	if (disk == NULL) {
		fprintf(stderr, "create_newdisk_dialog: can't alloc memory.\n");
		return;
	}
	memset(disk, 0, sizeof(*disk));

	milstr_ncpy(disk->com.filename,filebasename,sizeof(disk->com.filename));

	extName = file_getext(disk->com.filename);
	if (milstr_extendcmp(fileextname, "thd") == 0) {
		if ((milstr_extendcmp(extName, disk->com.filename) == 0) ||
		    (milstr_extendcmp(extName, "thd") != 0)) {
			len = strlen(disk->com.filename);
			if (disk->com.filename[len - 1] != '.')
				milstr_ncat(disk->com.filename, ".",
				    sizeof(disk->com.filename));
			milstr_ncat(disk->com.filename, fileextname,
			    sizeof(disk->com.filename));
		}
		newdisk_hd(&disk->hd);
	} else if ((milstr_extendcmp(fileextname, "d88") == 0) ||
	           (milstr_extendcmp(fileextname, "88d") == 0)) {
		if ((milstr_extendcmp(extName, disk->com.filename) == 0) ||
		    ((milstr_extendcmp(extName, "d88") != 0) && (milstr_extendcmp(extName, "88d") != 0))) {
			len = strlen(disk->com.filename);
			if (disk->com.filename[len - 1] != '.')
				milstr_ncat(disk->com.filename, ".",
				    sizeof(disk->com.filename));
			milstr_ncat(disk->com.filename, fileextname,
			    sizeof(disk->com.filename));
		}
		newdisk_fd(&disk->fd);
	}
}

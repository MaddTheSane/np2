/*	$Id: dialog_sound.c,v 1.1 2004/07/15 14:24:33 monaka Exp $	*/

/*
 * Copyright (c) 2002, 2003, 2004
 *    NONAKA Kimihiro (aw9k-nnk@asahi-net.or.jp) All rights reserved.
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

#include "gtk2/xnp2.h"
#include "gtk2/gtk_menu.h"

#include "np2.h"
#include "dosio.h"
#include "ini.h"
#include "opngen.h"
#include "pccore.h"
#include "iocore.h"

#include "soundmng.h"
#include "sysmng.h"


/*
 * Mixer
 */
static const struct {
	const char	*name;
	UINT8		*valp;
	gfloat		min;
	gfloat		max;
} mixer_vol_tbl[] = {
	{ "FM",     &np2cfg.vol_fm,     0.0, 128.0 },
	{ "PSG",    &np2cfg.vol_ssg,    0.0, 128.0 },
	{ "ADPCM",  &np2cfg.vol_adpcm,  0.0, 128.0 },
	{ "PCM",    &np2cfg.vol_pcm,    0.0, 128.0 },
	{ "Rhythm", &np2cfg.vol_rhythm, 0.0, 128.0 },
};

static GtkObject *mixer_adj[NELEMENTS(mixer_vol_tbl)];


/*
 * PC-9801-14
 */

static const char *snd14_vol_str[] = {
	"left", "right", "f2", "f4", "f8", "f16"
};

static GtkObject *snd14_adj[NELEMENTS(snd14_vol_str)];


/*
 * PC-9801-26
 */

#define	SND26_SHIFT_IOPORT	4
#define	SND26_SHIFT_INTR	6
#define	SND26_SHIFT_ROMADDR	0

#define	SND26_MASK_IOPORT	0x10
#define	SND26_MASK_INTR		0xc0
#define	SND26_MASK_ROMADDR	0x07

#define	SND26_GET_IOPORT() \
	((np2cfg.snd26opt & SND26_MASK_IOPORT) >> SND26_SHIFT_IOPORT)
#define	SND26_GET_INTR() \
	((np2cfg.snd26opt & SND26_MASK_INTR) >> SND26_SHIFT_INTR)
#define	SND26_GET_ROMADDR()	snd26_get_romaddr()

#define	SND26_SET_IOPORT(v) \
	    (((v) << SND26_SHIFT_IOPORT) & SND26_MASK_IOPORT)
#define	SND26_SET_INTR(v) \
	    (((v) << SND26_SHIFT_INTR) & SND26_MASK_INTR)
#define	SND26_SET_ROMADDR(v)	snd26_set_romaddr(v)

static int
snd26_get_romaddr(void)
{
	int idx;

	idx = ((np2cfg.snd26opt & SND26_MASK_ROMADDR) >> SND26_SHIFT_ROMADDR);
	if (idx < 4)
		return idx;
	return 4;
}

static int
snd26_set_romaddr(int idx)
{

	if (idx < 4)
		return ((idx << SND26_SHIFT_ROMADDR) & SND26_MASK_ROMADDR);
	return ((4 << SND26_SHIFT_ROMADDR) & SND26_MASK_ROMADDR);
}

static const char *snd26_ioport_str[] = {
	"0288", "0188"
};

static const char *snd26_intr_str[] = {
	"INT0", "INT6", "INT4", "INT5"
};

static const char *snd26_romaddr_str[] = {
	"C8000", "CC000", "D0000", "D4000", "N/C"
};

static GtkWidget *snd26_ioport_entry;
static GtkWidget *snd26_int_entry;
static GtkWidget *snd26_romaddr_entry;


/*
 * PC-9801-86
 */

#define	SND86_SHIFT_IOPORT	0
#define	SND86_SHIFT_BIOSROM	1
#define	SND86_SHIFT_INTR	2
#define	SND86_SHIFT_INTERRUPT	4
#define	SND86_SHIFT_SOUNDID	5

#define	SND86_MASK_IOPORT	0x01
#define	SND86_MASK_BIOSROM	0x02
#define	SND86_MASK_INTR		0x0c
#define	SND86_MASK_INTERRUPT	0x10
#define	SND86_MASK_SOUNDID	0xe0

#define	SND86_GET_IOPORT() \
	((np2cfg.snd86opt & SND86_MASK_IOPORT) >> SND86_SHIFT_IOPORT)
#define	SND86_GET_BIOSROM() \
	((np2cfg.snd86opt & SND86_MASK_BIOSROM) >> SND86_SHIFT_BIOSROM)
#define	SND86_GET_INTR() \
	((np2cfg.snd86opt & SND86_MASK_INTR) >> SND86_SHIFT_INTR)
#define	SND86_GET_INTERRUPT() \
	((np2cfg.snd86opt & SND86_MASK_INTERRUPT) >> SND86_SHIFT_INTERRUPT)
#define	SND86_GET_SOUNDID() \
	((np2cfg.snd86opt & SND86_MASK_SOUNDID) >> SND86_SHIFT_SOUNDID)

#define	SND86_SET_IOPORT(v) \
	    (((v) << SND86_SHIFT_IOPORT) & SND86_MASK_IOPORT)
#define	SND86_SET_BIOSROM(v) \
	    (((v) << SND86_SHIFT_BIOSROM) & SND86_MASK_BIOSROM)
#define	SND86_SET_INTR(v) \
	    (((v) << SND86_SHIFT_INTR) & SND86_MASK_INTR)
#define	SND86_SET_INTERRUPT(v) \
	    (((v) << SND86_SHIFT_INTERRUPT) & SND86_MASK_INTERRUPT)
#define	SND86_SET_SOUNDID(v) \
	    (((v) << SND86_SHIFT_SOUNDID) & SND86_MASK_SOUNDID)

static const char *snd86_ioport_str[] = {
	"0288", "0188",
};

static const char *snd86_intr_str[] = {
	"INT0", "INT4", "INT6", "INT5",
};

static const char *snd86_soundid_str[] = {
	"7x", "6x", "5x", "4x", "3x", "2x", "1x", "0x",
};

static GtkWidget *snd86_ioport_entry;
static GtkWidget *snd86_int_entry;
static GtkWidget *snd86_soundid_entry;
static GtkWidget *snd86_int_checkbutton;
static GtkWidget *snd86_rom_checkbutton;


/*
 * Speak board
 */

#define	SPB_SHIFT_IOPORT	SND26_SHIFT_IOPORT
#define	SPB_SHIFT_INTR		SND26_SHIFT_INTR
#define	SPB_SHIFT_ROMADDR	SND26_SHIFT_ROMADDR

#define	SPB_MASK_IOPORT		SND26_MASK_IOPORT
#define	SPB_MASK_INTR		SND26_MASK_INTR	
#define	SPB_MASK_ROMADDR	SND26_MASK_ROMADDR

#define	SPB_GET_IOPORT()	SND26_GET_IOPORT()
#define	SPB_GET_INTR()		SND26_GET_INTR()
#define	SPB_GET_ROMADDR()	SND26_GET_ROMADDR()

#define	SPB_SET_IOPORT(v)	SND26_SET_IOPORT(v)
#define	SPB_SET_INTR(v)		SND26_SET_INTR(v)
#define	SPB_SET_ROMADDR(v)	SND26_SET_ROMADDR(v)

static const char *spb_ioport_str[] = {
	"0088", "0188"
};

#define	spb_intr_str	snd26_intr_str
#define	spb_romaddr_str	snd26_romaddr_str

static const char *spb_vr_channel_str[] = {
	"L", "R"
};

static GtkWidget *spb_ioport_entry;
static GtkWidget *spb_int_entry;
static GtkWidget *spb_romaddr_entry;
static GtkWidget *spb_vr_channel_checkbutton[2];
static GtkWidget *spb_reverse_channel_checkbutton;
static GtkObject *spb_vr_level_adj;


/*
 * Driver
 */

static const char *driver_name[SNDDRV_DRVMAX] = {
	"None",
	"NetBSD",
	"OSS",
	"EsounD",
	"SDL",
};

static GtkWidget *driver_audio_device_entry;
static int driver_snddrv;


static void
ok_button_clicked(GtkButton *b, gpointer d)
{
	/* mixer */
	guint mixer_vol[NELEMENTS(mixer_vol_tbl)];

	/* PC-9801-14 */
	guint snd14_vol14[NELEMENTS(snd14_vol_str)];

	/* PC-9801-26 */
	const gchar *snd26_ioport;
	const gchar *snd26_intr;
	const gchar *snd26_romaddr;
	UINT8 snd26opt, snd26opt_mask;

	/* PC-9801-86 */
	const gchar *snd86_ioport;
	const gchar *snd86_intr;
	const gchar *snd86_soundid;
	gint snd86_interrupt;
	gint snd86_biosrom;
	UINT8 snd86opt, snd86opt_mask;

	/* Speak board */
	const gchar *spb_ioport;
	const gchar *spb_intr;
	const gchar *spb_romaddr;
	UINT8 spb_vrc;
	UINT8 spb_vrl;
	UINT8 spb_x;
	UINT8 spbopt, spbopt_mask;

	/* Driver */
	const gchar *driver_audiodevp;

	/* common */
	int i;
	BOOL renewal;

	UNUSED(b);

	/* Mixer */
	renewal = FALSE;
	for (i = 0; i < NELEMENTS(mixer_vol_tbl); i++) {
		mixer_vol[i] = (guint)(GTK_ADJUSTMENT(mixer_adj[i])->value);
		if (*mixer_vol_tbl[i].valp != mixer_vol[i]) {
			*mixer_vol_tbl[i].valp = mixer_vol[i];
			renewal = TRUE;
		}
	}

	if (renewal) {
		sysmng_update(SYS_UPDATECFG);
	}

	/* PC-9801-14 */
	renewal = FALSE;
	for (i = 0; i < NELEMENTS(snd14_vol_str); i++) {
		snd14_vol14[i] = (guint)(GTK_ADJUSTMENT(snd14_adj[i])->value);
		if (np2cfg.vol14[i] != snd14_vol14[i]) {
			np2cfg.vol14[i] = snd14_vol14[i];
			renewal = TRUE;
		}
	}

	if (renewal) {
		sysmng_update(SYS_UPDATECFG);
	}

	/* PC-9801-26 */
	snd26_ioport = gtk_entry_get_text(GTK_ENTRY(snd26_ioport_entry));
	snd26_intr = gtk_entry_get_text(GTK_ENTRY(snd26_int_entry));
	snd26_romaddr = gtk_entry_get_text(GTK_ENTRY(snd26_romaddr_entry));

	renewal = FALSE;
	snd26opt = snd26opt_mask = 0;
	for (i = 0; i < NELEMENTS(snd26_ioport_str); i++) {
		if (strcmp(snd26_ioport, snd26_ioport_str[i]) == 0) {
			if (SND26_GET_IOPORT() != i) {
				snd86opt |= SND26_SET_IOPORT(i);
				snd86opt_mask |= SND26_MASK_IOPORT;
				renewal = TRUE;
			}
			break;
		}
	}
	for (i = 0; i < NELEMENTS(snd26_intr_str); i++) {
		if (strcmp(snd26_intr, snd26_intr_str[i]) == 0) {
			if (SND26_GET_INTR() != i) {
				snd26opt |= SND26_SET_INTR(i);
				snd26opt_mask |= SND26_MASK_INTR;
				renewal = TRUE;
			}
			break;
		}
	}
	for (i = 0; i < NELEMENTS(snd26_romaddr_str); i++) {
		if (strcmp(snd26_romaddr, snd26_romaddr_str[i]) == 0) {
			if (SND26_GET_ROMADDR() != i) {
				snd26opt |= SND26_SET_ROMADDR(i);
				snd26opt_mask |= SND26_MASK_ROMADDR;
				renewal = TRUE;
			}
			break;
		}
	}

	if (renewal) {
		np2cfg.snd26opt &= ~snd26opt_mask;
		np2cfg.snd26opt |= snd26opt;
		sysmng_update(SYS_UPDATECFG);
	}

	/* PC-9801-86 */
	snd86_ioport = gtk_entry_get_text(GTK_ENTRY(snd86_ioport_entry));
	snd86_intr = gtk_entry_get_text(GTK_ENTRY(snd86_int_entry));
	snd86_soundid = gtk_entry_get_text(GTK_ENTRY(snd86_soundid_entry));
	snd86_interrupt = GTK_TOGGLE_BUTTON(snd86_int_checkbutton)->active;
	snd86_biosrom = GTK_TOGGLE_BUTTON(snd86_rom_checkbutton)->active;

	renewal = FALSE;
	snd86opt = snd86opt_mask = 0;
	for (i = 0; i < NELEMENTS(snd86_ioport_str); i++) {
		if (strcmp(snd86_ioport, snd86_ioport_str[i]) == 0) {
			if (SND86_GET_IOPORT() != i) {
				snd86opt |= SND86_SET_IOPORT(i);
				snd86opt_mask |= SND86_MASK_IOPORT;
				renewal = TRUE;
			}
			break;
		}
	}
	for (i = 0; i < NELEMENTS(snd86_intr_str); i++) {
		if (strcmp(snd86_intr, snd86_intr_str[i]) == 0) {
			if (SND86_GET_INTR() != i) {
				snd86opt |= SND86_SET_INTR(i);
				snd86opt_mask |= SND86_MASK_INTR;
				renewal = TRUE;
			}
			break;
		}
	}
	for (i = 0; i < NELEMENTS(snd86_soundid_str); i++) {
		if (strcmp(snd86_soundid, snd86_soundid_str[i]) == 0) {
			if (SND86_GET_SOUNDID() != i) {
				snd86opt |= SND86_SET_SOUNDID(i);
				snd86opt_mask |= SND86_MASK_SOUNDID;
				renewal = TRUE;
			}
			break;
		}
	}
	if (SND86_GET_INTERRUPT() != snd86_interrupt) {
		snd86opt |= SND86_SET_INTERRUPT(i);
		snd86opt_mask |= SND86_MASK_INTERRUPT;
		renewal = TRUE;
	}
	if (SND86_GET_BIOSROM() != snd86_biosrom) {
		snd86opt |= SND86_SET_BIOSROM(i);
		snd86opt_mask |= SND86_MASK_BIOSROM;
		renewal = TRUE;
	}

	if (renewal) {
		np2cfg.snd86opt &= ~snd86opt_mask;
		np2cfg.snd86opt |= snd86opt;
		sysmng_update(SYS_UPDATECFG);
	}

	/* Speak board */
	spb_ioport = gtk_entry_get_text(GTK_ENTRY(spb_ioport_entry));
	spb_intr = gtk_entry_get_text(GTK_ENTRY(spb_int_entry));
	spb_romaddr = gtk_entry_get_text(GTK_ENTRY(spb_romaddr_entry));
	spb_vrl = (UINT8)(GTK_ADJUSTMENT(spb_vr_level_adj)->value);
	spb_x = GTK_TOGGLE_BUTTON(spb_reverse_channel_checkbutton)->active;

	renewal = FALSE;
	spbopt = spbopt_mask = 0;
	for (i = 0; i < NELEMENTS(spb_ioport_str); i++) {
		if (strcmp(spb_ioport, spb_ioport_str[i]) == 0) {
			if (SPB_GET_IOPORT() != i) {
				snd86opt |= SPB_SET_IOPORT(i);
				snd86opt_mask |= SPB_MASK_IOPORT;
				renewal = TRUE;
			}
			break;
		}
	}
	for (i = 0; i < NELEMENTS(spb_intr_str); i++) {
		if (strcmp(spb_intr, spb_intr_str[i]) == 0) {
			if (SPB_GET_INTR() != i) {
				spbopt |= SPB_SET_INTR(i);
				spbopt_mask |= SPB_MASK_INTR;
				renewal = TRUE;
			}
			break;
		}
	}
	for (i = 0; i < NELEMENTS(spb_romaddr_str); i++) {
		if (strcmp(spb_romaddr, spb_romaddr_str[i]) == 0) {
			if (SPB_GET_ROMADDR() != i) {
				spbopt |= SPB_SET_ROMADDR(i);
				spbopt_mask |= SPB_MASK_ROMADDR;
				renewal = TRUE;
			}
			break;
		}
	}
	spb_vrc = 0;
	for (i = 0; i < NELEMENTS(spb_vr_channel_str); i++) {
		spb_vrc |= GTK_TOGGLE_BUTTON(spb_vr_channel_checkbutton[i])->active ? (1 << i) : 0;
	}
	if (np2cfg.spb_vrc != spb_vrc) {
		np2cfg.spb_vrc = spb_vrc;
		renewal = TRUE;
	}
	if (np2cfg.spb_vrl != spb_vrl) {
		np2cfg.spb_vrl = spb_vrl;
		renewal = TRUE;
	}
	if (np2cfg.spb_x != spb_x) {
		np2cfg.spb_x = !np2cfg.spb_x;
		renewal = TRUE;
	}

	if (renewal) {
		np2cfg.spbopt &= ~spbopt_mask;
		np2cfg.spbopt |= spbopt;
		opngen_setVR(np2cfg.spb_vrc, np2cfg.spb_vrl);
		sysmng_update(SYS_UPDATEOSCFG);
	}

	/* Driver */
	driver_audiodevp = gtk_entry_get_text(GTK_ENTRY(driver_audio_device_entry));

	renewal = FALSE;
	if (np2oscfg.snddrv != driver_snddrv) {
		np2oscfg.snddrv = driver_snddrv;
		renewal = TRUE;
	}
	if (strcmp(np2oscfg.audiodev, driver_audiodevp) != 0) {
		milstr_ncpy(np2oscfg.audiodev, driver_audiodevp, sizeof(np2oscfg.audiodev));
		renewal = TRUE;
	}

	if (renewal) {
		sysmng_update(SYS_UPDATEOSCFG);
		soundrenewal = 1;
	}

	gtk_widget_destroy((GtkWidget *)d);
}

static void
dialog_destroy(GtkWidget *w, GtkWidget **wp)
{

	UNUSED(wp);

	install_idle_process();
	gtk_widget_destroy(w);
}

static void
mixer_default_button_clicked(GtkButton *b, gpointer d)
{
	int i;

	UNUSED(b);
	UNUSED(d);

	for (i = 0; i < NELEMENTS(mixer_vol_tbl); i++) {
		gtk_adjustment_set_value(GTK_ADJUSTMENT(mixer_adj[i]), 64.0);
	}
}

static void
snd14_default_button_clicked(GtkButton *b, gpointer d)
{
	static const gfloat defval[NELEMENTS(snd14_adj)] = {
		12.0, 12.0, 8.0, 6.0, 3.0, 12.0
	};
	int i;

	UNUSED(b);
	UNUSED(d);

	for (i = 0; i < NELEMENTS(snd14_adj); i++) {
		gtk_adjustment_set_value(GTK_ADJUSTMENT(snd14_adj[i]), defval[i]);
	}
}

static void
snd26_default_button_clicked(GtkButton *b, gpointer d)
{

	UNUSED(b);
	UNUSED(d);

	gtk_entry_set_text(GTK_ENTRY(snd26_ioport_entry), "0188");
	gtk_entry_set_text(GTK_ENTRY(snd26_int_entry), "INT5");
	gtk_entry_set_text(GTK_ENTRY(snd26_romaddr_entry), "CC000");
}

static void
snd86_default_button_clicked(GtkButton *b, gpointer d)
{

	UNUSED(b);
	UNUSED(d);

	gtk_entry_set_text(GTK_ENTRY(snd86_ioport_entry), "0188");
	gtk_entry_set_text(GTK_ENTRY(snd86_int_entry), "INT5");
	gtk_entry_set_text(GTK_ENTRY(snd86_soundid_entry), "4x");
	if (!GTK_TOGGLE_BUTTON(snd86_int_checkbutton)->active)
		g_signal_emit_by_name(GTK_OBJECT(snd86_int_checkbutton), "clicked");
	if (!GTK_TOGGLE_BUTTON(snd86_rom_checkbutton)->active)
		g_signal_emit_by_name(GTK_OBJECT(snd86_rom_checkbutton), "clicked");
}

static void
spb_default_button_clicked(GtkButton *b, gpointer d)
{
	int i;

	UNUSED(b);
	UNUSED(d);

	gtk_entry_set_text(GTK_ENTRY(spb_ioport_entry), "0188");
	gtk_entry_set_text(GTK_ENTRY(spb_int_entry), "INT5");
	gtk_entry_set_text(GTK_ENTRY(spb_romaddr_entry), "CC000");
	for (i = 0; i < NELEMENTS(spb_vr_channel_str); i++) {
		if (GTK_TOGGLE_BUTTON(spb_vr_channel_checkbutton[i])->active)
			g_signal_emit_by_name(GTK_OBJECT(spb_vr_channel_checkbutton[i]), "clicked");
	}
}

static void
driver_radiobutton_clicked(GtkButton *b, gpointer d)
{

	UNUSED(b);

	driver_snddrv = (int)d;
}

static GtkWidget *
create_mixer_note(void)
{
	GtkWidget *root_widget;
	GtkWidget *table;
	GtkWidget *vol_label[NELEMENTS(snd14_adj)];
	GtkWidget *vol_hscale[NELEMENTS(snd14_adj)];
	GtkWidget *mixer_default_button;
	GtkWidget *hbox;
	int i;

	root_widget = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(root_widget), 5);
	gtk_widget_show(root_widget);

	table = gtk_table_new(5, 4, FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(table), 5);
	gtk_box_pack_start(GTK_BOX(root_widget), table, FALSE, FALSE, 0);
	gtk_widget_show(table);

	for (i = 0; i < NELEMENTS(mixer_vol_tbl); i++) {
		vol_label[i] = gtk_label_new(mixer_vol_tbl[i].name);
		gtk_table_attach_defaults(GTK_TABLE(table), vol_label[i],
		    0, 1, i, i+1);
		gtk_widget_show(vol_label[i]);

		mixer_adj[i] = gtk_adjustment_new(*mixer_vol_tbl[i].valp,
		    mixer_vol_tbl[i].min, mixer_vol_tbl[i].max, 1.0, 1.0, 0.0);
		vol_hscale[i] = gtk_hscale_new(GTK_ADJUSTMENT(mixer_adj[i]));
		gtk_widget_show(vol_hscale[i]);
		gtk_scale_set_value_pos(GTK_SCALE(vol_hscale[i]),GTK_POS_RIGHT);
		gtk_scale_set_digits(GTK_SCALE(vol_hscale[i]), 0);
		gtk_table_attach_defaults(GTK_TABLE(table), vol_hscale[i],
		    1, 4, i, i+1);
	}

	/* "Default" button */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(root_widget), hbox, TRUE, FALSE, 0);

	mixer_default_button = gtk_button_new_with_label("Default");
	gtk_widget_show(mixer_default_button);
	gtk_box_pack_end(GTK_BOX(hbox), mixer_default_button, FALSE, FALSE, 5);
	g_signal_connect_swapped(GTK_OBJECT(mixer_default_button), "clicked",
	    GTK_SIGNAL_FUNC(mixer_default_button_clicked), NULL);

	return root_widget;
}

static GtkWidget *
create_pc9801_14_note(void)
{
	GtkWidget *root_widget;
	GtkWidget *table;
	GtkWidget *label[NELEMENTS(snd14_vol_str)];
	GtkWidget *scale[NELEMENTS(snd14_vol_str)];
	GtkWidget *snd14_default_button;
	GtkWidget *hbox;
	int i;

	root_widget = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(root_widget), 5);
	gtk_widget_show(root_widget);

	table = gtk_table_new(6, 4, FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(table), 5);
	gtk_box_pack_start(GTK_BOX(root_widget), table, FALSE, FALSE, 0);
	gtk_widget_show(table);

	for (i = 0; i < NELEMENTS(snd14_vol_str); i++) {
		label[i] = gtk_label_new(snd14_vol_str[i]);
		gtk_widget_show(label[i]);
		gtk_table_attach_defaults(GTK_TABLE(table), label[i], 0, 1, i, i+1);

		snd14_adj[i] = gtk_adjustment_new(np2cfg.vol14[i], 0.0, 15.0, 1.0, 1.0, 0.0);
		scale[i] = gtk_hscale_new(GTK_ADJUSTMENT(snd14_adj[i]));
		gtk_scale_set_default_values(GTK_SCALE(scale[i]));
		gtk_scale_set_digits(GTK_SCALE(scale[i]), 0);
		gtk_widget_show(scale[i]);
		gtk_table_attach_defaults(GTK_TABLE(table), scale[i], 1, 4, i, i+1);
	}

	/* "Default" button */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(root_widget), hbox, TRUE, FALSE, 0);

	snd14_default_button = gtk_button_new_with_label("Default");
	gtk_widget_show(snd14_default_button);
	gtk_box_pack_end(GTK_BOX(hbox), snd14_default_button, FALSE, FALSE, 5);
	g_signal_connect_swapped(GTK_OBJECT(snd14_default_button), "clicked",
	    GTK_SIGNAL_FUNC(snd14_default_button_clicked), NULL);

	return root_widget;
}

static GtkWidget*
create_pc9801_26_note(void)
{
	GtkWidget *root_widget;
	GtkWidget *table;
	GtkWidget *ioport_label;
	GtkWidget *ioport_combo;
	GtkWidget *int_label;
	GtkWidget *int_combo;
	GtkWidget *romaddr_label;
	GtkWidget *romaddr_combo;
	GtkWidget *snd26_default_button;
	GtkWidget *hbox;
	int i;

	root_widget = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(root_widget), 5);
	gtk_widget_show(root_widget);

	table = gtk_table_new(2, 4, FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(table), 5);
	gtk_table_set_col_spacings(GTK_TABLE(table), 5);
	gtk_box_pack_start(GTK_BOX(root_widget), table, FALSE, FALSE, 0);
	gtk_widget_show(table);

	/* I/O port */
	ioport_label = gtk_label_new("I/O port");
	gtk_widget_show(ioport_label);
	gtk_table_attach_defaults(GTK_TABLE(table), ioport_label, 0, 1, 0, 1);

	ioport_combo = gtk_combo_box_entry_new_text();
	gtk_widget_show(ioport_combo);
	gtk_table_attach_defaults(GTK_TABLE(table), ioport_combo, 1, 2, 0, 1);
	gtk_widget_set_size_request(ioport_combo, 80, -1);
	for (i = NELEMENTS(snd26_ioport_str) - 1; i >= 0; i--) {
		gtk_combo_box_append_text(GTK_COMBO_BOX(ioport_combo), snd26_ioport_str[i]);
	}

	snd26_ioport_entry = GTK_BIN(ioport_combo)->child;
	gtk_widget_show(snd26_ioport_entry);
	gtk_editable_set_editable(GTK_EDITABLE(snd26_ioport_entry), FALSE);
	gtk_entry_set_text(GTK_ENTRY(snd26_ioport_entry), snd26_ioport_str[SND26_GET_IOPORT()]);

	/* interrupt */
	int_label = gtk_label_new("Interrupt");
	gtk_widget_show(int_label);
	gtk_table_attach_defaults(GTK_TABLE(table), int_label, 2, 3, 0, 1);

	int_combo = gtk_combo_box_entry_new_text();
	gtk_widget_show(int_combo);
	gtk_table_attach_defaults(GTK_TABLE(table), int_combo, 3, 4, 0, 1);
	gtk_widget_set_size_request(int_combo, 80, -1);
	for (i = 0; i < NELEMENTS(snd26_intr_str); i++) {
		gtk_combo_box_append_text(GTK_COMBO_BOX(int_combo), snd26_intr_str[i]);
	}

	snd26_int_entry = GTK_BIN(int_combo)->child;
	gtk_widget_show(snd26_int_entry);
	gtk_editable_set_editable(GTK_EDITABLE(snd26_int_entry), FALSE);
	gtk_entry_set_text(GTK_ENTRY(snd26_int_entry), snd26_intr_str[SND26_GET_INTR()]);

	/* ROM address */
	romaddr_label = gtk_label_new("ROM");
	gtk_widget_show(romaddr_label);
	gtk_table_attach_defaults(GTK_TABLE(table), romaddr_label, 0, 1, 1, 2);

	romaddr_combo = gtk_combo_box_entry_new_text();
	gtk_widget_show(romaddr_combo);
	gtk_table_attach_defaults(GTK_TABLE(table), romaddr_combo, 1, 2, 1, 2);
	gtk_widget_set_size_request(romaddr_combo, 80, -1);
	for (i = 0; i < NELEMENTS(snd26_romaddr_str); i++) {
		gtk_combo_box_append_text(GTK_COMBO_BOX(romaddr_combo), snd26_romaddr_str[i]);
	}

	snd26_romaddr_entry = GTK_BIN(romaddr_combo)->child;
	gtk_widget_show(snd26_romaddr_entry);
	gtk_editable_set_editable(GTK_EDITABLE(snd26_romaddr_entry), FALSE);
	gtk_entry_set_text(GTK_ENTRY(snd26_romaddr_entry), snd26_romaddr_str[SND26_GET_ROMADDR()]);

	/* "Default" button */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(root_widget), hbox, FALSE, FALSE, 0);

	snd26_default_button = gtk_button_new_with_label("Default");
	gtk_widget_show(snd26_default_button);
	gtk_box_pack_end(GTK_BOX(hbox), snd26_default_button, FALSE, FALSE, 5);
	g_signal_connect_swapped(GTK_OBJECT(snd26_default_button), "clicked",
	    GTK_SIGNAL_FUNC(snd26_default_button_clicked), NULL);

	return root_widget;
}

static GtkWidget *
create_pc9801_86_note(void)
{
	GtkWidget *root_widget;
	GtkWidget *table;
	GtkWidget *ioport_label;
	GtkWidget *ioport_combo;
	GtkWidget *int_combo;
	GtkWidget *soundid_label;
	GtkWidget *soundid_combo;
	GtkWidget *snd86_default_button;
	GtkWidget *hbox;
	int i;

	root_widget = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(root_widget), 5);
	gtk_widget_show(root_widget);

	table = gtk_table_new(2, 5, FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(table), 5);
	gtk_table_set_col_spacings(GTK_TABLE(table), 5);
	gtk_box_pack_start(GTK_BOX(root_widget), table, FALSE, FALSE, 0);
	gtk_widget_show(table);

	/* I/O port */
	ioport_label = gtk_label_new("I/O port");
	gtk_widget_show(ioport_label);
	gtk_table_attach_defaults(GTK_TABLE(table), ioport_label, 0, 1, 0, 1);

	ioport_combo = gtk_combo_box_entry_new_text();
	gtk_widget_show(ioport_combo);
	gtk_table_attach_defaults(GTK_TABLE(table), ioport_combo, 1, 2, 0, 1);
	gtk_widget_set_size_request(ioport_combo, 80, -1);
	for (i = NELEMENTS(snd86_ioport_str) - 1; i >= 0; i--) {
		gtk_combo_box_append_text(GTK_COMBO_BOX(ioport_combo), snd86_ioport_str[i]);
	}

	snd86_ioport_entry = GTK_BIN(ioport_combo)->child;
	gtk_widget_show(snd86_ioport_entry);
	gtk_editable_set_editable(GTK_EDITABLE(snd86_ioport_entry), FALSE);
	gtk_entry_set_text(GTK_ENTRY(snd86_ioport_entry), snd86_ioport_str[SND86_GET_IOPORT()]);

	/* interrupt */
	snd86_int_checkbutton = gtk_check_button_new_with_label("Interrupt");
	gtk_widget_show(snd86_int_checkbutton);
	gtk_table_attach_defaults(GTK_TABLE(table), snd86_int_checkbutton, 2, 3, 0, 1);
	if (SND86_GET_INTERRUPT())
		g_signal_emit_by_name(GTK_OBJECT(snd86_int_checkbutton), "clicked");

	int_combo = gtk_combo_box_entry_new_text();
	gtk_widget_show(int_combo);
	gtk_table_attach_defaults(GTK_TABLE(table), int_combo, 3, 4, 0, 1);
	gtk_widget_set_size_request(int_combo, 80, -1);
	for (i = 0; i < NELEMENTS(snd86_intr_str); i++) {
		gtk_combo_box_append_text(GTK_COMBO_BOX(int_combo), snd86_intr_str[i]);
	}

	snd86_int_entry = GTK_BIN(int_combo)->child;
	gtk_widget_show(snd86_int_entry);
	gtk_editable_set_editable(GTK_EDITABLE(snd86_int_entry), FALSE);
	gtk_entry_set_text(GTK_ENTRY(snd86_int_entry), snd86_intr_str[SND86_GET_INTR()]);

	/* Sound ID */
	soundid_label = gtk_label_new("Sound ID");
	gtk_widget_show(soundid_label);
	gtk_table_attach_defaults(GTK_TABLE(table), soundid_label, 0, 1, 1, 2);

	soundid_combo = gtk_combo_box_entry_new_text();
	gtk_widget_show(soundid_combo);
	gtk_table_attach_defaults(GTK_TABLE(table), soundid_combo, 1, 2, 1, 2);
	gtk_widget_set_size_request(soundid_combo, 80, -1);
	for (i = NELEMENTS(snd86_soundid_str) - 1; i >= 0; i--) {
		gtk_combo_box_append_text(GTK_COMBO_BOX(soundid_combo), snd86_soundid_str[i]);
	}

	snd86_soundid_entry = GTK_BIN(soundid_combo)->child;
	gtk_widget_show(snd86_soundid_entry);
	gtk_editable_set_editable(GTK_EDITABLE(snd86_soundid_entry), FALSE);
	gtk_entry_set_text(GTK_ENTRY(snd86_soundid_entry), snd86_soundid_str[SND86_GET_SOUNDID()]);

	/* ROM */
	snd86_rom_checkbutton = gtk_check_button_new_with_label("ROM");
	gtk_widget_show(snd86_rom_checkbutton);
	gtk_table_attach_defaults(GTK_TABLE(table), snd86_rom_checkbutton, 2, 3, 1, 2);
	if (SND86_GET_BIOSROM())
		g_signal_emit_by_name(GTK_OBJECT(snd86_rom_checkbutton), "clicked");

	/* "Default" button */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
	gtk_box_pack_start(GTK_BOX(root_widget), hbox, FALSE, FALSE, 0);

	snd86_default_button = gtk_button_new_with_label("Default");
	gtk_widget_show(snd86_default_button);
	gtk_box_pack_end(GTK_BOX(hbox), snd86_default_button, FALSE, FALSE, 5);
	g_signal_connect_swapped(GTK_OBJECT(snd86_default_button), "clicked",
	    GTK_SIGNAL_FUNC(snd86_default_button_clicked), NULL);

	return root_widget;
}

static GtkWidget *
create_spb_note(void)
{
	GtkWidget *root_widget;
	GtkWidget *table;
	GtkWidget *ioport_label;
	GtkWidget *ioport_combo;
	GtkWidget *int_label;
	GtkWidget *int_combo;
	GtkWidget *romaddr_label;
	GtkWidget *romaddr_combo;
	GtkWidget *spb_default_button;
	GtkWidget *vr_label;
	GtkWidget *vr_level_label;
	GtkWidget *vr_level_scale;
	GtkWidget *hbox;
	int i;

	root_widget = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(root_widget), 5);
	gtk_widget_show(root_widget);

	table = gtk_table_new(2, 6, FALSE);
	gtk_table_set_row_spacings(GTK_TABLE(table), 5);
	gtk_table_set_col_spacings(GTK_TABLE(table), 5);
	gtk_box_pack_start(GTK_BOX(root_widget), table, FALSE, FALSE, 0);
	gtk_widget_show(table);

	/* I/O port */
	ioport_label = gtk_label_new("I/O port");
	gtk_widget_show(ioport_label);
	gtk_table_attach_defaults(GTK_TABLE(table), ioport_label, 0, 1, 0, 1);

	ioport_combo = gtk_combo_box_entry_new_text();
	gtk_widget_show(ioport_combo);
	gtk_table_attach_defaults(GTK_TABLE(table), ioport_combo, 1, 3, 0, 1);
	gtk_widget_set_size_request(ioport_combo, 80, -1);
	for (i = NELEMENTS(spb_ioport_str) - 1; i >= 0; i--) {
		gtk_combo_box_append_text(GTK_COMBO_BOX(ioport_combo), spb_ioport_str[i]);
	}

	spb_ioport_entry = GTK_BIN(ioport_combo)->child;
	gtk_widget_show(spb_ioport_entry);
	gtk_editable_set_editable(GTK_EDITABLE(spb_ioport_entry), FALSE);
	gtk_entry_set_text(GTK_ENTRY(spb_ioport_entry), spb_ioport_str[SPB_GET_IOPORT()]);

	/* interrupt */
	int_label = gtk_label_new("Interrupt");
	gtk_widget_show(int_label);
	gtk_table_attach_defaults(GTK_TABLE(table), int_label, 3, 4, 0, 1);

	int_combo = gtk_combo_box_entry_new_text();
	gtk_widget_show(int_combo);
	gtk_table_attach_defaults(GTK_TABLE(table), int_combo, 4, 6, 0, 1);
	gtk_widget_set_size_request(int_combo, 80, -1);
	for (i = 0; i < NELEMENTS(spb_intr_str); i++) {
		gtk_combo_box_append_text(GTK_COMBO_BOX(int_combo), spb_intr_str[i]);
	}

	spb_int_entry = GTK_BIN(int_combo)->child;
	gtk_widget_show(spb_int_entry);
	gtk_editable_set_editable(GTK_EDITABLE(spb_int_entry), FALSE);
	gtk_entry_set_text(GTK_ENTRY(spb_int_entry), spb_intr_str[SPB_GET_INTR()]);

	/* ROM address */
	romaddr_label = gtk_label_new("ROM");
	gtk_widget_show(romaddr_label);
	gtk_table_attach_defaults(GTK_TABLE(table), romaddr_label, 0, 1, 1, 2);

	romaddr_combo = gtk_combo_box_entry_new_text();
	gtk_widget_show(romaddr_combo);
	gtk_table_attach_defaults(GTK_TABLE(table), romaddr_combo, 1, 3, 1, 2);
	gtk_widget_set_size_request(romaddr_combo, 80, -1);
	for (i = 0; i < NELEMENTS(spb_romaddr_str); i++) {
		gtk_combo_box_append_text(GTK_COMBO_BOX(romaddr_combo), spb_romaddr_str[i]);
	}

	spb_romaddr_entry = GTK_BIN(romaddr_combo)->child;
	gtk_widget_show(spb_romaddr_entry);
	gtk_editable_set_editable(GTK_EDITABLE(spb_romaddr_entry), FALSE);
	gtk_entry_set_text(GTK_ENTRY(spb_romaddr_entry), spb_romaddr_str[SPB_GET_ROMADDR()]);

	/* VR */
	vr_label = gtk_label_new("VR");
	gtk_widget_show(vr_label);
	gtk_table_attach_defaults(GTK_TABLE(table), vr_label, 0, 1, 2, 3);

	for (i = 0; i < NELEMENTS(spb_vr_channel_str); i++) {
		spb_vr_channel_checkbutton[i] = gtk_check_button_new_with_label(spb_vr_channel_str[i]);
		gtk_widget_show(spb_vr_channel_checkbutton[i]);
		gtk_table_attach_defaults(GTK_TABLE(table), spb_vr_channel_checkbutton[i], i+1, i+2, 2, 3);
		if (np2cfg.spb_vrc & (1 << i))
			g_signal_emit_by_name(GTK_OBJECT(spb_vr_channel_checkbutton[i]), "clicked");
	}

	vr_level_label = gtk_label_new("level");
	gtk_widget_show(vr_level_label);
	gtk_table_attach_defaults(GTK_TABLE(table), vr_level_label, 3, 4, 2, 3);

	spb_vr_level_adj = gtk_adjustment_new(np2cfg.spb_vrl, 0.0, 24.0, 1.0, 1.0, 0.0);
	vr_level_scale = gtk_hscale_new(GTK_ADJUSTMENT(spb_vr_level_adj));
	gtk_scale_set_default_values(GTK_SCALE(vr_level_scale));
	gtk_scale_set_digits(GTK_SCALE(vr_level_scale), 0);
	gtk_scale_set_draw_value(GTK_SCALE(vr_level_scale), FALSE);
	gtk_widget_show(vr_level_scale);
	gtk_table_attach_defaults(GTK_TABLE(table), vr_level_scale, 4, 6, 2, 3);

	spb_reverse_channel_checkbutton = gtk_check_button_new_with_label("Reversed channel (SPB default)");
	gtk_widget_show(spb_reverse_channel_checkbutton);
	gtk_table_attach_defaults(GTK_TABLE(table), spb_reverse_channel_checkbutton, 0, 6, 3, 4);
	if (np2cfg.spb_x)
		g_signal_emit_by_name(GTK_OBJECT(spb_reverse_channel_checkbutton), "clicked");

	/* "Default" button */
	hbox = gtk_hbox_new(FALSE, 0);
	gtk_widget_show(hbox);
	gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);
	gtk_box_pack_start(GTK_BOX(root_widget), hbox, FALSE, FALSE, 0);

	spb_default_button = gtk_button_new_with_label("Default");
	gtk_widget_show(spb_default_button);
	gtk_box_pack_end(GTK_BOX(hbox), spb_default_button, FALSE, FALSE, 5);
	g_signal_connect_swapped(GTK_OBJECT(spb_default_button), "clicked",
	    GTK_SIGNAL_FUNC(spb_default_button_clicked), NULL);

	return root_widget;
}

static GtkWidget *
create_driver_note(void)
{
	GtkWidget *root_widget;
	GtkWidget *driver_frame;
	GtkWidget *driver_vbox;
	GtkWidget *driver_radiobutton[SNDDRV_DRVMAX];
	GtkWidget *audio_device_label;
	GtkWidget *snddrv_hbox;
	int i;

	root_widget = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(root_widget), 5);
	gtk_widget_show(root_widget);

	driver_frame = gtk_frame_new("Sound driver");
	gtk_widget_show(driver_frame);
	gtk_box_pack_start(GTK_BOX(root_widget), driver_frame, TRUE, TRUE, 0);

	driver_vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(driver_vbox), 5);
	gtk_widget_show(driver_vbox);
	gtk_container_add(GTK_CONTAINER(driver_frame), driver_vbox);

	for (i = 0; i < SNDDRV_DRVMAX; i++) {
		driver_radiobutton[i] = gtk_radio_button_new_with_label_from_widget((i > 0) ? GTK_RADIO_BUTTON(driver_radiobutton[i-1]) : NULL, driver_name[i]);
		gtk_widget_show(driver_radiobutton[i]);
		gtk_box_pack_start(GTK_BOX(driver_vbox), driver_radiobutton[i], TRUE, FALSE, 0);
		g_signal_connect(GTK_OBJECT(driver_radiobutton[i]), "clicked",
		    GTK_SIGNAL_FUNC(driver_radiobutton_clicked), (gpointer)i);
	}
#if !defined(USE_NETBSDAUDIO)
	gtk_widget_set_sensitive(driver_radiobutton[SNDDRV_NETBSD], FALSE);
#endif
#if !defined(USE_OSSAUDIO)
	gtk_widget_set_sensitive(driver_radiobutton[SNDDRV_OSS], FALSE);
#endif
#if !defined(USE_ESDAUDIO)
	gtk_widget_set_sensitive(driver_radiobutton[SNDDRV_ESD], FALSE);
#endif
#if !defined(USE_SDLAUDIO) && !defined(USE_SDLMIXER)
	gtk_widget_set_sensitive(driver_radiobutton[SNDDRV_SDL], FALSE);
#endif

	switch (np2oscfg.snddrv) {
	case SNDDRV_NODRV:
#if defined(USE_NETBSDAUDIO)
	case SNDDRV_NETBSD:
#endif
#if defined(USE_OSSAUDIO)
	case SNDDRV_OSS:
#endif
#if defined(USE_ESDAUDIO)
	case SNDDRV_ESD:
#endif
#if defined(USE_SDLAUDIO) || defined(USE_SDLMIXER)
	case SNDDRV_SDL:
#endif
		g_signal_emit_by_name(GTK_OBJECT(driver_radiobutton[np2oscfg.snddrv]), "clicked");
		break;

#if !defined(USE_NETBSDAUDIO)
	case SNDDRV_NETBSD:
#endif
#if !defined(USE_OSSAUDIO)
	case SNDDRV_OSS:
#endif
#if !defined(USE_ESDAUDIO)
	case SNDDRV_ESD:
#endif
#if !defined(USE_SDLAUDIO) && !defined(USE_SDLMIXER)
	case SNDDRV_SDL:
#endif
	case SNDDRV_DRVMAX:
	default:
		np2oscfg.snddrv = SNDDRV_NODRV;
		sysmng_update(SYS_UPDATEOSCFG);
		break;
	}

	snddrv_hbox = gtk_hbox_new(FALSE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(snddrv_hbox), 5);
	gtk_widget_show(snddrv_hbox);
	gtk_box_pack_start(GTK_BOX(root_widget), snddrv_hbox, FALSE, FALSE, 0);

	audio_device_label = gtk_label_new("Sound device");
	gtk_widget_show(audio_device_label);
	gtk_box_pack_start(GTK_BOX(snddrv_hbox), audio_device_label, FALSE, FALSE, 3);

	driver_audio_device_entry = gtk_entry_new();
	gtk_widget_show(driver_audio_device_entry);
	gtk_entry_set_text(GTK_ENTRY(driver_audio_device_entry), np2oscfg.audiodev);
	gtk_box_pack_start(GTK_BOX(snddrv_hbox), driver_audio_device_entry, TRUE, TRUE, 3);

	return root_widget;
}

void
create_sound_dialog(void)
{
	GtkWidget *sound_dialog;
	GtkWidget *main_vbox;
	GtkWidget *notebook;
	GtkWidget *mixer_note;
	GtkWidget *pc9801_14_note;
	GtkWidget *pc9801_26_note;
	GtkWidget *pc9801_86_note;
	GtkWidget *spb_note;
	GtkWidget *driver_note;
	GtkWidget *confirm_widget;
	GtkWidget *ok_button;
	GtkWidget *cancel_button;

	uninstall_idle_process();

	sound_dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(sound_dialog), "Sound option");
	gtk_window_set_position(GTK_WINDOW(sound_dialog), GTK_WIN_POS_CENTER);
	gtk_window_set_modal(GTK_WINDOW(sound_dialog), TRUE);
	gtk_window_set_resizable(GTK_WINDOW(sound_dialog), FALSE);

	g_signal_connect(GTK_OBJECT(sound_dialog), "destroy",
	    GTK_SIGNAL_FUNC(dialog_destroy), NULL);

	main_vbox = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(main_vbox);
	gtk_container_add(GTK_CONTAINER(sound_dialog), main_vbox);

	notebook = gtk_notebook_new();
	gtk_widget_show(notebook);
	gtk_box_pack_start(GTK_BOX(main_vbox), notebook, TRUE, TRUE, 0);

	/* "Mixer" note */
	mixer_note = create_mixer_note();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), mixer_note, gtk_label_new("Mixer"));

	/* "PC-9801-14" note */
	pc9801_14_note = create_pc9801_14_note();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pc9801_14_note, gtk_label_new("PC-9801-14"));

	/* "26" note */
	pc9801_26_note = create_pc9801_26_note();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pc9801_26_note, gtk_label_new("26"));

	/* "86" note */
	pc9801_86_note = create_pc9801_86_note();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), pc9801_86_note, gtk_label_new("86"));

	/* "SPB" note */
	spb_note = create_spb_note();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), spb_note, gtk_label_new("SPB"));

	/* "Driver" note */
	driver_note = create_driver_note();
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), driver_note, gtk_label_new("Driver"));

	/*
	 * OK, Cancel button
	 */
	confirm_widget = gtk_hbox_new(FALSE, 5);
	gtk_widget_show(confirm_widget);
	gtk_box_pack_start(GTK_BOX(main_vbox), confirm_widget, FALSE, FALSE, 5);

	cancel_button = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	gtk_widget_show(cancel_button);
	gtk_box_pack_end(GTK_BOX(confirm_widget), cancel_button, FALSE, FALSE, 0);
	GTK_WIDGET_SET_FLAGS(cancel_button, GTK_CAN_DEFAULT);
	g_signal_connect_swapped(GTK_OBJECT(cancel_button), "clicked",
	    GTK_SIGNAL_FUNC(gtk_widget_destroy), GTK_OBJECT(sound_dialog));

	ok_button = gtk_button_new_from_stock(GTK_STOCK_OK);
	gtk_widget_show(ok_button);
	gtk_box_pack_end(GTK_BOX(confirm_widget), ok_button, FALSE, FALSE, 0);
	GTK_WIDGET_SET_FLAGS(ok_button, GTK_CAN_DEFAULT);
	GTK_WIDGET_SET_FLAGS(ok_button, GTK_HAS_DEFAULT);
	g_signal_connect(GTK_OBJECT(ok_button), "clicked",
	    GTK_SIGNAL_FUNC(ok_button_clicked), (gpointer)sound_dialog);
	gtk_widget_grab_default(ok_button);

	gtk_widget_show_all(sound_dialog);
}

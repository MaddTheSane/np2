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

#ifndef	NP2_GTK_GTKMENU_H__
#define	NP2_GTK_GTKMENU_H__

#include "gtk/xnp2.h"
#include <gtk/gtkitemfactory.h>

typedef struct {
	GtkItemFactory *item_factory;
} _MENU_HDL, *MENU_HDL;

enum {
	DISP_VSYNC,
	REAL_PALETTES,
	NO_WAIT,
	SEEK_SOUND,
	MOUSE_MODE,
	XSHIFT_SHIFT,
	XSHIFT_CTRL,
	XSHIFT_GRPH,
	CLOCK_DISP,
	FRAME_DISP,
	JOY_REVERSE,
	JOY_RAPID,
	MOUSE_RAPID,
	S98_LOGGING,
	TOOL_WINDOW,
	KEY_DISPLAY,
	SOFT_KBD,
	JAST_SOUND,
	NUM_TOGGLE_ITEMS
};

GtkWidget *create_menu(void);

void xmenu_toggle_item(int, int, int);
void xmenu_select_item(MENU_HDL, const char *);

typedef BOOL file_selection_ok_callback(void *arg, const char *path);
typedef void file_selection_destrroy_callback(void *arg, BOOL result);

void create_file_selection(const char *, const char *, void *,
                              file_selection_ok_callback *,
                              file_selection_destrroy_callback *);


void create_about_dialog(void);
void create_calendar_dialog(void);
void create_configure_dialog(void);
void create_midi_dialog(void);
void create_screen_dialog(void);
void create_sound_dialog(void);
void create_newdisk_dialog(const char *filebasename, const char *fileextname);

#endif	/* NP2_GTK_GTKMENU_H__ */

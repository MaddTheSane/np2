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

#include <sys/stat.h>
#include <errno.h>

#include "gtk/xnp2.h"
#include "gtk/gtkkeyboard.h"
#include "gtk/gtkmenu.h"

#include <gdk/gdkkeysyms.h>
#include <gtk/gtkmenu.h>

#include "np2.h"
#include "dosio.h"
#include "ini.h"
#include "pccore.h"
#include "iocore.h"

#include "beep.h"
#include "diskdrv.h"
#include "font.h"
#include "mpu98ii.h"
#include "pc9861k.h"
#include "s98.h"
#include "scrnbmp.h"
#include "kdispwin.h"
#include "toolwin.h"

#include "mousemng.h"
#include "scrnmng.h"
#include "sysmng.h"


static void disable_item(const char *);

static void disable_unused_items(void);

static void xmenu_select_framerate(int);
static void xmenu_select_beepvol(int);
static void xmenu_select_soundboard(int);
static void xmenu_select_extmem(int);
static void xmenu_select_mouse_move_ratio(int);

static void exit_from_menu(gpointer, guint, GtkWidget *);
static void reset(gpointer, guint, GtkWidget *);
static void rotate(gpointer, guint, GtkWidget *);

static void fddopen(gpointer, guint, GtkWidget *);
static void fddeject(gpointer, guint, GtkWidget *);
static void sasiopen(gpointer, guint, GtkWidget *);
static void sasiremove(gpointer, guint, GtkWidget *);

static void change_font(gpointer, guint, GtkWidget *);

static void new_disk(gpointer, guint, GtkWidget *);

static void framerate(gpointer, guint, GtkWidget *);
enum {
	FRAME_AUTO,
	FRAME_FULL,
	FRAME_30,
	FRAME_20,
	FRAME_15,
	NUM_FRAMERATE
};

static void keyboard(gpointer, guint, GtkWidget *);
static void f12(gpointer, guint, GtkWidget *);

static void beepvol(gpointer, guint, GtkWidget *);
enum {
	BEEP_OFF,
	BEEP_LOW,
	BEEP_MID,
	BEEP_HIGH,
	NUM_BEEP
};

static void sound_board(gpointer, guint, GtkWidget *);
enum {
	NOSOUNDBOARD = 0,
	PC9801_14 = 1,
	PC9801_26K = 2,
	PC9801_86 = 4,
	PC9801_118 = 8,
	CHIBI_OTO = 0x14,
	SPEAK_BOARD = 0x20,
	SPARK_BOARD = 0x40,
	AMD98_BOARD = 0x80
};

static void memory(gpointer, guint, GtkWidget *);

static void mouse_ratio(gpointer, guint, GtkWidget *);
static void midi_panic(gpointer, guint, GtkWidget *);

static void bmpsave(gpointer, guint, GtkWidget *);

static void toggle(gpointer, guint, GtkWidget *);


/*
 * dialog function adaptor
 */
static void
_create_configure_dialog(gpointer p, guint action, GtkWidget *w)
{
	UNUSED(p);
	UNUSED(action);
	UNUSED(w);

	create_configure_dialog();
}

static void
_create_screen_dialog(gpointer p, guint action, GtkWidget *w)
{
	UNUSED(p);
	UNUSED(action);
	UNUSED(w);

	create_screen_dialog();
}

static void
_create_midi_dialog(gpointer p, guint action, GtkWidget *w)
{
	UNUSED(p);
	UNUSED(action);
	UNUSED(w);

	create_midi_dialog();
}

static void
_create_sound_dialog(gpointer p, guint action, GtkWidget *w)
{
	UNUSED(p);
	UNUSED(action);
	UNUSED(w);

	create_sound_dialog();
}

static void
_create_calendar_dialog(gpointer p, guint action, GtkWidget *w)
{
	UNUSED(p);
	UNUSED(action);
	UNUSED(w);

	create_calendar_dialog();
}

static void
_create_about_dialog(gpointer p, guint action, GtkWidget *w)
{
	UNUSED(p);
	UNUSED(action);
	UNUSED(w);

	create_about_dialog();
}


/*
 * Menu items
 */
#define	f(f)	((GtkItemFactoryCallback)(f))
static GtkItemFactoryEntry menu_items[] = {
{ "/Emulate", 			NULL, NULL, 0, "<Branch>" },
{ "/Emulate/_Reset", 		NULL, f(reset), 0, NULL },
{ "/Emulate/sep1",		NULL, NULL, 0, "<Separator>" },
{ "/Emulate/_Configure...",	NULL, f(_create_configure_dialog), 0, NULL },
{ "/Emulate/_New Disk",		NULL, NULL, 0, "<Branch>" },
{ "/Emulate/New Disk/_FD image...", NULL, f(new_disk), 0, NULL },
{ "/Emulate/New Disk/_HD image...", NULL, f(new_disk), 1, NULL },
{ "/Emulate/_Font...",		NULL, f(change_font), 0, NULL },
{ "/Emulate/sep2",		NULL, NULL, 0, "<Separator>" },
{ "/Emulate/E_xit",		NULL, f(exit_from_menu), 0, NULL },
{ "/FDD",			NULL, NULL, 0, "<Branch>" },
{ "/FDD/Drive_1",		NULL, NULL, 0, "<Branch>" },
{ "/FDD/Drive1/_Open...",	NULL, f(fddopen), 0, NULL },
{ "/FDD/Drive1/_Eject",		NULL, f(fddeject), 0, NULL },
{ "/FDD/Drive_2",		NULL, NULL, 0, "<Branch>" },
{ "/FDD/Drive2/_Open...",	NULL, f(fddopen), 1, NULL },
{ "/FDD/Drive2/_Eject",		NULL, f(fddeject), 1, NULL },
{ "/FDD/sep1",			NULL, NULL, 0, "<Separator>" },
{ "/FDD/_Eject All",		NULL, f(fddeject), ~0, NULL },
{ "/HardDisk",			NULL, NULL, 0, "<Branch>" },
{ "/HardDisk/SASI-_1",		NULL, NULL, 0, "<Branch>" },
{ "/HardDisk/SASI-1/_Open...",	NULL, f(sasiopen), 0, NULL },
{ "/HardDisk/SASI-1/_Remove",	NULL, f(sasiremove), 0, NULL },
{ "/HardDisk/SASI-_2",		NULL, NULL, 0, "<Branch>" },
{ "/HardDisk/SASI-2/_Open...",	NULL, f(sasiopen), 1, NULL },
{ "/HardDisk/SASI-2/_Remove",	NULL, f(sasiremove), 1, NULL },
{ "/HardDisk/sep1",		NULL, NULL, 0, "<Separator>" },
{ "/HardDisk/_Remove All",	NULL, f(sasiremove), ~0, NULL },
{ "/Screen",			NULL, NULL, 0, "<Branch>" },
{ "/Screen/_Window",		NULL, NULL, 0, "<RadioItem>" },
{ "/Screen/_FullScreen",	NULL, NULL, 0, "/Screen/Window" },
{ "/Screen/sep1",		NULL, NULL, 0, "<Separator>" },
{ "/Screen/Nor_mal",		NULL, f(rotate), 0, "<RadioItem>"},
{ "/Screen/_Left Rotated",	NULL, f(rotate), SCRNMODE_ROTATELEFT,"/Screen/Normal"},
{ "/Screen/_Right Rotated",	NULL, f(rotate), SCRNMODE_ROTATERIGHT, "/Screen/Left Rotated" },
{ "/Screen/sep2",		NULL, NULL, 0, "<Separator>" },
{ "/Screen/_Disp Vsync",	NULL, f(toggle), DISP_VSYNC, "<ToggleItem>"},
{ "/Screen/Real _Palettes",	NULL, f(toggle), REAL_PALETTES, "<ToggleItem>"},
{ "/Screen/_No Wait",		NULL, f(toggle), NO_WAIT, "<ToggleItem>" },
{ "/Screen/_Auto frame",	NULL, f(framerate), FRAME_AUTO, "<RadioItem>" },
{ "/Screen/_60 fps",		NULL, f(framerate), FRAME_FULL, "/Screen/Auto frame" },
{ "/Screen/_30 fps",		NULL, f(framerate), FRAME_30, "/Screen/60 fps"},
{ "/Screen/_20 fps",		NULL, f(framerate), FRAME_20, "/Screen/30 fps"},
{ "/Screen/_15 fps",		NULL, f(framerate), FRAME_15, "/Screen/20 fps"},
{ "/Screen/sep3",		NULL, NULL, 0, "<Separator>" },
{ "/Screen/_Screen option...",	NULL, f(_create_screen_dialog), 0, NULL },
{ "/Device",			NULL, NULL, 0, "<Branch>" },
{ "/Device/_Keyboard",		NULL, NULL, 0, "<Branch>" },
{ "/Device/Keyboard/_Keyboard",	NULL, f(keyboard), 0, "<RadioItem>" },
{ "/Device/Keyboard/Joykey-_1",	NULL, f(keyboard), 1, "/Device/Keyboard/Keyboard"},
{ "/Device/Keyboard/Joykey-_2",	NULL, f(keyboard), 2, "/Device/Keyboard/Joykey-1"},
{ "/Device/Keyboard/sep1",	NULL, NULL, 0, "<Separator>" },
{ "/Device/Keyboard/mechanical _SHIFT",	NULL, f(toggle), XSHIFT_SHIFT, "<ToggleItem>" },
{ "/Device/Keyboard/mechanical _CTRL",	NULL, f(toggle), XSHIFT_CTRL, "<ToggleItem>" },
{ "/Device/Keyboard/mechanical _GRPH",	NULL, f(toggle), XSHIFT_GRPH, "<ToggleItem>" },
{ "/Device/Keyboard/sep2",	NULL, NULL, 0, "<Separator>" },
{ "/Device/Keyboard/F12 = _Mouse", NULL, f(f12), 0, "<RadioItem>" },
{ "/Device/Keyboard/F12 = Co_py",NULL, f(f12),1,"/Device/Keyboard/F12 = Mouse"},
{ "/Device/Keyboard/F12 = S_top",NULL, f(f12),2,"/Device/Keyboard/F12 = Copy" },
{ "/Device/Keyboard/F12 = tenkey [=]", NULL, f(f12),3,"/Device/Keyboard/F12 = Stop" },
{ "/Device/Keyboard/F12 = tenkey [,]", NULL, f(f12),4,"/Device/Keyboard/F12 = tenkey [=]" },
{ "/Device/_Sound",		NULL, NULL, 0, "<Branch>" },
{ "/Device/Sound/Beep _off",	NULL, f(beepvol), BEEP_OFF, "<RadioItem>" },
{ "/Device/Sound/Beep _low",	NULL, f(beepvol), BEEP_LOW, "/Device/Sound/Beep off" },
{ "/Device/Sound/Beep _mid",	NULL, f(beepvol), BEEP_MID, "/Device/Sound/Beep low" },
{ "/Device/Sound/Beep _high",	NULL, f(beepvol), BEEP_HIGH, "/Device/Sound/Beep mid" },
{ "/Device/Sound/sep1",		NULL, NULL, 0, "<Separator>" },
{ "/Device/Sound/_Disable board", NULL, f(sound_board), NOSOUNDBOARD, "<RadioItem>" },
{ "/Device/Sound/PC-9801-_14",	NULL, f(sound_board), PC9801_14, "/Device/Sound/Disable board" },
{ "/Device/Sound/PC-9801-_26K",	NULL, f(sound_board), PC9801_26K, "/Device/Sound/PC-9801-14" },
{ "/Device/Sound/PC-9801-8_6",	NULL, f(sound_board), PC9801_86, "/Device/Sound/PC-9801-26K" },
{ "/Device/Sound/PC-9801-11_8",	NULL, f(sound_board), PC9801_118, "/Device/Sound/PC-9801-86" },
{ "/Device/Sound/_Chibi-oto",	NULL, f(sound_board), CHIBI_OTO, "/Device/Sound/PC-9801-118" },
{ "/Device/Sound/S_peak board",	NULL, f(sound_board), SPEAK_BOARD, "/Device/Sound/Chibi-oto" },
{ "/Device/Sound/Sp_ark board",	NULL, f(sound_board), SPARK_BOARD, "/Device/Sound/Speak board" },
{ "/Device/Sound/_AMD98",	NULL, f(sound_board), SPARK_BOARD, "/Device/Sound/Spark board" },
{ "/Device/Sound/sep2",		NULL, NULL, 0, "<Separator>" },
{ "/Device/Sound/_Seek Sound",	NULL, NULL, 0, "<ToggleItem>" },
{ "/Device/M_emory",		NULL, NULL, 0, "<Branch>" },
{ "/Device/Memory/64_0KB",	NULL, f(memory), 0, "<RadioItem>" },
{ "/Device/Memory/_1.6MB",	NULL, f(memory), 1, "/Device/Memory/640KB" },
{ "/Device/Memory/_3.6MB",	NULL, f(memory), 2, "/Device/Memory/1.6MB" },
{ "/Device/Memory/_5.6MB",	NULL, f(memory), 3, "/Device/Memory/3.6MB" },
{ "/Device/Memory/_7.6MB",	NULL, f(memory), 4, "/Device/Memory/5.6MB" },
{ "/Device/Memory/_9.6MB",	NULL, f(memory), 5, "/Device/Memory/7.6MB" },
{ "/Device/Memory/13._6MB",	NULL, f(memory), 6, "/Device/Memory/9.6MB" },
{ "/Device/_Mouse",		NULL, NULL, 0, "<Branch>" },
{ "/Device/Mouse/_Mouse mode",	NULL, f(toggle), MOUSE_MODE, "<ToggleItem>" },
{ "/Device/Mouse/sep",		NULL, NULL, 0, "<Separator>" },
{ "/Device/Mouse/x_0.5",	NULL, f(mouse_ratio), MOUSE_RATIO_050, "<RadioItem>" },
{ "/Device/Mouse/x0._75",	NULL, f(mouse_ratio), MOUSE_RATIO_075, "/Device/Mouse/x0.5" },
{ "/Device/Mouse/x_1.0",	NULL, f(mouse_ratio), MOUSE_RATIO_100, "/Device/Mouse/x0.75" },
{ "/Device/Mouse/x1._5",	NULL, f(mouse_ratio), MOUSE_RATIO_150, "/Device/Mouse/x1.0" },
{ "/Device/Mouse/x_2.0",	NULL, f(mouse_ratio), MOUSE_RATIO_200, "/Device/Mouse/x1.5" },
{ "/Device/Mouse/x_4.0",	NULL, f(mouse_ratio), MOUSE_RATIO_400, "/Device/Mouse/x2.0" },
{ "/Device/Mouse/x_8.0",	NULL, f(mouse_ratio), MOUSE_RATIO_800, "/Device/Mouse/x4.0" },
{ "/Device/sep2",		NULL, NULL, 0, "<Separator>" },
{ "/Device/MIDI _option...",	NULL, f(_create_midi_dialog), 0, NULL },
{ "/Device/MIDI _Panic",	NULL, f(midi_panic), 0, NULL },
{ "/Device/sep3",		NULL, NULL, 0, "<Separator>" },
{ "/Device/So_und option...",	NULL, f(_create_sound_dialog), 0, NULL },
{ "/Other",			NULL, NULL, 0, "<Branch>" },
{ "/Other/_BMP Save...",	NULL, f(bmpsave), 0, NULL },
{ "/Other/_S98 logging...",	NULL, f(toggle), S98_LOGGING, "<ToggleItem>" },
{ "/Other/Ca_lendar...",	NULL, f(_create_calendar_dialog), 0, NULL },
{ "/Other/_Clock Disp",		NULL, f(toggle), CLOCK_DISP, "<ToggleItem>" },
{ "/Other/_Frame Disp",		NULL, f(toggle), FRAME_DISP, "<ToggleItem>" },
{ "/Other/Joy Re_verse",	NULL, f(toggle), JOY_REVERSE, "<ToggleItem>" },
{ "/Other/Joy _Rapid",		NULL, f(toggle), JOY_RAPID, "<ToggleItem>" },
{ "/Other/_Mouse Rapid",	NULL, f(toggle), MOUSE_RAPID, "<ToggleItem>" },
{ "/Other/sep1",		NULL, NULL, 0, "<Separator>" },
{ "/Other/_Tool Window",	NULL, f(toggle), TOOL_WINDOW, "<ToggleItem>" },
{ "/Other/_Key Display",	NULL, f(toggle), KEY_DISPLAY, "<ToggleItem>" },
{ "/Other/sep2",		NULL, NULL, 0, "<Separator>" },
{ "/Other/_About...",		NULL, f(_create_about_dialog), 0, NULL },
};
#undef	f

/*
 * toggle items
 */
static struct {
	char *name;
	int stat;
	int flag;
} toggle_items[] = {
	{ "/Screen/Disp Vsync",			0, SYS_UPDATECFG },
	{ "/Screen/Real Palettes",		0, SYS_UPDATECFG },
	{ "/Screen/No Wait",			0, SYS_UPDATEOSCFG },
	{ "/Device/Sound/Seek Sound",		0, SYS_UPDATECFG },
	{ "/Device/Mouse/Mouse mode",		0, SYS_UPDATEOSCFG },
	{ "/Device/Keyboard/mechanical SHIFT",	0, SYS_UPDATECFG },
	{ "/Device/Keyboard/mechanical CTRL",	0, SYS_UPDATECFG },
	{ "/Device/Keyboard/mechanical GRPH",	0, SYS_UPDATECFG },
	{ "/Other/Clock Disp",			0, SYS_UPDATEOSCFG },
	{ "/Other/Frame Disp",			0, SYS_UPDATEOSCFG },
	{ "/Other/Joy Reverse",			0, SYS_UPDATECFG },
	{ "/Other/Joy Rapid",			0, SYS_UPDATECFG },
	{ "/Other/Mouse Rapid",			0, SYS_UPDATECFG },
	{ "/Other/S98 logging...",		0, 0 },
	{ "/Other/Tool Window",			0, SYS_UPDATEOSCFG },
	{ "/Other/Key Display",			0, SYS_UPDATEOSCFG },
};

static _MENU_HDL menu_hdl;
static BOOL inited = FALSE;

/*
 * menu initialize
 */
GtkWidget *
create_menu(GtkWidget *w)
{
	GtkAccelGroup *accel_group;

	accel_group = gtk_accel_group_new();
	menu_hdl.item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", accel_group);
	gtk_item_factory_create_items(menu_hdl.item_factory, NELEMENTS(menu_items), menu_items, NULL);
	gtk_accel_group_attach(accel_group, GTK_OBJECT(w));

	disable_unused_items();

	xmenu_toggle_item(DISP_VSYNC, np2cfg.DISPSYNC, TRUE);
	xmenu_toggle_item(REAL_PALETTES, np2cfg.RASTER, TRUE);
	xmenu_toggle_item(NO_WAIT, np2oscfg.NOWAIT, TRUE);
	xmenu_toggle_item(SEEK_SOUND, np2cfg.MOTOR, TRUE);
	xmenu_toggle_item(MOUSE_MODE, np2oscfg.MOUSE_SW, TRUE);
	xmenu_toggle_item(CLOCK_DISP, np2oscfg.DISPCLK & 1, TRUE);
	xmenu_toggle_item(FRAME_DISP, np2oscfg.DISPCLK & 2, TRUE);
	xmenu_toggle_item(XSHIFT_SHIFT, np2cfg.XSHIFT & 1, TRUE);
	xmenu_toggle_item(XSHIFT_CTRL, np2cfg.XSHIFT & 2, TRUE);
	xmenu_toggle_item(XSHIFT_GRPH, np2cfg.XSHIFT & 4, TRUE);
	xmenu_toggle_item(JOY_REVERSE, np2cfg.BTN_MODE, TRUE);
	xmenu_toggle_item(JOY_RAPID, np2cfg.BTN_RAPID, TRUE);
	xmenu_toggle_item(MOUSE_RAPID, np2cfg.MOUSERAPID, TRUE);
	xmenu_toggle_item(TOOL_WINDOW, np2oscfg.toolwin, TRUE);
	xmenu_toggle_item(KEY_DISPLAY, np2oscfg.keydisp, TRUE);

	xmenu_select_framerate(np2oscfg.DRAW_SKIP);
	xmenu_select_beepvol(np2cfg.BEEP_VOL);
	xmenu_select_soundboard(np2cfg.SOUND_SW);
	xmenu_select_extmem(np2cfg.EXTMEM);
	xmenu_select_mouse_move_ratio(np2oscfg.mouse_move_ratio);

	inited = 1;

	return gtk_item_factory_get_widget(menu_hdl.item_factory, "<main>");
}

static void
disable_unused_items(void)
{
	static const char *items[] = {
		"/Screen/FullScreen",
		"/Other/Tool Window",
#if defined(NOSOUND)
		"/Device/Sound/Beep low",
		"/Device/Sound/Beep mid",
		"/Device/Sound/Beep high",
		"/Device/Sound/PC-9801-14",
		"/Device/Sound/PC-9801-26K",
		"/Device/Sound/PC-9801-86",
		"/Device/Sound/PC-9801-118",
		"/Device/Sound/Chibi-oto",
		"/Device/Sound/Speak board",
		"/Device/Sound/Spark board",
		"/Device/Sound/AMD98",
		"/Device/Sound/Seek Sound"
#endif
	};
	int i;

	for (i = 0; i < NELEMENTS(items); i++)
		disable_item(items[i]);
}

/*
 * misc
 */
static void
disable_item(const char* name)
{
	GtkWidget *w;

	w = gtk_item_factory_get_widget(menu_hdl.item_factory, name);
	gtk_widget_set_sensitive(w, FALSE);
}

void
xmenu_select_item(MENU_HDL hdl, const char* name)
{
	GtkWidget *w;

	w = gtk_item_factory_get_widget(hdl->item_factory, name);
	gtk_signal_emit_by_name(GTK_OBJECT(w), "activate-item");
}

void
xmenu_toggle_item(int arg, int onoff, int emitp)
{

	if (arg < NELEMENTS(toggle_items)) {
		if (onoff != toggle_items[arg].stat) {
			toggle_items[arg].stat = onoff;
			if (emitp)
				xmenu_select_item(&menu_hdl, toggle_items[arg].name);
			if (inited && arg != MOUSE_MODE) {
				sysmng_update(toggle_items[arg].flag);
			}
		}
	}
}

static void
xmenu_select_framerate(int kind)
{
	static const char *name[NUM_FRAMERATE] = {
		"/Screen/Auto frame",
		"/Screen/60 fps",
		"/Screen/30 fps",
		"/Screen/20 fps",
		"/Screen/15 fps",
	};

	if (kind < NUM_FRAMERATE) {
		xmenu_select_item(&menu_hdl, name[kind]);
		sysmng_update(SYS_UPDATECFG);
	}
}

static void
xmenu_select_beepvol(int kind)
{
#if !defined(NOSOUND)
	static const char *name[NUM_BEEP] = {
		"/Device/Sound/Beep off",
		"/Device/Sound/Beep low",
		"/Device/Sound/Beep mid",
		"/Device/Sound/Beep high",
	};

	if (kind < NUM_BEEP) {
		xmenu_select_item(&menu_hdl, name[kind]);
		sysmng_update(SYS_UPDATECFG);
	}
#else
	UNUSED(kind);

	np2cfg.BEEP_VOL = 0;
	xmenu_select_item(&menu_hdl, "/Device/Sound/Beep off");
#endif
}

static void
xmenu_select_soundboard(int kind)
{
#if !defined(NOSOUND)
	static const struct {
		gchar *name;
		gint num;
	} soundboard[] = {
		{ "/Device/Sound/Disable board", NOSOUNDBOARD },
		{ "/Device/Sound/PC-9801-14", PC9801_14 },
		{ "/Device/Sound/PC-9801-26K", PC9801_26K },
		{ "/Device/Sound/PC-9801-86", PC9801_86 },
		{ "/Device/Sound/PC-9801-118", PC9801_118 },
		{ "/Device/Sound/Chibi-oto", CHIBI_OTO },
		{ "/Device/Sound/Speak board", SPEAK_BOARD },
		{ "/Device/Sound/Spark board", SPARK_BOARD },
		{ "/Device/Sound/AMD98", AMD98_BOARD },
	};
	gint i;

	for (i = 0; i < NELEMENTS(soundboard); i++) {
		if (kind == soundboard[i].num) {
			xmenu_select_item(&menu_hdl, soundboard[i].name);
			sysmng_update(SYS_UPDATECFG);
			break;
		}
	}
#else
	UNUSED(kind);

	np2cfg.SOUND_SW = 0;
	xmenu_select_item(&menu_hdl, "/Device/Sound/Disable board");
#endif
}

static void
xmenu_select_extmem(int kind)
{
	static const struct {
		gchar *name;
		gint num;
	} extmem[] = {
		{ "/Device/Memory/640KB", 0 },
		{ "/Device/Memory/1.6MB", 1 },
		{ "/Device/Memory/3.6MB", 3 },
		{ "/Device/Memory/5.6MB", 5 },
		{ "/Device/Memory/7.6MB", 7 },
		{ "/Device/Memory/9.6MB", 9 },
		{ "/Device/Memory/13.6MB", 13 },
	};
	gint i;

	for (i = 0; i < NELEMENTS(extmem); i++) {
		if (kind == extmem[i].num) {
			xmenu_select_item(&menu_hdl, extmem[i].name);
			sysmng_update(SYS_UPDATECFG);
			return;
		}
	}
	xmenu_select_item(&menu_hdl, "/Device/Memory/1.6MB");
	sysmng_update(SYS_UPDATECFG);
}

static void
xmenu_select_mouse_move_ratio(int kind)
{
	static const struct {
		gchar *name;
		gint num;
	} moveratio[] = {
		{ "/Device/Mouse/x0.5", MOUSE_RATIO_050 },
		{ "/Device/Mouse/x0.75", MOUSE_RATIO_075 },
		{ "/Device/Mouse/x1.0", MOUSE_RATIO_100 },
		{ "/Device/Mouse/x1.5", MOUSE_RATIO_150 },
		{ "/Device/Mouse/x2.0", MOUSE_RATIO_200 },
		{ "/Device/Mouse/x4.0", MOUSE_RATIO_400 },
		{ "/Device/Mouse/x8.0", MOUSE_RATIO_800 },
	};
	gint i;

	for (i = 0; i < NELEMENTS(moveratio); i++) {
		if (kind == moveratio[i].num) {
			xmenu_select_item(&menu_hdl, moveratio[i].name);
			sysmng_update(SYS_UPDATEOSCFG);
			return;
		}
	}
	xmenu_select_item(&menu_hdl, "/Device/Mouse/x1.0");
	sysmng_update(SYS_UPDATEOSCFG);
}

/*
 * item function
 */
static void
exit_from_menu(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(action);
	UNUSED(w);

	gtk_widget_destroy(GTK_WIDGET(window));
}

static void
reset(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(action);
	UNUSED(w);

	pccore_cfgupdate();
	pccore_reset();
}

static void
rotate(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(w);

	changescreen((scrnmode & ~SCRNMODE_ROTATEMASK) | action);
}

/* ----- FDD */
static BOOL
fddopen_dialog_ok_cb(void *arg, const char *path)
{
	struct stat sb;
	int drive = (int)arg;
	BOOL result = FALSE;

	if (stat(path, &sb) == 0) {
		if (S_ISREG(sb.st_mode) && (sb.st_mode & S_IRUSR)) {
			file_cpyname(fddfolder, path, sizeof(fddfolder));
			diskdrv_setfdd(drive, path, !(sb.st_mode & S_IWUSR));
			sysmng_update(SYS_UPDATEOSCFG);
			result = TRUE;
		}
	}

	return result;
}

static void
fddopen(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(w);

	create_file_selection("Open FDD image", fddfolder, (void *)action,
	    fddopen_dialog_ok_cb, NULL);
}

static void
fddeject(gpointer data, guint action, GtkWidget *w)
{
	int i;

	UNUSED(data);
	UNUSED(w);

	if (action == (guint)~0) {
		for (i = 0; i < 4; i++)
			diskdrv_setfdd(i, NULL, FALSE);
			toolwin_setfdd(i, NULL);
	} else {
		diskdrv_setfdd(action, NULL, FALSE);
		toolwin_setfdd(action, NULL);
	}

}

/* ----- SASI */
static BOOL
sasiopen_dialog_ok_cb(void *arg, const char *path)
{
	struct stat sb;
	int drive = (int)arg;
	BOOL result = FALSE;

	if (stat(path, &sb) == 0) {
		if (S_ISREG(sb.st_mode) && (sb.st_mode & S_IRUSR)) {
			file_cpyname(hddfolder, path, sizeof(hddfolder));
			diskdrv_sethdd(drive, path);
			sysmng_update(SYS_UPDATEOSCFG);
			result = TRUE;
		}
	}

	return result;
}

static void
sasiopen(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(w);

	create_file_selection("Open SASI image", hddfolder, (void *)action,
	    sasiopen_dialog_ok_cb, NULL);
}

static void
sasiremove(gpointer data, guint action, GtkWidget *w)
{
	int i;

	UNUSED(data);
	UNUSED(w);

	if (action == (guint)~0) {
		for (i = 0; i < 2; i++)
			diskdrv_sethdd(i, "");
	} else {
		diskdrv_sethdd(action, "");
	}

}

/* ---- font */
static BOOL
font_change_dialog_ok_cb(void *arg, const char *path)
{
	struct stat sb;
	BOOL result = FALSE;

	UNUSED(arg);

	if (stat(path, &sb) == 0) {
		if (S_ISREG(sb.st_mode) && (sb.st_mode & S_IRUSR)) {
			if (font_load(path, FALSE)) {
				gdcs.textdisp |= GDCSCRN_ALLDRAW2;
				milstr_ncpy(np2cfg.fontfile, path, sizeof(np2cfg.fontfile));
				sysmng_update(SYS_UPDATECFG);
				result = TRUE;
			}
		}
	}

	return result;
}

static void
change_font(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(action);
	UNUSED(w);

	create_file_selection("Open font file", np2cfg.fontfile, 0,
	    font_change_dialog_ok_cb, NULL);
}

/* ---- new disk */
static BOOL
newdisk_dialog_ok_cb(void *arg, const char *path)
{
	struct stat sb;
	BOOL result = FALSE;

	if (stat(path, &sb) == 0) {
		if (S_ISREG(sb.st_mode)
		 && ((sb.st_mode & (S_IRUSR|S_IWUSR)) == (S_IRUSR|S_IWUSR))) {
			result = TRUE;
		}
	} else if (errno == ENOENT) {
		if (strlen(path) > 0) {
			result = TRUE;
		}
	}
	if (result) {
		create_newdisk_dialog(path, arg);
	}
	return result;
}

static void
new_disk(gpointer data, guint action, GtkWidget *w)
{
	char newdiskfolder[MAX_PATH];
	char *title;
	char *arg;

	UNUSED(data);
	UNUSED(w);

	switch (action) {
	case 0:
		title = "Create new floppy disk image file";
		arg = "d88";
		break;

	case 1:
		title = "Create new hard disk image file";
		arg = "thd";
		break;

	default:
		return;
	}

	milstr_ncpy(newdiskfolder, fddfolder, sizeof(newdiskfolder));
	file_cutname(newdiskfolder);
	file_setseparator(newdiskfolder, sizeof(newdiskfolder) - 1);
	milstr_ncat(newdiskfolder, "newdisk", sizeof(newdiskfolder));
	create_file_selection(title, newdiskfolder, arg,
	    newdisk_dialog_ok_cb, NULL);
}

static void
framerate(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(w);

	if (np2oscfg.DRAW_SKIP != action) {
		np2oscfg.DRAW_SKIP = action;
		sysmng_update(SYS_UPDATECFG);
	}
}

static void
keyboard(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(w);

	if (action >= 4)
		action = 0;

	if (np2cfg.KEY_MODE != action) {
		np2cfg.KEY_MODE = action;
		keystat_resetjoykey();
		sysmng_update(SYS_UPDATECFG);
	}
}

static void
f12(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(w);

	if (action >= 5)
		action = 0;

	if (np2oscfg.F12COPY != action) {
		np2oscfg.F12COPY = action;
		gtkkbd_resetf12();
		sysmng_update(SYS_UPDATEOSCFG);
	}
}

static void
beepvol(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(w);

	if (np2cfg.BEEP_VOL != action) {
		beep_setvol(action);
		sysmng_update(SYS_UPDATECFG);
	}
}

static void
sound_board(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(w);

	switch (action) {
	case NOSOUNDBOARD:
	case PC9801_86:
	case PC9801_14:
	case PC9801_26K:
	case PC9801_118:
	case CHIBI_OTO:
	case SPEAK_BOARD:
	case SPARK_BOARD:
	case AMD98_BOARD:
		/* XXX: ÆóËçº¹¤· */
		if (np2cfg.SOUND_SW != action) {
			np2cfg.SOUND_SW = action;
			sysmng_update(SYS_UPDATECFG);
		}
		break;
	}
}

static void
memory(gpointer data, guint action, GtkWidget *w)
{
	static const BYTE extmem[] = { 0, 1, 3, 5, 7, 9, 13 };

	UNUSED(data);
	UNUSED(w);

	if (action >= NELEMENTS(extmem))
		action = 1;

	if (np2cfg.EXTMEM != extmem[action]) {
		np2cfg.EXTMEM = extmem[action];
		sysmng_update(SYS_UPDATECFG);
	}
}

static void
mouse_ratio(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(w);

	switch (action) {
	case MOUSE_RATIO_050:
	case MOUSE_RATIO_075:
	case MOUSE_RATIO_100:
	case MOUSE_RATIO_150:
	case MOUSE_RATIO_200:
	case MOUSE_RATIO_400:
	case MOUSE_RATIO_800:
		if (action != np2oscfg.mouse_move_ratio) {
			mousemng_set_ratio(action);
			sysmng_update(SYS_UPDATEOSCFG);
		}
		break;
	}
}

static void
midi_panic(gpointer data, guint action, GtkWidget *w)
{

	UNUSED(data);
	UNUSED(action);
	UNUSED(w);

	rs232c_midipanic();
	mpu98ii_midipanic();
	pc9861k_midipanic();
}

/* ----- BMP save */
static BOOL
bmpsave_dialog_ok_cb(void *arg, const char *path)
{
	SCRNBMP bmp = (SCRNBMP)arg;
	BOOL result = FALSE;
	FILEH fh;

	file_cpyname(bmpfilefolder, path, sizeof(bmpfilefolder));
	sysmng_update(SYS_UPDATEOSCFG);
	fh = file_create(path);
	if (fh != FILEH_INVALID) {
		file_write(fh, bmp->ptr, bmp->size);
		file_close(fh);
		result = TRUE;
	}
	return result;
}

static void
bmpsave(gpointer data, guint action, GtkWidget *w)
{
	SCRNBMP bmp;

	UNUSED(data);
	UNUSED(action);
	UNUSED(w);

	bmp = scrnbmp();
	if (bmp) {
		create_file_selection("Save as bitmap file",
		    bmpfilefolder, (void *)bmp, bmpsave_dialog_ok_cb, NULL);
	}
}

/* ----- s98 logging */
static BOOL
s98logging_dialog_ok_cb(void *arg, const char *path)
{
	BOOL result = FALSE;

	UNUSED(arg);

	if (S98_open(path) == SUCCESS) {
		file_cpyname(bmpfilefolder, path, sizeof(bmpfilefolder));
		sysmng_update(SYS_UPDATEOSCFG);
		result = TRUE;
	}

	return result;
}

static void
s98logging_dialog_destroy_cb(void *arg, BOOL result)
{

	UNUSED(arg);

	if (!result)
		xmenu_toggle_item(S98_LOGGING, FALSE, TRUE);
}

/* ----- toggle item */
static void
toggle(gpointer data, guint action, GtkWidget *w)
{
	char work[MAX_PATH];
	char work2[64];

	UNUSED(data);
	UNUSED(w);

	if (!inited)
		return;

	switch (action) {
	case DISP_VSYNC:
		np2cfg.DISPSYNC = !np2cfg.DISPSYNC;
		xmenu_toggle_item(DISP_VSYNC, np2cfg.DISPSYNC, FALSE);
		break;

	case REAL_PALETTES:
		np2cfg.RASTER = !np2cfg.RASTER;
		xmenu_toggle_item(REAL_PALETTES, np2cfg.RASTER, FALSE);
		break;

	case NO_WAIT:
		np2oscfg.NOWAIT = !np2oscfg.NOWAIT;
		xmenu_toggle_item(NO_WAIT, np2oscfg.NOWAIT, FALSE);
		break;

	case SEEK_SOUND:
		np2cfg.MOTOR = !np2cfg.MOTOR;
		xmenu_toggle_item(SEEK_SOUND, np2cfg.MOTOR, TRUE);
		break;

	case MOUSE_MODE:
		mouse_running(MOUSE_XOR);
		np2oscfg.MOUSE_SW = !np2oscfg.MOUSE_SW;
		xmenu_toggle_item(MOUSE_MODE, np2oscfg.MOUSE_SW, FALSE);
		break;

	case CLOCK_DISP:
		np2oscfg.DISPCLK ^= 1;
		xmenu_toggle_item(CLOCK_DISP, np2oscfg.DISPCLK & 1, FALSE);
		sysmng_workclockrenewal();
		sysmng_updatecaption(3);
		break;

	case FRAME_DISP:
		np2oscfg.DISPCLK ^= 2;
		xmenu_toggle_item(FRAME_DISP, np2oscfg.DISPCLK & 2, FALSE);
		sysmng_workclockrenewal();
		sysmng_updatecaption(3);
		break;

	case XSHIFT_SHIFT:
		np2cfg.XSHIFT ^= 1;
		xmenu_toggle_item(XSHIFT_SHIFT, np2cfg.XSHIFT & 1, FALSE);
		keystat_forcerelease(0x70);
		break;

	case XSHIFT_CTRL:
		np2cfg.XSHIFT ^= 2;
		xmenu_toggle_item(XSHIFT_SHIFT, np2cfg.XSHIFT & 2, FALSE);
		keystat_forcerelease(0x74);
		break;

	case XSHIFT_GRPH:
		np2cfg.XSHIFT ^= 4;
		xmenu_toggle_item(XSHIFT_SHIFT, np2cfg.XSHIFT & 4, FALSE);
		keystat_forcerelease(0x73);
		break;

	case JOY_REVERSE:
		np2cfg.BTN_MODE = !np2cfg.BTN_MODE;
		xmenu_toggle_item(JOY_REVERSE, np2cfg.BTN_MODE, FALSE);
		break;

	case JOY_RAPID:
		np2cfg.BTN_RAPID = !np2cfg.BTN_RAPID;
		xmenu_toggle_item(JOY_RAPID, np2cfg.BTN_RAPID, FALSE);
		break;

	case MOUSE_RAPID:
		np2cfg.MOUSERAPID = !np2cfg.MOUSERAPID;
		xmenu_toggle_item(MOUSE_RAPID, np2cfg.MOUSERAPID, FALSE);
		break;

	case S98_LOGGING:
		xmenu_toggle_item(S98_LOGGING, !s98logging, FALSE);
		if (s98logging) {
			s98logging = FALSE;
		} else {
			s98logging = TRUE;
			file_cpyname(work, bmpfilefolder, sizeof(work));
			file_cutname(work);
			g_snprintf(work2, sizeof(work2), "np2_%04d.s98",
			    s98log_count++);
			if (s98log_count > 9999) {
				s98log_count = 0;
			}
			file_catname(work, work2, sizeof(work));
			create_file_selection("Save as S98 log", work, 0,
			    s98logging_dialog_ok_cb,
			    s98logging_dialog_destroy_cb);
		}
		break;

	case TOOL_WINDOW:
		np2oscfg.toolwin = !np2oscfg.toolwin;
		xmenu_toggle_item(TOOL_WINDOW, np2oscfg.toolwin, FALSE);
		if (np2oscfg.toolwin) {
			toolwin_create();
		} else {
			toolwin_destroy();
		}
		break;

	case KEY_DISPLAY:
		np2oscfg.keydisp = !np2oscfg.keydisp;
		xmenu_toggle_item(KEY_DISPLAY, np2oscfg.keydisp, FALSE);
		if (np2oscfg.keydisp) {
			kdispwin_create();
		} else {
			kdispwin_destroy();
		}
		break;

	case NUM_TOGGLE_ITEMS:
	default:
		break;
	}
}

#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"menu.h"


#define	MFCHECK(a) ((a)?MF_CHECKED:MF_UNCHECKED)


void disable_windowmenu(void) {

	HMENU	hmenu = GetMenu(hWndMain);

	EnableMenuItem(hmenu, IDM_WINDOW, MF_GRAYED);
	EnableMenuItem(hmenu, IDM_FULLSCREEN, MF_GRAYED);
}

void xmenu_setsound(BYTE value) {

	HMENU	hmenu = GetMenu(hWndMain);

	sysmng_update(SYS_UPDATESBOARD);
	np2cfg.SOUND_SW = value;
	CheckMenuItem(hmenu, IDM_NOSOUND, MFCHECK(np2cfg.SOUND_SW == 0));
	CheckMenuItem(hmenu, IDM_PC9801_14, MFCHECK(np2cfg.SOUND_SW & 1));
	CheckMenuItem(hmenu, IDM_PC9801_26K, MFCHECK(np2cfg.SOUND_SW & 2));
	CheckMenuItem(hmenu, IDM_PC9801_86, MFCHECK(np2cfg.SOUND_SW & 4));
	CheckMenuItem(hmenu, IDM_PC9801_118, MFCHECK(np2cfg.SOUND_SW & 8));
	CheckMenuItem(hmenu, IDM_CHIBIOTO, MFCHECK(np2cfg.SOUND_SW & 0x10));
	CheckMenuItem(hmenu, IDM_SPEAKBOARD, MFCHECK(np2cfg.SOUND_SW & 0x20));
	CheckMenuItem(hmenu, IDM_SPARKBOARD, MFCHECK(np2cfg.SOUND_SW & 0x40));
	CheckMenuItem(hmenu, IDM_AMD98, MFCHECK(np2cfg.SOUND_SW & 0x80));
}

void xmenu_setbeepvol(BYTE value) {

	HMENU	hmenu = GetMenu(hWndMain);

	np2cfg.BEEP_VOL = value & 3;
	CheckMenuItem(hmenu, IDM_BEEPOFF, MFCHECK(np2cfg.BEEP_VOL == 0));
	CheckMenuItem(hmenu, IDM_BEEPLOW, MFCHECK(np2cfg.BEEP_VOL == 1));
	CheckMenuItem(hmenu, IDM_BEEPMID, MFCHECK(np2cfg.BEEP_VOL == 2));
	CheckMenuItem(hmenu, IDM_BEEPHIGH, MFCHECK(np2cfg.BEEP_VOL == 3));
}

void xmenu_setkey(BYTE value) {

	HMENU	hmenu = GetMenu(hWndMain);

	if (value >= 3) {
		value = 0;
	}
	np2cfg.KEY_MODE = value;
	CheckMenuItem(hmenu, IDM_KEY, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_JOY1, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_JOY2, MFCHECK(value == 2));
}

void xmenu_setbtnrapid(BYTE value) {

	np2cfg.BTN_RAPID = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_RAPID, MFCHECK(np2cfg.BTN_RAPID));
}

void xmenu_setbtnmode(BYTE value) {

	np2cfg.BTN_MODE = value & 1;
	CheckMenuItem(GetMenu(hWndMain), IDM_JOYX, MFCHECK(np2cfg.BTN_MODE));
}

void xmenu_setmsrapid(BYTE value) {

	np2cfg.MOUSERAPID = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_MSRAPID, MFCHECK(np2cfg.MOUSERAPID));
}

void xmenu_setwaitflg(BYTE value) {

	np2oscfg.NOWAIT = value & 1;
	CheckMenuItem(GetMenu(hWndMain), IDM_NOWAIT, MFCHECK(np2oscfg.NOWAIT));
}

void xmenu_setframe(BYTE value) {

	HMENU	hmenu = GetMenu(hWndMain);

	np2oscfg.DRAW_SKIP = value;
	CheckMenuItem(hmenu, IDM_AUTOFPS, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_60FPS, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_30FPS, MFCHECK(value == 2));
	CheckMenuItem(hmenu, IDM_20FPS, MFCHECK(value == 3));
	CheckMenuItem(hmenu, IDM_15FPS, MFCHECK(value == 4));
}

void xmenu_setdispmode(BYTE value) {

	value &= 1;
	np2cfg.DISPSYNC = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_DISPSYNC, MFCHECK(value));
}

void xmenu_setmouse(BYTE value) {

	value &= 1;
	np2oscfg.MOUSE_SW = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_MOUSE, MFCHECK(value));
}

void xmenu_setmotorflg(BYTE value) {

	np2cfg.MOTOR = value & 1;
	CheckMenuItem(GetMenu(hWndMain), IDM_SEEKSND, MFCHECK(np2cfg.MOTOR));
}

void xmenu_seti286save(BYTE value) {

	if (!value) {
		EnableMenuItem(GetMenu(hWndMain), IDM_I286SAVE, MF_GRAYED);
	}
}

void xmenu_setroltate(BYTE value) {

	HMENU	hmenu = GetMenu(hWndMain);

	CheckMenuItem(hmenu, IDM_ROLNORMAL, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_ROLLEFT, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_ROLRIGHT, MFCHECK(value == 2));
}

void xmenu_sets98logging(BYTE value) {

	CheckMenuItem(GetMenu(hWndMain), IDM_S98LOGGING, MFCHECK(value));
}

void xmenu_setxshift(BYTE value) {

	HMENU	hmenu = GetMenu(hWndMain);

	np2cfg.XSHIFT = value;
	CheckMenuItem(hmenu, IDM_XSHIFT, MFCHECK(value & 1));
	CheckMenuItem(hmenu, IDM_XCTRL, MFCHECK(value & 2));
	CheckMenuItem(hmenu, IDM_XGRPH, MFCHECK(value & 4));
}

void xmenu_setf12copy(BYTE value) {

	HMENU	hmenu = GetMenu(hWndMain);

	if (value >= 3) {
		value = 0;
	}
	np2oscfg.F12COPY = value;
	CheckMenuItem(hmenu, IDM_F12MOUSE, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_F12COPY, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_F12STOP, MFCHECK(value == 2));
}

void xmenu_setextmem(BYTE value) {								// ver0.28

	HMENU	hmenu = GetMenu(hWndMain);

	sysmng_update(SYS_UPDATEMEMORY);
	np2cfg.EXTMEM = value;
	CheckMenuItem(hmenu, IDM_MEM640, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_MEM16, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_MEM36, MFCHECK(value == 3));
	CheckMenuItem(hmenu, IDM_MEM76, MFCHECK(value == 7));
}

void xmenu_setraster(BYTE value) {

	np2cfg.RASTER = (value & 1);
	CheckMenuItem(GetMenu(hWndMain), IDM_RASTER, MFCHECK(np2cfg.RASTER));
}


#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"pccore.h"
#include	"menu.h"


#define	MFCHECK(a) ((a)?MF_CHECKED:MF_UNCHECKED)

static const char menu_i286save[] = "&i286 save";


void xmenu_disablewindow(void) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	EnableMenuItem(hmenu, IDM_WINDOW, MF_GRAYED);
	EnableMenuItem(hmenu, IDM_FULLSCREEN, MF_GRAYED);
}

void xmenu_setroltate(BYTE value) {

	HMENU	hmenu;

	hmenu = GetMenu(hWndMain);
	CheckMenuItem(hmenu, IDM_ROLNORMAL, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_ROLLEFT, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_ROLRIGHT, MFCHECK(value == 2));
}

void xmenu_setdispmode(BYTE value) {

	value &= 1;
	np2cfg.DISPSYNC = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_DISPSYNC, MFCHECK(value));
}

void xmenu_setraster(BYTE value) {

	value &= 1;
	np2cfg.RASTER = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_RASTER, MFCHECK(value));
}

void xmenu_setwaitflg(BYTE value) {

	value &= 1;
	np2oscfg.NOWAIT = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_NOWAIT, MFCHECK(value));
}

void xmenu_setframe(BYTE value) {

	HMENU	hmenu;

	np2oscfg.DRAW_SKIP = value;
	hmenu = GetMenu(hWndMain);
	CheckMenuItem(hmenu, IDM_AUTOFPS, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_60FPS, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_30FPS, MFCHECK(value == 2));
	CheckMenuItem(hmenu, IDM_20FPS, MFCHECK(value == 3));
	CheckMenuItem(hmenu, IDM_15FPS, MFCHECK(value == 4));
}

void xmenu_setkey(BYTE value) {

	HMENU	hmenu;

	if (value >= 4) {
		value = 0;
	}
	np2cfg.KEY_MODE = value;
	hmenu = GetMenu(hWndMain);
	CheckMenuItem(hmenu, IDM_KEY, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_JOY1, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_JOY2, MFCHECK(value == 2));
}

void xmenu_setxshift(BYTE value) {

	HMENU	hmenu;

	np2cfg.XSHIFT = value;
	hmenu = GetMenu(hWndMain);
	CheckMenuItem(hmenu, IDM_XSHIFT, MFCHECK(value & 1));
	CheckMenuItem(hmenu, IDM_XCTRL, MFCHECK(value & 2));
	CheckMenuItem(hmenu, IDM_XGRPH, MFCHECK(value & 4));
}

void xmenu_setf12copy(BYTE value) {

	HMENU	hmenu;

	if (value >= 5) {
		value = 0;
	}
	np2oscfg.F12COPY = value;
	hmenu = GetMenu(hWndMain);
	CheckMenuItem(hmenu, IDM_F12MOUSE, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_F12COPY, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_F12STOP, MFCHECK(value == 2));
	CheckMenuItem(hmenu, IDM_F12EQU, MFCHECK(value == 3));
	CheckMenuItem(hmenu, IDM_F12COMMA, MFCHECK(value == 4));
}

void xmenu_setbeepvol(BYTE value) {

	HMENU	hmenu;

	value &= 3;
	np2cfg.BEEP_VOL = value;
	hmenu = GetMenu(hWndMain);
	CheckMenuItem(hmenu, IDM_BEEPOFF, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_BEEPLOW, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_BEEPMID, MFCHECK(value == 2));
	CheckMenuItem(hmenu, IDM_BEEPHIGH, MFCHECK(value == 3));
}

void xmenu_setsound(BYTE value) {

	HMENU	hmenu;

	sysmng_update(SYS_UPDATESBOARD);
	np2cfg.SOUND_SW = value;
	hmenu = GetMenu(hWndMain);
	CheckMenuItem(hmenu, IDM_NOSOUND, MFCHECK(value == 0x00));
	CheckMenuItem(hmenu, IDM_PC9801_14, MFCHECK(value == 0x01));
	CheckMenuItem(hmenu, IDM_PC9801_26K, MFCHECK(value == 0x02));
	CheckMenuItem(hmenu, IDM_PC9801_86, MFCHECK(value == 0x04));
	CheckMenuItem(hmenu, IDM_PC9801_26_86, MFCHECK(value == 0x06));
	CheckMenuItem(hmenu, IDM_PC9801_86_CB, MFCHECK(value == 0x14));
	CheckMenuItem(hmenu, IDM_PC9801_118, MFCHECK(value == 0x08));
	CheckMenuItem(hmenu, IDM_SPEAKBOARD, MFCHECK(value == 0x20));
	CheckMenuItem(hmenu, IDM_SPARKBOARD, MFCHECK(value == 0x40));
	CheckMenuItem(hmenu, IDM_AMD98, MFCHECK(value == 0x80));
}

void xmenu_setjastsound(BYTE value) {

	value &= 1;
	np2oscfg.jastsnd = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_JASTSOUND, MFCHECK(value));
}

void xmenu_setmotorflg(BYTE value) {

	value &= 1;
	np2cfg.MOTOR = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_SEEKWAIT, MFCHECK(value));
}

void xmenu_setextmem(BYTE value) {

	HMENU	hmenu;

	sysmng_update(SYS_UPDATEMEMORY);
	np2cfg.EXTMEM = value;
	hmenu = GetMenu(hWndMain);
	CheckMenuItem(hmenu, IDM_MEM640, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_MEM16, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_MEM36, MFCHECK(value == 3));
	CheckMenuItem(hmenu, IDM_MEM76, MFCHECK(value == 7));
}

void xmenu_setmouse(BYTE value) {

	value &= 1;
	np2oscfg.MOUSE_SW = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_MOUSE, MFCHECK(value));
}

void xmenu_sets98logging(BYTE value) {

	CheckMenuItem(GetMenu(hWndMain), IDM_S98LOGGING, MFCHECK(value));
}

void xmenu_setbtnmode(BYTE value) {

	value &= 1;
	np2cfg.BTN_MODE = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_JOYX, MFCHECK(value));
}

void xmenu_setbtnrapid(BYTE value) {

	value &= 1;
	np2cfg.BTN_RAPID = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_RAPID, MFCHECK(value));
}

void xmenu_setmsrapid(BYTE value) {

	value &= 1;
	np2cfg.MOUSERAPID = value;
	CheckMenuItem(GetMenu(hWndMain), IDM_MSRAPID, MFCHECK(value));
}

void xmenu_seti286save(BYTE value) {

	if (value) {
		InsertMenu(GetSubMenu(GetMenu(hWndMain), 6), 10,
					MF_BYPOSITION | MF_STRING, IDM_I286SAVE, menu_i286save);
	}
}


#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"pccore.h"


#define	MFCHECK(a) ((a)?MF_CHECKED:MF_UNCHECKED)


// ----

static const char smenu_memdump[] = "&Memory Dump";

void sysmenu_initialize(void) {

	HMENU	hMenu;

	if (np2oscfg.I286SAVE) {
		hMenu = GetSystemMenu(hWndMain, FALSE);
		InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, IDM_MEMORYDUMP,
							smenu_memdump);
		InsertMenu(hMenu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
	}
}


// ----

static const char xmenu_i286save[] = "&i286 save";
#if defined(SUPPORT_SCSI)
static const char xmenu_scsi[] = "SCSI #%d";
static const char xmenu_open[] = "&Open...";
static const char xmenu_remove[] = "&Remove";
#endif

#if defined(SUPPORT_SCSI)
static void addscsimenu(HMENU hMenu, UINT drv, UINT16 open, UINT16 eject) {

	HMENU	hSubMenu;
	char	buf[16];

	hSubMenu = CreatePopupMenu();
	AppendMenu(hSubMenu, MF_STRING, open, xmenu_open);
	AppendMenu(hSubMenu, MF_SEPARATOR, 0, NULL);
	AppendMenu(hSubMenu, MF_STRING, eject, xmenu_remove);

	SPRINTF(buf, xmenu_scsi, drv);
	AppendMenu(hMenu, MF_POPUP, (UINT32)hSubMenu, buf);
}
#endif

void xmenu_initialize(void) {

	HMENU	hMenu;
	HMENU	hSubMenu;

	hMenu = GetMenu(hWndMain);

#if defined(SUPPORT_SCSI)
	hSubMenu = GetSubMenu(hMenu, 3);
	AppendMenu(hSubMenu, MF_SEPARATOR, 0, NULL);
	addscsimenu(hSubMenu, 0, IDM_SCSI0OPEN, IDM_SCSI0EJECT);
	addscsimenu(hSubMenu, 1, IDM_SCSI1OPEN, IDM_SCSI1EJECT);
	addscsimenu(hSubMenu, 2, IDM_SCSI2OPEN, IDM_SCSI2EJECT);
	addscsimenu(hSubMenu, 3, IDM_SCSI3OPEN, IDM_SCSI3EJECT);
#endif

	if (np2oscfg.I286SAVE) {
		hSubMenu = GetSubMenu(hMenu, 6);
		InsertMenu(hSubMenu, 10,
					MF_BYPOSITION | MF_STRING, IDM_I286SAVE, xmenu_i286save);
	}
}

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

	if (value > 4) {
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
	CheckMenuItem(GetMenu(hWndMain), IDM_SEEKSND, MFCHECK(value));
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
	CheckMenuItem(hmenu, IDM_MEM116, MFCHECK(value == 11));
	CheckMenuItem(hmenu, IDM_MEM136, MFCHECK(value == 13));
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


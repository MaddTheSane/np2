#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"np2class.h"
#include	"pccore.h"


#define	MFCHECK(a) ((a)?MF_CHECKED:MF_UNCHECKED)

typedef struct {
const char	*str;
	int		id;
} SMENUITEM;

static const char smenu_toolwin[] = "&Tool Window";
static const char smenu_keydisp[] = "&Key display";
static const char smenu_center[] = "&Centering";
static const char smenu_snap[] = "&Window Snap";
static const char smenu_bg[] = "&Background";
static const char smenu_bgsnd[] = "Background &Sound";
static const char smenu_320x200[] = " 320x200";
static const char smenu_480x300[] = " 480x300";
static const char smenu_640x400[] = " 640x400";
static const char smenu_800x500[] = " 800x500";
static const char smenu_960x600[] = " 960x600";
static const char smenu_1280x800[] = "1280x600";

static const char smenu_memdump[] = "&Memory Dump";
static const char smenu_dbguty[] = "&Debug Utility";
static const char menu_i286save[] = "&i286 save";

static const SMENUITEM smenuitem[] = {
			{smenu_toolwin,		IDM_TOOLWIN},
			{smenu_keydisp,		IDM_KEYDISP},
			{NULL,				0},
			{smenu_center,		IDM_SCREENCENTER},
			{smenu_snap,		IDM_SNAPENABLE},
			{smenu_bg,			IDM_BACKGROUND},
			{smenu_bgsnd,		IDM_BGSOUND},
			{NULL,				0},
			{smenu_320x200,		IDM_SCRNMUL4},
			{smenu_480x300,		IDM_SCRNMUL6},
			{smenu_640x400,		IDM_SCRNMUL8},
			{smenu_800x500,		IDM_SCRNMUL10},
			{smenu_960x600,		IDM_SCRNMUL12},
			{smenu_1280x800,	IDM_SCRNMUL16},
			{NULL,				0}};

void sysmenu_initialize(void) {

	HMENU	hMenu;
	UINT	i;

	hMenu = GetSystemMenu(hWndMain, FALSE);
	for (i=0; i<(sizeof(smenuitem)/sizeof(SMENUITEM)); i++) {
		if (smenuitem[i].str) {
			InsertMenu(hMenu, i, MF_BYPOSITION | MF_STRING,
									smenuitem[i].id, smenuitem[i].str);
		}
		else {
			InsertMenu(hMenu, i, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
		}
	}
	if (np2oscfg.I286SAVE) {
		InsertMenu(hMenu, 7, MF_BYPOSITION | MF_STRING, IDM_MEMORYDUMP,
							smenu_memdump);
		InsertMenu(hMenu, 8, MF_BYPOSITION | MF_STRING, IDM_DEBUGUTY,
							smenu_dbguty);
	}
}

void sysmenu_settoolwin(BYTE value) {

	value &= 1;
	np2oscfg.toolwin = value;
	CheckMenuItem(GetSystemMenu(hWndMain, FALSE),
											IDM_TOOLWIN, MFCHECK(value));
}

void sysmenu_setkeydisp(BYTE value) {

	value &= 1;
	np2oscfg.keydisp = value;
	CheckMenuItem(GetSystemMenu(hWndMain, FALSE),
											IDM_KEYDISP, MFCHECK(value));
}

void sysmenu_setwinsnap(BYTE value) {

	value &= 1;
	np2oscfg.WINSNAP = value;
	CheckMenuItem(GetSystemMenu(hWndMain, FALSE),
											IDM_SNAPENABLE, MFCHECK(value));
}

void sysmenu_setbackground(BYTE value) {

	HMENU	hmenu;

	np2oscfg.background &= 2;
	np2oscfg.background |= (value & 1);
	hmenu = GetSystemMenu(hWndMain, FALSE);
	if (value & 1) {
		CheckMenuItem(hmenu, IDM_BACKGROUND, MF_UNCHECKED);
		EnableMenuItem(hmenu, IDM_BGSOUND, MF_GRAYED);
	}
	else {
		CheckMenuItem(hmenu, IDM_BACKGROUND, MF_CHECKED);
		EnableMenuItem(hmenu, IDM_BGSOUND, MF_ENABLED);
	}
}

void sysmenu_setbgsound(BYTE value) {

	np2oscfg.background &= 1;
	np2oscfg.background |= (value & 2);
	CheckMenuItem(GetSystemMenu(hWndMain, FALSE),
									IDM_BGSOUND, MFCHECK((value & 2) ^ 2));
}

void sysmenu_setscrnmul(BYTE value) {

	HMENU	hmenu;

//	np2cfg.scrnmul = value;
	hmenu = GetSystemMenu(hWndMain, FALSE);
	CheckMenuItem(hmenu, IDM_SCRNMUL4, MFCHECK(value == 4));
	CheckMenuItem(hmenu, IDM_SCRNMUL6, MFCHECK(value == 6));
	CheckMenuItem(hmenu, IDM_SCRNMUL8, MFCHECK(value == 8));
	CheckMenuItem(hmenu, IDM_SCRNMUL10, MFCHECK(value == 10));
	CheckMenuItem(hmenu, IDM_SCRNMUL12, MFCHECK(value == 12));
	CheckMenuItem(hmenu, IDM_SCRNMUL16, MFCHECK(value == 16));
}


// ----

// ‚±‚ê‚Á‚ÄAPI‚ ‚é‚Ì‚©H
void menu_addmenubar(HMENU popup, HMENU menubar) {

	UINT			cnt;
	UINT			pos;
	UINT			i;
	MENUITEMINFO	mii;
	char			str[128];
	HMENU			hSubMenu;

	cnt = GetMenuItemCount(menubar);
	pos = 0;
	for (i=0; i<cnt; i++) {
		ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU |
																	MIIM_DATA;
		mii.dwTypeData = str;
		mii.cch = sizeof(str);
		if (GetMenuItemInfo(menubar, i, TRUE, &mii)) {
			if (mii.hSubMenu) {
				hSubMenu = CreatePopupMenu();
				menu_addmenubar(hSubMenu, mii.hSubMenu);
				mii.hSubMenu = hSubMenu;
			}
			InsertMenuItem(popup, pos, TRUE, &mii);
			pos++;
		}
	}
}


// ----

void xmenu_disablewindow(void) {

	HMENU	hmenu;

	hmenu = np2class_gethmenu(hWndMain);
	EnableMenuItem(hmenu, IDM_WINDOW, MF_GRAYED);
	EnableMenuItem(hmenu, IDM_FULLSCREEN, MF_GRAYED);
}

void xmenu_setroltate(BYTE value) {

	HMENU	hmenu;

	hmenu = np2class_gethmenu(hWndMain);
	CheckMenuItem(hmenu, IDM_ROLNORMAL, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_ROLLEFT, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_ROLRIGHT, MFCHECK(value == 2));
}

void xmenu_setdispmode(BYTE value) {

	value &= 1;
	np2cfg.DISPSYNC = value;
	CheckMenuItem(np2class_gethmenu(hWndMain), IDM_DISPSYNC, MFCHECK(value));
}

void xmenu_setraster(BYTE value) {

	value &= 1;
	np2cfg.RASTER = value;
	CheckMenuItem(np2class_gethmenu(hWndMain), IDM_RASTER, MFCHECK(value));
}

void xmenu_setwaitflg(BYTE value) {

	value &= 1;
	np2oscfg.NOWAIT = value;
	CheckMenuItem(np2class_gethmenu(hWndMain), IDM_NOWAIT, MFCHECK(value));
}

void xmenu_setframe(BYTE value) {

	HMENU	hmenu;

	np2oscfg.DRAW_SKIP = value;
	hmenu = np2class_gethmenu(hWndMain);
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
	hmenu = np2class_gethmenu(hWndMain);
	CheckMenuItem(hmenu, IDM_KEY, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_JOY1, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_JOY2, MFCHECK(value == 2));
}

void xmenu_setxshift(BYTE value) {

	HMENU	hmenu;

	np2cfg.XSHIFT = value;
	hmenu = np2class_gethmenu(hWndMain);
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
	hmenu = np2class_gethmenu(hWndMain);
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
	hmenu = np2class_gethmenu(hWndMain);
	CheckMenuItem(hmenu, IDM_BEEPOFF, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_BEEPLOW, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_BEEPMID, MFCHECK(value == 2));
	CheckMenuItem(hmenu, IDM_BEEPHIGH, MFCHECK(value == 3));
}

void xmenu_setsound(BYTE value) {

	HMENU	hmenu;

	sysmng_update(SYS_UPDATESBOARD);
	np2cfg.SOUND_SW = value;
	hmenu = np2class_gethmenu(hWndMain);
	CheckMenuItem(hmenu, IDM_NOSOUND, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_PC9801_14, MFCHECK(value & 1));
	CheckMenuItem(hmenu, IDM_PC9801_26K, MFCHECK(value & 2));
	CheckMenuItem(hmenu, IDM_PC9801_86, MFCHECK(value & 4));
	CheckMenuItem(hmenu, IDM_PC9801_118, MFCHECK(value & 8));
	CheckMenuItem(hmenu, IDM_CHIBIOTO, MFCHECK(value & 0x10));
	CheckMenuItem(hmenu, IDM_SPEAKBOARD, MFCHECK(value & 0x20));
	CheckMenuItem(hmenu, IDM_SPARKBOARD, MFCHECK(value & 0x40));
	CheckMenuItem(hmenu, IDM_AMD98, MFCHECK(value & 0x80));
}

void xmenu_setmotorflg(BYTE value) {

	value &= 1;
	np2cfg.MOTOR = value;
	CheckMenuItem(np2class_gethmenu(hWndMain), IDM_SEEKSND, MFCHECK(value));
}

void xmenu_setextmem(BYTE value) {

	HMENU	hmenu;

	sysmng_update(SYS_UPDATEMEMORY);
	np2cfg.EXTMEM = value;
	hmenu = np2class_gethmenu(hWndMain);
	CheckMenuItem(hmenu, IDM_MEM640, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_MEM16, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_MEM36, MFCHECK(value == 3));
	CheckMenuItem(hmenu, IDM_MEM76, MFCHECK(value == 7));
}

void xmenu_setmouse(BYTE value) {

	value &= 1;
	np2oscfg.MOUSE_SW = value;
	CheckMenuItem(np2class_gethmenu(hWndMain), IDM_MOUSE, MFCHECK(value));
}

void xmenu_sets98logging(BYTE value) {

	CheckMenuItem(np2class_gethmenu(hWndMain),
											IDM_S98LOGGING, MFCHECK(value));
}

void xmenu_setshortcut(BYTE value) {

	HMENU	hmenu;

	np2oscfg.shortcut = value;
	hmenu = np2class_gethmenu(hWndMain);
	CheckMenuItem(hmenu, IDM_ALTENTER, MFCHECK(value & 1));
	CheckMenuItem(hmenu, IDM_ALTF4, MFCHECK(value & 2));
}

void xmenu_setdispclk(BYTE value) {

	HMENU	hmenu;

	value &= 3;
	np2oscfg.DISPCLK = value;
	hmenu = np2class_gethmenu(hWndMain);
	CheckMenuItem(hmenu, IDM_DISPCLOCK, MFCHECK(value & 1));
	CheckMenuItem(hmenu, IDM_DISPFRAME, MFCHECK(value & 2));
	sysmng_workclockrenewal();
	sysmng_updatecaption(3);
}

void xmenu_setbtnmode(BYTE value) {

	value &= 1;
	np2cfg.BTN_MODE = value;
	CheckMenuItem(np2class_gethmenu(hWndMain), IDM_JOYX, MFCHECK(value));
}

void xmenu_setbtnrapid(BYTE value) {

	value &= 1;
	np2cfg.BTN_RAPID = value;
	CheckMenuItem(np2class_gethmenu(hWndMain), IDM_RAPID, MFCHECK(value));
}

void xmenu_setmsrapid(BYTE value) {

	value &= 1;
	np2cfg.MOUSERAPID = value;
	CheckMenuItem(np2class_gethmenu(hWndMain), IDM_MSRAPID, MFCHECK(value));
}

void xmenu_setsstp(BYTE value) {

	value &= 1;
	np2oscfg.sstp = value;
	CheckMenuItem(np2class_gethmenu(hWndMain), IDM_SSTP, MFCHECK(value));
}

void xmenu_seti286save(BYTE value) {

	if (value) {
		InsertMenu(GetSubMenu(np2class_gethmenu(hWndMain), 6), 10,
					MF_BYPOSITION | MF_STRING, IDM_I286SAVE, menu_i286save);
	}
}


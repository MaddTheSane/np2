#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"sysmng.h"
#include	"menu.h"
#include	"np2class.h"
#include	"pccore.h"



#define	MFCHECK(a) ((a)?MF_CHECKED:MF_UNCHECKED)

typedef struct {
	UINT16	id;
	UINT16	str;
} MENUITEMS;


// Ç±ÇÍÇ¡ÇƒAPIÇ†ÇÈÇÃÇ©ÅH
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

static void insertresmenu(HMENU menu, UINT pos, UINT flag,
													UINT item, UINT str) {

	char	tmp[128];

	if (LoadString(hInst, str, tmp, sizeof(tmp))) {
		InsertMenu(menu, pos, flag, item, tmp);
	}
}

static void insertresmenus(HMENU menu, UINT pos,
									const MENUITEMS *item, UINT items) {

const MENUITEMS *iterm;

	iterm = item + items;
	while(item < iterm) {
		if (item->id) {
			insertresmenu(menu, pos, MF_BYPOSITION | MF_STRING,
													item->id, item->str);
		}
		else {
			InsertMenu(menu, pos, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
		}
		item++;
		pos++;
	}
}


// ----

static const MENUITEMS smenuitem[] = {
			{IDM_TOOLWIN,		IDS_TOOLWIN},
			{IDM_KEYDISP,		IDS_KEYDISP},
			{0,					0},
			{IDM_SCREENCENTER,	IDS_SCREENCENTER},
			{IDM_SNAPENABLE,	IDS_SNAPENABLE},
			{IDM_BACKGROUND,	IDS_BACKGROUND},
			{IDM_BGSOUND,		IDS_BGSOUND},
			{0,					0},
			{IDM_SCRNMUL4,		IDS_SCRNMUL4},
			{IDM_SCRNMUL6,		IDS_SCRNMUL6},
			{IDM_SCRNMUL8,		IDS_SCRNMUL8},
			{IDM_SCRNMUL10,		IDS_SCRNMUL10},
			{IDM_SCRNMUL12,		IDS_SCRNMUL12},
			{IDM_SCRNMUL16,		IDS_SCRNMUL16},
			{0,					0}};

static const MENUITEMS smenuitem2[] = {
			{IDM_MEMORYDUMP,	IDS_MEMORYDUMP},
			{IDM_DEBUGUTY,		IDS_DEBUGUTY}};


void sysmenu_initialize(void) {

	HMENU	hMenu;

	hMenu = GetSystemMenu(hWndMain, FALSE);
	insertresmenus(hMenu, 0, smenuitem, sizeof(smenuitem)/sizeof(MENUITEMS));
	if (np2oscfg.I286SAVE) {
		insertresmenus(hMenu, 7, smenuitem2,
										sizeof(smenuitem2)/sizeof(MENUITEMS));
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

typedef struct {
	UINT16		title;
	MENUITEMS	item[3];
} DISKMENU;

static const DISKMENU fddmenu[4] = {
		{IDS_FDD1,	{{IDM_FDD1OPEN,		IDS_OPEN},
					{0,					0},
					{IDM_FDD1EJECT,		IDS_EJECT}}},
		{IDS_FDD2,	{{IDM_FDD2OPEN,		IDS_OPEN},
					{0,					0},
					{IDM_FDD2EJECT,		IDS_EJECT}}},
		{IDS_FDD3,	{{IDM_FDD3OPEN,		IDS_OPEN},
					{0,					0},
					{IDM_FDD3EJECT,		IDS_EJECT}}},
		{IDS_FDD4,	{{IDM_FDD4OPEN,		IDS_OPEN},
					{0,					0},
					{IDM_FDD4EJECT,		IDS_EJECT}}}};

static void insdiskmenu(HMENU hMenu, UINT pos, const DISKMENU *m) {

	HMENU	hSubMenu;

	hSubMenu = CreatePopupMenu();
	insertresmenus(hSubMenu, 0, m->item, 3);
	insertresmenu(hMenu, pos, MF_BYPOSITION | MF_POPUP,
											(UINT)hSubMenu, m->title);
}

#if defined(SUPPORT_SCSI)
static const DISKMENU scsimenu[4] = {
		{IDS_SCSI0,	{{IDM_SCSI0OPEN,	IDS_OPEN},
					{0,					0},
					{IDM_SCSI0EJECT,	IDS_REMOVE}}},
		{IDS_SCSI1,	{{IDM_SCSI1OPEN,	IDS_OPEN},
					{0,					0},
					{IDM_SCSI1EJECT,	IDS_REMOVE}}},
		{IDS_SCSI2,	{{IDM_SCSI2OPEN,	IDS_OPEN},
					{0,					0},
					{IDM_SCSI2EJECT,	IDS_REMOVE}}},
		{IDS_SCSI3,	{{IDM_SCSI3OPEN,	IDS_OPEN},
					{0,					0},
					{IDM_SCSI3EJECT,	IDS_REMOVE}}}};
#endif

#if defined(SUPPORT_STATSAVE)
static const char xmenu_stat[] = "S&tat";
static const char xmenu_statsave[] = "Save %u";
static const char xmenu_statload[] = "Load %u";

static void addstatsavemenu(HMENU hMenu, UINT pos) {

	HMENU	hSubMenu;
	UINT	i;
	char	buf[16];

	hSubMenu = CreatePopupMenu();
	for (i=0; i<SUPPORT_STATSAVE; i++) {
		SPRINTF(buf, xmenu_statsave, i);
		AppendMenu(hSubMenu, MF_STRING, IDM_FLAGSAVE + i, buf);
	}
	AppendMenu(hSubMenu, MF_MENUBARBREAK, 0, NULL);
	for (i=0; i<SUPPORT_STATSAVE; i++) {
		SPRINTF(buf, xmenu_statload, i);
		AppendMenu(hSubMenu, MF_STRING, IDM_FLAGLOAD + i, buf);
	}
	InsertMenu(hMenu, pos, MF_BYPOSITION | MF_POPUP,
											(UINT32)hSubMenu, xmenu_stat);
}
#endif

void xmenu_initialize(void) {

	HMENU	hMenu;
	HMENU	hSubMenu;
	UINT	i;

	hMenu = np2class_gethmenu(hWndMain);
	if (np2oscfg.I286SAVE) {
		hSubMenu = GetSubMenu(hMenu, 4);
		insertresmenu(hSubMenu, 10, MF_BYPOSITION | MF_STRING,
												IDM_I286SAVE, IDS_I286SAVE);
	}

#if defined(SUPPORT_SCSI)
	hSubMenu = GetSubMenu(hMenu, 1);
	AppendMenu(hSubMenu, MF_SEPARATOR, 0, NULL);
	for (i=0; i<4; i++) {
		insdiskmenu(hSubMenu, i + 3, scsimenu + i);
	}
#endif

	for (i=4; i--;) {
		if (np2cfg.fddequip & (1 << i)) {
			insdiskmenu(hMenu, 1, fddmenu + i);
		}
	}

#if defined(SUPPORT_STATSAVE)
	if (np2oscfg.statsave) {
		addstatsavemenu(hMenu, 1);
	}
#endif
}

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

	if (value > 6) {
		value = 0;
	}
	np2oscfg.F12COPY = value;
	hmenu = np2class_gethmenu(hWndMain);
	CheckMenuItem(hmenu, IDM_F12MOUSE, MFCHECK(value == 0));
	CheckMenuItem(hmenu, IDM_F12COPY, MFCHECK(value == 1));
	CheckMenuItem(hmenu, IDM_F12STOP, MFCHECK(value == 2));
	CheckMenuItem(hmenu, IDM_F12EQU, MFCHECK(value == 3));
	CheckMenuItem(hmenu, IDM_F12COMMA, MFCHECK(value == 4));
	CheckMenuItem(hmenu, IDM_USERKEY1, MFCHECK(value == 5));
	CheckMenuItem(hmenu, IDM_USERKEY2, MFCHECK(value == 6));
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
	CheckMenuItem(np2class_gethmenu(hWndMain), IDM_JASTSOUND, MFCHECK(value));
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


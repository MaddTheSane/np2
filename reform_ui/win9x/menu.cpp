/**
 * @file	menu.cpp
 * @brief	���j���[�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#include "compiler.h"
#include "resource.h"
#include "menu.h"
#include "np2.h"
#include "scrnmng.h"
#include "sysmng.h"
#include "misc\tstring.h"
#include "dialog\np2class.h"
#include "pccore.h"

BOOL menu_searchmenu(HMENU hMenu, UINT uID, HMENU *phmenuRet, int *pnPos)
{
	int nCount = GetMenuItemCount(hMenu);
	for (int i = 0; i < nCount; i++)
	{
		MENUITEMINFO mii;
		ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_ID | MIIM_SUBMENU;
		if (GetMenuItemInfo(hMenu, i, TRUE, &mii))
		{
			if (mii.wID == uID)
			{
				if (phmenuRet)
				{
					*phmenuRet = hMenu;
				}
				if (pnPos)
				{
					*pnPos = i;
				}
				return TRUE;
			}
			else if ((mii.hSubMenu) &&
					(menu_searchmenu(mii.hSubMenu, uID, phmenuRet, pnPos)))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

#if 0
static BOOL searchsubmenu(HMENU hMenu, HMENU hmenuTarget,
												HMENU *phmenuRet, int *pnPos)
{
	int				nCount;
	int				i;
	MENUITEMINFO	mii;

	nCount = GetMenuItemCount(hMenu);
	for (i=0; i<nCount; i++)
	{
		ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_SUBMENU;
		if ((GetMenuItemInfo(hMenu, i, TRUE, &mii)) && (mii.hSubMenu))
		{
			if (mii.hSubMenu == hmenuTarget)
			{
				if (phmenuRet)
				{
					*phmenuRet = hMenu;
				}
				if (pnPos)
				{
					*pnPos = i;
				}
				return TRUE;
			}
			if (searchsubmenu(mii.hSubMenu, hmenuTarget, phmenuRet, pnPos))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
#endif	// 0

// �������API����̂��H
int menu_addmenu(HMENU hMenu, int nPos, HMENU hmenuAdd, BOOL bSeparator)
{
	if (nPos < 0)
	{
		nPos = GetMenuItemCount(hMenu);
	}
	int nCount = GetMenuItemCount(hmenuAdd);
	int nAdded = 0;
	for (int i = 0; i < nCount; i++)
	{
		MENUITEMINFO mii;
		ZeroMemory(&mii, sizeof(mii));

		TCHAR szString[128];
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_DATA;
		mii.dwTypeData = szString;
		mii.cch = _countof(szString);
		if (GetMenuItemInfo(hmenuAdd, i, TRUE, &mii))
		{
			if (mii.hSubMenu)
			{
				HMENU hmenuSub = CreatePopupMenu();
				(void)menu_addmenu(hmenuSub, 0, mii.hSubMenu, FALSE);
				mii.hSubMenu = hmenuSub;
			}
			if (bSeparator)
			{
				bSeparator = FALSE;
				InsertMenu(hMenu, nPos + nAdded, MF_BYPOSITION | MF_SEPARATOR,
																	0, NULL);
				nAdded++;
			}
			InsertMenuItem(hMenu, nPos + nAdded, TRUE, &mii);
			nAdded++;
		}
	}
	return nAdded;
}

int menu_addmenures(HMENU hMenu, int nPos, UINT uID, BOOL bSeparator)
{
	int nCount = 0;
	HMENU hmenuAdd = LoadMenu(g_hInstance, MAKEINTRESOURCE(uID));
	if (hmenuAdd)
	{
		nCount = menu_addmenu(hMenu, nPos, hmenuAdd, bSeparator);
		DestroyMenu(hmenuAdd);
	}
	return nCount;
}

int menu_addmenubyid(HMENU hMenu, UINT uByID, UINT uID)
{
	int nCount = 0;

	HMENU hmenuSub;
	int nSubPos;
	if (menu_searchmenu(hMenu, uByID, &hmenuSub, &nSubPos))
	{
		nCount = menu_addmenures(hmenuSub, nSubPos + 1, uID, FALSE);
	}
	return nCount;
}

BOOL menu_insertmenures(HMENU hMenu, int nPosition, UINT uFlags, UINT_PTR uIDNewItem, UINT uID)
{
	std::tstring rString(LoadTString(uID));

	BOOL bResult = FALSE;
	if (!rString.empty())
	{
		bResult = InsertMenu(hMenu, nPosition, uFlags, uIDNewItem, rString.c_str());
	}
	return bResult;
}

void menu_addmenubar(HMENU popup, HMENU menubar)
{
	(void)menu_addmenu(popup, 0, menubar, FALSE);
}


// ----

/**
 * ���j���[������
 */
void sysmenu_initialize()
{
	HMENU hMenu = GetSystemMenu(g_hWndMain, FALSE);
	UINT uPos = 0;

#if defined(SUPPORT_KEYDISP)
	uPos += menu_addmenures(hMenu, uPos, IDR_SYSKEYDISP, FALSE);
#endif
#if defined(SUPPORT_SOFTKBD)
	uPos += menu_addmenures(hMenu, uPos, IDR_SYSSOFTKBD, FALSE);
#endif

	uPos += menu_addmenures(hMenu, uPos, IDR_SYS, FALSE);
	if (np2oscfg.I286SAVE)
	{
#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
		uPos += menu_addmenures(hMenu, uPos, IDR_SYSDEBUG32, FALSE);
#endif
		uPos += menu_addmenures(hMenu, uPos, IDR_SYSDEBUG, FALSE);
	}
}

void sysmenu_settoolwin(UINT8 value) {

	value &= 1;
	np2oscfg.toolwin = value;
	CheckMenuItem(GetSystemMenu(g_hWndMain, FALSE),
											IDM_TOOLWIN, MFCHECK(value));
}

void sysmenu_setkeydisp(UINT8 value) {

	value &= 1;
	np2oscfg.keydisp = value;
	CheckMenuItem(GetSystemMenu(g_hWndMain, FALSE),
											IDM_KEYDISP, MFCHECK(value));
}

void sysmenu_setwinsnap(UINT8 value) {

	value &= 1;
	np2oscfg.WINSNAP = value;
	CheckMenuItem(GetSystemMenu(g_hWndMain, FALSE),
											IDM_SNAPENABLE, MFCHECK(value));
}

void sysmenu_setbackground(UINT8 value) {

	HMENU	hmenu;

	np2oscfg.background &= 2;
	np2oscfg.background |= (value & 1);
	hmenu = GetSystemMenu(g_hWndMain, FALSE);
	if (value & 1) {
		CheckMenuItem(hmenu, IDM_BACKGROUND, MF_UNCHECKED);
		EnableMenuItem(hmenu, IDM_BGSOUND, MF_GRAYED);
	}
	else {
		CheckMenuItem(hmenu, IDM_BACKGROUND, MF_CHECKED);
		EnableMenuItem(hmenu, IDM_BGSOUND, MF_ENABLED);
	}
}

void sysmenu_setbgsound(UINT8 value) {

	np2oscfg.background &= 1;
	np2oscfg.background |= (value & 2);
	CheckMenuItem(GetSystemMenu(g_hWndMain, FALSE),
									IDM_BGSOUND, MFCHECK((value & 2) ^ 2));
}

void sysmenu_setscrnmul(UINT8 value) {

	HMENU	hmenu;

//	np2cfg.scrnmul = value;
	hmenu = GetSystemMenu(g_hWndMain, FALSE);
	CheckMenuItem(hmenu, IDM_SCRNMUL4, MFCHECK(value == 4));
	CheckMenuItem(hmenu, IDM_SCRNMUL6, MFCHECK(value == 6));
	CheckMenuItem(hmenu, IDM_SCRNMUL8, MFCHECK(value == 8));
	CheckMenuItem(hmenu, IDM_SCRNMUL10, MFCHECK(value == 10));
	CheckMenuItem(hmenu, IDM_SCRNMUL12, MFCHECK(value == 12));
	CheckMenuItem(hmenu, IDM_SCRNMUL16, MFCHECK(value == 16));
}


// ----

/**
 * ������
 */
void xmenu_initialize(HMENU hMenu)
{
	int nPos = 1;
#if defined(SUPPORT_STATSAVE)
	if (np2oscfg.statsave)
	{
		nPos += menu_addmenures(hMenu, nPos, IDR_STAT, FALSE);
	}
#endif

	for (UINT i = 0; i < 4; i++)
	{
		if (np2cfg.fddequip & (1 << i))
		{
			nPos += menu_addmenures(hMenu, nPos, IDR_FDD1MENU + i, FALSE);
		}
	}

	HMENU hmenuSub = CreatePopupMenu();
	if (hmenuSub)
	{
		int nSubPos = 0;
#if defined(SUPPORT_IDEIO)
		nSubPos += menu_addmenures(hmenuSub, nSubPos, IDR_IDEMENU, FALSE);
#else
		nSubPos += menu_addmenures(hmenuSub, nSubPos, IDR_SASIMENU, FALSE);
#endif
#if defined(SUPPORT_SCSI)
		nSubPos += menu_addmenures(hmenuSub, nSubPos, IDR_SCSIMENU, TRUE);
#endif
		menu_insertmenures(hMenu, nPos, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hmenuSub, IDS_HDD);
	}

#if defined(SUPPORT_PX)
	(void)menu_addmenubyid(hMenu, IDM_SPARKBOARD, IDR_PXMENU);
#endif

#if defined(SUPPORT_WAVEREC)
	(void)menu_addmenubyid(hMenu, IDM_S98LOGGING, IDR_WAVEREC);
#endif

	if (np2oscfg.I286SAVE)
	{
#if defined(SUPPORT_PC9821)
		(void)menu_addmenubyid(hMenu, IDM_MSRAPID, IDR_CPUSAVE32);
#else	//	defined(SUPPORT_PC9821)
		(void)menu_addmenubyid(hMenu, IDM_MSRAPID, IDR_CPUSAVE16);
#endif	//	defined(SUPPORT_PC9821)
	}
}

/**
 * ���j���[��Ԃ��X�V����
 * @param[in] hMenu ���j���[ �n���h��
 */
void xmenu_update(HMENU hMenu)
{
	if (hMenu == NULL)
	{
		return;
	}

	// Screen
	const bool bFullScreen = ((g_scrnmode & SCRNMODE_FULLSCREEN) != 0);
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_WINDOW, MFCHECK(!bFullScreen));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_FULLSCREEN, MFCHECK(bFullScreen));
	const UINT8 nRotateMode = g_scrnmode & SCRNMODE_ROTATEMASK;
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_ROLNORMAL, MFCHECK(nRotateMode == 0));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_ROLLEFT, MFCHECK(nRotateMode == SCRNMODE_ROTATELEFT));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_ROLRIGHT, MFCHECK(nRotateMode == SCRNMODE_ROTATERIGHT));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_DISPSYNC, MFCHECK(np2cfg.DISPSYNC));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_RASTER, MFCHECK(np2cfg.RASTER));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_NOWAIT, MFCHECK(np2oscfg.NOWAIT));
	const UINT8 DRAW_SKIP = np2oscfg.DRAW_SKIP;
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_AUTOFPS, MFCHECK(DRAW_SKIP == 0));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_60FPS, MFCHECK(DRAW_SKIP == 1));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_30FPS, MFCHECK(DRAW_SKIP == 2));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_20FPS, MFCHECK(DRAW_SKIP == 3));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_15FPS, MFCHECK(DRAW_SKIP == 4));

	// Device-Keyboard
	const UINT8 KEY_MODE = np2cfg.KEY_MODE;
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_KEY, MFCHECK(KEY_MODE == 0));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_JOY1, MFCHECK(KEY_MODE == 1));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_JOY2, MFCHECK(KEY_MODE == 2));
	const UINT8 XSHIFT = np2cfg.XSHIFT;
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_XSHIFT, MFCHECK(XSHIFT & 1));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_XCTRL, MFCHECK(XSHIFT & 2));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_XGRPH, MFCHECK(XSHIFT & 4));
	const UINT8 F12COPY = np2oscfg.F12COPY;
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_F12MOUSE, MFCHECK(F12COPY == 0));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_F12COPY, MFCHECK(F12COPY == 1));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_F12STOP, MFCHECK(F12COPY == 2));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_F12EQU, MFCHECK(F12COPY == 3));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_F12COMMA, MFCHECK(F12COPY == 4));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_USERKEY1, MFCHECK(F12COPY == 5));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_USERKEY2, MFCHECK(F12COPY == 6));

	// Device-Sound
	const UINT8 BEEP_VOL = np2cfg.BEEP_VOL;
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_BEEPOFF, MFCHECK(BEEP_VOL == 0));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_BEEPLOW, MFCHECK(BEEP_VOL == 1));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_BEEPMID, MFCHECK(BEEP_VOL == 2));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_BEEPHIGH, MFCHECK(BEEP_VOL == 3));
	const UINT8 SOUND_SW = np2cfg.SOUND_SW;
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_NOSOUND, MFCHECK(SOUND_SW == 0x00));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_PC9801_14, MFCHECK(SOUND_SW == 0x01));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_PC9801_26K, MFCHECK(SOUND_SW == 0x02));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_PC9801_86, MFCHECK(SOUND_SW == 0x04));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_PC9801_26_86, MFCHECK(SOUND_SW == 0x06));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_PC9801_86_CB, MFCHECK(SOUND_SW == 0x14));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_PC9801_118, MFCHECK(SOUND_SW == 0x08));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_SPEAKBOARD, MFCHECK(SOUND_SW == 0x20));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_SPARKBOARD, MFCHECK(SOUND_SW == 0x40));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_AMD98, MFCHECK(SOUND_SW == 0x80));
#if defined(SUPPORT_PX)
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_PX1, MFCHECK(SOUND_SW == 0x30));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_PX2, MFCHECK(SOUND_SW == 0x50));
#endif	// defined(SUPPORT_PX)
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_JASTSOUND, MFCHECK(np2oscfg.jastsnd));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_SEEKSND, MFCHECK(np2cfg.MOTOR));

	// Device-Memory
	const UINT8 EXTMEM = np2cfg.EXTMEM;
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_MEM640, MFCHECK(EXTMEM == 0));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_MEM16, MFCHECK(EXTMEM == 1));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_MEM36, MFCHECK(EXTMEM == 3));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_MEM76, MFCHECK(EXTMEM == 7));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_MEM116, MFCHECK(EXTMEM == 11));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_MEM136, MFCHECK(EXTMEM == 13));

	// Device
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_MOUSE, MFCHECK(np2oscfg.MOUSE_SW));

	// Other-ShortcutKey
	const UINT8 shortcut = np2oscfg.shortcut;
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_ALTENTER, MFCHECK(shortcut & 1));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_ALTF4, MFCHECK(shortcut & 2));

	// Other
	const UINT8 DISPCLK = np2oscfg.DISPCLK;
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_DISPCLOCK, MFCHECK(DISPCLK & 1));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_DISPFRAME, MFCHECK(DISPCLK & 2));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_JOYX, MFCHECK(np2cfg.BTN_MODE));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_RAPID, MFCHECK(np2cfg.BTN_RAPID));
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_MSRAPID, MFCHECK(np2cfg.MOUSERAPID));
}

#if defined(SUPPORT_S98)
void xmenu_sets98logging(UINT8 value)
{
	CheckMenuItem(np2class_gethmenu(g_hWndMain), MF_BYCOMMAND | IDM_S98LOGGING, MFCHECK(value));
}
#endif

#if defined(SUPPORT_WAVEREC)
void xmenu_setwaverec(UINT8 value)
{
	CheckMenuItem(np2class_gethmenu(g_hWndMain), MF_BYCOMMAND | IDM_WAVEREC, MFCHECK(value));
}
#endif

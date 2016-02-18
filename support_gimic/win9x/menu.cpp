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
#include "misc\WndProc.h"
#include "pccore.h"
#if defined(SUPPORT_WAVEREC)
#include "sound\sound.h"
#endif
#if defined(SUPPORT_S98)
#include "sound\s98.h"
#endif

/**
 * ����
 * @param[in] hMenu ���j���[ �n���h��
 * @param[in] uID ID
 * @param[out] phmenuRet �����������j���[
 * @param[out] pnPos ���������ʒu
 * @retval true ��������
 * @retval false ������Ȃ�����
 */
bool menu_searchmenu(HMENU hMenu, UINT uID, HMENU *phmenuRet, int *pnPos)
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
				return true;
			}
			else if ((mii.hSubMenu) && (menu_searchmenu(mii.hSubMenu, uID, phmenuRet, pnPos)))
			{
				return true;
			}
		}
	}
	return false;
}

/**
 * ���j���[�ǉ�
 * @param[in] hMenu ���j���[ �n���h��
 * @param[in] nPos �ǉ�����ʒu
 * @param[in] hmenuAdd �ǉ����郁�j���[
 * @param[in] bSeparator �Z�p���[�^��ǉ�����
 * @return �ǉ���
 */
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
				InsertMenu(hMenu, nPos + nAdded, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
				nAdded++;
			}
			InsertMenuItem(hMenu, nPos + nAdded, TRUE, &mii);
			nAdded++;
		}
	}
	return nAdded;
}

/**
 * ���j���[�ǉ�
 * @param[in] hMenu ���j���[ �n���h��
 * @param[in] nPos �ǉ�����ʒu
 * @param[in] uID ���j���[ ID
 * @param[in] bSeparator �Z�p���[�^��ǉ�����
 * @return �ǉ���
 */
int menu_addmenures(HMENU hMenu, int nPos, UINT uID, BOOL bSeparator)
{
	int nCount = 0;
	HINSTANCE hInstance = CWndProc::FindResourceHandle(MAKEINTRESOURCE(uID), RT_MENU);
	HMENU hmenuAdd = LoadMenu(hInstance, MAKEINTRESOURCE(uID));
	if (hmenuAdd)
	{
		nCount = menu_addmenu(hMenu, nPos, hmenuAdd, bSeparator);
		DestroyMenu(hmenuAdd);
	}
	return nCount;
}

/**
 * ���j���[�ǉ�
 * @param[in] hMenu ���j���[ �n���h��
 * @param[in] uByID ���j���[�ʒu
 * @param[in] uID ���j���[ ID
 * @return �ǉ���
 */
static int menu_addmenubyid(HMENU hMenu, UINT uByID, UINT uID)
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

/**
 * ���j���[�ǉ�
 * @param[in] hMenu ���j���[�̃n���h��
 * @param[in] uPosition �V�������ڂ̒��O�Ɉʒu���鍀��
 * @param[in] uFlags �I�v�V����
 * @param[in] uIDNewItem ���ʎq�A���j���[�A�T�u���j���[�̂����ꂩ
 * @param[in] lpNewItem ���j���[ ������
 * @return �֐�����������ƁA0 �ȊO�̒l���Ԃ�܂�
 */
static BOOL InsertMenuString(HMENU hMenu, UINT uPosition, UINT uFlags, UINT_PTR uIDNewItem, UINT uStringID)
{
	std::tstring rString(LoadTString(uStringID));

	BOOL bResult = FALSE;
	if (!rString.empty())
	{
		bResult = InsertMenu(hMenu, uPosition, uFlags, uIDNewItem, rString.c_str());
	}
	return bResult;
}

/**
 * ���j���[�ǉ� (�P���R�s�[)
 * @param[in] popup �R�s�[��
 * @param[in] menubar �R�s�[��
 */
void menu_addmenubar(HMENU popup, HMENU menubar)
{
	(void)menu_addmenu(popup, 0, menubar, FALSE);
}



// ----

/**
 * �V�X�e�� ���j���[������
 * @param[in] hMenu ���j���[ �n���h��
 */
void sysmenu_initialize(HMENU hMenu)
{
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

/**
 * �V�X�e�� ���j���[�X�V
 * @param[in] hMenu ���j���[ �n���h��
 */
void sysmenu_update(HMENU hMenu)
{
	CheckMenuItem(hMenu, IDM_TOOLWIN, MF_BYCOMMAND | MFCHECK(np2oscfg.toolwin));
	CheckMenuItem(hMenu, IDM_KEYDISP, MF_BYCOMMAND | MFCHECK(np2oscfg.keydisp));
	CheckMenuItem(hMenu, IDM_SNAPENABLE, MF_BYCOMMAND | MFCHECK(np2oscfg.WINSNAP));

	const UINT8 background = np2oscfg.background ^ 3;
	EnableMenuItem(hMenu, IDM_BGSOUND, (background & 1) ? MF_ENABLED : MF_GRAYED);
	CheckMenuItem(hMenu, IDM_BACKGROUND, MF_BYCOMMAND | MFCHECK(background & 1));
	CheckMenuItem(hMenu, IDM_BGSOUND, MF_BYCOMMAND | MFCHECK(background & 2));

	const int scrnmul = scrnmng_getmultiple();
	CheckMenuItem(hMenu, IDM_SCRNMUL4, MF_BYCOMMAND | MFCHECK(scrnmul == 4));
	CheckMenuItem(hMenu, IDM_SCRNMUL6, MF_BYCOMMAND | MFCHECK(scrnmul == 6));
	CheckMenuItem(hMenu, IDM_SCRNMUL8, MF_BYCOMMAND | MFCHECK(scrnmul == 8));
	CheckMenuItem(hMenu, IDM_SCRNMUL10, MF_BYCOMMAND | MFCHECK(scrnmul == 10));
	CheckMenuItem(hMenu, IDM_SCRNMUL12, MF_BYCOMMAND | MFCHECK(scrnmul == 12));
	CheckMenuItem(hMenu, IDM_SCRNMUL16, MF_BYCOMMAND | MFCHECK(scrnmul == 16));
}



// ----

/**
 * ���j���[������
 * @param[in] ���j���[ �n���h��
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
		InsertMenuString(hMenu, nPos, MF_BYPOSITION | MF_POPUP, (UINT_PTR)hmenuSub, IDS_HDD);
	}

#if defined(SUPPORT_PX)
	(void)menu_addmenubyid(hMenu, IDM_SPARKBOARD, IDR_PXMENU);
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
	CheckMenuItem(hMenu, IDM_WINDOW, MF_BYCOMMAND | MFCHECK(!bFullScreen));
	CheckMenuItem(hMenu, IDM_FULLSCREEN, MF_BYCOMMAND | MFCHECK(bFullScreen));
	const UINT8 nRotateMode = g_scrnmode & SCRNMODE_ROTATEMASK;
	CheckMenuItem(hMenu, IDM_ROLNORMAL, MF_BYCOMMAND | MFCHECK(nRotateMode == 0));
	CheckMenuItem(hMenu, IDM_ROLLEFT, MF_BYCOMMAND | MFCHECK(nRotateMode == SCRNMODE_ROTATELEFT));
	CheckMenuItem(hMenu, IDM_ROLRIGHT, MF_BYCOMMAND | MFCHECK(nRotateMode == SCRNMODE_ROTATERIGHT));
	CheckMenuItem(hMenu, IDM_DISPSYNC, MF_BYCOMMAND | MFCHECK(np2cfg.DISPSYNC));
	CheckMenuItem(hMenu, IDM_RASTER, MF_BYCOMMAND | MFCHECK(np2cfg.RASTER));
	CheckMenuItem(hMenu, IDM_NOWAIT, MF_BYCOMMAND | MFCHECK(np2oscfg.NOWAIT));
	const UINT8 DRAW_SKIP = np2oscfg.DRAW_SKIP;
	CheckMenuItem(hMenu, IDM_AUTOFPS, MF_BYCOMMAND | MFCHECK(DRAW_SKIP == 0));
	CheckMenuItem(hMenu, IDM_60FPS, MF_BYCOMMAND | MFCHECK(DRAW_SKIP == 1));
	CheckMenuItem(hMenu, IDM_30FPS, MF_BYCOMMAND | MFCHECK(DRAW_SKIP == 2));
	CheckMenuItem(hMenu, IDM_20FPS, MF_BYCOMMAND | MFCHECK(DRAW_SKIP == 3));
	CheckMenuItem(hMenu, IDM_15FPS, MF_BYCOMMAND | MFCHECK(DRAW_SKIP == 4));

	// Device-Keyboard
	const UINT8 KEY_MODE = np2cfg.KEY_MODE;
	CheckMenuItem(hMenu, IDM_KEY, MF_BYCOMMAND | MFCHECK(KEY_MODE == 0));
	CheckMenuItem(hMenu, IDM_JOY1, MF_BYCOMMAND | MFCHECK(KEY_MODE == 1));
	CheckMenuItem(hMenu, IDM_JOY2, MF_BYCOMMAND | MFCHECK(KEY_MODE == 2));
	const UINT8 XSHIFT = np2cfg.XSHIFT;
	CheckMenuItem(hMenu, IDM_XSHIFT, MF_BYCOMMAND | MFCHECK(XSHIFT & 1));
	CheckMenuItem(hMenu, IDM_XCTRL, MF_BYCOMMAND | MFCHECK(XSHIFT & 2));
	CheckMenuItem(hMenu, IDM_XGRPH, MF_BYCOMMAND | MFCHECK(XSHIFT & 4));
	const UINT8 F12COPY = np2oscfg.F12COPY;
	CheckMenuItem(hMenu, IDM_F12MOUSE, MF_BYCOMMAND | MFCHECK(F12COPY == 0));
	CheckMenuItem(hMenu, IDM_F12COPY, MF_BYCOMMAND | MFCHECK(F12COPY == 1));
	CheckMenuItem(hMenu, IDM_F12STOP, MF_BYCOMMAND | MFCHECK(F12COPY == 2));
	CheckMenuItem(hMenu, IDM_F12EQU, MF_BYCOMMAND | MFCHECK(F12COPY == 3));
	CheckMenuItem(hMenu, IDM_F12COMMA, MF_BYCOMMAND | MFCHECK(F12COPY == 4));
	CheckMenuItem(hMenu, IDM_USERKEY1, MF_BYCOMMAND | MFCHECK(F12COPY == 5));
	CheckMenuItem(hMenu, IDM_USERKEY2, MF_BYCOMMAND | MFCHECK(F12COPY == 6));

	// Device-Sound
	const UINT8 BEEP_VOL = np2cfg.BEEP_VOL;
	CheckMenuItem(hMenu, IDM_BEEPOFF, MF_BYCOMMAND | MFCHECK(BEEP_VOL == 0));
	CheckMenuItem(hMenu, IDM_BEEPLOW, MF_BYCOMMAND | MFCHECK(BEEP_VOL == 1));
	CheckMenuItem(hMenu, IDM_BEEPMID, MF_BYCOMMAND | MFCHECK(BEEP_VOL == 2));
	CheckMenuItem(hMenu, IDM_BEEPHIGH, MF_BYCOMMAND | MFCHECK(BEEP_VOL == 3));
	const UINT8 SOUND_SW = np2cfg.SOUND_SW;
	CheckMenuItem(hMenu, IDM_NOSOUND, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x00));
	CheckMenuItem(hMenu, IDM_PC9801_14, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x01));
	CheckMenuItem(hMenu, IDM_PC9801_26K, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x02));
	CheckMenuItem(hMenu, IDM_PC9801_86, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x04));
	CheckMenuItem(hMenu, IDM_PC9801_26_86, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x06));
	CheckMenuItem(hMenu, IDM_PC9801_86_CB, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x14));
	CheckMenuItem(hMenu, IDM_PC9801_118, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x08));
	CheckMenuItem(hMenu, IDM_SPEAKBOARD, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x20));
	CheckMenuItem(hMenu, IDM_SPARKBOARD, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x40));
#if defined(SUPPORT_PX)
	CheckMenuItem(hMenu, IDM_PX1, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x30));
	CheckMenuItem(hMenu, IDM_PX2, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x50));
#endif	// defined(SUPPORT_PX)
	CheckMenuItem(hMenu, IDM_SOUNDORCHESTRA, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x32));
	CheckMenuItem(hMenu, IDM_SOUNDORCHESTRAV, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x82));
	CheckMenuItem(hMenu, IDM_AMD98, MF_BYCOMMAND | MFCHECK(SOUND_SW == 0x80));
	CheckMenuItem(hMenu, IDM_JASTSOUND, MF_BYCOMMAND | MFCHECK(np2oscfg.jastsnd));
	CheckMenuItem(hMenu, IDM_SEEKSND, MF_BYCOMMAND | MFCHECK(np2cfg.MOTOR));

	// Device-Memory
	const UINT8 EXTMEM = np2cfg.EXTMEM;
	CheckMenuItem(hMenu, IDM_MEM640, MF_BYCOMMAND | MFCHECK(EXTMEM == 0));
	CheckMenuItem(hMenu, IDM_MEM16, MF_BYCOMMAND | MFCHECK(EXTMEM == 1));
	CheckMenuItem(hMenu, IDM_MEM36, MF_BYCOMMAND | MFCHECK(EXTMEM == 3));
	CheckMenuItem(hMenu, IDM_MEM76, MF_BYCOMMAND | MFCHECK(EXTMEM == 7));
	CheckMenuItem(hMenu, IDM_MEM116, MF_BYCOMMAND | MFCHECK(EXTMEM == 11));
	CheckMenuItem(hMenu, IDM_MEM136, MF_BYCOMMAND | MFCHECK(EXTMEM == 13));

	// Device
	CheckMenuItem(hMenu, IDM_MOUSE, MF_BYCOMMAND | MFCHECK(np2oscfg.MOUSE_SW));

	// Other-ShortcutKey
	const UINT8 shortcut = np2oscfg.shortcut;
	CheckMenuItem(hMenu, IDM_ALTENTER, MF_BYCOMMAND | MFCHECK(shortcut & 1));
	CheckMenuItem(hMenu, IDM_ALTF4, MF_BYCOMMAND | MFCHECK(shortcut & 2));

	// Other
	bool bLogging = false;
#if defined(SUPPORT_S98)
	if (S98_isopened())
	{
		bLogging = true;
	}
#endif
#if defined(SUPPORT_WAVEREC)
	if (sound_isrecording())
	{
		bLogging = true;
	}
#endif
	CheckMenuItem(hMenu, MF_BYCOMMAND | IDM_S98LOGGING, MFCHECK(bLogging));
	const UINT8 DISPCLK = np2oscfg.DISPCLK;
	CheckMenuItem(hMenu, IDM_DISPCLOCK, MF_BYCOMMAND | MFCHECK(DISPCLK & 1));
	CheckMenuItem(hMenu, IDM_DISPFRAME, MF_BYCOMMAND | MFCHECK(DISPCLK & 2));
	CheckMenuItem(hMenu, IDM_JOYX, MF_BYCOMMAND | MFCHECK(np2cfg.BTN_MODE));
	CheckMenuItem(hMenu, IDM_RAPID, MF_BYCOMMAND | MFCHECK(np2cfg.BTN_RAPID));
	CheckMenuItem(hMenu, IDM_MSRAPID, MF_BYCOMMAND | MFCHECK(np2cfg.MOUSERAPID));
}

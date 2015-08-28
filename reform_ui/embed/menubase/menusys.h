/**
 * @file	menusys.h
 * @brief	Interface of the menu of the system
 */

#pragma once

#include <vector>
#include "menubase.h"

enum {
	MENUSYS_MAX			= 8
};

enum {
	SMSG_SETHIDE		= 0,
	SMSG_GETHIDE,
	SMSG_SETENABLE,
	SMSG_GETENABLE,
	SMSG_SETCHECK,
	SMSG_GETCHECK,
	SMSG_SETTEXT
};

enum {
	MENUS_POPUP			= 0x0010,
	MENUS_SYSTEM		= 0x0020,
	MENUS_MINIMIZE		= 0x0030,
	MENUS_CLOSE			= 0x0040,
	MENUS_CTRLMASK		= 0x0070
};

enum {
	MENUSTYLE_BOTTOM	= 0x0001
};

typedef struct _smi {
const OEMCHAR		*string;
const struct _smi	*child;
	MENUID			id;
	MENUFLG			flag;
} MSYSITEM;


#ifdef __cplusplus

struct MenuSysItem;
class MenuSysWnd;

/**
 * @brief システム メニュー
 */
class MenuSys : public IMenuBaseWnd
{
public:
	MenuSys();
	bool Initialize(const MSYSITEM *item, void (*cmd)(MENUID id), UINT16 icon, const OEMCHAR *title);
	void Deinitialize();
	bool Open(int x, int y);
	virtual void OnClose();
	virtual void OnMoving(int x, int y, int btn);
	virtual void OnKeyDown(UINT key);
	INTPTR Send(int ctrl, MENUID id, INTPTR arg);
	void SetStyle(UINT16 style);

private:
	MenuBase* m_pMenuBase;
	MenuSysWnd* m_root;				/*!< ルート ウィンドウ */
	std::vector<MenuSysWnd*> m_wnd;	/*!< ウィンドゥ */
	UINT16 m_icon;					/*!< アイコン */
	UINT16 m_style;					/*!< スタイル */
	void (*m_cmd)(MENUID id);		/*!< コマンド */
	int m_opened;
	int m_popupx;					/*!< X */
	int m_popupy;					/*!< Y */
	OEMCHAR m_title[128];			/*!< タイトル */

	bool OpenRootWnd();
	bool OpenChild(const MenuSysWnd* wnd, int pos);
	int OpenPopup();
	void CloseChild(const MenuSysWnd* wnd);
	MenuSysWnd* GetWnd(int x, int y) const;
	void FocusMove(MenuSysWnd* wnd, int dir);
	void FocusEnter(MenuSysWnd* wnd, bool exec);
	void SetFlag(MenuSysItem* item, MENUFLG flag, MENUFLG mask);
	void SetText(MenuSysItem* item, const OEMCHAR *arg);
	void RedrawItem(const MenuSysItem* item);
	static void Draw(VRAMHDL dst, const RECT_T *rect, void *arg);
};

extern "C"
{
#endif

BRESULT menusys_initialize(const MSYSITEM *item, void (*cmd)(MENUID id), UINT16 icon, const OEMCHAR *title);
void menusys_deinitialize(void);
BRESULT menusys_open(int x, int y);
INTPTR menusys_msg(int ctrl, MENUID id, INTPTR arg);
void menusys_setstyle(UINT16 style);

#ifdef __cplusplus
}
#endif


// ---- MACRO

#define menusys_sethide(id, hide)		\
				menusys_msg(SMSG_SETHIDE, (id), (INTPTR)(hide))
#define menusys_gethide(id)				\
				((int)menusys_msg(SMSG_GETHIDE, (id), 0))

#define menusys_setenable(id, enable)	\
				menusys_msg(SMSG_SETENABLE, (id), (long)(enable))
#define menusys_getenable(id)			\
				((int)menusys_msg(SMSG_GETENABLE, (id), 0))

#define menusys_setcheck(id, checked)	\
				menusys_msg(SMSG_SETCHECK, (id), (long)(checked))
#define menusys_getcheck(id)			\
				((int)menusys_msg(SMSG_GETCHECK, (id), 0))

#define menusys_settext(id, str)		\
				menusys_msg(SMSG_SETTEXT, (id), (INTPTR)(str))


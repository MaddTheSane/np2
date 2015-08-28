/**
 * @file	menubase.h
 * @brief	Interface of the base of the menu
 */

#pragma once

#include "../vramhdl.h"
#include "fontmng.h"

typedef unsigned short		MENUID;
typedef unsigned short		MENUFLG;

// #include	"menuvram.h"
// #include	"menuicon.h"
// #include	"menusys.h"
// #include	"menudlg.h"
// #include	"menumbox.h"
// #include	"menures.h"


enum {
	MENU_DISABLE		= 0x0001,
	MENU_GRAY			= 0x0002,
	MENU_CHECKED		= 0x0004,
	MENU_SEPARATOR		= 0x0008,
	MENU_REDRAW			= 0x1000,
	MENU_NOSEND			= 0x2000,
	MENU_TABSTOP		= 0x4000,
	MENU_DELETED		= 0x8000,
	MENU_STYLE			= 0x0ff0
};

enum {
	DID_STATIC			= 0,
	DID_OK,
	DID_CANCEL,
	DID_ABORT,
	DID_RETRY,
	DID_IGNORE,
	DID_YES,
	DID_NO,
	DID_APPLY,
	DID_USER
};

enum {
	SID_CAPTION			= 0x7ffd,
	SID_MINIMIZE		= 0x7ffe,
	SID_CLOSE			= 0x7fff
};



#ifdef __cplusplus

class MenuBase
{
public:
	static MenuBase* GetInstance();
	MenuBase();
	~MenuBase();
	void Initialize();
	void Deinitialize();
	bool Open(int num);
	void Close();
	bool OnMoving(int x, int y, int btn);
	bool OnKey(UINT key);
	void Invalidate(VRAMHDL vram, const RECT_T *rect);
	void Clear(VRAMHDL vram);
	void Draw(void (*draw)(VRAMHDL dst, const RECT_T *rect, void *arg), void *arg);

	static void DoModal();
	FONTMNGH GetFont();
	int Width() const;
	int Height() const;
	int Bpp() const;

private:
	static MenuBase sm_instance;
	FONTMNGH m_font;
	FONTMNGH m_font2;
	UNIRECT m_rect;
	int m_width;
	int m_height;
	UINT m_bpp;
	int m_num;
};
// typedef MenuBase MENUBASE;

inline MenuBase* MenuBase::GetInstance()
{
	return &sm_instance;
}

inline FONTMNGH MenuBase::GetFont()
{
	return m_font;
}

inline int MenuBase::Width() const
{
	return m_width;
}

inline int MenuBase::Height() const
{
	return m_height;
}

inline int MenuBase::Bpp() const
{
	return m_bpp;
}

extern "C"
{
#endif

extern	VRAMHDL		menuvram;

void menubase_initialize(void);
void menubase_deinitialize(void);
void menubase_close(void);
BRESULT menubase_moving(int x, int y, int btn);
BRESULT menubase_key(UINT key);
void menubase_modalproc(void);

#ifdef __cplusplus
}
#endif

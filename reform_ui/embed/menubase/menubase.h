/**
 * @file	menubase.h
 * @brief	Interface of the base of the menu
 */

#pragma once

#include "../vramhdl.h"
#include "fontmng.h"

typedef unsigned short		MENUID;
typedef unsigned short		MENUFLG;

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

/**
 * @brief MenuBase interface
 */
class IMenuBaseWnd
{
public:
	virtual void OnClose() = 0;
	virtual void OnMoving(int x, int y, int btn) = 0;
	virtual void OnKeyDown(UINT key) = 0;
};

/**
 * @brief MenuBase
 */
class MenuBase
{
public:
	static MenuBase* GetInstance();
	MenuBase();
	~MenuBase();
	void Initialize();
	void Deinitialize();
	bool Open(IMenuBaseWnd* pWnd);
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
	VRAMHDL Vram();

private:
	static MenuBase sm_instance;
	FONTMNGH m_font;
	FONTMNGH m_font2;
	UNIRECT m_rect;
	int m_width;
	int m_height;
	UINT m_bpp;
	VRAMHDL m_menuvram;
	IMenuBaseWnd* m_pWnd;
};

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

inline VRAMHDL MenuBase::Vram()
{
	return m_menuvram;
}

extern "C"
{
#endif

void menubase_initialize(void);
void menubase_deinitialize(void);
void menubase_close(void);
BRESULT menubase_moving(int x, int y, int btn);
BRESULT menubase_key(UINT key);
void menubase_modalproc(void);
BOOL menubase_isopened(void);
VRAMHDL menubase_vram(void);

#ifdef __cplusplus
}
#endif

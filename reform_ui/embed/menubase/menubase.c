/**
 * @file	menubase.c
 * @brief	Implementation of the base of the menu
 */

#include "compiler.h"
#include "menubase.h"
#include "menudeco.inc"
#include "menudlg.h"
#include "menuicon.h"
#include "menusys.h"
#include "fontmng.h"
#include "inputmng.h"
#include "scrnmng.h"
#include "taskmng.h"

	VRAMHDL		menuvram;

MenuBase MenuBase::sm_instance;

MenuBase::MenuBase()
	: m_font(NULL)
	, m_font2(NULL)
	, m_width(0)
	, m_height(0)
	, m_bpp(0)
	, m_pWnd(NULL)
{
}

MenuBase::~MenuBase()
{
}

void MenuBase::Initialize()
{
	m_font = fontmng_create(MENU_FONTSIZE, FDAT_PROPORTIONAL, NULL);
	m_font2 = fontmng_create(MENU_FONTSIZE, 0, NULL);
	MenuIcon::GetInstance()->Initialize();
}

void MenuBase::Deinitialize()
{
	MenuIcon::GetInstance()->Deinitialize();
	fontmng_destroy(m_font2);
	fontmng_destroy(m_font);

	m_font = NULL;
	m_font2 = NULL;
	m_pWnd = NULL;
}

bool MenuBase::Open(IMenuBaseWnd* pWnd)
{
	Close();

	SCRNMENU smenu;
	if (scrnmng_entermenu(&smenu) != SUCCESS)
	{
		return false;
	}
	m_width = smenu.width;
	m_height = smenu.height;
	m_bpp = smenu.bpp;
	VRAMHDL hdl = vram_create(m_width, m_height, TRUE, m_bpp);
	menuvram = hdl;
	if (hdl == NULL)
	{
		return false;
	}
	unionrect_rst(&m_rect);
	m_pWnd = pWnd;
	return true;
}

void MenuBase::Close()
{
	IMenuBaseWnd* pWnd = m_pWnd;
	if (pWnd)
	{
		m_pWnd = NULL;
		pWnd->OnClose();
		VRAMHDL hdl = menuvram;
		if (hdl)
		{
			Draw(NULL, NULL);
			menuvram = NULL;
			vram_destroy(hdl);
		}
		scrnmng_leavemenu();
		m_width = 0;
		m_height = 0;
		m_bpp = 0;
	}
}

bool MenuBase::OnMoving(int x, int y, int btn)
{
	if (m_pWnd)
	{
		m_pWnd->OnMoving(x, y, btn);
	}
	return true;
}

bool MenuBase::OnKey(UINT key)
{
	if (m_pWnd)
	{
		m_pWnd->OnKeyDown(key);
	}
	return true;
}

void MenuBase::Invalidate(VRAMHDL vram, const RECT_T *rect)
{
	if (vram)
	{
		RECT_T rct;
		if (rect == NULL)
		{
			vram_getrect(vram, &rct);
		}
		else
		{
			rct.left = vram->posx + rect->left;
			rct.top = vram->posy + rect->top;
			rct.right = vram->posx + rect->right;
			rct.bottom = vram->posy + rect->bottom;
		}
		unionrect_add(&m_rect, &rct);
	}
}

void MenuBase::Clear(VRAMHDL vram)
{
	if (vram)
	{
		RECT_T rct;
		vram_getrect(vram, &rct);
		vram_fillalpha(menuvram, &rct, 1);
		Invalidate(vram, NULL);
	}
}

void MenuBase::Draw(void (*draw)(VRAMHDL dst, const RECT_T *rect, void *arg), void *arg)
{
	if (m_rect.type)
	{
		const RECT_T* rect = unionrect_get(&m_rect);
		if (draw)
		{
			(*draw)(menuvram, rect, arg);
		}
		scrnmng_menudraw(rect);
		unionrect_rst(&m_rect);
	}
}

void MenuBase::DoModal()
{
	while ((taskmng_sleep(5)) && (menuvram != NULL))
	{
	}
}


// ----

void menubase_initialize(void)
{
	MenuBase::GetInstance()->Initialize();
}

void menubase_deinitialize(void)
{
	MenuBase::GetInstance()->Deinitialize();
}

void menubase_close(void)
{
	MenuBase::GetInstance()->Close();
}

BRESULT menubase_moving(int x, int y, int btn)
{
	return MenuBase::GetInstance()->OnMoving(x, y, btn) ? SUCCESS : FAILURE;
}

BRESULT menubase_key(UINT key)
{
	return MenuBase::GetInstance()->OnKey(key) ? SUCCESS : FAILURE;
}

void menubase_modalproc(void)
{
	MenuBase::GetInstance()->DoModal();
}

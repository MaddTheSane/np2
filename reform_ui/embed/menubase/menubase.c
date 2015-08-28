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
	, m_num(0)
{
}

MenuBase::~MenuBase()
{
}

bool MenuBase::Create()
{
	m_font = fontmng_create(MENU_FONTSIZE, FDAT_PROPORTIONAL, NULL);
	m_font2 = fontmng_create(MENU_FONTSIZE, 0, NULL);
	menuicon_initialize();
	return true;
}

void MenuBase::Destroy()
{
	menuicon_deinitialize();
	fontmng_destroy(m_font2);
	fontmng_destroy(m_font);

	m_font = NULL;
	m_font2 = NULL;
	m_num = 0;
}

bool MenuBase::Open(int num)
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
	m_num = num;
	return true;
}

void MenuBase::Close()
{
	int num = m_num;
	if (m_num)
	{
		m_num = 0;
		if (num == 1)
		{
			menusys_close();
		}
		else
		{
			menudlg_destroy();
		}
		VRAMHDL hdl = menuvram;
		if (hdl)
		{
			menubase_draw(NULL, NULL);
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
	if (m_num == 1)
	{
		menusys_moving(x, y, btn);
	}
	else if (m_num)
	{
		menudlg_moving(x, y, btn);
	}
	return true;
}

bool MenuBase::OnKey(UINT key)
{
	if (m_num == 1)
	{
		menusys_key(key);
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


// ----

void menubase_proc(void) {
}

void MenuBase::DoModal()
{
	while ((taskmng_sleep(5)) && (menuvram != NULL))
	{
	}
}


// ----

BRESULT menubase_create(void)
{
	return MenuBase::GetInstance()->Create() ? SUCCESS : FAILURE;
}

void menubase_destroy(void)
{
	MenuBase::GetInstance()->Destroy();
}

BRESULT menubase_open(int num)
{
	return MenuBase::GetInstance()->Open(num) ? SUCCESS : FAILURE;
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

void menubase_setrect(VRAMHDL vram, const RECT_T *rect)
{
	MenuBase::GetInstance()->Invalidate(vram, rect);
}

void menubase_clrrect(VRAMHDL vram)
{
	MenuBase::GetInstance()->Clear(vram);
}

void menubase_draw(void (*draw)(VRAMHDL dst, const RECT_T *rect, void *arg), void *arg)
{
	MenuBase::GetInstance()->Draw(draw, arg);
}

void menubase_modalproc(void)
{
	MenuBase::GetInstance()->DoModal();
}

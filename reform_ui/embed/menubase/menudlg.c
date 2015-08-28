/**
 * @file	menudlg.c
 * @brief	Implementation of the base of the dialog
 */

#include "compiler.h"
#include "menudlg.h"
#include "menudeco.inc"
#include "menuicon.h"
#include "menures.h"
#include "../vrammix.h"
#include "strres.h"
#include "fontmng.h"
#include <vector>

struct DLGLIST
{
	SINT16		fontsize;
	SINT16		scrollbar;
	int			dispmax;
	SINT16		basepos;
};

struct DLGSLD
{
	SINT16		minval;
	SINT16		maxval;
	int			pos;
	UINT8		type;
	UINT8		moving;
	UINT8		sldh;
	UINT8		sldv;
};

class MenuDialog;

/**
 * @brief The item class
 */
class MenuDlgItem
{
public:
	static MenuDlgItem* CreateInstance(int type, MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect, const void *arg);

	MenuDlgItem(MenuDialog* pParent, int type, MENUID id, MENUFLG flg, const RECT_T& rect);
	virtual ~MenuDlgItem();

	virtual BRESULT OnCreate(const void *arg);
	virtual void OnPaint() = 0;
	virtual void OnSetValue(int val);
	virtual void OnSetText(const OEMCHAR* lpString);
	virtual void OnSetFont(FONTMNGH font);
	virtual void OnClick(int x, int y);
	virtual void OnMove(int x, int y, int focus);
	virtual void OnRelease(int focus);
	virtual INTPTR ItemProc(int ctrl, INTPTR arg);

protected:
	MenuDialog* m_pParent;		//!< The instance of parent
	int			m_type;			// *
	MENUID		m_id;
	MENUFLG		m_flag;
	MENUID		m_page;			// *
	MENUID		m_group;		// *
	RECT_T		m_rect;
	int			m_nValue;
	VRAMHDL		m_vram;			// *
	const OEMCHAR* m_lpString;	/*!< Text */
	FONTMNGH m_font;			/*!< Font */

	int Send(int msg, long param = 0);
	MenuBase* GetMenuBase();
	VRAMHDL GetVram();
	FONTMNGH GetFont();
	void Invalidate();
	void PaintIcon(VRAMHDL src);
	static const OEMCHAR* StrDup(const OEMCHAR* lpString);

	friend MenuDialog;
};

/**
 * @brief Dialog Class
 */
class MenuDialog
{
public:
	MenuDialog();
	bool Create(int width, int height, const OEMCHAR *str, int (*proc)(int msg, MENUID id, long param));
	void Destroy();
	bool Append(const MENUPRM *res, int count);
	bool Append(int type, MENUID id, MENUFLG flg, const void *arg, int posx, int posy, int width, int height);
	void Moving(int x, int y, int btn);
	INTPTR Send(int ctrl, MENUID id, INTPTR arg);
	void SetPage(MENUID page);
	void DispPageHidden(MENUID page, bool hidden);

	void DrawItem(MenuDlgItem* item = NULL);
	void Draw();

public:
	MenuBase* m_pMenuBase;
	VRAMHDL		m_vram;
	std::vector<MenuDlgItem*> m_items;
	int			m_nLocked;
	bool		m_bClosing;
	int			m_sx;
	int			m_sy;
	FONTMNGH	m_font;
	MENUID		m_page;
	MENUID		m_group;
	int			(*m_proc)(int msg, MENUID id, long param);

	int			m_dragflg;
	int			m_btn;
	int			m_lastx;
	int			m_lasty;
	MENUID		m_lastid;

private:
	void DrawLock(bool lock);
	MenuDlgItem* GetItem(MENUID id) const;
	MenuDlgItem* GetItemFromPosition(int x, int y) const;
};

static MenuDialog s_menudlg;


/**
 * 文字列コピー
 * @param[in] lpString 文字列
 * @return 文字列
 */
const OEMCHAR* MenuDlgItem::StrDup(const OEMCHAR* lpString)
{
	OEMCHAR* ret = NULL;
	if ((lpString) && (lpString[0] != '\0'))
	{
		const int nLength = static_cast<int>(OEMSTRLEN(lpString));
		ret = new OEMCHAR[nLength + 1];
		memcpy(ret, lpString, (nLength + 1) * sizeof(OEMCHAR));
	}
	return ret;
}

/**
 * コンストラクタ
 */
MenuDlgItem::MenuDlgItem(MenuDialog* pParent, int type, MENUID id, MENUFLG flg, const RECT_T& rect)
	: m_pParent(pParent)
	, m_type(type)
	, m_id(id)
	, m_flag(flg)
	, m_rect(rect)
	, m_nValue(0)
	, m_vram(NULL)
	, m_lpString(NULL)
	, m_font(NULL)
{
	m_page = pParent->m_page;
	m_group = pParent->m_group;
}

/**
 * デストラクタ
 */
MenuDlgItem::~MenuDlgItem()
{
	delete[] m_lpString;
	vram_destroy(m_vram);
}

BRESULT MenuDlgItem::OnCreate(const void *arg)
{
	return SUCCESS;
}

void MenuDlgItem::OnSetValue(int val)
{
//	m_nValue = val;
}

void MenuDlgItem::OnSetText(const OEMCHAR* lpString)
{
	delete[] m_lpString;
	m_lpString = StrDup(lpString);
}

void MenuDlgItem::OnSetFont(FONTMNGH font)
{
	m_font = font;
}

void MenuDlgItem::OnClick(int x, int y)
{
}

void MenuDlgItem::OnMove(int x, int y, int focus)
{
}

void MenuDlgItem::OnRelease(int focus)
{
}

int MenuDlgItem::Send(int msg, long param)
{
	return (*m_pParent->m_proc)(msg, m_id, param);
}

/**
 * メニュー ベースを得る
 */
MenuBase* MenuDlgItem::GetMenuBase()
{
	return m_pParent->m_pMenuBase;
}

/**
 * 描画 VRAMを得る
 */
inline VRAMHDL MenuDlgItem::GetVram()
{
	return m_pParent->m_vram;
}

/**
 * フォントを得る
 */
FONTMNGH MenuDlgItem::GetFont()
{
	FONTMNGH font = m_font;
	if (font == NULL)
	{
		font = m_pParent->m_font;
	}
	return font;
}

/**
 * 再描画指令
 */
inline void MenuDlgItem::Invalidate()
{
	m_pParent->DrawItem(this);
}

/**
 * アイコン描画
 */
void MenuDlgItem::PaintIcon(VRAMHDL src)
{
	VRAMHDL vram = GetVram();

	RECT_U r;
	r.p.x = m_rect.left;
	r.p.y = m_rect.top;
	UINT32 bgcol = menucolor[MVC_STATIC];
	if (src)
	{
		if (src->alpha)
		{
			r.r.right = r.r.left + src->width;
			r.r.bottom = r.r.top + src->height;
			vram_filldat(vram, &r.r, bgcol);
			vramcpy_cpyex(vram, &r.p, src, NULL);
		}
		else
		{
			vramcpy_cpy(vram, &r.p, src, NULL);
		}
	}
	else
	{
		vram_filldat(vram, &m_rect, bgcol);
	}
}

static void getmid(POINT_T *pt, const RECT_T *rect, const POINT_T *sz) {

	pt->x = rect->left;
	pt->x += (rect->right - rect->left - sz->x) >> 1;
	pt->y = rect->top;
	pt->y += (rect->bottom - rect->top - sz->y) >> 1;
}



// ---- base

class MenuDlgItemBase : public MenuDlgItem
{
public:
	MenuDlgItemBase(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect);
	virtual BRESULT OnCreate(const void *arg);
	virtual void OnPaint();
	virtual void OnClick(int x, int y);
	virtual void OnMove(int x, int y, int focus);
	virtual void OnRelease(int focus);
};

MenuDlgItemBase::MenuDlgItemBase(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
	: MenuDlgItem(pParent, DLGTYPE_BASE, id, flg, rect)
{
}

BRESULT MenuDlgItemBase::OnCreate(const void *arg)
{
	int width = m_rect.right - m_rect.left - ((MENU_FBORDER + MENU_BORDER) * 2);
	m_vram = vram_create(width, MENUDLG_CYCAPTION, FALSE, GetVram()->bpp);
	if (m_vram == NULL)
	{
		return FAILURE;
	}
	m_vram->posx = (MENU_FBORDER + MENU_BORDER);
	m_vram->posy = (MENU_FBORDER + MENU_BORDER);

	RECT_T rct;
	rct.left = 0;
	rct.top = 0;
	rct.right = width;
	rct.bottom = MENUDLG_CYCAPTION;
	menuvram_caption(m_vram, &rct, MICON_NULL, (OEMCHAR *)arg);
	return SUCCESS;
}

void MenuDlgItemBase::OnPaint()
{
	VRAMHDL vram = GetVram();
	menuvram_base(vram);
	vrammix_cpy(vram, NULL, m_vram, NULL);
	GetMenuBase()->Invalidate(vram, NULL);
}

void MenuDlgItemBase::OnClick(int x, int y)
{
	RECT_T rct;
	vram_getrect(m_vram, &rct);
	m_pParent->m_dragflg = rect_in(&rct, x, y);
	m_pParent->m_lastx = x;
	m_pParent->m_lasty = y;
}

void MenuDlgItemBase::OnMove(int x, int y, int focus)
{
	if (m_pParent->m_dragflg)
	{
		x -= m_pParent->m_lastx;
		y -= m_pParent->m_lasty;
		if ((x) || (y))
		{
			MenuBase* pMenuBase = GetMenuBase();
			VRAMHDL vram = GetVram();
			pMenuBase->Clear(vram);
			vram->posx += x;
			vram->posy += y;
			pMenuBase->Invalidate(m_vram, NULL);
		}
	}
}

void MenuDlgItemBase::OnRelease(int focus)
{
}



// ---- close

class MenuDlgItemClose : public MenuDlgItem
{
public:
	MenuDlgItemClose(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
		: MenuDlgItem(pParent, DLGTYPE_CLOSE, id, flg, rect)
	{
	}

	virtual void OnPaint()
	{
		VRAMHDL vram = GetVram();
		menuvram_closebtn(vram, &m_rect, m_nValue);
	}

	virtual void OnClick(int x, int y)
	{
		m_nValue = 1;
		Invalidate();
	}

	virtual void OnMove(int x, int y, int focus)
	{
		if (m_nValue != focus)
		{
			m_nValue = focus;
			Invalidate();
		}
	}

	virtual void OnRelease(int focus)
	{
		if (focus)
		{
			Send(DLGMSG_CLOSE);
		}
	}
};



/**
 * @brief テキスト
 */
class MenuDlgItemText : public MenuDlgItem
{
public:
	MenuDlgItemText(MenuDialog* pParent, int type, MENUID id, MENUFLG flg, const RECT_T& rect);
	virtual ~MenuDlgItemText();
	virtual BRESULT OnCreate(const void *arg);
	virtual void OnPaint();
	virtual void OnSetText(const OEMCHAR* lpString);
	virtual INTPTR ItemProc(int ctrl, INTPTR arg);

protected:
	POINT_T m_size;
	VRAMHDL m_icon;

	POINT_T GetTextPos(const POINT_T& size, const RECT_T& rect) const;
	POINT_T GetTextSize() const;
	void DrawText(const POINT_T& pt);
};

MenuDlgItemText::MenuDlgItemText(MenuDialog* pParent, int type, MENUID id, MENUFLG flg, const RECT_T& rect)
	: MenuDlgItem(pParent, type, id, flg, rect)
	, m_icon(NULL)
{
	m_size.x = 0;
	m_size.y = 0;
}

MenuDlgItemText::~MenuDlgItemText()
{
	menuicon_unlock(m_icon);
}

BRESULT MenuDlgItemText::OnCreate(const void *arg)
{
	OnSetText(static_cast<const OEMCHAR*>(arg));
	return SUCCESS;
}

void MenuDlgItemText::OnPaint()
{
	VRAMHDL vram = GetVram();
	vram_filldat(vram, &m_rect, menucolor[MVC_STATIC]);

	POINT_T sz = GetTextSize();
	POINT_T pt = GetTextPos(sz, m_rect);
	DrawText(pt);
}

POINT_T MenuDlgItemText::GetTextPos(const POINT_T& size, const RECT_T& rect) const
{
	POINT_T pt;
	switch (m_flag & MST_POSMASK)
	{
		case MST_LEFT:
		default:
			pt.x = rect.left;
			break;

		case MST_CENTER:
			pt.x = rect.left;
			pt.x += (rect.right - rect.left - size.x) >> 1;
			break;

		case MST_RIGHT:
			pt.x = rect.right - size.x - MENU_DSTEXT;
			break;
	}
	pt.y = rect.top;
	return pt;
}

POINT_T MenuDlgItemText::GetTextSize() const
{
	POINT_T size = m_size;
	if (m_icon)
	{
		if (size.x)
		{
#if defined(SIZE_QVGA)
			size.x += 1;
#else
			size.x += 2;
#endif
		}
		size.x += size.y;
	}
	return size;
}

void MenuDlgItemText::OnSetText(const OEMCHAR* lpString)
{
	__super::OnSetText(lpString);
	if (fontmng_getsize(GetFont(), lpString, &m_size) != SUCCESS)
	{
		m_size.y = fontmng_getfontsize(GetFont());
	}
	Invalidate();
}

INTPTR MenuDlgItemText::ItemProc(int ctrl, INTPTR arg)
{
	INTPTR ret = 0;
	switch (ctrl)
	{
		case DMSG_SETICON:
			menuicon_unlock(m_icon);
			m_icon = menuicon_lock(static_cast<UINT16>(arg), m_size.y, m_size.y, GetVram()->bpp);
			Invalidate();
			break;

		default:
			ret = __super::ItemProc(ctrl, arg);
			break;
	}
	return ret;
}

void MenuDlgItemText::DrawText(const POINT_T& pt)
{
	VRAMHDL vram = GetVram();
	POINT_T fp = pt;
	if (m_icon)
	{
		if (m_icon->alpha)
		{
			vramcpy_cpyex(vram, &fp, m_icon, NULL);
		}
		else {
			vramcpy_cpy(vram, &fp, m_icon, NULL);
		}
		fp.x += m_icon->width;
#if defined(SIZE_QVGA)
		fp.x += 1;
#else
		fp.x += 2;
#endif
	}

	if (m_lpString)
	{
		FONTMNGH font = GetFont();
		int color;
		if (!(m_flag & MENU_GRAY))
		{
			color = MVC_TEXT;
		}
		else
		{
			POINT_T p;
			p.x = fp.x + MENU_DSTEXT;
			p.y = fp.y + MENU_DSTEXT;
			vrammix_text(vram, font, m_lpString, menucolor[MVC_GRAYTEXT2], &p, &m_rect);
			color = MVC_GRAYTEXT1;
		}
		vrammix_text(vram, font, m_lpString, menucolor[color], &fp, &m_rect);
	}
}



/**
 * @brief ボタン
 */
class MenuDlgItemButton : public MenuDlgItemText
{
public:
	MenuDlgItemButton(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
		: MenuDlgItemText(pParent, DLGTYPE_BUTTON, id, flg, rect)
	{
	}

	virtual void OnPaint()
	{
		VRAMHDL vram = GetVram();
		vram_filldat(vram, &m_rect, menucolor[MVC_BTNFACE]);

		UINT c;
		if (!m_nValue)
		{
			c = MVC4(MVC_HILIGHT, MVC_DARK, MVC_LIGHT, MVC_SHADOW);
		}
		else {
			c = MVC4(MVC_DARK, MVC_DARK, MVC_SHADOW, MVC_SHADOW);
		}
		menuvram_box2(vram, &m_rect, c);

		POINT_T sz = GetTextSize();
		{
			POINT_T pt;
			getmid(&pt, &m_rect, &sz);
			if (m_nValue)
			{
				pt.x += MENU_DSTEXT;
				pt.y += MENU_DSTEXT;
			}
			DrawText(pt);
		}
	}

	virtual void OnClick(int x, int y)
	{
		m_nValue = 1;
		Invalidate();
	}

	virtual void OnMove(int x, int y, int focus)
	{
		if (m_nValue != focus)
		{
			m_nValue = focus;
			Invalidate();
		}
	}

	virtual void OnRelease(int focus)
	{
		if (m_nValue != 0)
		{
			m_nValue = 0;
			Invalidate();
		}
		if (focus)
		{
			Send(DLGMSG_COMMAND);
		}
	}
};



/**
 * list
 */
class MenuDlgItemList : public MenuDlgItem
{
public:
	MenuDlgItemList(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect);
	virtual ~MenuDlgItemList();
	virtual BRESULT OnCreate(const void *arg);
	virtual void OnPaint();
	virtual void OnSetValue(int val);
	virtual void OnSetFont(FONTMNGH font);
	virtual void OnClick(int x, int y);
	virtual void OnMove(int x, int y, int focus);
	virtual void OnRelease(int focus);
//	virtual INTPTR ItemProc(int ctrl, INTPTR arg);

private:
	struct ListItem
	{
		const OEMCHAR* lpString;
		int width;
		VRAMHDL icon;
	};

	int GetPos(int y) const;
	int GetPc(int x, int y) const;
	void DrawItem(const ListItem& item, int focus, POINT_T *pt, RECT_T *rct);
	BOOL DrawSub(int pos, int focus);
	void SetBtn(int flg);
	void DrawAll();
	int BarPos() const;
	void DrawBar();

public:
	void Reset();
	int SetBasePos(int pos);
	BOOL Append(const OEMCHAR* arg);
	BOOL SetEx(const ITEMEXPRM *arg);

private:
	DLGLIST m_dl;
	std::vector<ListItem> m_items;
};

MenuDlgItemList::MenuDlgItemList(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
	: MenuDlgItem(pParent, DLGTYPE_LIST, id, flg, rect)
{
	memset(&m_dl, 0, sizeof(m_dl));
}

MenuDlgItemList::~MenuDlgItemList()
{
	for (std::vector<ListItem>::iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		delete[] it->lpString;
		menuicon_unlock(it->icon);
	}
}

void MenuDlgItemList::Reset()
{
	vram_filldat(m_vram, NULL, 0xffffff);

	for (std::vector<ListItem>::iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		delete[] it->lpString;
		menuicon_unlock(it->icon);
	}
	m_items.clear();

	m_nValue = -1;
	m_dl.scrollbar = 0;
	m_dl.basepos = 0;
}

BRESULT MenuDlgItemList::OnCreate(const void *arg)
{
	int width = m_rect.right - m_rect.left - (MENU_LINE * 4);
	int height = m_rect.bottom - m_rect.top - (MENU_LINE * 4);
	m_vram = vram_create(width, height, FALSE, GetVram()->bpp);
	if (m_vram == NULL)
	{
		return FAILURE;
	}
	m_vram->posx = m_rect.left + (MENU_LINE * 2);
	m_vram->posy = m_rect.top + (MENU_LINE * 2);
	OnSetFont(NULL);
	Reset();
	return SUCCESS;
}

void MenuDlgItemList::OnPaint()
{
	VRAMHDL vram = GetVram();
	menuvram_box2(vram, &m_rect, MVC4(MVC_SHADOW, MVC_HILIGHT, MVC_DARK, MVC_LIGHT));
	vrammix_cpy(vram, NULL, m_vram, NULL);
}

void MenuDlgItemList::DrawItem(const ListItem& item, int focus, POINT_T *pt, RECT_T *rct)
{
	vram_filldat(m_vram, rct, menucolor[(focus) ? MVC_CURBACK : MVC_HILIGHT]);

	POINT_T fp;
	fp.x = pt->x;
	fp.y = pt->y;
	if (item.icon)
	{
		if (item.icon->alpha)
		{
			vramcpy_cpyex(m_vram, &fp, item.icon, NULL);
		}
		else {
			vramcpy_cpy(m_vram, &fp, item.icon, NULL);
		}
		fp.x += item.icon->width;
#if defined(SIZE_QVGA)
		fp.x += 1;
#else
		fp.x += 2;
#endif
	}
	vrammix_text(m_vram, GetFont(), item.lpString, menucolor[(focus) ? MVC_CURTEXT : MVC_TEXT], &fp, rct);
}

BOOL MenuDlgItemList::DrawSub(int pos, int focus)
{
	if ((pos < 0) || (pos >= static_cast<int>(m_items.size())))
	{
		return FALSE;
	}

	pos -= m_dl.basepos;
	if (pos < 0)
	{
		return FALSE;
	}
	POINT_T pt;
	pt.x = 0;
	pt.y = pos * m_dl.fontsize;
	if (pt.y >= m_vram->height)
	{
		return FALSE;
	}
	RECT_T rct;
	rct.left = 0;
	rct.top = pt.y;
	rct.right = m_vram->width;
	if (static_cast<int>(m_items.size()) > m_dl.dispmax)
	{
		rct.right -= MENUDLG_CXVSCR;
	}
	rct.bottom = rct.top + m_dl.fontsize;
	DrawItem(m_items[pos], focus, &pt, &rct);
	return TRUE;
}

void MenuDlgItemList::SetBtn(int flg)
{
	const MENURES2* res = menures_scrbtn;

	RECT_T rct;
	rct.right = m_vram->width;
	rct.left = rct.right - MENUDLG_CXVSCR;
	if (!(flg & 2))
	{
		rct.top = 0;
	}
	else
	{
		rct.top = m_vram->height - MENUDLG_CYVSCR;
		if (rct.top < MENUDLG_CYVSCR)
		{
			rct.top = MENUDLG_CYVSCR;
		}
		res++;
	}
	rct.bottom = rct.top + MENUDLG_CYVSCR;
	vram_filldat(m_vram, &rct, menucolor[MVC_BTNFACE]);

	UINT mvc4;
	if (flg & 1)
	{
		mvc4 = MVC4(MVC_SHADOW, MVC_SHADOW, MVC_LIGHT, MVC_LIGHT);
	}
	else
	{
		mvc4 = MVC4(MVC_LIGHT, MVC_DARK, MVC_HILIGHT, MVC_SHADOW);
	}
	menuvram_box2(m_vram, &rct, mvc4);
	POINT_T pt;
	pt.x = rct.left + (MENU_LINE * 2);
	pt.y = rct.top + (MENU_LINE * 2);
	if (flg & 1)
	{
		pt.x += MENU_DSTEXT;
		pt.y += MENU_DSTEXT;
	}
	menuvram_res3put(m_vram, res, &pt, MVC_TEXT);
}

void MenuDlgItemList::DrawAll()
{
	RECT_T rct;
	rct.left = 0;
	rct.top = 0 - (m_dl.basepos * m_dl.fontsize);
	rct.right = m_vram->width;
	if (static_cast<int>(m_items.size()) > m_dl.dispmax)
	{
		rct.right -= MENUDLG_CXVSCR;
	}

	for (std::vector<ListItem>::iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		if (rct.top >= m_vram->height)
		{
			break;
		}
		if (rct.top >= 0)
		{
			rct.bottom = rct.top + m_dl.fontsize;
			POINT_T pt;
			pt.x = 0;
			pt.y = rct.top;
			DrawItem(*it, (std::distance(m_items.begin(), it) == m_nValue), &pt, &rct);
		}
		rct.top += m_dl.fontsize;
	}
	rct.bottom = m_vram->height;
	vram_filldat(m_vram, &rct, menucolor[MVC_HILIGHT]);
}

int MenuDlgItemList::BarPos() const
{
	int ret = m_vram->height - (MENUDLG_CYVSCR * 2);
	ret -= m_dl.scrollbar;
	ret *= m_dl.basepos;
	ret /= (static_cast<int>(m_items.size()) - m_dl.dispmax);
	return ret;
}

void MenuDlgItemList::DrawBar()
{
	RECT_T rct;
	rct.right = m_vram->width;
	rct.left = rct.right - MENUDLG_CXVSCR;
	rct.top = MENUDLG_CYVSCR;
	rct.bottom = m_vram->height - MENUDLG_CYVSCR;
	vram_filldat(m_vram, &rct, menucolor[MVC_SCROLLBAR]);

	rct.top += BarPos();
	rct.bottom = rct.top + m_dl.scrollbar;
	vram_filldat(m_vram, &rct, menucolor[MVC_BTNFACE]);
	menuvram_box2(m_vram, &rct, MVC4(MVC_LIGHT, MVC_DARK, MVC_HILIGHT, MVC_SHADOW));
}

BOOL MenuDlgItemList::Append(const OEMCHAR* lpString)
{
	ListItem item;
	memset(&item, 0, sizeof(item));
	item.lpString = StrDup(lpString);
	m_items.push_back(item);

	BOOL r = DrawSub(static_cast<int>(m_items.size()) - 1, FALSE);
	if (static_cast<int>(m_items.size()) > m_dl.dispmax)
	{
		int barsize = m_vram->height - (MENUDLG_CYVSCR * 2);
		if (barsize >= 8)
		{
			barsize *= m_dl.dispmax;
			barsize /= static_cast<int>(m_items.size());
			barsize = max(barsize, 6);
			if (!m_dl.scrollbar)
			{
				DrawAll();
				SetBtn(0);
				SetBtn(2);
			}
			m_dl.scrollbar = barsize;
			DrawBar();
		}
	}
	return r;
}

BOOL MenuDlgItemList::SetEx(const ITEMEXPRM *arg)
{
	if (arg == NULL)
	{
		return FALSE;
	}

	const int nIndex = arg->pos;
	if ((nIndex < 0) || (nIndex >= static_cast<int>(m_items.size())))
	{
		return FALSE;
	}

	ListItem& item = m_items[nIndex];

	delete[] item.lpString;
	item.lpString = StrDup(arg->str);

	menuicon_unlock(item.icon);
	item.icon = menuicon_lock(arg->icon, m_dl.fontsize, m_dl.fontsize, GetVram()->bpp);

	return DrawSub(nIndex, (nIndex == m_nValue));
}

int MenuDlgItemList::GetPos(int y) const
{
	const int val = (y / m_dl.fontsize) + m_dl.basepos;
	if ((val >= 0) && (val < static_cast<int>(m_items.size())))
	{
		return val;
	}
	else
	{
		return -1;
	}
}

enum {
	DLCUR_OUT		= -1,
	DLCUR_INLIST	= 0,
	DLCUR_UP		= 1,
	DLCUR_INBAR		= 2,
	DLCUR_DOWN		= 3,
	DLCUR_PGUP		= 4,
	DLCUR_PGDN		= 5,
	DLCUR_INCUR		= 6
};

int MenuDlgItemList::GetPc(int x, int y) const
{
	if ((unsigned int)x >= (unsigned int)m_vram->width)
	{
		return DLCUR_OUT;
	}
	if ((unsigned int)y >= (unsigned int)m_vram->height)
	{
		return DLCUR_OUT;
	}

	if ((m_items.size() < (size_t)m_dl.dispmax) || (x < (m_vram->width - MENUDLG_CXVSCR)))
	{
		return DLCUR_INLIST;
	}
	else if (y < MENUDLG_CYVSCR)
	{
		return DLCUR_UP;
	}
	else if (y >= (m_vram->height - MENUDLG_CYVSCR))
	{
		return DLCUR_DOWN;
	}
	y -= MENUDLG_CYVSCR;
	y -= BarPos();
	if (y < 0)
	{
		return DLCUR_PGUP;
	}
	else if (y < (int)m_dl.scrollbar)
	{
		return DLCUR_INBAR;
	}
	else
	{
		return DLCUR_PGDN;
	}
}

void MenuDlgItemList::OnSetFont(FONTMNGH font)
{
	// 後でスクロールバーの調整をすべし
	m_font = font;

	int nFontSize = fontmng_getfontsize(GetFont());
	nFontSize = max(nFontSize, 8);
	m_dl.fontsize = (SINT16)nFontSize;
	m_dl.dispmax = (SINT16)(m_vram->height / nFontSize);
}

void MenuDlgItemList::OnSetValue(int val)
{
	if ((val < 0) || (val >= static_cast<int>(m_items.size())))
	{
		val = -1;
	}
	if (val != m_nValue)
	{
		BOOL r = DrawSub(m_nValue, FALSE);
		r |= DrawSub(val, TRUE);
		m_nValue = val;
		if (r)
		{
			Invalidate();
		}
	}
}

int MenuDlgItemList::SetBasePos(int pos)
{
	if (pos < 0)
	{
		pos = 0;
	}
	else
	{
		int displimit = static_cast<int>(m_items.size()) - m_dl.dispmax;
		if (displimit < 0)
		{
			displimit = 0;
		}
		if (pos > displimit)
		{
			pos = displimit;
		}
	}

	int ret = m_dl.basepos;
	if (m_dl.basepos != pos)
	{
		m_dl.basepos = pos;
		DrawAll();
		DrawBar();
	}
	return ret;
}

void MenuDlgItemList::OnClick(int x, int y)
{
	x -= (MENU_LINE * 2);
	y -= (MENU_LINE * 2);
	int flg = GetPc(x, y);
	m_pParent->m_dragflg = flg;

	int val;
	switch (flg)
	{
		case DLCUR_INLIST:
			val = GetPos(y);
			if ((val == m_nValue) && (val != -1))
			{
				m_pParent->m_dragflg = DLCUR_INCUR;
			}
			OnSetValue(val);
			Send(DLGMSG_COMMAND, 0);
			break;

		case 1:
		case 3:
			SetBtn(flg);
			SetBasePos(m_dl.basepos + flg - 2);
			Invalidate();
			break;

		case DLCUR_INBAR:
			y -= MENUDLG_CYVSCR;
			y -= BarPos();
			if ((unsigned int)y < (unsigned int)m_dl.scrollbar)
			{
				m_pParent->m_lasty = y;
			}
			else {
				m_pParent->m_lasty = -1;
			}
			break;

		case DLCUR_PGUP:
			SetBasePos(m_dl.basepos - m_dl.dispmax);
			Invalidate();
			break;

		case DLCUR_PGDN:
			SetBasePos(m_dl.basepos + m_dl.dispmax);
			Invalidate();
			break;
	}
}

void MenuDlgItemList::OnMove(int x, int y, int focus)
{
	x -= (MENU_LINE * 2);
	y -= (MENU_LINE * 2);
	int flg = GetPc(x, y);

	switch (m_pParent->m_dragflg)
	{
		case DLCUR_INLIST:
		case DLCUR_INCUR:
			if (flg == DLCUR_INLIST)
			{
				int val = GetPos(y);
				if (val != m_nValue)
				{
					m_pParent->m_dragflg = DLCUR_INLIST;
					OnSetValue(val);
					Send(DLGMSG_COMMAND, 0);
				}
			}
			break;

		case 1:
		case 3:
			SetBtn(m_pParent->m_dragflg - ((m_pParent->m_dragflg == flg) ? 0 : 1));
			Invalidate();
			break;

		case DLCUR_INBAR:
			if (m_pParent->m_lasty >= 0)
			{
				y -= MENUDLG_CYVSCR;
				y -= m_pParent->m_lasty;
				int height = m_vram->height - (MENUDLG_CYVSCR * 2);
				height -= m_dl.scrollbar;
				if (y < 0)
				{
					y = 0;
				}
				else if (y > height)
				{
					y = height;
				}
				y *= (static_cast<int>(m_items.size()) - m_dl.dispmax);
				y /= height;
				SetBasePos(y);
				Invalidate();
			}
			break;
	}
}

void MenuDlgItemList::OnRelease(int focus)
{
	switch (m_pParent->m_dragflg)
	{
		case 1:
		case 3:
			SetBtn(m_pParent->m_dragflg - 1);
			Invalidate();
			break;

		case DLCUR_INCUR:
			Send(DLGMSG_COMMAND, 1);
			break;
	}
}



/**
 * @brief slider
 */
class MenuDlgItemSlider : public MenuDlgItem
{
public:
	MenuDlgItemSlider(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect);

	virtual BRESULT OnCreate(const void *arg);
	virtual void OnPaint();
	virtual void OnSetValue(int val);
	virtual void OnClick(int x, int y);
	virtual void OnMove(int x, int y, int focus);
	virtual void OnRelease(int focus);
//	virtual INTPTR ItemProc(int ctrl, INTPTR arg);

private:
	DLGSLD m_ds;
	void SetFlag();
	int SetPos(int val);
};

MenuDlgItemSlider::MenuDlgItemSlider(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
	: MenuDlgItem(pParent, DLGTYPE_SLIDER, id, flg, rect)
{
	memset(&m_ds, 0, sizeof(m_ds));

}

BRESULT MenuDlgItemSlider::OnCreate(const void *arg)
{
	m_ds.minval = (SINT16)(long)arg;
	m_ds.maxval = (SINT16)((long)arg >> 16);
	m_ds.moving = 0;
	SetFlag();
	m_ds.pos = SetPos(0);
	return SUCCESS;
}

void MenuDlgItemSlider::SetFlag()
{
	int size;
	if (!(m_flag & MSS_VERT))
	{
		size = m_rect.bottom - m_rect.top;
	}
	else
	{
		size = m_rect.right - m_rect.left;
	}

	UINT type;
	if (size < 13)
	{
		type = 0 + (9 << 8) + (5 << 16);
	}
	else if (size < 21)
	{
		type = 1 + (13 << 8) + (7 << 16);
	}
	else
	{
		type = 2 + (21 << 8) + (11 << 16);
	}
	m_ds.type = (UINT8)type;
	if (!(m_flag & MSS_VERT))
	{
		m_ds.sldh = (UINT8)(type >> 16);
		m_ds.sldv = (UINT8)(type >> 8);
	}
	else
	{
		m_ds.sldh = (UINT8)(type >> 8);
		m_ds.sldv = (UINT8)(type >> 16);
	}
}

void MenuDlgItemSlider::OnPaint()
{
	VRAMHDL vram = GetVram();
	vram_filldat(vram, &m_rect, menucolor[MVC_STATIC]);

	int ptr;
	switch (m_flag & MSS_POSMASK)
	{
		case MSS_BOTH:
			ptr = 1;
			break;

		case MSS_TOP:
			ptr = 2;
			break;

		default:
			ptr = 0;
			break;
	}

	POINT_T pt;
	if (!(m_flag & MSS_VERT))
	{
		RECT_U rct;
		rct.r.left = m_rect.left;
		rct.r.right = m_rect.right;
		rct.r.top = m_rect.top + ptr + (m_ds.sldv / 2) - (MENU_LINE * 2);
		rct.r.bottom = rct.r.top + (MENU_LINE * 4);
		menuvram_box2(vram, &rct.r, MVC4(MVC_SHADOW, MVC_HILIGHT, MVC_DARK, MVC_LIGHT));
		pt.x = m_rect.left + m_ds.pos;
		pt.y = m_rect.top;
	}
	else
	{
		RECT_U rct;
		rct.r.left = m_rect.left + ptr + (m_ds.sldh / 2) - (MENU_LINE * 2);
		rct.r.right = rct.r.left + (MENU_LINE * 4);
		rct.r.top = m_rect.top;
		rct.r.bottom = m_rect.bottom;
		menuvram_box2(vram, &rct.r, MVC4(MVC_SHADOW, MVC_HILIGHT, MVC_DARK, MVC_LIGHT));
		pt.x = m_rect.left;
		pt.y = m_rect.top + m_ds.pos;
		ptr += 3;
	}
	ptr *= 2;
	if ((m_flag & MENU_GRAY) || (m_ds.moving))
	{
		ptr++;
	}

	MENURES2 src;
	src.width = m_ds.sldh;
	src.height = m_ds.sldv;
	src.pat = menures_slddat + menures_sldpos[m_ds.type][ptr];
	menuvram_res2put(vram, &src, &pt);
}

void MenuDlgItemSlider::OnSetValue(int val)
{
	int pos = SetPos(val);
	if (m_ds.pos != pos)
	{
		m_ds.pos = pos;
		Invalidate();
	}
}

int MenuDlgItemSlider::SetPos(int val)
{
	int range = m_ds.maxval - m_ds.minval;
	if (range)
	{
		int dir = (range > 0) ? 1 : -1;
		val -= m_ds.minval;
		val *= dir;
		range *= dir;
		if (val < 0)
		{
			val = 0;
		}
		else if (val >= range)
		{
			val = range;
		}
		m_nValue = m_ds.minval + (val * dir);

		int width;
		if (!(m_flag & MSS_VERT))
		{
			width = m_rect.right - m_rect.left;
			width -= m_ds.sldh;
		}
		else {
			width = m_rect.bottom - m_rect.top;
			width -= m_ds.sldv;
		}
		if ((width > 0) || (range))
		{
			val *= width;
			val /= range;
		}
		else
		{
			val = 0;
		}
	}
	else
	{
		val = 0;
	}
	return val;
}

void MenuDlgItemSlider::OnClick(int x, int y)
{
	int width;
	if (!(m_flag & MSS_VERT))
	{
		width = m_ds.sldh;
	}
	else
	{
		width = m_ds.sldv;
		x = y;
	}
	x -= m_ds.pos;
	if ((x >= -1) && (x <= width))
	{
		m_pParent->m_dragflg = x;
		m_ds.moving = 1;
		Invalidate();
	}
	else {
		m_pParent->m_dragflg = -1;
		int dir = (x > 0) ? 1 : 0;
		int range = m_ds.maxval - m_ds.minval;
		if (range < 0)
		{
			range = 0 - range;
			dir ^= 1;
		}
		if (range < 16)
		{
			range = 16;
		}
		range >>= 4;
		if (!dir)
		{
			range = 0 - range;
		}
		OnSetValue(m_nValue + range);
		Send(DLGMSG_COMMAND);
	}
}

void MenuDlgItemSlider::OnMove(int x, int y, int focus)
{
	if (m_ds.moving)
	{
		int range = m_ds.maxval - m_ds.minval;
		if (range)
		{
			int dir = (range > 0) ? 1 : -1;
			range *= dir;
			int width = 0;
			if (!(m_flag & MSS_VERT))
			{
				width = m_rect.right - m_rect.left;
				width -= m_ds.sldh;
			}
			else
			{
				width = m_rect.bottom - m_rect.top;
				width -= m_ds.sldv;
				x = y;
			}
			x -= m_pParent->m_dragflg;
			if ((x < 0) || (width <= 0))
			{
				x = 0;
			}
			else if (x >= width)
			{
				x = range;
			}
			else
			{
				x *= range;
				x += (width >> 1);
				x /= width;
			}
			x = m_ds.minval + (x * dir);
			OnSetValue(x);
			Send(DLGMSG_COMMAND);
		}
	}
}

void MenuDlgItemSlider::OnRelease(int focus)
{
	if (m_ds.moving)
	{
		m_ds.moving = 0;
		Invalidate();
	}
}



// ---- tablist

class MenuDlgItemTabList : public MenuDlgItem
{
public:
	MenuDlgItemTabList(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect);
	virtual ~MenuDlgItemTabList();

	virtual BRESULT OnCreate(const void *arg);
	virtual void OnPaint();
	virtual void OnSetFont(FONTMNGH font);
	virtual void OnSetValue(int val);
	virtual void OnClick(int x, int y);
//	virtual INTPTR ItemProc(int ctrl, INTPTR arg);

public:
	void Append(const OEMCHAR* lpString);

private:
	struct TabItem
	{
		const OEMCHAR* lpString;
		int width;
	};
	std::vector<TabItem> m_items;
	int m_nFontSize;
};

MenuDlgItemTabList::MenuDlgItemTabList(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
	: MenuDlgItem(pParent, DLGTYPE_TABLIST, id, flg, rect)
	, m_nFontSize(0)
{
}

MenuDlgItemTabList::~MenuDlgItemTabList()
{
	for (std::vector<TabItem>::iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		delete[] it->lpString;
		it->lpString = NULL;
	}
}

BRESULT MenuDlgItemTabList::OnCreate(const void *arg)
{
	RECT_T rct;
	rct.right = m_rect.right - m_rect.left;
	m_nValue = -1;
	OnSetFont(NULL);
	return SUCCESS;
}

void MenuDlgItemTabList::OnPaint()
{
	VRAMHDL dst = GetVram();
	FONTMNGH font = GetFont();
	RECT_T rct = m_rect;
	vram_filldat(dst, &rct, menucolor[MVC_STATIC]);

	int tabey = rct.top + m_nFontSize + MENUDLG_SYTAB + MENUDLG_TYTAB + MENUDLG_EYTAB;
	rct.top = tabey;
	menuvram_box2(dst, &rct, MVC4(MVC_HILIGHT, MVC_DARK, MVC_LIGHT, MVC_SHADOW));

	int posx = m_rect.left + (MENU_LINE * 2);
	for (std::vector<TabItem>::iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		if (std::distance(m_items.begin(), it) != m_nValue)
		{
			POINT_T pt;
			pt.x = posx;
			pt.y = m_rect.top + MENUDLG_SYTAB;
			menuvram_liney(dst, pt.x, pt.y + (MENU_LINE * 2), tabey, MVC_HILIGHT);
			pt.x += MENU_LINE;
			menuvram_liney(dst, pt.x, pt.y + MENU_LINE, pt.y + (MENU_LINE * 2), MVC_HILIGHT);
			menuvram_liney(dst, pt.x, pt.y + (MENU_LINE * 2), tabey, MVC_LIGHT);
			pt.x += MENU_LINE;
			int lx = pt.x + it->width + (MENUDLG_TXTAB * 2);
			menuvram_linex(dst, pt.x, pt.y, lx, MVC_HILIGHT);
			menuvram_linex(dst, pt.x, pt.y + MENU_LINE, lx, MVC_LIGHT);

			menuvram_liney(dst, lx, pt.y + MENU_LINE, pt.y + (MENU_LINE * 2), MVC_DARK);
			menuvram_liney(dst, lx, pt.y + (MENU_LINE * 2), tabey, MVC_SHADOW);
			lx++;
			menuvram_liney(dst, lx, pt.y + (MENU_LINE * 2), tabey, MVC_DARK);
			pt.x += MENUDLG_TXTAB;
			pt.y += MENUDLG_TYTAB;
			vrammix_text(dst, font, it->lpString, menucolor[MVC_TEXT], &pt, NULL);
		}
		posx += it->width + (MENU_LINE * 4) + (MENUDLG_TXTAB) * 2;
	}

	posx = m_rect.left;
	for (std::vector<TabItem>::iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		if (std::distance(m_items.begin(), it) == m_nValue)
		{
			POINT_T pt;
			pt.x = posx;
			pt.y = m_rect.top;

			int tabdy;
			if (posx == m_rect.left)
			{
				tabdy = tabey + 2;
			}
			else
			{
				tabdy = tabey + 1;
				menuvram_linex(dst, pt.x, tabdy, pt.x + (MENU_LINE * 2), MVC_STATIC);
			}
			menuvram_liney(dst, pt.x, pt.y + (MENU_LINE * 2), tabdy, MVC_HILIGHT);
			pt.x += MENU_LINE;
			menuvram_liney(dst, pt.x, pt.y + MENU_LINE, pt.y + (MENU_LINE * 2), MVC_HILIGHT);
			menuvram_liney(dst, pt.x, pt.y + (MENU_LINE * 2), tabdy, MVC_LIGHT);
			pt.x += MENU_LINE;
			int lx = pt.x + it->width + (MENU_LINE * 4) + (MENUDLG_TXTAB * 2);
			menuvram_linex(dst, pt.x, pt.y, lx, MVC_HILIGHT);
			menuvram_linex(dst, pt.x, pt.y + MENU_LINE, lx, MVC_LIGHT);
			menuvram_linex(dst, pt.x, tabey, lx, MVC_STATIC);
			menuvram_linex(dst, pt.x, tabey + MENU_LINE, lx, MVC_STATIC);
			tabdy = tabey + 1;
			menuvram_liney(dst, lx, pt.y + MENU_LINE, pt.y + (MENU_LINE * 2), MVC_DARK);
			menuvram_liney(dst, lx, pt.y + (MENU_LINE * 2), tabdy, MVC_SHADOW);
			lx++;
			menuvram_liney(dst, lx, pt.y + (MENU_LINE * 2), tabdy, MVC_DARK);
			pt.x += MENUDLG_TXTAB + (MENU_LINE * 2);
			pt.y += MENUDLG_TYTAB;
			vrammix_text(dst, font, it->lpString, menucolor[MVC_TEXT], &pt, NULL);
			break;
		}
		posx += it->width + (MENU_LINE * 4) + (MENUDLG_TXTAB * 2);
	}
}

void MenuDlgItemTabList::OnSetFont(FONTMNGH font)
{
	m_font = font;
	font = GetFont();

	m_nFontSize = fontmng_getfontsize(font);
	m_nFontSize = max(m_nFontSize, 8);

	for (std::vector<TabItem>::iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		POINT_T pt;
		fontmng_getsize(font, it->lpString, &pt);
		it->width = pt.x;
	}
}

void MenuDlgItemTabList::OnSetValue(int val)
{
	if (m_nValue != val)
	{
		m_nValue = val;
		Invalidate();
	}
}

void MenuDlgItemTabList::Append(const OEMCHAR *lpString)
{
	POINT_T pt;
	fontmng_getsize(GetFont(), lpString, &pt);

	TabItem item;
	item.lpString = StrDup(lpString);
	item.width = pt.x;
	m_items.push_back(item);
}

void MenuDlgItemTabList::OnClick(int x, int y)
{
	if (y < (m_nFontSize + MENUDLG_SYTAB + MENUDLG_TYTAB + MENUDLG_EYTAB))
	{
		for (std::vector<TabItem>::iterator it = m_items.begin(); it != m_items.end(); ++it)
		{
			x -= (MENU_LINE * 4);
			if (x < 0)
			{
				break;
			}
			x -= it->width + (MENUDLG_TXTAB * 2);
			if (x < 0)
			{
				OnSetValue(static_cast<int>(std::distance(m_items.begin(), it)));
				Send(DLGMSG_COMMAND);
				break;
			}
		}
	}
}



/**
 * @brief EDIT
 */
class MenuDlgItemEdit : public MenuDlgItemText
{
public:
	MenuDlgItemEdit(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
		: MenuDlgItemText(pParent, DLGTYPE_EDIT, id, flg, rect)
	{
	}

	virtual void OnPaint()
	{
		VRAMHDL vram = GetVram();

		RECT_T rct = m_rect;
		menuvram_box2(vram, &rct, MVC4(MVC_SHADOW, MVC_HILIGHT, MVC_DARK, MVC_LIGHT));

		rct.left += (MENU_LINE * 2);
		rct.top += (MENU_LINE * 2);
		rct.right -= (MENU_LINE * 2);
		rct.bottom -= (MENU_LINE * 2);
		vram_filldat(vram, &rct, menucolor[(m_flag & MENU_GRAY) ? MVC_STATIC : MVC_HILIGHT]);

		if (m_lpString)
		{
			POINT_T pt;
			pt.x = rct.left + MENU_LINE;
			pt.y = rct.top + MENU_LINE;
			vrammix_text(vram, GetFont(), m_lpString, menucolor[MVC_TEXT], &pt, &rct);
		}
	}
};


/**
 * @brief frame
 */
class MenuDlgItemFrame : public MenuDlgItemText
{
public:
	MenuDlgItemFrame(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
		: MenuDlgItemText(pParent, DLGTYPE_FRAME, id, flg, rect)
	{
	}

	virtual void OnPaint()
	{
		VRAMHDL vram = GetVram();

		RECT_T rct;
		rct.left = m_rect.left;
		rct.top = m_rect.top + MENUDLG_SYFRAME;
		rct.right = m_rect.right;
		rct.bottom = m_rect.bottom;
		menuvram_box2(vram, &rct, MVC4(MVC_SHADOW, MVC_HILIGHT, MVC_HILIGHT, MVC_SHADOW));

		rct.left += MENUDLG_SXFRAME;
		rct.top = m_rect.top;
		rct.right = rct.left + (MENUDLG_PXFRAME * 2) + m_size.x;
		rct.bottom = rct.top + m_size.y + MENU_DSTEXT;
		vram_filldat(vram, &rct, menucolor[MVC_STATIC]);

		POINT_T pt;
		pt.x = rct.left + MENUDLG_PXFRAME;
		pt.y = rct.top;
		DrawText(pt);
	}
};



/**
 * @brief radio
 */
class MenuDlgItemRadio : public MenuDlgItemText
{
public:
	MenuDlgItemRadio(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect);

	virtual void OnPaint();
	virtual void OnSetValue(int val);
	virtual void OnClick(int x, int y);
};

MenuDlgItemRadio::MenuDlgItemRadio(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
	: MenuDlgItemText(pParent, DLGTYPE_RADIO, id, flg, rect)
{
}

void MenuDlgItemRadio::OnPaint()
{
	VRAMHDL vram = GetVram();
	vram_filldat(vram, &m_rect, menucolor[MVC_STATIC]);
	POINT_T pt;
	pt.x = m_rect.left;
	pt.y = m_rect.top;
	const MENURES2* src = menures_radio;
	int pat = (m_flag & MENU_GRAY) ? 1 : 0;
	menuvram_res2put(vram, src + pat, &pt);
	if (m_nValue)
	{
		menuvram_res3put(vram, src + 2, &pt, (m_flag & MENU_GRAY) ? MVC_GRAYTEXT1 : MVC_TEXT);
	}
	pt.x += MENUDLG_SXRADIO;
	DrawText(pt);
}

void MenuDlgItemRadio::OnSetValue(int val)
{
	if (m_nValue != val)
	{
		if (val)
		{
			for (std::vector<MenuDlgItem*>::iterator it = m_pParent->m_items.begin(); it != m_pParent->m_items.end(); ++it)
			{
				MenuDlgItemRadio* item = static_cast<MenuDlgItemRadio*>(*it);
				if ((item->m_type == DLGTYPE_RADIO) && (item->m_group == m_group))
				{
					item->OnSetValue(0);
				}
			}
		}
		m_nValue = val;
		Invalidate();
	}
}

void MenuDlgItemRadio::OnClick(int x, int y)
{
	if (x < (m_size.x + MENUDLG_SXRADIO))
	{
		OnSetValue(1);
		Send(DLGMSG_COMMAND);
	}
}



/**
 * @brief check
 */
class MenuDlgItemCheck : public MenuDlgItemText
{
public:
	MenuDlgItemCheck(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect);

	virtual void OnPaint();
	virtual void OnSetValue(int val);
	virtual void OnClick(int x, int y);
};

MenuDlgItemCheck::MenuDlgItemCheck(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
	: MenuDlgItemText(pParent, DLGTYPE_CHECK, id, flg, rect)
{
}

void MenuDlgItemCheck::OnPaint()
{

	VRAMHDL vram = GetVram();
	vram_filldat(vram, &m_rect, menucolor[MVC_STATIC]);

	RECT_T rct;
	rct.left = m_rect.left;
	rct.top = m_rect.top;
	rct.right = rct.left + MENUDLG_CXCHECK;
	rct.bottom = rct.top + MENUDLG_CYCHECK;

	UINT32 basecol;
	UINT32 txtcol;
	if (!(m_flag & MENU_GRAY))
	{
		basecol = MVC_HILIGHT;
		txtcol = MVC_TEXT;
	}
	else
	{
		basecol = MVC_STATIC;
		txtcol = MVC_GRAYTEXT1;
	}
	vram_filldat(vram, &rct, menucolor[basecol]);
	menuvram_box2(vram, &rct, MVC4(MVC_SHADOW, MVC_HILIGHT, MVC_DARK, MVC_LIGHT));
	if (m_nValue)
	{
		POINT_T pt;
		pt.x = rct.left + (MENU_LINE * 2);
		pt.y = rct.top + (MENU_LINE * 2);
		menuvram_res3put(vram, &menures_check, &pt, txtcol);
	}
	POINT_T pt;
	pt.x = rct.left + MENUDLG_SXCHECK;
	pt.y = rct.top;
	DrawText(pt);
}

void MenuDlgItemCheck::OnSetValue(int val)
{
	if (m_nValue != val)
	{
		m_nValue = val;
		Invalidate();
	}
}

void MenuDlgItemCheck::OnClick(int x, int y)
{
	if (x < (m_size.x + MENUDLG_SXCHECK))
	{
		OnSetValue(!m_nValue);
		Send(DLGMSG_COMMAND);
	}
}



/**
 * @brief アイコン
 */
class MenuDlgItemIcon : public MenuDlgItem
{
public:
	MenuDlgItemIcon(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
		: MenuDlgItem(pParent, DLGTYPE_ICON, id, flg, rect)
	{
	}

	virtual ~MenuDlgItemIcon()
	{
		menuicon_unlock(m_icon);
	}

	virtual BRESULT OnCreate(const void *arg)
	{
		int width = m_rect.right - m_rect.left;
		int height = m_rect.bottom - m_rect.top;
		m_icon = menuicon_lock(static_cast<UINT16>(reinterpret_cast<INTPTR>(arg)), width, height, GetVram()->bpp);
		return SUCCESS;
	}

	virtual void OnPaint()
	{
		PaintIcon(m_icon);
	}

private:
	VRAMHDL m_icon;
};



/**
 * @brief VRAM
 */
class MenuDlgItemVram : public MenuDlgItem
{
public:
	MenuDlgItemVram(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
		: MenuDlgItem(pParent, DLGTYPE_VRAM, id, flg, rect)
		, m_resource(NULL)
	{
	}

	virtual BRESULT OnCreate(const void *arg)
	{
		m_resource = static_cast<VRAMHDL>(const_cast<void*>(arg));
		return SUCCESS;
	}

	virtual void OnPaint()
	{
		PaintIcon(m_resource);
	}

	virtual INTPTR ItemProc(int ctrl, INTPTR arg)
	{
		INTPTR ret = 0;
		switch (ctrl)
		{
			case DMSG_SETVRAM:
				ret = reinterpret_cast<INTPTR>(m_resource);
				m_resource = reinterpret_cast<VRAMHDL>(arg);
				Invalidate();
				break;

			default:
				ret = __super::ItemProc(ctrl, arg);
				break;
		}
		return ret;
	}

private:
	VRAMHDL m_resource;
};



/**
 * @brief ライン
 */
class MenuDlgItemLine : public MenuDlgItem
{
public:
	MenuDlgItemLine(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
		: MenuDlgItem(pParent, DLGTYPE_LINE, id, flg, rect)
	{
	}

	virtual void OnPaint()
	{
		VRAMHDL vram = GetVram();
		if (!(m_flag & MSL_VERT))
		{
			menuvram_linex(vram, m_rect.left, m_rect.top, m_rect.right, MVC_SHADOW);
			menuvram_linex(vram, m_rect.left, m_rect.top + MENU_LINE, m_rect.right, MVC_HILIGHT);
		}
		else
		{
			menuvram_liney(vram, m_rect.left, m_rect.top, m_rect.bottom, MVC_SHADOW);
			menuvram_liney(vram, m_rect.left + MENU_LINE, m_rect.top, m_rect.bottom, MVC_HILIGHT);
		}

	}
};



/**
 * @brief BOX
 */
class MenuDlgItemBox : public MenuDlgItem
{
public:
	MenuDlgItemBox(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
		: MenuDlgItem(pParent, DLGTYPE_BOX, id, flg, rect)
	{
	}

	virtual void OnPaint()
	{
		VRAMHDL vram = GetVram();
		menuvram_box2(vram, &m_rect, MVC4(MVC_SHADOW, MVC_HILIGHT, MVC_HILIGHT, MVC_SHADOW));
	}
};



/**
 * アイテムを作成
 */
MenuDlgItem* MenuDlgItem::CreateInstance(int type, MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect, const void *arg)
{
	MenuDlgItem* item = NULL;
	switch (type)
	{
		case DLGTYPE_BASE:
			item = new MenuDlgItemBase(pParent, id, flg, rect);
			break;

		case DLGTYPE_CLOSE:
			item = new MenuDlgItemClose(pParent, id, flg, rect);
			break;

		case DLGTYPE_BUTTON:
			item = new MenuDlgItemButton(pParent, id, flg, rect);
			break;

		case DLGTYPE_LIST:
			item = new MenuDlgItemList(pParent, id, flg, rect);
			break;

		case DLGTYPE_SLIDER:
			item = new MenuDlgItemSlider(pParent, id, flg, rect);
			break;

		case DLGTYPE_TABLIST:
			item = new MenuDlgItemTabList(pParent, id, flg, rect);
			break;

		case DLGTYPE_RADIO:
			item = new MenuDlgItemRadio(pParent, id, flg, rect);
			break;

		case DLGTYPE_CHECK:
			item = new MenuDlgItemCheck(pParent, id, flg, rect);
			break;

		case DLGTYPE_EDIT:
			item = new MenuDlgItemEdit(pParent, id, flg, rect);
			break;

		case DLGTYPE_FRAME:
			item = new MenuDlgItemFrame(pParent, id, flg, rect);
			break;

		case DLGTYPE_TEXT:
			item = new MenuDlgItemText(pParent, type, id, flg, rect);
			break;

		case DLGTYPE_ICON:
			item = new MenuDlgItemIcon(pParent, id, flg, rect);
			break;

		case DLGTYPE_VRAM:
			item = new MenuDlgItemVram(pParent, id, flg, rect);
			break;

		case DLGTYPE_LINE:
			item = new MenuDlgItemLine(pParent, id, flg, rect);
			break;

		case DLGTYPE_BOX:
			item = new MenuDlgItemBox(pParent, id, flg, rect);
			break;
	}
	if (item != NULL)
	{
		if (item->OnCreate(arg) != SUCCESS)
		{
			delete item;
			item = NULL;
		}
	}
	return item;
}

// ----

/**
 * コンストラクタ
 */
MenuDialog::MenuDialog()
	: m_pMenuBase(MenuBase::GetInstance())
	, m_vram(NULL)
	, m_nLocked(0)
	, m_bClosing(false)
	, m_sx(0)
	, m_sy(0)
	, m_font(NULL)
	, m_page(0)
	, m_group(0)
	, m_proc(NULL)
	, m_dragflg(0)
	, m_btn(0)
	, m_lastx(0)
	, m_lasty(0)
	, m_lastid(0)
{
}

/**
 * アイテムを得る
 */
MenuDlgItem* MenuDialog::GetItem(MENUID id) const
{
	for (std::vector<MenuDlgItem*>::const_iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		const MenuDlgItem* item = *it;
		if (id == item->m_id)
		{
			return const_cast<MenuDlgItem*>(item);
		}
	}
	return NULL;
}

/**
 * アイテムを得る
 */
MenuDlgItem* MenuDialog::GetItemFromPosition(int x, int y) const
{
	for (std::vector<MenuDlgItem*>::const_reverse_iterator it = m_items.rbegin(); it != m_items.rend(); ++it)
	{
		const MenuDlgItem* item = *it;
		if ((!(item->m_flag & (MENU_DISABLE | MENU_GRAY))) && (rect_in(&item->m_rect, x, y)))
		{
			return const_cast<MenuDlgItem*>(item);
		}
	}
	return NULL;
}

void MenuDialog::DrawItem(MenuDlgItem* item)
{
	RECT_T rect;

	if (item)
	{
		if (item->m_flag & MENU_DISABLE)
		{
			return;
		}
		rect = item->m_rect;
	}
	else
	{
		rect.left = 0;
		rect.top = 0;
		rect.right = m_vram->width;
		rect.bottom = m_vram->height;
	}

	for (std::vector<MenuDlgItem*>::reverse_iterator it = m_items.rbegin(); it != m_items.rend(); ++it)
	{
		MenuDlgItem* item2 = *it;
		if ((!(item2->m_flag & MENU_DISABLE)) && (rect_isoverlap(&rect, &item2->m_rect)))
		{
			item2->m_flag |= MENU_REDRAW;
		}
		if (item == item2)
		{
			break;
		}
	}
	Draw();
}

void MenuDialog::DrawLock(bool lock)
{
	if (lock)
	{
		m_nLocked++;
	}
	else
	{
		m_nLocked--;
		Draw();
	}
}

static void draw(VRAMHDL dst, const RECT_T *rect, void *arg)
{
	MenuDialog* dlg = static_cast<MenuDialog*>(arg);
	vrammix_cpy2(dst, rect, dlg->m_vram, NULL, 2);
}

void MenuDialog::Draw()
{
	if (m_nLocked)
	{
		return;
	}

	for (std::vector<MenuDlgItem*>::iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		MenuDlgItem* item = *it;
		if (item->m_flag & MENU_REDRAW)
		{
			item->m_flag &= ~MENU_REDRAW;
			if (!(item->m_flag & MENU_DISABLE))
			{
				item->OnPaint();
				m_pMenuBase->Invalidate(m_vram, &item->m_rect);
			}
		}
	}
	m_pMenuBase->Draw(draw, this);
}

static int defproc(int msg, MENUID id, long param) {

	if (msg == DLGMSG_CLOSE)
	{
		MenuBase::GetInstance()->Close();
	}
	(void)id;
	(void)param;
	return(0);
}

/**
 * 作成
 */
bool MenuDialog::Create(int width, int height, const OEMCHAR *str, int (*proc)(int msg, MENUID id, long param))
{
	if ((width <= 0) || (height <= 0))
	{
		goto mdcre_err;
	}

	if (!m_pMenuBase->Open(2))
	{
		goto mdcre_err;
	}

	m_vram = NULL;
	m_nLocked = 0;
	m_bClosing = false;
	m_sx = 0;
	m_sy = 0;
	m_font = NULL;
	m_page = 0;
	m_group = 0;
	m_proc = NULL;
	m_dragflg = 0;
	m_btn = 0;
	m_lastx = 0;
	m_lasty = 0;
	m_lastid = 0;

	width += (MENU_FBORDER + MENU_BORDER) * 2;
	height += ((MENU_FBORDER + MENU_BORDER) * 2) + MENUDLG_CYCAPTION + MENUDLG_BORDER;

	m_font = m_pMenuBase->GetFont();
	m_vram = vram_create(width, height, FALSE, m_pMenuBase->Bpp());
	if (m_vram == NULL)
	{
		goto mdcre_err;
	}
	m_vram->posx = (m_pMenuBase->Width() - width) >> 1;
	m_vram->posy = (m_pMenuBase->Height() - height) >> 1;
	if (!Append(DLGTYPE_BASE, SID_CAPTION, 0, str, 0, 0, width, height))
	{
		goto mdcre_err;
	}
	if (!Append(DLGTYPE_CLOSE, SID_CLOSE, 0, NULL,
							width - (MENU_FBORDER + MENU_BORDER) -
									(MENUDLG_CXCLOSE + MENUDLG_PXCAPTION),
							(MENU_FBORDER + MENU_BORDER) +
								((MENUDLG_CYCAPTION - MENUDLG_CYCLOSE) / 2),
							MENUDLG_CXCLOSE, MENUDLG_CYCLOSE))
	{
		goto mdcre_err;
	}
	m_sx = (MENU_FBORDER + MENU_BORDER);
	m_sy = (MENU_FBORDER + MENU_BORDER) + (MENUDLG_CYCAPTION + MENUDLG_BORDER);
	if (proc == NULL)
	{
		proc = defproc;
	}
	m_proc = proc;
	m_nLocked = 0;
	DrawLock(true);
	(*proc)(DLGMSG_CREATE, 0, 0);
	DrawItem(NULL);
	DrawLock(false);

	return true;

mdcre_err:
	m_pMenuBase->Close();
	return false;
}

void MenuDialog::Destroy()
{
	if (m_bClosing)
	{
		return;
	}
	m_bClosing = false;
	(*m_proc)(DLGMSG_DESTROY, 0, 0);

	for (std::vector<MenuDlgItem*>::iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		delete (*it);
	}
	m_items.clear();

	m_pMenuBase->Clear(m_vram);
	vram_destroy(m_vram);
	m_vram = NULL;
}

/**
 * アイテムの追加
 */
bool MenuDialog::Append(const MENUPRM *res, int count)
{
	bool r = false;
	while (count--)
	{
		if (!Append(res->type, res->id, res->flg, res->arg, res->posx, res->posy, res->width, res->height))
		{
			r = true;
		}
		res++;
	}
	return !r;
}

/**
 * アイテムの追加
 */
bool MenuDialog::Append(int type, MENUID id, MENUFLG flg, const void *arg, int posx, int posy, int width, int height)
{
	if (flg & MENU_TABSTOP)
	{
		m_group++;
	}
	switch (type)
	{
		case DLGTYPE_LTEXT:
			type = DLGTYPE_TEXT;
			flg &= ~MST_POSMASK;
			flg |= MST_LEFT;
			break;

		case DLGTYPE_CTEXT:
			type = DLGTYPE_TEXT;
			flg &= ~MST_POSMASK;
			flg |= MST_CENTER;
			break;

		case DLGTYPE_RTEXT:
			type = DLGTYPE_TEXT;
			flg &= ~MST_POSMASK;
			flg |= MST_RIGHT;
			break;
	}

	RECT_T rect;
	rect.left = m_sx + posx;
	rect.top = m_sy + posy;
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;
	MenuDlgItem* item = MenuDlgItem::CreateInstance(type, this, id, flg, rect, arg);
	if (item == NULL)
	{
		return false;
	}

	DrawLock(true);
	m_items.push_back(item);
	DrawItem(item);
	DrawLock(false);
	return true;
}


// ---- moving

void MenuDialog::Moving(int x, int y, int btn)
{
	DrawLock(true);
	x -= m_vram->posx;
	y -= m_vram->posy;
	if (!m_btn)
	{
		if (btn == 1)
		{
			MenuDlgItem* hdl = GetItemFromPosition(x, y);
			if (hdl) {
				x -= hdl->m_rect.left;
				y -= hdl->m_rect.top;
				m_btn = 1;
				m_lastid = hdl->m_id;
				hdl->OnClick(x, y);
			}
		}
	}
	else
	{
		MenuDlgItem* hdl = GetItem(m_lastid);
		if (hdl) {
			int focus = rect_in(&hdl->m_rect, x, y);
			x -= hdl->m_rect.left;
			y -= hdl->m_rect.top;
			hdl->OnMove(x, y, focus);
			if (btn == 2)
			{
				m_btn = 0;
				hdl->OnRelease(focus);
			}
		}
	}
	DrawLock(false);
}


// ---- ctrl

INTPTR MenuDialog::Send(int ctrl, MENUID id, INTPTR arg)
{
	MenuDlgItem* hdl = GetItem(id);
	if (hdl == NULL)
	{
		return 0;
	}
	DrawLock(true);
	INTPTR ret = hdl->ItemProc(ctrl, arg);
	DrawLock(false);
	return ret;
}

INTPTR MenuDlgItem::ItemProc(int ctrl, INTPTR arg)
{
	int flg = 0;

	INTPTR ret = 0;
	switch (ctrl) 
	{
		case DMSG_SETHIDE:
			ret = (m_flag & MENU_DISABLE) ? 1 : 0;
			flg = (arg) ? MENU_DISABLE : 0;
			if ((m_flag ^ flg) & MENU_DISABLE)
			{
				m_flag ^= MENU_DISABLE;
				if (flg)
				{
					m_pParent->DrawItem(NULL);
				}
				else
				{
					Invalidate();
				}
			}
			break;

		case DMSG_GETHIDE:
			ret = (m_flag & MENU_DISABLE) ? 1 : 0;
			break;

		case DMSG_SETENABLE:
			ret = (m_flag & MENU_GRAY) ? 0 : 1;
			flg = (arg) ? 0 : MENU_GRAY;
			if ((m_flag ^ flg) & MENU_GRAY)
			{
				m_flag ^= MENU_GRAY;
				Invalidate();
			}
			break;

		case DMSG_GETENABLE:
			ret = (m_flag & MENU_GRAY) ? 0 : 1;
			break;

		case DMSG_SETVAL:
			ret = m_nValue;
			OnSetValue((int)arg);
			break;

		case DMSG_GETVAL:
			ret = m_nValue;
			break;

		case DMSG_SETTEXT:
			OnSetText(reinterpret_cast<OEMCHAR*>(arg));
			break;

		case DMSG_ITEMAPPEND:
			switch (m_type)
			{
				case DLGTYPE_LIST:
					if ((static_cast<MenuDlgItemList*>(this))->Append(reinterpret_cast<const OEMCHAR*>(arg)))
					{
						Invalidate();
					}
					break;

				case DLGTYPE_TABLIST:
					(static_cast<MenuDlgItemTabList*>(this))->Append(reinterpret_cast<const OEMCHAR*>(arg));
					Invalidate();
					break;
			}
			break;

		case DMSG_ITEMRESET:
			if ((m_pParent->m_btn) && (m_pParent->m_lastid == m_id))
			{
				m_pParent->m_btn = 0;
				OnRelease(FALSE);
			}
			if (m_type == DLGTYPE_LIST)
			{
				(static_cast<MenuDlgItemList*>(this))->Reset();
				Invalidate();
			}
			break;

		case DMSG_ITEMSETEX:
			if (m_type == DLGTYPE_LIST)
			{
				if ((static_cast<MenuDlgItemList*>(this))->SetEx((ITEMEXPRM *)arg))
				{
					Invalidate();
				}
			}
			break;

		case DMSG_SETLISTPOS:
			if (m_type == DLGTYPE_LIST)
			{
				ret = (static_cast<MenuDlgItemList*>(this))->SetBasePos((int)arg);
				Invalidate();
			}
			break;

		case DMSG_GETRECT:
			ret = (INTPTR)&m_rect;
			break;

		case DMSG_SETRECT:
			ret = (INTPTR)&m_rect;
			if ((m_type == DLGTYPE_TEXT) && (arg))
			{
				Invalidate();
				m_rect = *(RECT_T *)arg;
				Invalidate();
			}
			break;

		case DMSG_SETFONT:
			ret = reinterpret_cast<INTPTR>(GetFont());
			OnSetFont(reinterpret_cast<FONTMNGH>(arg));
			Invalidate();
			break;

		case DMSG_GETFONT:
			ret = reinterpret_cast<INTPTR>(GetFont());
			break;
	}
	return ret;
}



// --- page

void MenuDialog::SetPage(MENUID page)
{
	m_page = page;
}

void MenuDialog::DispPageHidden(MENUID page, bool hidden)
{
	const MENUFLG flag = (hidden) ? MENU_DISABLE : 0;
	for (std::vector<MenuDlgItem*>::iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		MenuDlgItem* item = *it;
		if (item->m_page == page)
		{
			item->m_flag &= ~MENU_DISABLE;
			item->m_flag |= flag;
		}
	}
	DrawLock(true);
	DrawItem(NULL);
	DrawLock(false);
}


// ----

BRESULT menudlg_create(int width, int height, const OEMCHAR *str, int (*proc)(int msg, MENUID id, long param))
{
	return s_menudlg.Create(width, height, str, proc) ? SUCCESS : FAILURE;
}

void menudlg_destroy(void)
{
	s_menudlg.Destroy();
}

BRESULT menudlg_appends(const MENUPRM *res, int count)
{
	return s_menudlg.Append(res, count) ? SUCCESS : FAILURE;
}

BRESULT menudlg_append(int type, MENUID id, MENUFLG flg, const void *arg, int posx, int posy, int width, int height)
{
	return s_menudlg.Append(type, id, flg, arg, posx, posy, width, height) ? SUCCESS : FAILURE;
}

void menudlg_moving(int x, int y, int btn)
{
	s_menudlg.Moving(x, y, btn);
}

INTPTR menudlg_msg(int ctrl, MENUID id, INTPTR arg)
{
	return s_menudlg.Send(ctrl, id, arg);
}

void menudlg_setpage(MENUID page)
{
	s_menudlg.SetPage(page);
}

void menudlg_disppagehidden(MENUID page, BOOL hidden)
{
	s_menudlg.DispPageHidden(page, (hidden) ? true : false);
}

/**
 * @file	menusys.c
 * @brief	Implementation of the menu of the system
 */

#include "compiler.h"
#include "menusys.h"
#include "menubase.h"
#include "menudeco.inc"
#include "menures.h"
#include "menuvram.h"
#include "../vrammix.h"
#include "fontmng.h"
#include "inputmng.h"
#include <vector>

class MenuSysWnd;

/**
 * @brief item
 */
struct MenuSysItem
{
	MenuSysWnd* child;
	MENUID		id;
	MENUFLG		flag;
	RECT_T		rct;
	OEMCHAR		string[32];
};
typedef MenuSysItem* MENUHDL;

/**
 * @brief wnd
 */
class MenuSysWnd : public std::vector<MenuSysItem>
{
public:
	MenuSysWnd(MenuSysWnd* pParent = NULL);
	~MenuSysWnd();
	void Append(const MSYSITEM* lpItems);
	MenuSysItem* GetItem(int id);

private:
	MenuSysWnd* m_pParent;

public:
	VRAMHDL m_vram;
	int m_focus;
};

/**
 * �R���X�g���N�^
 */
MenuSysWnd::MenuSysWnd(MenuSysWnd* pParent)
	: m_pParent(pParent)
	, m_vram(NULL)
	, m_focus(-1)
{
}

/**
 * �f�X�g���N�^
 */
MenuSysWnd::~MenuSysWnd()
{
	for (iterator it = begin(); it != end(); ++it)
	{
		if (it->child)
		{
			delete it->child;
			it->child = NULL;
		}
	}
}

/**
 * �A�C�e���ǉ�
 */
void MenuSysWnd::Append(const MSYSITEM *lpItems)
{
	if (lpItems == NULL)
	{
		return;
	}

	while (true /*CONSTCOND*/)
	{
		MenuSysItem item;
		memset(&item, 0, sizeof(item));
		item.id = lpItems->id;
		item.flag = lpItems->flag & (~MENU_DELETED);
		if (lpItems->string)
		{
			milstr_ncpy(item.string, lpItems->string, NELEMENTS(item.string));
		}
		if (lpItems->child)
		{
			item.child = new MenuSysWnd(this);
			item.child->Append(lpItems->child);
		}
		push_back(item);
		if (lpItems->flag & MENU_DELETED)
		{
			break;
		}
		lpItems++;
	}
}

/**
 * �A�C�e���𓾂�
 */
MenuSysItem* MenuSysWnd::GetItem(int id)
{
	for (MenuSysWnd::iterator it = begin(); it != end(); ++it)
	{
		if (it->id == id)
		{
			return &*it;
		}
		if (it->child)
		{
			MenuSysItem* item = it->child->GetItem(id);
			if (item)
			{
				return item;
			}
		}
	}
	return NULL;
}

class MenuSys : public std::vector<MenuSysWnd*>
{
public:
	MenuSys();
	BRESULT Create(const MSYSITEM *item, void (*cmd)(MENUID id), UINT16 icon, const OEMCHAR *title);
	void Destroy();
	BRESULT Open(int x, int y);
	void Close();
	void Moving(int x, int y, int btn);
	void Key(UINT key);
	INTPTR Send(int ctrl, MENUID id, INTPTR arg);
	void SetStyle(UINT16 style);

public:
	MenuSysWnd* m_root;
	UINT16		m_icon;
	UINT16		m_style;
	void		(*m_cmd)(MENUID id);
	int			m_opened;
	int			m_popupx;
	int			m_popupy;
	OEMCHAR		m_title[128];

private:
	MenuSysItem* GetItem(int depth, int pos);
	BRESULT OpenChild(int depth, int pos);
	int OpenPopup();
	void DrawItem(int depth, int pos, int flag);
	void FocusMove(int depth, int dir);
	void FocusEnter(int depth, bool exec);
	void SetFlag(MENUID id, MENUFLG flag, MENUFLG mask);
	void SetText(MENUID id, const OEMCHAR *arg);
};
typedef MenuSys MENUSYS;

static MenuSys s_menusys;

#if defined(OSLANG_SJIS) && !defined(RESOURCE_US)
static const OEMCHAR str_sysr[] = 			// ���̃T�C�Y�ɖ߂�
			"\214\263\202\314\203\124\203\103\203\131\202\311" \
			"\226\337\202\267";
static const OEMCHAR str_sysm[] =			// �ړ�
			"\210\332\223\256";
static const OEMCHAR str_syss[] =			// �T�C�Y�ύX
			"\203\124\203\103\203\131\225\317\215\130";
static const OEMCHAR str_sysn[] =			// �ŏ���
			"\215\305\217\254\211\273";
static const OEMCHAR str_sysx[] =			// �ő剻
			"\215\305\221\345\211\273";
static const OEMCHAR str_sysc[] =			// ����
			"\225\302\202\266\202\351";
#elif defined(OSLANG_EUC) && !defined(RESOURCE_US)
static const OEMCHAR str_sysr[] = 			// ���̃T�C�Y�ɖ߂�
			"\270\265\244\316\245\265\245\244\245\272\244\313" \
			"\314\341\244\271";
static const OEMCHAR str_sysm[] =			// �ړ�
			"\260\334\306\260";
static const OEMCHAR str_syss[] =			// �T�C�Y�ύX
			"\245\265\245\244\245\272\312\321\271\271";
static const OEMCHAR str_sysn[] =			// �ŏ���
			"\272\307\276\256\262\275";
static const OEMCHAR str_sysx[] =			// �ő剻
			"\272\307\302\347\262\275";
static const OEMCHAR str_sysc[] =			// ����
			"\312\304\244\270\244\353";
#elif defined(OSLANG_UTF8) && !defined(RESOURCE_US)
static const OEMCHAR str_sysr[] = 			// ���̃T�C�Y�ɖ߂�
			"\345\205\203\343\201\256\343\202\265\343\202\244\343\202\272" \
			"\343\201\253\346\210\273\343\201\231";
static const OEMCHAR str_sysm[] =			// �ړ�
			"\347\247\273\345\213\225";
static const OEMCHAR str_syss[] =			// �T�C�Y�ύX
			"\343\202\265\343\202\244\343\202\272\345\244\211\346\233\264";
static const OEMCHAR str_sysn[] =			// �ŏ���
			"\346\234\200\345\260\217\345\214\226";
static const OEMCHAR str_sysx[] =			// �ő剻
			"\346\234\200\345\244\247\345\214\226";
static const OEMCHAR str_sysc[] =			// ����
			"\351\226\211\343\201\230\343\202\213";
#else
static const OEMCHAR str_sysr[] = OEMTEXT("Restore");
static const OEMCHAR str_sysm[] = OEMTEXT("Move");
static const OEMCHAR str_syss[] = OEMTEXT("Size");
static const OEMCHAR str_sysn[] = OEMTEXT("Minimize");
static const OEMCHAR str_sysx[] = OEMTEXT("Maximize");
static const OEMCHAR str_sysc[] = OEMTEXT("Close");
#endif


static const MSYSITEM s_exit[] =
{
		{str_sysr,			NULL,		0,				MENU_GRAY},
		{str_sysm,			NULL,		0,				MENU_GRAY},
		{str_syss,			NULL,		0,				MENU_GRAY},
#if defined(MENU_TASKMINIMIZE)
		{str_sysn,			NULL,		SID_MINIMIZE,	0},
#else
		{str_sysn,			NULL,		0,				MENU_GRAY},
#endif
		{str_sysx,			NULL,		0,				MENU_GRAY},
		{NULL,				NULL,		0,				MENU_SEPARATOR},
		{str_sysc,			NULL,		SID_CLOSE,		MENU_DELETED}};

static const MSYSITEM s_root[] =
{
		{NULL,				s_exit,		0,				MENUS_SYSTEM},
#if defined(MENU_TASKMINIMIZE)
		{NULL,				NULL,		SID_MINIMIZE,	MENUS_MINIMIZE},
#endif
		{NULL,				NULL,		SID_CLOSE,		MENUS_CLOSE |
														MENU_DELETED}};

// ----

static void draw(VRAMHDL dst, const RECT_T *rect, void *arg)
{
	MenuSys* sys = static_cast<MenuSys*>(arg);
	for (MenuSys::iterator it = sys->begin(); it != sys->end(); ++it)
	{
		vrammix_cpy2(dst, rect, (*it)->m_vram, NULL, 2);
	}
}


MenuSysItem* MenuSys::GetItem(int depth, int pos)
{
	if ((unsigned int)depth >= size())
	{
		return NULL;
	}
	MenuSysWnd* wnd = at(depth);
	if (wnd)
	{
		if ((pos >= 0) && (pos < static_cast<int>(wnd->size())))
		{
			MenuSysItem* ret = &wnd->at(pos);
			if (!(ret->flag & (MENU_DISABLE | MENU_SEPARATOR)))
			{
				return ret;
			}
		}
	}
	return NULL;
}

static void wndclose(MenuSys* sys, int depth)
{
	while (sys->size() > (unsigned int)depth)
	{
		MenuSysWnd* wnd = sys->back();
		sys->pop_back();
		menubase_clrrect(wnd->m_vram);
		vram_destroy(wnd->m_vram);
		wnd->m_vram = NULL;
	}
}

static void bitemdraw(VRAMHDL vram, MENUHDL menu, int flag)
{
	FONTMNGH font = g_menubase.font;
	int menutype = menu->flag & MENUS_CTRLMASK;
	if (menutype == 0)
	{
		vram_filldat(vram, &menu->rct, menucolor[MVC_STATIC]);
		int pos = 0;
		if (flag)
		{
			pos = 1;
		}
		UINT32 color;
		if (!(menu->flag & MENU_GRAY))
		{
			color = menucolor[MVC_TEXT];
		}
		else
		{
#if 0
			if (flag == 2)
			{
				flag = 0;
				pos = 0;
			}
#endif
			POINT_T pt;
			pt.x = menu->rct.left + pos + MENUSYS_SXSYS + MENU_DSTEXT;
			pt.y = menu->rct.top + pos + MENUSYS_SYSYS + MENU_DSTEXT;
			vrammix_text(vram, font, menu->string, menucolor[MVC_GRAYTEXT2], &pt, NULL);
			color = menucolor[MVC_GRAYTEXT1];
		}

		POINT_T pt;
		pt.x = menu->rct.left + pos + MENUSYS_SXSYS;
		pt.y = menu->rct.top + pos + MENUSYS_SYSYS;
		vrammix_text(vram, font, menu->string, color, &pt, NULL);
		if (flag)
		{
			menuvram_box(vram, &menu->rct, MVC2(MVC_SHADOW, MVC_HILIGHT), (flag==2));
		}
	}
}

enum
{
	MEXIST_SYS		= 0x01,
	MEXIST_MINIMIZE	= 0x02,
	MEXIST_CLOSE	= 0x04,
	MEXIST_ITEM		= 0x08
};

static BRESULT wndopenbase(MENUSYS *sys)
{
	wndclose(sys, 0);

	UINT rootflg = 0;
	MenuSysWnd* wnd = sys->m_root;

	// ���j���[���e�𒲂ׂ�B
	for (MenuSysWnd::const_iterator it = wnd->begin(); it != wnd->end(); ++it)
	{
		if (!(it->flag & (MENU_DISABLE | MENU_SEPARATOR)))
		{
			switch (it->flag & MENUS_CTRLMASK)
			{
				case MENUS_POPUP:
					break;

				case MENUS_SYSTEM:
					rootflg |= MEXIST_SYS;
					break;

				case MENUS_MINIMIZE:
					rootflg |= MEXIST_MINIMIZE;
					break;

				case MENUS_CLOSE:
					rootflg |= MEXIST_CLOSE;
					break;

				default:
					rootflg |= MEXIST_ITEM;
					break;
			}
		}
	}

	RECT_T mrect;
	mrect.left = MENU_FBORDER + MENU_BORDER;
	mrect.top = MENU_FBORDER + MENU_BORDER;
	mrect.right = g_menubase.width - (MENU_FBORDER + MENU_BORDER);
	mrect.bottom = (MENU_FBORDER + MENU_BORDER) + MENUSYS_CYCAPTION;
	int height = ((MENU_FBORDER + MENU_BORDER) * 2) + MENUSYS_CYCAPTION;
	if (rootflg & MEXIST_ITEM)
	{
		height += (MENUSYS_BCAPTION * 3) + MENUSYS_CYSYS;
		mrect.left += MENUSYS_BCAPTION;
		mrect.top += MENUSYS_BCAPTION;
		mrect.right -= MENUSYS_BCAPTION;
		mrect.bottom += MENUSYS_BCAPTION;
	}
	VRAMHDL vram = menuvram_create(g_menubase.width, height, g_menubase.bpp);
	wnd->m_vram = vram;
	if (vram == NULL)
	{
		return FAILURE;
	}
	if (sys->m_style & MENUSTYLE_BOTTOM)
	{
		vram->posy = max(0, menuvram->height - height);
	}
	menuvram_caption(vram, &mrect, sys->m_icon, sys->m_title);
	menubase_setrect(vram, NULL);
	wnd->m_focus = -1;
	sys->push_back(wnd);
	int posx = MENU_FBORDER + MENU_BORDER + MENUSYS_BCAPTION;

	for (MenuSysWnd::iterator it = wnd->begin(); it != wnd->end(); ++it)
	{
		MenuSysItem* menu = &*it;
		if (!(menu->flag & (MENU_DISABLE | MENU_SEPARATOR)))
		{
			int menutype = menu->flag & MENUS_CTRLMASK;
			if (menutype == MENUS_POPUP)
			{
			}
			else if (menutype == MENUS_SYSTEM)
			{
				menu->rct.left = mrect.left + MENU_PXCAPTION;
				menu->rct.right = menu->rct.left;
				menu->rct.top = mrect.top + MENU_PYCAPTION;
				menu->rct.bottom = menu->rct.top;
				if (sys->m_icon)
				{
					menu->rct.right += MENUSYS_SZICON;
					menu->rct.bottom += MENUSYS_SZICON;
				}
			}
			else if (menutype == MENUS_MINIMIZE)
			{
				menu->rct.right = mrect.right - MENU_PXCAPTION;
				if (rootflg & MEXIST_CLOSE)
				{
					menu->rct.right -= MENUSYS_CXCLOSE + (MENU_LINE * 2);
				}
				menu->rct.left = menu->rct.right - MENUSYS_CXCLOSE;
				menu->rct.top = mrect.top +
								((MENUSYS_CYCAPTION - MENUSYS_CYCLOSE) / 2);
				menu->rct.bottom = menu->rct.top + MENUSYS_CYCLOSE;
				menuvram_minimizebtn(vram, &menu->rct, 0);
			}
			else if (menutype == MENUS_CLOSE)
			{
				menu->rct.right = mrect.right - MENU_PXCAPTION;
				menu->rct.left = menu->rct.right - MENUSYS_CXCLOSE;
				menu->rct.top = mrect.top +
								((MENUSYS_CYCAPTION - MENUSYS_CYCLOSE) / 2);
				menu->rct.bottom = menu->rct.top + MENUSYS_CYCLOSE;
				menuvram_closebtn(vram, &menu->rct, 0);
			}
			else
			{
				menu->rct.left = posx;
				menu->rct.top = mrect.bottom + MENUSYS_BCAPTION;
				menu->rct.bottom = menu->rct.top + MENUSYS_CYSYS;

				POINT_T pt;
				fontmng_getsize(g_menubase.font, menu->string, &pt);
				posx += MENUSYS_SXSYS + pt.x + MENUSYS_LXSYS;
				if (posx >= (g_menubase.width - (MENU_FBORDER + MENU_BORDER + MENUSYS_BCAPTION)))
				{
					break;
				}
				menu->rct.right = posx;
				bitemdraw(vram, menu, 0);
			}
		}
	}
	return SUCCESS;
}


// ----

static void citemdraw2(VRAMHDL vram, MENUHDL menu, UINT mvc, int pos)
{
	const MENURES2* res = menures_sys;

	if (menu->flag & MENU_CHECKED)
	{
		POINT_T pt;
		pt.x = menu->rct.left + MENUSYS_SXITEM + pos,
		pt.y = menu->rct.top + pos;
		menuvram_res3put(vram, res, &pt, mvc);
	}
	if (menu->child)
	{
		POINT_T pt;
		pt.x = menu->rct.right - MENUSYS_SXITEM - res[1].width + pos,
		pt.y = menu->rct.top + pos;
		menuvram_res3put(vram, res+1, &pt, mvc);
	}
}

static void citemdraw(VRAMHDL vram, MENUHDL menu, int flag)
{
	vram_filldat(vram, &menu->rct, (flag != 0)?0x000080:0xc0c0c0);

	if (menu->flag & MENU_SEPARATOR)
	{
		int left = menu->rct.left + MENUSYS_SXSEP;
		int right = menu->rct.right - MENUSYS_LXSEP;
		int top = menu->rct.top + MENUSYS_SYSEP;
		menuvram_linex(vram, left, top, right, MVC_SHADOW);
		menuvram_linex(vram, left, top + MENU_LINE, right, MVC_HILIGHT);
	}
	else
	{
		int left = menu->rct.left + MENUSYS_SXITEM + MENUSYS_CXCHECK;
		int top = menu->rct.top + MENUSYS_SYITEM;
		FONTMNGH font = g_menubase.font;
		UINT32 txtcol;
		if (!(menu->flag & MENU_GRAY))
		{
			txtcol = (flag != 0)?MVC_CURTEXT:MVC_TEXT;
		}
		else
		{
			if (flag == 0)
			{
				POINT_T pt;
				pt.x = left + MENU_DSTEXT;
				pt.y = top + MENU_DSTEXT;
				vrammix_text(vram, font, menu->string, menucolor[MVC_GRAYTEXT2], &pt, NULL);
				citemdraw2(vram, menu, MVC_GRAYTEXT2, 1);
			}
			txtcol = MVC_GRAYTEXT1;
		}
		POINT_T pt;
		pt.x = left;
		pt.y = top;
		vrammix_text(vram, font, menu->string, menucolor[txtcol], &pt, NULL);
		citemdraw2(vram, menu, txtcol, 0);
	}
}

BRESULT MenuSys::OpenChild(int depth, int pos)
{
	MenuSysItem* item = GetItem(depth, pos);
	if ((item == NULL) || (item->child == NULL))
	{
		TRACEOUT(("child not found."));
		goto copn_err;
	}
	MenuSysWnd* wnd = at(depth);

	RECT_T parent;
	int dir;
	if ((item->flag & MENUS_CTRLMASK) == MENUS_POPUP)
	{
		parent.left = m_popupx;
		parent.top = max(m_popupy, wnd->m_vram->height);
		parent.right = parent.left;
		parent.bottom = parent.top;
		dir = 0;
	}
	else
	{
		parent.left = wnd->m_vram->posx + item->rct.left;
		parent.top = wnd->m_vram->posy + item->rct.top;
		parent.right = wnd->m_vram->posx + item->rct.right;
		parent.bottom = wnd->m_vram->posy + item->rct.bottom;
		dir = depth + 1;
	}
	if (depth >= (MENUSYS_MAX - 1))
	{
		TRACEOUT(("menu max."));
		goto copn_err;
	}
	int width = 0;
	int height = (MENU_FBORDER + MENU_BORDER);

	wnd = item->child;
	for (MenuSysWnd::iterator it = wnd->begin(); it != wnd->end(); ++it)
	{
		MenuSysItem* menu = &*it;
		if (!(menu->flag & MENU_DISABLE))
		{
			menu->rct.left = (MENU_FBORDER + MENU_BORDER);
			menu->rct.top = height;
			if (menu->flag & MENU_SEPARATOR)
			{
				if (height > (g_menubase.height - MENUSYS_CYSEP - (MENU_FBORDER + MENU_BORDER)))
				{
					break;
				}
				height += MENUSYS_CYSEP;
				menu->rct.bottom = height;
			}
			else
			{
				if (height > (g_menubase.height - MENUSYS_CYITEM - (MENU_FBORDER + MENU_BORDER)))
				{
					break;
				}
				height += MENUSYS_CYITEM;
				menu->rct.bottom = height;
				POINT_T pt;
				fontmng_getsize(g_menubase.font, menu->string, &pt);
				if (width < pt.x)
				{
					width = pt.x;
				}
			}
		}
	}
	width += ((MENU_FBORDER + MENU_BORDER + MENUSYS_SXITEM) * 2) + MENUSYS_CXCHECK + MENUSYS_CXNEXT;
	if (width >= g_menubase.width)
	{
		width = g_menubase.width;
	}
	height += (MENU_FBORDER + MENU_BORDER);
	wnd->m_vram = menuvram_create(width, height, g_menubase.bpp);
	if (wnd->m_vram == NULL)
	{
		TRACEOUT(("sub menu vram couldn't create"));
		goto copn_err;
	}
	if (dir == 1)
	{
		if ((parent.top < height) || (parent.bottom < (g_menubase.height - height)))
		{
			parent.top = parent.bottom;
		}
		else
		{
			parent.top -= height;
		}
	}
	else if (dir >= 2)
	{
		if ((parent.left < width) || (parent.right < (g_menubase.width - width)))
		{
			parent.left = parent.right;
		}
		else
		{
			parent.left -= width;
		}
		if ((parent.top > (g_menubase.height - height)) && (parent.bottom >= height))
		{
			parent.top = parent.bottom - height;
		}
	}
	wnd->m_vram->posx = min(parent.left, g_menubase.width - width);
	wnd->m_vram->posy = min(parent.top, g_menubase.height - height);
	wnd->m_focus = -1;
	push_back(wnd);

	for (MenuSysWnd::iterator it = wnd->begin(); it != wnd->end(); ++it)
	{
		MenuSysItem* menu = &*it;
		if (!(menu->flag & MENU_DISABLE))
		{
			menu->rct.right = width - (MENU_FBORDER + MENU_BORDER);
			citemdraw(wnd->m_vram, menu, 0);
		}
	}
	menubase_setrect(wnd->m_vram, NULL);
	return SUCCESS;

copn_err:
	return FAILURE;
}

int MenuSys::OpenPopup()
{
	if (size() == 1)
	{
		int pos = 0;
		for (MenuSysWnd::const_iterator it = m_root->begin(); it != m_root->end(); ++it)
		{
			if (!(it->flag & (MENU_DISABLE | MENU_SEPARATOR)))
			{
				if ((it->flag & MENUS_CTRLMASK) == MENUS_POPUP)
				{
					at(0)->m_focus = pos;
					OpenChild(0, pos);
					return 1;
				}
			}
			pos++;
		}
	}
	return 0;
}

void MenuSys::DrawItem(int depth, int pos, int flag)
{
	MENUHDL menu = GetItem(depth, pos);
	if (menu)
	{
		VRAMHDL vram = at(depth)->m_vram;

		void (*drawfn)(VRAMHDL vram, MENUHDL menu, int flag);
		drawfn = (depth) ? citemdraw : bitemdraw;
		drawfn(vram, menu, flag);
		menubase_setrect(vram, &menu->rct);
	}
}


// ----

struct MENUPOS
{
	int		depth;
	int		pos;
	MenuSysWnd* wnd;
	MENUHDL	menu;
};

static void getposinfo(MENUSYS *sys, MENUPOS *pos, int x, int y)
{
	int cnt = sys->size();
	while (cnt--)
	{
		MenuSysWnd* wnd = sys->at(cnt);
		if (wnd->m_vram)
		{
			RECT_T rct;
			vram_getrect(wnd->m_vram, &rct);
			if (rect_in(&rct, x, y))
			{
				x -= wnd->m_vram->posx;
				y -= wnd->m_vram->posy;
				break;
			}
		}
	}
	if (cnt >= 0)
	{
		MenuSysWnd* w = sys->at(cnt);
		pos->depth = cnt;
		pos->wnd = w;
		cnt = 0;
		for (MenuSysWnd::iterator it = w->begin(); it != w->end(); ++it)
		{
			if (!(it->flag & (MENU_DISABLE | MENU_SEPARATOR)))
			{
				if (rect_in(&it->rct, x, y))
				{
					pos->pos = cnt;
					pos->menu = &*it;
					return;
				}
			}
			cnt++;
		}
	}
	else
	{
		pos->depth = -1;
		pos->wnd = NULL;
	}
	pos->pos = -1;
	pos->menu = NULL;
}


// ----

/**
 * �R���X�g���N�^
 */
MenuSys::MenuSys()
	: m_root(NULL)
	, m_icon(0)
	, m_style(0)
	, m_cmd(NULL)
	, m_opened(0)
	, m_popupx(0)
	, m_popupy(0)
{
	memset(m_title, 0, sizeof(m_title));
}

static void defcmd(MENUID id)
{
	(void)id;
}

/**
 * �쐬
 */
BRESULT MenuSys::Create(const MSYSITEM *item, void (*cmd)(MENUID id), UINT16 icon, const OEMCHAR *title)
{
	if (cmd == NULL)
	{
		cmd = defcmd;
	}

	m_root = NULL;
	m_icon = icon;
	m_style = 0;
	m_cmd = cmd;
	m_opened = 0;
	m_popupx = 0;
	m_popupy = 0;
	memset(m_title, 0, sizeof(m_title));

	if (title)
	{
		milstr_ncpy(m_title, title, NELEMENTS(m_title));
	}

	m_root = new MenuSysWnd();
	m_root->Append(s_root);
	m_root->Append(item);
	return SUCCESS;
}

void MenuSys::Destroy()
{
	wndclose(this, 0);

	delete m_root;
	m_root = NULL;
}

BRESULT MenuSys::Open(int x, int y)
{
	if (menubase_open(1) != SUCCESS)
	{
		goto msopn_err;
	}
	m_opened = 0;
	if (wndopenbase(this) != SUCCESS)
	{
		goto msopn_err;
	}
	m_popupx = x;
	m_popupy = y;
	m_opened = OpenPopup();
	menubase_draw(draw, this);
	return SUCCESS;

msopn_err:
	menubase_close();
	return FAILURE;
}

void MenuSys::Close()
{
	wndclose(this, 0);
}

void MenuSys::Moving(int x, int y, int btn)
{
	MENUPOS cur;
	getposinfo(this, &cur, x, y);

	/* ���j���[�����` */
	if (cur.depth < 0)
	{
		if (btn == 2)
		{
			menubase_close();
			return;
		}
	}
	int topwnd = size() - 1;
	if (cur.menu != NULL)
	{
		if (cur.wnd->m_focus != cur.pos)
		{
			if (m_opened)
			{
				if (cur.depth != topwnd)
				{
					wndclose(this, cur.depth + 1);
				}
				if ((!(cur.menu->flag & MENU_GRAY)) && (cur.menu->child != NULL))
				{
					OpenChild(cur.depth, cur.pos);
				}
			}
			DrawItem(cur.depth, cur.wnd->m_focus, 0);
			DrawItem(cur.depth, cur.pos, 2 - m_opened);
			cur.wnd->m_focus = cur.pos;
		}
		if (!(cur.menu->flag & MENU_GRAY))
		{
			if (btn == 1)
			{
				if ((!m_opened) && (cur.depth == 0) && (cur.menu->child != NULL))
				{
					wndclose(this, 1);
					DrawItem(0, cur.pos, 1);
					OpenChild(0, cur.pos);
					m_opened = 1;
				}
			}
			else if (btn == 2)
			{
				if ((cur.menu->id) && (!(cur.menu->flag & MENU_NOSEND)))
				{
					menubase_close();
					(*m_cmd)(cur.menu->id);
					return;
				}
			}
		}
	}
	else
	{
		if ((btn == 1) && (cur.depth == 0))
		{
			wndclose(this, 1);
			DrawItem(0, cur.wnd->m_focus, 0);
			m_opened = OpenPopup();
		}
		else if (cur.depth != topwnd)
		{
			cur.depth = topwnd;
			cur.pos = -1;
			cur.wnd = at(cur.depth);
			if (cur.wnd->m_focus != cur.pos)
			{
				DrawItem(cur.depth, cur.wnd->m_focus, 0);
				cur.wnd->m_focus = cur.pos;
			}
		}
	}
	menubase_draw(draw, this);
}

void MenuSys::FocusMove(int depth, int dir)
{
	MenuSysWnd* wnd = at(depth);
	MENUHDL target = NULL;
	int tarpos = 0;
	int pos = 0;
	for (MenuSysWnd::iterator it = wnd->begin(); it != wnd->end(); ++it)
	{
		MenuSysItem* menu = &*it;
		if (pos == wnd->m_focus)
		{
			if ((dir < 0) && (target != NULL))
			{
				break;
			}
		}
		else if (((menu->flag & MENUS_CTRLMASK) <= MENUS_SYSTEM) &&
				(!(menu->flag & (MENU_DISABLE | MENU_GRAY | MENU_SEPARATOR))))
		{
			if (dir < 0)
			{
				target = menu;
				tarpos = pos;
			}
			else
			{
				if (pos < wnd->m_focus)
				{
					if (target == NULL)
					{
						target = menu;
						tarpos = pos;
					}
				}
				else
				{
					target = menu;
					tarpos = pos;
					break;
				}
			}
		}
		pos++;
	}
	if (target == NULL)
	{
		return;
	}
	DrawItem(depth, wnd->m_focus, 0);
	DrawItem(depth, tarpos, 2 - m_opened);
	wnd->m_focus = tarpos;
//	TRACEOUT(("focus = %d", tarpos));
	if (depth == 0)
	{
		if (m_opened)
		{
			wndclose(this, 1);
			OpenChild(0, tarpos);
		}
	}
	else
	{
		if (depth != (size() - 1))
		{
			wndclose(this, depth + 1);
		}
	}
}

void MenuSys::FocusEnter(int depth, bool exec)
{
	MenuSysWnd* wnd = at(depth);
	MENUHDL menu = GetItem(depth, wnd->m_focus);
	if ((menu) && (!(menu->flag & MENU_GRAY)) && (menu->child != NULL))
	{
		if (depth == 0)
		{
			wndclose(this, 1);
			DrawItem(0, wnd->m_focus, 1);
			m_opened = 1;
		}
		OpenChild(depth, wnd->m_focus);
	}
	else if (exec)
	{
		if ((menu) && (menu->id))
		{
			menubase_close();
			(*m_cmd)(menu->id);
		}
	}
	else
	{
		FocusMove(0, 1);
	}
}

void MenuSys::Key(UINT key)
{
	MENUSYS* sys = this;

	int topwnd = size() - 1;
	if (topwnd == 0)
	{
		if (key & KEY_LEFT)
		{
			FocusMove(0, -1);
		}
		if (key & KEY_RIGHT)
		{
			FocusMove(0, 1);
		}
		if (key & KEY_DOWN)
		{
			FocusEnter(0, false);
		}
		if (key & KEY_ENTER)
		{
			FocusEnter(0, true);
		}
	}
	else
	{
		if (key & KEY_UP)
		{
			FocusMove(topwnd, -1);
		}
		if (key & KEY_DOWN)
		{
			FocusMove(topwnd, 1);
		}
		if (key & KEY_LEFT)
		{
			if (topwnd >= 2)
			{
				wndclose(sys, topwnd);
			}
			else
			{
				FocusMove(0, -1);
			}
		}
		if (key & KEY_RIGHT)
		{
			FocusEnter(topwnd, false);
		}
		if (key & KEY_ENTER)
		{
			FocusEnter(topwnd, true);
		}
	}
	menubase_draw(draw, this);
}


// ----

void MenuSys::SetFlag(MENUID id, MENUFLG flag, MENUFLG mask)
{
	MENUHDL itm = m_root->GetItem(id);
	if (itm == NULL)
	{
		return;
	}
	flag ^= itm->flag;
	flag &= mask;
	if (!flag)
	{
		return;
	}
	itm->flag ^= flag;

	// ���h���[���K�v�H
	unsigned int depth = 0;
	while (depth < size())
	{
		MenuSysWnd* w = at(depth);
		int pos = 0;
		for (MenuSysWnd::iterator it = w->begin(); it != w->end(); ++it)
		{
			if (it->id == id)
			{
				if (!(it->flag & (MENU_DISABLE | MENU_SEPARATOR)))
				{
					int focus = 0;
					if (w->m_focus == pos)
					{
						focus = 2 - m_opened;
					}
					DrawItem(depth, pos, focus);
					menubase_draw(draw, this);
					return;
				}
			}
			pos++;
		}
		depth++;
	}
}

void MenuSys::SetText(MENUID id, const OEMCHAR *arg)
{
	MENUHDL itm = m_root->GetItem(id);
	if (itm == NULL)
	{
		return;
	}

	if (arg)
	{
		milstr_ncpy(itm->string, arg, NELEMENTS(itm->string));
	}
	else
	{
		itm->string[0] = '\0';
	}

	// ���h���[���K�v�H (ToDo: �ăI�[�v�����ׂ�)
	unsigned int depth = 0;
	while (depth < size())
	{
		MenuSysWnd* w = at(depth);
		int pos = 0;
		for (MenuSysWnd::iterator it = w->begin(); it != w->end(); ++it)
		{
			if (it->id == id)
			{
				if (!(it->flag & (MENU_DISABLE | MENU_SEPARATOR)))
				{
					int focus = 0;
					if (w->m_focus == pos)
					{
						focus = 2 - m_opened;
					}
					DrawItem(depth, pos, focus);
					menubase_draw(draw, this);
					return;
				}
			}
			pos++;
		}
		depth++;
	}
}

INTPTR MenuSys::Send(int ctrl, MENUID id, INTPTR arg)
{
	MENUHDL itm = m_root->GetItem(id);
	if (itm == NULL)
	{
		return 0;
	}

	INTPTR ret = 0;
	switch (ctrl)
	{
		case SMSG_SETHIDE:
			ret = (itm->flag & MENU_DISABLE) ? 1 : 0;
			SetFlag(id, (MENUFLG)((arg) ? MENU_DISABLE : 0), MENU_DISABLE);
			break;

		case SMSG_GETHIDE:
			ret = (itm->flag & MENU_DISABLE) ? 1 : 0;
			break;

		case SMSG_SETENABLE:
			ret = (itm->flag & MENU_GRAY) ? 0 : 1;
			SetFlag(id, (MENUFLG)((arg) ? 0 : MENU_GRAY), MENU_GRAY);
			break;

		case SMSG_GETENABLE:
			ret = (itm->flag & MENU_GRAY) ? 0 : 1;
			break;

		case SMSG_SETCHECK:
			ret = (itm->flag & MENU_CHECKED) ? 1 : 0;
			SetFlag(id, (MENUFLG)((arg) ? MENU_CHECKED : 0), MENU_CHECKED);
			break;

		case SMSG_GETCHECK:
			ret = (itm->flag & MENU_CHECKED) ? 1 : 0;
			break;

		case SMSG_SETTEXT:
			SetText(id, reinterpret_cast<OEMCHAR*>(arg));
			break;
	}
	return ret;
}

/**
 * �X�^�C���̐ݒ�
 */
void MenuSys::SetStyle(UINT16 style)
{
	m_style = style;
}

// ----

BRESULT menusys_create(const MSYSITEM *item, void (*cmd)(MENUID id), UINT16 icon, const OEMCHAR *title)
{
	return s_menusys.Create(item, cmd, icon, title);
}

void menusys_destroy(void)
{
	s_menusys.Destroy();
}

BRESULT menusys_open(int x, int y)
{
	return s_menusys.Open(x, y);
}

void menusys_close(void)
{
	s_menusys.Close();
}

void menusys_moving(int x, int y, int btn)
{
	s_menusys.Moving(x, y, btn);
}

void menusys_key(UINT key)
{
	s_menusys.Key(key);
}

INTPTR menusys_msg(int ctrl, MENUID id, INTPTR arg)
{
	return s_menusys.Send(ctrl, id, arg);
}

void menusys_setstyle(UINT16 style)
{
	s_menusys.SetStyle(style);
}

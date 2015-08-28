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
#include <algorithm>
#include <vector>

class MenuSysWnd;

/**
 * ������R�s�[
 * @param[in] lpString ������
 * @return ������
 */
static const OEMCHAR* StrDup(const OEMCHAR* lpString)
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
 * @brief item
 */
struct MenuSysItem
{
	MenuSysWnd* child;			/*!< �q */
	MENUID		id;				/*!< ID */
	MENUFLG		flag;			/*!< �t���O */
	RECT_T		rct;			/*!< �̈� */
	const OEMCHAR* string;		/*!< ������ */

	MenuSysItem(const MSYSITEM& item)
		: child(NULL)
		, id(item.id)
		, flag(item.flag & (~MENU_DELETED))
		, string(StrDup(item.string))
	{
	}

	void DrawRootItem(VRAMHDL vram, int flag) const;
	void DrawSubItem(VRAMHDL vram, int flag) const;
	void DrawSubItemSub(VRAMHDL vram, UINT mvc, int pos) const;
};

/**
 * ���[�g �A�C�e���̕`��
 * @param[in] vram VRAM
 * @param[in] flag �t�H�[�J�X �t���O
 */
void MenuSysItem::DrawRootItem(VRAMHDL vram, int flag) const
{
	FONTMNGH font = MenuBase::GetInstance()->GetFont();
	int menutype = this->flag & MENUS_CTRLMASK;
	if (menutype == 0)
	{
		vram_filldat(vram, &this->rct, menucolor[MVC_STATIC]);
		int pos = 0;
		if (flag)
		{
			pos = 1;
		}
		UINT32 color;
		if (!(this->flag & MENU_GRAY))
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
			pt.x = this->rct.left + pos + MENUSYS_SXSYS + MENU_DSTEXT;
			pt.y = this->rct.top + pos + MENUSYS_SYSYS + MENU_DSTEXT;
			vrammix_text(vram, font, this->string, menucolor[MVC_GRAYTEXT2], &pt, NULL);
			color = menucolor[MVC_GRAYTEXT1];
		}

		POINT_T pt;
		pt.x = this->rct.left + pos + MENUSYS_SXSYS;
		pt.y = this->rct.top + pos + MENUSYS_SYSYS;
		vrammix_text(vram, font, this->string, color, &pt, NULL);
		if (flag)
		{
			menuvram_box(vram, &this->rct, MVC2(MVC_SHADOW, MVC_HILIGHT), (flag==2));
		}
	}
}

/**
 * �T�u �A�C�e���̕`��
 * @param[in] vram VRAM
 * @param[in] flag �t�H�[�J�X �t���O
 */
void MenuSysItem::DrawSubItem(VRAMHDL vram, int flag) const
{
	vram_filldat(vram, &this->rct, (flag != 0)?0x000080:0xc0c0c0);

	if (this->flag & MENU_SEPARATOR)
	{
		int left = this->rct.left + MENUSYS_SXSEP;
		int right = this->rct.right - MENUSYS_LXSEP;
		int top = this->rct.top + MENUSYS_SYSEP;
		menuvram_linex(vram, left, top, right, MVC_SHADOW);
		menuvram_linex(vram, left, top + MENU_LINE, right, MVC_HILIGHT);
	}
	else
	{
		int left = this->rct.left + MENUSYS_SXITEM + MENUSYS_CXCHECK;
		int top = this->rct.top + MENUSYS_SYITEM;
		FONTMNGH font = MenuBase::GetInstance()->GetFont();
		UINT32 txtcol;
		if (!(this->flag & MENU_GRAY))
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
				vrammix_text(vram, font, this->string, menucolor[MVC_GRAYTEXT2], &pt, NULL);
				DrawSubItemSub(vram, MVC_GRAYTEXT2, 1);
			}
			txtcol = MVC_GRAYTEXT1;
		}
		POINT_T pt;
		pt.x = left;
		pt.y = top;
		vrammix_text(vram, font, this->string, menucolor[txtcol], &pt, NULL);
		DrawSubItemSub(vram, txtcol, 0);
	}
}

void MenuSysItem::DrawSubItemSub(VRAMHDL vram, UINT mvc, int pos) const
{
	const MENURES2* res = menures_sys;

	if (this->flag & MENU_CHECKED)
	{
		POINT_T pt;
		pt.x = this->rct.left + MENUSYS_SXITEM + pos,
		pt.y = this->rct.top + pos;
		menuvram_res3put(vram, res, &pt, mvc);
	}
	if (this->child)
	{
		POINT_T pt;
		pt.x = this->rct.right - MENUSYS_SXITEM - res[1].width + pos,
		pt.y = this->rct.top + pos;
		menuvram_res3put(vram, res+1, &pt, mvc);
	}
}



/**
 * @brief wnd
 */
class MenuSysWnd : public std::vector<MenuSysItem>
{
public:
	MenuSysWnd(MenuBase* pMenuBase, MenuSysWnd* pParent = NULL);
	~MenuSysWnd();
	MenuSysWnd* GetParent();
	void Append(const MSYSITEM* lpItems);
	int GetIndex(const MenuSysItem* pItem) const;
	MenuSysItem* GetAt(int nIndex) const;
	MenuSysItem* GetItem(int id) const;
	MenuSysItem* GetItem(int x, int y) const;
	void Close();
	FONTMNGH GetFont();
	void DrawItem(int nIndex, int flag) const;
	void DrawItem(const MenuSysItem* pItem, int flag) const;

private:
	MenuBase* m_pMenuBase;
	MenuSysWnd* m_pParent;

public:
	VRAMHDL m_vram;
	int m_focus;
};

/**
 * �R���X�g���N�^
 * @param[in] pParent �e�E�B���h�E
 */
MenuSysWnd::MenuSysWnd(MenuBase* pMenuBase, MenuSysWnd* pParent)
	: m_pMenuBase(pMenuBase)
	, m_pParent(pParent)
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
		delete[] it->string;
	}
}

/**
 * �e���擾
 * @return �e
 */
inline MenuSysWnd* MenuSysWnd::GetParent()
{
	return m_pParent;
}

/**
 * �A�C�e���ǉ�
 * @param[in] lpItems �ǉ�����A�C�e��
 */
void MenuSysWnd::Append(const MSYSITEM *lpItems)
{
	if (lpItems == NULL)
	{
		return;
	}

	while (true /*CONSTCOND*/)
	{
		MenuSysItem item(*lpItems);
		if (lpItems->child)
		{
			item.child = new MenuSysWnd(m_pMenuBase, this);
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
 * �C���f�b�N�X�𓾂�
 * @param[in] pItem �A�C�e��
 * @return �C���f�b�N�X
 */
int MenuSysWnd::GetIndex(const MenuSysItem* pItem) const
{
	for (const_iterator it = begin(); it != end(); ++it)
	{
		if (pItem == &*it)
		{
			return std::distance(begin(), it);
		}
	}
	return -1;
}

/**
 * �A�C�e���𓾂�
 * @param[in] nIndex �C���f�b�N�X
 * @return �A�C�e��
 */
MenuSysItem* MenuSysWnd::GetAt(int nIndex) const
{
	if ((nIndex >= 0) && (nIndex < static_cast<int>(size())))
	{
		const MenuSysItem* ret = &at(nIndex);
		if (!(ret->flag & (MENU_DISABLE | MENU_SEPARATOR)))
		{
			return const_cast<MenuSysItem*>(ret);
		}
	}
	return NULL;
}

/**
 * �A�C�e���𓾂�
 * @param[in] id ID
 * @return �A�C�e��
 */
MenuSysItem* MenuSysWnd::GetItem(int id) const
{
	for (MenuSysWnd::const_iterator it = begin(); it != end(); ++it)
	{
		if (it->id == id)
		{
			return const_cast<MenuSysItem*>(&*it);
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

/**
 * �A�C�e���𓾂�
 * @param[in] x X
 * @param[in] y Y
 * @return �A�C�e��
 */
MenuSysItem* MenuSysWnd::GetItem(int x, int y) const
{
	x -= m_vram->posx;
	y -= m_vram->posy;
	for (MenuSysWnd::const_iterator it = begin(); it != end(); ++it)
	{
		if (!(it->flag & (MENU_DISABLE | MENU_SEPARATOR)))
		{
			if (rect_in(&it->rct, x, y))
			{
				return const_cast<MenuSysItem*>(&*it);
			}
		}
	}
	return NULL;
}

/**
 * ����
 */
void MenuSysWnd::Close()
{
	m_pMenuBase->Clear(m_vram);
	vram_destroy(m_vram);
	m_vram = NULL;
}

/**
 * �`��
 */
void MenuSysWnd::DrawItem(int nIndex, int flag) const
{
	MenuSysItem* pItem = GetAt(nIndex);
	if (pItem)
	{
		DrawItem(pItem, flag);
	}
}

/**
 * �`��
 * @param[in] pItem �A�C�e��
 * @param[in] flag �t�H�[�J�X �t���O
 */
void MenuSysWnd::DrawItem(const MenuSysItem* pItem, int flag) const
{
	if (!m_pParent)
	{
		pItem->DrawRootItem(m_vram, flag);
	}
	else
	{
		pItem->DrawSubItem(m_vram, flag);
	}
	m_pMenuBase->Invalidate(m_vram, &pItem->rct);
}

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

enum
{
	MEXIST_SYS		= 0x01,
	MEXIST_MINIMIZE	= 0x02,
	MEXIST_CLOSE	= 0x04,
	MEXIST_ITEM		= 0x08
};

/**
 * �R���X�g���N�^
 */
MenuSys::MenuSys()
	: m_pMenuBase(MenuBase::GetInstance())
	, m_root(NULL)
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
bool MenuSys::Initialize(const MSYSITEM *item, void (*cmd)(MENUID id), UINT16 icon, const OEMCHAR *title)
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

	m_root = new MenuSysWnd(m_pMenuBase);
	m_root->Append(s_root);
	m_root->Append(item);
	return true;
}

void MenuSys::Deinitialize()
{
	m_pMenuBase->Close();

	delete m_root;
	m_root = NULL;
}

/**
 * �I�[�v������
 */
bool MenuSys::Open(int x, int y)
{
	if (!m_pMenuBase->Open(this))
	{
		goto msopn_err;
	}
	m_opened = 0;
	if (!OpenRootWnd())
	{
		goto msopn_err;
	}
	m_popupx = x;
	m_popupy = y;
	m_opened = OpenPopup();
	m_pMenuBase->Draw(Draw, this);
	return true;

msopn_err:
	m_pMenuBase->Close();
	return false;
}

/**
 * ����
 */
void MenuSys::OnClose()
{
	for (std::vector<MenuSysWnd*>::iterator it = m_wnd.begin(); it != m_wnd.end(); ++it)
	{
		(*it)->Close();
	}
	m_wnd.clear();
}

/**
 * ���[�g �E�B���h�E���J��
 * @retval true ����
 * @retval false ���s
 */
bool MenuSys::OpenRootWnd()
{
	OnClose();

	MenuSysWnd* wnd = m_root;

	// ���j���[���e�𒲂ׂ�B
	UINT rootflg = 0;
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
	mrect.right = m_pMenuBase->Width() - (MENU_FBORDER + MENU_BORDER);
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
	VRAMHDL vram = menuvram_create(m_pMenuBase->Width(), height, m_pMenuBase->Bpp());
	wnd->m_vram = vram;
	if (vram == NULL)
	{
		return false;
	}
	if (m_style & MENUSTYLE_BOTTOM)
	{
		vram->posy = max(0, menuvram->height - height);
	}
	menuvram_caption(vram, &mrect, m_icon, m_title);
	m_pMenuBase->Invalidate(vram, NULL);
	wnd->m_focus = -1;
	m_wnd.push_back(wnd);
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
				if (m_icon)
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
				fontmng_getsize(m_pMenuBase->GetFont(), menu->string, &pt);
				posx += MENUSYS_SXSYS + pt.x + MENUSYS_LXSYS;
				if (posx >= (m_pMenuBase->Width() - (MENU_FBORDER + MENU_BORDER + MENUSYS_BCAPTION)))
				{
					break;
				}
				menu->rct.right = posx;
				menu->DrawRootItem(vram, 0);
			}
		}
	}
	return true;
}

/**
 * �q�E�B���h�E���J��
 * @param[in] wnd �E�B���h�E
 * @retval true ����
 * @retval false ���s
 */
bool MenuSys::OpenChild(const MenuSysWnd* wnd, int pos)
{
	MenuSysItem* item = wnd->GetAt(pos);
	if ((item == NULL) || (item->child == NULL))
	{
		TRACEOUT(("child not found."));
		return false;
	}

	int width = 0;
	int height = (MENU_FBORDER + MENU_BORDER);

	MenuSysWnd* childWnd = item->child;
	for (MenuSysWnd::iterator it = childWnd->begin(); it != childWnd->end(); ++it)
	{
		MenuSysItem* menu = &*it;
		if (!(menu->flag & MENU_DISABLE))
		{
			menu->rct.left = (MENU_FBORDER + MENU_BORDER);
			menu->rct.top = height;
			if (menu->flag & MENU_SEPARATOR)
			{
				if (height > (m_pMenuBase->Height() - MENUSYS_CYSEP - (MENU_FBORDER + MENU_BORDER)))
				{
					break;
				}
				height += MENUSYS_CYSEP;
				menu->rct.bottom = height;
			}
			else
			{
				if (height > (m_pMenuBase->Height() - MENUSYS_CYITEM - (MENU_FBORDER + MENU_BORDER)))
				{
					break;
				}
				height += MENUSYS_CYITEM;
				menu->rct.bottom = height;
				POINT_T pt;
				fontmng_getsize(m_pMenuBase->GetFont(), menu->string, &pt);
				if (width < pt.x)
				{
					width = pt.x;
				}
			}
		}
	}
	width += ((MENU_FBORDER + MENU_BORDER + MENUSYS_SXITEM) * 2) + MENUSYS_CXCHECK + MENUSYS_CXNEXT;
	if (width >= m_pMenuBase->Width())
	{
		width = m_pMenuBase->Width();
	}
	height += (MENU_FBORDER + MENU_BORDER);
	childWnd->m_vram = menuvram_create(width, height, m_pMenuBase->Bpp());
	if (childWnd->m_vram == NULL)
	{
		TRACEOUT(("sub menu vram couldn't create"));
		return false;
	}

	RECT_T parent;
	if ((item->flag & MENUS_CTRLMASK) == MENUS_POPUP)
	{
		parent.left = m_popupx;
		parent.top = max(m_popupy, wnd->m_vram->height);
		parent.right = parent.left;
		parent.bottom = parent.top;
	}
	else
	{
		parent.left = wnd->m_vram->posx + item->rct.left;
		parent.top = wnd->m_vram->posy + item->rct.top;
		parent.right = wnd->m_vram->posx + item->rct.right;
		parent.bottom = wnd->m_vram->posy + item->rct.bottom;

		if (wnd == m_root)
		{
			if ((parent.top < height) || (parent.bottom < (m_pMenuBase->Height() - height)))
			{
				parent.top = parent.bottom;
			}
			else
			{
				parent.top -= height;
			}
		}
		else
		{
			if ((parent.left < width) || (parent.right < (m_pMenuBase->Width() - width)))
			{
				parent.left = parent.right;
			}
			else
			{
				parent.left -= width;
			}
			if ((parent.top > (m_pMenuBase->Height() - height)) && (parent.bottom >= height))
			{
				parent.top = parent.bottom - height;
			}
		}
	}
	childWnd->m_vram->posx = min(parent.left, m_pMenuBase->Width() - width);
	childWnd->m_vram->posy = min(parent.top, m_pMenuBase->Height() - height);
	childWnd->m_focus = -1;
	m_wnd.push_back(childWnd);

	for (MenuSysWnd::iterator it = childWnd->begin(); it != childWnd->end(); ++it)
	{
		MenuSysItem* menu = &*it;
		if (!(menu->flag & MENU_DISABLE))
		{
			menu->rct.right = width - (MENU_FBORDER + MENU_BORDER);
			menu->DrawSubItem(childWnd->m_vram, 0);
		}
	}
	m_pMenuBase->Invalidate(childWnd->m_vram, NULL);
	return true;
}

/**
 * �|�b�v�A�b�v���J��
 */
int MenuSys::OpenPopup()
{
	if (m_wnd.size() == 1)
	{
		int pos = 0;
		for (MenuSysWnd::const_iterator it = m_root->begin(); it != m_root->end(); ++it)
		{
			if (!(it->flag & (MENU_DISABLE | MENU_SEPARATOR)))
			{
				if ((it->flag & MENUS_CTRLMASK) == MENUS_POPUP)
				{
					MenuSysWnd* wnd = m_wnd.back();
					wnd->m_focus = pos;
					OpenChild(wnd, pos);
					return 1;
				}
			}
			pos++;
		}
	}
	return 0;
}

/**
 * �q�����
 * @param[in] wnd �Ώۂ̃E�B���h�E
 */
void MenuSys::CloseChild(const MenuSysWnd* wnd)
{
	std::vector<MenuSysWnd*>::iterator it = m_wnd.begin();
	while (it != m_wnd.end())
	{
		MenuSysWnd* p = *it;
		++it;
		if (p == wnd)
		{
			break;
		}
	}

	// �q�����
	while (it != m_wnd.end())
	{
		(*it)->Close();
		it = m_wnd.erase(it);
	}
}

/**
 * 
 */
MenuSysWnd* MenuSys::GetWnd(int x, int y) const
{
	for (std::vector<MenuSysWnd*>::const_reverse_iterator it = m_wnd.rbegin(); it != m_wnd.rend(); ++it)
	{
		const MenuSysWnd* wnd = *it;
		if (wnd->m_vram)
		{
			RECT_T rct;
			vram_getrect(wnd->m_vram, &rct);
			if (rect_in(&rct, x, y))
			{
				return const_cast<MenuSysWnd*>(wnd);
			}
		}
	}
	return NULL;
}

void MenuSys::OnMoving(int x, int y, int btn)
{
	MenuSysWnd* wnd = GetWnd(x, y);
	if (wnd == NULL)
	{
		/* ���j���[�����` */
		if (btn == 2)
		{
			m_pMenuBase->Close();
			return;
		}
	}

	MenuSysItem* item = NULL;
	if (wnd)
	{
		item = wnd->GetItem(x, y);
	}
	if (item != NULL)
	{
		const int pos = wnd->GetIndex(item);
		if (wnd->m_focus != pos)
		{
			if (m_opened)
			{
				CloseChild(wnd);
				if ((!(item->flag & MENU_GRAY)) && (item->child != NULL))
				{
					OpenChild(wnd, pos);
				}
			}
			wnd->DrawItem(wnd->m_focus, 0);
			wnd->DrawItem(item, 2 - m_opened);
			wnd->m_focus = pos;
		}
		if (!(item->flag & MENU_GRAY))
		{
			if (btn == 1)
			{
				if ((!m_opened) && (wnd == m_root) && (item->child != NULL))
				{
					CloseChild(wnd);
					wnd->DrawItem(item, 1);
					OpenChild(wnd, pos);
					m_opened = 1;
				}
			}
			else if (btn == 2)
			{
				if ((item->id) && (!(item->flag & MENU_NOSEND)))
				{
					m_pMenuBase->Close();
					(*m_cmd)(item->id);
					return;
				}
			}
		}
	}
	else
	{
		if ((btn == 1) && (wnd == m_root))
		{
			CloseChild(wnd);
			wnd->DrawItem(wnd->m_focus, 0);
			m_opened = OpenPopup();
		}
		else if (wnd != m_wnd.back())
		{
			MenuSysWnd* wnd = m_wnd.back();
			if (wnd->m_focus != -1)
			{
				wnd->DrawItem(wnd->m_focus, 0);
				wnd->m_focus = -1;
			}
		}
	}
	m_pMenuBase->Draw(Draw, this);
}

void MenuSys::FocusMove(MenuSysWnd* wnd, int dir)
{
	MenuSysItem* target = NULL;
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
	wnd->DrawItem(wnd->m_focus, 0);
	wnd->DrawItem(tarpos, 2 - m_opened);
	wnd->m_focus = tarpos;
//	TRACEOUT(("focus = %d", tarpos));
	if (wnd == m_root)
	{
		if (m_opened)
		{
			CloseChild(wnd);
			OpenChild(wnd, tarpos);
		}
	}
	else
	{
		CloseChild(wnd);
	}
}

void MenuSys::FocusEnter(MenuSysWnd* wnd, bool exec)
{
	MenuSysItem* item = wnd->GetAt(wnd->m_focus);
	if ((item) && (!(item->flag & MENU_GRAY)) && (item->child != NULL))
	{
		if (wnd == m_root)
		{
			CloseChild(wnd);
			wnd->DrawItem(wnd->m_focus, 1);
			m_opened = 1;
		}
		OpenChild(wnd, wnd->m_focus);
	}
	else if (exec)
	{
		if ((item) && (item->id))
		{
			m_pMenuBase->Close();
			(*m_cmd)(item->id);
		}
	}
	else
	{
		FocusMove(m_root, 1);
	}
}

/**
 * �L�[ �C�x���g
 * @param[in] key �L�[
 */
void MenuSys::OnKeyDown(UINT key)
{
	if (m_wnd.empty())
	{
		return;
	}

	MenuSysWnd* wnd = m_wnd.back();
	if (wnd == m_root)
	{
		if (key & KEY_LEFT)
		{
			FocusMove(wnd, -1);
		}
		if (key & KEY_RIGHT)
		{
			FocusMove(wnd, 1);
		}
		if (key & KEY_DOWN)
		{
			FocusEnter(wnd, false);
		}
		if (key & KEY_ENTER)
		{
			FocusEnter(wnd, true);
		}
	}
	else
	{
		if (key & KEY_UP)
		{
			FocusMove(wnd, -1);
		}
		if (key & KEY_DOWN)
		{
			FocusMove(wnd, 1);
		}
		if (key & KEY_LEFT)
		{
			if (m_wnd.size() > 2)
			{
				CloseChild(wnd->GetParent());
			}
			else
			{
				FocusMove(m_root, -1);
			}
		}
		if (key & KEY_RIGHT)
		{
			FocusEnter(wnd, false);
		}
		if (key & KEY_ENTER)
		{
			FocusEnter(wnd, true);
		}
	}
	m_pMenuBase->Draw(Draw, this);
}


/**
 * �t���O�ύX
 */
void MenuSys::SetFlag(MenuSysItem* item, MENUFLG flag, MENUFLG mask)
{
	flag ^= item->flag;
	flag &= mask;
	if (!flag)
	{
		return;
	}
	item->flag ^= flag;
	RedrawItem(item);
}

/**
 * �e�L�X�g�ύX
 */
void MenuSys::SetText(MenuSysItem* item, const OEMCHAR *arg)
{
	delete[] item->string;
	item->string = StrDup(arg);

	// ���h���[���K�v�H (ToDo: �e�L�X�g�̒������ς��̂� �ăI�[�v�����ׂ�)
	RedrawItem(item);
}

/**
 * �A�C�e���̍ĕ`��
 * @param[in] item �ĕ`��A�C�e��
 */
void MenuSys::RedrawItem(const MenuSysItem* item)
{
	for (std::vector<MenuSysWnd*>::iterator it = m_wnd.begin(); it != m_wnd.end(); ++it)
	{
		MenuSysWnd* wnd = *it;
		const int pos = wnd->GetIndex(item);
		if (pos >= 0)
		{
			if (!(item->flag & (MENU_DISABLE | MENU_SEPARATOR)))
			{
				int focus = 0;
				if (wnd->m_focus == pos)
				{
					focus = 2 - m_opened;
				}
				wnd->DrawItem(item, focus);
				m_pMenuBase->Draw(Draw, this);
				return;
			}
		}
	}
}

/**
 * ���b�Z�[�W
 * @param[in] ctrl �R���g���[�� �R�[�h
 * @param[in] id ID
 * @param[in] arg �p�����^
 * @return ���U���g �R�[�h
 */
INTPTR MenuSys::Send(int ctrl, MENUID id, INTPTR arg)
{
	MenuSysItem* item = m_root->GetItem(id);
	if (item == NULL)
	{
		return 0;
	}

	INTPTR ret = 0;
	switch (ctrl)
	{
		case SMSG_SETHIDE:
			ret = (item->flag & MENU_DISABLE) ? 1 : 0;
			SetFlag(item, (MENUFLG)((arg) ? MENU_DISABLE : 0), MENU_DISABLE);
			break;

		case SMSG_GETHIDE:
			ret = (item->flag & MENU_DISABLE) ? 1 : 0;
			break;

		case SMSG_SETENABLE:
			ret = (item->flag & MENU_GRAY) ? 0 : 1;
			SetFlag(item, (MENUFLG)((arg) ? 0 : MENU_GRAY), MENU_GRAY);
			break;

		case SMSG_GETENABLE:
			ret = (item->flag & MENU_GRAY) ? 0 : 1;
			break;

		case SMSG_SETCHECK:
			ret = (item->flag & MENU_CHECKED) ? 1 : 0;
			SetFlag(item, (MENUFLG)((arg) ? MENU_CHECKED : 0), MENU_CHECKED);
			break;

		case SMSG_GETCHECK:
			ret = (item->flag & MENU_CHECKED) ? 1 : 0;
			break;

		case SMSG_SETTEXT:
			SetText(item, reinterpret_cast<OEMCHAR*>(arg));
			break;
	}
	return ret;
}

/**
 * �X�^�C���̐ݒ�
 * @param[in] style �X�^�C��
 */
void MenuSys::SetStyle(UINT16 style)
{
	m_style = style;
}

/**
 * �`�悷��
 */
void MenuSys::Draw(VRAMHDL dst, const RECT_T *rect, void *arg)
{
	MenuSys* sys = static_cast<MenuSys*>(arg);
	for (std::vector<MenuSysWnd*>::iterator it = sys->m_wnd.begin(); it != sys->m_wnd.end(); ++it)
	{
		vrammix_cpy2(dst, rect, (*it)->m_vram, NULL, 2);
	}
}



// ----

BRESULT menusys_initialize(const MSYSITEM *item, void (*cmd)(MENUID id), UINT16 icon, const OEMCHAR *title)
{
	return s_menusys.Initialize(item, cmd, icon, title) ? SUCCESS : FAILURE;
}

void menusys_deinitialize(void)
{
	s_menusys.Deinitialize();
}

BRESULT menusys_open(int x, int y)
{
	return s_menusys.Open(x, y) ? SUCCESS : FAILURE;
}

INTPTR menusys_msg(int ctrl, MENUID id, INTPTR arg)
{
	return s_menusys.Send(ctrl, id, arg);
}

void menusys_setstyle(UINT16 style)
{
	s_menusys.SetStyle(style);
}

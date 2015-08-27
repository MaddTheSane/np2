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

struct DlgItemParam
{
	DlgItemParam *_next;
	UINT16		width;
	UINT16		num;
	VRAMHDL		icon;
	OEMCHAR		str[96];
};
typedef DlgItemParam *DLGPRM;

#define	PRMNEXT_EMPTY	((DLGPRM)-1)

typedef struct {
	POINT_T		pt;
	FONTMNGH	font;
} DLGTEXT;

typedef struct {
	FONTMNGH	font;
	int			fontsize;
} DLGTAB;

typedef struct {
	FONTMNGH	font;
	SINT16		fontsize;
	SINT16		scrollbar;
	SINT16		dispmax;
	SINT16		basepos;
} DLGLIST;

typedef struct {
	SINT16		minval;
	SINT16		maxval;
	int			pos;
	UINT8		type;
	UINT8		moving;
	UINT8		sldh;
	UINT8		sldv;
} DLGSLD;

#if 0
typedef struct {
	VRAMHDL		vram;
} DLGVRAM;
#endif

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
	virtual void OnClick(int x, int y);
	virtual void OnMove(int x, int y, int focus);
	virtual void OnRelease(int focus);
	virtual INTPTR ItemProc(int ctrl, INTPTR arg);

protected:
	int Send(int msg, long param = 0);
	VRAMHDL GetVram();
	void Invalidate();
	void PaintIcon(VRAMHDL src);

public:
	MenuDialog* m_pParent;		//!< The instance of parent
	int			m_type;			// *
	MENUID		m_id;
	MENUFLG		m_flag;
	MENUID		m_page;			// *
	MENUID		m_group;		// *
	RECT_T		m_rect;
	DLGPRM		prm;
	int			prmcnt;
	int			m_nValue;
	VRAMHDL		m_vram;			// *
	union {
		DLGTEXT		dt;
		DLGTAB		dtl;
		DLGLIST		dl;
		DLGSLD		ds;
//		DLGVRAM		dv;
	} c;
};
typedef MenuDlgItem *DLGHDL;

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
	VRAMHDL		m_vram;
	std::vector<MenuDlgItem*> m_items;
	LISTARRAY	res;
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
	MenuDlgItem* GetItem(MENUID id);
	MenuDlgItem* GetItemFromPosition(int x, int y);
};
typedef MenuDialog *MENUDLG;

static MenuDialog s_menudlg;

static void drawctrls(MENUDLG dlg, DLGHDL hdl)
{
	dlg->DrawItem(hdl);
}

// ----

/**
 * �R���X�g���N�^
 */
MenuDlgItem::MenuDlgItem(MenuDialog* pParent, int type, MENUID id, MENUFLG flg, const RECT_T& rect)
	: m_pParent(pParent)
	, m_type(type)
	, m_id(id)
	, m_flag(flg)
	, m_rect(rect)
	, m_nValue(0)
	, m_vram(NULL)
{
	m_page = pParent->m_page;
	m_group = pParent->m_group;
	this->prm = NULL;
	this->prmcnt = 0;
	memset(&this->c, 0, sizeof(this->c));
}

/**
 * �f�X�g���N�^
 */
MenuDlgItem::~MenuDlgItem()
{
	vram_destroy(m_vram);
}

BRESULT MenuDlgItem::OnCreate(const void *arg)
{
	return SUCCESS;
}

void MenuDlgItem::OnSetValue(int val)
{
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
 * �`�� VRAM�𓾂�
 */
inline VRAMHDL MenuDlgItem::GetVram()
{
	return m_pParent->m_vram;
}

/**
 * �ĕ`��w��
 */
inline void MenuDlgItem::Invalidate()
{
	m_pParent->DrawItem(this);
}

/**
 * �A�C�R���`��
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

static BOOL seaprmempty(void *vpItem, void *vpArg) {

	if (((DLGPRM)vpItem)->_next == PRMNEXT_EMPTY) {
		menuicon_unlock(((DLGPRM)vpItem)->icon);
		((DLGPRM)vpItem)->icon = NULL;
		return(TRUE);
	}
	(void)vpArg;
	return(FALSE);
}

static DLGPRM resappend(MENUDLG dlg, const OEMCHAR *str) {

	DLGPRM	prm;

	prm = (DLGPRM)listarray_enum(dlg->res, seaprmempty, NULL);
	if (prm == NULL) {
		prm = (DLGPRM)listarray_append(dlg->res, NULL);
	}
	if (prm) {
		prm->_next = NULL;
		prm->width = 0;
		prm->num = 0;
		prm->icon = NULL;
		prm->str[0] = '\0';
		if (str) {
			milstr_ncpy(prm->str, str, NELEMENTS(prm->str));
		}
	}
	return(prm);
}

static void resattachicon(MENUDLG dlg, DLGPRM prm, UINT16 icon,
													int width, int height) {

	if (prm) {
		menuicon_unlock(prm->icon);
		prm->num = icon;
		prm->icon = menuicon_lock(icon, width, height, dlg->m_vram->bpp);
	}
}

static DLGPRM ressea(DLGHDL hdl, int pos) {

	DLGPRM	prm;

	if (pos >= 0) {
		prm = hdl->prm;
		while(prm) {
			if (!pos) {
				return(prm);
			}
			pos--;
			prm = prm->_next;
		}
	}
	return(NULL);
}

static BRESULT gettextsz(DLGHDL hdl, POINT_T *sz) {

	DLGPRM	prm;

	prm = hdl->prm;
	if (prm == NULL) {
		goto gts_err;
	}
	*sz = hdl->c.dt.pt;
	if (prm->icon) {
		if (sz->x) {
#if defined(SIZE_QVGA)
			sz->x += 1;
#else
			sz->x += 2;
#endif
		}
		sz->x += sz->y;
	}
	return(SUCCESS);

gts_err:
	return(FAILURE);
}

static void getleft(POINT_T *pt, const RECT_T *rect, const POINT_T *sz) {

	pt->x = rect->left;
	pt->y = rect->top;
	(void)sz;
}

static void getcenter(POINT_T *pt, const RECT_T *rect, const POINT_T *sz) {

	pt->x = rect->left;
	pt->x += (rect->right - rect->left - sz->x) >> 1;
	pt->y = rect->top;
}

static void getright(POINT_T *pt, const RECT_T *rect, const POINT_T *sz) {

	pt->x = rect->right - sz->x - MENU_DSTEXT;
	pt->y = rect->top;
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
	m_vram = vram_create(width, MENUDLG_CYCAPTION, FALSE, g_menubase.bpp);
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
	OEMCHAR* title = NULL;
	if (this->prm)
	{
		title = this->prm->str;
	}

	VRAMHDL vram = GetVram();
	menuvram_base(vram);
	vrammix_cpy(vram, NULL, m_vram, NULL);
	menubase_setrect(vram, NULL);
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
			VRAMHDL vram = m_pParent->m_vram;
			menubase_clrrect(vram);
			vram->posx += x;
			vram->posy += y;
			menubase_setrect(m_vram, NULL);
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
			(*m_pParent->m_proc)(DLGMSG_CLOSE, 0, 0);
		}
	}
};



/**
 * @brief �e�L�X�g
 */
class MenuDlgItemText : public MenuDlgItem
{
public:
	MenuDlgItemText(MenuDialog* pParent, int type, MENUID id, MENUFLG flg, const RECT_T& rect);

	virtual BRESULT OnCreate(const void *arg);
	virtual void OnPaint();
	virtual INTPTR ItemProc(int ctrl, INTPTR arg);

public:
	void DrawText(const POINT_T& pt);
};

MenuDlgItemText::MenuDlgItemText(MenuDialog* pParent, int type, MENUID id, MENUFLG flg, const RECT_T& rect)
	: MenuDlgItem(pParent, type, id, flg, rect)
{
}

BRESULT MenuDlgItemText::OnCreate(const void *arg)
{
	const OEMCHAR* str = static_cast<const OEMCHAR*>(arg);
	if (str == NULL)
	{
		str = str_null;
	}
	this->prm = resappend(m_pParent, str);
	this->c.dt.font = m_pParent->m_font;
	fontmng_getsize(m_pParent->m_font, str, &this->c.dt.pt);
	return SUCCESS;
}

void MenuDlgItemText::OnPaint()
{
	VRAMHDL vram = GetVram();
	vram_filldat(vram, &m_rect, menucolor[MVC_STATIC]);

	POINT_T sz;
	if (gettextsz(this, &sz) == SUCCESS)
	{
		void (*getpt)(POINT_T *pt, const RECT_T *rect, const POINT_T *sz);
		switch (m_flag & MST_POSMASK)
		{
			case MST_LEFT:
			default:
				getpt = getleft;
				break;

			case MST_CENTER:
				getpt = getcenter;
				break;

			case MST_RIGHT:
				getpt = getright;
				break;
		}
		POINT_T pt;
		(*getpt)(&pt, &m_rect, &sz);
		DrawText(pt);
	}
}

INTPTR MenuDlgItemText::ItemProc(int ctrl, INTPTR arg)
{
	INTPTR ret = 0;
	switch (ctrl)
	{
		case DMSG_SETTEXT:
			if (this->prm)
			{
				const OEMCHAR* str = reinterpret_cast<OEMCHAR*>(arg);
				if (str == NULL)
				{
					str = str_null;
				}
				milstr_ncpy(this->prm->str, str, NELEMENTS(this->prm->str));
				fontmng_getsize(this->c.dt.font, str, &this->c.dt.pt);
				Invalidate();
			}
			break;

		case DMSG_SETICON:
			if (this->prm)
			{
				resattachicon(m_pParent, this->prm, (UINT16)arg, this->c.dt.pt.y, this->c.dt.pt.y);
				Invalidate();
			}
			break;

		default:
			ret = __super::ItemProc(ctrl, arg);
			break;
	}
	return ret;
}

void MenuDlgItemText::DrawText(const POINT_T& pt)
{
	if (this->prm == NULL)
	{
		return;
	}

	VRAMHDL vram = GetVram();
	POINT_T fp = pt;
	VRAMHDL icon = this->prm->icon;
	if (icon)
	{
		if (icon->alpha)
		{
			vramcpy_cpyex(vram, &fp, icon, NULL);
		}
		else {
			vramcpy_cpy(vram, &fp, icon, NULL);
		}
		fp.x += icon->width;
#if defined(SIZE_QVGA)
		fp.x += 1;
#else
		fp.x += 2;
#endif
	}

	const OEMCHAR* string = this->prm->str;
	if (string)
	{
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
			vrammix_text(vram, this->c.dt.font, string, menucolor[MVC_GRAYTEXT2], &p, &m_rect);
			color = MVC_GRAYTEXT1;
		}
		vrammix_text(vram, this->c.dt.font, string, menucolor[color], &fp, &m_rect);
	}
}

#if 0
static void dlgtext_itemset(MENUDLG dlg, DLGHDL hdl, const OEMCHAR *str) {

	if (hdl->prm) {
		if (str == NULL) {
			str = str_null;
		}
		milstr_ncpy(hdl->prm->str, str, NELEMENTS(hdl->prm->str));
		fontmng_getsize(hdl->c.dt.font, str, &hdl->c.dt.pt);
	}
	(void)dlg;
}

static void dlgtext_iconset(MENUDLG dlg, DLGHDL hdl, UINT arg) {

	if (hdl->prm) {
		resattachicon(dlg, hdl->prm, (UINT16)arg, hdl->c.dt.pt.y, hdl->c.dt.pt.y);
	}
	(void)dlg;
}
#endif


static void dlg_text(MENUDLG dlg, DLGHDL hdl, const POINT_T *pt, const RECT_T *rect)
{
	if (hdl->prm == NULL)
	{
		return;
	}
	POINT_T fp = *pt;
	VRAMHDL icon = hdl->prm->icon;
	if (icon)
	{
		if (icon->alpha)
		{
			vramcpy_cpyex(dlg->m_vram, &fp, icon, NULL);
		}
		else {
			vramcpy_cpy(dlg->m_vram, &fp, icon, NULL);
		}
		fp.x += icon->width;
#if defined(SIZE_QVGA)
		fp.x += 1;
#else
		fp.x += 2;
#endif
	}

	const OEMCHAR* string = hdl->prm->str;
	if (string)
	{
		int color;
		if (!(hdl->m_flag & MENU_GRAY))
		{
			color = MVC_TEXT;
		}
		else
		{
			POINT_T p;
			p.x = fp.x + MENU_DSTEXT;
			p.y = fp.y + MENU_DSTEXT;
			vrammix_text(dlg->m_vram, hdl->c.dt.font, string, menucolor[MVC_GRAYTEXT2], &p, rect);
			color = MVC_GRAYTEXT1;
		}
		vrammix_text(dlg->m_vram, hdl->c.dt.font, string, menucolor[color], &fp, rect);
	}
}



// ---- button

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

		POINT_T sz;
		if (gettextsz(this, &sz) == SUCCESS)
		{
			POINT_T pt;
			getmid(&pt, &m_rect, &sz);
			if (m_nValue)
			{
				pt.x += MENU_DSTEXT;
				pt.y += MENU_DSTEXT;
			}
			dlg_text(m_pParent, this, &pt, &m_rect);
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
			(*m_pParent->m_proc)(DLGMSG_COMMAND, m_id, 0);
		}
	}
};



// ---- list

static FONTMNGH dlglist_setfont(DLGHDL hdl, FONTMNGH font) {
										// ��ŃX�N���[���o�[�̒��������ׂ�
	FONTMNGH ret;
	POINT_T	pt;

	ret = hdl->c.dl.font;
	hdl->c.dl.font = font;
	fontmng_getsize(font, mstr_fontcheck, &pt);
	if ((pt.y <= 0) || (pt.y >= 65536)) {
		pt.y = 16;
	}
	hdl->c.dl.fontsize = (SINT16)pt.y;
	hdl->c.dl.dispmax = (SINT16)(hdl->m_vram->height / pt.y);
	return(ret);
}

static void dlglist_reset(MENUDLG dlg, DLGHDL hdl) {

	DLGPRM	dp;
	DLGPRM	next;

	vram_filldat(hdl->m_vram, NULL, 0xffffff);
	dp = hdl->prm;
	while(dp) {
		next = dp->_next;
		dp->_next = PRMNEXT_EMPTY;
		dp = next;
	}
	hdl->prm = NULL;
	hdl->prmcnt = 0;
	hdl->m_nValue = -1;
	hdl->c.dl.scrollbar = 0;
	hdl->c.dl.basepos = 0;
}

static BRESULT _dlglist_create(MENUDLG dlg, DLGHDL hdl, const void *arg) {

	int		width;
	int		height;

	width = hdl->m_rect.right - hdl->m_rect.left - (MENU_LINE * 4);
	height = hdl->m_rect.bottom - hdl->m_rect.top - (MENU_LINE * 4);
	hdl->m_vram = vram_create(width, height, FALSE, g_menubase.bpp);
	if (hdl->m_vram == NULL) {
		goto dlcre_err;
	}
	hdl->m_vram->posx = hdl->m_rect.left + (MENU_LINE * 2);
	hdl->m_vram->posy = hdl->m_rect.top + (MENU_LINE * 2);
	dlglist_setfont(hdl, dlg->m_font);
	dlglist_reset(dlg, hdl);
	return(SUCCESS);

dlcre_err:
	(void)dlg;
	(void)arg;
	return(FAILURE);
}

static void _dlglist_paint(MENUDLG dlg, DLGHDL hdl) {

	menuvram_box2(dlg->m_vram, &hdl->m_rect,
						MVC4(MVC_SHADOW, MVC_HILIGHT, MVC_DARK, MVC_LIGHT));
	vrammix_cpy(dlg->m_vram, NULL, hdl->m_vram, NULL);
}

static void dlglist_drawitem(DLGHDL hdl, DLGPRM prm, int focus,
												POINT_T *pt, RECT_T *rct) {

	VRAMHDL	icon;
	POINT_T	fp;

	vram_filldat(hdl->m_vram, rct, menucolor[focus?MVC_CURBACK:MVC_HILIGHT]);
	fp.x = pt->x;
	fp.y = pt->y;
	icon = prm->icon;
	if (icon) {
		if (icon->alpha) {
			vramcpy_cpyex(hdl->m_vram, &fp, icon, NULL);
		}
		else {
			vramcpy_cpy(hdl->m_vram, &fp, icon, NULL);
		}
		fp.x += icon->width;
#if defined(SIZE_QVGA)
		fp.x += 1;
#else
		fp.x += 2;
#endif
	}
	vrammix_text(hdl->m_vram, hdl->c.dl.font, prm->str,
							menucolor[focus?MVC_CURTEXT:MVC_TEXT], &fp, rct);
}

static BOOL dlglist_drawsub(DLGHDL hdl, int pos, int focus) {

	DLGPRM	prm;
	POINT_T	pt;
	RECT_T	rct;

	prm = ressea(hdl, pos);
	if (prm == NULL) {
		return(FALSE);
	}
	pos -= hdl->c.dl.basepos;
	if (pos < 0) {
		return(FALSE);
	}
	pt.x = 0;
	pt.y = pos * hdl->c.dl.fontsize;
	if (pt.y >= hdl->m_vram->height) {
		return(FALSE);
	}
	rct.left = 0;
	rct.top = pt.y;
	rct.right = hdl->m_vram->width;
	if (hdl->prmcnt > hdl->c.dl.dispmax) {
		rct.right -= MENUDLG_CXVSCR;
	}
	rct.bottom = rct.top + hdl->c.dl.fontsize;
	dlglist_drawitem(hdl, prm, focus, &pt, &rct);
	return(TRUE);
}

static void dlglist_setbtn(DLGHDL hdl, int flg) {

	RECT_T		rct;
	POINT_T		pt;
	UINT		mvc4;
const MENURES2	*res;

	res = menures_scrbtn;
	rct.right = hdl->m_vram->width;
	rct.left = rct.right - MENUDLG_CXVSCR;
	if (!(flg & 2)) {
		rct.top = 0;
	}
	else {
		rct.top = hdl->m_vram->height - MENUDLG_CYVSCR;
		if (rct.top < MENUDLG_CYVSCR) {
			rct.top = MENUDLG_CYVSCR;
		}
		res++;
	}
	rct.bottom = rct.top + MENUDLG_CYVSCR;

	vram_filldat(hdl->m_vram, &rct, menucolor[MVC_BTNFACE]);
	if (flg & 1) {
		mvc4 = MVC4(MVC_SHADOW, MVC_SHADOW, MVC_LIGHT, MVC_LIGHT);
	}
	else {
		mvc4 = MVC4(MVC_LIGHT, MVC_DARK, MVC_HILIGHT, MVC_SHADOW);
	}
	menuvram_box2(hdl->m_vram, &rct, mvc4);
	pt.x = rct.left + (MENU_LINE * 2);
	pt.y = rct.top + (MENU_LINE * 2);
	if (flg & 1) {
		pt.x += MENU_DSTEXT;
		pt.y += MENU_DSTEXT;
	}
	menuvram_res3put(hdl->m_vram, res, &pt, MVC_TEXT);
}

static void dlglist_drawall(DLGHDL hdl) {

	DLGPRM	prm;
	POINT_T	pt;
	RECT_T	rct;
	int		pos;

	rct.left = 0;
	rct.top = 0 - (hdl->c.dl.basepos * hdl->c.dl.fontsize);
	rct.right = hdl->m_vram->width;
	if (hdl->prmcnt > hdl->c.dl.dispmax) {
		rct.right -= MENUDLG_CXVSCR;
	}

	prm = hdl->prm;
	pos = 0;
	while(prm) {
		if (rct.top >= hdl->m_vram->height) {
			break;
		}
		if (rct.top >= 0) {
			rct.bottom = rct.top + hdl->c.dl.fontsize;
			pt.x = 0;
			pt.y = rct.top;
			dlglist_drawitem(hdl, prm, (pos == hdl->m_nValue), &pt, &rct);
		}
		prm = prm->_next;
		pos++;
		rct.top += hdl->c.dl.fontsize;
	}
	rct.bottom = hdl->m_vram->height;
	vram_filldat(hdl->m_vram, &rct, menucolor[MVC_HILIGHT]);
}

static int dlglist_barpos(DLGHDL hdl) {

	int		ret;

	ret = hdl->m_vram->height - (MENUDLG_CYVSCR * 2);
	ret -= hdl->c.dl.scrollbar;
	ret *= hdl->c.dl.basepos;
	ret /= (hdl->prmcnt - hdl->c.dl.dispmax);
	return(ret);
}

static void dlglist_drawbar(DLGHDL hdl) {

	RECT_T	rct;

	rct.right = hdl->m_vram->width;
	rct.left = rct.right - MENUDLG_CXVSCR;
	rct.top = MENUDLG_CYVSCR;
	rct.bottom = hdl->m_vram->height - MENUDLG_CYVSCR;
	vram_filldat(hdl->m_vram, &rct, menucolor[MVC_SCROLLBAR]);

	rct.top += dlglist_barpos(hdl);
	rct.bottom = rct.top + hdl->c.dl.scrollbar;
	vram_filldat(hdl->m_vram, &rct, menucolor[MVC_BTNFACE]);
	menuvram_box2(hdl->m_vram, &rct,
						MVC4(MVC_LIGHT, MVC_DARK, MVC_HILIGHT, MVC_SHADOW));
}

static BOOL dlglist_append(MENUDLG dlg, DLGHDL hdl, const OEMCHAR* arg) {

	BOOL	r;
	DLGPRM	*sto;
	int		barsize;

	r = FALSE;
	sto = &hdl->prm;
	while(*sto) {
		sto = &((*sto)->_next);
	}
	*sto = resappend(dlg, arg);
	if (*sto) {
		r = dlglist_drawsub(hdl, hdl->prmcnt, FALSE);
		hdl->prmcnt++;
		if (hdl->prmcnt > hdl->c.dl.dispmax) {
			barsize = hdl->m_vram->height - (MENUDLG_CYVSCR * 2);
			if (barsize >= 8) {
				barsize *= hdl->c.dl.dispmax;
				barsize /= hdl->prmcnt;
				barsize = max(barsize, 6);
				if (!hdl->c.dl.scrollbar) {
					dlglist_drawall(hdl);
					dlglist_setbtn(hdl, 0);
					dlglist_setbtn(hdl, 2);
				}
				hdl->c.dl.scrollbar = barsize;
				dlglist_drawbar(hdl);
			}
		}
	}
	return(r);
}

static BOOL dlglist_setex(MENUDLG dlg, DLGHDL hdl, const ITEMEXPRM *arg) {

	DLGPRM	dp;
	UINT	cnt;

	if ((arg == NULL) || (arg->pos >= hdl->prmcnt)) {
		return(FALSE);
	}
	cnt = arg->pos;
	dp = hdl->prm;
	while((cnt) && (dp)) {
		cnt--;
		dp = dp->_next;
	}
	if (dp == NULL) {
		return(FALSE);
	}
	resattachicon(dlg, dp, arg->icon, hdl->c.dl.fontsize,
											hdl->c.dl.fontsize);
	milstr_ncpy(dp->str, arg->str, NELEMENTS(dp->str));
	return(dlglist_drawsub(hdl, arg->pos, (arg->pos == hdl->m_nValue)));
}

static int dlglist_getpos(DLGHDL hdl, int y) {

	int		val;

	val = (y / hdl->c.dl.fontsize) + hdl->c.dl.basepos;
	if ((unsigned int)val < (unsigned int)hdl->prmcnt) {
		return(val);
	}
	else {
		return(-1);
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

static int dlglist_getpc(DLGHDL hdl, int x, int y) {

	if ((unsigned int)x >= (unsigned int)hdl->m_vram->width) {
		goto dlgp_out;
	}
	if ((unsigned int)y >= (unsigned int)hdl->m_vram->height) {
		goto dlgp_out;
	}

	if ((hdl->prmcnt < hdl->c.dl.dispmax) ||
		(x < (hdl->m_vram->width - MENUDLG_CXVSCR))) {
		return(DLCUR_INLIST);
	}
	else if (y < MENUDLG_CYVSCR) {
		return(DLCUR_UP);
	}
	else if (y >= (hdl->m_vram->height - MENUDLG_CYVSCR)) {
		return(DLCUR_DOWN);
	}
	y -= MENUDLG_CYVSCR;
	y -= dlglist_barpos(hdl);
	if (y < 0) {
		return(DLCUR_PGUP);
	}
	else if (y < (int)hdl->c.dl.scrollbar) {
		return(DLCUR_INBAR);
	}
	else {
		return(DLCUR_PGDN);
	}

dlgp_out:
	return(DLCUR_OUT);
}

static void _dlglist_setval(MENUDLG dlg, DLGHDL hdl, int val) {

	BOOL	r;

	if ((unsigned int)val >= (unsigned int)hdl->prmcnt) {
		val = -1;
	}
	if (val != hdl->m_nValue)
	{
		r = dlglist_drawsub(hdl, hdl->m_nValue, FALSE);
		r |= dlglist_drawsub(hdl, val, TRUE);
		hdl->m_nValue = val;
		if (r) {
			drawctrls(dlg, hdl);
		}
	}
}

static void dlglist_setbasepos(MENUDLG dlg, DLGHDL hdl, int pos) {

	int		displimit;

	if (pos < 0) {
		pos = 0;
	}
	else {
		displimit = hdl->prmcnt - hdl->c.dl.dispmax;
		if (displimit < 0) {
			displimit = 0;
		}
		if (pos > displimit) {
			pos = displimit;
		}
	}
	if (hdl->c.dl.basepos != pos) {
		hdl->c.dl.basepos = pos;
		dlglist_drawall(hdl);
		dlglist_drawbar(hdl);
	}
	(void)dlg;
}

static void _dlglist_onclick(MENUDLG dlg, DLGHDL hdl, int x, int y) {

	int		flg;
	int		val;

	x -= (MENU_LINE * 2);
	y -= (MENU_LINE * 2);
	flg = dlglist_getpc(hdl, x, y);
	dlg->m_dragflg = flg;
	switch(flg) {
		case DLCUR_INLIST:
			val = dlglist_getpos(hdl, y);
			if ((val == hdl->m_nValue) && (val != -1)) {
				dlg->m_dragflg = DLCUR_INCUR;
			}
			_dlglist_setval(dlg, hdl, val);
			(*dlg->m_proc)(DLGMSG_COMMAND, hdl->m_id, 0);
			break;

		case 1:
		case 3:
			dlglist_setbtn(hdl, flg);
			dlglist_setbasepos(dlg, hdl, hdl->c.dl.basepos + flg - 2);
			drawctrls(dlg, hdl);
			break;

		case DLCUR_INBAR:
			y -= MENUDLG_CYVSCR;
			y -= dlglist_barpos(hdl);
			if ((unsigned int)y < (unsigned int)hdl->c.dl.scrollbar) {
				dlg->m_lasty = y;
			}
			else {
				dlg->m_lasty = -1;
			}
			break;

		case DLCUR_PGUP:
			dlglist_setbasepos(dlg, hdl, hdl->c.dl.basepos - hdl->c.dl.dispmax);
			drawctrls(dlg, hdl);
			break;

		case DLCUR_PGDN:
			dlglist_setbasepos(dlg, hdl, hdl->c.dl.basepos + hdl->c.dl.dispmax);
			drawctrls(dlg, hdl);
			break;
	}
}

static void _dlglist_move(MENUDLG dlg, DLGHDL hdl, int x, int y, int focus) {

	int		flg;
	int		val;
	int		height;

	x -= (MENU_LINE * 2);
	y -= (MENU_LINE * 2);
	flg = dlglist_getpc(hdl, x, y);
	switch(dlg->m_dragflg) {
		case DLCUR_INLIST:
		case DLCUR_INCUR:
			if (flg == DLCUR_INLIST) {
				val = dlglist_getpos(hdl, y);
				if (val != hdl->m_nValue) {
					dlg->m_dragflg = DLCUR_INLIST;
					_dlglist_setval(dlg, hdl, val);
					(*dlg->m_proc)(DLGMSG_COMMAND, hdl->m_id, 0);
				}
			}
			break;

		case 1:
		case 3:
			dlglist_setbtn(hdl, dlg->m_dragflg - ((dlg->m_dragflg == flg)?0:1));
			drawctrls(dlg, hdl);
			break;

		case DLCUR_INBAR:
			if (dlg->m_lasty >= 0) {
				y -= MENUDLG_CYVSCR;
				y -= dlg->m_lasty;
				height = hdl->m_vram->height - (MENUDLG_CYVSCR * 2);
				height -= hdl->c.dl.scrollbar;
				if (y < 0) {
					y = 0;
				}
				else if (y > height) {
					y = height;
				}
				y *= (hdl->prmcnt - hdl->c.dl.dispmax);
				y /= height;
				dlglist_setbasepos(dlg, hdl, y);
				drawctrls(dlg, hdl);
			}
			break;
	}
	(void)focus;
}

static void _dlglist_rel(MENUDLG dlg, DLGHDL hdl, int focus) {

	switch (dlg->m_dragflg)
	{
		case 1:
		case 3:
			dlglist_setbtn(hdl, dlg->m_dragflg - 1);
			drawctrls(dlg, hdl);
			break;

		case DLCUR_INCUR:
			(*dlg->m_proc)(DLGMSG_COMMAND, hdl->m_id, 1);
			break;
	}
	(void)focus;
}

class MenuDlgItemList : public MenuDlgItem
{
public:
	MenuDlgItemList(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
		: MenuDlgItem(pParent, DLGTYPE_LIST, id, flg, rect)
	{
	}

	virtual BRESULT OnCreate(const void *arg)
	{
		return _dlglist_create(m_pParent, this, arg);
	}

	virtual void OnPaint()
	{
		VRAMHDL vram = m_pParent->m_vram;
		menuvram_box2(vram, &m_rect, MVC4(MVC_SHADOW, MVC_HILIGHT, MVC_DARK, MVC_LIGHT));
		vrammix_cpy(vram, NULL, m_vram, NULL);
	}

	virtual void OnSetValue(int val)
	{
		_dlglist_setval(m_pParent, this, val);
	}

	virtual void OnClick(int x, int y)
	{
		_dlglist_onclick(m_pParent, this, x, y);
	}

	virtual void OnMove(int x, int y, int focus)
	{
		_dlglist_move(m_pParent, this, x, y, focus);
	}

	virtual void OnRelease(int focus)
	{
		_dlglist_rel(m_pParent, this, focus);
	}

//	virtual INTPTR ItemProc(int ctrl, INTPTR arg);

};

// ---- slider

static void dlgslider_setflag(DLGHDL hdl) {

	int		size;
	UINT	type;

	if (!(hdl->m_flag & MSS_VERT)) {
		size = hdl->m_rect.bottom - hdl->m_rect.top;
	}
	else {
		size = hdl->m_rect.right - hdl->m_rect.left;
	}
	if (size < 13) {
		type = 0 + (9 << 8) + (5 << 16);
	}
	else if (size < 21) {
		type = 1 + (13 << 8) + (7 << 16);
	}
	else {
		type = 2 + (21 << 8) + (11 << 16);
	}
	hdl->c.ds.type = (UINT8)type;
	if (!(hdl->m_flag & MSS_VERT)) {
		hdl->c.ds.sldh = (UINT8)(type >> 16);
		hdl->c.ds.sldv = (UINT8)(type >> 8);
	}
	else {
		hdl->c.ds.sldh = (UINT8)(type >> 8);
		hdl->c.ds.sldv = (UINT8)(type >> 16);
	}
}

static int dlgslider_setpos(DLGHDL hdl, int val) {

	int		range;
	int		width;
	int		dir;

	range = hdl->c.ds.maxval - hdl->c.ds.minval;
	if (range) {
		dir = (range > 0)?1:-1;
		val -= hdl->c.ds.minval;
		val *= dir;
		range *= dir;
		if (val < 0) {
			val = 0;
		}
		else if (val >= range) {
			val = range;
		}
		hdl->m_nValue = hdl->c.ds.minval + (val * dir);
		if (!(hdl->m_flag & MSS_VERT)) {
			width = hdl->m_rect.right - hdl->m_rect.left;
			width -= hdl->c.ds.sldh;
		}
		else {
			width = hdl->m_rect.bottom - hdl->m_rect.top;
			width -= hdl->c.ds.sldv;
		}
		if ((width > 0) || (range)) {
			val *= width;
			val /= range;
		}
		else {
			val = 0;
		}
	}
	else {
		val = 0;
	}
	return(val);
}

static BRESULT _dlgslider_create(MENUDLG dlg, DLGHDL hdl, const void *arg) {

	hdl->c.ds.minval = (SINT16)(long)arg;
	hdl->c.ds.maxval = (SINT16)((long)arg >> 16);
	hdl->c.ds.moving = 0;
	dlgslider_setflag(hdl);
	hdl->c.ds.pos = dlgslider_setpos(hdl, 0);
	(void)dlg;
	return(SUCCESS);
}

static void _dlgslider_paint(MENUDLG dlg, DLGHDL hdl) {

	UINT		flag;
	int			ptr;
	RECT_U		rct;
	POINT_T		pt;
	MENURES2	src;

	flag = hdl->m_flag;
	switch(flag & MSS_POSMASK) {
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
	vram_filldat(dlg->m_vram, &hdl->m_rect, menucolor[MVC_STATIC]);
	if (!(hdl->m_flag & MSS_VERT)) {
		rct.r.left = hdl->m_rect.left;
		rct.r.right = hdl->m_rect.right;
		rct.r.top = hdl->m_rect.top + ptr +
									(hdl->c.ds.sldv / 2) - (MENU_LINE * 2);
		rct.r.bottom = rct.r.top + (MENU_LINE * 4);
		menuvram_box2(dlg->m_vram, &rct.r,
						MVC4(MVC_SHADOW, MVC_HILIGHT, MVC_DARK, MVC_LIGHT));
		pt.x = hdl->m_rect.left + hdl->c.ds.pos;
		pt.y = hdl->m_rect.top;
	}
	else {
		rct.r.left = hdl->m_rect.left + ptr +
									(hdl->c.ds.sldh / 2) - (MENU_LINE * 2);
		rct.r.right = rct.r.left + (MENU_LINE * 4);
		rct.r.top = hdl->m_rect.top;
		rct.r.bottom = hdl->m_rect.bottom;
		menuvram_box2(dlg->m_vram, &rct.r,
						MVC4(MVC_SHADOW, MVC_HILIGHT, MVC_DARK, MVC_LIGHT));
		pt.x = hdl->m_rect.left;
		pt.y = hdl->m_rect.top + hdl->c.ds.pos;
		ptr += 3;
	}
	ptr *= 2;
	if ((hdl->m_flag & MENU_GRAY) || (hdl->c.ds.moving)) {
		ptr++;
	}
	src.width = hdl->c.ds.sldh;
	src.height = hdl->c.ds.sldv;
	src.pat = menures_slddat + menures_sldpos[hdl->c.ds.type][ptr];
	menuvram_res2put(dlg->m_vram, &src, &pt);
}

static void _dlgslider_setval(MENUDLG dlg, DLGHDL hdl, int val) {

	int		pos;

	pos = dlgslider_setpos(hdl, val);
	if (hdl->c.ds.pos != pos) {
		hdl->c.ds.pos = pos;
		drawctrls(dlg, hdl);
	}
}

static void _dlgslider_onclick(MENUDLG dlg, DLGHDL hdl, int x, int y) {

	int		width;
	int		range;
	int		dir;

	if (!(hdl->m_flag & MSS_VERT)) {
		width = hdl->c.ds.sldh;
	}
	else {
		width = hdl->c.ds.sldv;
		x = y;
	}
	x -= hdl->c.ds.pos;
	if ((x >= -1) && (x <= width)) {
		dlg->m_dragflg = x;
		hdl->c.ds.moving = 1;
		drawctrls(dlg, hdl);
	}
	else {
		dlg->m_dragflg = -1;
		dir = (x > 0)?1:0;
		range = hdl->c.ds.maxval - hdl->c.ds.minval;
		if (range < 0) {
			range = 0 - range;
			dir ^= 1;
		}
		if (range < 16) {
			range = 16;
		}
		range >>= 4;
		if (!dir) {
			range = 0 - range;
		}
		_dlgslider_setval(dlg, hdl, hdl->m_nValue + range);
		(*dlg->m_proc)(DLGMSG_COMMAND, hdl->m_id, 0);
	}
}

static void _dlgslider_move(MENUDLG dlg, DLGHDL hdl, int x, int y, int focus) {

	int		range;
	int		width;
	int		dir;

	if (hdl->c.ds.moving) {
		range = hdl->c.ds.maxval - hdl->c.ds.minval;
		if (range) {
			dir = (range > 0)?1:-1;
			range *= dir;
			if (!(hdl->m_flag & MSS_VERT)) {
				width = hdl->m_rect.right - hdl->m_rect.left;
				width -= hdl->c.ds.sldh;
			}
			else {
				width = hdl->m_rect.bottom - hdl->m_rect.top;
				width -= hdl->c.ds.sldv;
				x = y;
			}
			x -= dlg->m_dragflg;
			if ((x < 0) || (width <= 0)) {
				x = 0;
			}
			else if (x >= width) {
				x = range;
			}
			else {
				x *= range;
				x += (width >> 1);
				x /= width;
			}
			x = hdl->c.ds.minval + (x * dir);
			_dlgslider_setval(dlg, hdl, x);
			(*dlg->m_proc)(DLGMSG_COMMAND, hdl->m_id, 0);
		}
	}
	(void)focus;
}


static void _dlgslider_rel(MENUDLG dlg, DLGHDL hdl, int focus) {

	if (hdl->c.ds.moving) {
		hdl->c.ds.moving = 0;
		drawctrls(dlg, hdl);
	}
	(void)focus;
}


class MenuDlgItemSlider : public MenuDlgItem
{
public:
	MenuDlgItemSlider(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
		: MenuDlgItem(pParent, DLGTYPE_SLIDER, id, flg, rect)
	{
	}

	virtual BRESULT OnCreate(const void *arg)
	{
		return _dlgslider_create(m_pParent, this, arg);
	}

	virtual void OnPaint()
	{
		_dlgslider_paint(m_pParent, this);
	}

	virtual void OnSetValue(int val)
	{
		_dlgslider_setval(m_pParent, this, val);
	}

	virtual void OnClick(int x, int y)
	{
		_dlgslider_onclick(m_pParent, this, x, y);
	}

	virtual void OnMove(int x, int y, int focus)
	{
		_dlgslider_move(m_pParent, this, x, y, focus);
	}

	virtual void OnRelease(int focus)
	{
		_dlgslider_rel(m_pParent, this, focus);
	}

//	virtual INTPTR ItemProc(int ctrl, INTPTR arg);

};


// ---- tablist

static FONTMNGH dlgtablist_setfont(DLGHDL hdl, FONTMNGH font) {

	FONTMNGH ret;
	POINT_T	pt;
	DLGPRM	prm;

	ret = hdl->c.dtl.font;
	hdl->c.dtl.font = font;
	fontmng_getsize(font, mstr_fontcheck, &pt);
	if ((pt.y <= 0) || (pt.y >= 65536)) {
		pt.y = 16;
	}
	hdl->c.dtl.fontsize = pt.y;
	prm = hdl->prm;
	while(prm) {
		fontmng_getsize(hdl->c.dtl.font, prm->str, &pt);
		prm->width = pt.x;
		prm = prm->_next;
	}
	return(ret);
}

static BRESULT _dlgtablist_create(MENUDLG dlg, DLGHDL hdl, const void *arg) {

	RECT_T	rct;

	rct.right = hdl->m_rect.right - hdl->m_rect.left;
	hdl->m_nValue = -1;
	dlgtablist_setfont(hdl, dlg->m_font);
	(void)arg;
	return(SUCCESS);
}

static void _dlgtablist_paint(MENUDLG dlg, DLGHDL hdl) {

	VRAMHDL	dst;
	DLGPRM	prm;
	POINT_T	pt;
	RECT_T	rct;
	int		posx;
	int		lx;
	int		cnt;
	int		tabey;
	int		tabdy;

	dst = dlg->m_vram;
	rct = hdl->m_rect;
	vram_filldat(dst, &rct, menucolor[MVC_STATIC]);
	tabey = rct.top + hdl->c.dtl.fontsize +
							MENUDLG_SYTAB + MENUDLG_TYTAB + MENUDLG_EYTAB;
	rct.top = tabey;
	menuvram_box2(dst, &rct,
						MVC4(MVC_HILIGHT, MVC_DARK, MVC_LIGHT, MVC_SHADOW));

	posx = hdl->m_rect.left + (MENU_LINE * 2);
	prm = hdl->prm;
	cnt = hdl->m_nValue;
	while(prm) {
		if (cnt) {
			pt.x = posx;
			pt.y = hdl->m_rect.top + MENUDLG_SYTAB;
			menuvram_liney(dst, pt.x, pt.y + (MENU_LINE * 2),
														tabey, MVC_HILIGHT);
			pt.x += MENU_LINE;
			menuvram_liney(dst, pt.x, pt.y + MENU_LINE,
										pt.y + (MENU_LINE * 2), MVC_HILIGHT);
			menuvram_liney(dst, pt.x, pt.y + (MENU_LINE * 2),
														tabey, MVC_LIGHT);
			pt.x += MENU_LINE;
			lx = pt.x + prm->width + (MENUDLG_TXTAB * 2);
			menuvram_linex(dst, pt.x, pt.y, lx, MVC_HILIGHT);
			menuvram_linex(dst, pt.x, pt.y + MENU_LINE, lx, MVC_LIGHT);

			menuvram_liney(dst, lx, pt.y + MENU_LINE,
										pt.y + (MENU_LINE * 2), MVC_DARK);
			menuvram_liney(dst, lx, pt.y + (MENU_LINE * 2),
														tabey, MVC_SHADOW);
			lx++;
			menuvram_liney(dst, lx, pt.y + (MENU_LINE * 2),
														tabey, MVC_DARK);
			pt.x += MENUDLG_TXTAB;
			pt.y += MENUDLG_TYTAB;
			vrammix_text(dst, hdl->c.dtl.font, prm->str,
											menucolor[MVC_TEXT], &pt, NULL);
		}
		cnt--;
		posx += prm->width + (MENU_LINE * 4) + (MENUDLG_TXTAB) * 2;
		prm = prm->_next;
	}

	posx = hdl->m_rect.left;
	prm = hdl->prm;
	cnt = hdl->m_nValue;
	while(prm) {
		if (!cnt) {
			pt.x = posx;
			pt.y = hdl->m_rect.top;
			if (posx == hdl->m_rect.left) {
				tabdy = tabey + 2;
			}
			else {
				tabdy = tabey + 1;
				menuvram_linex(dst, pt.x, tabdy,
										pt.x + (MENU_LINE * 2), MVC_STATIC);
			}
			menuvram_liney(dst, pt.x, pt.y + (MENU_LINE * 2),
														tabdy, MVC_HILIGHT);
			pt.x += MENU_LINE;
			menuvram_liney(dst, pt.x, pt.y + MENU_LINE,
										pt.y + (MENU_LINE * 2), MVC_HILIGHT);
			menuvram_liney(dst, pt.x, pt.y + (MENU_LINE * 2),
														tabdy, MVC_LIGHT);
			pt.x += MENU_LINE;
			lx = pt.x + prm->width + (MENU_LINE * 4) + (MENUDLG_TXTAB * 2);
			menuvram_linex(dst, pt.x, pt.y, lx, MVC_HILIGHT);
			menuvram_linex(dst, pt.x, pt.y + MENU_LINE, lx, MVC_LIGHT);
			menuvram_linex(dst, pt.x, tabey, lx, MVC_STATIC);
			menuvram_linex(dst, pt.x, tabey + MENU_LINE, lx, MVC_STATIC);
			tabdy = tabey + 1;
			menuvram_liney(dst, lx, pt.y + MENU_LINE,
										pt.y + (MENU_LINE * 2), MVC_DARK);
			menuvram_liney(dst, lx, pt.y + (MENU_LINE * 2),
														tabdy, MVC_SHADOW);
			lx++;
			menuvram_liney(dst, lx, pt.y + (MENU_LINE * 2),
														tabdy, MVC_DARK);
			pt.x += MENUDLG_TXTAB + (MENU_LINE * 2);
			pt.y += MENUDLG_TYTAB;
			vrammix_text(dst, hdl->c.dtl.font, prm->str,
											menucolor[MVC_TEXT], &pt, NULL);
			break;
		}
		cnt--;
		posx += prm->width + (MENU_LINE * 4) + (MENUDLG_TXTAB * 2);
		prm = prm->_next;
	}
}

static void _dlgtablist_setval(MENUDLG dlg, DLGHDL hdl, int val) {

	if (hdl->m_nValue != val)
	{
		hdl->m_nValue = val;
		drawctrls(dlg, hdl);
	}
}

static void dlgtablist_append(MENUDLG dlg, DLGHDL hdl, const OEMCHAR *arg) {

	DLGPRM	res;
	DLGPRM	*sto;
	POINT_T	pt;

	sto = &hdl->prm;
	while(*sto) {
		sto = &((*sto)->_next);
	}
	res = resappend(dlg, arg);
	if (res) {
		*sto = res;
		fontmng_getsize(hdl->c.dtl.font, (OEMCHAR *)arg, &pt);
		res->width = pt.x;
		hdl->prmcnt++;
	}
}

static void _dlgtablist_onclick(MENUDLG dlg, DLGHDL hdl, int x, int y) {

	DLGPRM	prm;
	int		pos;

	if (y < (hdl->c.dtl.fontsize +
							MENUDLG_SYTAB + MENUDLG_TYTAB + MENUDLG_EYTAB)) {
		pos = 0;
		prm = hdl->prm;
		while(prm) {
			x -= (MENU_LINE * 4);
			if (x < 0) {
				break;
			}
			x -= prm->width + (MENUDLG_TXTAB * 2);
			if (x < 0) {
				_dlgtablist_setval(dlg, hdl, pos);
				(*dlg->m_proc)(DLGMSG_COMMAND, hdl->m_id, 0);
				break;
			}
			pos++;
			prm = prm->_next;
		}
	}
}


class MenuDlgItemTabList : public MenuDlgItem
{
public:
	MenuDlgItemTabList(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
		: MenuDlgItem(pParent, DLGTYPE_TABLIST, id, flg, rect)
	{
	}

	virtual BRESULT OnCreate(const void *arg)
	{
		return _dlgtablist_create(m_pParent, this, arg);
	}

	virtual void OnPaint()
	{
		_dlgtablist_paint(m_pParent, this);
	}

	virtual void OnSetValue(int val)
	{
		_dlgtablist_setval(m_pParent, this, val);
	}

	virtual void OnClick(int x, int y)
	{
		_dlgtablist_onclick(m_pParent, this, x, y);
	}

//	virtual INTPTR ItemProc(int ctrl, INTPTR arg);

};



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
		if (this->prm)
		{
			const OEMCHAR* string = this->prm->str;
			if (string)
			{
				POINT_T pt;
				pt.x = rct.left + MENU_LINE;
				pt.y = rct.top + MENU_LINE;
				vrammix_text(vram, this->c.dt.font, string, menucolor[MVC_TEXT], &pt, &rct);
			}
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
		rct.right = rct.left + (MENUDLG_PXFRAME * 2) + this->c.dt.pt.x;
		rct.bottom = rct.top + this->c.dt.pt.y + MENU_DSTEXT;
		vram_filldat(vram, &rct, menucolor[MVC_STATIC]);
		if (this->prm)
		{
			POINT_T pt;
			pt.x = rct.left + MENUDLG_PXFRAME;
			pt.y = rct.top;
			DrawText(pt);
		}
	}
};



// ---- radio

static void _dlgradio_paint(MENUDLG dlg, DLGHDL hdl) {

	POINT_T		pt;
const MENURES2	*src;
	int			pat;

	vram_filldat(dlg->m_vram, &hdl->m_rect, menucolor[MVC_STATIC]);
	pt.x = hdl->m_rect.left;
	pt.y = hdl->m_rect.top;
	src = menures_radio;
	pat = (hdl->m_flag & MENU_GRAY)?1:0;
	menuvram_res2put(dlg->m_vram, src + pat, &pt);
	if (hdl->m_nValue) {
		menuvram_res3put(dlg->m_vram, src + 2, &pt,
					(hdl->m_flag & MENU_GRAY)?MVC_GRAYTEXT1:MVC_TEXT);
	}
	pt.x += MENUDLG_SXRADIO;
	dlg_text(dlg, hdl, &pt, &hdl->m_rect);
}

static void _dlgradio_setval(MENUDLG dlg, DLGHDL hdl, int val)
{
	if (hdl->m_nValue != val)
	{
		if (val)
		{
			for (std::vector<MenuDlgItem*>::iterator it = dlg->m_items.begin(); it != dlg->m_items.end(); ++it)
			{
				MenuDlgItem* item = *it;
				if ((item->m_type == DLGTYPE_RADIO) && (item->m_nValue) && (item->m_group == hdl->m_group))
				{
					item->m_nValue = 0;
					drawctrls(dlg, item);
				}
			}
		}
		hdl->m_nValue = val;
		drawctrls(dlg, hdl);
	}
}

static void _dlgradio_onclick(MENUDLG dlg, DLGHDL hdl, int x, int y) {

	if (x < (hdl->c.dt.pt.x + MENUDLG_SXRADIO)) {
		_dlgradio_setval(dlg, hdl, 1);
		(*dlg->m_proc)(DLGMSG_COMMAND, hdl->m_id, 0);
	}
	(void)y;
}

/**
 * @brief radio
 */
class MenuDlgItemRadio : public MenuDlgItemText
{
public:
	MenuDlgItemRadio(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
		: MenuDlgItemText(pParent, DLGTYPE_RADIO, id, flg, rect)
	{
	}

	virtual void OnPaint()
	{
		_dlgradio_paint(m_pParent, this);
	}

	virtual void OnSetValue(int val)
	{
		_dlgradio_setval(m_pParent, this, val);
	}

	virtual void OnClick(int x, int y)
	{
		_dlgradio_onclick(m_pParent, this, x, y);
	}
};



// ---- check

static void _dlgcheck_paint(MENUDLG dlg, DLGHDL hdl) {

	POINT_T	pt;
	RECT_T	rct;
	UINT32	basecol;
	UINT32	txtcol;

	vram_filldat(dlg->m_vram, &hdl->m_rect, menucolor[MVC_STATIC]);
	rct.left = hdl->m_rect.left;
	rct.top = hdl->m_rect.top;
	rct.right = rct.left + MENUDLG_CXCHECK;
	rct.bottom = rct.top + MENUDLG_CYCHECK;
	if (!(hdl->m_flag & MENU_GRAY)) {
		basecol = MVC_HILIGHT;
		txtcol = MVC_TEXT;
	}
	else {
		basecol = MVC_STATIC;
		txtcol = MVC_GRAYTEXT1;
	}
	vram_filldat(dlg->m_vram, &rct, menucolor[basecol]);
	menuvram_box2(dlg->m_vram, &rct,
						MVC4(MVC_SHADOW, MVC_HILIGHT, MVC_DARK, MVC_LIGHT));
	if (hdl->m_nValue) {
		pt.x = rct.left + (MENU_LINE * 2);
		pt.y = rct.top + (MENU_LINE * 2);
		menuvram_res3put(dlg->m_vram, &menures_check, &pt, txtcol);
	}
	pt.x = rct.left + MENUDLG_SXCHECK;
	pt.y = rct.top;
	dlg_text(dlg, hdl, &pt, &hdl->m_rect);
}

static void _dlgcheck_setval(MENUDLG dlg, DLGHDL hdl, int val) {

	if (hdl->m_nValue != val)
	{
		hdl->m_nValue = val;
		drawctrls(dlg, hdl);
	}
}

static void _dlgcheck_onclick(MENUDLG dlg, DLGHDL hdl, int x, int y) {

	if (x < (hdl->c.dt.pt.x + MENUDLG_SXCHECK)) {
		_dlgcheck_setval(dlg, hdl, !hdl->m_nValue);
		(*dlg->m_proc)(DLGMSG_COMMAND, hdl->m_id, 0);
	}
	(void)y;
}

/**
 * @brief check
 */
class MenuDlgItemCheck : public MenuDlgItemText
{
public:
	MenuDlgItemCheck(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
		: MenuDlgItemText(pParent, DLGTYPE_CHECK, id, flg, rect)
	{
	}

	virtual void OnPaint()
	{
		_dlgcheck_paint(m_pParent, this);
	}

	virtual void OnSetValue(int val)
	{
		_dlgcheck_setval(m_pParent, this, val);
	}

	virtual void OnClick(int x, int y)
	{
		_dlgcheck_onclick(m_pParent, this, x, y);
	}
};



/**
 * @brief �A�C�R��
 */
class MenuDlgItemIcon : public MenuDlgItem
{
public:
	MenuDlgItemIcon(MenuDialog* pParent, MENUID id, MENUFLG flg, const RECT_T& rect)
		: MenuDlgItem(pParent, DLGTYPE_ICON, id, flg, rect)
	{
	}

	virtual BRESULT OnCreate(const void *arg)
	{
		this->prm = resappend(m_pParent, NULL);
		resattachicon(m_pParent, this->prm, (UINT16)(long)arg, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top);
		return SUCCESS;
	}

	virtual void OnPaint()
	{
		if (this->prm)
		{
			PaintIcon(this->prm->icon);
		}
	}
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
				drawctrls(m_pParent, this);
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
 * @brief ���C��
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
 * �A�C�e�����쐬
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
 * �R���X�g���N�^
 */
MenuDialog::MenuDialog()
	: m_vram(NULL)
	, res(NULL)
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
 * �A�C�e���𓾂�
 */
MenuDlgItem* MenuDialog::GetItem(MENUID id)
{
	for (std::vector<MenuDlgItem*>::iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		MenuDlgItem* item = *it;
		if (id == item->m_id)
		{
			return item;
		}
	}
	return NULL;
}

/**
 * �A�C�e���𓾂�
 */
MenuDlgItem* MenuDialog::GetItemFromPosition(int x, int y)
{
	for (std::vector<MenuDlgItem*>::reverse_iterator it = m_items.rbegin(); it != m_items.rend(); ++it)
	{
		MenuDlgItem* item = *it;
		if ((!(item->m_flag & (MENU_DISABLE | MENU_GRAY))) && (rect_in(&item->m_rect, x, y)))
		{
			return item;
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
	MENUDLG dlg = (MENUDLG)arg;
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
				menubase_setrect(m_vram, &item->m_rect);
			}
		}
	}
	menubase_draw(draw, this);
}

static int defproc(int msg, MENUID id, long param) {

	if (msg == DLGMSG_CLOSE) {
		menubase_close();
	}
	(void)id;
	(void)param;
	return(0);
}

/**
 * �쐬
 */
bool MenuDialog::Create(int width, int height, const OEMCHAR *str, int (*proc)(int msg, MENUID id, long param))
{
	if (menubase_open(2) != SUCCESS)
	{
		goto mdcre_err;
	}

	m_vram = NULL;
	res = NULL;
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

	if ((width <= 0) || (height <= 0))
	{
		goto mdcre_err;
	}
	width += (MENU_FBORDER + MENU_BORDER) * 2;
	height += ((MENU_FBORDER + MENU_BORDER) * 2) + MENUDLG_CYCAPTION + MENUDLG_BORDER;

	MENUBASE* mb = &g_menubase;
	m_font = mb->font;
	m_vram = vram_create(width, height, FALSE, mb->bpp);
	if (m_vram == NULL)
	{
		goto mdcre_err;
	}
	m_vram->posx = (mb->width - width) >> 1;
	m_vram->posy = (mb->height - height) >> 1;
	this->res = listarray_new(sizeof(DlgItemParam), 32);
	if (this->res == NULL)
	{
		goto mdcre_err;
	}
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
	menubase_close();
	return false;
}

static BOOL delicon(void *vpItem, void *vpArg) {

	menuicon_unlock(((DLGPRM)vpItem)->icon);
	(void)vpArg;
	return(FALSE);
}

void MenuDialog::Destroy()
{
	if (m_bClosing) {
		return;
	}
	m_bClosing = false;
	(*m_proc)(DLGMSG_DESTROY, 0, 0);

	for (std::vector<MenuDlgItem*>::iterator it = m_items.begin(); it != m_items.end(); ++it)
	{
		delete (*it);
	}
	m_items.clear();

	menubase_clrrect(m_vram);
	vram_destroy(m_vram);
	m_vram = NULL;
	listarray_enum(this->res, delicon, NULL);
	listarray_destroy(this->res);
	this->res = NULL;
}

/**
 * �A�C�e���̒ǉ�
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
 * �A�C�e���̒ǉ�
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

static DLGHDL hdlpossea(MENUDLG dlg, int x, int y)
{
	for (std::vector<MenuDlgItem*>::reverse_iterator it = dlg->m_items.rbegin(); it != dlg->m_items.rend(); ++it)
	{
		MenuDlgItem* item = *it;
		if ((!(item->m_flag & (MENU_DISABLE | MENU_GRAY))) && (rect_in(&item->m_rect, x, y)))
		{
			return item;
		}
	}
	return NULL;
}

void MenuDialog::Moving(int x, int y, int btn)
{
	DrawLock(true);
	MENUDLG dlg = this;
	x -= m_vram->posx;
	y -= m_vram->posy;
	if (!m_btn) {
		if (btn == 1) {
			DLGHDL hdl = GetItemFromPosition(x, y);
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
		DLGHDL hdl = GetItem(m_lastid);
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
	DLGHDL hdl = GetItem(id);
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
	MenuDialog* dlg = m_pParent;
	MenuDlgItem* hdl = this;

	int flg = 0;

	INTPTR ret = 0;
	switch (ctrl) 
	{
		case DMSG_SETHIDE:
			ret = (hdl->m_flag & MENU_DISABLE) ? 1 : 0;
			flg = (arg) ? MENU_DISABLE : 0;
			if ((hdl->m_flag ^ flg) & MENU_DISABLE) {
				hdl->m_flag ^= MENU_DISABLE;
				if (flg) {
					drawctrls(dlg, NULL);
				}
				else {
					drawctrls(dlg, hdl);
				}
			}
			break;

		case DMSG_GETHIDE:
			ret = (hdl->m_flag & MENU_DISABLE) ? 1 : 0;
			break;

		case DMSG_SETENABLE:
			ret = (hdl->m_flag & MENU_GRAY) ? 0 : 1;
			flg = (arg) ? 0 : MENU_GRAY;
			if ((hdl->m_flag ^ flg) & MENU_GRAY) {
				hdl->m_flag ^= MENU_GRAY;
				drawctrls(dlg, hdl);
			}
			break;

		case DMSG_GETENABLE:
			ret = (hdl->m_flag & MENU_GRAY) ? 0 : 1;
			break;

		case DMSG_SETVAL:
			ret = hdl->m_nValue;
			hdl->OnSetValue((int)arg);
			break;

		case DMSG_GETVAL:
			ret = hdl->m_nValue;
			break;

#if 0
		case DMSG_SETVRAM:
			if (hdl->m_type == DLGTYPE_VRAM) {
				ret = (INTPTR)hdl->c.dv.vram;
				hdl->c.dv.vram = (VRAMHDL)arg;
				drawctrls(dlg, hdl);
			}
			break;

		case DMSG_SETTEXT:
			switch(hdl->m_type) {
				case DLGTYPE_BUTTON:
				case DLGTYPE_RADIO:
				case DLGTYPE_CHECK:
				case DLGTYPE_EDIT:
				case DLGTYPE_TEXT:
					dlgtext_itemset(dlg, hdl, (OEMCHAR*)arg);
					drawctrls(dlg, hdl);
					break;
			}
			break;

		case DMSG_SETICON:
			switch(hdl->m_type) {
				case DLGTYPE_BUTTON:
				case DLGTYPE_RADIO:
				case DLGTYPE_CHECK:
				case DLGTYPE_EDIT:
				case DLGTYPE_TEXT:
					dlgtext_iconset(dlg, hdl, (UINT)arg);
					drawctrls(dlg, hdl);
					break;
			}
			break;
#endif

		case DMSG_ITEMAPPEND:
			switch(hdl->m_type) {
				case DLGTYPE_LIST:
					if (dlglist_append(dlg, hdl, (OEMCHAR*)arg)) {
						drawctrls(dlg, hdl);
					}
					break;

				case DLGTYPE_TABLIST:
					dlgtablist_append(dlg, hdl, (OEMCHAR*)arg);
					drawctrls(dlg, hdl);
					break;
			}
			break;

		case DMSG_ITEMRESET:
			if ((dlg->m_btn) && (dlg->m_lastid == hdl->m_id))
			{
				dlg->m_btn = 0;
				hdl->OnRelease(FALSE);
			}
			if (hdl->m_type == DLGTYPE_LIST) {
				dlglist_reset(dlg, hdl);
				drawctrls(dlg, hdl);
			}
			break;

		case DMSG_ITEMSETEX:
			if (hdl->m_type == DLGTYPE_LIST) {
				if (dlglist_setex(dlg, hdl, (ITEMEXPRM *)arg)) {
					drawctrls(dlg, hdl);
				}
			}
			break;

		case DMSG_SETLISTPOS:
			if (hdl->m_type == DLGTYPE_LIST) {
				ret = hdl->c.dl.basepos;
				dlglist_setbasepos(dlg, hdl, (int)arg);
				drawctrls(dlg, hdl);
			}
			break;

		case DMSG_GETRECT:
			ret = (INTPTR)&hdl->m_rect;
			break;

		case DMSG_SETRECT:
			ret = (INTPTR)&hdl->m_rect;
			if ((hdl->m_type == DLGTYPE_TEXT) && (arg)) {
				drawctrls(dlg, hdl);
				hdl->m_rect = *(RECT_T *)arg;
				drawctrls(dlg, hdl);
			}
			break;

		case DMSG_SETFONT:
			if (hdl->m_type == DLGTYPE_LIST) {
				ret = (INTPTR)dlglist_setfont(hdl, (FONTMNGH)arg);
				drawctrls(dlg, hdl);
			}
			else if (hdl->m_type == DLGTYPE_TABLIST) {
				ret = (INTPTR)dlgtablist_setfont(hdl, (FONTMNGH)arg);
				drawctrls(dlg, hdl);
			}
			else if (hdl->m_type == DLGTYPE_TEXT) {
				ret = (INTPTR)hdl->c.dt.font;
				hdl->c.dt.font = (FONTMNGH)arg;
				drawctrls(dlg, hdl);
			}
			break;

		case DMSG_GETFONT:
			if (hdl->m_type == DLGTYPE_LIST) {
				ret = (INTPTR)hdl->c.dl.font;
			}
			else if (hdl->m_type == DLGTYPE_TABLIST) {
				ret = (INTPTR)hdl->c.dtl.font;
			}
			else if (hdl->m_type == DLGTYPE_TEXT) {
				ret = (INTPTR)hdl->c.dt.font;
			}
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

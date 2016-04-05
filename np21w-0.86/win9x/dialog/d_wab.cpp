/**
 * @file	d_wab.cpp
 * @brief	Window Accelerator Board configure dialog procedure
 *
 * @author	$Author: SimK $
 * @date	$Date: 2016/03/11 $
 */

#include "compiler.h"
#include "resource.h"
#include "strres.h"
#include "dialog.h"
#include "c_combodata.h"
#include "np2class.h"
#include "dosio.h"
#include "joymng.h"
#include "np2.h"
#include "sysmng.h"
#include "misc\PropProc.h"
#include "pccore.h"
#include "iocore.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

/**
 * @brief ウィンドウアクセラレータ基本設定ページ
 * @param[in] hwndParent 親ウィンドウ
 */
class CWABPage : public CPropPageProc
{
public:
	CWABPage();
	virtual ~CWABPage();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	UINT8 m_awitch;				//!< アナログスイッチモード
	CWndProc m_chkawitch;		//!< ANALOG SWITCH IC
};

/**
 * コンストラクタ
 */
CWABPage::CWABPage()
	: CPropPageProc(IDD_WAB)
{
}
/**
 * デストラクタ
 */
CWABPage::~CWABPage()
{
}

/**
 * このメソッドは WM_INITDIALOG のメッセージに応答して呼び出されます
 * @retval TRUE 最初のコントロールに入力フォーカスを設定
 * @retval FALSE 既に設定済
 */
BOOL CWABPage::OnInitDialog()
{
	m_awitch = np2cfg.wabasw;
	m_chkawitch.SubclassDlgItem(IDC_WABASWITCH, this);
	if(m_awitch)
		m_chkawitch.SendMessage(BM_SETCHECK , BST_CHECKED , 0);
	else
		m_chkawitch.SendMessage(BM_SETCHECK , BST_UNCHECKED , 0);
	

	m_chkawitch.SetFocus();

	return FALSE;
}

/**
 * ユーザーが OK のボタン (IDOK ID がのボタン) をクリックすると呼び出されます
 */
void CWABPage::OnOK()
{
	UINT update = 0;

	if (np2cfg.wabasw != m_awitch)
	{
		np2cfg.wabasw = m_awitch;
		update |= SYS_UPDATECFG;
	}
	::sysmng_update(update);
}

/**
 * ユーザーがメニューの項目を選択したときに、フレームワークによって呼び出されます
 * @param[in] wParam パラメタ
 * @param[in] lParam パラメタ
 * @retval TRUE アプリケーションがこのメッセージを処理した
 */
BOOL CWABPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDC_WABASWITCH:
			m_awitch = (m_chkawitch.SendMessage(BM_GETCHECK , 0 , 0) ? 1 : 0);
			return TRUE;
	}
	return FALSE;
}

/**
 * CWndProc オブジェクトの Windows プロシージャ (WindowProc) が用意されています
 * @param[in] nMsg 処理される Windows メッセージを指定します
 * @param[in] wParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @param[in] lParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @return メッセージに依存する値を返します
 */
LRESULT CWABPage::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}


/**
 * @brief CL-GD5430 設定ページ
 * @param[in] hwndParent 親ウィンドウ
 */
class CGD5430Page : public CPropPageProc
{
public:
	CGD5430Page();
	virtual ~CGD5430Page();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	UINT8 m_enabled;			//!< 有効フラグ
	UINT8 m_type;				//!< 機種設定
	CWndProc m_chkenabled;		//!< ENABLED
	CComboData m_cmbtype;		//!< 機種
	CWndProc m_btnreset;		//!< RESET
	void SetWABType(UINT8 cValue);
	UINT8 GetWABType() const;
};

/**
 * コンストラクタ
 */
CGD5430Page::CGD5430Page()
	: CPropPageProc(IDD_GD5430)
{
}
/**
 * デストラクタ
 */
CGD5430Page::~CGD5430Page()
{
}

/**
 * 機種リスト
 */
static const CComboData::Entry s_type[] =
{
	{MAKEINTRESOURCE(IDS_GD5430_XE),		0x58},
	{MAKEINTRESOURCE(IDS_GD5430_CB),		0x59},
	{MAKEINTRESOURCE(IDS_GD5430_CF),		0x5A},
	{MAKEINTRESOURCE(IDS_GD5430_XE10),		0x5B},
	{MAKEINTRESOURCE(IDS_GD5430_CB2),		0x5C},
	{MAKEINTRESOURCE(IDS_GD5430_CX2),		0x5D},
};

/**
 * このメソッドは WM_INITDIALOG のメッセージに応答して呼び出されます
 * @retval TRUE 最初のコントロールに入力フォーカスを設定
 * @retval FALSE 既に設定済
 */
BOOL CGD5430Page::OnInitDialog()
{
	m_enabled = np2cfg.usegd5430;
	m_type = np2cfg.gd5430type;

	m_chkenabled.SubclassDlgItem(IDC_GD5430ENABLED, this);
	if(m_enabled)
		m_chkenabled.SendMessage(BM_SETCHECK , BST_CHECKED , 0);
	else
		m_chkenabled.SendMessage(BM_SETCHECK , BST_UNCHECKED , 0);
	
	m_cmbtype.SubclassDlgItem(IDC_GD5430TYPE, this);
	m_cmbtype.Add(s_type, _countof(s_type));
	SetWABType(m_type);
	
	m_cmbtype.SetFocus();

	return FALSE;
}

/**
 * ユーザーが OK のボタン (IDOK ID がのボタン) をクリックすると呼び出されます
 */
void CGD5430Page::OnOK()
{
	UINT update = 0;

	if (np2cfg.usegd5430 != m_enabled
		|| np2cfg.gd5430type != m_type)
	{
		np2cfg.usegd5430 = m_enabled;
		np2cfg.gd5430type = m_type;
		update |= SYS_UPDATECFG;
	}
	::sysmng_update(update);
}

/**
 * ユーザーがメニューの項目を選択したときに、フレームワークによって呼び出されます
 * @param[in] wParam パラメタ
 * @param[in] lParam パラメタ
 * @retval TRUE アプリケーションがこのメッセージを処理した
 */
BOOL CGD5430Page::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDC_GD5430ENABLED:
			m_enabled = (m_chkenabled.SendMessage(BM_GETCHECK , 0 , 0) ? 1 : 0);
			return TRUE;

		case IDC_GD5430TYPE:
			m_type = GetWABType();
			return TRUE;

		case IDC_GD5430DEF:
			m_type = 0x5B;
			SetWABType(m_type);
			return TRUE;
	}
	return FALSE;
}

/**
 * CWndProc オブジェクトの Windows プロシージャ (WindowProc) が用意されています
 * @param[in] nMsg 処理される Windows メッセージを指定します
 * @param[in] wParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @param[in] lParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @return メッセージに依存する値を返します
 */
LRESULT CGD5430Page::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	//switch (nMsg)
	//{
	//}
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}

/**
 * I/O を設定
 * @param[in] cValue 設定
 */
void CGD5430Page::SetWABType(UINT8 cValue)
{
	m_cmbtype.SetCurItemData(cValue);
}

/**
 * I/O を取得
 * @return I/O
 */
UINT8 CGD5430Page::GetWABType() const
{
	return m_cmbtype.GetCurItemData(0x5B);
}


/**
 * コンフィグ ダイアログ
 * @param[in] hwndParent 親ウィンドウ
 */
void dialog_wabopt(HWND hwndParent)
{
	CPropSheetProc prop(IDS_WABOPTION, hwndParent);
	
	CWABPage wab;
	prop.AddPage(&wab);
	
#if defined(SUPPORT_CL_GD5430)
	CGD5430Page gd5430;
	prop.AddPage(&gd5430);
#endif

	prop.m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_USEHICON | PSH_USECALLBACK;
	prop.m_psh.hIcon = LoadIcon(CWndProc::GetResourceHandle(), MAKEINTRESOURCE(IDI_ICON2));
	prop.m_psh.pfnCallback = np2class_propetysheet;
	prop.DoModal();

	InvalidateRect(hwndParent, NULL, TRUE);
}

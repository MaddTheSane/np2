/**
 * @file	d_ide.cpp
 * @brief	IDE 設定ダイアログ
 */

#include "compiler.h"
#include "resource.h"
#include "dialog.h"
#include "c_combodata.h"
#include "np2.h"
#include "commng.h"
#include "sysmng.h"
#include "misc/DlgProc.h"
#include "pccore.h"
#include "common/strres.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif


/**
 * @brief MPU-PC98 設定ダイアログ
 * @param[in] hwndParent 親ウィンドウ
 */
class CIdeDlg : public CDlgProc
{
public:
	CIdeDlg(HWND hwndParent);

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	CComboData m_cmbpm;			//!< プライマリ マスタ
	CComboData m_cmbps;			//!< プライマリ スレーブ
	CComboData m_cmbsm;			//!< セカンダリ マスタ
	CComboData m_cmbss;			//!< セカンダリ スレーブ
};

/**
 * 割り込みリスト
 */
static const CComboData::Entry s_type[] =
{
	{MAKEINTRESOURCE(IDS_IDETYPE_NONE ),		0},
	{MAKEINTRESOURCE(IDS_IDETYPE_HDD  ),		1},
	{MAKEINTRESOURCE(IDS_IDETYPE_CDROM),		2},
};

/**
 * コンストラクタ
 * @param[in] hwndParent 親ウィンドウ
 */
CIdeDlg::CIdeDlg(HWND hwndParent)
	: CDlgProc(IDD_IDE, hwndParent)
{
}

/**
 * このメソッドは WM_INITDIALOG のメッセージに応答して呼び出されます
 * @retval TRUE 最初のコントロールに入力フォーカスを設定
 * @retval FALSE 既に設定済
 */
BOOL CIdeDlg::OnInitDialog()
{
	m_cmbpm.SubclassDlgItem(IDC_IDE1TYPE, this);
	m_cmbpm.Add(s_type, _countof(s_type));
	m_cmbpm.SetCurItemData(np2cfg.idetype[0]);
	
	m_cmbps.SubclassDlgItem(IDC_IDE2TYPE, this);
	m_cmbps.Add(s_type, _countof(s_type));
	m_cmbps.SetCurItemData(np2cfg.idetype[1]);

	m_cmbsm.SubclassDlgItem(IDC_IDE3TYPE, this);
	m_cmbsm.Add(s_type, _countof(s_type));
	m_cmbsm.SetCurItemData(np2cfg.idetype[2]);

	m_cmbss.SubclassDlgItem(IDC_IDE4TYPE, this);
	m_cmbss.Add(s_type, _countof(s_type));
	m_cmbss.SetCurItemData(np2cfg.idetype[3]);

	m_cmbpm.SetFocus();

	return FALSE;
}

/**
 * ユーザーが OK のボタン (IDOK ID がのボタン) をクリックすると呼び出されます
 */
void CIdeDlg::OnOK()
{
	UINT update = 0;

	if (m_cmbpm.GetCurItemData(np2cfg.idetype[0])!=np2cfg.idetype[0])
	{
		np2cfg.idetype[0] = m_cmbpm.GetCurItemData(np2cfg.idetype[0]);
		update |= SYS_UPDATECFG | SYS_UPDATEHDD;
	}
	if (m_cmbps.GetCurItemData(np2cfg.idetype[1])!=np2cfg.idetype[1])
	{
		np2cfg.idetype[1] = m_cmbps.GetCurItemData(np2cfg.idetype[1]);
		update |= SYS_UPDATECFG | SYS_UPDATEHDD;
	}
	if (m_cmbsm.GetCurItemData(np2cfg.idetype[2])!=np2cfg.idetype[2])
	{
		np2cfg.idetype[2] = m_cmbsm.GetCurItemData(np2cfg.idetype[2]);
		update |= SYS_UPDATECFG | SYS_UPDATEHDD;
	}
	if (m_cmbss.GetCurItemData(np2cfg.idetype[3])!=np2cfg.idetype[3])
	{
		np2cfg.idetype[3] = m_cmbss.GetCurItemData(np2cfg.idetype[3]);
		update |= SYS_UPDATECFG | SYS_UPDATEHDD;
	}
	sysmng_update(update);

	CDlgProc::OnOK();
}

/**
 * ユーザーがメニューの項目を選択したときに、フレームワークによって呼び出されます
 * @param[in] wParam パラメタ
 * @param[in] lParam パラメタ
 * @retval TRUE アプリケーションがこのメッセージを処理した
 */
BOOL CIdeDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDC_IDE1TYPE:
		case IDC_IDE2TYPE:
		case IDC_IDE3TYPE:
		case IDC_IDE4TYPE:
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
LRESULT CIdeDlg::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}

/**
 * コンフィグ ダイアログ
 * @param[in] hwndParent 親ウィンドウ
 */
void dialog_ideopt(HWND hwndParent)
{
	CIdeDlg dlg(hwndParent);
	dlg.DoModal();
}

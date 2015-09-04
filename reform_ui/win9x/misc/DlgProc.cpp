/**
 * @file	DlgProc.cpp
 * @brief	ダイアログ クラスの動作の定義を行います
 */

#include "compiler.h"
#include "DlgProc.h"

/**
 * コンストラクタ
 */
CDlgProc::CDlgProc()
	: m_lpszTemplateName(NULL)
	, m_hwndParent(NULL)
{
}

/**
 * コンストラクタ
 * @param[in] nIDTemplate ダイアログ ボックス テンプレートのリソース id 番号を指定します
 * @param[in] hwndParent 親ウィンドウ
 */
CDlgProc::CDlgProc(UINT nIDTemplate, HWND hwndParent)
	: m_lpszTemplateName(MAKEINTRESOURCE(nIDTemplate))
	, m_hwndParent(hwndParent)
{
}

/**
 * デストラクタ
 */
CDlgProc::~CDlgProc()
{
}

/**
 * モーダル
 * @return ダイアログ ボックスを閉じるために使用される、CDialog::EndDialog のメンバー関数に渡された nResult のパラメーター値を指定する int の値
 */
INT_PTR CDlgProc::DoModal()
{
	HookWindowCreate(this);

	const INT_PTR nRet = ::DialogBox(GetInstanceHandle(), m_lpszTemplateName, m_hwndParent, DlgProc);

	if (!UnhookWindowCreate())
	{
		PostNcDestroy();
	}
	return nRet;
}

/**
 * ダイアログ プロシージャ
 * @param[in] hWnd ウィンドウ ハンドル
 * @param[in] message 処理される Windows メッセージを指定します
 * @param[in] wParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @param[in] lParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @return メッセージに依存する値を返します
 */
BOOL CALLBACK CDlgProc::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_INITDIALOG)
	{
		CDlgProc* pDlg = static_cast<CDlgProc*>(FromHandlePermanent(hWnd));
		if (pDlg != NULL)
		{
			return pDlg->OnInitDialog();
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

/**
 * CDlgProc オブジェクトの Windows プロシージャ
 * @param[in] nMsg 処理される Windows メッセージを指定します
 * @param[in] wParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @param[in] lParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @return メッセージに依存する値を返します
 */
LRESULT CDlgProc::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (nMsg == WM_COMMAND)
	{
		switch (LOWORD(wParam))
		{
			case IDOK:
				OnOK();
				return TRUE;

			case IDCANCEL:
				OnCancel();
				return TRUE;
		}
	}
	return CWndProc::WindowProc(nMsg, wParam, lParam);
}

/**
 * このメソッドは WM_INITDIALOG のメッセージに応答して呼び出されます
 * @retval TRUE 最初のコントロールに入力フォーカスを設定
 * @retval FALSE 既に設定済
 */
BOOL CDlgProc::OnInitDialog()
{
	return TRUE;
}

/**
 * ユーザーが OK のボタン (IDOK ID がのボタン) をクリックすると呼び出されます
 */
void CDlgProc::OnOK()
{
	EndDialog(IDOK);
}

/**
 * フレームワークは、ユーザーが [キャンセル] をクリックするか、モーダルまたはモードレス ダイアログ ボックスの Esc キーを押したときにこのメソッドを呼び出します
 */
void CDlgProc::OnCancel()
{
	EndDialog(IDCANCEL);
}

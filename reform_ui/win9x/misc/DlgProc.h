/**
 * @file	DlgProc.h
 * @brief	ダイアログ クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "WndProc.h"

/**
 * @brief ダイアログ クラス
 */
class CDlgProc : public CWndProc
{
public:
	CDlgProc();
	CDlgProc(UINT nIDTemplate, HWND hwndParent = NULL);
	virtual ~CDlgProc();
	virtual INT_PTR DoModal();
	virtual BOOL OnInitDialog();
	void EndDialog(int nResult);

protected:
	LPCTSTR m_lpszTemplateName;		//!< テンプレート名
	HWND m_hwndParent;				//!< 親ウィンドウ

	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();

	static BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

/**
 * モーダル ダイアログ ボックスを終了する
 * @param[in] nResult DoModalの呼び出し元に返す値
 */
inline void CDlgProc::EndDialog(int nResult)
{
	::EndDialog(m_hWnd, nResult);
}

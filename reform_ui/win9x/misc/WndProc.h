/**
 * @file	WndProc.h
 * @brief	プロシージャ クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "WndBase.h"

/**
 * @brief プロシージャ クラス
 */
class CWndProc : public CWndBase
{
public:
	static void Initialize(HINSTANCE hInstance);
	static void SetResourceHandle(HINSTANCE hInstance);
	static HINSTANCE GetResourceHandle();

	CWndProc();
	virtual ~CWndProc();
	operator HWND() const;
	HWND GetSafeHwnd() const;
	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu);
	BOOL DestroyWindow();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	static HINSTANCE sm_hInstance;		//!< インスタンス ハンドル
	static HINSTANCE sm_hResource;		//!< リソース ハンドル
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

/**
 * リソース ハンドルを設定
 * @param[in] hInstance リソース ハンドル
 */
inline void CWndProc::SetResourceHandle(HINSTANCE hInstance)
{
	sm_hResource = hInstance;
}

/**
 * リソース ハンドルを取得
 * @return リソース ハンドル
 */
inline HINSTANCE CWndProc::GetResourceHandle()
{
	return sm_hResource;
}

/**
 * HWND オペレータ
 * @return HWND
 */
inline CWndProc::operator HWND() const
{
	return m_hWnd;
}

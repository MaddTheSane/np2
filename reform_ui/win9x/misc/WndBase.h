/**
 * @file	WndBase.h
 * @brief	ウィンドウ基底クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

/**
 * @brief ウィンドウ基底クラス
 */
class CWndBase
{
public:
	HWND m_hWnd;            /*!< must be first data member */

public:
	static void Initialize(HINSTANCE hInstance);
	static void SetResourceHandle(HINSTANCE hInstance);
	static HINSTANCE GetResourceHandle();

	CWndBase();
	virtual ~CWndBase();
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
inline void CWndBase::SetResourceHandle(HINSTANCE hInstance)
{
	sm_hResource = hInstance;
}

/**
 * リソース ハンドルを取得
 * @return リソース ハンドル
 */
inline HINSTANCE CWndBase::GetResourceHandle()
{
	return sm_hResource;
}

/**
 * HWND オペレータ
 * @return HWND
 */
inline CWndBase::operator HWND() const
{
	return m_hWnd;
}

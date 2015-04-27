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
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

/**
 * HWND オペレータ
 * @return HWND
 */
inline CWndBase::operator HWND() const
{
	return m_hWnd;
}

/**
 * @file	WndBase.cpp
 * @brief	ウィンドウ基底クラスの動作の定義を行います
 */

#include "compiler.h"
#include "WndBase.h"
#include "..\resource.h"

//! インスタンス
HINSTANCE CWndBase::sm_hInstance;

//! クラス名
static const TCHAR s_szClassName[] = TEXT("WndBase");

/**
 * 初期化
 * @param[in] hInstance インスタンス
 */
void CWndBase::Initialize(HINSTANCE hInstance)
{
	sm_hInstance = hInstance;

	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(NULL_BRUSH));
	wc.lpszClassName = s_szClassName;
	::RegisterClass(&wc);
}

/**
 * コンストラクタ
 */
CWndBase::CWndBase()
	: m_hWnd(NULL)
{
}

/**
 * デストラクタ
 */
CWndBase::~CWndBase()
{
	DestroyWindow();
}

/**
 * dwExStyle で指定した拡張スタイルで、オーバーラップ ウィンドウ、ポップアップ ウィンドウ、または子ウィンドウを作成します
 * @param[in] lpszWindowName ウィンドウ名を持つ NULL で終わる文字列へのポインタ
 * @param[in] dwStyle ウィンドウのスタイル属性を指定します
 * @param[in] x CWndBase ウィンドウの初期 x 位置を指定します
 * @param[in] y CWndBase ウィンドウの初期 y 位置を指定します
 * @param[in] nWidth CWndBase ウィンドウの幅を (デバイス単位で) 指定します
 * @param[in] nHeight CWndBase ウィンドウの高さを (デバイス単位で) 指定します
 * @param[in] hwndParent 作成される CWnd ウィンドウの親ウィンドウまたはオーナー ウィンドウを指定します。NULL を指定すると、トップレベルのウィンドウになります。
 * @param[in] nIDorHMenu メニューまたは子メニューの識別子を指定します。ウィンドウのスタイルによって意味が異なります
 * @return 正常終了した場合は 0 以外を返します。それ以外の場合は 0 を返します
 */
BOOL CWndBase::Create(LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu)
{
	const HWND hWnd = ::CreateWindow(s_szClassName, lpszWindowName, dwStyle, x, y, nWidth, nHeight, hwndParent, nIDorHMenu, sm_hInstance, this);
	return (hWnd != NULL) ? TRUE : FALSE;
}

/**
 * CWndBase オブジェクトに関連付けられた Windows のウィンドウを破棄します
 * @return ウィンドウが破棄された場合は 0 以外を返します。それ以外の場合は 0 を返します
 */
BOOL CWndBase::DestroyWindow()
{
	if (!m_hWnd)
	{
		return FALSE;
	}
	return ::DestroyWindow(m_hWnd);
}

/**
 * ウィンドウ プロシージャ
 * @param[in] hWnd ウィンドウ ハンドル
 * @param[in] message 処理される Windows メッセージを指定します
 * @param[in] wParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @param[in] lParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @return メッセージに依存する値を返します
 */
LRESULT CALLBACK CWndBase::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CWndBase* pWnd = NULL;
	if (message == WM_CREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pWnd = static_cast<CWndBase*>(pCreate->lpCreateParams);
		pWnd->m_hWnd = hWnd;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
	}
	else
	{
		pWnd = reinterpret_cast<CWndBase*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	if (pWnd == NULL)
	{
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}

	LRESULT lResult = pWnd->WindowProc(message, wParam, lParam);

	if (message == WM_NCDESTROY)
	{
		pWnd->m_hWnd = NULL;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
	}

	return lResult;
}

/**
 * CWndBase オブジェクトの Windows プロシージャ (WindowProc) が用意されています
 * @param[in] message 処理される Windows メッセージを指定します
 * @param[in] wParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @param[in] lParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @return メッセージに依存する値を返します
 */
LRESULT CWndBase::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(message, wParam, lParam);
}

/**
 * 既定のウィンドウ プロシージャを呼び出します
 * @param[in] message 処理される Windows メッセージを指定します
 * @param[in] wParam メッセージ依存の追加情報を指定します
 * @param[in] lParam メッセージ依存の追加情報を指定します
 * @return 送られたメッセージに依存します
 */
LRESULT CWndBase::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProc(m_hWnd, message, wParam, lParam);
}

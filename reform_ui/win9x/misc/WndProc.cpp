/**
 * @file	WndProc.cpp
 * @brief	プロシージャ クラスの動作の定義を行います
 */

#include "compiler.h"
#include "WndProc.h"
#include "..\resource.h"

//! インスタンス
HINSTANCE CWndProc::sm_hInstance;
//! リソース
HINSTANCE CWndProc::sm_hResource;

//! クラス名
static const TCHAR s_szClassName[] = TEXT("WndProc");

/**
 * 初期化
 * @param[in] hInstance インスタンス
 */
void CWndProc::Initialize(HINSTANCE hInstance)
{
	sm_hInstance = hInstance;
	sm_hResource = hInstance;

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
CWndProc::CWndProc()
	: CWndBase(NULL)
{
}

/**
 * デストラクタ
 */
CWndProc::~CWndProc()
{
	DestroyWindow();
}

/**
 * dwExStyle で指定した拡張スタイルで、オーバーラップ ウィンドウ、ポップアップ ウィンドウ、または子ウィンドウを作成します
 * @param[in] lpszWindowName ウィンドウ名を持つ NULL で終わる文字列へのポインタ
 * @param[in] dwStyle ウィンドウのスタイル属性を指定します
 * @param[in] x CWndProc ウィンドウの初期 x 位置を指定します
 * @param[in] y CWndProc ウィンドウの初期 y 位置を指定します
 * @param[in] nWidth CWndProc ウィンドウの幅を (デバイス単位で) 指定します
 * @param[in] nHeight CWndProc ウィンドウの高さを (デバイス単位で) 指定します
 * @param[in] hwndParent 作成される CWnd ウィンドウの親ウィンドウまたはオーナー ウィンドウを指定します。NULL を指定すると、トップレベルのウィンドウになります。
 * @param[in] nIDorHMenu メニューまたは子メニューの識別子を指定します。ウィンドウのスタイルによって意味が異なります
 * @return 正常終了した場合は 0 以外を返します。それ以外の場合は 0 を返します
 */
BOOL CWndProc::Create(LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu)
{
	const HWND hWnd = ::CreateWindow(s_szClassName, lpszWindowName, dwStyle, x, y, nWidth, nHeight, hwndParent, nIDorHMenu, sm_hInstance, this);
	return (hWnd != NULL) ? TRUE : FALSE;
}

/**
 * CWndProc オブジェクトに関連付けられた Windows のウィンドウを破棄します
 * @return ウィンドウが破棄された場合は 0 以外を返します。それ以外の場合は 0 を返します
 */
BOOL CWndProc::DestroyWindow()
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
LRESULT CALLBACK CWndProc::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	CWndProc* pWnd = NULL;
	if (message == WM_CREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pWnd = static_cast<CWndProc*>(pCreate->lpCreateParams);
		pWnd->m_hWnd = hWnd;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
	}
	else
	{
		pWnd = reinterpret_cast<CWndProc*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
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
 * CWndProc オブジェクトの Windows プロシージャ (WindowProc) が用意されています
 * @param[in] message 処理される Windows メッセージを指定します
 * @param[in] wParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @param[in] lParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @return メッセージに依存する値を返します
 */
LRESULT CWndProc::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
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
LRESULT CWndProc::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProc(m_hWnd, message, wParam, lParam);
}

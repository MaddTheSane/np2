/**
 * @file	subwnd.cpp
 * @brief	サブ ウィンドウの基底クラスの動作の定義を行います
 */

#include "compiler.h"
#include "resource.h"
#include "subwnd.h"
#include "np2.h"
#include "winloc.h"
#include "np2class.h"

extern WINLOCEX np2_winlocexallwin(HWND base);

//! クラス名
static const TCHAR s_szClassName[] = TEXT("NP2-SubWnd");

/**
 * 初期化
 * @param[in] hInstance インスタンス
 */
void CSubWndBase::Initialize(HINSTANCE hInstance)
{
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = ::DefWindowProc;
	wc.cbWndExtra = NP2GWLP_SIZE;
	wc.hInstance = hInstance;
	wc.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(NULL_BRUSH));
	wc.lpszClassName = s_szClassName;
	RegisterClass(&wc);
}

/**
 * コンストラクタ
 */
CSubWndBase::CSubWndBase()
{
}

/**
 * デストラクタ
 */
CSubWndBase::~CSubWndBase()
{
}

/**
 * ウィンドウ作成
 * 
 */
BOOL CSubWndBase::Create(LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu)
{
	return CreateEx(0, s_szClassName, lpszWindowName, dwStyle, x, y, nWidth, nHeight, hwndParent, nIDorHMenu);
}

/**
 * ウィンドウ タイプの設定
 * @param[in] nType タイプ
 */
void CSubWndBase::SetWndType(UINT8 nType)
{
	WINLOCEX wlex = ::np2_winlocexallwin(g_hWndMain);
	winlocex_setholdwnd(wlex, m_hWnd);
	np2class_windowtype(m_hWnd, nType);
	winlocex_move(wlex);
	winlocex_destroy(wlex);
}

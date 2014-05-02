/**
 *  @file	tawnd.h
 *	@brief	Tool-assisted ウィンドウ クラスの動作の定義を行います
 */

#include "compiler.h"
#include "tawnd.h"
#include "np2.h"
#include "resource.h"
#include "joymng.h"
#include "mousemng.h"
#include "recvideo.h"
#include "pccore.h"

//! 唯一のインスタンスです
CTAWnd CTAWnd::sm_instance;

/**
 * コンストラクタ
 */
CTAWnd::CTAWnd()
	: m_nFrames(0)
	, m_nExecFrames(0)
{
}

/**
 * 作成
 * @retval true 成功
 * @retval false 失敗
 */
bool CTAWnd::Create()
{
	const BOOL r = CWndBase::Create(TEXT("Tool assisted"), WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 200, 80, NULL, NULL);
	if (r)
	{
		::ShowWindow(*this, SW_NORMAL);
		::UpdateWindow(*this);
	}
	return (r) ? true : false;
}

/**
 * CWndBase オブジェクトの Windows プロシージャ (WindowProc) が用意されています
 * @param[in] message 処理される Windows メッセージを指定します
 * @param[in] wParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @param[in] lParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @return メッセージに依存する値を返します
 */
LRESULT CTAWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_KEYDOWN)
	{
		if ((wParam >= VK_F1) && (wParam <= VK_F10))
		{
			UINT nID = (::GetKeyState(VK_SHIFT) < 0) ? IDM_FLAGSAVE : IDM_FLAGLOAD;
			nID += (wParam - VK_F1);
			::SendMessage(g_hWndMain, WM_COMMAND, nID, 0);
		}
		else if ((wParam >= '0') && (wParam <= '9'))
		{
			int nFrames = wParam - '0';
			if (nFrames == 0)
			{
				nFrames = 10;
			}
			if (::GetKeyState(VK_SHIFT) < 0)
			{
				nFrames *= 10;
			}
			m_nExecFrames += nFrames;
		}
	}
	else if (message == WM_PAINT)
	{
		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(*this, &ps);

		::FillRect(hdc, &rcClient, static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH)));

		::SetTextColor(hdc, RGB(0, 0, 0));
		::SetBkMode(hdc, TRANSPARENT);

		TCHAR szText[16];
		wsprintf(szText, TEXT("%6d"), m_nFrames);
		::DrawText(hdc, szText, -1, &rcClient, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		::EndPaint(*this, &ps);
	}

	return CWndBase::WindowProc(message, wParam, lParam);
}

/**
 * 実行
 * @retval true アクティブ
 * @retval false 非アクティブ
 */
bool CTAWnd::execute()
{
	if (m_hWnd == NULL)
	{
		return false;
	}

	while (m_nExecFrames)
	{
		m_nExecFrames--;
		joymng_sync();
		mousemng_sync();
		pccore_exec(TRUE);
		recvideo_write();
	}

	if (m_nFrames != pccore.frames)
	{
		m_nFrames = pccore.frames;
		::InvalidateRect(*this, NULL, TRUE);
	}

	return (::GetForegroundWindow() == *this);
}

/**
 * @file	viewer.cpp
 * @brief	DebugUty 用ビューワ クラスの動作の定義を行います
 */

#include "compiler.h"
#include "resource.h"
#include "np2.h"
#include "viewer.h"
#include "viewcmn.h"
#include "cpucore.h"

		const TCHAR		np2viewfont[] = _T("ＭＳ ゴシック");
static CDebugUtyView* g_np2view[NP2VIEW_MAX];

//! ビュー クラス名
static const TCHAR s_szViewClass[] = TEXT("NP2-ViewWindow");

//! 最後のTick
DWORD CDebugUtyView::sm_dwLastTick;

//! チェック マクロ
#define MFCHECK(bChecked) ((bChecked) ? MF_CHECKED : MF_UNCHECKED)

/**
 * 初期化
 * @param[in] hInstance インスタンス
 */
void CDebugUtyView::Initialize(HINSTANCE hInstance)
{
	sm_dwLastTick = ::GetTickCount();

	ZeroMemory(g_np2view, sizeof(g_np2view));

	WNDCLASS np2vc;
	np2vc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	np2vc.lpfnWndProc = ::DefWindowProc;
	np2vc.cbClsExtra = 0;
	np2vc.cbWndExtra = 0;
	np2vc.hInstance = hInstance;
	np2vc.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	np2vc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	np2vc.hbrBackground = static_cast<HBRUSH>(NULL);
	np2vc.lpszMenuName = MAKEINTRESOURCE(IDR_VIEW);
	np2vc.lpszClassName = s_szViewClass;
	::RegisterClass(&np2vc);
}

/**
 * 新しいウィンドウを作成する
 */
void CDebugUtyView::New()
{
	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		CDebugUtyView* lpView = g_np2view[i];
		if (lpView != NULL)
		{
			continue;
		}

		CDebugUtyView* view = new CDebugUtyView;
		if (view->CreateEx(0, s_szViewClass, NULL, WS_OVERLAPPEDWINDOW | WS_VSCROLL, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, 0))
		{
			g_np2view[i] = view;
			viewcmn_setmode(view, NULL, VIEWMODE_REG);
			view->UpdateCaption();
			view->ShowWindow(SW_SHOWNORMAL);
			view->UpdateWindow();
		}
		break;
	}
}

/**
 * すべて閉じる
 */
void CDebugUtyView::AllClose()
{
	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		CDebugUtyView* lpView = g_np2view[i];
		if (lpView != NULL)
		{
			lpView->DestroyWindow();
		}
	}
}

/**
 * すべて更新
 * @param[in] bForce 強制的に更新する
 */
void CDebugUtyView::AllUpdate(bool bForce)
{
	const DWORD dwNow  = ::GetTickCount();
	if ((!bForce) || ((dwNow - sm_dwLastTick) >= 200))
	{
		sm_dwLastTick = dwNow;

		for (size_t i = 0; i < _countof(g_np2view); i++)
		{
			CDebugUtyView* lpView = g_np2view[i];
			if (lpView != NULL)
			{
				lpView->UpdateView();
			}
		}
	}
}

/**
 * コンストラクタ
 */
CDebugUtyView::CDebugUtyView()
	: m_bActive(false)
	, m_nVPos(0)
	, m_nVLines(0)
	, m_nVPage(0)
	, m_nVMultiple(1)
	, type(0)
	, lock(0)
	, seg(0)
	, off(0)
{
	ZeroMemory(&this->buf1, sizeof(this->buf1));
	ZeroMemory(&this->buf2, sizeof(this->buf2));
	ZeroMemory(&this->dmem, sizeof(this->dmem));
}

/**
 * デストラクタ
 */
CDebugUtyView::~CDebugUtyView()
{
	viewcmn_free(&this->buf1);
	viewcmn_free(&this->buf2);

	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		if (g_np2view[i] == this)
		{
			g_np2view[i] = NULL;
			UpdateActive();
			break;
		}
	}
}

/**
 * キャプションの更新
 */
void CDebugUtyView::UpdateCaption()
{
	int nIndex = -1;
	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		if (g_np2view[i] == this)
		{
			nIndex = static_cast<int>(i);
			break;
		}
	}
	LPCTSTR lpMode = (this->lock) ? TEXT("Locked") : TEXT("Realtime");

	TCHAR szTitle[256];
	wsprintf(szTitle, TEXT("%d.%s - NP2 Debug Utility"), nIndex + 1, lpMode);

	SetWindowText(szTitle);
}

/**
 * V スクロール位置の設定
 * @param[in] nPos 新しい位置
 */
void CDebugUtyView::SetVScrollPos(UINT nPos)
{
	if (m_nVPos != nPos)
	{
		m_nVPos = nPos;
		UpdateVScroll();
		Invalidate();
	}
}

/**
 * V スクロールの設定
 * @param[in] nPos 新しい位置
 * @param[in] nLines ライン数
 */
void CDebugUtyView::SetVScroll(UINT nPos, UINT nLines)
{
	if ((m_nVPos != nPos) || (m_nVLines != nLines))
	{
		m_nVPos = nPos;
		m_nVLines = nLines;
		m_nVMultiple = ((nLines - 1) / 0xFFFF) + 1;
		UpdateVScroll();
		Invalidate();
	}
}

/**
 * V スクロールバーの更新
 */
void CDebugUtyView::UpdateVScroll()
{
	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = ((m_nVLines + m_nVMultiple - 1) / m_nVMultiple) - 1;
	si.nPos = m_nVPos / m_nVMultiple;
	si.nPage = m_nVPage / m_nVMultiple;
	SetScrollInfo(SB_VERT, &si, TRUE);
}

/**
 * ウィンドウ プロシージャ
 * @param[in] message メッセージ
 * @param[in] wParam パラメタ
 * @param[in] lParam パラメタ
 * @return リザルト コード
 */
LRESULT CDebugUtyView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// NP2VIEW_T* view = this;
	// UINT msg = message;
	// HWND hWnd = *this;

	switch (message)
	{
		case WM_CREATE:
			break;

		case WM_COMMAND:
			if (!OnCommand(wParam, lParam))
			{
				return viewcmn_dispat(this, message, wParam, lParam);
			}
			break;

		case WM_PAINT:
			return viewcmn_dispat(this, message, wParam, lParam);

		case WM_SIZE:
			{
				RECT rc;
				GetClientRect(&rc);
				m_nVPage = rc.bottom / 16;
				UpdateVScroll();
			}
			break;

		case WM_VSCROLL:
			OnVScroll(LOWORD(wParam), HIWORD(wParam), reinterpret_cast<HWND>(lParam));
			break;

		case WM_ENTERMENULOOP:
			OnEnterMenuLoop(static_cast<BOOL>(wParam));
			break;

		case WM_ACTIVATE:
			m_bActive = (LOWORD(wParam) != WA_INACTIVE);
			UpdateActive();
			break;

		case WM_CLOSE:
			DestroyWindow();
			break;

		default:
			return CWndProc::WindowProc(message, wParam, lParam);
	}
	return 0;
}

/**
 * ユーザーがメニューの項目を選択したときに、フレームワークによって呼び出されます
 * @param[in] wParam パラメタ
 * @param[in] lParam パラメタ
 * @retval TRUE アプリケーションがこのメッセージを処理した
 */
BOOL CDebugUtyView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDM_VIEWWINNEW:
			New();
			break;

		case IDM_VIEWWINCLOSE:
			DestroyWindow();
			break;

		case IDM_VIEWWINALLCLOSE:
			AllClose();
			break;

		case IDM_VIEWMODEREG:
			SetMode(VIEWMODE_REG);
			break;

		case IDM_VIEWMODESEG:
			SetMode(VIEWMODE_SEG);
			break;

		case IDM_VIEWMODE1MB:
			SetMode(VIEWMODE_1MB);
			break;

		case IDM_VIEWMODEASM:
			SetMode(VIEWMODE_ASM);
			break;

		case IDM_VIEWMODESND:
			SetMode(VIEWMODE_SND);
			break;

		default:
			return FALSE;
	}
	return TRUE;
}

/**
 * フレームワークは、ユーザーがウィンドウに垂直スクロール バーをクリックすると、このメンバー関数を呼び出します
 * @param[in] nSBCodeユーザーの要求を示すスクロール バー コードを指定します
 * @param[in] nPos 現在のスクロール ボックスの位置
 * @param[in] hwndScrollBar スクロール バー コントロール
 */
void CDebugUtyView::OnVScroll(UINT nSBCode, UINT nPos, HWND hwndScrollBar)
{
	UINT32 nNewPos = m_nVPos;
	switch (nSBCode)
	{
		case SB_LINEUP:
			if (nNewPos)
			{
				nNewPos--;
			}
			break;

		case SB_LINEDOWN:
			if ((nNewPos + m_nVPage) < m_nVLines)
			{
				nNewPos++;
			}
			break;

		case SB_PAGEUP:
			if (nNewPos > m_nVPage)
			{
				nNewPos -= m_nVPage;
			}
			else
			{
				nNewPos = 0;
			}
			break;

		case SB_PAGEDOWN:
			nNewPos += m_nVPage;
			if (nNewPos > (m_nVLines - m_nVPage))
			{
				nNewPos = m_nVLines - m_nVPage;
			}
			break;

		case SB_THUMBTRACK:
			nNewPos = nPos * m_nVMultiple;
			break;
	}
	SetVScrollPos(nNewPos);
}

/**
 * The framework calls this member function when a menu modal loop has been entered
 * @param[in] bIsTrackPopupMenu Specifies whether the menu involved is a popup menu
 */
void CDebugUtyView::OnEnterMenuLoop(BOOL bIsTrackPopupMenu)
{
	HMENU hMenu = GetMenu();
	if (hMenu == NULL)
	{
		return;
	}
	::CheckMenuItem(hMenu, IDM_VIEWMODELOCK, MF_BYCOMMAND | MFCHECK(this->lock));
	::CheckMenuItem(hMenu, IDM_VIEWMODEREG, MF_BYCOMMAND | MFCHECK(this->type == VIEWMODE_REG));
	::CheckMenuItem(hMenu, IDM_VIEWMODESEG, MF_BYCOMMAND | MFCHECK(this->type == VIEWMODE_SEG));
	::CheckMenuItem(hMenu, IDM_VIEWMODE1MB, MF_BYCOMMAND | MFCHECK(this->type == VIEWMODE_1MB));
	::CheckMenuItem(hMenu, IDM_VIEWMODEASM, MF_BYCOMMAND | MFCHECK(this->type == VIEWMODE_ASM));
	::CheckMenuItem(hMenu, IDM_VIEWMODESND, MF_BYCOMMAND | MFCHECK(this->type == VIEWMODE_SND));

	HMENU hSubMenu = ::GetSubMenu(hMenu, 2);
	if (hSubMenu)
	{
		SetSegmentItem(hSubMenu, IDM_SEGCS, TEXT("CS"), CPU_CS);
		SetSegmentItem(hSubMenu, IDM_SEGDS, TEXT("DS"), CPU_DS);
		SetSegmentItem(hSubMenu, IDM_SEGES, TEXT("ES"), CPU_ES);
		SetSegmentItem(hSubMenu, IDM_SEGSS, TEXT("SS"), CPU_SS);
		DrawMenuBar();
	}
}

/**
 * Called by the default OnNcDestroy member function after the window has been destroyed.
 */
void CDebugUtyView::PostNcDestroy()
{
	delete this;
}

/**
 * モード変更
 * @param[in] type タイプ
 */
void CDebugUtyView::SetMode(UINT8 type)
{
	if (this->type != type)
	{
		viewcmn_setmode(this, this, type);
		this->dmem.Update();
		UpdateVScroll();
		Invalidate();
	}
}

/**
 * ビュー更新
 */
void CDebugUtyView::UpdateView()
{
	if (!this->lock)
	{
		if (this->type == VIEWMODE_ASM)
		{
			this->seg = CPU_CS;
			this->off = CPU_IP;
			m_nVPos = 0;
			UpdateVScroll();
		}
		this->dmem.Update();
		Invalidate();
	}
}

/**
 * メニュー アイテムを更新
 * @param[in] hMenu メニュー ハンドル
 * @param[in] hId メニュー ID
 * @param[in] lpSegment セグメント名
 * @param[in] nSegment セグメント値
 */
void CDebugUtyView::SetSegmentItem(HMENU hMenu, int nId, LPCTSTR lpSegment, UINT nSegment)
{
	TCHAR szString[32];
	wsprintf(szString, TEXT("Seg = &%s [%04x]"), lpSegment, nSegment);
	::ModifyMenu(hMenu, nId, MF_BYCOMMAND | MF_STRING, nId, szString);
}

/**
 * アクティブ フラグを更新
 */
void CDebugUtyView::UpdateActive()
{
	bool bActive = false;
	for (size_t i = 0; i < _countof(g_np2view); i++)
	{
		const CDebugUtyView* lpView = g_np2view[i];
		if ((lpView != NULL) && (lpView->m_bActive))
		{
			bActive = true;
		}
	}

	if (bActive)
	{
		np2break |= NP2BREAK_DEBUG;
	}
	else
	{
		np2break &= ~NP2BREAK_DEBUG;
	}
	np2active_renewal();
}

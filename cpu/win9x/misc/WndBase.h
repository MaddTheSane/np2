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

	CWndBase(HWND hWnd = NULL);
	CWndBase& operator=(HWND hWnd);
	void Attach(HWND hWnd);
	HWND Detach();
//	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu);
//	BOOL DestroyWindow();

	// Attributes
	operator HWND() const;

	// Window Text Functions
	BOOL SetWindowText(LPCTSTR lpString);

	// Menu Functions (non-child windows only)
	HMENU GetMenu() const;
	BOOL DrawMenuBar();

	// Window Size and Position Functions
	BOOL GetWindowRect(LPRECT lpRect) const;
	BOOL GetClientRect(LPRECT lpRect) const;

	// Update and Painting Functions
	HDC BeginPaint(LPPAINTSTRUCT lpPaint);
	void EndPaint(LPPAINTSTRUCT lpPaint);
	BOOL UpdateWindow();
	BOOL Invalidate(BOOL bErase = TRUE);
	BOOL InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE);
	BOOL ShowWindow(int nCmdShow);

	// Misc. Operations
	int SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE);
};

/**
 * コンストラクタ
 * @param[in] hWnd ウィンドウ ハンドル
 */
inline CWndBase::CWndBase(HWND hWnd)
	: m_hWnd(hWnd)
{
}

/**
 * オペレータ
 * @param[in] hWnd ウィンドウ ハンドル
 * @return インスタンス
 */
inline CWndBase& CWndBase::operator=(HWND hWnd)
{
	m_hWnd = hWnd;
	return *this;
}

/**
 * アタッチ
 * @param[in] hWnd ウィンドウ ハンドル
 */
inline void CWndBase::Attach(HWND hWnd)
{
	m_hWnd = hWnd;
}

/**
 * デタッチ
 * @return 以前のインスタンス
 */
inline HWND CWndBase::Detach()
{
	HWND hWnd = m_hWnd;
	m_hWnd = NULL;
	return hWnd;
}

/**
 * HWND オペレータ
 * @return HWND
 */
inline CWndBase::operator HWND() const
{
	return m_hWnd;
}

/**
 * 指定されたウィンドウのタイトルバーのテキストを変更します
 * @param[in] lpString 新しいウィンドウタイトルまたはコントロールのテキストとして使われる、NULL で終わる文字列へのポインタを指定します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::SetWindowText(LPCTSTR lpString)
{
	return ::SetWindowText(m_hWnd, lpString);
}


/**
 * 指定されたウィンドウに割り当てられているメニューのハンドルを取得します
 * @return メニューのハンドルが返ります
 */
inline HMENU CWndBase::GetMenu() const
{
	return ::GetMenu(m_hWnd);
}

/**
 * 指定されたウィンドウのメニューバーを再描画します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::DrawMenuBar()
{
	return ::DrawMenuBar(m_hWnd);
}

/**
 * 指定されたウィンドウの左上端と右下端の座標をスクリーン座標で取得します
 * @param[out] lpRect 構造体へのポインタを指定します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::GetWindowRect(LPRECT lpRect) const
{
	return ::GetWindowRect(m_hWnd, lpRect);
}

/**
 * lpRectが指す構造に CWnd のクライアント領域のクライアント座標をコピーします
 * @param[out] lpRect 構造体へのポインタを指定します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::GetClientRect(LPRECT lpRect) const
{
	return ::GetClientRect(m_hWnd, lpRect);
}

/**
 * 描画を開始します
 * @param[out] lpPaint 描画情報へのポインタを指定します
 * @return デバイス コンテキスト
 */
inline HDC CWndBase::BeginPaint(LPPAINTSTRUCT lpPaint)
{
	return ::BeginPaint(m_hWnd, lpPaint);
}

/**
 * 描画の終了します
 * @param[in] lpPaint 描画情報へのポインタを指定します
 */
inline void CWndBase::EndPaint(LPPAINTSTRUCT lpPaint)
{
	::EndPaint(m_hWnd, lpPaint);
}

/**
 * 指定されたウィンドウの更新リージョンが空ではない場合、ウィンドウへ メッセージを送信し、そのウィンドウのクライアント領域を更新します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::UpdateWindow()
{
	return ::UpdateWindow(m_hWnd);
}

/**
 * 指定されたウィンドウのすべてを更新リージョンにします
 * @param[in] bErase 更新リージョンを処理するときに、更新リージョン内の背景を消去するかどうかを指定します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::Invalidate(BOOL bErase)
{
	return ::InvalidateRect(m_hWnd, NULL, bErase);
}

/**
 * 指定されたウィンドウの更新リージョンに1個の長方形を追加します
 * @param[in] lpRect 更新リージョンへ追加したい長方形のクライアント座標を保持する1個の構造体へのポインタを指定します
 * @param[in] bErase 更新リージョンを処理するときに、更新リージョン内の背景を消去するかどうかを指定します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::InvalidateRect(LPCRECT lpRect, BOOL bErase)
{
	return ::InvalidateRect(m_hWnd, lpRect, bErase);
}

/**
 * 指定されたウィンドウの表示状態を設定します
 * @param[in] nCmdShow ウィンドウの表示方法を指定します
 * @retval TRUE 成功
 * @retval FALSE 失敗
 */
inline BOOL CWndBase::ShowWindow(int nCmdShow)
{
	return ::ShowWindow(m_hWnd, nCmdShow);
}

/**
 * スクロールバーのさまざまなパラメータを設定します
 * @param[in] nBar パラメータを設定するべきスクロールバーのタイプを指定します
 * @param[in] lpScrollInfo 設定するべき情報を保持している、1個の構造体へのポインタを指定します
 * @param[in] bRedraw スクロールバーを再描画するかどうかを指定します
 * @return スクロールバーの現在のスクロール位置が返ります
 */
inline int CWndBase::SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw)
{
	return ::SetScrollInfo(m_hWnd, nBar, lpScrollInfo, bRedraw);
}

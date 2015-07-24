/**
 * @file	wndloc.cpp
 * @brief	Window ロケータ クラスの動作の定義を行います
 */

#include "compiler.h"
#include "wndloc.h"

/**
 * 設定
 */
enum
{
	SNAPDOTPULL		= 12,		//!< プル レート
	SNAPDOTREL		= 16		//!< リリース レート
};

/**
 * 開始
 */
void CWndLoc::Start()
{
	ZeroMemory(this, sizeof(*this));
}

/**
 * 移動処理
 * @param[in,out] lpRect 領域
 */
void CWndLoc::Moving(LPRECT lpRect)
{
	RECT rcWork;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);

	const int nWidth = lpRect->right - lpRect->left;
	const int nHeight = lpRect->bottom - lpRect->top;
	if ((nWidth > (rcWork.right - rcWork.left)) || (nHeight > (rcWork.bottom - rcWork.top)))
	{
		return;
	}

	if (m_nFlag & 1)
	{
		int d = lpRect->left - m_pt.x;
		m_dx += d;
		if (abs(m_dx) >= SNAPDOTREL)
		{
			m_nFlag &= ~1;
			d -= m_dx;
			m_dx = 0;
		}
		lpRect->left -= d;
		lpRect->right -= d;
	}
	if (m_nFlag & 2)
	{
		int d = lpRect->top - m_pt.y;
		m_dy += d;
		if (abs(m_dy) >= SNAPDOTREL)
		{
			m_nFlag &= ~2;
			d -= m_dy;
			m_dy = 0;
		}
		lpRect->top -= d;
		lpRect->bottom -= d;
	}

	if (!(m_nFlag & 1))
	{
		const int l = lpRect->left - rcWork.left;
		const int r = lpRect->right - rcWork.right;
		const int d = (abs(l) < abs(r)) ? l : r;
		if (abs(d) < SNAPDOTPULL)
		{
			lpRect->left -= d;
			lpRect->right -= d;
			m_nFlag |= 1;
			m_dx = d;
			m_pt.x = lpRect->left;
		}
	}
	if (!(m_nFlag & 2))
	{
		const int t = lpRect->top - rcWork.top;
		const int b = lpRect->bottom - rcWork.bottom;
		const int d = (abs(t) < abs(b)) ? t : b;
		if (abs(d) < SNAPDOTPULL)
		{
			lpRect->top -= d;
			lpRect->bottom -= d;
			m_nFlag |= 2;
			m_dy = d;
			m_pt.y = lpRect->top;
		}
	}
}

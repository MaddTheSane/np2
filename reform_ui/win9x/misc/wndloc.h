/**
 * @file	wndloc.h
 * @brief	Window ロケータ クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

/**
 * @brief Window ロケータ クラス
 */
class CWndLoc
{
public:
	void Start();
	void Moving(LPRECT lpRect);

private:
	UINT m_nFlag;	//!< 接続フラグ
	POINT m_pt;		//!< 位置
	int m_dx;		//!< デルタX
	int m_dy;		//!< デルタY
};

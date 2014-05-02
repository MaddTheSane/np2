/**
 *	@file	tawnd.h
 *	@brief	Tool-assisted ウィンドウ クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "misc\wndbase.h"

/**
 * @brief Tool-assisted ウィンドウ クラス
 */
class CTAWnd : public CWndBase
{
public:
	static CTAWnd* GetInstance();

	CTAWnd();
	bool Create();
	bool execute();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	static CTAWnd sm_instance;		//!< 唯一のインスタンスです

	UINT m_nFrames;					//!< フレーム
	UINT m_nExecFrames;				//!< 実行フレーム
};

/**
 * インスタンスを返します
 * @return インスタンス
 */
inline CTAWnd* CTAWnd::GetInstance()
{
	return &sm_instance;
}

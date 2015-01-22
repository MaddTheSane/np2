/**
 * @file	viewer.h
 * @brief	DebugUty 用ビューワ クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "..\misc\WndProc.h"
#include "viewcmn.h"
#include "viewmem.h"

#define	NP2VIEW_MAX	8

enum
{
	VIEWMODE_REG = 0,
	VIEWMODE_SEG,
	VIEWMODE_1MB,
	VIEWMODE_ASM,
	VIEWMODE_SND
};

/**
 * @brief ビュー クラス
 */
class CDebugUtyView : public CWndProc
{
public:
	static void Initialize(HINSTANCE hInstance);
	static void New();
	static void AllClose();
	static void AllUpdate(bool bForce);

	CDebugUtyView();
	virtual ~CDebugUtyView();
	void UpdateCaption();
	UINT32 GetVScrollPos() const;
	void SetVScrollPos(UINT nPos);
	void SetVScroll(UINT nPos, UINT nLines);
	void UpdateVScroll();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void OnVScroll(UINT nSBCode, UINT nPos, HWND hwndScrollBar);
	void OnEnterMenuLoop(BOOL bIsTrackPopupMenu);
	virtual void PostNcDestroy();

private:
	bool m_bActive;			//!< アクティブ フラグ
	UINT m_nVPos;			//!< 位置
	UINT m_nVLines;			//!< ライン数
	UINT m_nVPage;			//!< 1ページの表示数
	UINT m_nVMultiple;		//!< 倍率

public:
	VIEWMEMBUF	buf1;
	VIEWMEMBUF	buf2;
	UINT8		type;
	UINT8		lock;
	UINT16		seg;
	UINT16		off;
	DebugUtyViewMemory dmem;

private:
	static DWORD sm_dwLastTick;		//!< 最後のTick
	void SetMode(UINT8 type);
	void SetSegmentItem(HMENU hMenu, int nId, LPCTSTR lpSegment, UINT nSegment);
	void UpdateView();
	static void UpdateActive();
};

/**
 * 現在の位置を返す
 * @return 位置
 */
inline UINT32 CDebugUtyView::GetVScrollPos() const
{
	return m_nVPos;
}

extern	const TCHAR		np2viewfont[];

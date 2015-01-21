/**
 * @file	viewer.h
 * @brief	DebugUty �p�r���[�� �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
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
 * @brief �r���[ �N���X
 */
class CDebugUtyView : public CWndProc
{
public:
//	static void Initialize();
//	static void New();
//	static void AllClose();
//	static void AllUpdate(bool bForce);

	CDebugUtyView();
	virtual ~CDebugUtyView();
	void UpdateCaption();
	UINT32 GetVScrollPos() const;
	void SetVScrollPos(UINT nPos);
	void SetVScroll(UINT nPos, UINT nLines);
	void UpdateVScroll();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	void OnVScroll(UINT nSBCode, UINT nPos, HWND hwndScrollBar);
	void OnEnterMenuLoop(BOOL bIsTrackPopupMenu);
	virtual void PostNcDestroy();

public:
	VIEWMEMBUF	buf1;
	VIEWMEMBUF	buf2;
	UINT		m_nVPos;			//!< �ʒu
	UINT		m_nVLines;			//!< ���C����
	UINT		m_nVPage;			//!< 1�y�[�W�̕\����
	UINT		m_nVMultiple;		//!< �{��
	UINT8		type;
	UINT8		lock;
	UINT8		active;
	UINT16		seg;
	UINT16		off;
	DebugUtyViewMemory dmem;

private:
	void SetMode(UINT8 type);
	void SetSegmentItem(HMENU hMenu, int nId, LPCTSTR lpSegment, UINT nSegment);
};

/**
 * ���݂̈ʒu��Ԃ�
 * @return �ʒu
 */
inline UINT32 CDebugUtyView::GetVScrollPos() const
{
	return m_nVPos;
}


extern	const TCHAR		np2viewfont[];


BOOL viewer_init(HINSTANCE hInstance);
void viewer_term(void);

void viewer_open(HINSTANCE hInstance);
void viewer_allclose(void);

void viewer_allreload(BOOL force);


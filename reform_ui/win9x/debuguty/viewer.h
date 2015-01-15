/**
 * @file	viewer.h
 * @brief	DebugUty 用ビューワ クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "..\misc\WndProc.h"
#include "viewmem.h"

#define	NP2VIEW_MAX	8

enum {
	VIEWMODE_REG = 0,
	VIEWMODE_SEG,
	VIEWMODE_1MB,
	VIEWMODE_ASM,
	VIEWMODE_SND
};

enum {
	ALLOCTYPE_NONE = 0,
	ALLOCTYPE_REG,
	ALLOCTYPE_SEG,
	ALLOCTYPE_1MB,
	ALLOCTYPE_ASM,
	ALLOCTYPE_SND,

	ALLOCTYPE_ERROR = 0xffffffff
};

typedef struct {
	UINT32	type;
	UINT32	arg;
	UINT32	size;
	void	*ptr;
} VIEWMEMBUF;

/**
 * @brief ビュー クラス
 */
class CDebugUtyView : public CWndProc
{
public:
	static CDebugUtyView* FromWnd(HWND hWnd);

	CDebugUtyView();
	virtual ~CDebugUtyView();
	void UpdateCaption();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	HWND		hwnd;
	VIEWMEMBUF	buf1;
	VIEWMEMBUF	buf2;
	UINT32		pos;
	UINT32		maxline;
	UINT16		step;
	UINT16		mul;
	UINT8		type;
	UINT8		lock;
	UINT8		active;
	UINT16		seg;
	UINT16		off;
	DebugUtyViewMemory dmem;

private:
	void SetMode(UINT8 type);
};

typedef CDebugUtyView NP2VIEW_T;

extern	const TCHAR		np2viewfont[];


BOOL viewer_init(HINSTANCE hInstance);
void viewer_term(void);

void viewer_open(HINSTANCE hInstance);
void viewer_allclose(void);

void viewer_allreload(BOOL force);


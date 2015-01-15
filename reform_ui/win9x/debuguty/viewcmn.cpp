/**
 * @file	viewcmn.cpp
 * @brief	DebugUty 用ビューワ共通クラスの動作の定義を行います
 */

#include "compiler.h"
#include "resource.h"
#include "np2.h"
#include "viewer.h"
#include "viewcmn.h"
#include "viewmenu.h"
#include "viewmem.h"
#include "viewreg.h"
#include "viewseg.h"
#include "view1mb.h"
#include "viewasm.h"
#include "viewsnd.h"
#include "cpucore.h"

const char viewcmn_hex[16] = {
				'0', '1', '2', '3', '4', '5', '6', '7',
				'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

void viewcmn_putcaption(NP2VIEW_T *view)
{
	view->UpdateCaption();
}

// ----

BOOL viewcmn_alloc(VIEWMEMBUF *buf, UINT32 size) {

	if (buf->type == ALLOCTYPE_ERROR) {
		return(FAILURE);
	}
	if (buf->size < size) {
		if (buf->ptr) {
			free(buf->ptr);
		}
		ZeroMemory(buf, sizeof(VIEWMEMBUF));
		buf->ptr = malloc(size);
		if (!buf->ptr) {
			buf->type = ALLOCTYPE_ERROR;
			return(FAILURE);
		}
		buf->size = size;
	}
	return(SUCCESS);
}


void viewcmn_free(VIEWMEMBUF *buf) {

	if (buf->ptr) {
		free(buf->ptr);
	}
	ZeroMemory(buf, sizeof(VIEWMEMBUF));
}


// ----

void viewcmn_setmode(NP2VIEW_T *dst, NP2VIEW_T *src, UINT8 type) {

	switch(type) {
		case VIEWMODE_REG:
			viewreg_init(dst, src);
			break;

		case VIEWMODE_SEG:
			viewseg_init(dst, src);
			break;

		case VIEWMODE_1MB:
			view1mb_init(dst, src);
			break;

		case VIEWMODE_ASM:
			viewasm_init(dst, src);
			break;

		case VIEWMODE_SND:
			viewsnd_init(dst, src);
			break;
	}
	viewmenu_mode(dst);
}

LRESULT CALLBACK viewcmn_dispat(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

	NP2VIEW_T* dbg = CDebugUtyView::FromWnd(hwnd);
	if (dbg)
	{
		switch(dbg->type) {
			case VIEWMODE_REG:
				return(viewreg_proc(dbg, hwnd, msg, wp, lp));

			case VIEWMODE_SEG:
				return(viewseg_proc(dbg, hwnd, msg, wp, lp));

			case VIEWMODE_1MB:
				return(view1mb_proc(dbg, hwnd, msg, wp, lp));

			case VIEWMODE_ASM:
				return(viewasm_proc(dbg, hwnd, msg, wp, lp));

			case VIEWMODE_SND:
				return(viewsnd_proc(dbg, hwnd, msg, wp, lp));
		}
	}
	return(0L);
}


// ----

static void modmenu(HMENU hmenu, int pos, UINT16 id,
											const TCHAR *seg, UINT16 value) {

	TCHAR	buf[256];

	wsprintf(buf, _T("Seg = &%s [%04x]"), seg, value);
	ModifyMenu(hmenu, pos, MF_BYPOSITION | MF_STRING, id, buf);
}

void viewcmn_setmenuseg(HWND hwnd) {

	HMENU	hparent;
	HMENU	hmenu;

	hparent = GetMenu(hwnd);
	if (hparent == NULL) {
		return;
	}
	hmenu = GetSubMenu(hparent, 2);

	if (hmenu) {
		modmenu(hmenu, 2, IDM_SEGCS, _T("CS"), CPU_CS);
		modmenu(hmenu, 3, IDM_SEGDS, _T("DS"), CPU_DS);
		modmenu(hmenu, 4, IDM_SEGES, _T("ES"), CPU_ES);
		modmenu(hmenu, 5, IDM_SEGSS, _T("SS"), CPU_SS);
		DrawMenuBar(hwnd);
	}
}

// -----

void viewcmn_setvscroll(HWND hWnd, NP2VIEW_T *view)
{
	SCROLLINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = ((view->maxline + view->mul - 1) / view->mul) - 1;
	si.nPos = view->pos / view->mul;
	si.nPage = view->step / view->mul;
	SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
}

// -----

void viewcmn_paint(NP2VIEW_T *view, UINT32 bkgcolor,
					void (*callback)(NP2VIEW_T *view, RECT *rc, HDC hdc)) {

	HDC			hdc;
	PAINTSTRUCT	ps;
	RECT		rc;
	HDC			hmemdc;
	HBITMAP		hbitmap;
	HBRUSH		hbrush;

	hdc = BeginPaint(view->hwnd, &ps);
	GetClientRect(view->hwnd, &rc);
	hmemdc = CreateCompatibleDC(hdc);
	hbitmap = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
	hbitmap = (HBITMAP)SelectObject(hmemdc, hbitmap);
	hbrush = (HBRUSH)SelectObject(hmemdc, CreateSolidBrush(bkgcolor));
	PatBlt(hmemdc, 0, 0, rc.right, rc.bottom, PATCOPY);
	DeleteObject(SelectObject(hmemdc, hbrush));

	callback(view, &rc, hmemdc);

	BitBlt(hdc, 0, 0, rc.right, rc.bottom, hmemdc, 0, 0, SRCCOPY);
	DeleteObject(SelectObject(hmemdc, hbitmap));
	DeleteDC(hmemdc);
	EndPaint(view->hwnd, &ps);
}

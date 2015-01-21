/**
 * @file	viewcmn.cpp
 * @brief	DebugUty 用ビューワ共通クラスの動作の定義を行います
 */

#include "compiler.h"
#include "resource.h"
#include "np2.h"
#include "viewer.h"
#include "viewcmn.h"
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
}

LRESULT viewcmn_dispat(NP2VIEW_T* dbg, UINT msg, WPARAM wp, LPARAM lp) {

	HWND hwnd = dbg->m_hWnd;
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

// -----

void viewcmn_paint(NP2VIEW_T *view, UINT32 bkgcolor,
					void (*callback)(NP2VIEW_T *view, RECT *rc, HDC hdc)) {

	HDC			hdc;
	PAINTSTRUCT	ps;
	RECT		rc;
	HDC			hmemdc;
	HBITMAP		hbitmap;
	HBRUSH		hbrush;

	hdc = view->BeginPaint(&ps);
	view->GetClientRect(&rc);
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
	view->EndPaint(&ps);
}

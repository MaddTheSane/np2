#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"i286.h"
#include	"debugsub.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"viewmenu.h"
#include	"viewmem.h"
#include	"viewreg.h"


static void viewreg_paint(NP2VIEW_T *view, RECT *rc, HDC hdc) {

	LONG		y;
	DWORD		pos;
	char		str[128];
	HFONT		hfont;
	I286REGS	*r;

	hfont = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 
					SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, FIXED_PITCH, "‚l‚r ƒSƒVƒbƒN");
	SetTextColor(hdc, 0xffffff);
	SetBkColor(hdc, 0x400000);
	hfont = (HFONT)SelectObject(hdc, hfont);

	if (view->lock) {
		if (view->buf1.type != ALLOCTYPE_REG) {
			if (viewcmn_alloc(&view->buf1, sizeof(I286REGS))) {
				view->lock = FALSE;
				viewmenu_lock(view);
			}
			else {
				view->buf1.type = ALLOCTYPE_REG;
				CopyMemory(view->buf1.ptr, &i286r, sizeof(I286REGS));
			}
			viewcmn_putcaption(view);
		}
	}

	pos = view->pos;
	if (view->lock) {
		r = (I286REGS *)view->buf1.ptr;
	}
	else {
		r = &i286r;
	}

	for (y=0; y<rc->bottom && pos<4; y+=16, pos++) {
		switch(pos) {
			case 0:
				wsprintf(str, "AX=%04x  BX=%04x  CX=%04x  DX=%04x",
									r->w.ax, r->w.bx, r->w.cx, r->w.dx);
				break;

			case 1:
				wsprintf(str, "SP=%04x  BP=%04x  SI=%04x  DI=%04x",
									r->w.sp, r->w.bp, r->w.si, r->w.di);
				break;

			case 2:
				wsprintf(str, "DS=%04x  ES=%04x  SS=%04x  CS=%04x",
									r->w.ds, r->w.es, r->w.ss, r->w.cs);
				break;

			case 3:
				wsprintf(str, "IP=%04x   %s",
									r->w.ip, debugsub_flags(r->w.flag));
				break;
		}
		TextOut(hdc, 0, y, str, strlen(str));
	}
	DeleteObject(SelectObject(hdc, hfont));
}


LRESULT CALLBACK viewreg_proc(NP2VIEW_T *view,
								HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {

		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDM_VIEWMODELOCK:
					view->lock ^= 1;
					viewmenu_lock(view);
					viewcmn_putcaption(view);
					InvalidateRect(hwnd, NULL, TRUE);
					break;
			}
			break;

		case WM_PAINT:
			viewcmn_paint(view, 0x400000, viewreg_paint);
			break;

	}
	return(0L);
}


// ---------------------------------------------------------------------------

void viewreg_init(NP2VIEW_T *dst, NP2VIEW_T *src) {

	dst->type = VIEWMODE_REG;
	dst->maxline = 4;
	dst->mul = 1;
	dst->pos = 0;
}

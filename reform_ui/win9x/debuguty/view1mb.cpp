#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"viewmem.h"
#include	"view1mb.h"
#include	"cpucore.h"

static void view1mb_paint(NP2VIEW_T *view, RECT *rc, HDC hdc) {

	int		x;
	LONG	y;
	UINT8	*p;
	UINT8	buf[16];
	TCHAR	str[16];
	HFONT	hfont;

	hfont = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 
					SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, FIXED_PITCH, np2viewfont);
	SetTextColor(hdc, 0xffffff);
	SetBkColor(hdc, 0x400000);
	hfont = (HFONT)SelectObject(hdc, hfont);

	if (view->lock) {
		if (view->buf1.type != ALLOCTYPE_1MB) {
			if (viewcmn_alloc(&view->buf1, 0x10fff0)) {
				view->lock = FALSE;
			}
			else {
				view->buf1.type = ALLOCTYPE_1MB;
				view->dmem.Read(0, view->buf1.ptr, 0x10fff0);
			}
			viewcmn_putcaption(view);
		}
	}

	UINT off = view->GetVScrollPos() << 4;
	for (y=0; y<rc->bottom && off<0x10fff0; y+=16, off+=16) {
		wsprintf(str, _T("%08x"), off);
		TextOut(hdc, 0, y, str, 8);
		if (view->lock) {
			p = (UINT8 *)view->buf1.ptr;
			p += off;
		}
		else {
			p = buf;
			view->dmem.Read(off, buf, 16);
		}
		for (x=0; x<16; x++) {
			str[0] = viewcmn_hex[*p >> 4];
			str[1] = viewcmn_hex[*p & 15];
			str[2] = 0;
			p++;
			TextOut(hdc, (10 + x * 3) * 8, y, str, 2);
		}
	}

	DeleteObject(SelectObject(hdc, hfont));
}


LRESULT CALLBACK view1mb_proc(NP2VIEW_T *view,
								HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDM_SEGCS:
					view->SetVScrollPos(CPU_CS);
					break;

				case IDM_SEGDS:
					view->SetVScrollPos(CPU_DS);
					break;

				case IDM_SEGES:
					view->SetVScrollPos(CPU_ES);
					break;

				case IDM_SEGSS:
					view->SetVScrollPos(CPU_SS);
					break;

				case IDM_SEGTEXT:
					view->SetVScrollPos(0xa000);
					break;

				case IDM_VIEWMODELOCK:
					view->lock ^= 1;
					viewcmn_putcaption(view);
					InvalidateRect(hwnd, NULL, TRUE);
					break;
			}
			break;

		case WM_PAINT:
			viewcmn_paint(view, 0x400000, view1mb_paint);
	}
	return(0L);
}


// ---------------------------------------------------------------------------

void view1mb_init(NP2VIEW_T *dst, NP2VIEW_T *src) {

	UINT nPos = 0;
	if (src) {
		switch(src->type) {
			case VIEWMODE_SEG:
				nPos = src->seg;
				break;

			case VIEWMODE_1MB:
				nPos = src->GetVScrollPos();
				break;

			case VIEWMODE_ASM:
				nPos = src->seg;
				break;
		}
	}
	dst->type = VIEWMODE_1MB;
	dst->SetVScroll(nPos, 0x10fff);
}

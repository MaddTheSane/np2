#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"i286.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"viewmenu.h"
#include	"viewmem.h"
#include	"viewasm.h"
#include	"unasm.h"


static void set_viewseg(HWND hwnd, NP2VIEW_T *view, WORD seg) {

	if (view->seg != seg) {
		view->seg = seg;
		InvalidateRect(hwnd, NULL, TRUE);
	}
}


static void viewasm_paint(NP2VIEW_T *view, RECT *rc, HDC hdc) {

	LONG		y;
	DWORD		seg4;
	WORD		off;
	DWORD		pos;
	BYTE		*p;
	BYTE		buf[16];
	char		str[16];
	HFONT		hfont;
	PREFIX_T	fix;
	UNASM_T		una;
	int			step;

	hfont = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 
					SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, FIXED_PITCH, "‚l‚rƒSƒVƒbƒN");
	SetTextColor(hdc, 0xffffff);
	SetBkColor(hdc, 0x400000);
	hfont = (HFONT)SelectObject(hdc, hfont);

	if (view->lock) {
		if ((view->buf1.type != ALLOCTYPE_SEG) ||
			(view->buf1.arg != view->seg)) {
			if (viewcmn_alloc(&view->buf1, 0x10000)) {
				view->lock = FALSE;
				viewmenu_lock(view);
			}
			else {
				view->buf1.type = ALLOCTYPE_SEG;
				view->buf1.arg = view->seg;
				viewmem_read(&view->dmem, view->buf1.arg << 4,
											(BYTE *)view->buf1.ptr, 0x10000);
				view->buf2.type = ALLOCTYPE_NONE;
			}
			viewcmn_putcaption(view);
		}
	}

	seg4 = view->seg << 4;
	pos = view->pos;
	if (view->pos) {
		if ((view->buf2.type != ALLOCTYPE_ASM) ||
			(view->buf2.arg != (seg4 + view->off))) {
			if (viewcmn_alloc(&view->buf2, 256*2)) {
				pos = 0;
			}
			else {
				int i;
				WORD *r;
				r = (WORD *)view->buf2.ptr;
				view->buf2.type = ALLOCTYPE_ASM;
				view->buf2.arg = seg4 + view->off;
				off = view->off;
				unasm_reset(&fix);
				for (i=0; i<255; i++) {
					off &= 0xffff;
					*r++ = off;
					if (view->lock) {
						p = (BYTE *)view->buf1.ptr;
						p += off;
						if (off > 0xfff0) {
							DWORD pos = 0x10000 - off;
							CopyMemory(buf, p, pos);
							CopyMemory(buf + pos, view->buf1.ptr, 16 - pos);
							p = buf;
						}
					}
					else {
						p = buf;
						viewmem_read(&(view->dmem), seg4 + off, buf, 16);
					}
					step = unasm(off, p, &fix, NULL);
					off += (WORD)step;
				}
				*r = off;
			}
		}
	}

	if ((pos) && (pos < 256)) {
		off = *(((WORD *)view->buf2.ptr) + pos);
	}
	else {
		off = view->off;
	}

	unasm_reset(&fix);
	for (y=0; y<rc->bottom; y+=16) {
		wsprintf(str, "%04x:%04x", view->seg, off);
		TextOut(hdc, 0, y, str, 9);
		off &= 0xffff;
		if (view->lock) {
			p = (BYTE *)view->buf1.ptr;
			p += off;
			if (off > 0xfff0) {
				DWORD pos = 0x10000 - off;
				CopyMemory(buf, p, pos);
				CopyMemory(buf + pos, view->buf1.ptr, 16 - pos);
				p = buf;
			}
		}
		else {
			p = buf;
			viewmem_read(&(view->dmem), seg4 + off, buf, 16);
		}
		step = unasm(off, p, &fix, &una);
		if (!step) {
			break;
		}
		TextOut(hdc, 11 * 8, y, una.mnemonic, strlen(una.mnemonic));
		if (una.operand[0]) {
			TextOut(hdc, (11 + 7) * 8, y, una.operand, strlen(una.operand));
		}
		off += (WORD)step;
	}

	DeleteObject(SelectObject(hdc, hfont));
}


LRESULT CALLBACK viewasm_proc(NP2VIEW_T *view,
								HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch (msg) {
		case WM_COMMAND:
			switch(LOWORD(wp)) {
				case IDM_SEGCS:
					set_viewseg(hwnd, view, I286_CS);
					break;

				case IDM_SEGDS:
					set_viewseg(hwnd, view, I286_DS);
					break;

				case IDM_SEGES:
					set_viewseg(hwnd, view, I286_ES);
					break;

				case IDM_SEGSS:
					set_viewseg(hwnd, view, I286_SS);
					break;

				case IDM_SEGTEXT:
					set_viewseg(hwnd, view, 0xa000);
					break;

				case IDM_VIEWMODELOCK:
					view->lock ^= 1;
					viewmenu_lock(view);
					viewcmn_putcaption(view);
					InvalidateRect(hwnd, NULL, TRUE);
					break;
			}
			break;

		case WM_PAINT:
			viewcmn_paint(view, 0x400000, viewasm_paint);
	}
	return(0L);
}


// ---------------------------------------------------------------------------

void viewasm_init(NP2VIEW_T *dst, NP2VIEW_T *src) {

	if (src) {
		switch(src->type) {
			case VIEWMODE_SEG:
				dst->seg = dst->seg;
				dst->off = (WORD)(dst->pos << 4);
				break;

			case VIEWMODE_1MB:
				if (dst->pos < 0x10000) {
					dst->seg = (WORD)dst->pos;
					dst->off = 0;
				}
				else {
					dst->seg = 0xffff;
					dst->off = (WORD)((dst->pos - 0xffff) << 4);
				}
				break;

			case VIEWMODE_ASM:
				dst->seg = src->seg;
				dst->off = src->off;
				break;

			default:
				src = NULL;
				break;
		}
	}
	if (!src) {
		dst->seg = I286_CS;
		dst->off = I286_IP;
	}
	dst->type = VIEWMODE_ASM;
	dst->maxline = 256;
	dst->mul = 1;
	dst->pos = 0;
}

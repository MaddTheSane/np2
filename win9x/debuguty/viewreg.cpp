#include	"compiler.h"
#include	"resource.h"
#include	"np2.h"
#include	"debugsub.h"
#include	"viewer.h"
#include	"viewcmn.h"
#include	"viewmenu.h"
#include	"viewmem.h"
#include	"viewreg.h"
#include	"cpucore.h"


#if !defined(CPUCORE_IA32)
static void viewreg_paint(NP2VIEW_T *view, RECT *rc, HDC hdc) {

	LONG		y;
	DWORD		pos;
	char		str[128];
	HFONT		hfont;
	I286STAT	*r;

	hfont = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 
					SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, FIXED_PITCH, "‚l‚r ƒSƒVƒbƒN");
	SetTextColor(hdc, 0xffffff);
	SetBkColor(hdc, 0x400000);
	hfont = (HFONT)SelectObject(hdc, hfont);

	if (view->lock) {
		if (view->buf1.type != ALLOCTYPE_REG) {
			if (viewcmn_alloc(&view->buf1, sizeof(i286core.s))) {
				view->lock = FALSE;
				viewmenu_lock(view);
			}
			else {
				view->buf1.type = ALLOCTYPE_REG;
				CopyMemory(view->buf1.ptr, &i286core.s, sizeof(i286core.s));
			}
			viewcmn_putcaption(view);
		}
	}

	pos = view->pos;
	if (view->lock) {
		r = (I286STAT *)view->buf1.ptr;
	}
	else {
		r = &i286core.s;
	}

	for (y=0; y<rc->bottom && pos<4; y+=16, pos++) {
		switch(pos) {
			case 0:
				wsprintf(str, "AX=%.4x  BX=%.4x  CX=%.4x  DX=%.4x",
								r->r.w.ax, r->r.w.bx, r->r.w.cx, r->r.w.dx);
				break;

			case 1:
				wsprintf(str, "SP=%.4x  BP=%.4x  SI=%.4x  DI=%.4x",
								r->r.w.sp, r->r.w.bp, r->r.w.si, r->r.w.di);
				break;

			case 2:
				wsprintf(str, "CS=%.4x  DS=%.4x  ES=%.4x  SS=%.4x",
								r->r.w.cs, r->r.w.ds, r->r.w.es, r->r.w.ss);
				break;

			case 3:
				wsprintf(str, "IP=%.4x   %s",
								r->r.w.ip, debugsub_flags(r->r.w.flag));
				break;
		}
		TextOut(hdc, 0, y, str, strlen(str));
	}
	DeleteObject(SelectObject(hdc, hfont));
}
#else
static void viewreg_paint(NP2VIEW_T *view, RECT *rc, HDC hdc) {

	LONG		y;
	DWORD		pos;
	char		str[128];
	HFONT		hfont;
	I386STAT	*r;

	hfont = CreateFont(16, 0, 0, 0, 0, 0, 0, 0, 
					SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					DEFAULT_QUALITY, FIXED_PITCH, "‚l‚r ƒSƒVƒbƒN");
	SetTextColor(hdc, 0xffffff);
	SetBkColor(hdc, 0x400000);
	hfont = (HFONT)SelectObject(hdc, hfont);

	if (view->lock) {
		if (view->buf1.type != ALLOCTYPE_REG) {
			if (viewcmn_alloc(&view->buf1, sizeof(i386core.s))) {
				view->lock = FALSE;
				viewmenu_lock(view);
			}
			else {
				view->buf1.type = ALLOCTYPE_REG;
				CopyMemory(view->buf1.ptr, &i386core.s, sizeof(i386core.s));
			}
			viewcmn_putcaption(view);
		}
	}

	pos = view->pos;
	if (view->lock) {
		r = (I386STAT *)view->buf1.ptr;
	}
	else {
		r = &i386core.s;
	}

	for (y=0; y<rc->bottom && pos<4; y+=16, pos++) {
		switch(pos) {
			case 0:
				wsprintf(str, "EAX=%.8x EBX=%.8x ECX=%.8x EDX=%.8x",
								r->cpu_regs.reg[CPU_EAX_INDEX].d,
								r->cpu_regs.reg[CPU_EBX_INDEX].d,
								r->cpu_regs.reg[CPU_ECX_INDEX].d,
								r->cpu_regs.reg[CPU_EDX_INDEX].d);
				break;

			case 1:
				wsprintf(str, "ESP=%.8x EBP=%.8x ESI=%.8x EDI=%.8x",
								r->cpu_regs.reg[CPU_ESP_INDEX].d,
								r->cpu_regs.reg[CPU_EBP_INDEX].d,
								r->cpu_regs.reg[CPU_ESI_INDEX].d,
								r->cpu_regs.reg[CPU_EDI_INDEX].d);
				break;

			case 2:
				wsprintf(str, "CS=%.4x DS=%.4x ES=%.4x " \
								"FS=%.4x GS=%.4x SS=%.4x",
								r->cpu_regs.sreg[CPU_CS_INDEX],
								r->cpu_regs.sreg[CPU_DS_INDEX],
								r->cpu_regs.sreg[CPU_ES_INDEX],
								r->cpu_regs.sreg[CPU_FS_INDEX],
								r->cpu_regs.sreg[CPU_GS_INDEX],
								r->cpu_regs.sreg[CPU_SS_INDEX]);
				break;

			case 3:
				wsprintf(str, "EIP=%.8x   %s",
								r->cpu_regs.eip.d,
								debugsub_flags(r->cpu_regs.eflags.d));
				break;
		}
		TextOut(hdc, 0, y, str, strlen(str));
	}
	DeleteObject(SelectObject(hdc, hfont));
}
#endif

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


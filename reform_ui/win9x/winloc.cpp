#include	"compiler.h"
#include	"winloc.h"


enum {
	SNAPDOTPULL		= 12,
	SNAPDOTREL		= 16
};

void winloc_setclientsize(HWND hWnd, int nWidth, int nHeight)
{
	winloc_setclientsize(hWnd, CW_USEDEFAULT, CW_USEDEFAULT, nWidth, nHeight);
}

void winloc_setclientsize(HWND hWnd, int nPosX, int nPosY, int nWidth, int nHeight)
{
	RECT rcDesktop;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);

	const int nScreenCX = GetSystemMetrics(SM_CXSCREEN);
	const int nScreenCY = GetSystemMetrics(SM_CYSCREEN);

	UINT nCount = 2;
	do
	{
		RECT rcWindow;
		GetWindowRect(hWnd, &rcWindow);

		RECT rcClient;
		GetClientRect(hWnd, &rcClient);

		const int w = nWidth + (rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left);
		const int h = nHeight + (rcWindow.bottom - rcWindow.top) - (rcClient.bottom - rcClient.top);

		int x = (nPosX != CW_USEDEFAULT) ? nPosX : rcWindow.left;
		int y = (nPosY != CW_USEDEFAULT) ? nPosY : rcWindow.top;
		if (nScreenCX < w)
		{
			x = (nScreenCX - w) / 2;
		}
		else
		{
			if ((x + w) > rcDesktop.right)
			{
				x = rcDesktop.right - w;
			}
			if (x < rcDesktop.left)
			{
				x = rcDesktop.left;
			}
		}
		if (nScreenCY < h)
		{
			y = (nScreenCY - h) / 2;
		}
		else
		{
			if ((y + h) > rcDesktop.bottom)
			{
				y = rcDesktop.bottom - h;
			}
			if (y < rcDesktop.top)
			{
				y = rcDesktop.top;
			}
		}
		::MoveWindow(hWnd, x, y, w, h, TRUE);
	} while (--nCount);
}


// ----

static UINT8 isconnect(const RECT *parent, const RECT *self) {

	UINT8	connect;

	connect = 0;
	if ((self->bottom >= parent->top) && (self->top <= parent->bottom)) {
		if (self->right == parent->left) {
			connect += 1;
		}
		else if (self->left == parent->right) {
			connect += 2;
		}
	}
	if ((!(connect & 0x0f)) &&
		((self->bottom == parent->top) || (self->top == parent->bottom))) {
		if (self->left == parent->left) {
			connect += 3;
		}
		else if (self->right == parent->right) {
			connect += 4;
		}
	}

	if ((self->right >= parent->left) && (self->left <= parent->right)) {
		if (self->bottom == parent->top) {
			connect += 1 << 4;
		}
		else if (self->top == parent->bottom) {
			connect += 2 << 4;
		}
	}
	if ((!(connect & 0xf0)) &&
		((self->right == parent->left) || (self->left == parent->right))) {
		if (self->top == parent->top) {
			connect += 3 << 4;
		}
		else if (self->bottom == parent->bottom) {
			connect += 4 << 4;
		}
	}
	return(connect);
}

WINLOCEX winlocex_create(HWND base, const HWND *child, UINT count) {

	WINLOCEX	ret;
	HWND		*list;
	UINT		inlist;
	UINT		i;
	UINT		j;
	HWND		hwnd;
	UINT		allocsize;
	WLEXWND		*wnd;
	RECT		rect;
	UINT8		connect;
	WLEXWND		*p;

	if (child == NULL) {
		count = 0;
	}
	ret = NULL;
	list = NULL;
	inlist = 0;
	if (count) {
		list = (HWND *)_MALLOC(count * sizeof(HWND *), "wnd list");
		if (list == NULL) {
			goto wlecre_err1;
		}
		for (i=0; i<count; i++) {
			hwnd = child[i];
			if ((hwnd != NULL) && (hwnd != base) &&
				(!(GetWindowLong(hwnd, GWL_STYLE) &
											(WS_MAXIMIZE | WS_MINIMIZE)))) {
				for (j=0; j<inlist; j++) {
					if (list[j] == hwnd) {
						break;
					}
				}
				if (j >= inlist) {
					list[inlist++] = hwnd;
				}
			}
		}
	}
	allocsize = sizeof(_WINLOCEX) + (sizeof(WLEXWND) * inlist);
	ret = (WINLOCEX)_MALLOC(allocsize, "winlocex");
	if (ret == NULL) {
		goto wlecre_err2;
	}
	ZeroMemory(ret, allocsize);
	wnd = (WLEXWND *)(ret + 1);

	if (base) {
		// 親と接続されてる？
		ret->base = base;
		GetWindowRect(base, &ret->rect);
		for (i=0; i<inlist; i++) {
			hwnd = list[i];
			if (hwnd) {
				GetWindowRect(hwnd, &rect);
				connect = isconnect(&ret->rect, &rect);
				if (connect) {
					list[i] = NULL;
					wnd->hwnd = hwnd;
					CopyMemory(&wnd->rect, &rect, sizeof(RECT));
					wnd->connect = connect;
//					wnd->parent = 0;
					wnd++;
					ret->count++;
				}
			}
		}
		// 子と接続されてる？
		p = (WLEXWND *)(ret + 1);
		for (i=0; i<ret->count; i++, p++) {
			for (j=0; j<inlist; j++) {
				hwnd = list[j];
				if (hwnd) {
					GetWindowRect(hwnd, &rect);
					connect = isconnect(&p->rect, &rect);
					if (connect) {
						list[j] = NULL;
						wnd->hwnd = hwnd;
						CopyMemory(&wnd->rect, &rect, sizeof(RECT));
						wnd->connect = connect;
						wnd->parent = i + 1;
						wnd++;
						ret->count++;
					}
				}
			}
		}
	}

	for (i=0; i<inlist; i++) {
		hwnd = list[i];
		if (hwnd) {
			wnd->hwnd = hwnd;
			GetWindowRect(hwnd, &wnd->rect);
			wnd++;
			ret->count++;
		}
	}

wlecre_err2:
	if (list) {
		_MFREE(list);
	}

wlecre_err1:
	return(ret);
}

void winlocex_destroy(WINLOCEX wle) {

	if (wle) {
		_MFREE(wle);
	}
}

void winlocex_setholdwnd(WINLOCEX wle, HWND hold) {

	RECT	workrc;
	RECT	rect;
	UINT	flag;

	if ((wle == NULL) || (hold == NULL)) {
		return;
	}
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrc, 0);
	GetWindowRect(hold, &rect);
	flag = 0;
	if (workrc.left == rect.left) {
		flag = 1;
	}
	else if (workrc.right == rect.right) {
		flag = 2;
	}
	if (workrc.top == rect.top) {
		flag += 1 << 4;
	}
	else if (workrc.bottom == rect.bottom) {
		flag += 2 << 4;
	}
	wle->hold = hold;
	wle->holdflag = flag;
}

static BOOL gravityx(WINLOCEX wle, RECT *rect) {

	int		d;
	WLEXWND	*wnd;
	UINT	i;
	RECT	workrc;

	d = SNAPDOTPULL;
	wnd = (WLEXWND *)(wle + 1);
	for (i=0; i<wle->count; i++, wnd++) {
		if (!wnd->connect) {
			if ((rect->bottom >= wnd->rect.top) &&
				(rect->top <= wnd->rect.bottom)) {
				d = rect->left - wnd->rect.right;
				if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
					break;
				}
				d = rect->right - wnd->rect.left;
				if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
					break;
				}
			}
			if ((rect->bottom == wnd->rect.top) ||
				(rect->top == wnd->rect.bottom)) {
				d = rect->left - wnd->rect.left;
				if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
					break;
				}
				d = rect->right - wnd->rect.right;
				if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
					break;
				}
			}
		}
	}
	if (i < wle->count) {
		wle->flagx = i + 1;
		rect->left -= d;
		rect->right -= d;
		wle->gx = d;
		wle->tx = rect->left;
		return(TRUE);
	}

	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrc, 0);
	wnd = (WLEXWND *)(wle + 1);
	for (i=0; i<wle->count; i++, wnd++) {
		if (wnd->connect) {
			d = wnd->rect.left + (rect->left - wle->rect.left)
															- workrc.left;
			if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
				break;
			}
			d = wnd->rect.right + (rect->right - wle->rect.right)
															- workrc.right;
			if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
				break;
			}
		}
	}
	if (i < wle->count) {
		wle->flagx = (i + 1) << 16;
		rect->left -= d;
		rect->right -= d;
		wle->gx = d;
		wle->tx = rect->left;
		return(TRUE);
	}

	d = rect->left - workrc.left;
	if ((d >= SNAPDOTPULL) || (d <= -SNAPDOTPULL)) {
		d = rect->right - workrc.right;
	}
	if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
		wle->flagx = (UINT)-1;
		rect->left -= d;
		rect->right -= d;
		wle->gx = d;
		wle->tx = rect->left;
		return(TRUE);
	}
	return(FALSE);
}

static BOOL gravityy(WINLOCEX wle, RECT *rect) {

	int		d;
	WLEXWND	*wnd;
	UINT	i;
	RECT	workrc;

	d = SNAPDOTPULL;
	wnd = (WLEXWND *)(wle + 1);
	for (i=0; i<wle->count; i++, wnd++) {
		if (!wnd->connect) {
			if ((rect->right >= wnd->rect.left) &&
				(rect->left <= wnd->rect.right)) {
				d = rect->top - wnd->rect.bottom;
				if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
					break;
				}
				d = rect->bottom - wnd->rect.top;
				if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
					break;
				}
			}
			if ((rect->right == wnd->rect.left) ||
				(rect->left == wnd->rect.right)) {
				d = rect->top - wnd->rect.top;
				if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
					break;
				}
				d = rect->bottom - wnd->rect.bottom;
				if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
					break;
				}
			}
		}
	}
	if (i < wle->count) {
		wle->flagy = i + 1;
		rect->top -= d;
		rect->bottom -= d;
		wle->gy = d;
		wle->ty = rect->top;
		return(TRUE);
	}

	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrc, 0);
	wnd = (WLEXWND *)(wle + 1);
	for (i=0; i<wle->count; i++, wnd++) {
		if (wnd->connect) {
			d = wnd->rect.top + (rect->top - wle->rect.top)
															- workrc.top;
			if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
				break;
			}
			d = wnd->rect.bottom + (rect->bottom - wle->rect.bottom)
															- workrc.bottom;
			if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
				break;
			}
		}
	}
	if (i < wle->count) {
		wle->flagy = (i + 1) << 16;
		rect->top -= d;
		rect->bottom -= d;
		wle->gy = d;
		wle->ty = rect->top;
		return(TRUE);
	}

	d = rect->top - workrc.top;
	if ((d >= SNAPDOTPULL) || (d <= -SNAPDOTPULL)) {
		d = rect->bottom - workrc.bottom;
	}
	if ((d < SNAPDOTPULL) && (d > -SNAPDOTPULL)) {
		wle->flagy = (UINT)-1;
		rect->top -= d;
		rect->bottom -= d;
		wle->gy = d;
		wle->ty = rect->top;
		return(TRUE);
	}
	return(FALSE);
}

void winlocex_moving(WINLOCEX wle, RECT *rect) {

	int		d;
	UINT	num;
	RECT	*rc;
	BOOL	changes;

	if (wle == NULL) {
		return;
	}

	// ひっついてた時
	if (wle->flagx) {
		d = rect->left - wle->tx;
		wle->gx += d;
		rect->left -= d;
		rect->right -= d;
		if ((wle->gx >= SNAPDOTREL) || (wle->gx <= -SNAPDOTREL)) {
			wle->flagx = 0;
			rect->left += wle->gx;
			rect->right += wle->gx;
			wle->gx = 0;
		}
	}
	if (wle->flagy) {
		d = rect->top - wle->ty;
		wle->gy += d;
		rect->top -= d;
		rect->bottom -= d;
		if ((wle->gy >= SNAPDOTREL) || (wle->gy <= -SNAPDOTREL)) {
			wle->flagy = 0;
			rect->top += wle->gy;
			rect->bottom += wle->gy;
			wle->gy = 0;
		}
	}

	// リリース処理
	num = wle->flagx - 1;
	if (num < wle->count) {
		rc = &(((WLEXWND *)(wle + 1))[num].rect);
		if ((rect->left > rc->right) || (rect->right < rc->left) ||
			(rect->top > rc->bottom) || (rect->bottom < rc->top)) {
			rect->left += wle->gx;
			rect->right += wle->gx;
			wle->flagx = 0;
			wle->gx = 0;
		}
	}
	num = wle->flagy - 1;
	if (num < wle->count) {
		rc = &(((WLEXWND *)(wle + 1))[num].rect);
		if ((rect->left > rc->right) || (rect->right < rc->left) ||
			(rect->top > rc->bottom) || (rect->bottom < rc->top)) {
			rect->top += wle->gy;
			rect->bottom += wle->gy;
			wle->flagy = 0;
			wle->gy = 0;
		}
	}

	// 重力
	do {
		changes = FALSE;
		if (!wle->flagx) {
			changes = gravityx(wle, rect);
		}
		if (!wle->flagy) {
			changes = gravityy(wle, rect);
		}
	} while(changes);
}

void winlocex_move(WINLOCEX wle) {

	RECT	workrc;
	WLEXWND	*wnd;
	UINT	i;
	RECT	rect;
	int		cx;
	int		cy;
	RECT	baserect;
	int		dx;
	int		dy;
	UINT	num;
	RECT	*rc;

	if ((wle == NULL) || (wle->base == NULL)) {
		return;
	}

	wnd = (WLEXWND *)(wle + 1);
	for (i=0; i<wle->count; i++) {
		if ((wle->hold == wnd->hwnd) && (wnd->connect)) {
			break;
		}
	}
	if ((i >= wle->count) && (wle->holdflag)) {
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workrc, 0);
		GetWindowRect(wle->hold, &rect);
		cx = rect.right - rect.left;
		cy = rect.bottom - rect.top;
		switch(wle->holdflag & 0x0f) {
			case 1:
				rect.left = workrc.left;
				break;

			case 2:
				rect.left = workrc.right - cx;
				break;
		}
		switch(wle->holdflag >> 4) {
			case 1:
				rect.top = workrc.top;
				break;

			case 2:
				rect.top = workrc.bottom - cy;
				break;
		}
		MoveWindow(wle->hold, rect.left, rect.top, cx, cy, TRUE);
	}

	GetWindowRect(wle->base, &baserect);
	dx = baserect.left - wle->rect.left;
	dy = baserect.top - wle->rect.top;
	wnd = (WLEXWND *)(wle + 1);
	for (i=0; i<wle->count; i++, wnd++) {
		if (wnd->connect) {
			GetWindowRect(wnd->hwnd, &rect);
			cx = rect.right - rect.left;
			cy = rect.bottom - rect.top;
			rect.left += dx;
			rect.top += dy;
			num = wnd->parent - 1;
			if (num < wle->count) {
				rc = &(((WLEXWND *)(wle + 1))[num].rect);
			}
			else {
				rc = &baserect;
			}
			switch(wnd->connect & 0x0f) {
				case 1:
					rect.left = rc->left - cx;
					break;

				case 2:
					rect.left = rc->right;
					break;

				case 3:
					rect.left = rc->left;
					break;

				case 4:
					rect.left = rc->right - cx;
					break;
			}
			switch((wnd->connect >> 4) & 0x0f) {
				case 1:
					rect.top = rc->top - cy;
					break;

				case 2:
					rect.top = rc->bottom;
					break;

				case 3:
					rect.top = rc->top;
					break;

				case 4:
					rect.top = rc->bottom - cy;
					break;
			}
			MoveWindow(wnd->hwnd, rect.left, rect.top, cx, cy, TRUE);
			wnd->rect.left = rect.left;
			wnd->rect.top = rect.top;
			wnd->rect.right = rect.left + cx;
			wnd->rect.bottom = rect.top + cy;
		}
	}
}

void winlocex_close(WINLOCEX wle) {

	WLEXWND	*wnd;
	UINT	i;

	if ((wle == NULL) || (wle->base == NULL)) {
		return;
	}
	wnd = (WLEXWND *)(wle + 1);
	for (i=0; i<wle->count; i++) {
		if (wnd->connect) {
			CloseWindow(wnd->hwnd);
		}
		wnd++;
	}
}


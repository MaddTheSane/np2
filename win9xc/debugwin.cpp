#include	"compiler.h"

#if defined(SUPPORT_MEMDBG32)

#include	"np2.h"
#include	"cmndraw.h"
#include	"memdbg32.h"


#pragma pack(push, 1)
typedef struct {
	BITMAPINFOHEADER	bmiHeader;
	UINT32				bmiColors[256];
} _BMPINFO, *BMPINFO;
#pragma pack(pop)


static void setclientsize(HWND hwnd, int width, int height) {

	RECT	rectDisktop;
	int		scx;
	int		scy;
	UINT	cnt;
	RECT	rectWindow;
	RECT	rectClient;
	int		x, y, w, h;

	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectDisktop, 0);
	scx = GetSystemMetrics(SM_CXSCREEN);
	scy = GetSystemMetrics(SM_CYSCREEN);

	cnt = 2;
	do {
		GetWindowRect(hwnd, &rectWindow);
		GetClientRect(hwnd, &rectClient);
		w = width + (rectWindow.right - rectWindow.left)
					- (rectClient.right - rectClient.left);
		h = height + (rectWindow.bottom - rectWindow.top)
					- (rectClient.bottom - rectClient.top);

		x = rectWindow.left;
		y = rectWindow.top;
		if (scx < w) {
			x = (scx - w) / 2;
		}
		else {
			if ((x + w) > rectDisktop.right) {
				x = rectDisktop.right - w;
			}
			if (x < rectDisktop.left) {
				x = rectDisktop.left;
			}
		}
		if (scy < h) {
			y = (scy - h) / 2;
		}
		else {
			if ((y + h) > rectDisktop.bottom) {
				y = rectDisktop.bottom - h;
			}
			if (y < rectDisktop.top) {
				y = rectDisktop.top;
			}
		}
		MoveWindow(hwnd, x, y, w, h, TRUE);
	} while(--cnt);
}

static HBITMAP allocbmp(int width, int height, CMNVRAM *vram) {

	_BMPINFO	bi;
	HDC			hdc;
	HBITMAP		ret;
	void		*image;

	ZeroMemory(&bi, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = width;
	bi.bmiHeader.biHeight = 0 - height;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32;
	bi.bmiHeader.biCompression = BI_BITFIELDS;
	bi.bmiColors[0] = 0xff0000;
	bi.bmiColors[1] = 0x00ff00;
	bi.bmiColors[2] = 0x0000ff;
	hdc = GetDC(NULL);
	ret = CreateDIBSection(hdc, (BITMAPINFO *)&bi, DIB_RGB_COLORS,
														&image, NULL, 0);
	ReleaseDC(NULL, hdc);
	if (ret != NULL) {
		ZeroMemory(image, width * height * 4);
		if (vram) {
			vram->ptr = (BYTE *)image;
			vram->width = width;
			vram->height = height;
			vram->xalign = 4;
			vram->yalign = width * 4;
			vram->bpp = 32;
		}
	}
	return(ret);
}


// ---- memdbg

#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)

typedef struct {
	HWND	hwnd;
	HBITMAP	hbmp;
	CMNVRAM	vram;
} MEMDBG;

static	MEMDBG		memdbg;

static const char memdbgclass[] = "np2-ia32page";
static const char memdbgtitle[] = "IA32 - paging";

static const UINT32 mdwinpal[MEMDBG32_PALS] =
			{0x00333333, 0x00000000, 
				0x00ffaa00, 0x00ff0000, 0x0000ff00, 0xffffffff};

static UINT32 mdwin_getpal32(CMNPALFN *self, UINT num) {

	if (num < MEMDBG32_PALS) {
		return(mdwinpal[num] & 0xffffff);
	}
	return(0);
}

static void mdwincreate(HWND hWnd) {

	int			width;
	int			height;
	CMNPALFN	palfn;

	memdbg32_initialize();
	memdbg32_getsize(&width, &height);
	palfn.get8 = NULL;
	palfn.get32 = mdwin_getpal32;
	palfn.cnv16 = NULL;
	palfn.userdata = 0;
	memdbg32_setpal(&palfn);
	setclientsize(hWnd, width, height);
	memdbg.hbmp = allocbmp(width, height, &memdbg.vram);
	if (memdbg.hbmp) {
		memdbg32_paint(&memdbg.vram, TRUE);
	}
}

static void mdwinpaint(HWND hWnd) {

	HDC			hdc;
	PAINTSTRUCT	ps;
	BITMAP		bmp;
	HDC			hmdc;

	hdc = BeginPaint(hWnd, &ps);
	if (memdbg.hbmp) {
		GetObject(memdbg.hbmp, sizeof(BITMAP), &bmp);
		hmdc = CreateCompatibleDC(hdc);
		SelectObject(hmdc, memdbg.hbmp);
		BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hmdc, 0, 0, SRCCOPY);
		DeleteDC(hmdc);
	}
	EndPaint(hWnd, &ps);
}

static LRESULT CALLBACK mdproc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {

	switch(msg) {
		case WM_CREATE:
			mdwincreate(hWnd);
			break;

		case WM_PAINT:
			mdwinpaint(hWnd);
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			DeleteObject(memdbg.hbmp);
			memdbg.hbmp = NULL;
			memdbg.hwnd = NULL;
			break;

		default:
			return(DefWindowProc(hWnd, msg, wp, lp));
	}
	return(0);
}

static BOOL memdbg_initapp(HINSTANCE hInstance) {

	WNDCLASS wc;

	wc.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = mdproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = memdbgclass;
	return(RegisterClass(&wc));
}

static void memdbg_create(void) {

	HWND	hWnd;

	if (memdbg.hwnd) {
		return;
	}
	hWnd = CreateWindow(memdbgclass, memdbgtitle,
							WS_SYSMENU | WS_MINIMIZEBOX,
							0, 0, 128, 128,
							NULL, NULL, hInst, NULL);
	memdbg.hwnd = hWnd;
	if (hWnd == NULL) {
		goto mdope_err;
	}
	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOWNOACTIVATE);
	SetForegroundWindow(hWndMain);
	return;

mdope_err:
	return;
}

static void memdbg_destroy(void) {

	if (memdbg.hwnd) {
		DestroyWindow(memdbg.hwnd);
	}
}

static void memdbg_process(void) {

	if ((memdbg.hwnd) && (memdbg.hbmp)) {
		if (memdbg32_paint(&memdbg.vram, FALSE)) {
			InvalidateRect(memdbg.hwnd, NULL, TRUE);
		}
	}
}
#endif

// ----

BOOL debugwin_initapp(HINSTANCE hInstance) {

	BOOL	r;

	r = FALSE;
#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
	r = memdbg_initapp(hInstance);
#endif
	return(r);
}

void debugwin_create(void) {

#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
	memdbg_create();
#endif
}

void debugwin_destroy(void) {

#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
	memdbg_destroy();
#endif
}

void debugwin_process(void) {

#if defined(CPUCORE_IA32) && defined(SUPPORT_MEMDBG32)
	memdbg_process();
#endif
}

#endif


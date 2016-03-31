
#include "compiler.h"

#if defined(SUPPORT_CL_GD5430)

#include "np2.h"
#include "resource.h"
#include "dosio.h"
#include "cpucore.h"
#include "pccore.h"
#include "iocore.h"
#include "video.h"
#include "joymng.h"
#include "mousemng.h"
#include "scrnmng.h"
#include "soundmng.h"
#include "sysmng.h"
#include "winkbd.h"
#include "winloc.h"

BITMAPINFO bmpInfo = {0};
DisplayState ds = {0};
HDC  g_hDC = NULL;
HWND g_hWndVGA = NULL;
UINT8 *g_memptr = NULL;
TCHAR gName[100] = _T("NP2 Window Accelerator");
HWND hWndMain = NULL;

REG8 ga_relay = 0;
static HANDLE	ga_hThread = NULL;
static int		ga_exitThread = 0;
static int		ga_threadmode = 1;
static int		ga_scrnmode = 0;
int		ga_wndwidth = 640;
int		ga_wndheight = 480;

static void changescreen(UINT8 newmode) {

	UINT8		change;
	UINT8		renewal;

	change = ga_scrnmode ^ newmode;
	renewal = (change & SCRNMODE_FULLSCREEN);
	ga_scrnmode = newmode;
	if(change){
		// フルスクリーンは真面目に作らんといかんね
		//if(ga_scrnmode==SCRNMODE_FULLSCREEN){
		//	SetWindowLong(g_hWndVGA, GWL_STYLE, WS_VISIBLE | WS_POPUP);
		//	ShowCursor(FALSE);
		//	ShowWindow(g_hWndVGA, SW_SHOWMAXIMIZED);
		//}else{
		//	ShowWindow(g_hWndVGA, SW_SHOWNORMAL);
		//	ShowCursor(TRUE);
		//	SetWindowLong(g_hWndVGA, GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
		//}
		//SetWindowPos( g_hWndVGA, NULL, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED );
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
	PAINTSTRUCT	ps;
	RECT		rc;
	HDC			hdc;
	BOOL		b;
	UINT		update;
	HWND		subwin;

	switch (msg) {
		case WM_CREATE:
			break;

		case WM_MOVE:
			break;

		case WM_ENTERSIZEMOVE:
			break;

		case WM_MOVING:
			break;
			
		case WM_SIZE:
		case WM_SIZING:
			GetClientRect( hWnd, &rc );
			ga_wndwidth = rc.right - rc.left;
			ga_wndheight = rc.bottom - rc.top;
			break;

		case WM_EXITSIZEMOVE:
			break;

		case WM_KEYDOWN:
			break;

		case WM_KEYUP:
			break;

		case WM_SYSKEYDOWN:
			if (lParam & 0x20000000) {								// ver0.30
				if ((wParam == VK_RETURN)) {
					changescreen(ga_scrnmode ^ SCRNMODE_FULLSCREEN);
					break;
				}
			}
			break;

		case WM_SYSKEYUP:
			break;

		case WM_MOUSEMOVE:
			//if (scrnmng_isfullscreen()) {
			//	POINT p;
			//	if (GetCursorPos(&p)) {
			//		scrnmng_fullscrnmenu(p.y);
			//	}
			//}
			break;

		case WM_LBUTTONDOWN:
			break;

		case WM_LBUTTONUP:
			break;

		case WM_MBUTTONDOWN:
			//mousemng_toggle(MOUSEPROC_SYSTEM);
			//np2oscfg.MOUSE_SW = !np2oscfg.MOUSE_SW;
			//sysmng_update(SYS_UPDATECFG);
			SetForegroundWindow(hWndMain);
			SendMessage(hWndMain, msg, wParam, lParam);
			break;
		case WM_LBUTTONDBLCLK:
			pc98_cirrus_vga_resetscreensize();
			break;

		case WM_CLOSE:
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return(0L);
}

void np2vga_ds_dpy_update(struct DisplayState *s, int x, int y, int w, int h)
{
}
void np2vga_ds_dpy_resize(struct DisplayState *s)
{
}
void np2vga_ds_dpy_setdata(struct DisplayState *s)
{
}
void np2vga_ds_dpy_refresh(struct DisplayState *s)
{
}
void np2vga_ds_dpy_copy(struct DisplayState *s, int src_x, int src_y,
                    int dst_x, int dst_y, int w, int h)
{
}
void np2vga_ds_dpy_fill(struct DisplayState *s, int x, int y,
                    int w, int h, uint32_t_ c)
{
}
void np2vga_ds_dpy_text_cursor(struct DisplayState *s, int x, int y)
{
}


DisplaySurface np2vga_ds_surface = {0};
DisplayChangeListener np2vga_ds_listeners = {0, 0, np2vga_ds_dpy_update, np2vga_ds_dpy_resize, 
											  np2vga_ds_dpy_setdata, np2vga_ds_dpy_refresh, 
											  np2vga_ds_dpy_copy, np2vga_ds_dpy_fill, 
											  np2vga_ds_dpy_text_cursor, NULL};

void np2vga_ds_mouse_set(int x, int y, int on){

}
void np2vga_ds_cursor_define(int width, int height, int bpp, int hot_x, int hot_y,
                          uint8_t *image, uint8_t *mask){

}

// 同期描画（ga_threadmodeが偽）
void np2vga_drawframe()
{
	if(!ga_threadmode && g_memptr){
		cirrusvga_drawGraphic();
	}
}
// 非同期描画（ga_threadmodeが真）
DWORD WINAPI ga_ThreadFunc(LPVOID vdParam) {
	DWORD time = GetTickCount();
	int timeleft = 0;
	while (!ga_exitThread && ga_threadmode) {
		if(g_memptr){
			if(ga_relay) cirrusvga_drawGraphic();
		}
		while(GetTickCount() >= time && GetTickCount()-time < 16){
			timeleft = 16 - (GetTickCount()-time);
			if(timeleft>0) Sleep(timeleft);
		}
		time = GetTickCount();
	}
	ga_threadmode = 0;
	return 0;
}

void np2vga_init(HINSTANCE hInstance, HWND g_hWndMain)
{
	WNDPROC wndproc = NULL;
	WNDCLASSEX wcex = {0};
	DWORD dwID;

	hWndMain = g_hWndMain;
	
	//wndproc = (WNDPROC)GetWindowLongPtr(g_hWndMain, GWLP_WNDPROC);
	if(!wndproc) wndproc = WndProc;

	wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc = wndproc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszClassName = (TCHAR*)gName;
	
	if(!RegisterClassEx(&wcex)) return;
	
	if(!(g_hWndVGA = CreateWindow(gName, gName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL))) return;

	//ShowWindow(g_hWndVGA, SW_SHOWDEFAULT);
	
	//SetWindowPos(g_hWndVGA, NULL, 0, 0, 640, 480, SWP_NOMOVE);
	
	ds.surface = &np2vga_ds_surface;
	ds.listeners = &np2vga_ds_listeners;
	ds.mouse_set = np2vga_ds_mouse_set;
	ds.cursor_define = np2vga_ds_cursor_define;
	ds.next = NULL;

	g_hDC = GetDC(g_hWndVGA);

	cirrusvga_setGAWindow(g_hWndVGA, g_hDC);
	
	ga_threadmode = 1;
	ga_hThread  = CreateThread(NULL , 0 , ga_ThreadFunc  , NULL , 0 , &dwID);
}
void np2vga_shutdown()
{
	ga_exitThread = 1;
	WaitForSingleObject(ga_hThread, 8000);
	bmpInfo.bmiHeader.biWidth = bmpInfo.bmiHeader.biHeight = 0;
	ga_hThread = NULL;
	ReleaseDC(g_hWndVGA, g_hDC);
	DestroyWindow(g_hWndVGA);
}

DisplayState *graphic_console_init(vga_hw_update_ptr update,
                                   vga_hw_invalidate_ptr invalidate,
                                   vga_hw_screen_dump_ptr screen_dump,
                                   vga_hw_text_update_ptr text_update,
								   void *opaque)
{
	ds.opaque = opaque;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = 1024;
	bmpInfo.bmiHeader.biHeight = 512;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;
	g_memptr = mem;
	np2vga_ds_surface.width = 640;
	np2vga_ds_surface.height = 480;
	np2vga_ds_surface.pf.bits_per_pixel = 32;
	np2vga_ds_surface.pf.bytes_per_pixel = 4;

	return &ds;
}

void np2vga_resetRelay()
{
	ShowWindow(g_hWndVGA, SW_HIDE);
	if(ga_relay){
		soundmng_pcmplay(SOUND_RELAY1, FALSE);
		ga_relay = 0;
	}
}

#endif	/* SUPPORT_CL_GD5430 */
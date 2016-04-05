/**
 * @file	wab.c
 * @brief	Window Accelerator Board Interface
 *
 * @author	$Author: SimK $
 */

#include "compiler.h"

#if defined(SUPPORT_CL_GD5430)

#include "np2.h"
#include "resource.h"
#include "dosio.h"
#include "cpucore.h"
#include "pccore.h"
#include "iocore.h"
#include "wab.h"
#include "joymng.h"
#include "mousemng.h"
#include "scrnmng.h"
#include "soundmng.h"
#include "sysmng.h"
#include "winkbd.h"
#include "winloc.h"

	NP2WAB np2wab;

TCHAR	g_Name[100] = _T("NP2 Window Accelerator");

static HANDLE	ga_hThread = NULL;
static int		ga_exitThread = 0;
static int		ga_threadmode = 1;
static int		ga_scrnmode = 0;

void np2wab_setScreenSize(int width, int height)
{
	RECT rect = { 0, 0, width, height };
	np2wab.wndWidth = width;
	np2wab.wndHeight = height;
	AdjustWindowRectEx( &rect, WS_OVERLAPPEDWINDOW, FALSE, 0 );
	SetWindowPos( np2wab.hWndWAB, NULL, 0, 0, rect.right-rect.left, rect.bottom-rect.top, SWP_NOMOVE|SWP_NOZORDER );
}

void np2wab_resetscreensize()
{
	RECT rect = {0};
	rect.right = np2wab.wndWidth = np2wab.realWidth;
	rect.bottom = np2wab.wndHeight = np2wab.realHeight;
	AdjustWindowRectEx( &rect, WS_OVERLAPPEDWINDOW, FALSE, 0 );
	SetWindowPos( np2wab.hWndWAB, NULL, 0, 0, rect.right-rect.left, rect.bottom-rect.top, SWP_NOMOVE|SWP_NOZORDER );
}

static void changescreen(UINT8 newmode) {

	UINT8		change;
	UINT8		renewal;

	change = ga_scrnmode ^ newmode;
	renewal = (change & SCRNMODE_FULLSCREEN);
	ga_scrnmode = newmode;
	if(change){
		// ƒtƒ‹ƒXƒNƒŠ[ƒ“‚Í^–Ê–Ú‚Éì‚ç‚ñ‚Æ‚¢‚©‚ñ‚Ë
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
	RECT		rc;

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
			np2wab.wndWidth = rc.right - rc.left;
			np2wab.wndHeight = rc.bottom - rc.top;
			break;

		case WM_EXITSIZEMOVE:
			break;

		case WM_KEYDOWN:
			SendMessage(np2wab.hWndMain, msg, wParam, lParam); // •KŽEŠÛ“Š‚°
			break;

		case WM_KEYUP:
			SendMessage(np2wab.hWndMain, msg, wParam, lParam); // •KŽEŠÛ“Š‚°
			break;

		case WM_SYSKEYDOWN:
			//if (lParam & 0x20000000) {
			//	if ((wParam == VK_RETURN)) {
			//		changescreen(ga_scrnmode ^ SCRNMODE_FULLSCREEN);
			//		break;
			//	}
			//}
			SendMessage(np2wab.hWndMain, msg, wParam, lParam); // •KŽEŠÛ“Š‚°
			break;

		case WM_SYSKEYUP:
			SendMessage(np2wab.hWndMain, msg, wParam, lParam); // •KŽEŠÛ“Š‚°
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
			SetForegroundWindow(np2wab.hWndMain);
			SendMessage(np2wab.hWndMain, msg, wParam, lParam);
			break;
		case WM_LBUTTONDBLCLK:
			np2wab_resetscreensize();
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


// “¯Šú•`‰æiga_threadmode‚ª‹Uj
void np2wab_drawframe()
{
	if(!ga_threadmode){
		np2wab.drawframe();
	}
}
// ”ñ“¯Šú•`‰æiga_threadmode‚ª^j
DWORD WINAPI ga_ThreadFunc(LPVOID vdParam) {
	DWORD time = GetTickCount();
	int timeleft = 0;
	while (!ga_exitThread && ga_threadmode) {
		if(np2wab.drawframe!=NULL){
			np2wab.drawframe();
			while(GetTickCount() >= time && GetTickCount()-time < 16){
				timeleft = 16 - (GetTickCount()-time);
				if(timeleft>0) Sleep(timeleft);
			}
			time = GetTickCount();
		}else{
			Sleep(1000);
		}
	}
	ga_threadmode = 0;
	return 0;
}

static void IOOUTCALL np2wab_ofac(UINT port, REG8 dat) {
	TRACEOUT(("WAB: out FACh set relay %04X d=%02X", port, dat));
	dat = dat & ~0xfc;
	if(np2wab.relay != dat){
		np2wab_setRelayState(dat);
	}
	(void)port;
	(void)dat;
}
static REG8 IOINPCALL np2wab_ifac(UINT port) {
	TRACEOUT(("WAB: inp FACh get relay %04X", port));
	return 0xfc | np2wab.relay;
}

void np2wab_init(HINSTANCE hInstance, HWND hWndMain)
{
	WNDCLASSEX wcex = {0};
	DWORD dwID;

	np2wab.hWndMain = hWndMain;
	
	wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wcex.lpszClassName = (TCHAR*)g_Name;
	
	if(!RegisterClassEx(&wcex)) return;
	
	if(!(np2wab.hWndWAB = CreateWindow(g_Name, g_Name, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL))) return;

	np2wab.hDCWAB = GetDC(np2wab.hWndWAB);

	ga_threadmode = 1;
	ga_hThread  = CreateThread(NULL , 0 , ga_ThreadFunc  , NULL , 0 , &dwID);
}
void np2wab_reset(const NP2CFG *pConfig)
{

}
void np2wab_bind(void)
{
	iocore_attachout(0xfac, np2wab_ofac);
	iocore_attachinp(0xfac, np2wab_ifac);
}
void np2wab_shutdown()
{
	ga_exitThread = 1;
	WaitForSingleObject(ga_hThread, INFINITE);
	ga_hThread = NULL;
	ReleaseDC(np2wab.hWndWAB, np2wab.hDCWAB);
	DestroyWindow(np2wab.hWndWAB);
}

void np2wab_setRelayState(REG8 state)
{
	if((np2wab.relay & 0x3) != (state & 0x3)){
		if(state&0x3){
			if(!np2cfg.wabasw) soundmng_pcmplay(SOUND_RELAY1, FALSE);
			ShowWindow(np2wab.hWndWAB, SW_SHOWNOACTIVATE);
			SetWindowPos(np2wab.hWndWAB, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOSENDCHANGING | SWP_SHOWWINDOW);
		}else{
			if(!np2cfg.wabasw) soundmng_pcmplay(SOUND_RELAY1, FALSE);
			ShowWindow(np2wab.hWndWAB, SW_HIDE);
		}
		np2wab.relay = state & 0x3;
	}
}

#endif	/* SUPPORT_CL_GD5430 */
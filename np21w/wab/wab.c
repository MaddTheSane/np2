/**
 * @file	wab.c
 * @brief	Window Accelerator Board Interface
 *
 * @author	$Author: SimK $
 */

#include "compiler.h"

#if defined(SUPPORT_WAB)

#include "np2.h"
#include "resource.h"
#include "dosio.h"
#include "cpucore.h"
#include "pccore.h"
#include "iocore.h"
#include "joymng.h"
#include "mousemng.h"
#include "scrnmng.h"
#include "soundmng.h"
#include "sysmng.h"
#include "winkbd.h"
#include "winloc.h"
#include "profile.h"
#include "ini.h"
#include "dispsync.h"
#include "wab.h"

NP2WAB np2wab = {0};

TCHAR	g_Name[100] = _T("NP2 Window Accelerator");

	NP2WABCFG		np2wabcfg;
	
static HINSTANCE ga_hInstance = NULL;
static HANDLE	ga_hThread = NULL;
static int		ga_exitThread = 0;
static int		ga_threadmode = 1;
static int		ga_lastwabwidth = 640;
static int		ga_lastwabheight = 480;
static int		ga_reqChangeWindowSize = 0;
static int		ga_reqChangeWindowSize_w = 0;
static int		ga_reqChangeWindowSize_h = 0;

/**
 * 設定
 */
static const PFTBL s_wabwndini[] =
{
	PFVAL("WindposX", PFTYPE_SINT32,	&np2wabcfg.posx),
	PFVAL("WindposY", PFTYPE_SINT32,	&np2wabcfg.posy),
	PFVAL("MULTIWND", PFTYPE_BOOL,		&np2wabcfg.multiwindow),
	PFVAL("MULTHREAD",PFTYPE_BOOL,		&np2wabcfg.multithread),
	PFVAL("HALFTONE", PFTYPE_BOOL,		&np2wabcfg.halftone),
};

/**
 * 設定読み込み
 */
static void wabwin_readini()
{
	OEMCHAR szPath[MAX_PATH];

	ZeroMemory(&np2wabcfg, sizeof(np2wabcfg));
	np2wabcfg.posx = CW_USEDEFAULT;
	np2wabcfg.posy = CW_USEDEFAULT;
	np2wabcfg.multiwindow = 0;
	np2wabcfg.multithread = 0;
	np2wabcfg.halftone = 0;

	initgetfile(szPath, NELEMENTS(szPath));
	ini_read(szPath, g_Name, s_wabwndini, NELEMENTS(s_wabwndini));
}

/**
 * 設定書き込み
 */
static void wabwin_writeini()
{
	OEMCHAR szPath[MAX_PATH];
	initgetfile(szPath, NELEMENTS(szPath));
	ini_write(szPath, g_Name, s_wabwndini, NELEMENTS(s_wabwndini));
}

//void np2wab_updateWABChildWindowSize()
//{
//	static RECT lastrect = {0};
//	static int laststyle = 0;
//	if(!np2wab.multiwindow){
//		RECT r;
//		int style = GetWindowLong(np2wab.hWndMain, GWL_STYLE);
//		GetClientRect(np2wab.hWndMain, &r);
//		if(lastrect.right!=r.right || lastrect.bottom!=r.bottom || style!=laststyle){
//			if(style & WS_CAPTION){
//				SetWindowPos(
//						np2wab.hWndWAB, 0, 
//						1, 1, 
//						(r.right-r.left)-2, (r.bottom-r.top)-2, 
//						SWP_NOACTIVATE
//					);
//			}else{
//				// XXX: キャプションがないときは多分フルスクリーンなので。
//				SetWindowPos(
//						np2wab.hWndWAB, 0, 
//						0, 0, 
//						(r.right-r.left), (r.bottom-r.top), 
//						SWP_NOACTIVATE
//					);
//			}
//			if(np2wab.relay&0x3){
//				HDC hdc;
//				RECT rect;
//				GetClientRect(np2wab.hWndMain, &rect);
//				hdc = GetDC(np2wab.hWndMain);
//				SelectObject(hdc, GetStockObject(NULL_BRUSH));
//				SelectObject(hdc, GetStockObject(BLACK_PEN));
//				Rectangle(hdc, rect.top, rect.left, rect.right, rect.bottom);
//				ReleaseDC(np2wab.hWndMain, hdc);
//			}
//			lastrect = r;
//			laststyle = style;
//		}
//	}
//}

void np2wab_setScreenSize(int width, int height)
{
	if(np2wab.multiwindow){
		RECT rect = { 0, 0, width, height };
		np2wab.wndWidth = width;
		np2wab.wndHeight = height;
		AdjustWindowRectEx( &rect, WS_OVERLAPPEDWINDOW, FALSE, 0 );
		SetWindowPos( np2wab.hWndWAB, NULL, 0, 0, rect.right-rect.left, rect.bottom-rect.top, SWP_NOMOVE|SWP_NOZORDER );
	}else{
		np2wab.wndWidth = ga_lastwabwidth = width;
		np2wab.wndHeight = ga_lastwabheight = height;
		if(np2wab.relay & 0x3){
			scrnmng_setwidth(0, width);
			scrnmng_setheight(0, height);
			scrnmng_updatefsres();
			mousemng_updateclip(); // マウスキャプチャのクリップ範囲を修正
			//np2wab_updateWABChildWindowSize();
		}
	}
	np2wab.paletteChanged = 1;
}
// マルチスレッド用（すぐに更新できない場合はnp2wab.ready=0に）
void np2wab_setScreenSizeMT(int width, int height)
{
	if(!ga_threadmode){
		np2wab_setScreenSize(width, height);
	}else{
		ga_reqChangeWindowSize_w = width;
		ga_reqChangeWindowSize_h = height;
		ga_reqChangeWindowSize = 1;
		np2wab.ready = 0; // 更新待ち
	}
}

void np2wab_resetscreensize()
{
	int style = GetWindowLong(np2wab.hWndMain, GWL_STYLE);
	if(style & WS_CAPTION){
		if(np2wab.multiwindow){
			RECT rect = {0};
			rect.right = np2wab.wndWidth = np2wab.realWidth;
			rect.bottom = np2wab.wndHeight = np2wab.realHeight;
			AdjustWindowRectEx( &rect, WS_OVERLAPPEDWINDOW, FALSE, 0 );
			SetWindowPos( np2wab.hWndWAB, NULL, 0, 0, rect.right-rect.left, rect.bottom-rect.top, SWP_NOMOVE|SWP_NOZORDER );
		}else{
		}
	}else{
		RECT rect;
		GetClientRect(np2wab.hWndMain, &rect);
		np2wab.wndWidth = rect.right-rect.left;
		np2wab.wndHeight = rect.bottom-rect.top;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
	RECT		rc;

	switch (msg) {
		case WM_CREATE:
			break;

		case WM_MOVE:
			GetWindowRect(hWnd, &rc);
			if(np2wab.multiwindow){
				np2wabcfg.posx = rc.left;
				np2wabcfg.posy = rc.top;
			}
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
			SendMessage(np2wab.hWndMain, msg, wParam, lParam); // 必殺丸投げ
			break;

		case WM_KEYUP:
			SendMessage(np2wab.hWndMain, msg, wParam, lParam); // 必殺丸投げ
			break;

		case WM_SYSKEYDOWN:
			SendMessage(np2wab.hWndMain, msg, wParam, lParam); // 必殺丸投げ
			break;

		case WM_SYSKEYUP:
			SendMessage(np2wab.hWndMain, msg, wParam, lParam); // 必殺丸投げ
			break;

		case WM_MOUSEMOVE:
			SendMessage(np2wab.hWndMain, msg, wParam, lParam); // 必殺丸投げ
			break;

		case WM_LBUTTONDOWN:
			if(!np2wab.multiwindow){
				SendMessage(np2wab.hWndMain, msg, wParam, lParam);
			}
			break;

		case WM_LBUTTONUP:
			if(!np2wab.multiwindow){
				SendMessage(np2wab.hWndMain, msg, wParam, lParam);
			}
			break;

		case WM_RBUTTONDOWN:
			if(!np2wab.multiwindow){
				SendMessage(np2wab.hWndMain, msg, wParam, lParam);
			}
			break;

		case WM_RBUTTONUP:
			if(!np2wab.multiwindow){
				SendMessage(np2wab.hWndMain, msg, wParam, lParam);
			}
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
			break;

		default:
			return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return(0L);
}

// WAB窓に転送
static int ga_lastscalemode = 0;
static int ga_lastrealwidth = 0;
static int ga_lastrealheight = 0;
void np2wab_drawWABWindow(HDC hdc)
{
	int scalemode = 0;
	int srcwidth = np2wab.realWidth;
	int srcheight = np2wab.realHeight;
	if(ga_lastrealwidth != srcwidth || ga_lastrealheight != srcheight){
		// 解像度が変わっていたらウィンドウサイズも変える
		np2wab.paletteChanged = 1;
		np2wab_setScreenSizeMT(srcwidth, srcheight);
		ga_lastrealwidth = srcwidth;
		ga_lastrealheight = srcheight;
		if(!np2wab.ready) return;
	}
	if(np2wab.multiwindow){
		scalemode = np2wab.wndWidth!=srcwidth || np2wab.wndHeight!=srcheight;
		if(ga_lastscalemode!=scalemode){
			if(scalemode){
				SetStretchBltMode(np2wab.hDCWAB, np2wabcfg.halftone ? HALFTONE : COLORONCOLOR);
				SetBrushOrgEx(np2wab.hDCWAB , 0 , 0 , NULL);
			}else{
				SetStretchBltMode(np2wab.hDCWAB, BLACKONWHITE);
			}
			ga_lastscalemode = scalemode;
			np2wab.paletteChanged = 1;
		}
		if(scalemode){
			if(np2wab.wndWidth * srcheight > srcwidth * np2wab.wndHeight){
				// 横長
				int dstw = srcwidth * np2wab.wndHeight / srcheight;
				int dsth = np2wab.wndHeight;
				int mgnw = (np2wab.wndWidth - dstw);
				int shx = 0;
				if(mgnw&0x1) shx = 1;
				mgnw = mgnw>>1;
				BitBlt(np2wab.hDCWAB, 0, 0, mgnw, np2wab.wndHeight, NULL, 0, 0, BLACKNESS);
				BitBlt(np2wab.hDCWAB, np2wab.wndWidth-mgnw-shx, 0, mgnw+shx, np2wab.wndHeight, NULL, 0, 0, BLACKNESS);
				StretchBlt(np2wab.hDCWAB, mgnw, 0, dstw, dsth, np2wab.hDCBuf, 0, 0, srcwidth, srcheight, SRCCOPY);
			}else if(np2wab.wndWidth * srcheight < srcwidth * np2wab.wndHeight){
				// 縦長
				int dstw = np2wab.wndWidth;
				int dsth = srcheight * np2wab.wndWidth / srcwidth;
				int mgnh = (np2wab.wndHeight - dsth);
				int shy = 0;
				if(mgnh&0x1) shy = 1;
				mgnh = mgnh>>1;
				BitBlt(np2wab.hDCWAB, 0, 0, np2wab.wndWidth, mgnh, NULL, 0, 0, BLACKNESS);
				BitBlt(np2wab.hDCWAB, 0, np2wab.wndHeight-mgnh-shy, np2wab.wndWidth, mgnh+shy, NULL, 0, 0, BLACKNESS);
				StretchBlt(np2wab.hDCWAB, 0, mgnh, dstw, dsth, np2wab.hDCBuf, 0, 0, srcwidth, srcheight, SRCCOPY);
			}else{
				StretchBlt(np2wab.hDCWAB, 0, 0, np2wab.wndWidth, np2wab.wndHeight, np2wab.hDCBuf, 0, 0, srcwidth, srcheight, SRCCOPY);
			}
		}else{
			BitBlt(np2wab.hDCWAB, 0, 0, srcwidth, srcheight, np2wab.hDCBuf, 0, 0, SRCCOPY);
		}
	}else{
		scrnmng_blthdc(np2wab.hDCBuf);
	}
}

// 同期描画（ga_threadmodeが偽）
void np2wab_drawframe()
{
	if(!ga_threadmode){
		if(np2wab.ready && np2wab.hWndWAB!=NULL && (np2wab.relay&0x3)!=0){
			//np2wab_updateWABChildWindowSize();
			np2wab.drawframe();
			np2wab_drawWABWindow(np2wab.hDCBuf);
		}
	}else{
		if(np2wab.hWndWAB!=NULL){
			if(ga_reqChangeWindowSize){
				ga_reqChangeWindowSize = 0;
				np2wab_setScreenSize(ga_reqChangeWindowSize_w, ga_reqChangeWindowSize_h);
				np2wab.ready = 1;
			}else{
				//np2wab_updateWABChildWindowSize();
			}
			if(np2wab.ready && (np2wab.relay&0x3)!=0){
				if(!np2wab.multiwindow){
					// 画面転送だけメインスレッドで
					np2wab_drawWABWindow(np2wab.hDCBuf);
				}
				ResumeThread(ga_hThread);
			}
		}
	}
}
// 非同期描画（ga_threadmodeが真）
DWORD WINAPI ga_ThreadFunc(LPVOID vdParam) {
	DWORD time = GetTickCount();
	int timeleft = 0;
	while (!ga_exitThread && ga_threadmode) {
		if(np2wab.ready && np2wab.hWndWAB!=NULL && np2wab.drawframe!=NULL && (np2wab.relay&0x3)!=0){
			np2wab.drawframe();
			if(np2wab.multiwindow){
				// 別窓モードは画面転送も別スレッドで
				np2wab_drawWABWindow(np2wab.hDCBuf); 
			}
			// 画面転送待ち
			if(!ga_exitThread) SuspendThread(ga_hThread);
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
	wabwin_readini();

	ga_hInstance = hInstance;
	np2wab.hWndMain = hWndMain;
}
void np2wab_reset(const NP2CFG *pConfig)
{

}
void np2wab_bind(void)
{
	DWORD dwID;
	WNDCLASSEX wcex = {0};
	HDC hdc;
	
	if(ga_threadmode){
		ga_exitThread = 1;
		ResumeThread(ga_hThread);
		while(WaitForSingleObject(ga_hThread, 200)==WAIT_TIMEOUT){
			ResumeThread(ga_hThread);
		}
		ga_hThread = NULL;
		ga_exitThread = 0;
	}
	np2wab.ready = 0;
	ga_lastscalemode = 0;
	ga_lastrealwidth = 0;
	ga_lastrealheight = 0;

	np2wab.multiwindow = np2wabcfg.multiwindow;
	ga_threadmode = np2wabcfg.multithread;
	np2wab.wndWidth = 640;
	np2wab.wndHeight = 480;
	np2wab.fps = 60;
	np2wab.paletteChanged = 1;
	ga_lastwabwidth = 640;
	ga_lastwabheight = 480;
	ga_reqChangeWindowSize = 0;
	
	if(np2wab.hWndWAB){
		DeleteDC(np2wab.hDCBuf);
		DeleteObject(np2wab.hBmpBuf);
		DestroyWindow(np2wab.hWndWAB);
		np2wab.hWndWAB = NULL;
		UnregisterClass((TCHAR*)g_Name, ga_hInstance);
	}

	wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | (np2wab.multiwindow ? CS_DBLCLKS : 0);
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = ga_hInstance;
    wcex.hIcon = LoadIcon(ga_hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
    wcex.lpszClassName = (TCHAR*)g_Name;
	
	if(!RegisterClassEx(&wcex)) return;
	//np2wab.hWndWAB = CreateWindowEx(
	//		np2wab.multiwindow ? 0 : WS_EX_TRANSPARENT, 
	//		g_Name, g_Name, 
	//		np2wab.multiwindow ? WS_OVERLAPPEDWINDOW : WS_CHILD,
	//		np2wab.multiwindow ? np2wabcfg.posx : 0, np2wab.multiwindow ? np2wabcfg.posy : 0, 
	//		640, 480, 
	//		np2wab.multiwindow ? NULL : np2wab.hWndMain, 
	//		NULL, ga_hInstance, NULL
	//	);
	np2wab.hWndWAB = CreateWindowEx(
			0, 
			g_Name, g_Name, 
			WS_OVERLAPPEDWINDOW,
			0, 0, 
			640, 480, 
			np2wab.multiwindow ? NULL : np2wab.hWndMain, 
			NULL, ga_hInstance, NULL
		);
	if(!np2wab.hWndWAB) return;

	np2wab.hDCWAB = GetDC(np2wab.hWndWAB);
	hdc = np2wab.multiwindow ? GetDC(NULL) : np2wab.hDCWAB;
	np2wab.hBmpBuf = CreateCompatibleBitmap(hdc, 1024, 768); // XXX: 1024x768以上にならないのでこれで十分
	np2wab.hDCBuf = CreateCompatibleDC(hdc);
	SelectObject(np2wab.hDCBuf, np2wab.hBmpBuf);

	iocore_attachout(0xfac, np2wab_ofac);
	iocore_attachinp(0xfac, np2wab_ifac);
	
	if(ga_threadmode){
		ga_hThread  = CreateThread(NULL , 0 , ga_ThreadFunc  , NULL , 0 , &dwID);
	}

	np2wab.ready = 1;
}
void np2wab_shutdown()
{
	ga_exitThread = 1;
	ResumeThread(ga_hThread);
	while(WaitForSingleObject(ga_hThread, 200)==WAIT_TIMEOUT){
		ResumeThread(ga_hThread);
	}
	ga_hThread = NULL;
	DeleteDC(np2wab.hDCBuf);
	DeleteObject(np2wab.hBmpBuf);
	ReleaseDC(np2wab.hWndWAB, np2wab.hDCWAB);
	np2wab.hDCWAB = NULL;
	DestroyWindow(np2wab.hWndWAB);
	np2wab.hWndWAB = NULL;

	wabwin_writeini();
}

void np2wab_setRelayState(REG8 state)
{
	if((np2wab.relay & 0x3) != (state & 0x3)){
		np2wab.relay = state & 0x3;
		if(state&0x3){
			if(!np2cfg.wabasw) soundmng_pcmplay(SOUND_RELAY1, FALSE);
			if(np2wab.multiwindow){
				ShowWindow(np2wab.hWndWAB, SW_SHOWNOACTIVATE);
				SetWindowPos(np2wab.hWndWAB, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOSENDCHANGING | SWP_SHOWWINDOW);
			}else{
				np2wab_setScreenSize(ga_lastwabwidth, ga_lastwabheight);
			}
		}else{
			if(!np2cfg.wabasw) soundmng_pcmplay(SOUND_RELAY1, FALSE);
			if(np2wab.multiwindow){
				ShowWindow(np2wab.hWndWAB, SW_HIDE);
			}else{
				scrnmng_setwidth(dsync.scrnxpos, dsync.scrnxmax);
				scrnmng_setheight(0, dsync.scrnymax);
				scrnmng_updatefsres();
				mousemng_updateclip(); // マウスキャプチャのクリップ範囲を修正
			}
		}
	}
}


#endif	/* SUPPORT_WAB */
/**
 * @file	wab.h
 * @brief	Window Accelerator Board Interface
 *
 * @author	$Author: SimK $
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void NP2WAB_DrawFrame();
typedef struct {
	HWND hWndMain; // メインウィンドウのハンドル
	HWND hWndWAB; // ウィンドウアクセラレータのハンドル
	HDC hDCWAB;
	REG8 relay;
	int realWidth;
	int realHeight;
	int wndWidth;
	int wndHeight;
	NP2WAB_DrawFrame *drawframe;
} NP2WAB;

void np2wab_init(HINSTANCE hInstance, HWND g_hWndMain);
void np2wab_reset(const NP2CFG *pConfig);
void np2wab_bind(void);
void np2wab_drawframe();
void np2wab_shutdown();

void np2wab_setRelayState(REG8 state);
void np2wab_setScreenSize(int width, int height);

extern NP2WAB np2wab;

#ifdef __cplusplus
}
#endif


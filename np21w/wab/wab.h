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
	
typedef struct {
	int		posx;
	int		posy;
	int		multiwindow;
	int		multithread;
	int		halftone;
	int		forcevga;
} NP2WABCFG;

typedef void NP2WAB_DrawFrame();
typedef struct {
	int ready; // 0以外なら描いても良いよ
	int multiwindow; // 別窓モード
	HWND hWndMain; // メインウィンドウのハンドル
	HWND hWndWAB; // ウィンドウアクセラレータ別窓のハンドル
	HDC hDCWAB; // ウィンドウアクセラレータ別窓のHDC
	HBITMAP hBmpBuf; // バッファビットマップ（常に等倍）
	HDC     hDCBuf; // バッファのHDC
	REG8 relay; // 画面出力リレー状態（bit0=内蔵ウィンドウアクセラレータ, bit1=RGB INスルー, それ以外のビットはReserved。bit0,1が00で98グラフィック）
	REG8 paletteChanged; // パレット要更新フラグ
	int realWidth; // 画面解像度(幅)
	int realHeight; // 画面解像度(高さ)
	int wndWidth; // 描画領域サイズ(幅)
	int wndHeight; // 描画領域サイズ(高さ)
	int fps; // リフレッシュレート（大体合わせてくれるかもしれない）
	NP2WAB_DrawFrame *drawframe; // 画面描画関数。hDCBufにアクセラレータ画面データを転送する。

	int vramoffs;
} NP2WAB;

void np2wab_init(HINSTANCE hInstance, HWND g_hWndMain);
void np2wab_reset(const NP2CFG *pConfig);
void np2wab_bind(void);
void np2wab_drawframe();
void np2wab_shutdown();

void np2wab_setRelayState(REG8 state);
void np2wab_setScreenSize(int width, int height);
void np2wab_setScreenSizeMT(int width, int height);

extern NP2WAB np2wab;
extern NP2WABCFG np2wabcfg;

#ifdef __cplusplus
}
#endif


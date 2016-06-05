/**
 * @file	dd2.h
 * @brief	DirectDraw2 描画クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include <ddraw.h>
#include "cmndraw.h"

/**
 * @brief DirectDraw2 class
 */
class DDraw2
{
public:
	DDraw2();
	virtual ~DDraw2();

	bool Create(HWND hWnd, int nWidth, int nHeight, UINT nBpp = 0);
	void Destroy();
	LPDIRECTDRAWSURFACE CreateBackSurface(int nWidth, int nHeight);
	void Blt(LPDIRECTDRAWSURFACE pSurface, const POINT* pt, const RECT* lpRect = NULL);
	void QueryPalette();
	RGB16 GetPalette16(RGB32 pal) const;
	operator LPDIRECTDRAW2();
	UINT GetBpp() const;

protected:
	HWND					m_hWnd;				/*!< ウィンドウ ハンドル */
	LPDIRECTDRAW			m_pDDraw;			/*!< DirectDraw インスタンス */
	LPDIRECTDRAW2			m_pDDraw2;			/*!< DirectDraw2 インスタンス */
	LPDIRECTDRAWSURFACE		m_pPrimarySurface;	/*!< プライマリ サーフェス */
	LPDIRECTDRAWCLIPPER		m_pClipper;			/*!< クリッパー */
	LPDIRECTDRAWPALETTE		m_pPalette;			/*!< パレット */
	UINT					m_nBpp;				/*!< BPP */
	bool					m_bFullscreen;		/*!< フルスクリーン */
	RGB32					m_pal16;			/*!< 16BPPマスク */
	UINT8					m_r16b;				/*!< B シフト量 */
	UINT8					m_l16r;				/*!< R シフト量 */
	UINT8					m_l16g;				/*!< G シフト量 */
	PALETTEENTRY			m_pal[256];			/*!< パレット */

	void InitializePalette();
	virtual void OnInitializePalette(LPPALETTEENTRY pPalette, UINT nPalettes);
	void Make16Mask(DWORD dwBBitMask, DWORD dwRBitMask, DWORD dwGBitMask);
};

/**
 * DirectDraw2 インスタンスを得る
 * @return DirectDraw2 インスタンス
 */
inline DDraw2::operator LPDIRECTDRAW2()
{
	return m_pDDraw2;
}

/**
 * BPPを得る
 * @return BPP
 */
inline UINT DDraw2::GetBpp() const
{
	return m_nBpp;
}

/**
 * @brief DirectDraw2 surface class
 */
class DDraw2Surface
{
public:
	DDraw2Surface();
	~DDraw2Surface();
	bool Create(DDraw2& dd2, int nWidth, int nHeight);
	void Destroy();
	CMNVRAM* Lock();
	void Unlock();
	operator LPDIRECTDRAWSURFACE();

protected:
	LPDIRECTDRAWSURFACE		m_pBackSurface;		/*!< バック サーフェス */
	CMNVRAM					m_vram;				/*!< VRAM */
};

/**
 * バックサーフェスを得る
 * @return バックサーフェス
 */
inline DDraw2Surface::operator LPDIRECTDRAWSURFACE()
{
	return m_pBackSurface;
}

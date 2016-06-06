/**
 * @file	scrnmng.h
 * @brief	スクリーン マネージャ クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

enum
{
	RGB24_B	= 0,
	RGB24_G	= 1,
	RGB24_R	= 2
};

typedef struct {
	UINT8	*ptr;
	int		xalign;
	int		yalign;
	int		width;
	int		height;
	UINT	bpp;
	int		extend;
} SCRNSURF;

enum
{
	SCRNMODE_FULLSCREEN		= 0x01,
	SCRNMODE_HIGHCOLOR		= 0x02,
	SCRNMODE_ROTATE			= 0x10,
	SCRNMODE_ROTATEDIR		= 0x20,
	SCRNMODE_ROTATELEFT		= (SCRNMODE_ROTATE + 0),
	SCRNMODE_ROTATERIGHT	= (SCRNMODE_ROTATE + SCRNMODE_ROTATEDIR),
	SCRNMODE_ROTATEMASK		= 0x30,
};

enum
{
	FSCRNMOD_NORESIZE		= 0x00,
	FSCRNMOD_ASPECTFIX8		= 0x01,
	FSCRNMOD_ASPECTFIX		= 0x02,
	FSCRNMOD_LARGE			= 0x03,
	FSCRNMOD_ASPECTMASK		= 0x03,
	FSCRNMOD_SAMERES		= 0x04,
	FSCRNMOD_SAMEBPP		= 0x08
};

#ifdef __cplusplus
extern "C"
{
#endif

void scrnmng_setwidth(int posx, int width);
void scrnmng_setextend(int extend);
void scrnmng_setheight(int posy, int height);
BOOL scrnmng_isfullscreen(void);
BOOL scrnmng_haveextend(void);
UINT scrnmng_getbpp(void);
const SCRNSURF *scrnmng_surflock(void);
void scrnmng_surfunlock(const SCRNSURF *surf);
void scrnmng_update(void);
void scrnmng_allflash(void);
void scrnmng_palchanged(void);
RGB16 scrnmng_makepal16(RGB32 pal32);

#ifdef __cplusplus

// ---- for windows

#include "subwnd\dd2.h"

/**
 * @brief スクリーン マネージャ
 */
class CScreenManager : public DDraw2
{
public:
	static CScreenManager* GetInstance();

	static void Initialize(HWND hWnd);
	CScreenManager();
	bool Create(HWND hWnd, UINT8 scrnmode);
	void Destroy();
	void OnMouseMove(const POINT& pt);
	void EnableUI();
	void DisableUI();
	void SetWidth(int nWidth);
	void SetExtend(int nExtend);
	void SetHeight(int nHeight);
	void SetMultiple(int nMultiple);
	int GetMultiple() const;
	const SCRNSURF* Lock();
	void Unlock(const SCRNSURF *surf);
	void Update();
	bool HasExtendColumn() const;
	void AllFlash();
	void ChangePalette();
	void EnterSizing();

protected:
	static CScreenManager sm_instance;		/*!< インスタンス */
	UINT m_nScreenMode;						/*!< 画面モード */
	int m_nWidth;							/*!< 幅 */
	int m_nExtend;							/*!< 拡張幅 */
	int m_nHeight;							/*!< 高さ */
	int m_nMultiple;						/*!< 拡大率 */
	int m_nCliping;							/*!< クリッピング回数 */
	bool m_bHasExtendColumn;				/*!< 拡張カラムを持つか? */
	bool m_bDisplayedMenu;					/*!< メニュー表示中か? */
	bool m_bAllFlash;						/*!< フラッシュ要求 */
	bool m_bChangedPalette;					/*!< パレット変更 */
	int m_nMenuHeight;						/*!< メニューの高さ */
	RECT m_rcProjection;					/*!< 投影領域 */
	RECT m_rcSurface;						/*!< サーフェス領域 */
	RECT				scrnclip;
	RECT				rectclip;
	LPDIRECTDRAWSURFACE m_pBackSurface;		/*!< バックサーフェス */
	SCRNSURF m_scrnsurf;					/*!< バックサーフェス情報 */

	void RenewalClientSize(bool bWndLoc);
	void ClearOutOfRect(const RECT* target, const RECT* base);
	void ClearOutScreen();
	void ClearOutFullscreen();
	virtual void OnInitializePalette(LPPALETTEENTRY pPalette, UINT nPalettes);
	void PaletteSet();

#if defined(SUPPORT_DCLOCK)
public:
	bool IsDispClockClick(const POINT *pt) const;
	void DispClock();

private:
	LPDIRECTDRAWSURFACE m_pClockSurface;
#endif	// defined(SUPPORT_DCLOCK)
};

/**
 * インスタンスを取得
 * @return インスタンス
 */
inline CScreenManager* CScreenManager::GetInstance()
{
	return &sm_instance;
}

/**
 * ノーマルディスプレイ用の拡張カラムをサポートしているか?
 * @retval true サポート
 * @retval false 未サポート
 */
inline bool CScreenManager::HasExtendColumn() const
{
	return m_bHasExtendColumn;
}

/**
 * 倍率を取得
 * @return 倍率
 */
inline int CScreenManager::GetMultiple() const
{
	return m_nMultiple;
}

/**
 * フラッシュ要求
 */
inline void CScreenManager::AllFlash()
{
	m_bAllFlash = true;
}

/**
 * パレット更新要求
 */
inline void CScreenManager::ChangePalette()
{
	m_bChangedPalette = true;
}

void scrnmng_entersizing(void);
void scrnmng_sizing(UINT side, RECT *rect);
void scrnmng_exitsizing(void);

}
#endif

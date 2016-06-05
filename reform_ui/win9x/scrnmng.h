
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

typedef struct
{
	UINT8	allflash;
	UINT8	palchanged;
} SCRNMNG;


#ifdef __cplusplus
extern "C"
{
#endif

extern	SCRNMNG		g_scrnmng;			// マクロ用

void scrnmng_initialize(void);
BRESULT scrnmng_create(UINT8 scrnmode);
void scrnmng_destroy(void);

void scrnmng_setwidth(int posx, int width);
void scrnmng_setextend(int extend);
void scrnmng_setheight(int posy, int height);
BOOL scrnmng_isfullscreen(void);
BOOL scrnmng_haveextend(void);
UINT scrnmng_getbpp(void);
const SCRNSURF *scrnmng_surflock(void);
void scrnmng_surfunlock(const SCRNSURF *surf);
void scrnmng_update(void);

#define	scrnmng_allflash()		g_scrnmng.allflash = TRUE
#define	scrnmng_palchanged()	g_scrnmng.palchanged = TRUE

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

	CScreenManager();
	BRESULT Create(HWND hWnd, UINT8 scrnmode);
	void Destroy();
	void OnMouseMove(const POINT& pt);
	void EnableUI();
	void DisableUI();
	const SCRNSURF* Lock();
	void Unlock(const SCRNSURF *surf);
	void Update();
	bool HasExtendColumn() const;
	void EnterSizing();
	void RenewalClientSize(bool bWndLoc);

protected:
	static CScreenManager sm_instance;		/*!< インスタンス */

	LPDIRECTDRAWSURFACE	m_pBackSurface;		/*!< バック サーフェス */
	UINT				m_nScreenMode;
	int					m_nCliping;
	bool				m_bHasExtendColumn;
	bool				m_bDisplayedMenu;
	int					m_nMenuHeight;
	RECT				m_rcProjection;
	RECT				m_rcSurface;
	RECT				scrnclip;
	RECT				rectclip;
	SCRNSURF			m_scrnsurf;

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

void scrnmng_setmultiple(int multiple);
int scrnmng_getmultiple(void);

void scrnmng_entersizing(void);
void scrnmng_sizing(UINT side, RECT *rect);
void scrnmng_exitsizing(void);

}
#endif

/**
 * @file	scrnmng.cpp
 * @brief	スクリーン マネージャ クラスの動作の定義を行います
 */

#include "compiler.h"
#include "scrnmng.h"
#include <algorithm>
#ifndef __GNUC__
#include <winnls32.h>
#endif
#include "np2.h"
#include "winloc.h"
#include "mousemng.h"
#include "dialog\np2class.h"
#include "pccore.h"
#include "scrndraw.h"
#include "palettes.h"
#if defined(SUPPORT_DCLOCK)
#include "subwnd\dclock.h"
#endif
#include "recvideo.h"

//! 8BPP パレット数
#define PALLETES_8BPP	NP2PAL_TEXT3

extern WINLOCEX np2_winlocexallwin(HWND base);

/*! インスタンス */
CScreenManager CScreenManager::sm_instance;

static void setclientsize(HWND hWnd, int nWidth, int nHeight)
{
	nWidth += np2oscfg.paddingx * 2;
	nHeight += np2oscfg.paddingy * 2;
	winloc_setclientsize(hWnd, np2oscfg.winx, np2oscfg.winy, nWidth, nHeight);
}

/**
 * 初期化
 * @param[in] hWnd ウィンドウハンドル
 */
void CScreenManager::Initialize(HWND hWnd)
{
	setclientsize(hWnd, 640, 400);
}

/**
 * コンストラクタ
 */
CScreenManager::CScreenManager()
	: m_pBackSurface(NULL)
	, m_nScreenMode(0)
	, m_nWidth(640)
	, m_nExtend(1)
	, m_nHeight(400)
	, m_nMultiple(8)
	, m_nCliping(0)
	, m_bHasExtendColumn(false)
	, m_bDisplayedMenu(false)
	, m_bAllFlash(false)
	, m_bChangedPalette(false)
	, m_nMenuHeight(0)
#if defined(SUPPORT_DCLOCK)
	, m_pClockSurface(NULL)
#endif	// defined(SUPPORT_DCLOCK)
{
	ZeroMemory(&m_rcProjection, sizeof(m_rcProjection));
	ZeroMemory(&m_rcSurface, sizeof(m_rcSurface));
	ZeroMemory(&this->scrnclip, sizeof(this->scrnclip));
	ZeroMemory(&this->rectclip, sizeof(this->rectclip));
	ZeroMemory(&m_scrnsurf, sizeof(m_scrnsurf));
}

/**
 * クライアント領域の更新
 * @param[in] bWndLoc リサイズ時に他のウィンドウを移動するか?
 */
void CScreenManager::RenewalClientSize(bool bWndLoc)
{
	const int width = min(m_nWidth, m_szScreen.cx);
	const int height = min(m_nHeight, m_szScreen.cy);
	int extend = 0;

	// 描画範囲～
	if (m_nScreenMode & SCRNMODE_FULLSCREEN)
	{
		m_rcSurface.right = width;
		m_rcSurface.bottom = height;
		int scrnwidth = width;
		int scrnheight = height;
		const UINT fscrnmod = np2oscfg.fscrnmod & FSCRNMOD_ASPECTMASK;
		switch (fscrnmod)
		{
			default:
			case FSCRNMOD_NORESIZE:
				break;

			case FSCRNMOD_ASPECTFIX8:
				{
					const int nMultipleX = (m_szScreen.cx << 3) / width;
					const int nMultipleY = (m_szScreen.cy << 3) / height;
					const int nMultiple = min(nMultipleX, nMultipleY);
					scrnwidth = (width * nMultiple) >> 3;
					scrnheight = (height * nMultiple) >> 3;
				}
				break;

			case FSCRNMOD_ASPECTFIX:
				scrnwidth = m_szScreen.cx;
				scrnheight = (scrnwidth * height) / width;
				if (scrnheight >= m_szScreen.cy)
				{
					scrnheight = m_szScreen.cy;
					scrnwidth = (scrnheight * width) / height;
				}
				break;

			case FSCRNMOD_LARGE:
				scrnwidth = m_szScreen.cx;
				scrnheight = m_szScreen.cy;
				break;
		}
		m_rcProjection.left = (m_szScreen.cx - scrnwidth) / 2;
		m_rcProjection.top = (m_szScreen.cy - scrnheight) / 2;
		m_rcProjection.right = m_rcProjection.left + scrnwidth;
		m_rcProjection.bottom = m_rcProjection.top + scrnheight;

		// メニュー表示時の描画領域
		this->rectclip = m_rcSurface;
		this->scrnclip = m_rcProjection;
		if (this->scrnclip.top < m_nMenuHeight)
		{
			this->scrnclip.top = m_nMenuHeight;
			int tmpcy = m_szScreen.cy - m_nMenuHeight;
			if (scrnheight > tmpcy)
			{
				switch (fscrnmod)
				{
					default:
					case FSCRNMOD_NORESIZE:
						tmpcy = min(tmpcy, height);
						this->rectclip.bottom = tmpcy;
						break;

					case FSCRNMOD_ASPECTFIX8:
					case FSCRNMOD_ASPECTFIX:
						this->rectclip.bottom = (tmpcy * height) / scrnheight;
						break;

					case FSCRNMOD_LARGE:
						break;
				}
			}
			this->scrnclip.bottom = m_nMenuHeight + tmpcy;
		}
	}
	else
	{
		int scrnwidth = width;
		int scrnheight = height;
		int nExtendX = ((m_nMultiple == 8) && (m_bHasExtendColumn)) ? m_nExtend : 0;
		int nExtendY = 0;

		if (m_nScreenMode & SCRNMODE_ROTATE)
		{
			std::swap(scrnwidth, scrnheight);
			std::swap(nExtendX, nExtendY);
		}

		const int nPaddingX = max(np2oscfg.paddingx, 0);
		const int nPaddingY = max(np2oscfg.paddingy, 0);
		nExtendX = min(nExtendX, nPaddingX);
		nExtendY = min(nExtendY, nPaddingY);

		m_rcSurface.right = scrnwidth + nExtendX;
		m_rcSurface.bottom = scrnheight + nExtendY;

		scrnwidth = (scrnwidth * m_nMultiple) >> 3;
		scrnheight = (scrnheight * m_nMultiple) >> 3;
		m_rcProjection.left = nPaddingX - nExtendX;
		m_rcProjection.top = nPaddingY - nExtendY;
		m_rcProjection.right = nPaddingX + scrnwidth;
		m_rcProjection.bottom = nPaddingY + scrnheight;
		extend = nExtendX | nExtendY;

		WINLOCEX wlex = NULL;
		if (bWndLoc)
		{
			wlex = np2_winlocexallwin(m_hWnd);
		}
		winlocex_setholdwnd(wlex, m_hWnd);
		setclientsize(m_hWnd, scrnwidth, scrnheight);
		winlocex_move(wlex);
		winlocex_destroy(wlex);
	}
	m_scrnsurf.width = width;
	m_scrnsurf.height = height;
	m_scrnsurf.extend = extend;
}

/**
 * クリア
 * @param[in] target ターゲット
 * @param[in] base 領域
 */
void CScreenManager::ClearOutOfRect(const RECT *target, const RECT *base)
{
	RECT rect;
	rect.left = base->left;
	rect.right = base->right;
	rect.top = base->top;
	rect.bottom = target->top;
	if (rect.top < rect.bottom)
	{
		ZeroFill(&rect);
	}
	rect.top = target->bottom;
	rect.bottom = base->bottom;
	if (rect.top < rect.bottom)
	{
		ZeroFill(&rect);
	}

	rect.top = max(base->top, target->top);
	rect.bottom = min(base->bottom, target->bottom);
	if (rect.top < rect.bottom)
	{
		rect.left = base->left;
		rect.right = target->left;
		if (rect.left < rect.right)
		{
			ZeroFill(&rect);
		}
		rect.left = target->right;
		rect.right = base->right;
		if (rect.left < rect.right)
		{
			ZeroFill(&rect);
		}
	}
}

void CScreenManager::ClearOutScreen()
{
	RECT base;
	GetClientRect(m_hWnd, &base);

	ClearOutOfRect(&m_rcProjection, &base);
}

void CScreenManager::ClearOutFullscreen()
{
	RECT base;
	base.left = 0;
	base.top = 0;
	base.right = m_szScreen.cx;
	base.bottom = m_szScreen.cy;

	const RECT* scrn;
	if (GetWindowLongPtr(m_hWnd, NP2GWLP_HMENU))
	{
		scrn = &m_rcProjection;
		base.top = 0;
	}
	else
	{
		scrn = &this->scrnclip;
		base.top = m_nMenuHeight;
	}
	ClearOutOfRect(scrn, &base);
#if defined(SUPPORT_DCLOCK)
	DispClock::GetInstance()->Redraw();
#endif	// defined(SUPPORT_DCLOCK)
}

/**
 * パレット初期化イベント
 * @param[in,out] pPalette パレット
 * @param[in] nPalettes パレット数
 */
void CScreenManager::OnInitializePalette(LPPALETTEENTRY pPalette, UINT nPalettes)
{
#if defined(SUPPORT_DCLOCK)
	const RGB32* pal32 = DispClock::GetInstance()->GetPalettes();
	for (UINT i = 0; i < 4; i++)
	{
		m_pal[i + START_PALORG].peBlue = pal32[i].p.b;
		m_pal[i + START_PALORG].peRed = pal32[i].p.r;
		m_pal[i + START_PALORG].peGreen = pal32[i].p.g;
		m_pal[i + START_PALORG].peFlags = PC_RESERVED | PC_NOCOLLAPSE;
	}
#endif
	for (UINT i = 0; i < PALLETES_8BPP; i++)
	{
		m_pal[i + START_PAL].peFlags = PC_RESERVED | PC_NOCOLLAPSE;
	}
}

/**
 * パレット更新
 */
void CScreenManager::PaletteSet()
{
	if (m_pPalette)
	{
		for (UINT i = 0; i < PALLETES_8BPP; i++)
		{
			m_pal[i + START_PAL].peRed = np2_pal32[i].p.r;
			m_pal[i + START_PAL].peBlue = np2_pal32[i].p.b;
			m_pal[i + START_PAL].peGreen = np2_pal32[i].p.g;
		}
		m_pPalette->SetEntries(0, START_PAL, PALLETES_8BPP, &m_pal[START_PAL]);
	}
}

/**
 * 作成
 * @param[in] hWnd ウィンドウ ハンドル
 * @param[in] scrnmode モード
 * @retval true 成功
 * @retval false 失敗
 */
bool CScreenManager::Create(HWND hWnd, UINT8 scrnmode)
{
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	DWORD dwStyleEx = GetWindowLong(hWnd, GWL_EXSTYLE);
	if (scrnmode & SCRNMODE_FULLSCREEN)
	{
		scrnmode &= ~SCRNMODE_ROTATEMASK;
		dwStyle &= ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME);
		dwStyle |= WS_POPUP;
		dwStyleEx |= WS_EX_TOPMOST;
		m_bDisplayedMenu = false;
		m_nMenuHeight = GetSystemMetrics(SM_CYMENU);
		np2class_enablemenu(hWnd, FALSE);
	}
	else
	{
		dwStyle |= WS_SYSMENU;
		if (np2oscfg.thickframe)
		{
			dwStyle |= WS_THICKFRAME;
		}
		if (np2oscfg.wintype < 2)
		{
			dwStyle |= WS_CAPTION;
		}
		dwStyle &= ~WS_POPUP;
		dwStyleEx &= ~WS_EX_TOPMOST;
	}
	SetWindowLong(hWnd, GWL_STYLE, dwStyle);
	SetWindowLong(hWnd, GWL_EXSTYLE, dwStyleEx);

	if (scrnmode & SCRNMODE_FULLSCREEN)
	{
#if defined(SUPPORT_DCLOCK)
		DispClock::GetInstance()->Initialize();
#endif
		int nWidth = np2oscfg.fscrn_cx;
		int nHeight = np2oscfg.fscrn_cy;
		UINT bitcolor = np2oscfg.fscrnbpp;
		const UINT nFullscreenMode = np2oscfg.fscrnmod;
		if (nFullscreenMode & (FSCRNMOD_SAMERES | FSCRNMOD_SAMEBPP))
		{
			DEVMODE devmode;
			if (::EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &devmode))
			{
				if (nFullscreenMode & FSCRNMOD_SAMERES)
				{
					nWidth = devmode.dmPelsWidth;
					nHeight = devmode.dmPelsHeight;
				}
				if (nFullscreenMode & FSCRNMOD_SAMEBPP)
				{
					bitcolor = devmode.dmBitsPerPel;
				}
			}
		}
		if ((nWidth == 0) || (nHeight == 0))
		{
			nWidth = 640;
			nHeight = (np2oscfg.force400) ? 400 : 480;
		}
		if (bitcolor == 0)
		{
#if !defined(SUPPORT_PC9821)
			bitcolor = (scrnmode & SCRNMODE_HIGHCOLOR) ? 16 : 8;
#else
			bitcolor = 16;
#endif
		}

		if (!DDraw2::Create(hWnd, nWidth, nHeight, bitcolor))
		{
			goto scre_err;
		}

		m_pBackSurface = CreateBackSurface(640, 480);
		if (m_pBackSurface == NULL)
		{
			goto scre_err;
		}

#if defined(SUPPORT_DCLOCK)
		DispClock::GetInstance()->SetPalettes(bitcolor);
		m_pClockSurface = CreateBackSurface(DCLOCK_WIDTH, DCLOCK_HEIGHT);
		DispClock::GetInstance()->Reset();
#endif
	}
	else
	{
		if (!DDraw2::Create(hWnd, 640, 480))
		{
			goto scre_err;
		}

		if (!(scrnmode & SCRNMODE_ROTATE))
		{
			m_pBackSurface = CreateBackSurface(641, 480);
		}
		else
		{
			m_pBackSurface = CreateBackSurface(480, 641);
		}
		if (m_pBackSurface == NULL)
		{
			goto scre_err;
		}
		m_bHasExtendColumn = true;
	}
	m_scrnsurf.bpp = GetBpp();
	m_nScreenMode = scrnmode;
	m_nCliping = 0;
	RenewalClientSize(false);
	return true;

scre_err:
	Destroy();
	return false;
}

/**
 * 破棄
 */
void CScreenManager::Destroy()
{
	if (m_nScreenMode & SCRNMODE_FULLSCREEN)
	{
		np2class_enablemenu(m_hWnd, (!np2oscfg.wintype));
	}
#if defined(SUPPORT_DCLOCK)
	if (m_pClockSurface)
	{
		m_pClockSurface->Release();
		m_pClockSurface = NULL;
	}
#endif
	if (m_pBackSurface)
	{
		m_pBackSurface->Release();
		m_pBackSurface = NULL;
	}

	DDraw2::Destroy();

	m_nScreenMode = 0;
	m_nCliping = 0;
	m_bHasExtendColumn = false;
	m_bDisplayedMenu = false;
	m_bAllFlash = false;
	m_bChangedPalette = false;
	m_nMenuHeight = 0;
	ZeroMemory(&m_rcProjection, sizeof(m_rcProjection));
	ZeroMemory(&m_rcSurface, sizeof(m_rcSurface));
	ZeroMemory(&this->scrnclip, sizeof(this->scrnclip));
	ZeroMemory(&this->rectclip, sizeof(this->rectclip));
	ZeroMemory(&m_scrnsurf, sizeof(m_scrnsurf));
}

/**
 * マウスが移動して時に呼ばれる
 * @param[in] pt 位置
 */
void CScreenManager::OnMouseMove(const POINT& pt)
{
	if (IsFullscreen())
	{
		const bool bDisplayedMenu = (pt.y >= 0) && (pt.y < m_nMenuHeight);
		if (m_bDisplayedMenu != bDisplayedMenu)
		{
			m_bDisplayedMenu = bDisplayedMenu;
			if (bDisplayedMenu)
			{
				np2class_enablemenu(m_hWnd, TRUE);
			}
			else
			{
				np2class_enablemenu(m_hWnd, FALSE);
				ClearOutFullscreen();
			}
		}
	}
}

/**
 * UIを有効化
 */
void CScreenManager::EnableUI()
{
	mousemng_disable(MOUSEPROC_WINUI);
	if (!m_nCliping++)
	{
		if (m_nScreenMode & SCRNMODE_FULLSCREEN)
		{
			m_pPrimarySurface->SetClipper(m_pClipper);
		}
#ifndef __GNUC__
		WINNLSEnableIME(m_hWnd, TRUE);
#endif
	}
}

/**
 * UIを無効化
 */
void CScreenManager::DisableUI()
{
	if ((m_nCliping > 0) && (!(--m_nCliping)))
	{
#ifndef __GNUC__
		WINNLSEnableIME(m_hWnd, FALSE);
#endif
		if (m_nScreenMode & SCRNMODE_FULLSCREEN)
		{
			m_pPrimarySurface->SetClipper(NULL);
		}
	}
	if (m_nScreenMode & SCRNMODE_FULLSCREEN)
	{
		np2class_enablemenu(m_hWnd, FALSE);
		ClearOutFullscreen();
		m_bDisplayedMenu = false;
	}
	else
	{
		if (np2oscfg.wintype)
		{
			np2class_enablemenu(m_hWnd, FALSE);
			InvalidateRect(m_hWnd, NULL, TRUE);
		}
	}
	mousemng_enable(MOUSEPROC_WINUI);
}

/**
 * 幅を設定
 * @param[in] nWidth 幅
 */
void CScreenManager::SetWidth(int nWidth)
{
	m_nWidth = nWidth;
	RenewalClientSize(true);
}

/**
 * 拡張幅を設定
 * @param[in] nExtend 拡張
 */
void CScreenManager::SetExtend(int nExtend)
{
	m_nExtend = nExtend;
	AllFlash();
	RenewalClientSize(true);
}

/**
 * 高さを設定
 * @param[in] nHeight 高さ
 */
void CScreenManager::SetHeight(int nHeight)
{
	m_nHeight = nHeight;
	RenewalClientSize(true);
}

/**
 * 倍率設定
 * @param[in] nMultiple 倍率
 */
void CScreenManager::SetMultiple(int nMultiple)
{
	if (m_nMultiple != nMultiple)
	{
		m_nMultiple = nMultiple;
		RenewalClientSize(true);
	}
}

/**
 * バックサーフェス ロック
 * @return サーフェス
 */
const SCRNSURF* CScreenManager::Lock()
{
	DDSURFACEDESC destscrn;
	ZeroMemory(&destscrn, sizeof(destscrn));
	destscrn.dwSize = sizeof(destscrn);
	if (m_pBackSurface == NULL)
	{
		return NULL;
	}
	HRESULT hr = m_pBackSurface->Lock(NULL, &destscrn, DDLOCK_WAIT, NULL);
	if (hr == DDERR_SURFACELOST)
	{
		m_pBackSurface->Restore();
		hr = m_pBackSurface->Lock(NULL, &destscrn, DDLOCK_WAIT, NULL);
	}
	if (hr != DD_OK)
	{
//		TRACEOUT(("backsurf lock error: %d (%d)", r));
		return NULL;
	}
	if (!(m_nScreenMode & SCRNMODE_ROTATE))
	{
		m_scrnsurf.ptr = (UINT8 *)destscrn.lpSurface;
		m_scrnsurf.xalign = m_scrnsurf.bpp >> 3;
		m_scrnsurf.yalign = destscrn.lPitch;
	}
	else if (!(m_nScreenMode & SCRNMODE_ROTATEDIR))
	{
		m_scrnsurf.ptr = (UINT8 *)destscrn.lpSurface;
		m_scrnsurf.ptr += (m_scrnsurf.width + m_scrnsurf.extend - 1) * destscrn.lPitch;
		m_scrnsurf.xalign = 0 - destscrn.lPitch;
		m_scrnsurf.yalign = m_scrnsurf.bpp >> 3;
	}
	else
	{
		m_scrnsurf.ptr = (UINT8 *)destscrn.lpSurface;
		m_scrnsurf.ptr += (m_scrnsurf.height - 1) * (m_scrnsurf.bpp >> 3);
		m_scrnsurf.xalign = destscrn.lPitch;
		m_scrnsurf.yalign = 0 - (m_scrnsurf.bpp >> 3);
	}
	return &m_scrnsurf;
}

/**
 * バックサーフェス アンロック
 * @param[in] surf サーフェス
 */
void CScreenManager::Unlock(const SCRNSURF *surf)
{
	m_pBackSurface->Unlock(NULL);
	scrnmng_update();
	recvideo_update();
}

/**
 * 更新
 */
void CScreenManager::Update()
{
	if (m_bChangedPalette)
	{
		m_bChangedPalette = false;
		PaletteSet();
	}
	if (m_pBackSurface)
	{
		if (m_nScreenMode & SCRNMODE_FULLSCREEN)
		{
			if (m_bAllFlash)
			{
				m_bAllFlash = false;
				ClearOutFullscreen();
			}

			RECT* rect;
			RECT* scrn;
			if (GetWindowLongPtr(m_hWnd, NP2GWLP_HMENU))
			{
				rect = &m_rcSurface;
				scrn = &m_rcProjection;
			}
			else
			{
				rect = &this->rectclip;
				scrn = &this->scrnclip;
			}
			HRESULT hr = m_pPrimarySurface->Blt(scrn, m_pBackSurface, rect, DDBLT_WAIT, NULL);
			if (hr == DDERR_SURFACELOST)
			{
				m_pBackSurface->Restore();
				m_pPrimarySurface->Restore();
				m_pPrimarySurface->Blt(scrn, m_pBackSurface, rect, DDBLT_WAIT, NULL);
			}
		}
		else
		{
			if (m_bAllFlash)
			{
				m_bAllFlash = false;
				ClearOutScreen();
			}
			HRESULT hr = Blt(&m_rcProjection, m_pBackSurface, &m_rcSurface, DDBLT_WAIT, NULL);
			if (hr == DDERR_SURFACELOST)
			{
				m_pBackSurface->Restore();
				m_pPrimarySurface->Restore();
				Blt(&m_rcProjection, m_pBackSurface, &m_rcSurface, DDBLT_WAIT, NULL);
			}
		}
	}
}

#if defined(SUPPORT_DCLOCK)
static const RECT rectclk = {0, 0, DCLOCK_WIDTH, DCLOCK_HEIGHT};

/**
 * 時計の領域か?
 * @param[in] pt 位置
 * @retval true 領域内
 * @retval false 領域外
 */
bool CScreenManager::IsDispClockClick(const POINT *pt) const
{
	return (pt->y >= (m_szScreen.cy - DCLOCK_HEIGHT));
}

/**
 * 時計を表示
 */
void CScreenManager::DispClock()
{
	if (m_pClockSurface == NULL)
	{
		return;
	}
	if (!DispClock::GetInstance()->IsDisplayed())
	{
		return;
	}

	const RECT* scrn;
	if (GetWindowLongPtr(m_hWnd, NP2GWLP_HMENU))
	{
		scrn = &m_rcProjection;
	}
	else
	{
		scrn = &this->scrnclip;
	}
	if ((scrn->bottom + DCLOCK_HEIGHT) > m_szScreen.cy)
	{
		return;
	}
	DispClock::GetInstance()->Make();

	DDSURFACEDESC dest;
	ZeroMemory(&dest, sizeof(dest));
	dest.dwSize = sizeof(dest);
	if (m_pClockSurface->Lock(NULL, &dest, DDLOCK_WAIT, NULL) == DD_OK)
	{
		DispClock::GetInstance()->Draw(GetBpp(), dest.lpSurface, dest.lPitch);
		m_pClockSurface->Unlock(NULL);
	}
	if (m_pPrimarySurface->BltFast(m_szScreen.cx - DCLOCK_WIDTH - 4,
									m_szScreen.cy - DCLOCK_HEIGHT,
									m_pClockSurface, (RECT *)&rectclk,
									DDBLTFAST_WAIT) == DDERR_SURFACELOST)
	{
		m_pPrimarySurface->Restore();
		m_pClockSurface->Restore();
	}
	DispClock::GetInstance()->CountDown(np2oscfg.DRAW_SKIP);
}
#endif	// defined(SUPPORT_DCLOCK)

// ----

RGB16 scrnmng_makepal16(RGB32 pal32)
{
	return CScreenManager::GetInstance()->GetPalette16(pal32);
}

void scrnmng_setwidth(int posx, int width)
{
	CScreenManager::GetInstance()->SetWidth(width);
}

void scrnmng_setextend(int extend)
{
	CScreenManager::GetInstance()->SetExtend(extend);
}

void scrnmng_setheight(int posy, int height)
{
	CScreenManager::GetInstance()->SetHeight(height);
}

BOOL scrnmng_isfullscreen(void)
{
	return CScreenManager::GetInstance()->IsFullscreen();
}

BOOL scrnmng_haveextend(void)
{
	return CScreenManager::GetInstance()->HasExtendColumn();
}

UINT scrnmng_getbpp(void)
{
	return CScreenManager::GetInstance()->GetBpp();
}

const SCRNSURF* scrnmng_surflock(void)
{
	return CScreenManager::GetInstance()->Lock();
}

void scrnmng_surfunlock(const SCRNSURF *surf)
{
	CScreenManager::GetInstance()->Unlock(surf);
}

void scrnmng_update(void)
{
	CScreenManager::GetInstance()->Update();
}

void scrnmng_allflash(void)
{
	CScreenManager::GetInstance()->AllFlash();
}

void scrnmng_palchanged(void)
{
	CScreenManager::GetInstance()->ChangePalette();
}

// ----

typedef struct
{
	int		bx;
	int		by;
	int		cx;
	int		cy;
	int		mul;
} SCRNSIZING;

static	SCRNSIZING	scrnsizing;

enum
{
	SIZING_ADJUST	= 12
};

void scrnmng_entersizing(void)
{
	CScreenManager::GetInstance()->EnterSizing();
}

void CScreenManager::EnterSizing()
{
	RECT rectwindow;
	GetWindowRect(g_hWndMain, &rectwindow);

	RECT rectclient;
	GetClientRect(g_hWndMain, &rectclient);

	scrnsizing.bx = (np2oscfg.paddingx * 2) +
					(rectwindow.right - rectwindow.left) -
					(rectclient.right - rectclient.left);
	scrnsizing.by = (np2oscfg.paddingy * 2) +
					(rectwindow.bottom - rectwindow.top) -
					(rectclient.bottom - rectclient.top);
	int cx = min(m_nWidth, m_szScreen.cx);
	cx = (cx + 7) >> 3;
	int cy = min(m_nHeight, m_szScreen.cy);
	cy = (cy + 7) >> 3;
	if (m_nScreenMode & SCRNMODE_ROTATE)
	{
		std::swap(cx, cy);
	}
	scrnsizing.cx = cx;
	scrnsizing.cy = cy;
	scrnsizing.mul = m_nMultiple;
}

void scrnmng_sizing(UINT side, RECT* rect)
{
	int nWidth = 16;
	if ((side != WMSZ_TOP) && (side != WMSZ_BOTTOM))
	{
		nWidth = rect->right - rect->left - scrnsizing.bx + SIZING_ADJUST;
		nWidth /= scrnsizing.cx;
	}

	int nHeight = 16;
	if ((side != WMSZ_LEFT) && (side != WMSZ_RIGHT))
	{
		nHeight = rect->bottom - rect->top - scrnsizing.by + SIZING_ADJUST;
		nHeight /= scrnsizing.cy;
	}
	int mul = min(nWidth, nHeight);
	if (mul <= 0)
	{
		mul = 1;
	}
	else if (mul > 16)
	{
		mul = 16;
	}
	nWidth = scrnsizing.bx + (scrnsizing.cx * mul);
	nHeight = scrnsizing.by + (scrnsizing.cy * mul);
	switch (side)
	{
		case WMSZ_LEFT:
		case WMSZ_TOPLEFT:
		case WMSZ_BOTTOMLEFT:
			rect->left = rect->right - nWidth;
			break;

		case WMSZ_RIGHT:
		case WMSZ_TOP:
		case WMSZ_TOPRIGHT:
		case WMSZ_BOTTOM:
		case WMSZ_BOTTOMRIGHT:
		default:
			rect->right = rect->left + nWidth;
			break;
	}

	switch (side)
	{
		case WMSZ_TOP:
		case WMSZ_TOPLEFT:
		case WMSZ_TOPRIGHT:
			rect->top = rect->bottom - nHeight;
			break;

		case WMSZ_LEFT:
		case WMSZ_RIGHT:
		case WMSZ_BOTTOM:
		case WMSZ_BOTTOMLEFT:
		case WMSZ_BOTTOMRIGHT:
		default:
			rect->bottom = rect->top + nHeight;
			break;
	}
	scrnsizing.mul = mul;
}

void scrnmng_exitsizing(void)
{
	CScreenManager::GetInstance()->SetMultiple(scrnsizing.mul);
	InvalidateRect(g_hWndMain, NULL, TRUE);		// ugh
}

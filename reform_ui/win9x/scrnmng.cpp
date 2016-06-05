/**
 * @file	scrnmng.cpp
 * @brief	Screen Manager (DirectDraw2)
 *
 * @author	$Author: yui $
 * @date	$Date: 2011/03/07 09:54:11 $
 */

#include "compiler.h"
#ifndef __GNUC__
#include <winnls32.h>
#endif
#include "resource.h"
#include "np2.h"
#include "winloc.h"
#include "mousemng.h"
#include "scrnmng.h"
#include "dialog\np2class.h"
#include "pccore.h"
#include "scrndraw.h"
#include "palettes.h"
#include "subwnd\dd2.h"
#if defined(SUPPORT_DCLOCK)
#include "subwnd\dclock.h"
#endif
#include "recvideo.h"

//! 8BPP パレット数
#define PALLETES_8BPP	NP2PAL_TEXT3

extern WINLOCEX np2_winlocexallwin(HWND base);

/**
 *
 */
class CScreenManager : public DDraw2
{
public:
	CScreenManager();
	BRESULT Create(HWND hWnd, UINT8 scrnmode);
	void Destroy();
	void FullscreenMenu(int y);
	void TopWinUI();
	void ClearWinUI();
	const SCRNSURF* Lock();
	void Unlock(const SCRNSURF *surf);
	void Update();

#if defined(SUPPORT_DCLOCK)
	BOOL IsDispClockClick(const POINT *pt);
	void DispClock();
#endif	// defined(SUPPORT_DCLOCK)

	void EnterSizing();

public:
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

#if defined(SUPPORT_DCLOCK)
	LPDIRECTDRAWSURFACE	m_pClockSurface;
#endif

	void RenewalClientSize(BOOL winloc);
	void ClearOutOfRect(const RECT *target, const RECT *base);
	void ClearOutScreen();
	void ClearOutFullscreen();
	virtual void OnInitializePalette(LPPALETTEENTRY pPalette, UINT nPalettes);
	void PaletteSet();
};

typedef struct
{
	int		width;
	int		height;
	int		extend;
	int		multiple;
} SCRNSTAT;

static CScreenManager s_ddraw;
		SCRNMNG		g_scrnmng;
static	SCRNSTAT	s_scrnstat;

static void setwindowsize(HWND hWnd, int width, int height)
{
	RECT workrc;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &workrc, 0);
	const int scx = GetSystemMetrics(SM_CXSCREEN);
	const int scy = GetSystemMetrics(SM_CYSCREEN);

	UINT cnt = 2;
	do
	{
		RECT rectwindow;
		GetWindowRect(hWnd, &rectwindow);
		RECT rectclient;
		GetClientRect(hWnd, &rectclient);
		int winx = (np2oscfg.winx != CW_USEDEFAULT) ? np2oscfg.winx : rectwindow.left;
		int winy = (np2oscfg.winy != CW_USEDEFAULT) ? np2oscfg.winy : rectwindow.top;
		int cx = width;
		cx += np2oscfg.paddingx * 2;
		cx += rectwindow.right - rectwindow.left;
		cx -= rectclient.right - rectclient.left;
		int cy = height;
		cy += np2oscfg.paddingy * 2;
		cy += rectwindow.bottom - rectwindow.top;
		cy -= rectclient.bottom - rectclient.top;

		if (scx < cx)
		{
			winx = (scx - cx) / 2;
		}
		else
		{
			if ((winx + cx) > workrc.right)
			{
				winx = workrc.right - cx;
			}
			if (winx < workrc.left)
			{
				winx = workrc.left;
			}
		}
		if (scy < cy)
		{
			winy = (scy - cy) / 2;
		}
		else
		{
			if ((winy + cy) > workrc.bottom)
			{
				winy = workrc.bottom - cy;
			}
			if (winy < workrc.top)
			{
				winy = workrc.top;
			}
		}
		MoveWindow(hWnd, winx, winy, cx, cy, TRUE);
	} while (--cnt);
}

/**
 * コンストラクタ
 */
CScreenManager::CScreenManager()
	: m_pBackSurface(NULL)
{
	m_nScreenMode = 0;
	m_nCliping = 0;
	m_bHasExtendColumn = false;
	m_bDisplayedMenu = false;
	m_nMenuHeight = 0;
	ZeroMemory(&m_rcProjection, sizeof(m_rcProjection));
	ZeroMemory(&m_rcSurface, sizeof(m_rcSurface));
	ZeroMemory(&this->scrnclip, sizeof(this->scrnclip));
	ZeroMemory(&this->rectclip, sizeof(this->rectclip));
	ZeroMemory(&m_scrnsurf, sizeof(m_scrnsurf));
}

void CScreenManager::RenewalClientSize(BOOL winloc)
{
	int			width;
	int			height;
	int			extend;
	int			multiple;
	int			scrnwidth;
	int			scrnheight;

	width = min(s_scrnstat.width, m_szScreen.cx);
	height = min(s_scrnstat.height, m_szScreen.cy);
	extend = 0;

	// 描画範囲～
	if (m_nScreenMode & SCRNMODE_FULLSCREEN)
	{
		m_rcSurface.right = width;
		m_rcSurface.bottom = height;
		scrnwidth = width;
		scrnheight = height;
		UINT fscrnmod = np2oscfg.fscrnmod & FSCRNMOD_ASPECTMASK;
		switch (fscrnmod)
		{
			default:
			case FSCRNMOD_NORESIZE:
				break;

			case FSCRNMOD_ASPECTFIX8:
				scrnwidth = (m_szScreen.cx << 3) / width;
				scrnheight = (m_szScreen.cy << 3) / height;
				multiple = min(scrnwidth, scrnheight);
				scrnwidth = (width * multiple) >> 3;
				scrnheight = (height * multiple) >> 3;
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
		multiple = s_scrnstat.multiple;
		if (!(m_nScreenMode & SCRNMODE_ROTATE))
		{
			if ((np2oscfg.paddingx) && (multiple == 8) && (m_bHasExtendColumn))
			{
				extend = s_scrnstat.extend;
			}
			scrnwidth = (width * multiple) >> 3;
			scrnheight = (height * multiple) >> 3;
			m_rcSurface.right = width + extend;
			m_rcSurface.bottom = height;
			m_rcProjection.left = np2oscfg.paddingx - extend;
			m_rcProjection.top = np2oscfg.paddingy;
		}
		else
		{
			if ((np2oscfg.paddingy) && (multiple == 8) && (m_bHasExtendColumn))
			{
				extend = s_scrnstat.extend;
			}
			scrnwidth = (height * multiple) >> 3;
			scrnheight = (width * multiple) >> 3;
			m_rcSurface.right = height;
			m_rcSurface.bottom = width + extend;
			m_rcProjection.left = np2oscfg.paddingx;
			m_rcProjection.top = np2oscfg.paddingy - extend;
		}
		m_rcProjection.right = np2oscfg.paddingx + scrnwidth;
		m_rcProjection.bottom = np2oscfg.paddingy + scrnheight;

		WINLOCEX wlex = NULL;
		if (winloc)
		{
			wlex = np2_winlocexallwin(m_hWnd);
		}
		winlocex_setholdwnd(wlex, m_hWnd);
		setwindowsize(m_hWnd, scrnwidth, scrnheight);
		winlocex_move(wlex);
		winlocex_destroy(wlex);
	}
	m_scrnsurf.width = width;
	m_scrnsurf.height = height;
	m_scrnsurf.extend = extend;
}

/**
 * クリア
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
#endif
}

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
 */
BRESULT CScreenManager::Create(HWND hWnd, UINT8 scrnmode)
{
	ZeroMemory(&g_scrnmng, sizeof(g_scrnmng));
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	DWORD dwStyleEx = GetWindowLong(hWnd, GWL_EXSTYLE);
	if (scrnmode & SCRNMODE_FULLSCREEN)
	{
		scrnmode &= ~SCRNMODE_ROTATEMASK;
		g_scrnmng.flag = SCRNFLAG_FULLSCREEN;
		dwStyle &= ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME);
		dwStyle |= WS_POPUP;
		dwStyleEx |= WS_EX_TOPMOST;
		m_bDisplayedMenu = false;
		m_nMenuHeight = GetSystemMetrics(SM_CYMENU);
		np2class_enablemenu(hWnd, FALSE);
	}
	else
	{
		g_scrnmng.flag = SCRNFLAG_HAVEEXTEND;
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
	g_scrnmng.bpp = GetBpp();
	m_scrnsurf.bpp = GetBpp();
	m_nScreenMode = scrnmode;
	m_nCliping = 0;
	RenewalClientSize(FALSE);
	return SUCCESS;

scre_err:
	scrnmng_destroy();
	return FAILURE;
}

/**
 * 破棄
 */
void CScreenManager::Destroy()
{
	if (g_scrnmng.flag & SCRNFLAG_FULLSCREEN)
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
	m_nMenuHeight = 0;
	ZeroMemory(&m_rcProjection, sizeof(m_rcProjection));
	ZeroMemory(&m_rcSurface, sizeof(m_rcSurface));
	ZeroMemory(&this->scrnclip, sizeof(this->scrnclip));
	ZeroMemory(&this->rectclip, sizeof(this->rectclip));
	ZeroMemory(&m_scrnsurf, sizeof(m_scrnsurf));
}

void CScreenManager::FullscreenMenu(int y)
{
	if (g_scrnmng.flag & SCRNFLAG_FULLSCREEN)
	{
		const bool bDisplayedMenu = (y >= 0) && (y < m_nMenuHeight);
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

void CScreenManager::TopWinUI()
{
	mousemng_disable(MOUSEPROC_WINUI);
	if (!m_nCliping++)
	{
		if (g_scrnmng.flag & SCRNFLAG_FULLSCREEN)
		{
			m_pPrimarySurface->SetClipper(m_pClipper);
		}
#ifndef __GNUC__
		WINNLSEnableIME(m_hWnd, TRUE);
#endif
	}
}

void CScreenManager::ClearWinUI()
{
	if ((m_nCliping > 0) && (!(--m_nCliping)))
	{
#ifndef __GNUC__
		WINNLSEnableIME(m_hWnd, FALSE);
#endif
		if (g_scrnmng.flag & SCRNFLAG_FULLSCREEN)
		{
			m_pPrimarySurface->SetClipper(NULL);
		}
	}
	if (g_scrnmng.flag & SCRNFLAG_FULLSCREEN)
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

void CScreenManager::Unlock(const SCRNSURF *surf)
{
	m_pBackSurface->Unlock(NULL);
	scrnmng_update();
	recvideo_update();
}

void CScreenManager::Update()
{
	if (g_scrnmng.palchanged)
	{
		g_scrnmng.palchanged = FALSE;
		PaletteSet();
	}
	if (m_pBackSurface)
	{
		if (m_nScreenMode & SCRNMODE_FULLSCREEN)
		{
			if (g_scrnmng.allflash)
			{
				g_scrnmng.allflash = 0;
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
			if (g_scrnmng.allflash)
			{
				g_scrnmng.allflash = 0;
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

BOOL CScreenManager::IsDispClockClick(const POINT *pt)
{
	if (pt->y >= (m_szScreen.cy - DCLOCK_HEIGHT))
	{
		return(TRUE);
	}
	else
	{
		return(FALSE);
	}
}

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
		DispClock::GetInstance()->Draw(g_scrnmng.bpp, dest.lpSurface, dest.lPitch);
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

void scrnmng_initialize(void)
{
	s_scrnstat.width = 640;
	s_scrnstat.height = 400;
	s_scrnstat.extend = 1;
	s_scrnstat.multiple = 8;
	setwindowsize(g_hWndMain, 640, 400);
}

BRESULT scrnmng_create(UINT8 scrnmode)
{
	return s_ddraw.Create(g_hWndMain, scrnmode);
}

void scrnmng_destroy(void)
{
	s_ddraw.Destroy();
}

void scrnmng_querypalette(void)
{
	s_ddraw.QueryPalette();
}

RGB16 scrnmng_makepal16(RGB32 pal32)
{
	return s_ddraw.GetPalette16(pal32);
}

void scrnmng_fullscrnmenu(int y)
{
	s_ddraw.FullscreenMenu(y);
}

void scrnmng_topwinui(void)
{
	s_ddraw.TopWinUI();
}

void scrnmng_clearwinui(void)
{
	s_ddraw.ClearWinUI();
}

void scrnmng_setwidth(int posx, int width)
{
	s_scrnstat.width = width;
	s_ddraw.RenewalClientSize(TRUE);
}

void scrnmng_setextend(int extend)
{
	s_scrnstat.extend = extend;
	g_scrnmng.allflash = TRUE;
	s_ddraw.RenewalClientSize(TRUE);
}

void scrnmng_setheight(int posy, int height)
{
	s_scrnstat.height = height;
	s_ddraw.RenewalClientSize(TRUE);
}

const SCRNSURF* scrnmng_surflock(void)
{
	return s_ddraw.Lock();
}

void scrnmng_surfunlock(const SCRNSURF *surf)
{
	s_ddraw.Unlock(surf);
}

void scrnmng_update(void)
{
	s_ddraw.Update();
}

void scrnmng_setmultiple(int multiple)
{
	if (s_scrnstat.multiple != multiple)
	{
		s_scrnstat.multiple = multiple;
		s_ddraw.RenewalClientSize(TRUE);
	}
}

int scrnmng_getmultiple(void)
{
	return s_scrnstat.multiple;
}

#if defined(SUPPORT_DCLOCK)
BOOL scrnmng_isdispclockclick(const POINT *pt)
{
	return s_ddraw.IsDispClockClick(pt);
}

void scrnmng_dispclock(void)
{
	s_ddraw.DispClock();
}
#endif	// defined(SUPPORT_DCLOCK)

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
	s_ddraw.EnterSizing();
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
	int cx = min(s_scrnstat.width, m_szScreen.cx);
	cx = (cx + 7) >> 3;
	int cy = min(s_scrnstat.height, m_szScreen.cy);
	cy = (cy + 7) >> 3;
	if (!(m_nScreenMode & SCRNMODE_ROTATE))
	{
		scrnsizing.cx = cx;
		scrnsizing.cy = cy;
	}
	else
	{
		scrnsizing.cx = cy;
		scrnsizing.cy = cx;
	}
	scrnsizing.mul = s_scrnstat.multiple;
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
	scrnmng_setmultiple(scrnsizing.mul);
	InvalidateRect(g_hWndMain, NULL, TRUE);		// ugh
}

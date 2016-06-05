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

public:
	LPDIRECTDRAWSURFACE		m_pBackSurface;		/*!< バック サーフェス */

	UINT				scrnmode;
	int					_width;
	int					_height;
	int					_extend;
	int					cliping;
	UINT8				menudisp;
	int					menusize;
	RECT				scrn;
	RECT				rect;
	RECT				scrnclip;
	RECT				rectclip;
	SCRNSURF			m_scrnsurf;

#if defined(SUPPORT_DCLOCK)
	LPDIRECTDRAWSURFACE	clocksurf;
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
	this->scrnmode = 0;
	this->_width = 0;
	this->_height = 0;
	this->_extend = 0;
	this->cliping = 0;
	this->menudisp = 0;
	this->menusize = 0;
	ZeroMemory(&this->scrn, sizeof(this->scrn));
	ZeroMemory(&this->rect, sizeof(this->rect));
	ZeroMemory(&this->scrnclip, sizeof(this->scrnclip));
	ZeroMemory(&this->rectclip, sizeof(this->rectclip));
	ZeroMemory(&m_scrnsurf, sizeof(m_scrnsurf));
}

void CScreenManager::RenewalClientSize(BOOL winloc)
{
	CScreenManager& ddraw = *this;

	int			width;
	int			height;
	int			extend;
	int			multiple;
	int			scrnwidth;
	int			scrnheight;

	width = min(s_scrnstat.width, this->_width);
	height = min(s_scrnstat.height, this->_height);
	extend = 0;

	// 描画範囲～
	if (ddraw.scrnmode & SCRNMODE_FULLSCREEN)
	{
		ddraw.rect.right = width;
		ddraw.rect.bottom = height;
		scrnwidth = width;
		scrnheight = height;
		UINT fscrnmod = np2oscfg.fscrnmod & FSCRNMOD_ASPECTMASK;
		switch (fscrnmod)
		{
			default:
			case FSCRNMOD_NORESIZE:
				break;

			case FSCRNMOD_ASPECTFIX8:
				scrnwidth = (this->_width << 3) / width;
				scrnheight = (this->_height << 3) / height;
				multiple = min(scrnwidth, scrnheight);
				scrnwidth = (width * multiple) >> 3;
				scrnheight = (height * multiple) >> 3;
				break;

			case FSCRNMOD_ASPECTFIX:
				scrnwidth = this->_width;
				scrnheight = (scrnwidth * height) / width;
				if (scrnheight >= this->_height)
				{
					scrnheight = this->_height;
					scrnwidth = (scrnheight * width) / height;
				}
				break;

			case FSCRNMOD_LARGE:
				scrnwidth = this->_width;
				scrnheight = this->_height;
				break;
		}
		ddraw.scrn.left = (this->_width - scrnwidth) / 2;
		ddraw.scrn.top = (this->_height - scrnheight) / 2;
		ddraw.scrn.right = ddraw.scrn.left + scrnwidth;
		ddraw.scrn.bottom = ddraw.scrn.top + scrnheight;

		// メニュー表示時の描画領域
		ddraw.rectclip = ddraw.rect;
		ddraw.scrnclip = ddraw.scrn;
		if (ddraw.scrnclip.top < ddraw.menusize)
		{
			ddraw.scrnclip.top = ddraw.menusize;
			int tmpcy = this->_height - ddraw.menusize;
			if (scrnheight > tmpcy)
			{
				switch (fscrnmod)
				{
					default:
					case FSCRNMOD_NORESIZE:
						tmpcy = min(tmpcy, height);
						ddraw.rectclip.bottom = tmpcy;
						break;

					case FSCRNMOD_ASPECTFIX8:
					case FSCRNMOD_ASPECTFIX:
						ddraw.rectclip.bottom = (tmpcy * height) / scrnheight;
						break;

					case FSCRNMOD_LARGE:
						break;
				}
			}
			ddraw.scrnclip.bottom = ddraw.menusize + tmpcy;
		}
	}
	else
	{
		multiple = s_scrnstat.multiple;
		if (!(ddraw.scrnmode & SCRNMODE_ROTATE))
		{
			if ((np2oscfg.paddingx) && (multiple == 8))
			{
				extend = min(s_scrnstat.extend, this->_extend);
			}
			scrnwidth = (width * multiple) >> 3;
			scrnheight = (height * multiple) >> 3;
			ddraw.rect.right = width + extend;
			ddraw.rect.bottom = height;
			ddraw.scrn.left = np2oscfg.paddingx - extend;
			ddraw.scrn.top = np2oscfg.paddingy;
		}
		else
		{
			if ((np2oscfg.paddingy) && (multiple == 8))
			{
				extend = min(s_scrnstat.extend, this->_extend);
			}
			scrnwidth = (height * multiple) >> 3;
			scrnheight = (width * multiple) >> 3;
			ddraw.rect.right = height;
			ddraw.rect.bottom = width + extend;
			ddraw.scrn.left = np2oscfg.paddingx;
			ddraw.scrn.top = np2oscfg.paddingy - extend;
		}
		ddraw.scrn.right = np2oscfg.paddingx + scrnwidth;
		ddraw.scrn.bottom = np2oscfg.paddingy + scrnheight;

		WINLOCEX wlex = NULL;
		if (winloc)
		{
			wlex = np2_winlocexallwin(g_hWndMain);
		}
		winlocex_setholdwnd(wlex, g_hWndMain);
		setwindowsize(g_hWndMain, scrnwidth, scrnheight);
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
	if (m_pPrimarySurface == NULL)
	{
		return;
	}

	DDBLTFX ddbf;
	ZeroMemory(&ddbf, sizeof(ddbf));
	ddbf.dwSize = sizeof(ddbf);
	ddbf.dwFillColor = 0;

	RECT rect;
	rect.left = base->left;
	rect.right = base->right;
	rect.top = base->top;
	rect.bottom = target->top;
	if (rect.top < rect.bottom)
	{
		m_pPrimarySurface->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
	}
	rect.top = target->bottom;
	rect.bottom = base->bottom;
	if (rect.top < rect.bottom)
	{
		m_pPrimarySurface->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
	}

	rect.top = max(base->top, target->top);
	rect.bottom = min(base->bottom, target->bottom);
	if (rect.top < rect.bottom)
	{
		rect.left = base->left;
		rect.right = target->left;
		if (rect.left < rect.right)
		{
			m_pPrimarySurface->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
		}
		rect.left = target->right;
		rect.right = base->right;
		if (rect.left < rect.right)
		{
			m_pPrimarySurface->Blt(&rect, NULL, NULL, DDBLT_COLORFILL, &ddbf);
		}
	}
}

void CScreenManager::ClearOutScreen()
{
	CScreenManager& ddraw = *this;

	RECT base;
	GetClientRect(g_hWndMain, &base);

	POINT clipt;
	clipt.x = 0;
	clipt.y = 0;
	ClientToScreen(g_hWndMain, &clipt);
	base.left += clipt.x;
	base.top += clipt.y;
	base.right += clipt.x;
	base.bottom += clipt.y;

	RECT target;
	target.left = base.left + ddraw.scrn.left;
	target.top = base.top + ddraw.scrn.top;
	target.right = base.left + ddraw.scrn.right;
	target.bottom = base.top + ddraw.scrn.bottom;
	ClearOutOfRect(&target, &base);
}

void CScreenManager::ClearOutFullscreen()
{
	CScreenManager& ddraw = *this;

	RECT base;
	base.left = 0;
	base.top = 0;
	base.right = this->_width;
	base.bottom = this->_height;

	const RECT* scrn;
	if (GetWindowLongPtr(g_hWndMain, NP2GWLP_HMENU))
	{
		scrn = &ddraw.scrn;
		base.top = 0;
	}
	else
	{
		scrn = &ddraw.scrnclip;
		base.top = ddraw.menusize;
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
	CScreenManager& ddraw = *this;

	int				width;
	int				height;
	UINT			bitcolor;
	UINT			fscrnmod;

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
		ddraw.menudisp = 0;
		ddraw.menusize = GetSystemMetrics(SM_CYMENU);
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
		width = np2oscfg.fscrn_cx;
		height = np2oscfg.fscrn_cy;
		bitcolor = np2oscfg.fscrnbpp;
		fscrnmod = np2oscfg.fscrnmod;
		if (fscrnmod & (FSCRNMOD_SAMERES | FSCRNMOD_SAMEBPP))
		{
			DEVMODE devmode;
			if (::EnumDisplaySettings(NULL, ENUM_REGISTRY_SETTINGS, &devmode))
			{
				if (fscrnmod & FSCRNMOD_SAMERES)
				{
					width = devmode.dmPelsWidth;
					height = devmode.dmPelsHeight;
				}
				if (fscrnmod & FSCRNMOD_SAMEBPP)
				{
					bitcolor = devmode.dmBitsPerPel;
				}
			}
		}
		if ((width == 0) || (height == 0))
		{
			width = 640;
			height = (np2oscfg.force400)?400:480;
		}
		if (bitcolor == 0)
		{
#if !defined(SUPPORT_PC9821)
			bitcolor = (scrnmode & SCRNMODE_HIGHCOLOR)?16:8;
#else
			bitcolor = 16;
#endif
		}

		if (!DDraw2::Create(hWnd, width, height, bitcolor))
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
		this->clocksurf = CreateBackSurface(DCLOCK_WIDTH, DCLOCK_HEIGHT);
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

		width = 640;
		height = 480;
		bitcolor = GetBpp();
		this->_extend = 1;
	}
	g_scrnmng.bpp = (UINT8)bitcolor;
	m_scrnsurf.bpp = bitcolor;
	ddraw.scrnmode = scrnmode;
	this->_width = width;
	this->_height = height;
	ddraw.cliping = 0;
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
	CScreenManager& ddraw = *this;

	if (g_scrnmng.flag & SCRNFLAG_FULLSCREEN)
	{
		np2class_enablemenu(g_hWndMain, (!np2oscfg.wintype));
	}
#if defined(SUPPORT_DCLOCK)
	if (ddraw.clocksurf)
	{
		ddraw.clocksurf->Release();
		ddraw.clocksurf = NULL;
	}
#endif
	if (m_pBackSurface)
	{
		m_pBackSurface->Release();
		m_pBackSurface = NULL;
	}

	DDraw2::Destroy();

	this->scrnmode = 0;
	this->_width = 0;
	this->_height = 0;
	this->_extend = 0;
	this->cliping = 0;
	this->menudisp = 0;
	this->menusize = 0;
	ZeroMemory(&this->scrn, sizeof(this->scrn));
	ZeroMemory(&this->rect, sizeof(this->rect));
	ZeroMemory(&this->scrnclip, sizeof(this->scrnclip));
	ZeroMemory(&this->rectclip, sizeof(this->rectclip));
	ZeroMemory(&m_scrnsurf, sizeof(m_scrnsurf));
}

void CScreenManager::FullscreenMenu(int y)
{
	CScreenManager& ddraw = *this;

	if (g_scrnmng.flag & SCRNFLAG_FULLSCREEN)
	{
		UINT8 menudisp = ((y >= 0) && (y < ddraw.menusize)) ? 1 : 0;
		if (ddraw.menudisp != menudisp)
		{
			ddraw.menudisp = menudisp;
			if (menudisp == 1)
			{
				np2class_enablemenu(g_hWndMain, TRUE);
			}
			else
			{
				np2class_enablemenu(g_hWndMain, FALSE);
				ClearOutFullscreen();
			}
		}
	}
}

void CScreenManager::TopWinUI()
{
	mousemng_disable(MOUSEPROC_WINUI);
	if (!this->cliping++)
	{
		if (g_scrnmng.flag & SCRNFLAG_FULLSCREEN)
		{
			m_pPrimarySurface->SetClipper(m_pClipper);
		}
#ifndef __GNUC__
		WINNLSEnableIME(g_hWndMain, TRUE);
#endif
	}
}

void CScreenManager::ClearWinUI()
{
	CScreenManager& ddraw = *this;

	if ((ddraw.cliping > 0) && (!(--ddraw.cliping)))
	{
#ifndef __GNUC__
		WINNLSEnableIME(g_hWndMain, FALSE);
#endif
		if (g_scrnmng.flag & SCRNFLAG_FULLSCREEN)
		{
			m_pPrimarySurface->SetClipper(NULL);
		}
	}
	if (g_scrnmng.flag & SCRNFLAG_FULLSCREEN)
	{
		np2class_enablemenu(g_hWndMain, FALSE);
		ClearOutFullscreen();
		ddraw.menudisp = 0;
	}
	else
	{
		if (np2oscfg.wintype)
		{
			np2class_enablemenu(g_hWndMain, FALSE);
			InvalidateRect(g_hWndMain, NULL, TRUE);
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
	if (!(this->scrnmode & SCRNMODE_ROTATE))
	{
		m_scrnsurf.ptr = (UINT8 *)destscrn.lpSurface;
		m_scrnsurf.xalign = m_scrnsurf.bpp >> 3;
		m_scrnsurf.yalign = destscrn.lPitch;
	}
	else if (!(this->scrnmode & SCRNMODE_ROTATEDIR))
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
	CScreenManager& ddraw = *this;

	if (g_scrnmng.palchanged)
	{
		g_scrnmng.palchanged = FALSE;
		PaletteSet();
	}
	if (m_pBackSurface)
	{
		if (ddraw.scrnmode & SCRNMODE_FULLSCREEN)
		{
			if (g_scrnmng.allflash)
			{
				g_scrnmng.allflash = 0;
				ClearOutFullscreen();
			}

			RECT* rect;
			RECT* scrn;
			if (GetWindowLongPtr(g_hWndMain, NP2GWLP_HMENU))
			{
				rect = &ddraw.rect;
				scrn = &ddraw.scrn;
			}
			else
			{
				rect = &ddraw.rectclip;
				scrn = &ddraw.scrnclip;
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
			POINT clip;
			clip.x = 0;
			clip.y = 0;
			ClientToScreen(g_hWndMain, &clip);

			RECT dst;
			dst.left = clip.x + ddraw.scrn.left;
			dst.top = clip.y + ddraw.scrn.top;
			dst.right = clip.x + ddraw.scrn.right;
			dst.bottom = clip.y + ddraw.scrn.bottom;
			HRESULT hr = m_pPrimarySurface->Blt(&dst, m_pBackSurface, &ddraw.rect, DDBLT_WAIT, NULL);
			if (hr == DDERR_SURFACELOST)
			{
				m_pBackSurface->Restore();
				m_pPrimarySurface->Restore();
				m_pPrimarySurface->Blt(&dst, m_pBackSurface, &ddraw.rect, DDBLT_WAIT, NULL);
			}
		}
	}
}

#if defined(SUPPORT_DCLOCK)
static const RECT rectclk = {0, 0, DCLOCK_WIDTH, DCLOCK_HEIGHT};

BOOL CScreenManager::IsDispClockClick(const POINT *pt)
{
	if (pt->y >= (this->_height - DCLOCK_HEIGHT))
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
	CScreenManager& ddraw = *this;

	if (!ddraw.clocksurf)
	{
		return;
	}
	if (!DispClock::GetInstance()->IsDisplayed())
	{
		return;
	}

	const RECT* scrn;
	if (GetWindowLongPtr(g_hWndMain, NP2GWLP_HMENU))
	{
		scrn = &ddraw.scrn;
	}
	else
	{
		scrn = &ddraw.scrnclip;
	}
	if ((scrn->bottom + DCLOCK_HEIGHT) > this->_height)
	{
		return;
	}
	DispClock::GetInstance()->Make();

	DDSURFACEDESC dest;
	ZeroMemory(&dest, sizeof(dest));
	dest.dwSize = sizeof(dest);
	if (ddraw.clocksurf->Lock(NULL, &dest, DDLOCK_WAIT, NULL) == DD_OK)
	{
		DispClock::GetInstance()->Draw(g_scrnmng.bpp, dest.lpSurface, dest.lPitch);
		ddraw.clocksurf->Unlock(NULL);
	}
	if (m_pPrimarySurface->BltFast(this->_width - DCLOCK_WIDTH - 4,
									this->_height - DCLOCK_HEIGHT,
									ddraw.clocksurf, (RECT *)&rectclk,
									DDBLTFAST_WAIT) == DDERR_SURFACELOST)
	{
		m_pPrimarySurface->Restore();
		ddraw.clocksurf->Restore();
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
	CScreenManager& ddraw = s_ddraw;

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
	int cx = min(s_scrnstat.width, ddraw._width);
	cx = (cx + 7) >> 3;
	int cy = min(s_scrnstat.height, ddraw._height);
	cy = (cy + 7) >> 3;
	if (!(ddraw.scrnmode & SCRNMODE_ROTATE))
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

void scrnmng_sizing(UINT side, RECT *rect)
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

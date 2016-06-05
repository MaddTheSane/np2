/**
 * @file	dd2.cpp
 * @brief	DirectDraw2 描画クラスの動作の定義を行います
 */

#include "compiler.h"
#include "dd2.h"

#if !defined(__GNUC__)
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")
#endif	// !defined(__GNUC__)

/**
 * コンストラクタ
 */
DDraw2::DDraw2()
	: m_hWnd(NULL)
	, m_pDDraw(NULL)
	, m_pDDraw2(NULL)
	, m_pPrimarySurface(NULL)
	, m_pClipper(NULL)
	, m_pPalette(NULL)
	, m_nBpp(0)
	, m_bFullscreen(false)
	, m_r16b(0)
	, m_l16r(0)
	, m_l16g(0)
{
	m_pal16.d = 0;
	ZeroMemory(&m_pal, sizeof(m_pal));
}

/**
 * デストラクタ
 */
DDraw2::~DDraw2()
{
	Destroy();
}

/**
 * 作成
 * @param[in] hWnd ウィンドウ ハンドル
 * @param[in] nWidth 幅
 * @param[in] nHeight 高さ
 * @paran[in] nBpp BPP
 * @retval true 成功
 * @retval false 失敗
 */
bool DDraw2::Create(HWND hWnd, int nWidth, int nHeight, UINT nBpp)
{
	m_hWnd = hWnd;
	m_bFullscreen = (nBpp != 0);

	do
	{
		if (DirectDrawCreate(NULL, &m_pDDraw, NULL) != DD_OK)
		{
			break;
		}
		m_pDDraw->QueryInterface(IID_IDirectDraw2, reinterpret_cast<LPVOID*>(&m_pDDraw2));

		const DWORD dwFlags = (m_bFullscreen) ? DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN : DDSCL_NORMAL;
		m_pDDraw2->SetCooperativeLevel(hWnd, dwFlags);

		if (m_bFullscreen)
		{
			if (m_pDDraw2->SetDisplayMode(nWidth, nHeight, nBpp, 0, 0) != DD_OK)
			{
				break;
			}
		}

		DDSURFACEDESC ddsd;
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
		if (m_pDDraw2->CreateSurface(&ddsd, &m_pPrimarySurface, NULL) != DD_OK)
		{
			break;
		}

		m_pDDraw2->CreateClipper(0, &m_pClipper, NULL);
		m_pClipper->SetHWnd(0, hWnd);
		if (!m_bFullscreen)
		{
			m_pPrimarySurface->SetClipper(m_pClipper);
		}

		DDPIXELFORMAT ddpf;
		ZeroMemory(&ddpf, sizeof(ddpf));
		ddpf.dwSize = sizeof(ddpf);
		if (m_pPrimarySurface->GetPixelFormat(&ddpf) != DD_OK)
		{
			break;
		}

		if (ddpf.dwRGBBitCount == 8)
		{
			InitializePalette();
		}
		else if (ddpf.dwRGBBitCount == 16)
		{
			Make16Mask(ddpf.dwBBitMask, ddpf.dwRBitMask, ddpf.dwGBitMask);
		}
		else if (ddpf.dwRGBBitCount == 24)
		{
		}
		else if (ddpf.dwRGBBitCount == 32)
		{
		}
		else
		{
			break;
		}
		m_nBpp = ddpf.dwRGBBitCount;
		return true;
	} while (false /*CONSTCOND*/);

	Destroy();
	return false;
}

/**
 * 解放
 */
void DDraw2::Destroy()
{
	if (m_pPalette)
	{
		m_pPalette->Release();
		m_pPalette = NULL;
	}
	if (m_pClipper)
	{
		m_pClipper->Release();
		m_pClipper = NULL;
	}
	if (m_pPrimarySurface)
	{
		m_pPrimarySurface->Release();
		m_pPrimarySurface = NULL;
	}
	if (m_pDDraw2)
	{
		if (m_bFullscreen)
		{
			m_pDDraw2->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL);
		}
		m_pDDraw2->Release();
		m_pDDraw2 = NULL;
	}
	if (m_pDDraw)
	{
		m_pDDraw->Release();
		m_pDDraw = NULL;
	}

	m_hWnd = NULL;
	m_nBpp = 0;
	m_bFullscreen = false;
	m_pal16.d = 0;
	m_r16b = 0;
	m_l16r = 0;
	m_l16g = 0;
	ZeroMemory(&m_pal, sizeof(m_pal));
}

/**
 * バックサーフェス作成
 */
LPDIRECTDRAWSURFACE DDraw2::CreateBackSurface(int nWidth, int nHeight)
{
	if (m_pDDraw2 == NULL)
	{
		return NULL;
	}

	DDSURFACEDESC ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = nWidth;
	ddsd.dwHeight = nHeight;

	LPDIRECTDRAWSURFACE pBackSurface = NULL;
	if (m_pDDraw2->CreateSurface(&ddsd, &pBackSurface, NULL) != DD_OK)
	{
		return NULL;
	}
	return pBackSurface;
}

/**
 * blt
 * @param[in] pSurface サーフェス
 * @param[in] pt 位置
 * @param[in] lpRect 領域
 */
void DDraw2::Blt(LPDIRECTDRAWSURFACE pSurface, const POINT* pt, const RECT* lpRect)
{
	if (pSurface)
	{
		POINT clipt;
		clipt.x = 0;
		clipt.y = 0;
		if (pt)
		{
			clipt = *pt;
		}
		if (!m_bFullscreen)
		{
			::ClientToScreen(m_hWnd, &clipt);
		}
		RECT scrn;
		scrn.left = clipt.x;
		scrn.top = clipt.y;
		scrn.right = clipt.x + lpRect->right - lpRect->left;
		scrn.bottom = clipt.y + lpRect->bottom - lpRect->top;
		if (m_pPrimarySurface->Blt(&scrn, pSurface, const_cast<LPRECT>(lpRect), DDBLT_WAIT, NULL) == DDERR_SURFACELOST)
		{
			pSurface->Restore();
			m_pPrimarySurface->Restore();
		}
	}
}

/**
 * パレット更新
 */
void DDraw2::InitializePalette()
{
	HDC hdc = ::GetDC(m_hWnd);
	::GetSystemPaletteEntries(hdc, 0, 256, m_pal);
	::ReleaseDC(m_hWnd, hdc);
	OnInitializePalette(m_pal, 256);
	m_pDDraw2->CreatePalette(DDPCAPS_8BIT, m_pal, &m_pPalette, 0);
	m_pPrimarySurface->SetPalette(m_pPalette);
}

/**
 * パレット更新イベント
 */
void DDraw2::OnInitializePalette(LPPALETTEENTRY pPalette, UINT nPalettes)
{
}

/**
 * パレット更新
 */
void DDraw2::QueryPalette()
{
	if (m_pPalette)
	{
		m_pPrimarySurface->SetPalette(m_pPalette);
	}
}

/**
 * 16BPP用のマスク作成
 */
void DDraw2::Make16Mask(DWORD dwBBitMask, DWORD dwRBitMask, DWORD dwGBitMask)
{
	UINT8 sft = 0;
	while ((!(dwBBitMask & 0x80)) && (sft < 32))
	{
		dwBBitMask <<= 1;
		sft++;
	}
	m_pal16.p.b = static_cast<UINT8>(dwBBitMask);
	m_r16b = sft;

	sft = 0;
	while ((dwRBitMask & 0xffffff00) && (sft < 32))
	{
		dwRBitMask >>= 1;
		sft++;
	}
	m_pal16.p.r = static_cast<UINT8>(dwRBitMask);
	m_l16r = sft;

	sft = 0;
	while ((dwGBitMask & 0xffffff00) && (sft < 32))
	{
		dwGBitMask >>= 1;
		sft++;
	}
	m_pal16.p.g = static_cast<UINT8>(dwGBitMask);
	m_l16g = sft;
}

/**
 * 16BPP 色を得る
 * @param[in] pal 色
 * @return 16BPP色
 */
RGB16 DDraw2::GetPalette16(RGB32 pal) const
{
	pal.d &= m_pal16.d;
	return (static_cast<UINT>(pal.p.g) << m_l16g) | (static_cast<UINT>(pal.p.r) << m_l16r) | (pal.p.b >> m_r16b);
}



// ----

/**
 * コンストラクタ
 */
DDraw2Surface::DDraw2Surface()
	: m_pBackSurface(NULL)
{
	ZeroMemory(&m_vram, sizeof(m_vram));
}

/**
 * デストラクタ
 */
DDraw2Surface::~DDraw2Surface()
{
	Destroy();
}

/**
 * 作成
 * @param[in] nWidth 幅
 * @param[in] nHeight 高さ
 * @retval true 成功
 * @retval false 失敗
 */
bool DDraw2Surface::Create(DDraw2& dd2, int nWidth, int nHeight)
{
	Destroy();

	m_pBackSurface = dd2.CreateBackSurface(nWidth, nHeight);
	if (m_pBackSurface == NULL)
	{
		return false;
	}
	m_vram.width = nWidth;
	m_vram.height = nHeight;
	m_vram.xalign = dd2.GetBpp() / 8;
	m_vram.bpp = dd2.GetBpp();
	return true;
}

/**
 * 解放
 */
void DDraw2Surface::Destroy()
{
	if (m_pBackSurface)
	{
		m_pBackSurface->Release();
		m_pBackSurface = NULL;
	}
	ZeroMemory(&m_vram, sizeof(m_vram));
}

/**
 * バッファ ロック
 * @return バッファ
 */
CMNVRAM* DDraw2Surface::Lock()
{
	if (m_pBackSurface == NULL)
	{
		return NULL;
	}
	DDSURFACEDESC surface;
	ZeroMemory(&surface, sizeof(DDSURFACEDESC));
	surface.dwSize = sizeof(surface);
	HRESULT r = m_pBackSurface->Lock(NULL, &surface, DDLOCK_WAIT, NULL);
	if (r == DDERR_SURFACELOST)
	{
		m_pBackSurface->Restore();
		r = m_pBackSurface->Lock(NULL, &surface, DDLOCK_WAIT, NULL);
	}
	if (r != DD_OK)
	{
		return NULL;
	}
	m_vram.ptr = static_cast<UINT8*>(surface.lpSurface);
	m_vram.yalign = surface.lPitch;
	return &m_vram;
}

/**
 * バッファ アンロック
 */
void DDraw2Surface::Unlock()
{
	if (m_pBackSurface)
	{
		m_pBackSurface->Unlock(NULL);
	}
}

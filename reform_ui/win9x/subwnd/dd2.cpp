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
 * @retval true 成功
 * @retval false 失敗
 */
bool DDraw2::Create(HWND hWnd, int nWidth, int nHeight)
{
	m_hWnd = hWnd;

	do
	{
		if (DirectDrawCreate(NULL, &m_pDDraw, NULL) != DD_OK)
		{
			break;
		}
		m_pDDraw->QueryInterface(IID_IDirectDraw2, reinterpret_cast<LPVOID*>(&m_pDDraw2));
		m_pDDraw2->SetCooperativeLevel(hWnd, DDSCL_NORMAL);

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
		m_pPrimarySurface->SetClipper(m_pClipper);

		DDPIXELFORMAT ddpf;
		ZeroMemory(&ddpf, sizeof(ddpf));
		ddpf.dwSize = sizeof(ddpf);
		if (m_pPrimarySurface->GetPixelFormat(&ddpf) != DD_OK)
		{
			break;
		}

#if 0
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwWidth = nWidth;
		ddsd.dwHeight = nHeight;
		if (m_pDDraw2->CreateSurface(&ddsd, &m_pBackSurface, NULL) != DD_OK)
		{
			break;
		}
#endif
		if (ddpf.dwRGBBitCount == 8)
		{
			HDC hdc = ::GetDC(hWnd);
			::GetSystemPaletteEntries(hdc, 0, 256, m_pal);
			::ReleaseDC(hWnd, hdc);
			m_pDDraw2->CreatePalette(DDPCAPS_8BIT, m_pal, &m_pPalette, 0);
			m_pPrimarySurface->SetPalette(m_pPalette);
		}
		else if (ddpf.dwRGBBitCount == 16)
		{
			WORD bit;
			UINT8 cnt;

			m_pal16.d = 0;
			for (bit = 1; (bit) && (!(ddpf.dwBBitMask & bit)); bit <<= 1)
			{
			}
			for (m_r16b = 8; (m_r16b) && (ddpf.dwBBitMask & bit); m_r16b--, bit <<= 1)
			{
				m_pal16.p.b >>= 1;
				m_pal16.p.b |= 0x80;
			}
			for (m_l16r = 0, bit = 1; (bit) && (!(ddpf.dwRBitMask & bit)); m_l16r++, bit <<= 1)
			{
			}
			for (cnt = 0x80; (cnt) && (ddpf.dwRBitMask & bit); cnt >>= 1, bit <<= 1)
			{
				m_pal16.p.r |= cnt;
			}
			for (; cnt; cnt>>=1)
			{
				m_l16r--;
			}
			for (m_l16g = 0, bit = 1; (bit) && (!(ddpf.dwGBitMask & bit)); m_l16g++, bit <<= 1)
			{
			}
			for (cnt = 0x80; (cnt) && (ddpf.dwGBitMask & bit); cnt >>= 1, bit <<= 1)
			{
				m_pal16.p.g |= cnt;
			}
			for (; cnt; cnt >>= 1)
			{
				m_l16g--;
			}
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
		m_pDDraw2->Release();
		m_pDDraw2 = NULL;
	}
	if (m_pDDraw)
	{
		m_pDDraw->Release();
		m_pDDraw = NULL;
	}
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
		if (pt)
		{
			clipt = *pt;
		}
		else
		{
			clipt.x = 0;
			clipt.y = 0;
		}
		::ClientToScreen(m_hWnd, &clipt);
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
 * 16BPP 色を得る
 * @param[in] pal 色
 * @return 16BPP色
 */
UINT16 DDraw2::GetPalette16(RGB32 pal) const
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

	LPDIRECTDRAW2 pDDraw2 = dd2;
	if (pDDraw2 == NULL)
	{
		return false;
	}

	DDSURFACEDESC ddsd;
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = nWidth;
	ddsd.dwHeight = nHeight;
	if (pDDraw2->CreateSurface(&ddsd, &m_pBackSurface, NULL) != DD_OK)
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

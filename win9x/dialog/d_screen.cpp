/**
 * @file	d_screen.cpp
 * @brief	スクリーン設定ダイアログ
 */

#include "compiler.h"
#include "resource.h"
#include "dialog.h"
#include "c_combodata.h"
#include "c_slidervalue.h"
#include "np2class.h"
#include "np2.h"
#include "scrnmng.h"
#include "sysmng.h"
#include "misc\PropProc.h"
#include "pccore.h"
#include "iocore.h"
#include "common\strres.h"
#include "vram\scrndraw.h"
#include "vram\palettes.h"

/**
 * @brief Video ページ
 */
class ScrOptVideoPage : public CPropPageProc
{
public:
	ScrOptVideoPage();
	virtual ~ScrOptVideoPage();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	CSliderValue m_skiplight;		//!< スキップライン
};

/**
 * コンストラクタ
 */
ScrOptVideoPage::ScrOptVideoPage()
	: CPropPageProc(IDD_SCROPT1)
{
}

/**
 * デストラクタ
 */
ScrOptVideoPage::~ScrOptVideoPage()
{
}

/**
 * このメソッドは WM_INITDIALOG のメッセージに応答して呼び出されます
 * @retval TRUE 最初のコントロールに入力フォーカスを設定
 * @retval FALSE 既に設定済
 */
BOOL ScrOptVideoPage::OnInitDialog()
{
	CheckDlgButton(IDC_LCD, (np2cfg.LCD_MODE & 1) ? BST_CHECKED : BST_UNCHECKED);
	GetDlgItem(IDC_LCDX).EnableWindow((np2cfg.LCD_MODE & 1) ? TRUE : FALSE);
	CheckDlgButton(IDC_LCDX, (np2cfg.LCD_MODE & 2) ? BST_CHECKED : BST_UNCHECKED);

	CheckDlgButton(IDC_SKIPLINE, (np2cfg.skipline) ? BST_CHECKED : BST_UNCHECKED);

	m_skiplight.SubclassDlgItem(IDC_SKIPLIGHT, this);
	m_skiplight.SetStaticId(IDC_LIGHTSTR);
	m_skiplight.SetRange(0, 255);
	m_skiplight.SetPos(np2cfg.skiplight);
	return TRUE;
}

/**
 * ユーザーが OK のボタン (IDOK ID がのボタン) をクリックすると呼び出されます
 */
void ScrOptVideoPage::OnOK()
{
	bool bUpdated = false;

	const UINT8 cSkipLine = (IsDlgButtonChecked(IDC_SKIPLINE) != BST_UNCHECKED) ? 1 : 0;
	if (np2cfg.skipline != cSkipLine)
	{
		np2cfg.skipline = cSkipLine;
		bUpdated = true;
	}
	const UINT8 cSkipLight = static_cast<UINT8>(m_skiplight.GetPos());
	if (np2cfg.skiplight != cSkipLight)
	{
		np2cfg.skiplight = cSkipLight;
		bUpdated = true;
	}
	if (bUpdated)
	{
		::pal_makeskiptable();
	}

	UINT8 cMode = 0;
	if (IsDlgButtonChecked(IDC_LCD) != BST_UNCHECKED)
	{
		cMode |= 1;
	}
	if (IsDlgButtonChecked(IDC_LCDX) != BST_UNCHECKED)
	{
		cMode |= 2;
	}
	if (np2cfg.LCD_MODE != cMode)
	{
		np2cfg.LCD_MODE = cMode;
		pal_makelcdpal();
		bUpdated = true;
	}
	if (bUpdated)
	{
		::scrndraw_redraw();
		::sysmng_update(SYS_UPDATECFG);
	}
}

/**
 * ユーザーがメニューの項目を選択したときに、フレームワークによって呼び出されます
 * @param[in] wParam パラメタ
 * @param[in] lParam パラメタ
 * @retval TRUE アプリケーションがこのメッセージを処理した
 */
BOOL ScrOptVideoPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) == IDC_LCD)
	{
		const BOOL bEnable = (IsDlgButtonChecked(IDC_LCD) != BST_UNCHECKED) ? TRUE : FALSE;
		GetDlgItem(IDC_LCDX).EnableWindow(bEnable);
		return TRUE;
	}
	return FALSE;
}

/**
 * CWndProc オブジェクトの Windows プロシージャ (WindowProc) が用意されています
 * @param[in] nMsg 処理される Windows メッセージを指定します
 * @param[in] wParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @param[in] lParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @return メッセージに依存する値を返します
 */
LRESULT ScrOptVideoPage::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (nMsg == WM_HSCROLL)
	{
		switch (::GetDlgCtrlID(reinterpret_cast<HWND>(lParam)))
		{
			case IDC_SKIPLIGHT:
				m_skiplight.UpdateValue();
				break;

			default:
				break;
		}
	}
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}



// ----
/**
 * @brief Chip ページ
 */
class ScrOptChipPage : public CPropPageProc
{
public:
	ScrOptChipPage();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};

/**
 * コンストラクタ
 */
ScrOptChipPage::ScrOptChipPage()
	: CPropPageProc(IDD_SCROPT2)
{
}

//! GDC チップ
static const UINT s_gdcchip[4] = {IDC_GRCGNON, IDC_GRCG, IDC_GRCG2, IDC_EGC};

/**
 * このメソッドは WM_INITDIALOG のメッセージに応答して呼び出されます
 * @retval TRUE 最初のコントロールに入力フォーカスを設定
 * @retval FALSE 既に設定済
 */
BOOL ScrOptChipPage::OnInitDialog()
{
	CheckDlgButton((np2cfg.uPD72020) ? IDC_GDC72020 : IDC_GDC7220, BST_CHECKED);
	CheckDlgButton(s_gdcchip[np2cfg.grcg & 3], BST_CHECKED);
	CheckDlgButton(IDC_PC980124, (np2cfg.color16) ? BST_CHECKED : BST_UNCHECKED);

#if defined(SUPPORT_PC9821)
	static const UINT s_disabled[] =
	{
		IDC_GCBOX,
			IDC_GRCGNON, IDC_GRCG, IDC_GRCG2, IDC_EGC,
		IDC_PC980124
	};
	for (UINT i = 0; i < _countof(s_disabled); i++)
	{
		GetDlgItem(s_disabled[i]).EnableWindow(FALSE);
	}
#endif	// defined(SUPPORT_PC9821)

	return TRUE;
}

/**
 * ユーザーが OK のボタン (IDOK ID がのボタン) をクリックすると呼び出されます
 */
void ScrOptChipPage::OnOK()
{
	bool bUpdated = false;

	const UINT8 cMode = (IsDlgButtonChecked(IDC_GDC72020) != BST_UNCHECKED) ? 1 : 0;
	if (np2cfg.uPD72020 != cMode)
	{
		np2cfg.uPD72020 = cMode;
		bUpdated = true;
		gdc_restorekacmode();
		gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
	}

	for (UINT i = 0; i < _countof(s_gdcchip); i++)
	{
		if (IsDlgButtonChecked(s_gdcchip[i]) != BST_UNCHECKED)
		{
			const UINT8 cType = static_cast<UINT8>(i);
			if (np2cfg.grcg != cType)
			{
				np2cfg.grcg = cType;
				bUpdated = true;
				gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
			}
			break;
		}
	}

	const UINT8 cColor16 = (IsDlgButtonChecked(IDC_PC980124) != BST_UNCHECKED) ? 1 : 0;
	if (np2cfg.color16 != cColor16)
	{
		np2cfg.color16 = cColor16;
		bUpdated = true;
	}

	if (bUpdated)
	{
		::sysmng_update(SYS_UPDATECFG);
	}
}



// ----

/**
 * @brief Timing ページ
 */
class ScrOptTimingPage : public CPropPageProc
{
public:
	ScrOptTimingPage();
	virtual ~ScrOptTimingPage();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	CSliderValue m_tram;	//!< TRAM
	CSliderValue m_vram;	//!< VRAM
	CSliderValue m_grcg;	//!< GRCG
	CSliderValue m_raster;	//!< ラスタ
};

/**
 * コンストラクタ
 */
ScrOptTimingPage::ScrOptTimingPage()
	: CPropPageProc(IDD_SCROPT3)
{
}

/**
 * デストラクタ
 */
ScrOptTimingPage::~ScrOptTimingPage()
{
}

/**
 * このメソッドは WM_INITDIALOG のメッセージに応答して呼び出されます
 * @retval TRUE 最初のコントロールに入力フォーカスを設定
 * @retval FALSE 既に設定済
 */
BOOL ScrOptTimingPage::OnInitDialog()
{
	m_tram.SubclassDlgItem(IDC_TRAMWAIT, this);
	m_tram.SetStaticId(IDC_TRAMSTR);
	m_tram.SetRange(0, 32);
	m_tram.SetPos(np2cfg.wait[0]);

	m_vram.SubclassDlgItem(IDC_VRAMWAIT, this);
	m_vram.SetStaticId(IDC_VRAMSTR);
	m_vram.SetRange(0, 32);
	m_vram.SetPos(np2cfg.wait[2]);

	m_grcg.SubclassDlgItem(IDC_GRCGWAIT, this);
	m_grcg.SetStaticId(IDC_GRCGSTR);
	m_grcg.SetRange(0, 32);
	m_grcg.SetPos(np2cfg.wait[4]);

	m_raster.SubclassDlgItem(IDC_REALPAL, this);
	m_raster.SetStaticId(IDC_REALPALSTR);
	m_raster.SetRange(-32, 32);
	m_raster.SetPos(np2cfg.realpal - 32);

	return TRUE;
}

/**
 * ユーザーが OK のボタン (IDOK ID がのボタン) をクリックすると呼び出されます
 */
void ScrOptTimingPage::OnOK()
{
	bool bUpdated = false;

	UINT8 sWait[6];
	ZeroMemory(sWait, sizeof(sWait));
	sWait[0] = static_cast<UINT8>(m_tram.GetPos());
	sWait[2] = static_cast<UINT8>(m_vram.GetPos());
	sWait[4] = static_cast<UINT8>(m_grcg.GetPos());

	sWait[1] = (sWait[0]) ? 1 : 0;
	sWait[3] = (sWait[2]) ? 1 : 0;
	sWait[5] = (sWait[4]) ? 1 : 0;

	for (UINT i = 0; i < 6; i++)
	{
		if (np2cfg.wait[i] != sWait[i])
		{
			np2cfg.wait[i] = sWait[i];
			bUpdated = true;
		}
	}

	const UINT8 cRealPal = static_cast<UINT8>(m_raster.GetPos() + 32);
	if (np2cfg.realpal != cRealPal)
	{
		np2cfg.realpal = cRealPal;
		bUpdated = true;
	}

	if (bUpdated)
	{
		::sysmng_update(SYS_UPDATECFG);
	}
}

/**
 * CWndProc オブジェクトの Windows プロシージャ (WindowProc) が用意されています
 * @param[in] nMsg 処理される Windows メッセージを指定します
 * @param[in] wParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @param[in] lParam メッセージの処理で使う付加情報を提供します。このパラメータの値はメッセージに依存します
 * @return メッセージに依存する値を返します
 */
LRESULT ScrOptTimingPage::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (nMsg == WM_HSCROLL)
	{
		switch (::GetDlgCtrlID(reinterpret_cast<HWND>(lParam)))
		{
			case IDC_TRAMWAIT:
				m_tram.UpdateValue();
				break;

			case IDC_VRAMWAIT:
				m_vram.UpdateValue();
				break;

			case IDC_GRCGWAIT:
				m_grcg.UpdateValue();
				break;

			case IDC_REALPAL:
				m_raster.UpdateValue();
				break;

			default:
				break;
		}
	}
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}



// ----

/**
 * @brief Fullscreen ページ
 */
class ScrOptFullscreenPage : public CPropPageProc
{
public:
	ScrOptFullscreenPage();
	virtual ~ScrOptFullscreenPage();

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

private:
	CComboData m_zoom;				//!< ズーム
};

//! ズーム リスト
static const CComboData::Entry s_zoom[] =
{
	{MAKEINTRESOURCE(IDS_ZOOM_NONE),			0},
	{MAKEINTRESOURCE(IDS_ZOOM_FIXEDASPECT),		1},
	{MAKEINTRESOURCE(IDS_ZOOM_ADJUSTASPECT),	2},
	{MAKEINTRESOURCE(IDS_ZOOM_FULL),			3},
};

/**
 * コンストラクタ
 */
ScrOptFullscreenPage::ScrOptFullscreenPage()
	: CPropPageProc(IDD_SCROPT_FULLSCREEN)
{
}

/**
 * デストラクタ
 */
ScrOptFullscreenPage::~ScrOptFullscreenPage()
{
}

/**
 * このメソッドは WM_INITDIALOG のメッセージに応答して呼び出されます
 * @retval TRUE 最初のコントロールに入力フォーカスを設定
 * @retval FALSE 既に設定済
 */
BOOL ScrOptFullscreenPage::OnInitDialog()
{
	const UINT8 c = np2oscfg.fscrnmod;
	CheckDlgButton(IDC_FULLSCREEN_SAMEBPP, (c & FSCRNMOD_SAMEBPP) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_FULLSCREEN_SAMERES, (c & FSCRNMOD_SAMERES) ? BST_CHECKED : BST_UNCHECKED);

	m_zoom.SubclassDlgItem(IDC_FULLSCREEN_ZOOM, this);
	m_zoom.Add(s_zoom, _countof(s_zoom));
	m_zoom.SetCurItemData(c & 3);
	m_zoom.EnableWindow((c & FSCRNMOD_SAMERES) ? TRUE : FALSE);

	return TRUE;
}

/**
 * ユーザーが OK のボタン (IDOK ID がのボタン) をクリックすると呼び出されます
 */
void ScrOptFullscreenPage::OnOK()
{
	UINT8 c = 0;
	if (IsDlgButtonChecked(IDC_FULLSCREEN_SAMEBPP) != BST_UNCHECKED)
	{
		c |= FSCRNMOD_SAMEBPP;
	}
	if (IsDlgButtonChecked(IDC_FULLSCREEN_SAMERES) != BST_UNCHECKED)
	{
		c |= FSCRNMOD_SAMERES;
	}
	c |= m_zoom.GetCurItemData(np2oscfg.fscrnmod & 3);
	if (np2oscfg.fscrnmod != c)
	{
		np2oscfg.fscrnmod = c;
		::sysmng_update(SYS_UPDATEOSCFG);
	}
}

/**
 * ユーザーがメニューの項目を選択したときに、フレームワークによって呼び出されます
 * @param[in] wParam パラメタ
 * @param[in] lParam パラメタ
 * @retval TRUE アプリケーションがこのメッセージを処理した
 */
BOOL ScrOptFullscreenPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) == IDC_FULLSCREEN_SAMERES)
	{
		m_zoom.EnableWindow((IsDlgButtonChecked(IDC_FULLSCREEN_SAMERES) != BST_UNCHECKED) ? TRUE : FALSE);
		return TRUE;
	}
	return FALSE;
}



// ----

/**
 * スクリーン設定
 * @param[in] hwndParent 親ウィンドウ
 */
void dialog_scropt(HWND hwndParent)
{
	CPropSheetProc prop(IDS_SCREENOPTION, hwndParent);

	ScrOptVideoPage video;
	prop.AddPage(&video);

	ScrOptChipPage chip;
	prop.AddPage(&chip);

	ScrOptTimingPage timing;
	prop.AddPage(&timing);

	ScrOptFullscreenPage fullscreen;
	prop.AddPage(&fullscreen);

	prop.m_psh.dwFlags |= PSH_NOAPPLYNOW | PSH_USEHICON | PSH_USECALLBACK;
	prop.m_psh.hIcon = LoadIcon(CWndProc::GetResourceHandle(), MAKEINTRESOURCE(IDI_ICON2));
	prop.m_psh.pfnCallback = np2class_propetysheet;
	prop.DoModal();

	::InvalidateRect(hwndParent, NULL, TRUE);
}

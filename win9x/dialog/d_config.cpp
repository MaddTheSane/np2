/**
 * @file	d_config.cpp
 * @brief	設定ダイアログ
 */

#include "compiler.h"
#include "resource.h"
#include "dialog.h"
#include "c_combodata.h"
#include "np2.h"
#include "sysmng.h"
#include "misc/DlgProc.h"
#include "pccore.h"
#include "common/strres.h"

/**
 * @brief 設定ダイアログ
 * @param[in] hwndParent 親ウィンドウ
 */
class CConfigureDlg : public CDlgProc
{
public:
	CConfigureDlg(HWND hwndParent);

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

private:
	CComboData m_baseClock;				//!< ベース クロック
	CComboData m_multiple;				//!< 倍率
	CComboData m_rate;					//!< レート
	void SetClock(UINT nMultiple = 0);
};

//! コンボ ボックス アイテム
static const CComboData::Entry s_baseclock[] =
{
	{MAKEINTRESOURCE(IDS_2_0MHZ),	PCBASECLOCK20},
	{MAKEINTRESOURCE(IDS_2_5MHZ),	PCBASECLOCK25},
};

//! 倍率リスト
static const UINT32 s_mulval[10] = {1, 2, 4, 5, 6, 8, 10, 12, 16, 20};

//! クロック フォーマット
static const TCHAR str_clockfmt[] = _T("%2u.%.4u");

//! サンプリング レート
static const UINT32 s_nSamplingRate[] = {11025, 22050, 44100, 48000};

/**
 * コンストラクタ
 * @param[in] hwndParent 親ウィンドウ
 */
CConfigureDlg::CConfigureDlg(HWND hwndParent)
	: CDlgProc(IDD_CONFIG, hwndParent)
{
}

/**
 * このメソッドは WM_INITDIALOG のメッセージに応答して呼び出されます
 * @retval TRUE 最初のコントロールに入力フォーカスを設定
 * @retval FALSE 既に設定済
 */
BOOL CConfigureDlg::OnInitDialog()
{
	m_baseClock.SubclassDlgItem(IDC_BASECLOCK, this);
	m_baseClock.Add(s_baseclock, _countof(s_baseclock));
	const UINT32 nBaseClock = (np2cfg.baseclock == PCBASECLOCK20) ? PCBASECLOCK20 : PCBASECLOCK25;
	m_baseClock.SetCurItemData(nBaseClock);

	m_multiple.SubclassDlgItem(IDC_MULTIPLE, this);
	m_multiple.Add(s_mulval, _countof(s_mulval));
	SetDlgItemInt(IDC_MULTIPLE, np2cfg.multiple, FALSE);

	UINT nModel;
	if (!milstr_cmp(np2cfg.model, str_VM))
	{
		nModel = IDC_MODELVM;
	}
	else if (!milstr_cmp(np2cfg.model, str_EPSON))
	{
		nModel = IDC_MODELEPSON;
	}
	else
	{
		nModel = IDC_MODELVX;
	}
	CheckDlgButton(nModel, BST_CHECKED);

	m_rate.SubclassDlgItem(IDC_SOUND_RATE, this);
	m_rate.Add(s_nSamplingRate, _countof(s_nSamplingRate));
	int nIndex = m_rate.FindItemData(np2cfg.samplingrate);
	if (nIndex == CB_ERR)
	{
		nIndex = m_rate.Add(np2cfg.samplingrate);
	}
	m_rate.SetCurSel(nIndex);

	SetDlgItemInt(IDC_SOUND_BUFFER, np2cfg.delayms, FALSE);

	CheckDlgButton(IDC_ALLOWRESIZE, (np2oscfg.thickframe) ? BST_CHECKED : BST_UNCHECKED);

#if !defined(_WIN64)
	if (mmxflag & MMXFLAG_NOTSUPPORT)
	{
		GetDlgItem(IDC_DISABLEMMX).EnableWindow(FALSE);
		CheckDlgButton(IDC_DISABLEMMX, BST_CHECKED);
	}
	else
	{
		CheckDlgButton(IDC_DISABLEMMX, (np2oscfg.disablemmx) ? BST_CHECKED : BST_UNCHECKED);
	}
#endif	// !defined(_WIN64)

	CheckDlgButton(IDC_COMFIRM, (np2oscfg.comfirm) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(IDC_RESUME, (np2oscfg.resume) ? BST_CHECKED : BST_UNCHECKED);
	SetClock();
	m_baseClock.SetFocus();

	return FALSE;
}

/**
 * ユーザーが OK のボタン (IDOK ID がのボタン) をクリックすると呼び出されます
 */
void CConfigureDlg::OnOK()
{
	UINT update = 0;

	const UINT nBaseClock = m_baseClock.GetCurItemData(PCBASECLOCK20);
	if (np2cfg.baseclock != nBaseClock)
	{
		np2cfg.baseclock = nBaseClock;
		update |= SYS_UPDATECFG | SYS_UPDATECLOCK;
	}

	UINT nMultiple = GetDlgItemInt(IDC_MULTIPLE, NULL, FALSE);
	nMultiple = max(nMultiple, 1);
	nMultiple = min(nMultiple, 32);
	if (np2cfg.multiple != nMultiple)
	{
		np2cfg.multiple = nMultiple;
		update |= SYS_UPDATECFG | SYS_UPDATECLOCK;
	}

	LPCTSTR str;
	if (IsDlgButtonChecked(IDC_MODELVM) != BST_UNCHECKED)
	{
		str = str_VM;
	}
	else if (IsDlgButtonChecked(IDC_MODELEPSON) != BST_UNCHECKED)
	{
		str = str_EPSON;
	}
	else {
		str = str_VX;
	}
	if (milstr_cmp(np2cfg.model, str))
	{
		milstr_ncpy(np2cfg.model, str, NELEMENTS(np2cfg.model));
		update |= SYS_UPDATECFG;
	}

	const UINT nSamplingRate = m_rate.GetCurItemData(np2cfg.samplingrate);
	if (np2cfg.samplingrate != nSamplingRate)
	{
		np2cfg.samplingrate = nSamplingRate;
		update |= SYS_UPDATECFG | SYS_UPDATERATE;
		soundrenewal = 1;
	}

	UINT nBuffer = GetDlgItemInt(IDC_SOUND_BUFFER, NULL, FALSE);
	nBuffer = max(nBuffer, 40);
	nBuffer = min(nBuffer, 1000);
	if (np2cfg.delayms != static_cast<UINT16>(nBuffer))
	{
		np2cfg.delayms = static_cast<UINT16>(nBuffer);
		update |= SYS_UPDATECFG | SYS_UPDATESBUF;
		soundrenewal = 1;
	}

	const UINT8 bAllowResize = (IsDlgButtonChecked(IDC_ALLOWRESIZE) != BST_UNCHECKED) ? 1 : 0;
	if (np2oscfg.thickframe != bAllowResize)
	{
		np2oscfg.thickframe = bAllowResize;
		update |= SYS_UPDATEOSCFG;
	}

#if !defined(_WIN64)
	if (!(mmxflag & MMXFLAG_NOTSUPPORT))
	{
		const UINT8 bDisableMMX = (IsDlgButtonChecked(IDC_DISABLEMMX) != BST_UNCHECKED) ? 1 : 0;
		if (np2oscfg.disablemmx != bDisableMMX)
		{
			np2oscfg.disablemmx = bDisableMMX;
			mmxflag &= ~MMXFLAG_DISABLE;
			mmxflag |= (bDisableMMX) ? MMXFLAG_DISABLE : 0;
			update |= SYS_UPDATEOSCFG;
		}
	}
#endif

	const UINT8 bConfirm = (IsDlgButtonChecked(IDC_COMFIRM) != BST_UNCHECKED) ? 1 : 0;
	if (np2oscfg.comfirm != bConfirm)
	{
		np2oscfg.comfirm = bConfirm;
		update |= SYS_UPDATEOSCFG;
	}

	const UINT8 bResume = (IsDlgButtonChecked(IDC_RESUME) != BST_UNCHECKED) ? 1 : 0;
	if (np2oscfg.resume != bResume)
	{
		np2oscfg.resume = bResume;
		update |= SYS_UPDATEOSCFG;
	}
	sysmng_update(update);

	CDlgProc::OnOK();
}

/**
 * ユーザーがメニューの項目を選択したときに、フレームワークによって呼び出されます
 * @param[in] wParam パラメタ
 * @param[in] lParam パラメタ
 * @retval TRUE アプリケーションがこのメッセージを処理した
 */
BOOL CConfigureDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDC_BASECLOCK:
			SetClock();
			return TRUE;

		case IDC_MULTIPLE:
			if (HIWORD(wParam) == CBN_SELCHANGE)
			{
				const int nIndex = m_multiple.GetCurSel();
				if ((nIndex >= 0) && (nIndex < _countof(s_mulval)))
				{
					SetClock(s_mulval[nIndex]);
				}
			}
			else
			{
				SetClock(0);
			}
			return TRUE;
	}
	return FALSE;
}

/**
 * クロックを設定する
 * @param[in] nMultiple 倍率
 */
void CConfigureDlg::SetClock(UINT nMultiple)
{
	const UINT nBaseClock = m_baseClock.GetCurItemData(PCBASECLOCK20);
	if (nMultiple == 0)
	{
		nMultiple = GetDlgItemInt(IDC_MULTIPLE, NULL, FALSE);
	}
	nMultiple = max(nMultiple, 1);
	nMultiple = min(nMultiple, 32);

	const UINT nClock = (nBaseClock / 100) * nMultiple;

	TCHAR szWork[32];
	wsprintf(szWork, str_clockfmt, nClock / 10000, nClock % 10000);
	SetDlgItemText(IDC_CLOCKMSG, szWork);
}

/**
 * 設定ダイアログ
 * @param[in] hwndParent 親ウィンドウ
 */
void dialog_configure(HWND hwndParent)
{
	CConfigureDlg dlg(hwndParent);
	dlg.DoModal();
}

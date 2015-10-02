/**
 * @file	d_config.cpp
 * @brief	�ݒ�_�C�A���O
 */

#include "compiler.h"
#include "resource.h"
#include "dialog.h"
#include "dialogs.h"
#include "pccore.h"
#include "common/strres.h"
#include "np2.h"
#include "sysmng.h"
#include "misc/DlgProc.h"

/**
 * @brief �ݒ�_�C�A���O
 * @param[in] hwndParent �e�E�B���h�E
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
	void SetClock(UINT nMultiple = 0);
	CNp2ComboBox m_baseClock;			//!< �x�[�X �N���b�N
	CNp2ComboBox m_multiple;			//!< �{��
};

//! �R���{ �{�b�N�X �A�C�e��
static const CBPARAM s_cpBase[] =
{
	{MAKEINTRESOURCE(IDS_2_0MHZ),	PCBASECLOCK20},
	{MAKEINTRESOURCE(IDS_2_5MHZ),	PCBASECLOCK25},
};

//! �{�����X�g
static const UINT32 s_mulval[10] = {1, 2, 4, 5, 6, 8, 10, 12, 16, 20};

//! �N���b�N �t�H�[�}�b�g
static const TCHAR str_clockfmt[] = _T("%2u.%.4u");

/**
 * �R���X�g���N�^
 * @param[in] hwndParent �e�E�B���h�E
 */
CConfigureDlg::CConfigureDlg(HWND hwndParent)
	: CDlgProc(IDD_CONFIG, hwndParent)
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL CConfigureDlg::OnInitDialog()
{
	m_baseClock.SubclassDlgItem(IDC_BASECLOCK, this);
	m_baseClock.Add(s_cpBase, _countof(s_cpBase));
	const UINT32 nBaseClock = (np2cfg.baseclock < AVE(PCBASECLOCK25, PCBASECLOCK20)) ? PCBASECLOCK20 : PCBASECLOCK25;
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

	UINT nSamplingRate;
	if (np2cfg.samplingrate < AVE(11025, 22050))
	{
		nSamplingRate = IDC_RATE11;
	}
	else if (np2cfg.samplingrate < AVE(22050, 44100))
	{
		nSamplingRate = IDC_RATE22;
	}
	else
	{
		nSamplingRate = IDC_RATE44;
	}
	CheckDlgButton(nSamplingRate, BST_CHECKED);
	SetDlgItemInt(IDC_SOUNDBUF, np2cfg.delayms, FALSE);

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
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
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

	UINT16 nSamplingRate;
	if (IsDlgButtonChecked(IDC_RATE11) != BST_UNCHECKED)
	{
		nSamplingRate = 11025;
	}
	else if (IsDlgButtonChecked(IDC_RATE22) != BST_UNCHECKED)
	{
		nSamplingRate = 22050;
	}
	else
	{
		nSamplingRate = 44100;
	}
	if (np2cfg.samplingrate != nSamplingRate)
	{
		np2cfg.samplingrate = nSamplingRate;
		update |= SYS_UPDATECFG | SYS_UPDATERATE;
		soundrenewal = 1;
	}

	UINT16 nBuffer = GetDlgItemInt(IDC_SOUNDBUF, NULL, FALSE);
	nBuffer = max(nBuffer, 40);
	nBuffer = min(nBuffer, 1000);
	if (np2cfg.delayms != nBuffer)
	{
		np2cfg.delayms = nBuffer;
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
 * ���[�U�[�����j���[�̍��ڂ�I�������Ƃ��ɁA�t���[�����[�N�ɂ���ČĂяo����܂�
 * @param[in] wParam �p�����^
 * @param[in] lParam �p�����^
 * @retval TRUE �A�v���P�[�V���������̃��b�Z�[�W����������
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
 * �N���b�N��ݒ肷��
 * @param[in] nMultiple �{��
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
 * �ݒ�_�C�A���O
 * @param[in] hwndParent �e�E�B���h�E
 */
void dialog_configure(HWND hwndParent)
{
	CConfigureDlg dlg(hwndParent);
	dlg.DoModal();
}

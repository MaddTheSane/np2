/**
 * @file	d_disk.cpp
 * @brief	disk dialog
 */

#include "compiler.h"
#include "resource.h"
#include "dialog.h"
#include "dialogs.h"
#include "dosio.h"
#include "np2.h"
#include "sysmng.h"
#include "misc/DlgProc.h"
#include "subwnd/toolwnd.h"
#include "pccore.h"
#include "common/strres.h"
#include "fdd/diskdrv.h"
#include "fdd/fddfile.h"
#include "fdd/newdisk.h"

static const FSPARAM fpFDD =
{
	MAKEINTRESOURCE(IDS_FDDTITLE),
	MAKEINTRESOURCE(IDS_FDDEXT),
	MAKEINTRESOURCE(IDS_FDDFILTER),
	3
};

#if defined(SUPPORT_SASI)
static const FSPARAM fpSASI =
{
	MAKEINTRESOURCE(IDS_SASITITLE),
	MAKEINTRESOURCE(IDS_HDDEXT),
	MAKEINTRESOURCE(IDS_HDDFILTER),
	4
};
#else	// defined(SUPPORT_SASI)
static const FSPARAM fpSASI =
{
	MAKEINTRESOURCE(IDS_HDDTITLE),
	MAKEINTRESOURCE(IDS_HDDEXT),
	MAKEINTRESOURCE(IDS_HDDFILTER),
	4
};
#endif	// defined(SUPPORT_SASI)

#if defined(SUPPORT_IDEIO)
static const FSPARAM fpISO =
{
	MAKEINTRESOURCE(IDS_ISOTITLE),
	MAKEINTRESOURCE(IDS_ISOEXT),
	MAKEINTRESOURCE(IDS_ISOFILTER),
	3
};
#endif	// defined(SUPPORT_IDEIO)

#if defined(SUPPORT_SCSI)
static const FSPARAM fpSCSI =
{
	MAKEINTRESOURCE(IDS_SCSITITLE),
	MAKEINTRESOURCE(IDS_SCSIEXT),
	MAKEINTRESOURCE(IDS_SCSIFILTER),
	1
};
#endif	// defined(SUPPORT_SCSI)



// ----

/**
 * FDD �I���_�C�A���O
 * @param[in] hWnd �e�E�B���h�E
 * @param[in] drv �h���C�u
 */
void dialog_changefdd(HWND hWnd, REG8 drv)
{
	if (drv < 4)
	{
		LPCTSTR p = fdd_diskname(drv);
		if ((p == NULL) || (p[0] == '\0'))
		{
			p = fddfolder;
		}
		TCHAR path[MAX_PATH];
		file_cpyname(path, p, _countof(path));

		int readonly;
		if (dlgs_openfile(hWnd, &fpFDD, path, _countof(path), &readonly))
		{
			file_cpyname(fddfolder, path, _countof(fddfolder));
			sysmng_update(SYS_UPDATEOSCFG);
			diskdrv_setfdd(drv, path, readonly);
			toolwin_setfdd(drv, path);
		}
	}
}

/**
 * HDD �I���_�C�A���O
 * @param[in] hWnd �e�E�B���h�E
 * @param[in] drv �h���C�u
 */
void dialog_changehdd(HWND hWnd, REG8 drv)
{
	LPCTSTR p = diskdrv_getsxsi(drv);
	UINT num = drv & 0x0f;

	PCFSPARAM pfp = NULL;
	if (!(drv & 0x20))			// SASI/IDE
	{
		if (num < 2)
		{
			pfp = &fpSASI;
		}
#if defined(SUPPORT_IDEIO)
		else if (num == 2)
		{
			pfp = &fpISO;
		}
#endif
	}
#if defined(SUPPORT_SCSI)
	else						// SCSI
	{
		if (num < 4)
		{
			pfp = &fpSCSI;
		}
	}
#endif
	if (pfp == NULL)
	{
		return;
	}
	if ((p == NULL) || (p[0] == '\0'))
	{
		p = hddfolder;
	}
	TCHAR path[MAX_PATH];
	file_cpyname(path, p, _countof(path));
	if (dlgs_openfile(hWnd, pfp, path, _countof(path), NULL))
	{
		file_cpyname(hddfolder, path, _countof(hddfolder));
		sysmng_update(SYS_UPDATEOSCFG);
		diskdrv_sethdd(drv, path);
	}
}


// ---- newdisk

/** �f�t�H���g�� */
static const TCHAR str_newdisk[] = TEXT("newdisk");

/** HDD �T�C�Y */
static const UINT32 hddsizetbl[5] = {20, 41, 65, 80, 128};

/** SASI HDD */
static const UINT16 s_sasires[6] = 
{
	IDC_NEWSASI5MB, IDC_NEWSASI10MB,
	IDC_NEWSASI15MB, IDC_NEWSASI20MB,
	IDC_NEWSASI30MB, IDC_NEWSASI40MB
};

/**
 * @brief �V����HDD
 */
class CNewHddDlg : public CDlgProc
{
public:
	/**
	 * �R���X�g���N�^
	 * @param[in] hwndParent �e�E�B���h�E
	 * @param[in] nHddMinSize �ŏ��T�C�Y
	 * @param[in] nHddMaxSize �ő�T�C�Y
	 */
	CNewHddDlg(HWND hwndParent, UINT nHddMinSize, UINT nHddMaxSize)
		: CDlgProc(IDD_NEWHDDDISK, hwndParent)
		, m_nHddSize(0)
		, m_nHddMinSize(nHddMinSize)
		, m_nHddMaxSize(nHddMaxSize)
	{
	}

	/**
	 * �T�C�Y��Ԃ�
	 * @return �T�C�Y
	 */
	UINT GetSize() const
	{
		return m_nHddSize;
	}

protected:
	/**
	 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
	 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
	 * @retval FALSE ���ɐݒ��
	 */
	virtual BOOL OnInitDialog()
	{
		SETLISTUINT32(m_hWnd, IDC_HDDSIZE, hddsizetbl);

		TCHAR work[32];
		::wsprintf(work, TEXT("(%d-%dMB)"), m_nHddMinSize, m_nHddMaxSize);
		SetDlgItemText(IDC_HDDLIMIT, work);
		GetDlgItem(IDC_HDDSIZE).SetFocus();
		return FALSE;
	}

	/**
	 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
	 */
	virtual void OnOK()
	{
		UINT nSize = GetDlgItemInt(IDC_HDDSIZE, NULL, FALSE);
		nSize = max(nSize, m_nHddMinSize);
		nSize = min(nSize, m_nHddMaxSize);
		m_nHddSize = nSize;
		CDlgProc::OnOK();
	}

private:
	UINT m_nHddSize;				/*!< HDD �T�C�Y */
	UINT m_nHddMinSize;				/*!< �ŏ��T�C�Y */
	UINT m_nHddMaxSize;				/*!< �ő�T�C�Y */
};



/**
 * @brief �V����HDD
 */
class CNewSasiDlg : public CDlgProc
{
public:
	/**
	 * �R���X�g���N�^
	 * @param[in] hwndParent �e�E�B���h�E
	 */
	CNewSasiDlg(HWND hwndParent)
		: CDlgProc(IDD_NEWSASI, hwndParent)
		, m_nType(0)
	{
	}

	/**
	 * HDD �^�C�v�𓾂�
	 * @return HDD �^�C�v
	 */
	UINT GetType() const
	{
		return m_nType;
	}

protected:
	/**
	 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
	 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
	 * @retval FALSE ���ɐݒ��
	 */
	virtual BOOL OnInitDialog()
	{
		GetDlgItem(IDC_NEWSASI5MB).SetFocus();
		return FALSE;
	}

	/**
	 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
	 */
	virtual void OnOK()
	{
		for (UINT i = 0; i < 6; i++)
		{
			if (IsDlgButtonChecked(s_sasires[i]) != BST_UNCHECKED)
			{
				m_nType = (i > 3) ? (i + 1) : i;
				CDlgProc::OnOK();
				break;
			}
		}
	}

private:
	UINT m_nType;			/*!< HDD �^�C�v */
};

/**
 * @brief �V����FDD
 */
class CNewFddDlg : public CDlgProc
{
public:
	/**
	 * �R���X�g���N�^
	 * @param[in] hwndParent �e�E�B���h�E
	 */
	CNewFddDlg(HWND hwndParent)
		: CDlgProc((np2cfg.usefd144) ? IDD_NEWDISK2 : IDD_NEWDISK, hwndParent)
		, m_nFdType(DISKTYPE_2HD << 4)
	{
	}

	/**
	 * �^�C�v�𓾂�
	 * @return �^�C�v
	 */
	UINT8 GetType() const
	{
		return m_nFdType;
	}

	/**
	 * ���x���𓾂�
	 * @return ���x��
	 */
	LPCTSTR GetLabel() const
	{
		return m_szDiskLabel;
	}

protected:
	/**
	 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
	 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
	 * @retval FALSE ���ɐݒ��
	 */
	virtual BOOL OnInitDialog()
	{
		UINT res;
		switch (m_nFdType)
		{
			case (DISKTYPE_2DD << 4):
				res = IDC_MAKE2DD;
				break;

			case (DISKTYPE_2HD << 4):
				res = IDC_MAKE2HD;
				break;

			default:
				res = IDC_MAKE144;
				break;
		}
		CheckDlgButton(res, BST_CHECKED);
		GetDlgItem(IDC_DISKLABEL).SetFocus();
		return FALSE;
	}

	/**
	 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
	 */
	virtual void OnOK()
	{
		GetDlgItemText(IDC_DISKLABEL, m_szDiskLabel, _countof(m_szDiskLabel));
		if (milstr_kanji1st(m_szDiskLabel, _countof(m_szDiskLabel) - 1))
		{
			m_szDiskLabel[_countof(m_szDiskLabel) - 1] = '\0';
		}
		if (IsDlgButtonChecked(IDC_MAKE2DD) != BST_UNCHECKED)
		{
			m_nFdType = (DISKTYPE_2DD << 4);
		}
		else if (IsDlgButtonChecked(IDC_MAKE2HD) != BST_UNCHECKED)
		{
			m_nFdType = (DISKTYPE_2HD << 4);
		}
		else
		{
			m_nFdType = (DISKTYPE_2HD << 4) + 1;
		}
		CDlgProc::OnOK();
	}

private:
	UINT m_nFdType;					/*!< �^�C�v */
	TCHAR m_szDiskLabel[16 + 1];	/*!< ���x�� */
};

/**
 * �V�K�f�B�X�N�쐬 �_�C�A���O
 * @param[in] hWnd �e�E�B���h�E
 */
void dialog_newdisk(HWND hWnd)
{
	TCHAR szPath[MAX_PATH];
	file_cpyname(szPath, fddfolder, _countof(szPath));
	file_cutname(szPath);
	file_catname(szPath, str_newdisk, _countof(szPath));

	std::tstring rTitle(LoadTString(IDS_NEWDISKTITLE));
	std::tstring rDefExt(LoadTString(IDS_NEWDISKEXT));
#if defined(SUPPORT_SCSI)
	std::tstring rFilter(LoadTString(IDS_NEWDISKFILTER));
#else	// defined(SUPPORT_SCSI)
	std::tstring rFilter(LoadTString(IDS_NEWDISKFILTER2));
#endif	// defined(SUPPORT_SCSI)

	CFileDlg fileDlg(FALSE, rDefExt.c_str(), szPath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, rFilter.c_str(), hWnd);
	fileDlg.m_ofn.lpstrTitle = rTitle.c_str();
	if (fileDlg.DoModal() != IDOK)
	{
		return;
	}

	LPCTSTR lpPath = fileDlg.GetPathName();
	LPCTSTR ext = file_getext(lpPath);
	if (!file_cmpname(ext, str_thd))
	{
		CNewHddDlg dlg(hWnd, 5, 256);
		if (dlg.DoModal() == IDOK)
		{
			newdisk_thd(lpPath, dlg.GetSize());
		}
	}
	else if (!file_cmpname(ext, str_nhd))
	{
		CNewHddDlg dlg(hWnd, 5, 512);
		if (dlg.DoModal() == IDOK)
		{
			newdisk_nhd(lpPath, dlg.GetSize());
		}
	}
	else if (!file_cmpname(ext, str_hdi))
	{
		CNewSasiDlg dlg(hWnd);
		if (dlg.DoModal() == IDOK)
		{
			newdisk_hdi(lpPath, dlg.GetType());
		}
	}
#if defined(SUPPORT_SCSI)
	else if (!file_cmpname(ext, str_hdd))
	{
		CNewHddDlg dlg(hWnd, 2, 512);
		if (dlg.DoModal() == IDOK)
		{
			newdisk_vhd(lpPath, dlg.GetSize());
		}
	}
#endif
	else if ((!file_cmpname(ext, str_d88)) ||
			(!file_cmpname(ext, str_d98)) ||
			(!file_cmpname(ext, str_88d)) ||
			(!file_cmpname(ext, str_98d)))
	{
		CNewFddDlg dlg(hWnd);
		if (dlg.DoModal()  == IDOK)
		{
			newdisk_fdd(lpPath, dlg.GetType(), dlg.GetLabel());
		}
	}
}

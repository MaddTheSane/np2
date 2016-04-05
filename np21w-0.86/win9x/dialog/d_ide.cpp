/**
 * @file	d_ide.cpp
 * @brief	IDE �ݒ�_�C�A���O
 */

#include "compiler.h"
#include "resource.h"
#include "dialog.h"
#include "c_combodata.h"
#include "np2.h"
#include "commng.h"
#include "sysmng.h"
#include "misc/DlgProc.h"
#include "pccore.h"
#include "common/strres.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif


/**
 * @brief MPU-PC98 �ݒ�_�C�A���O
 * @param[in] hwndParent �e�E�B���h�E
 */
class CIdeDlg : public CDlgProc
{
public:
	CIdeDlg(HWND hwndParent);

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);

private:
	CComboData m_cmbpm;			//!< �v���C�}�� �}�X�^
	CComboData m_cmbps;			//!< �v���C�}�� �X���[�u
	CComboData m_cmbsm;			//!< �Z�J���_�� �}�X�^
	CComboData m_cmbss;			//!< �Z�J���_�� �X���[�u
};

/**
 * ���荞�݃��X�g
 */
static const CComboData::Entry s_type[] =
{
	{MAKEINTRESOURCE(IDS_IDETYPE_NONE ),		0},
	{MAKEINTRESOURCE(IDS_IDETYPE_HDD  ),		1},
	{MAKEINTRESOURCE(IDS_IDETYPE_CDROM),		2},
};

/**
 * �R���X�g���N�^
 * @param[in] hwndParent �e�E�B���h�E
 */
CIdeDlg::CIdeDlg(HWND hwndParent)
	: CDlgProc(IDD_IDE, hwndParent)
{
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL CIdeDlg::OnInitDialog()
{
	m_cmbpm.SubclassDlgItem(IDC_IDE1TYPE, this);
	m_cmbpm.Add(s_type, _countof(s_type));
	m_cmbpm.SetCurItemData(np2cfg.idetype[0]);
	
	m_cmbps.SubclassDlgItem(IDC_IDE2TYPE, this);
	m_cmbps.Add(s_type, _countof(s_type));
	m_cmbps.SetCurItemData(np2cfg.idetype[1]);

	m_cmbsm.SubclassDlgItem(IDC_IDE3TYPE, this);
	m_cmbsm.Add(s_type, _countof(s_type));
	m_cmbsm.SetCurItemData(np2cfg.idetype[2]);

	m_cmbss.SubclassDlgItem(IDC_IDE4TYPE, this);
	m_cmbss.Add(s_type, _countof(s_type));
	m_cmbss.SetCurItemData(np2cfg.idetype[3]);

	m_cmbpm.SetFocus();

	return FALSE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void CIdeDlg::OnOK()
{
	UINT update = 0;

	if (m_cmbpm.GetCurItemData(np2cfg.idetype[0])!=np2cfg.idetype[0])
	{
		np2cfg.idetype[0] = m_cmbpm.GetCurItemData(np2cfg.idetype[0]);
		update |= SYS_UPDATECFG | SYS_UPDATEHDD;
	}
	if (m_cmbps.GetCurItemData(np2cfg.idetype[1])!=np2cfg.idetype[1])
	{
		np2cfg.idetype[1] = m_cmbps.GetCurItemData(np2cfg.idetype[1]);
		update |= SYS_UPDATECFG | SYS_UPDATEHDD;
	}
	if (m_cmbsm.GetCurItemData(np2cfg.idetype[2])!=np2cfg.idetype[2])
	{
		np2cfg.idetype[2] = m_cmbsm.GetCurItemData(np2cfg.idetype[2]);
		update |= SYS_UPDATECFG | SYS_UPDATEHDD;
	}
	if (m_cmbss.GetCurItemData(np2cfg.idetype[3])!=np2cfg.idetype[3])
	{
		np2cfg.idetype[3] = m_cmbss.GetCurItemData(np2cfg.idetype[3]);
		update |= SYS_UPDATECFG | SYS_UPDATEHDD;
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
BOOL CIdeDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDC_IDE1TYPE:
		case IDC_IDE2TYPE:
		case IDC_IDE3TYPE:
		case IDC_IDE4TYPE:
			return TRUE;
	}
	return FALSE;
}

/**
 * CWndProc �I�u�W�F�N�g�� Windows �v���V�[�W�� (WindowProc) ���p�ӂ���Ă��܂�
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT CIdeDlg::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	return CDlgProc::WindowProc(nMsg, wParam, lParam);
}

/**
 * �R���t�B�O �_�C�A���O
 * @param[in] hwndParent �e�E�B���h�E
 */
void dialog_ideopt(HWND hwndParent)
{
	CIdeDlg dlg(hwndParent);
	dlg.DoModal();
}

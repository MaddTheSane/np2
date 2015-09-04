/**
 * @file	DlgProc.cpp
 * @brief	�_�C�A���O �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "DlgProc.h"

/**
 * �R���X�g���N�^
 */
CDlgProc::CDlgProc()
	: m_lpszTemplateName(NULL)
	, m_hwndParent(NULL)
{
}

/**
 * �R���X�g���N�^
 * @param[in] nIDTemplate �_�C�A���O �{�b�N�X �e���v���[�g�̃��\�[�X id �ԍ����w�肵�܂�
 * @param[in] hwndParent �e�E�B���h�E
 */
CDlgProc::CDlgProc(UINT nIDTemplate, HWND hwndParent)
	: m_lpszTemplateName(MAKEINTRESOURCE(nIDTemplate))
	, m_hwndParent(hwndParent)
{
}

/**
 * �f�X�g���N�^
 */
CDlgProc::~CDlgProc()
{
}

/**
 * ���[�_��
 * @return �_�C�A���O �{�b�N�X����邽�߂Ɏg�p�����ACDialog::EndDialog �̃����o�[�֐��ɓn���ꂽ nResult �̃p�����[�^�[�l���w�肷�� int �̒l
 */
INT_PTR CDlgProc::DoModal()
{
	HookWindowCreate(this);

	const INT_PTR nRet = ::DialogBox(GetInstanceHandle(), m_lpszTemplateName, m_hwndParent, DlgProc);

	if (!UnhookWindowCreate())
	{
		PostNcDestroy();
	}
	return nRet;
}

/**
 * �_�C�A���O �v���V�[�W��
 * @param[in] hWnd �E�B���h�E �n���h��
 * @param[in] message ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
BOOL CALLBACK CDlgProc::DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_INITDIALOG)
	{
		CDlgProc* pDlg = static_cast<CDlgProc*>(FromHandlePermanent(hWnd));
		if (pDlg != NULL)
		{
			return pDlg->OnInitDialog();
		}
		else
		{
			return TRUE;
		}
	}
	return FALSE;
}

/**
 * CDlgProc �I�u�W�F�N�g�� Windows �v���V�[�W��
 * @param[in] nMsg ��������� Windows ���b�Z�[�W���w�肵�܂�
 * @param[in] wParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @param[in] lParam ���b�Z�[�W�̏����Ŏg���t������񋟂��܂��B���̃p�����[�^�̒l�̓��b�Z�[�W�Ɉˑ����܂�
 * @return ���b�Z�[�W�Ɉˑ�����l��Ԃ��܂�
 */
LRESULT CDlgProc::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (nMsg == WM_COMMAND)
	{
		switch (LOWORD(wParam))
		{
			case IDOK:
				OnOK();
				return TRUE;

			case IDCANCEL:
				OnCancel();
				return TRUE;
		}
	}
	return CWndProc::WindowProc(nMsg, wParam, lParam);
}

/**
 * ���̃��\�b�h�� WM_INITDIALOG �̃��b�Z�[�W�ɉ������ČĂяo����܂�
 * @retval TRUE �ŏ��̃R���g���[���ɓ��̓t�H�[�J�X��ݒ�
 * @retval FALSE ���ɐݒ��
 */
BOOL CDlgProc::OnInitDialog()
{
	return TRUE;
}

/**
 * ���[�U�[�� OK �̃{�^�� (IDOK ID ���̃{�^��) ���N���b�N����ƌĂяo����܂�
 */
void CDlgProc::OnOK()
{
	EndDialog(IDOK);
}

/**
 * �t���[�����[�N�́A���[�U�[�� [�L�����Z��] ���N���b�N���邩�A���[�_���܂��̓��[�h���X �_�C�A���O �{�b�N�X�� Esc �L�[���������Ƃ��ɂ��̃��\�b�h���Ăяo���܂�
 */
void CDlgProc::OnCancel()
{
	EndDialog(IDCANCEL);
}

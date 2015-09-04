/**
 * @file	PropProc.cpp
 * @brief	�v���p�e�B �V�[�g �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "PropProc.h"

/**
 * �R���X�g���N�^
 * @param[in] nIDTemplate ���̃y�[�W�Ɏg�p����e���v���[�g�� ID
 * @param[in] nIDCaption ���̃y�[�W�̃^�u�ɐݒ肳��閼�O�� ID
 */
CPropPageProc::CPropPageProc(UINT nIDTemplate, UINT nIDCaption)
{
	Construct(MAKEINTRESOURCE(nIDTemplate), nIDCaption);
}

/**
 * �R���X�g���N�^
 * @param[in] lpszTemplateName ���̃y�[�W�̃e���v���[�g�̖��O���܂ޕ�����ւ̃|�C���^�[
 * @param[in] nIDCaption ���̃y�[�W�̃^�u�ɐݒ肳��閼�O�� ID
 */
CPropPageProc::CPropPageProc(LPCTSTR lpszTemplateName, UINT nIDCaption)
{
	Construct(lpszTemplateName, nIDCaption);
}

/**
 * �f�X�g���N�^
 */
CPropPageProc::~CPropPageProc()
{
}

/**
 * �R���X�g���N�g
 * @param[in] nIDTemplate ���̃y�[�W�Ɏg�p����e���v���[�g�� ID
 * @param[in] nIDCaption ���̃y�[�W�̃^�u�ɐݒ肳��閼�O�� ID
 */
void CPropPageProc::Construct(UINT nIDTemplate, UINT nIDCaption)
{
	Construct(MAKEINTRESOURCE(nIDTemplate), nIDCaption);
}

/**
 * �R���X�g���N�g
 * @param[in] lpszTemplateName ���̃y�[�W�̃e���v���[�g�̖��O���܂ޕ�����ւ̃|�C���^�[
 * @param[in] nIDCaption ���̃y�[�W�̃^�u�ɐݒ肳��閼�O�� ID
 */
void CPropPageProc::Construct(LPCTSTR lpszTemplateName, UINT nIDCaption)
{
	ZeroMemory(&m_psp, sizeof(m_psp));
	m_psp.dwSize = sizeof(m_psp);
	m_psp.dwFlags = PSP_USECALLBACK;
	m_psp.hInstance = GetInstanceHandle();
	m_psp.pszTemplate = lpszTemplateName;
	m_psp.pfnDlgProc = DlgProc;
	m_psp.lParam = reinterpret_cast<LPARAM>(this);
	m_psp.pfnCallback = PropPageCallback;
}

/**
 * �v���p�e�B �y�[�W �v���V�[�W��
 * @param[in] hWnd �E�B���h�E �n���h��
 * @param[in] message ���b�Z�[�W
 * @param[in] pPropPage ���̃v���p�e�B �V�[�g �y�[�W�̃|�C���^
 * @return 0
 */
UINT CALLBACK CPropPageProc::PropPageCallback(HWND hWnd, UINT message, LPPROPSHEETPAGE pPropPage)
{
	switch (message)
	{
		case PSPCB_CREATE:
			HookWindowCreate(reinterpret_cast<CPropPageProc*>(pPropPage->lParam));
			return TRUE;

		case PSPCB_RELEASE:
			UnhookWindowCreate();
			break;
	}
	return 0;
}

/**
 * �t���[�����[�N�́A�C�x���g���R���g���[���ɔ�������ꍇ��A�R���g���[�����ꕔ�̎�ނ̏���v������R���g���[����e�E�B���h�E�ɒʒm���邽�߂ɁA���̃����o�[�֐����Ăяo���܂�
 * @param[in] wParam ���b�Z�[�W���R���g���[�����炻�̃��b�Z�[�W�𑗐M����R���g���[�������ʂ��܂�
 * @param[in] lParam �ʒm�R�[�h�ƒǉ������܂ޒʒm���b�Z�[�W (NMHDR) �̍\���̂ւ̃|�C���^�[
 * @param[out] pResult ���b�Z�[�W���������ꂽ�Ƃ����ʂ��i�[����R�[�h���� LRESULT �̕ϐ��ւ̃|�C���^�[
 * @retval TRUE ���b�Z�[�W����������
 * @retval FALSE ���b�Z�[�W���������Ȃ�����
 */
BOOL CPropPageProc::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* pNMHDR = reinterpret_cast<NMHDR*>(lParam);

	// allow message map to override
	if (CDlgProc::OnNotify(wParam, lParam, pResult))
	{
		return TRUE;
	}

	// don't handle messages not from the page/sheet itself
	if (pNMHDR->hwndFrom != m_hWnd && pNMHDR->hwndFrom != ::GetParent(m_hWnd))
	{
		return FALSE;
	}

	// handle default
	switch (pNMHDR->code)
	{
		case PSN_APPLY:
			*pResult = OnApply() ? PSNRET_NOERROR : PSNRET_INVALID_NOCHANGEPAGE;
			break;

		case PSN_RESET:
			OnReset();
			break;

		default:
			return FALSE;   // not handled
	}

	return TRUE;    // handled
}

/**
 * ���̃����o�[�֐��́A�t���[�����[�N�ɂ���� OnKillActive�t���[�����[�N�����Ăяo��������Ƀ��[�U�[��[OK]��I�����邩�A�X�V���ɌĂяo����܂�
 * @retval TRUE �ύX�����F���ꂽ
 * @retval FALSE �ύX�����F����Ȃ�����
 */
BOOL CPropPageProc::OnApply()
{
	OnOK();
	return TRUE;
}

/**
 * ���̃����o�[�֐��́A�t���[�����[�N�ɂ���ă��[�U�[��[�L�����Z��]��I������Ƃ��ɌĂяo����܂��B
 */
void CPropPageProc::OnReset()
{
	OnCancel();
}

/**
 * ���̃����o�[�֐��́A�t���[�����[�N�ɂ���� OnKillActive�t���[�����[�N�����Ăяo��������Ƀ��[�U�[��[OK]��I�����邩�A�X�V���ɌĂяo����܂�
 */
void CPropPageProc::OnOK()
{
}

/**
 * ���̃����o�[�֐��́A�t���[�����[�N��[�L�����Z��]�{�^�����I�����ꂽ�Ƃ��ɌĂяo����܂�
 */
void CPropPageProc::OnCancel()
{
}

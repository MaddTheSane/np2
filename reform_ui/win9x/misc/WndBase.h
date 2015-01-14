/**
 * @file	WndBase.h
 * @brief	�E�B���h�E���N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief �E�B���h�E���N���X
 */
class CWndBase
{
public:
	HWND m_hWnd;            /*!< must be first data member */

	CWndBase(HWND hWnd = NULL);
	CWndBase& operator=(HWND hWnd);
	void Attach(HWND hWnd);
	HWND Detach();
//	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu);
//	BOOL DestroyWindow();

	// Attributes
	operator HWND() const;

	// Message Functions

	// Window Text Functions
	BOOL SetWindowText(LPCTSTR lpString);

	// Window Size and Position Functions
	BOOL GetWindowRect(LPRECT lpRect) const;

	// Update and Painting Functions
	BOOL UpdateWindow();
	BOOL Invalidate(BOOL bErase = TRUE);
	BOOL InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE);
	BOOL ShowWindow(int nCmdShow);

	// Timer Functions
//	UINT_PTR SetTimer(UINT_PTR nIDEvent, UINT nElapse, void (CALLBACK* lpfnTimer)(HWND, UINT, UINT_PTR, DWORD) = NULL);
//	BOOL KillTimer(UINT_PTR nIDEvent);
};

/**
 * �R���X�g���N�^
 * @param[in] hWnd �E�B���h�E �n���h��
 */
inline CWndBase::CWndBase(HWND hWnd)
	: m_hWnd(hWnd)
{
}

/**
 * �I�y���[�^
 * @param[in] hWnd �E�B���h�E �n���h��
 * @return �C���X�^���X
 */
inline CWndBase& CWndBase::operator=(HWND hWnd)
{
	m_hWnd = hWnd;
	return *this;
}

/**
 * �A�^�b�`
 * @param[in] hWnd �E�B���h�E �n���h��
 */
inline void CWndBase::Attach(HWND hWnd)
{
	m_hWnd = hWnd;
}

/**
 * �f�^�b�`
 * @return �ȑO�̃C���X�^���X
 */
inline HWND CWndBase::Detach()
{
	HWND hWnd = m_hWnd;
	m_hWnd = NULL;
	return hWnd;
}

/**
 * HWND �I�y���[�^
 * @return HWND
 */
inline CWndBase::operator HWND() const
{
	return m_hWnd;
}

/**
 * �w�肳�ꂽ�E�B���h�E�̃^�C�g���o�[�̃e�L�X�g��ύX���܂�
 * @param[in] lpString �V�����E�B���h�E�^�C�g���܂��̓R���g���[���̃e�L�X�g�Ƃ��Ďg����ANULL �ŏI��镶����ւ̃|�C���^���w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::SetWindowText(LPCTSTR lpString)
{
	return ::SetWindowText(m_hWnd, lpString);
}

/**
 * �w�肳�ꂽ�E�B���h�E�̍���[�ƉE���[�̍��W���X�N���[�����W�Ŏ擾���܂�
 * @param[out] lpRect �\���̂ւ̃|�C���^���w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::GetWindowRect(LPRECT lpRect) const
{
	return ::GetWindowRect(m_hWnd, lpRect);
}

/**
 * �w�肳�ꂽ�E�B���h�E�̍X�V���[�W��������ł͂Ȃ��ꍇ�A�E�B���h�E�� ���b�Z�[�W�𑗐M���A���̃E�B���h�E�̃N���C�A���g�̈���X�V���܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::UpdateWindow()
{
	return ::UpdateWindow(m_hWnd);
}

/**
 * �w�肳�ꂽ�E�B���h�E�̂��ׂĂ��X�V���[�W�����ɂ��܂�
 * @param[in] bErase �X�V���[�W��������������Ƃ��ɁA�X�V���[�W�������̔w�i���������邩�ǂ������w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::Invalidate(BOOL bErase)
{
	return ::InvalidateRect(m_hWnd, NULL, bErase);
}

/**
 * �w�肳�ꂽ�E�B���h�E�̍X�V���[�W������1�̒����`��ǉ����܂�
 * @param[in] lpRect �X�V���[�W�����֒ǉ������������`�̃N���C�A���g���W��ێ�����1�̍\���̂ւ̃|�C���^���w�肵�܂�
 * @param[in] bErase �X�V���[�W��������������Ƃ��ɁA�X�V���[�W�������̔w�i���������邩�ǂ������w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::InvalidateRect(LPCRECT lpRect, BOOL bErase)
{
	return ::InvalidateRect(m_hWnd, lpRect, bErase);
}

/**
 * �w�肳�ꂽ�E�B���h�E�̕\����Ԃ�ݒ肵�܂�
 * @param[in] nCmdShow �E�B���h�E�̕\�����@���w�肵�܂�
 * @retval TRUE ����
 * @retval FALSE ���s
 */
inline BOOL CWndBase::ShowWindow(int nCmdShow)
{
	return ::ShowWindow(m_hWnd, nCmdShow);
}

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

public:
	static void Initialize(HINSTANCE hInstance);

	CWndBase();
	virtual ~CWndBase();
	operator HWND() const;
	HWND GetSafeHwnd() const;
	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu);
	BOOL DestroyWindow();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	static HINSTANCE sm_hInstance;		//!< �C���X�^���X �n���h��
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

/**
 * HWND �I�y���[�^
 * @return HWND
 */
inline CWndBase::operator HWND() const
{
	return m_hWnd;
}

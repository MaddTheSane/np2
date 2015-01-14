/**
 * @file	WndProc.h
 * @brief	�v���V�[�W�� �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "WndBase.h"

/**
 * @brief �v���V�[�W�� �N���X
 */
class CWndProc : public CWndBase
{
public:
	static void Initialize(HINSTANCE hInstance);
	static void SetResourceHandle(HINSTANCE hInstance);
	static HINSTANCE GetResourceHandle();

	CWndProc();
	virtual ~CWndProc();
	operator HWND() const;
	HWND GetSafeHwnd() const;
	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, HMENU nIDorHMenu);
	BOOL DestroyWindow();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	static HINSTANCE sm_hInstance;		//!< �C���X�^���X �n���h��
	static HINSTANCE sm_hResource;		//!< ���\�[�X �n���h��
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

/**
 * ���\�[�X �n���h����ݒ�
 * @param[in] hInstance ���\�[�X �n���h��
 */
inline void CWndProc::SetResourceHandle(HINSTANCE hInstance)
{
	sm_hResource = hInstance;
}

/**
 * ���\�[�X �n���h�����擾
 * @return ���\�[�X �n���h��
 */
inline HINSTANCE CWndProc::GetResourceHandle()
{
	return sm_hResource;
}

/**
 * HWND �I�y���[�^
 * @return HWND
 */
inline CWndProc::operator HWND() const
{
	return m_hWnd;
}

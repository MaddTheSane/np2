/**
 * @file	DlgProc.h
 * @brief	�_�C�A���O �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "WndProc.h"

/**
 * @brief �_�C�A���O �N���X
 */
class CDlgProc : public CWndProc
{
public:
	CDlgProc();
	CDlgProc(UINT nIDTemplate, HWND hwndParent = NULL);
	virtual ~CDlgProc();
	virtual INT_PTR DoModal();
	virtual BOOL OnInitDialog();
	void EndDialog(int nResult);

protected:
	LPCTSTR m_lpszTemplateName;		//!< �e���v���[�g��
	HWND m_hwndParent;				//!< �e�E�B���h�E

	virtual LRESULT WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();

	static BOOL CALLBACK DlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

/**
 * ���[�_�� �_�C�A���O �{�b�N�X���I������
 * @param[in] nResult DoModal�̌Ăяo�����ɕԂ��l
 */
inline void CDlgProc::EndDialog(int nResult)
{
	::EndDialog(m_hWnd, nResult);
}

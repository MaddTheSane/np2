/**
 * @file	PropProc.h
 * @brief	�v���p�e�B �V�[�g �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "DlgProc.h"

/**
 * @brief �v���p�e�B �V�[�g �y�[�W
 */
class CPropPageProc : public CDlgProc
{
public:
	PROPSHEETPAGE m_psp;			//!< �v���p�e�B �V�[�g �y�[�W�\����

public:
	CPropPageProc(UINT nIDTemplate, UINT nIDCaption = 0);
	CPropPageProc(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);
	virtual ~CPropPageProc();
	void Construct(UINT nIDTemplate, UINT nIDCaption = 0);
	void Construct(LPCTSTR lpszTemplateName, UINT nIDCaption = 0);

protected:
	BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnApply();
	virtual void OnReset();
	virtual void OnOK();
	virtual void OnCancel();

private:
	static UINT CALLBACK PropPageCallback(HWND hWnd, UINT message, LPPROPSHEETPAGE pPropPage);
};

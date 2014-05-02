/**
 *	@file	tawnd.h
 *	@brief	Tool-assisted �E�B���h�E �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include "misc\wndbase.h"

/**
 * @brief Tool-assisted �E�B���h�E �N���X
 */
class CTAWnd : public CWndBase
{
public:
	static CTAWnd* GetInstance();

	CTAWnd();
	bool Create();
	bool execute();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	static CTAWnd sm_instance;		//!< �B��̃C���X�^���X�ł�

	UINT m_nFrames;					//!< �t���[��
	UINT m_nExecFrames;				//!< ���s�t���[��
};

/**
 * �C���X�^���X��Ԃ��܂�
 * @return �C���X�^���X
 */
inline CTAWnd* CTAWnd::GetInstance()
{
	return &sm_instance;
}

/**
 * @file	wndloc.h
 * @brief	Window ���P�[�^ �N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

/**
 * @brief Window ���P�[�^ �N���X
 */
class CWndLoc
{
public:
	void Start();
	void Moving(LPRECT lpRect);

private:
	UINT m_nFlag;	//!< �ڑ��t���O
	POINT m_pt;		//!< �ʒu
	int m_dx;		//!< �f���^X
	int m_dy;		//!< �f���^Y
};

/**
 * @file	tstring.h
 * @brief	������N���X�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

#include <string>

namespace std
{
#ifdef _UNICODE
typedef wstring			tstring;				//!< tchar string �^��`
#else	// _UNICODE
typedef string			tstring;				//!< tchar string�^��`
#endif	// _UNICODE
}

/**
 * @file	dialogs.h
 * @brief	�_�C�A���O �w���p�[�̐錾����уC���^�[�t�F�C�X�̒�`�����܂�
 */

#pragma once

struct tagFileSelectParam
{
	LPTSTR	lpszTitle;
	LPTSTR	lpszDefExt;
	LPTSTR	lpszFilter;
	int		nFilterIndex;
};
typedef struct tagFileSelectParam		FSPARAM;
typedef struct tagFileSelectParam		*PFSPARAM;
typedef const struct tagFileSelectParam	*PCFSPARAM;

BOOL dlgs_openfile(HWND hWnd, PCFSPARAM pcParam, LPTSTR pszPath, UINT uSize, int *puRO);

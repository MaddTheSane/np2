/**
 * @file	c_combodata.cpp
 * @brief	�R���{ �f�[�^ �N���X�̓���̒�`���s���܂�
 */

#include "compiler.h"
#include "c_combodata.h"

#if 0
#include "strres.h"
#include "bmpdata.h"
#include "dosio.h"
#include "misc\tstring.h"
#include "commng.h"
#include "dialogs.h"
#include "np2.h"
#if defined(MT32SOUND_DLL)
#include "..\ext\mt32snd.h"
#endif
#endif

/**
 * �ǉ�
 * @param[in] lpValues �l�̔z��
 * @param[in] cchValues �l�̐�
 */
void CComboData::Add(const UINT32* lpValues, UINT cchValues)
{
	for (UINT i = 0; i < cchValues; i++)
	{
		TCHAR szStr[16];
		wsprintf(szStr, TEXT("%u"), lpValues[i]);
		Add(szStr, lpValues[i]);
	}
}

/**
 * �ǉ�
 * @param[in] lpEntries �G���g���̔z��
 * @param[in] cchEntries �G���g���̐�
 */
void CComboData::Add(const Entry* lpEntries, UINT cchEntries)
{
	for (UINT i = 0; i < cchEntries; i++)
	{
		std::tstring rString(LoadTString(lpEntries[i].lpcszString));
		Add(rString.c_str(), lpEntries[i].nItemData);
	}
}

/**
 * �ǉ�
 * @param[in] lpString �\����
 * @param[in] nItemData �f�[�^
 */
void CComboData::Add(LPCTSTR lpString, UINT32 nItemData)
{
	const int nIndex = AddString(lpString);
	if (nIndex >= 0)
	{
		SetItemData(nIndex, static_cast<DWORD_PTR>(nItemData));
	}
}

/**
 * �J�[�\���ݒ�
 * @param[in] nValue �l
 */
void CComboData::SetCurItemData(UINT32 nValue)
{
	const int nItems = GetCount();
	for (int i = 0; i < nItems; i++)
	{
		if (GetItemData(i) == nValue)
		{
			SetCurSel(i);
			break;
		}
	}
}

/**
 * �J�[�\���̒l���擾
 * @param[in] nDefault �f�t�H���g�l
 * @return �l
 */
UINT32 CComboData::GetCurItemData(UINT32 nDefault) const
{
	const int nIndex = GetCurSel();
	if (nIndex >= 0)
	{
		return static_cast<UINT32>(GetItemData(nIndex));
	}
	return nDefault;
}

/**
 * @file	c_combodata.cpp
 * @brief	コンボ データ クラスの動作の定義を行います
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
 * 追加
 * @param[in] lpValues 値の配列
 * @param[in] cchValues 値の数
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
 * 追加
 * @param[in] lpEntries エントリの配列
 * @param[in] cchEntries エントリの数
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
 * 追加
 * @param[in] lpString 表示名
 * @param[in] nItemData データ
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
 * カーソル設定
 * @param[in] nValue 値
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
 * カーソルの値を取得
 * @param[in] nDefault デフォルト値
 * @return 値
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

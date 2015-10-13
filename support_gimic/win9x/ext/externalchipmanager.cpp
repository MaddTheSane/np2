/**
 * @file	externalchipmanager.cpp
 * @brief	外部チップ管理クラスの動作の定義を行います
 */

#include "compiler.h"
#include "externalchipmanager.h"
#include <algorithm>
#include "np2.h"
#include "externalopna.h"
// #include "romeo\juliet.h"

/*! 唯一のインスタンスです */
CExternalChipManager CExternalChipManager::sm_instance;

/**
 * コンストラクタ
 */
CExternalChipManager::CExternalChipManager()
{
}

/**
 * 初期化
 */
void CExternalChipManager::Initialize()
{
}

/**
 * 解放
 */
void CExternalChipManager::Deinitialize()
{
	std::vector<IExternalChip*>::iterator it = m_chips.begin();
	while (it != m_chips.end())
	{
		IExternalChip* pChip = *it;
		it = m_chips.erase(it);

		pChip->Reset();
		delete pChip;
	}

	m_gimic.Deinitialize();
	m_rebirth.Deinitialize();
}

/**
 * チップ確保
 * @param[in] nType チップ タイプ
 * @param[in] nClock チップ クロック
 * @return インスタンス
 */
IExternalChip* CExternalChipManager::GetInterface(IExternalChip::ChipType nType, UINT nClock)
{
	IExternalChip* pChip = GetInterfaceInner(nType, nClock);
	if (pChip == NULL)
	{
		if (nType == IExternalChip::kYMF288)
		{
			pChip = GetInterface(IExternalChip::kYM2608, nClock);
		}
		else if (nType == IExternalChip::kYM3438)
		{
			pChip = GetInterface(IExternalChip::kYMF288, nClock);
		}
	}
	return pChip;
}

/**
 * チップ確保 (Inner)
 * @param[in] nType チップ タイプ
 * @param[in] nClock チップ クロック
 * @return インスタンス
 */
IExternalChip* CExternalChipManager::GetInterfaceInner(IExternalChip::ChipType nType, UINT nClock)
{
	IExternalChip* pChip = NULL;

	if (pChip == NULL)
	{
		pChip = m_rebirth.GetInterface(nType, nClock);
	}
	if (pChip == NULL)
	{
		pChip = m_gimic.GetInterface(nType, nClock);
	}

	// ラッピング
	if (pChip)
	{
		switch (nType)
		{
			case IExternalChip::kYM2608:
			case IExternalChip::kYM3438:
			case IExternalChip::kYMF288:
				pChip = new CExternalOpna(pChip);
				break;
		}
	}
	if (pChip)
	{
		m_chips.push_back(pChip);
	}
	return pChip;
}

/**
 * チップ解放
 * @param[in] pChip チップ
 */
void CExternalChipManager::Release(IExternalChip* pChip)
{
	std::vector<IExternalChip*>::iterator it = std::find(m_chips.begin(), m_chips.end(), pChip);
	if (it != m_chips.end())
	{
		m_chips.erase(it);
		(*it)->Reset();
		delete *it;
	}
}

/**
 * 音源リセット
 */
void CExternalChipManager::Reset()
{
	for (std::vector<IExternalChip*>::iterator it = m_chips.begin(); it != m_chips.end(); ++it)
	{
		(*it)->Reset();
	}
}

/**
 * ミュート
 * @param[in] bMute ミュート
 */
void CExternalChipManager::Mute(bool bMute)
{
	for (std::vector<IExternalChip*>::iterator it = m_chips.begin(); it != m_chips.end(); ++it)
	{
		(*it)->Message(IExternalChip::kMute, static_cast<INTPTR>(bMute));
	}
}

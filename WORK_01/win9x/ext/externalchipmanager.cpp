/**
 * @file	externalchipmanager.cpp
 * @brief	外部チップ管理クラスの動作の定義を行います
 */

#include "compiler.h"
#include "externalchipmanager.h"
#include <algorithm>
#include "np2.h"
#include "externalopna.h"

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

	m_c86ctl.Deinitialize();
	m_juliet.Deinitialize();
	m_scci.Deinitialize();
}

/**
 * チップ確保
 * @param[in] nChipType チップ タイプ
 * @param[in] nClock チップ クロック
 * @return インスタンス
 */
IExternalChip* CExternalChipManager::GetInterface(IExternalChip::ChipType nChipType, UINT nClock)
{
	IExternalChip* pChip = GetInterfaceInner(nChipType, nClock);
	if (pChip == NULL)
	{
		if (nChipType == IExternalChip::kYMF288)
		{
			pChip = GetInterface(IExternalChip::kYM2608, nClock);
		}
		else if (nChipType == IExternalChip::kYM3438)
		{
			pChip = GetInterface(IExternalChip::kYMF288, nClock);
		}
	}
	return pChip;
}

/**
 * チップ確保 (Inner)
 * @param[in] nChipType チップ タイプ
 * @param[in] nClock チップ クロック
 * @return インスタンス
 */
IExternalChip* CExternalChipManager::GetInterfaceInner(IExternalChip::ChipType nChipType, UINT nClock)
{
	IExternalChip* pChip = NULL;

	if (np2oscfg.useromeo)
	{
		if (pChip == NULL)
		{
			pChip = m_juliet.GetInterface(nChipType, nClock);
		}
	}
	if (pChip == NULL)
	{
		pChip = m_scci.GetInterface(nChipType, nClock);
	}
	if (pChip == NULL)
	{
		pChip = m_c86ctl.GetInterface(nChipType, nClock);
	}

	// ラッピング
	if (pChip)
	{
		switch (nChipType)
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
		pChip->Reset();
		delete pChip;
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

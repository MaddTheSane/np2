/**
 * @file	externalopna.cpp
 * @brief	外部 OPNA 演奏クラスの動作の定義を行います
 */

#include "compiler.h"
#include "externalchipmanager.h"
#include "gimic/gimic.h"
#include "spfm/spfmlight.h"
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
 * デストラクタ
 */
CExternalChipManager::~CExternalChipManager()
{
	Deinitialize();
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
	m_spfm.Deinitialize();
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

	// G.I.M.I.C / C86BOX
	if (pChip == NULL)
	{
		pChip = m_gimic.GetInterface(nChipType, nClock);
	}

	// SPFM Light
	if (pChip == NULL)
	{
		pChip = m_spfm.GetInterface(nChipType, nClock);
	}

	if (pChip)
	{
		pChip = new CExternalOpna(pChip);
	}

	return pChip;
}

/**
 * チップ解放
 * @param[in] pChip チップ
 */
void CExternalChipManager::Release(IExternalChip* pChip)
{
	if (pChip != NULL)
	{
		delete pChip;
	}
}

/**
 * 音源リセット
 */
void CExternalChipManager::Reset()
{
	m_spfm.Reset();
}

/**
 * ミュート
 * @param[in] bMute ミュート
 */
void CExternalChipManager::Mute(bool bMute)
{
	m_spfm.Message(IExternalChip::kMute, static_cast<INTPTR>(bMute));
}

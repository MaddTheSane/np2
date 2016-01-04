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
	: m_pGimic(NULL)
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
	Release(m_pGimic);
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
	// G.I.M.I.C / C86BOX
	if (m_pGimic == NULL)
	{
		CGimic* pModule = new CGimic;
		if (pModule->Initialize(nChipType, nClock))
		{
			m_pGimic = new CExternalOpna(pModule);
			return m_pGimic;
		}
		else
		{
			delete pModule;
		}
	}

	// SPFM Light
	IExternalChip* pSpfm = m_spfm.GetInterface(nChipType, nClock);
	if (pSpfm != NULL)
	{
		return new CExternalOpna(pSpfm);
	}
	return NULL;
}

/**
 * チップ解放
 * @param[in] pChip チップ
 */
void CExternalChipManager::Release(IExternalChip* pChip)
{
	if (pChip == NULL)
	{
		return;
	}
	if (m_pGimic == pChip)
	{
		m_pGimic = NULL;
		pChip->Reset();
		delete pChip;
	}
	else
	{
		delete pChip;
	}
}

/**
 * 音源リセット
 */
void CExternalChipManager::Reset()
{
	if (m_pGimic)
	{
		m_pGimic->Reset();
	}
	m_spfm.Reset();
}

/**
 * ミュート
 * @param[in] bMute ミュート
 */
void CExternalChipManager::Mute(bool bMute)
{
	if (m_pGimic)
	{
		m_pGimic->Message(IExternalChip::kMute, static_cast<INTPTR>(bMute));
	}
	m_spfm.Message(IExternalChip::kMute, static_cast<INTPTR>(bMute));
}

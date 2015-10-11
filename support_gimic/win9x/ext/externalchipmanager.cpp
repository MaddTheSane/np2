/**
 * @file	externalchipmanager.c
 * @brief	外部チップ管理クラスの動作の定義を行います
 */

#include "compiler.h"
#include "externalchipmanager.h"
#include <algorithm>
#include "np2.h"
#include "externalopna.h"
// #include "c86ctl\c86ctlif.h"
#include "rebirth\rebirth.h"
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
#if 0
	IExternalChip* pModule;

	// ROMEO
	if (np2oscfg.useromeo)
	{
		pModule = new CJuliet;
		if (pModule->Initialize())
		{
			m_module = pModule;
//			m_bHasADPCM = pModule->HasADPCM();
			return;
		}
		delete pModule;
	}

	// G.I.M.I.C / C86BOX
	pModule = new C86CtlIf;
	if (pModule->Initialize())
	{
		m_module = pModule;
		m_bHasADPCM = pModule->HasADPCM();
		return;
	}
	delete pModule;

	// RE:birth
	pModule = new CRebirth;
	if (pModule->Initialize())
	{
		m_module = pModule;
		m_bHasADPCM = pModule->HasADPCM();
		return;
	}
	delete pModule;
#endif
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

	m_rebirth.Deinitialize();
}

/**
 * チップ確保
 * @return リソース
 */
IExternalChip* CExternalChipManager::GetInterface(IExternalChip::ChipType nType, UINT nClock)
{
	IExternalChip* pChip = NULL;

	if (pChip == NULL)
	{
		pChip = m_rebirth.GetInterface(nType, nClock);
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

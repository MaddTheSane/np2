/**
 * @file	rebirth.cpp
 * @brief	SCCI アクセス クラスの動作の定義を行います
 */

#include "compiler.h"
#include "rebirth.h"
#include "scci.h"
#include "SCCIDefines.h"
#include <algorithm>

/**
 * コンストラクタ
 */
CRebirth::CRebirth()
	: m_hModule(NULL)
	, m_pManager(NULL)
{
}

/**
 * デストラクタ
 */
CRebirth::~CRebirth()
{
	Deinitialize();
}

/**
 * 初期化
 * @retval true 成功
 * @retval false 失敗
 */
bool CRebirth::Initialize()
{
	if (m_hModule)
	{
		return false;
	}

	do
	{
		m_hModule = ::LoadLibrary(TEXT("SCCI.DLL"));
		if (m_hModule == NULL)
		{
			break;
		}

		// サウンドインターフェースマネージャー取得用関数アドレス取得
		SCCIFUNC fnGetSoundInterfaceManager = reinterpret_cast<SCCIFUNC>(::GetProcAddress(m_hModule, "getSoundInterfaceManager"));
		if (fnGetSoundInterfaceManager == NULL)
		{
			break;
		}

		// サウンドインターフェースマネージャー取得
		m_pManager = (*fnGetSoundInterfaceManager)();
		if (m_pManager == NULL)
		{
			break;
		}

		// サウンドインターフェースマネージャーインスタンス初期化
		// 必ず最初に実行してください
		if (!m_pManager->initializeInstance())
		{
			break;
		}

		// リセットを行う
		Reset();
		return true;
	} while (false /*CONSTCOND*/);

	Deinitialize();
	return false;
}

/**
 * 解放
 */
void CRebirth::Deinitialize()
{
	if (m_pManager)
	{
		while (!m_chips.empty())
		{
			delete m_chips.front();
		}
		m_chips.clear();

		// 一括開放する場合（チップ一括開放の場合）
		m_pManager->releaseAllSoundChip();

		// サウンドインターフェースマネージャーインスタンス開放
		// FreeLibraryを行う前に必ず呼び出ししてください
		m_pManager->releaseInstance();

		m_pManager = NULL;
	}

	if (m_hModule)
	{
		::FreeLibrary(m_hModule);
		m_hModule = NULL;
	}
}

/**
 * 音源リセット
 */
void CRebirth::Reset()
{
	if (m_pManager)
	{
		// リセットを行う
		m_pManager->reset();
	}
}

/**
 * インターフェイス取得
 */
IExternalChip* CRebirth::GetInterface(IExternalChip::ChipType nType, UINT nClock)
{
	const bool bInitialized = Initialize();

	do
	{
		if (m_pManager == NULL)
		{
			break;
		}

		SC_CHIP_TYPE iSoundChipType = SC_TYPE_NONE;
		switch (nType)
		{
			case IExternalChip::kYM2608:
				iSoundChipType = SC_TYPE_YM2608;
				break;

			case IExternalChip::kYM3438:
				iSoundChipType = SC_TYPE_YM2612;
				break;

			case IExternalChip::kYMF288:
				iSoundChipType = SC_TYPE_YMF288;
				break;
		}

		SoundChip* pSoundChip = m_pManager->getSoundChip(iSoundChipType, nClock);
		if (pSoundChip != NULL)
		{
			// サウンドチップ取得できた
			Chip* pChip = new Chip(this, pSoundChip);
			m_chips.push_back(pChip);
			return pChip;
		}
	} while (false /*CONSTCOND*/);

	if (bInitialized)
	{
		Deinitialize();
	}
	return NULL;
}

/**
 * 解放
 * @param[in] pChip チップ
 */
void CRebirth::Detach(CRebirth::Chip* pChip)
{
	std::vector<Chip*>::iterator it = std::find(m_chips.begin(), m_chips.end(), pChip);
	if (it != m_chips.end())
	{
		m_chips.erase(it);
	}

	// チップの開放（チップ単位で開放の場合）
	if (m_pManager)
	{
		m_pManager->releaseSoundChip(*pChip);
	}
}

// ---- チップ

/**
 * コンストラクタ
 */
CRebirth::Chip::Chip(CRebirth* pRebirth, SoundChip* pChip)
	: m_pRebirth(pRebirth)
	, m_pChip(pChip)
{
}

/**
 * デストラクタ
 */
CRebirth::Chip::~Chip()
{
	m_pRebirth->Detach(this);
}

/**
 * オペレータ
 */
CRebirth::Chip::operator SoundChip*()
{
	return m_pChip;
}

/**
 * Get chip type
 * @return The type of the chip
 */
IExternalChip::ChipType CRebirth::Chip::GetChipType()
{
	switch (m_pChip->getSoundChipType())
	{
		case SC_TYPE_YM2608:
			return IExternalChip::kYM2608;

		case SC_TYPE_YM2612:
			return IExternalChip::kYM3438;

		case SC_TYPE_YMF288:
			return IExternalChip::kYMF288;
	}
	return IExternalChip::kNone;
}

/**
 * リセット
 */
void CRebirth::Chip::Reset()
{
}

/**
 * レジスタ書き込み
 * @param[in] nAddr アドレス
 * @param[in] cData データ
 */
void CRebirth::Chip::WriteRegister(UINT nAddr, UINT8 cData)
{
	m_pChip->setRegister(nAddr, cData);
}

/**
 * メッセージ
 */
INTPTR CRebirth::Chip::Message(UINT nMessage, INTPTR nParameter)
{
	return 0;
}

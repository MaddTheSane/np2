/**
 * @file	rebirth.h
 * @brief	SCCI アクセス クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include <vector>
#include "..\externalchip.h"

class SoundInterfaceManager;
class SoundChip;

/**
 * @brief SCCI アクセス クラス
 */
class CRebirth
{
public:
	CRebirth();
	~CRebirth();
	bool Initialize();
	void Deinitialize();
	void Reset();
	IExternalChip* GetInterface(IExternalChip::ChipType nType, UINT nClock);

private:
	HMODULE m_hModule;					//!< モジュール	
	SoundInterfaceManager* m_pManager;	//!< マネージャ

	/**
	 * @brief チップ クラス
	 */
	class Chip : public IExternalChip
	{
		public:
			Chip(CRebirth* pRebirth, SoundChip* pChip);
			virtual ~Chip();
			operator SoundChip*();
			virtual ChipType GetChipType();
			virtual void Reset();
			virtual void WriteRegister(UINT nAddr, UINT8 cData);
			virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

		private:
			CRebirth* m_pRebirth;		//!< 親インスタンス
			SoundChip* m_pChip;			//!< チップ インスタンス
	};

	std::vector<Chip*> m_chips;			//!< チップ
	void Detach(Chip* pChip);
	friend class Chip;
};

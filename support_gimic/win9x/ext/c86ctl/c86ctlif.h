/**
 * @file	c86ctlif.h
 * @brief	G.I.M.I.C アクセス クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include <map>
#include "..\externalchip.h"

namespace c86ctl
{
	interface IRealChipBase;
	interface IGimic;
	interface IRealChip;
}

/**
 * @brief G.I.M.I.C アクセス クラス
 */
class C86CtlIf
{
public:
	C86CtlIf();
	~C86CtlIf();
	bool Initialize();
	void Deinitialize();
	void Reset();
	IExternalChip* GetInterface(IExternalChip::ChipType nType, UINT nClock);

private:
	HMODULE m_hModule;					//!< モジュール ハンドル
	c86ctl::IRealChipBase* m_pChipBase;	//!< チップ ベース インスタンス

	/**
	 * @brief チップ クラス
	 */
	class Chip : public IExternalChip
	{
		public:
			Chip(C86CtlIf* pC86Ctl, c86ctl::IRealChip* pRealChip, c86ctl::IGimic* pGimic, ChipType nType, UINT nClock);
			virtual ~Chip();
			virtual ChipType GetChipType();
			virtual void Reset();
			virtual void WriteRegister(UINT nAddr, UINT8 cData);
			virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

		private:
			C86CtlIf* m_pC86Ctl;
			c86ctl::IRealChip* m_pRealChip;
			c86ctl::IGimic* m_pGimic;			//!< G.I.M.I.C インタフェイス
			ChipType m_nType;					//!< 
			UINT m_nClock;						//!< 
	};

	std::map<int, Chip*> m_chips;			//!< チップ
	void Detach(Chip* pChip);
	friend class Chip;
};

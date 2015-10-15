/**
 * @file	c86ctlif.h
 * @brief	G.I.M.I.C アクセス クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

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
class C86CtlIf : public IExternalChip
{
public:
	C86CtlIf();
	virtual ~C86CtlIf();
	virtual bool Initialize();
	virtual void Deinitialize();
	virtual bool IsEnabled();
	virtual bool IsBusy();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);
	virtual bool HasADPCM();

private:
	HMODULE m_hModule;					//!< モジュール ハンドル
	c86ctl::IRealChipBase* m_chipbase;	//!< チップ ベース インスタンス
	c86ctl::IGimic* m_gimic;			//!< G.I.M.I.C インタフェイス
	c86ctl::IRealChip* m_chip;			//!< チップ インタフェイス
	bool m_bHasADPCM;					//!< ADPCM フラグ
};

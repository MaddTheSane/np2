/**
 * @file	gimic.h
 * @brief	G.I.M.I.C アクセス クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "..\extendmodule.h"

class IC86RealChip;

/**
 * @brief G.I.M.I.C アクセス クラス
 */
class CGimic : public IExtendModule
{
public:
	CGimic();
	virtual ~CGimic();
	virtual bool Initialize();
	virtual void Deinitialize();
	virtual bool IsEnabled();
	virtual bool IsBusy();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);

private:
	IC86RealChip* m_device;		/*!< USB デバイス */
};

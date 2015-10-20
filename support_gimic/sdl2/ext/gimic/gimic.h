/**
 * @file	gimic.h
 * @brief	Interface of accessing G.I.M.I.C
 */

#pragma once

#include "../externalchip.h"

namespace c86ctl
{
	class IC86RealChip;
}

/**
 * @brief The class of accessing G.I.M.I.C
 */
class CGimic : public IExternalChip
{
public:
	CGimic();
	virtual ~CGimic();
	bool Initialize(IExternalChip::ChipType nChipType, UINT nClock);
	void Deinitialize();
	virtual ChipType GetChipType();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);
	virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

private:
	c86ctl::IC86RealChip* m_pChip;		/*!< The instance of the chip */
	UINT m_nClock;						/*!< The clock */
	static IExternalChip::ChipType GetChipTypeInner(c86ctl::IC86RealChip* pDevice);
};

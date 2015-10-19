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
	bool Initialize();
	void Deinitialize();
	virtual ChipType GetChipType();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);
	virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

private:
	c86ctl::IC86RealChip* m_device;		/*!< The instance of the chip */
};

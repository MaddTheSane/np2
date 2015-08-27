/**
 * @file	gimic.h
 * @brief	Interface of accessing G.I.M.I.C
 */

#pragma once

#include "../externalchip.h"

class IC86RealChip;

/**
 * @brief The class of accessing G.I.M.I.C
 */
class CGimic : public IExternalChip
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
	IC86RealChip* m_device;		/*!< The instance of the chip */
};

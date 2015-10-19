/**
 * @file	spfmlight.h
 * @brief	Interface of accessing SPFM Light
 */

#pragma once

#include "../externalchip.h"
#include "misc/tty.h"

/**
 * @brief The class of SPFM
 */
class CSpfmLight : public IExternalChip
{
public:
	CSpfmLight();
	virtual ~CSpfmLight();
	virtual bool Initialize();
	virtual void Deinitialize();
	virtual bool IsEnabled();
	virtual bool IsBusy();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);

private:
	CTty m_serial;		/*!< Serial */
};

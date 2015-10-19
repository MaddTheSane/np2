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
	bool Initialize();
	void Deinitialize();
	virtual ChipType GetChipType();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);
	virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

private:
	CTty m_serial;		/*!< Serial */
};

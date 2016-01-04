/**
 * @file	spfminterface.h
 * @brief	Interface of interface
 */

#pragma once

#include "../externalchip.h"

class CSpfmLight;

/**
 * @brief The class of interface
 */
class CSpfmInterface : public IExternalChip
{
public:
	CSpfmInterface(CSpfmLight* pDevice, UINT nSlot, ChipType nChipType, UINT nClock);
	virtual ~CSpfmInterface();
	void Deinitialize();
	virtual ChipType GetChipType();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);
	virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

private:
	CSpfmLight* m_pDevice;		/*!< Device */
	UINT m_nSlot;				/*!< Slot */
	ChipType m_nChipType;		/*!< ChipType */
};

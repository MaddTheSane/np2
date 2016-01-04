/**
 * @file	spfminterface.h
 * @brief	Interface of interface
 */

#pragma once

#include "../externalchip.h"
#include "misc/tty.h"

class CSpfmManager;

/**
 * @brief The class of interface
 */
class CSpfmInterface : public IExternalChip
{
public:
	static CSpfmInterface* CreateInstance(CSpfmManager* pManager, const OEMCHAR* lpDeviceName, ChipType nChipType);

	CSpfmInterface(CSpfmManager* pManager, ChipType nChipType);
	virtual ~CSpfmInterface();
	void Deinitialize();
	virtual ChipType GetChipType();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);
	virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

private:
	CSpfmManager* m_pManager;		/*!< Manager */
	ChipType m_nChipType;			/*!< ChipType */
	CTty m_serial;					/*!< Serial */
	bool Open(const OEMCHAR* lpDeviceName);
};

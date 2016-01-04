/**
 * @file	spfmlight.h
 * @brief	Interface of accessing SPFM Light
 */

#pragma once

#include <map>
#include "../externalchip.h"
#include "misc/tty.h"

class CSpfmInterface;
class CSpfmManager;

/**
 * @brief The class of SPFM
 */
class CSpfmLight
{
public:
	static CSpfmLight* CreateInstance(CSpfmManager* pManager, const OEMCHAR* lpDeviceName);

	IExternalChip* Attach(UINT nSlot, IExternalChip::ChipType nChipType, UINT nClock);
	void Detach(UINT nSlot);
	bool IsAttached(UINT nSlot) const;
	void Reset();
	void WriteRegister(UINT nSlot, UINT nAddr, UINT8 cData);
	INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

private:
	CSpfmManager* m_pManager;						/*!< Manager */
	CTty m_serial;									/*!< Serial */
	bool m_bReseted;								/*!< Reset flag */
	std::map<UINT, CSpfmInterface*> m_interfaces;	/*!< interfaces */

	CSpfmLight(CSpfmManager* pManager);
	~CSpfmLight();
	bool OpenTty(const OEMCHAR* lpDeviceName);
};

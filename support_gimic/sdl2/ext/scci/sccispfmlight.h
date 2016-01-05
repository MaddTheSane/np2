/**
 * @file	sccispfmlight.h
 * @brief	Interface of accessing SPFM Light
 */

#pragma once

#include "sccisoundchip.h"
#include "sccisoundinterface.h"
#include "misc/tty.h"

namespace scci
{

/**
 * @brief The class of SPFM
 */
class CSpfmLight : public CSoundInterface
{
public:
	static CSpfmLight* CreateInstance(CSoundInterfaceManager* pManager, const OEMCHAR* lpDeviceName);
	virtual bool setData(const unsigned char* pData, size_t dSendDataLen);
	virtual size_t getData(unsigned char* pData, size_t dGetDataLen);
	virtual bool reset();

	virtual SoundChip* Attach(UINT nSlot, SC_CHIP_TYPE iSoundChipType, UINT dClock);

protected:
	bool m_bReseted;								/*!< Reset flag */
	CTty m_serial;									/*!< Serial */

	/**
	 * @brief The class of Chip
	 */
	class Chip : public CSoundChip
	{
	public:
		Chip(CSoundInterface* pInterface, UINT nSlot, SC_CHIP_TYPE iSoundChipType, UINT dClock);
		virtual bool setRegister(UINT dAddr, UINT dData);
	};

	CSpfmLight(CSoundInterfaceManager* pManager);
	virtual ~CSpfmLight();
	bool OpenTty(const OEMCHAR* lpDeviceName);

	friend class Chip;
};

}	// namespace scci

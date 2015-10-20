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
	bool Initialize(IExternalChip::ChipType nChipType, UINT nClock);
	void Deinitialize();
	virtual ChipType GetChipType();
	virtual void Reset();
	virtual void WriteRegister(UINT nAddr, UINT8 cData);
	virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

private:
	/**
	 * Sound chip list
	 */
	enum SC_CHIP_TYPE
	{
		SC_TYPE_NONE	= 0,
		SC_TYPE_YM2608,
		SC_TYPE_YM2151,
		SC_TYPE_YM2610,
		SC_TYPE_YM2203,
		SC_TYPE_YM2612,
		SC_TYPE_AY8910,
		SC_TYPE_SN76489,
		SC_TYPE_YM3812,
		SC_TYPE_YMF262,
		SC_TYPE_YM2413,
		SC_TYPE_YM3526,
		SC_TYPE_YMF288,
		SC_TYPE_SCC,
		SC_TYPE_SCCS,
		SC_TYPE_Y8950,
		SC_TYPE_MAX
	};

	CTty m_serial;					/*!< Serial */
	SC_CHIP_TYPE m_nScChipType;		/*!< ChipType */
	bool Open(const OEMCHAR* lpDeviceName);
};

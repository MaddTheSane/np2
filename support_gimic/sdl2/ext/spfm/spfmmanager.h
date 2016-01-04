/**
 * @file	spfmmanager.h
 * @brief	Interface of manager
 */

#pragma once

#include <vector>
#include "../externalchip.h"

class CSpfmInterface;
class CSpfmLight;

/**
 * @brief The class of manager
 */
class CSpfmManager
{
public:
	CSpfmManager();
	~CSpfmManager();
	void Deinitialize();
	IExternalChip* GetInterface(IExternalChip::ChipType nChipType, UINT nClock);
	void Detach(CSpfmInterface* pInstance);
	void Reset();
	INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

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
	std::vector<CSpfmInterface*> m_interfaces;			/*!< interfaces */
};

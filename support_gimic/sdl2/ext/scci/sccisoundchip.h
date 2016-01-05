/**
 * @file	sccisoundchip.h
 * @brief	Interface of sound chip
 */

#pragma once

#include "scci.h"

namespace scci
{

class CSoundInterface;

/**
 * @brief The class of chip
 */
class CSoundChip : public SoundChip
{
public:
	CSoundChip(CSoundInterface* pInterface, UINT nSlot, SC_CHIP_TYPE iSoundChipType, UINT dClock);
	virtual ~CSoundChip();
	virtual SC_CHIP_TYPE getSoundChipType();

protected:
	CSoundInterface* m_pInterface;	/*!< Interface */
	UINT m_nSlot;					/*!< Slot */
	SC_CHIP_TYPE m_iSoundChipType;	/*!< Chiptype */
	UINT m_dClock;					/*!< Clock */
};

}	// namespace scci

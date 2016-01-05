/**
 * @file	sccisoundchip.cpp
 * @brief	Implementation of sound chip
 */

#include "compiler.h"
#include "sccisoundchip.h"
#include "sccisoundinterface.h"

namespace scci
{

/**
 * Constructor
 * @param[in] pInterface The instance of the sound interface
 * @param[in] nSlot The number of slot
 * @param[in] iSoundChipType The type of the chip
 * @param[in] dClock The clock of the chip
 */
CSoundChip::CSoundChip(CSoundInterface* pInterface, UINT nSlot, SC_CHIP_TYPE iSoundChipType, UINT dClock)
	: m_pInterface(pInterface)
	, m_nSlot(nSlot)
	, m_iSoundChipType(iSoundChipType)
	, m_dClock(dClock)
{
}

/**
 * Destructor
 */
CSoundChip::~CSoundChip()
{
	m_pInterface->Detach(m_nSlot);
}

/**
 * Gets sound chip type
 * @return The type of the chip
 */
SC_CHIP_TYPE CSoundChip::getSoundChipType()
{
	return m_iSoundChipType;
}

}	// namespace scci

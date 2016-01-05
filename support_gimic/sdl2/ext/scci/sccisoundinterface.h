/**
 * @file	sccisoundinterface.h
 * @brief	Interface of sound interfaces
 */

#pragma once

#include <map>
#include "scci.h"

namespace scci
{

class CSoundChip;
class CSoundInterfaceManager;

/**
 * @brief The class of interface
 */
class CSoundInterface : public SoundInterface
{
public:
	CSoundInterface(CSoundInterfaceManager* pManager);
	virtual ~CSoundInterface();

	/**
	 * Attach
	 * @param[in] nSlot The number of the slot
	 * @param[in] iSoundChipType The type of the chip
	 * @param[in] dClock The clock of the chip
	 * @return The instance of the chip
	 */
	virtual SoundChip* Attach(UINT nSlot, SC_CHIP_TYPE iSoundChipType, UINT dClock) = 0;

	bool IsAttached(UINT nSlot) const;

protected:
	CSoundInterfaceManager* m_pManager;				/*!< Manager */
	std::map<UINT, CSoundChip*> m_chips;			/*!< interfaces */
	void Detach(UINT nSlot);

	friend class CSoundChip;
};

}	// namespace scci

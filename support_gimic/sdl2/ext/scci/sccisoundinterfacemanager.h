/**
 * @file	sccisoundinterfacemanager.h
 * @brief	Interface of the SCCI manager
 */

#pragma once

#include <map>
#include <string>
#include "scci.h"

namespace scci
{

class CSoundInterface;

/**
 * @brief The class of the sound interface manager
 */
class CSoundInterfaceManager : public SoundInterfaceManager
{
private:
	virtual bool releaseInterface(SoundInterface* pSoundInterface);
	virtual bool releaseAllInterface();
	virtual SoundChip* getSoundChip(SC_CHIP_TYPE iSoundChipType, UINT dClock);
	virtual bool releaseSoundChip(SoundChip* pSoundChip);
	virtual bool releaseAllSoundChip();
	virtual bool reset();

private:
	static CSoundInterfaceManager sm_instance;					/*!< Singleton */
	std::map<std::string, CSoundInterface*> m_interfaces;		/*!< interfaces */

	CSoundInterfaceManager();
	~CSoundInterfaceManager();
	void Detach(CSoundInterface* pInterface);

	friend class CSoundInterface;
};

}	// namespace scci

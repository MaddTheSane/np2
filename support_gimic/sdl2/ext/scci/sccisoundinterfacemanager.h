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
public:
	static CSoundInterfaceManager* GetInstance();

	virtual bool releaseInterface(SoundInterface* pSoundInterface);
	virtual bool releaseAllInterface();
	virtual SoundChip* getSoundChip(SC_CHIP_TYPE iSoundChipType, UINT dClock);
	virtual bool releaseSoundChip(SoundChip* pSoundChip);
	virtual bool releaseAllSoundChip();
	virtual bool reset();
	virtual bool initializeInstance();
	virtual bool releaseInstance();

private:
	static CSoundInterfaceManager sm_instance;					/*!< Singleton */
	std::map<std::string, CSoundInterface*> m_interfaces;		/*!< interfaces */

	CSoundInterfaceManager();
	~CSoundInterfaceManager();
	void Detach(CSoundInterface* pInterface);

	friend class CSoundInterface;
};

/**
 * Gets instance
 * @return The instance of sound manager
 */
inline CSoundInterfaceManager* CSoundInterfaceManager::GetInstance()
{
	return &sm_instance;
}

}	// namespace scci

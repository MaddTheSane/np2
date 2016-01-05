/**
 * @file	sccisoundinterfacemanager.cpp
 * @brief	Implementation of manager
 */

#include "compiler.h"
#include "sccisoundinterfacemanager.h"
#include "sccisoundinterface.h"
#include "sccispfmlight.h"
#include "dosio.h"
#include "common/profile.h"

namespace scci
{

/**
 * Constructor
 */
CSoundInterfaceManager::CSoundInterfaceManager()
{
}

/**
 * Destructor
 */
CSoundInterfaceManager::~CSoundInterfaceManager()
{
}

/**
 * Detach
 * @param[in] pInterface The instance of the sound interface
 */
void CSoundInterfaceManager::Detach(CSoundInterface* pInterface)
{
	for (std::map<std::string, CSoundInterface*>::iterator it = m_interfaces.begin(); it != m_interfaces.end(); ++it)
	{
		if (it->second == pInterface)
		{
			m_interfaces.erase(it);
			break;
		}
	}
}

/**
 * Releases the sound interface
 * @param[in] pSoundInterface The instance of the sound interface
 * @retval true If succeeded
 * @retval false If failed
 */
bool CSoundInterfaceManager::releaseInterface(SoundInterface* pSoundInterface)
{
	for (std::map<std::string, CSoundInterface*>::iterator it = m_interfaces.begin(); it != m_interfaces.end(); ++it)
	{
		if (it->second == pSoundInterface)
		{
			delete static_cast<CSoundInterface*>(pSoundInterface);
			return true;
		}
	}
	return false;
}

/**
 * Releases all interfaces
 * @retval true If succeeded
 * @retval false If failed
 */
bool CSoundInterfaceManager::releaseAllInterface()
{
	while (!m_interfaces.empty())
	{
		if (!releaseInterface(m_interfaces.begin()->second))
		{
			return false;
		}
	}
	return true;
}

/**
 * Gets sound chip instance
 * @param[in] iSoundChipType The type of the chip
 * @param[in] dClock The clock of the chip
 * @return The interface
 */
SoundChip* CSoundInterfaceManager::getSoundChip(SC_CHIP_TYPE iSoundChipType, UINT dClock)
{
	if (iSoundChipType == SC_TYPE_NONE)
	{
		return NULL;
	}

	SoundChip* pChip = NULL;

	OEMCHAR szPath[MAX_PATH];
	milstr_ncpy(szPath, file_getcd(OEMTEXT("SCCI.ini")), NELEMENTS(szPath));
	PFILEH pfh = profile_open(szPath, PFILEH_READONLY);

	OEMCHAR szSections[4096];
	if (profile_getsectionnames(szSections, NELEMENTS(szSections), pfh))
	{
		OEMCHAR* lpSections = szSections;
		while ((pChip == NULL) && (*lpSections != '\0'))
		{
			OEMCHAR* lpKeyName = lpSections;
			const size_t cchKeyName = OEMSTRLEN(lpSections);
			lpSections += cchKeyName + 1;

			if (milstr_memcmp(lpKeyName, OEMTEXT("SPFM Light")) != 0)
			{
				continue;
			}
			if ((lpKeyName[10] != '(') || (lpKeyName[cchKeyName - 1] != ')'))
			{
				continue;
			}

			if (profile_readint(lpKeyName, OEMTEXT("ACTIVE"), 0, pfh) == 0)
			{
				continue;
			}

			std::string deviceName(lpKeyName + 11, lpKeyName + cchKeyName - 1);

			CSoundInterface* pInterface = NULL;
			std::map<std::string, CSoundInterface*>::iterator it = m_interfaces.find(deviceName);
			if (it != m_interfaces.end())
			{
				pInterface = it->second;
			}

			for (UINT i = 0; i < 4; i++)
			{
				if ((pInterface) && (pInterface->IsAttached(i)))
				{
					continue;
				}

				OEMCHAR szAppName[32];
				OEMSPRINTF(szAppName, OEMTEXT("SLOT_%02d_CHIP_ID"), i);
				if (profile_readint(lpKeyName, szAppName, 0, pfh) != iSoundChipType)
				{
					continue;
				}
				if (pInterface == NULL)
				{
					pInterface = CSpfmLight::CreateInstance(this, deviceName.c_str());
					if (pInterface == NULL)
					{
						break;
					}
					m_interfaces[deviceName] = pInterface;
				}

				pChip = pInterface->Attach(i, iSoundChipType, dClock);
				if (pChip)
				{
					break;
				}
			}
		}
	}
	profile_close(pfh);

	return pChip;
}

/**
 * Releases the instance of the sound chip
 * @param[in] pSoundChip The instance of the sound chip
 * @retval true If succeeded
 * @retval false If failed
 */
bool CSoundInterfaceManager::releaseSoundChip(SoundChip* pSoundChip)
{
	delete static_cast<CSoundChip*>(pSoundChip);
	return true;
}

/**
 * Releases all instances of the sound chip
 * @retval true If succeeded
 * @retval false If failed
 */
bool CSoundInterfaceManager::releaseAllSoundChip()
{
	return releaseAllInterface();
}

/**
 * Resets all interfaces
 * @retval true If succeeded
 * @retval false If failed
 */
bool CSoundInterfaceManager::reset()
{
	bool err = false;
	for (std::map<std::string, CSoundInterface*>::iterator it = m_interfaces.begin(); it != m_interfaces.end(); ++it)
	{
		if (!it->second->reset())
		{
			err = true;
		}
	}
	return !err;
}

}	// namespace scci

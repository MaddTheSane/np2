/**
 * @file	spfmmanager.cpp
 * @brief	Implementation of manager
 */

#include "compiler.h"
#include "spfmmanager.h"
#include "spfmlight.h"
#include "dosio.h"
#include "common/profile.h"

/**
 * Constructor
 */
CSpfmManager::CSpfmManager()
{
}

/**
 * Destructor
 */
CSpfmManager::~CSpfmManager()
{
	Deinitialize();
}

/**
 * Deinitialize
 */
void CSpfmManager::Deinitialize()
{
}

/**
 * GetInterface
 * @param[in] nChipType The type og chip
 * @param[in] nClock The clock
 * @return interface
 */
IExternalChip* CSpfmManager::GetInterface(IExternalChip::ChipType nChipType, UINT nClock)
{
	SC_CHIP_TYPE nScChipType = SC_TYPE_NONE;
	switch (nChipType)
	{
		case IExternalChip::kYM2608:
			nScChipType = SC_TYPE_YM2608;
			break;

		case IExternalChip::kYM3438:
			nScChipType = SC_TYPE_YM2612;
			break;

		case IExternalChip::kYMF288:
			nScChipType = SC_TYPE_YMF288;
			break;

		default:
			break;
	}
	if (nScChipType == SC_TYPE_NONE)
	{
		return NULL;
	}

	IExternalChip* pChip = NULL;

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

			CSpfmLight* pDevice = NULL;
			std::map<std::string, CSpfmLight*>::iterator it = m_devices.find(deviceName);
			if (it != m_devices.end())
			{
				pDevice = it->second;
			}

			for (UINT i = 0; i < 4; i++)
			{
				if ((pDevice) && (pDevice->IsAttached(i)))
				{
					continue;
				}

				OEMCHAR szAppName[32];
				OEMSPRINTF(szAppName, OEMTEXT("SLOT_%02d_CHIP_ID"), i);
				if (profile_readint(lpKeyName, szAppName, 0, pfh) != nScChipType)
				{
					continue;
				}
				if (pDevice == NULL)
				{
					pDevice = CSpfmLight::CreateInstance(this, deviceName.c_str());
					if (pDevice == NULL)
					{
						break;
					}
					m_devices[deviceName] = pDevice;
				}

				pChip = pDevice->Attach(i, nChipType, nClock);
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
 * Detach
 */
void CSpfmManager::Detach(CSpfmLight* pInstance)
{
	for (std::map<std::string, CSpfmLight*>::iterator it = m_devices.begin(); it != m_devices.end(); ++it)
	{
		if (it->second == pInstance)
		{
			m_devices.erase(it);
			return;
		}
	}
}

/**
 * Reset
 */
void CSpfmManager::Reset()
{
	for (std::map<std::string, CSpfmLight*>::iterator it = m_devices.begin(); it != m_devices.end(); ++it)
	{
		it->second->Reset();
	}
}

/**
 * メッセージ
 * @param[in] nMessage メッセージ
 * @param[in] nParameter パラメータ
 * @return リザルト
 */
INTPTR CSpfmManager::Message(UINT nMessage, INTPTR nParameter)
{
	return 0;
}

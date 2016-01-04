/**
 * @file	spfmmanager.cpp
 * @brief	Implementation of manager
 */

#include "compiler.h"
#include "spfmmanager.h"
#include <algorithm>
#include "spfminterface.h"
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

	CSpfmInterface* ret = NULL;

	OEMCHAR szPath[MAX_PATH];
	milstr_ncpy(szPath, file_getcd(OEMTEXT("SCCI.ini")), NELEMENTS(szPath));
	PFILEH pfh = profile_open(szPath, PFILEH_READONLY);

	OEMCHAR szSections[4096];
	if (profile_getsectionnames(szSections, NELEMENTS(szSections), pfh))
	{
		OEMCHAR* lpSections = szSections;
		while (*lpSections != '\0')
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

			if (profile_readint(lpKeyName, OEMTEXT("SLOT_00_CHIP_ID"), 0, pfh) != nScChipType)
			{
				continue;
			}

			lpKeyName[cchKeyName - 1] = '\0';
			const OEMCHAR* lpDeviceName = lpKeyName + 11;

			ret = CSpfmInterface::CreateInstance(this, lpDeviceName, nChipType);
			if (ret)
			{
				m_interfaces.push_back(ret);
			}
		}
	}
	profile_close(pfh);

	return ret;
}

/**
 * Detach
 */
void CSpfmManager::Detach(CSpfmInterface* pInstance)
{
	std::vector<CSpfmInterface*>::iterator it = std::find(m_interfaces.begin(), m_interfaces.end(), pInstance);
	if (it == m_interfaces.end())
	{
		return;
	}
	m_interfaces.erase(it);
}

/**
 * Reset
 */
void CSpfmManager::Reset()
{
	for (std::vector<CSpfmInterface*>::iterator it = m_interfaces.begin(); it != m_interfaces.end(); ++it)
	{
		(*it)->Reset();
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

/**
 * @file	spfmlight.cpp
 * @brief	Implementation of accessing SPFM Light
 */

#include "compiler.h"
#include "spfmlight.h"
#include "dosio.h"
#include "common/profile.h"
#include "misc/threadbase.h"

/**
 * Constructor
 */
CSpfmLight::CSpfmLight()
{
}

/**
 * Destructor
 */
CSpfmLight::~CSpfmLight()
{
}

/**
 * Initialize
 * @retval true Succeeded
 * @retval false Failed
 */
bool CSpfmLight::Initialize()
{
	Deinitialize();

	bool bOpened = false;

	OEMCHAR szPath[MAX_PATH];
	milstr_ncpy(szPath, file_getcd(OEMTEXT("SCCI.ini")), NELEMENTS(szPath));
	PFILEH pfh = profile_open(szPath, PFILEH_READONLY);

	OEMCHAR szSections[4096];
	if (profile_getsectionnames(szSections, NELEMENTS(szSections), pfh))
	{
		OEMCHAR* lpKeyName = szSections;
		while ((!bOpened) && (*lpKeyName != '\0'))
		{
			const UINT cchKeyName = OEMSTRLEN(lpKeyName);
			if (milstr_memcmp(lpKeyName, OEMTEXT("SPFM Light")) == 0)
			{
				if ((lpKeyName[10] == '(') && (lpKeyName[cchKeyName - 1] == ')'))
				{
					lpKeyName[cchKeyName - 1] = '\0';
					bOpened = m_serial.Open(lpKeyName + 11, 1500000, OEMTEXT("8N1"));
				}
			}
			lpKeyName += cchKeyName + 1;
		}
	}
	profile_close(pfh);

	if (!bOpened)
	{
		return false;
	}

	const unsigned char query[1] = {0xff};
	m_serial.Write(query, sizeof(query));

	CThreadBase::Delay(100 * 1000);

	char buffer[4];
	if ((m_serial.Read(buffer, 2) != 2) || (buffer[0] != 'L') || (buffer[1] != 'T'))
	{
		m_serial.Close();
		return false;
	}
	return true;
}

/**
 * Deinitialize
 */
void CSpfmLight::Deinitialize()
{
	m_serial.Close();
}

/**
 * Is device enabled?
 * @retval true Enabled
 * @retval false Disabled
 */
bool CSpfmLight::IsEnabled()
{
	return m_serial.IsOpened();
}

/**
 * Is device busy?
 * @retval true Busy
 * @retval false Ready
 */
bool CSpfmLight::IsBusy()
{
	return false;
}

/**
 * Reset
 */
void CSpfmLight::Reset()
{
	const unsigned char reset[1] = {0xfe};
	m_serial.Write(reset, sizeof(reset));

	CThreadBase::Delay(10 * 1000);

	char buffer[4];
	m_serial.Read(buffer, 2);
}

/**
 * Writes register
 * @param[in] nAddr The address
 * @param[in] cData The data
 */
void CSpfmLight::WriteRegister(UINT nAddr, UINT8 cData)
{
	unsigned char cmd[4];
	cmd[0] = 0x00;
	cmd[1] = static_cast<unsigned char>((nAddr >> 7) & 2);
	cmd[2] = static_cast<unsigned char>(nAddr & 0xff);
	cmd[3] = cData;
	m_serial.Write(cmd, sizeof(cmd));
}

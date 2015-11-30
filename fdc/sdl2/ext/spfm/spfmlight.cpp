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
	: m_nScChipType(SC_TYPE_NONE)
{
}

/**
 * Destructor
 */
CSpfmLight::~CSpfmLight()
{
	Deinitialize();
}


/**
 * Initialize
 * @param[in] nChipType The type og chip
 * @param[in] nClock The clock
 * @retval true Succeeded
 * @retval false Failed
 */
bool CSpfmLight::Initialize(IExternalChip::ChipType nChipType, UINT nClock)
{
	Deinitialize();

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
		return false;
	}

	bool bOpened = false;

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
			bOpened = Open(lpKeyName + 11);
			if (bOpened)
			{
				m_nScChipType = nScChipType;
				break;
			}
		}
	}
	profile_close(pfh);

	return bOpened;
}

/**
 * Open serial
 */
bool CSpfmLight::Open(const OEMCHAR* lpDeviceName)
{
	if (!m_serial.Open(lpDeviceName, 1500000, OEMTEXT("8N1")))
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
	m_nScChipType = SC_TYPE_NONE;
}

/**
 * Get chip type
 * @return The type of the chip
 */
IExternalChip::ChipType CSpfmLight::GetChipType()
{
	switch (m_nScChipType)
	{
		case SC_TYPE_YM2608:
			return IExternalChip::kYM2608;

		case SC_TYPE_YM2612:
			return IExternalChip::kYM3438;

		case SC_TYPE_YMF288:
			return IExternalChip::kYMF288;

		default:
			break;
	}
	return IExternalChip::kNone;
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

/**
 * メッセージ
 * @param[in] nMessage メッセージ
 * @param[in] nParameter パラメータ
 * @return リザルト
 */
INTPTR CSpfmLight::Message(UINT nMessage, INTPTR nParameter)
{
	return 0;
}

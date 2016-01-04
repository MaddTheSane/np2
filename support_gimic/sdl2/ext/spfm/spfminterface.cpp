/**
 * @file	spfminterface.cpp
 * @brief	Implementation of interface
 */

#include "compiler.h"
#include "spfminterface.h"
#include "spfmmanager.h"
#include "misc/threadbase.h"

/**
 *
 */
CSpfmInterface* CSpfmInterface::CreateInstance(CSpfmManager* pManager, const OEMCHAR* lpDeviceName, ChipType nChipType)
{
	CSpfmInterface* pInstance = new CSpfmInterface(pManager, nChipType);
	if (!pInstance->Open(lpDeviceName))
	{
		delete pInstance;
		return NULL;
	}
	return pInstance;
}

/**
 * Constructor
 * @param[in] pMagager The instance of manager
 * @param[in] nChipType The type of chip
 */
CSpfmInterface::CSpfmInterface(CSpfmManager* pManager, IExternalChip::ChipType nChipType)
	: m_pManager(pManager)
	, m_nChipType(nChipType)
{
}

/**
 * Destructor
 */
CSpfmInterface::~CSpfmInterface()
{
	Deinitialize();
	if (m_pManager)
	{
		m_pManager->Detach(this);
	}
}

/**
 * Open serial
 */
bool CSpfmInterface::Open(const OEMCHAR* lpDeviceName)
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
void CSpfmInterface::Deinitialize()
{
	m_serial.Close();
}

/**
 * Get chip type
 * @return The type of the chip
 */
IExternalChip::ChipType CSpfmInterface::GetChipType()
{
	return m_nChipType;
}

/**
 * Reset
 */
void CSpfmInterface::Reset()
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
void CSpfmInterface::WriteRegister(UINT nAddr, UINT8 cData)
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
INTPTR CSpfmInterface::Message(UINT nMessage, INTPTR nParameter)
{
	return 0;
}

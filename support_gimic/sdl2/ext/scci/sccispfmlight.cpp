/**
 * @file	sccispfmlight.cpp
 * @brief	Implementation of accessing SPFM Light
 */

#include "compiler.h"
#include "sccispfmlight.h"
#include "sccisoundchip.h"
#include "misc/threadbase.h"

namespace scci
{

/**
 * Creates instance
 * @param[in] pManager The instance of the manager
 * @param[in] lpDeviceName The name of the device
 * @return The instance
 */
CSpfmLight* CSpfmLight::CreateInstance(CSoundInterfaceManager* pManager, const OEMCHAR* lpDeviceName)
{
	CSpfmLight* pInterface = new CSpfmLight(pManager);
	if (!pInterface->OpenTty(lpDeviceName))
	{
		delete pInterface;
		pInterface = NULL;
	}
	return pInterface;
}

/**
 * Constructor
 * @param[in] pManager The instance of the manager
 */
CSpfmLight::CSpfmLight(CSoundInterfaceManager* pManager)
	: CSoundInterface(pManager)
	, m_bReseted(false)
{
}

/**
 * Open serial
 * @param[in] lpDeviceName The name of the device
 * @retval true If succeeded
 * @retval false If failed
 */
bool CSpfmLight::OpenTty(const OEMCHAR* lpDeviceName)
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

	m_bReseted = false;
	return true;
}

/**
 * Destructor
 */
CSpfmLight::~CSpfmLight()
{
	m_serial.Close();
}

/**
 * Attach
 * @param[in] nSlot The number of the slot
 * @param[in] iSoundChipType The type of the chip
 * @param[in] dClock The clock of the chip
 * @return The instance of the chip
 */
SoundChip* CSpfmLight::Attach(UINT nSlot, SC_CHIP_TYPE iSoundChipType, UINT dClock)
{
	if (IsAttached(nSlot))
	{
		return NULL;
	}

	CSoundChip* pChip = new Chip(this, nSlot, iSoundChipType, dClock);
	if (pChip)
	{
		m_chips[nSlot] = pChip;
	}
	return pChip;
}

/**
 * Sends data to the interface
 * @param[in] pData The buffer of data
 * @param[in] dSendDataLen The length of data
 * @retval true If succeeded
 * @retval false If failed
 */
bool CSpfmLight::setData(const unsigned char* pData, size_t dSendDataLen)
{
	m_bReseted = false;
	return (m_serial.Write(pData, dSendDataLen) == dSendDataLen);
}

/**
 * Gets data from the interface
 * @param[out] pData The buffer of data
 * @param[in] dGetDataLen The length of data
 * @return The size of read
 */
size_t CSpfmLight::getData(unsigned char* pData, size_t dGetDataLen)
{
	return m_serial.Read(pData, dGetDataLen);
}

/**
 * Resets the interface
 * @retval true If succeeded
 * @retval false If failed
 */
bool CSpfmLight::reset()
{
	if (m_bReseted)
	{
		return true;
	}
	m_bReseted = true;

	const unsigned char reset[1] = {0xfe};
	m_serial.Write(reset, sizeof(reset));

	CThreadBase::Delay(10 * 1000);

	unsigned char buffer[4];
	m_serial.Read(buffer, 2);
	return true;
}

/**
 * Constructor
 * @param[in] pInterface The instance of the sound interface
 * @param[in] nSlot The number of the slot
 * @param[in] iSoundChipType The type of the chip
 * @param[in] dClock The clock of the chip
 */
CSpfmLight::Chip::Chip(CSoundInterface* pInterface, UINT nSlot, SC_CHIP_TYPE iSoundChipType, UINT dClock)
	: CSoundChip(pInterface, nSlot, iSoundChipType, dClock)
{
}

/**
 * Sets Register data
 * Writes the register
 * @param[in] dAddr The address of register
 * @param[in] dData The data
 * @retval true If succeeded
 * @retval false If failed
 */
bool CSpfmLight::Chip::setRegister(UINT dAddr, UINT dData)
{
	unsigned char cmd[4];
	cmd[0] = static_cast<unsigned char>(m_nSlot & 0x0f);
	cmd[1] = static_cast<unsigned char>((dAddr >> 7) & 2);
	cmd[2] = static_cast<unsigned char>(dAddr & 0xff);
	cmd[3] = static_cast<unsigned char>(dData);
	return m_pInterface->setData(cmd, sizeof(cmd));
}

}	// namespace scci

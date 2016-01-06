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
 * Constructor
 * @param[in] pManager The instance of the manager
 * @param[in] deviceName The name of the device
 */
CSpfmLight::CSpfmLight(CSoundInterfaceManager* pManager, const std::string& deviceName)
	: CSoundInterface(pManager, deviceName)
	, m_bReseted(false)
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
 * @retval true If succeeded
 * @retval false If failed
 */
bool CSpfmLight::Initialize()
{
	CTty serial;
	if (!serial.Open(m_info.cInterfaceName, 1500000, OEMTEXT("8N1")))
	{
		return false;
	}

	const unsigned char query[1] = {0xff};
	serial.Write(query, sizeof(query));

	CThreadBase::Delay(100 * 1000);

	char buffer[4];
	if ((serial.Read(buffer, 2) != 2) || (buffer[0] != 'L') || (buffer[1] != 'T'))
	{
		return false;
	}

	m_bReseted = false;
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
 * Increments the reference count
 * @return The new reference count
 */
size_t CSpfmLight::AddRef()
{
	const size_t nRef = CSoundInterface::AddRef();
	if (nRef == 1)
	{
		m_serial.Open(m_info.cInterfaceName, 1500000, OEMTEXT("8N1"));
	}
	return nRef;
}

/**
 * Decrements the reference count
 * @return The new reference count
 */
size_t CSpfmLight::Release()
{
	const size_t nRef = CSoundInterface::Release();
	if (nRef == 0)
	{
		m_serial.Close();
	}
	return nRef;
}

/**
 * Add
 * @param[in] info The information
 */
void CSpfmLight::Add(const SCCI_SOUND_CHIP_INFO& info)
{
	std::map<UINT, CSoundChip*>::iterator it = m_chips.find(info.dBusID);
	if (it == m_chips.end())
	{
		m_chips[info.dBusID] = new Chip(this, info);
		m_info.iSoundChipCount++;
	}
}

/**
 * Is supported low level API
 * @retval true yes
 */
bool CSpfmLight::isSupportLowLevelApi()
{
	return true;
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
 * @param[in] info The information
 */
CSpfmLight::Chip::Chip(CSoundInterface* pInterface, const SCCI_SOUND_CHIP_INFO& info)
	: CSoundChip(pInterface, info)
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
	cmd[0] = static_cast<unsigned char>(m_info.dBusID & 0x0f);
	cmd[1] = static_cast<unsigned char>((dAddr >> 7) & 2);
	cmd[2] = static_cast<unsigned char>(dAddr & 0xff);
	cmd[3] = static_cast<unsigned char>(dData);
	return m_pInterface->setData(cmd, sizeof(cmd));
}

/**
 * Initializes sound chip(clear registers)
 * @retval true If succeeded
 * @retval false If failed
 */
bool CSpfmLight::Chip::init()
{
	return m_pInterface->reset();
}

}	// namespace scci

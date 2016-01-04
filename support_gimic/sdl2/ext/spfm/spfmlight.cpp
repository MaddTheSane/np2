/**
 * @file	spfmlight.cpp
 * @brief	Implementation of accessing SPFM Light
 */

#include "compiler.h"
#include "spfmlight.h"
#include "spfminterface.h"
#include "spfmmanager.h"
#include "misc/threadbase.h"

/**
 * Creates instance
 * @param[in] pManager The instance of the manager
 * @param[in] lpDeviceName The name of the device
 * @return The instance
 */
CSpfmLight* CSpfmLight::CreateInstance(CSpfmManager* pManager, const OEMCHAR* lpDeviceName)
{
	CSpfmLight* pDevice = new CSpfmLight(pManager);
	if (!pDevice->OpenTty(lpDeviceName))
	{
		delete pDevice;
		pDevice = NULL;
	}
	return pDevice;
}

/**
 * Constructor
 * @param[in] pManager The instance of the manager
 */
CSpfmLight::CSpfmLight(CSpfmManager* pManager)
	: m_pManager(pManager)
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
	m_pManager->Detach(this);
	m_serial.Close();
}

/**
 * Attach
 * @param[in] nSlot The number of the slot
 * @param[in] nChipType The type of the chip
 * @param[in] nClock The clock
 * @return The instance of the chip
 */
IExternalChip* CSpfmLight::Attach(UINT nSlot, IExternalChip::ChipType nChipType, UINT nClock)
{
	if (IsAttached(nSlot))
	{
		return NULL;
	}

	CSpfmInterface* pInterface = new CSpfmInterface(this, nSlot, nChipType, nClock);
	if (pInterface)
	{
		m_interfaces[nSlot] = pInterface;
	}
	return pInterface;
}

/**
 * Detach
 * @param[in] nSlot Then number of the slot
 */
void CSpfmLight::Detach(UINT nSlot)
{
	std::map<UINT, CSpfmInterface*>::iterator it = m_interfaces.find(nSlot);
	if (it != m_interfaces.end())
	{
		m_interfaces.erase(it);
	}

	if (m_interfaces.empty())
	{
		delete this;
	}
}

/**
 * Is attached?
 * @param[in] nSlot The number of the slot
 * @retval true Yes
 * @retval false No
 */
bool CSpfmLight::IsAttached(UINT nSlot) const
{
	return (m_interfaces.find(nSlot) != m_interfaces.end());
}

/**
 * Reset
 */
void CSpfmLight::Reset()
{
	if (m_bReseted)
	{
		return;
	}
	m_bReseted = true;

	const unsigned char reset[1] = {0xfe};
	m_serial.Write(reset, sizeof(reset));

	CThreadBase::Delay(10 * 1000);

	char buffer[4];
	m_serial.Read(buffer, 2);
}

/**
 * Writes register
 * @param[in] nSlot The number of slot
 * @param[in] nAddr The address
 * @param[in] cData The data
 */
void CSpfmLight::WriteRegister(UINT nSlot, UINT nAddr, UINT8 cData)
{
	m_bReseted = false;

	unsigned char cmd[4];
	cmd[0] = static_cast<unsigned char>(nSlot & 0x0f);
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

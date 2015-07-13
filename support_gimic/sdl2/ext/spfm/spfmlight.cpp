/**
 * @file	spfmlight.cpp
 * @brief	Implementation of accessing SPFM Light
 */

#include "compiler.h"
#include "spfmlight.h"
#include "misc/threadbase.h"

/**
 * Constractor
 */
CSpfmLight::CSpfmLight()
{
}

/**
 * Destractor
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

#ifdef _WIN32
	bool r = m_serial.Open(TEXT("USB Serial Port"), 1500000, TEXT("8N1"));
#else
	bool r = m_serial.Open("/dev/tty.usbserial-A601HD4I", 1500000, "8N1");
#endif

	if (!r)
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

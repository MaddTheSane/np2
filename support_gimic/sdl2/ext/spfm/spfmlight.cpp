/**
 * @file	spfmlight.cpp
 * @brief	Implementation of accessing SPFM Light
 */

#include "compiler.h"
#include "spfmlight.h"

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

	return m_serial.Open(5, 1500000, TEXT("8N1"));
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

	::Sleep(1);

	char buffer[4];
	m_serial.Read(buffer, 2);
	buffer[2] = '\0';
	printf("ret = %s (%x,%x)\n", buffer, buffer[0], buffer[1]);
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

/**
 * @file	spfminterface.cpp
 * @brief	Implementation of interface
 */

#include "compiler.h"
#include "spfminterface.h"
#include "spfmlight.h"

/**
 * Constructor
 * @param[in] pMagager The instance of manager
 * @param[in] nSlot The number of slot
 * @param[in] nChipType The type of chip
 */
CSpfmInterface::CSpfmInterface(CSpfmLight* pDevice, UINT nSlot, ChipType nChipType, UINT nClock)
	: m_pDevice(pDevice)
	, m_nSlot(nSlot)
	, m_nChipType(nChipType)
{
}

/**
 * Destructor
 */
CSpfmInterface::~CSpfmInterface()
{
	m_pDevice->Detach(m_nSlot);
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
	m_pDevice->Reset();
}

/**
 * Writes register
 * @param[in] nAddr The address
 * @param[in] cData The data
 */
void CSpfmInterface::WriteRegister(UINT nAddr, UINT8 cData)
{
	m_pDevice->WriteRegister(m_nSlot, nAddr, cData);
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

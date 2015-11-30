/**
 * @file	gimic.cpp
 * @brief	Implementation of accessing G.I.M.I.C
 */

#include "compiler.h"
#include "gimic.h"
#include "c86boxusb.h"
#include "gimicusb.h"

/**
 * Constructor
 */
CGimic::CGimic()
	: m_pChip(NULL)
	, m_nClock(7987200)
{
}

/**
 * Destructor
 */
CGimic::~CGimic()
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
bool CGimic::Initialize(IExternalChip::ChipType nChipType, UINT nClock)
{
	Deinitialize();

	CGimicUSB* pGimic = new CGimicUSB();
	if ((pGimic->Initialize() == C86CTL_ERR_NONE) && (GetChipTypeInner(pGimic) == nChipType))
	{
		m_pChip = pGimic;
		m_nClock = nClock;
		Reset();
		return true;
	}
	delete pGimic;

	C86BoxUSB* pC86Box = new C86BoxUSB();
	if ((pC86Box->Initialize() == C86CTL_ERR_NONE) && (GetChipTypeInner(pC86Box) == nChipType))
	{
		m_pChip = pC86Box;
		Reset();
		return true;
	}
	delete pC86Box;
	return false;
}

/**
 * Deinitialize
 */
void CGimic::Deinitialize()
{
	if (m_pChip)
	{
		m_pChip->Deinitialize();
		delete m_pChip;
		m_pChip = NULL;
	}
}

/**
 * Get chip type
 * @return The type of the chip
 */
IExternalChip::ChipType CGimic::GetChipType()
{
	return GetChipTypeInner(m_pChip);
}

/**
 * Get chip type
 * @param[in] pChip The instance of device
 * @return The type of the chip
 */
IExternalChip::ChipType CGimic::GetChipTypeInner(c86ctl::IC86RealChip* pChip)
{
	c86ctl::ChipType nType;
	if ((pChip) && (pChip->GetChipType(&nType) == C86CTL_ERR_NONE))
	{
		switch (nType)
		{
			case c86ctl::CHIP_OPNA:
				return IExternalChip::kYM2608;

			case c86ctl::CHIP_YM2608NOADPCM:
			case c86ctl::CHIP_OPN3L:
				return IExternalChip::kYMF288;

			case c86ctl::CHIP_YM2612:
			case c86ctl::CHIP_YM3438:
				return IExternalChip::kYM3438;

			default:
				break;
		}
	}
	return IExternalChip::kNone;
}

/**
 * Reset
 */
void CGimic::Reset()
{
	if (m_pChip)
	{
		m_pChip->Reset();

		// G.I.M.I.C の初期化
		CGimicUSB* gimicusb = dynamic_cast<CGimicUSB*>(m_pChip);
		if (gimicusb)
		{
			gimicusb->SetPLLClock(m_nClock);
			gimicusb->SetSSGVolume(31);
		}
	}
}

/**
 * Writes register
 * @param[in] nAddr The address
 * @param[in] cData The data
 */
void CGimic::WriteRegister(UINT nAddr, UINT8 cData)
{
	// printf("WriteReg %04x %02x\n", nAddr, cData);
	if (m_pChip)
	{
		m_pChip->Out(nAddr, cData);
	}
}

/**
 * メッセージ
 * @param[in] nMessage メッセージ
 * @param[in] nParameter パラメータ
 * @return リザルト
 */
INTPTR CGimic::Message(UINT nMessage, INTPTR nParameter)
{
	return 0;
}

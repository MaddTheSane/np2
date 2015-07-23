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
	: m_device(NULL)
{
}

/**
 * Destructor
 */
CGimic::~CGimic()
{
}

/**
 * Initialize
 * @retval true Succeeded
 * @retval false Failed
 */
bool CGimic::Initialize()
{
	Deinitialize();

	CGimicUSB* gimic = new CGimicUSB();
	if (gimic->Initialize() == C86CTL_ERR_NONE)
	{
		m_device = gimic;
		Reset();
		return true;
	}
	delete gimic;

	C86BoxUSB* c86box = new C86BoxUSB();
	if (c86box->Initialize() == C86CTL_ERR_NONE)
	{
		m_device = c86box;
		Reset();
		return true;
	}
	delete c86box;
	return false;
}

/**
 * Deinitialize
 */
void CGimic::Deinitialize()
{
	if (m_device)
	{
		m_device->Deinitialize();
		delete m_device;
		m_device = NULL;
	}
}

/**
 * Is device enabled?
 * @retval true Enabled
 * @retval false Disabled
 */
bool CGimic::IsEnabled()
{
	return (m_device != NULL);
}

/**
 * Is device busy?
 * @retval true Busy
 * @retval false Ready
 */
bool CGimic::IsBusy()
{
	return false;
}

/**
 * Reset
 */
void CGimic::Reset()
{
	if (m_device)
	{
		m_device->Reset();

		// G.I.M.I.C ‚Ì‰Šú‰»
		CGimicUSB* gimicusb = dynamic_cast<CGimicUSB*>(m_device);
		if (gimicusb)
		{
			gimicusb->SetPLLClock(7987200);
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
	if (m_device)
	{
		m_device->Out(nAddr, cData);
	}
}

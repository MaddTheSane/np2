/**
 * @file	sccisoundinterface.cpp
 * @brief	Implementation of sound interfaces
 */

#include "compiler.h"
#include "sccisoundinterface.h"
#include "sccisoundchip.h"
#include "sccisoundinterfacemanager.h"

namespace scci
{

/**
 * Constructor
 * @param[in] pManager The instance of the manager
 */
CSoundInterface::CSoundInterface(CSoundInterfaceManager* pManager)
	: m_pManager(pManager)
{
}

/**
 * Destructor
 */
CSoundInterface::~CSoundInterface()
{
	while (!m_chips.empty())
	{
		delete m_chips.begin()->second;
	}
	m_pManager->Detach(this);
}

/**
 * Detach
 * @param[in] nSlot Then number of the slot
 */
void CSoundInterface::Detach(UINT nSlot)
{
	std::map<UINT, CSoundChip*>::iterator it = m_chips.find(nSlot);
	if (it != m_chips.end())
	{
		m_chips.erase(it);
	}
}

/**
 * Is attached?
 * @param[in] nSlot The number of the slot
 * @retval true Yes
 * @retval false No
 */
bool CSoundInterface::IsAttached(UINT nSlot) const
{
	return (m_chips.find(nSlot) != m_chips.end());
}

}	// namespace scci

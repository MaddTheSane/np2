/**
 * @file	gimic.h
 * @brief	Interface of accessing G.I.M.I.C
 */

#pragma once

#include "../externalchip.h"

namespace c86ctl
{
	class IC86RealChip;
}

/**
 * @brief The class of accessing G.I.M.I.C
 */
class CGimic
{
public:
	CGimic();
	virtual ~CGimic();
	IExternalChip* GetInterface(IExternalChip::ChipType nChipType, UINT nClock);
	void Detach(IExternalChip* pChip);
	static IExternalChip::ChipType GetChipTypeInner(c86ctl::IC86RealChip* pDevice);

private:
	/**
	 * @brief The interface
	 */
	class CInterface : public IExternalChip
	{
	public:
		CInterface(CGimic* pManager, c86ctl::IC86RealChip* pChip, UINT nClock);
		virtual ~CInterface();
		virtual ChipType GetChipType();
		virtual void Reset();
		virtual void WriteRegister(UINT nAddr, UINT8 cData);
		virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0);

	private:
		CGimic* m_pManager;					/*!< The instance of the manager */
		c86ctl::IC86RealChip* m_pChip;		/*!< The instance of the chip */
		UINT m_nClock;						/*!< The clock */
	};

	IExternalChip* m_pChip;			/* The instance of the interface */
};

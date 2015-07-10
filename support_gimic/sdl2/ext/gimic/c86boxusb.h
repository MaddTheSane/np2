/**
 * @file	c86boxusb.h
 * @brief	Interface of accessing C86BOX USB
 */

#pragma once

#include "c86ctl.h"
#include "misc/guard.h"
#include "misc/threadbase.h"
#include "misc/usbdev.h"

/**
 * @brief The class of accessing C86BOX USB
 */
class C86BoxUSB : public IC86RealChip, protected CThreadBase
{
public:
	C86BoxUSB();
	virtual ~C86BoxUSB();

	// IRealChipBase
	virtual int Initialize();
	virtual int Deinitialize();

	// IRealChip
	virtual int Reset();
	virtual void Out(UINT nAddr, UINT8 cData);
	virtual UINT8 In(UINT nAddr);

	// IRealChip2
	virtual int GetChipStatus(UINT nAddr, UINT8* pcStatus);
	virtual void DirectOut(UINT nAddr, UINT8 cData);

	// IRealChip3
	virtual int GetChipType(ChipType* pnType);

protected:
	virtual bool Task();

private:
	CUsbDev m_usb;			/*!< USB */
	CGuard m_usbGuard;		/*!< The quard of accessing USB */
	CGuard m_queGuard;		/*!< The quard of que */
	ChipType m_nChipType;	/*!< The type of chip */
	int m_nDevId;			/*!< The type of devices */
	size_t m_nQueIndex;		/*!< The position in que */
	size_t m_nQueCount;		/*!< The count in que */
	UINT m_que[0x400];		/*!< que */
	UINT8 m_sReg[0x200];	/*!< register */

	int Transaction(const void* lpOutput, size_t cbOutput, void* lpInput = NULL, size_t cbInput = 0);
};

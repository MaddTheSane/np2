/**
 * @file	gimicusb.h
 * @brief	Interface of accessing G.I.M.I.C USB
 */

#pragma once

#include "c86ctl.h"
#include "misc/guard.h"
#include "misc/threadbase.h"
#include "misc/usbdev.h"

/**
 * @brief The class of accessing G.I.M.I.C USB
 */
class CGimicUSB : public IC86RealChip, protected CThreadBase
{
public:
	CGimicUSB();
	~CGimicUSB();
	virtual int Initialize();
	virtual int Deinitialize();

	// IGimic
	int SetSSGVolume(UINT8 cVolume);
	int GetSSGVolume(UINT8* pcVolume);
	int SetPLLClock(UINT nClock);
	int GetPLLClock(UINT* pnClock);
	int GetFWVer(UINT* pnMajor, UINT* pnMinor, UINT* pnRev, UINT* pnBuild);
	int GetMBInfo(Devinfo* pInfo);
	int GetModuleInfo(Devinfo* pInfo);

	// IGimic2
	int GetModuleType(ChipType* pnType);

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
	/**
	 * @brief FM data
	 */
	struct FMDATA
	{
		UINT16 wAddr;		/*!< address */
		UINT8 cData;		/*!< data */
		UINT8 cPadding;		/*!< padding */
	};

	CUsbDev m_usb;			/*!< USB */
	CGuard m_usbGuard;		/*!< The guard of accessing USB */
	CGuard m_queGuard;		/*!< The guard of que */
	ChipType m_nChipType;	/*!< The type of chip */
	size_t m_nQueIndex;		/*!< The position in que */
	size_t m_nQueCount;		/*!< The count in que */
	FMDATA m_que[0x400];	/*!< que */
	UINT8 m_sReg[0x200];	/*!< register */

	int Transaction(const void* lpOutput, int cbOutput, void* lpInput = NULL, int cbInput = 0);
	int GetInfo(UINT8 cParam, Devinfo* pInfo);
	static void TailZeroFill(char* lpBuffer, size_t cbBuffer);
	UINT GetChipAddr(UINT nAddr) const;
};

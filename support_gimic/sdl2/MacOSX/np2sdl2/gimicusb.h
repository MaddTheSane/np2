/**
 * @file	gimicusb.h
 * @brief	G.I.M.I.C USB アクセス クラス
 */

#pragma once

#include <IOKit/usb/IOUSBLib.h>
#include "c86ctl.h"

/**
 * @brief G.I.M.I.C USB アクセス クラス
 */
class CGimicUSB
{
public:
	CGimicUSB();
	bool Open();
	void Close();
	bool IsEnabled() const;

private:
	IOUSBDeviceInterface** m_device;			/*!< デバイス */
	IOUSBInterfaceInterface** m_interface;		/*!< インタフェイス */
	static IOReturn ConfigureDevice(IOUSBDeviceInterface** dev);
	int Send(const void* lpBuffer, size_t cbBuffer);
	int Recv(void* lpBuffer, size_t cbBuffer);

public:
	// IGimic
	int SetSSGVolume(UINT8 cVolume);
	int GetSSGVolume(UINT8* pcVolume);
	int SetPLLClock(UINT nClock);
	int GetPLLClock(UINT* pnClock);
	int GetFWVer(UINT* pnMajor, UINT* pnMinor, UINT* pnRev, UINT* pnBuild);
	int GetMBInfo(Devinfo* pInfo);
	int GetModuleInfo(Devinfo* pInfo);

	// IRealChip
	int Reset();
	void Out(UINT nAddr, UINT8 cData);
	UINT8 In(UINT nAddr);

private:
	ChipType m_nChipType;		/*!< チップ タイプ */
	UINT8 m_sReg[0x200];		/*!< レジスタ */
	int GetInfo(UINT8 cParam, Devinfo* pInfo);
	static void TailZeroFill(char* lpBuffer, size_t cbBuffer);
};

/**
 * G.I.M.I.Cは有効?
 * @retval true 有効
 * @retval false 無効
 */
inline bool CGimicUSB::IsEnabled() const
{
	return (m_interface != NULL);
}

/**
 * @file	gimicusb.h
 * @brief	G.I.M.I.C USB アクセス クラスの宣言およびインターフェイスの定義をします
 */

#pragma once

#include "c86ctl.h"
#include "usbdev.h"
#include "misc/guard.h"
#include "misc/threadbase.h"

/**
 * @brief G.I.M.I.C USB アクセス クラス
 */
class CGimicUSB : protected CUsbDev, protected CThreadBase
{
public:
	CGimicUSB();
	~CGimicUSB();
	bool Initialize();
	void Deinitialize();
	bool IsEnabled() const;

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
	int Reset();
	void Out(UINT nAddr, UINT8 cData);
	UINT8 In(UINT nAddr);

	// IRealChip2
	int GetChipStatus(UINT nAddr, UINT8* pcStatus);
	void DirectOut(UINT nAddr, UINT8 cData);

protected:
	virtual bool Task();

private:
	/**
	 * @brief FM データ
	 */
	struct FMDATA
	{
		UINT16 wAddr;		/*!< アドレス */
		UINT8 cData;		/*!< データ */
		UINT8 cPadding;		/*!< パディング */
	};

	CGuard m_usbGuard;		/*!< USBアクセス */
	CGuard m_queGuard;		/*!< キュー */
	ChipType m_nChipType;	/*!< チップ タイプ */
	size_t m_nQueIndex;		/*!< データ インデックス */
	size_t m_nQueCount;		/*!< データ カウント */
	FMDATA m_que[0x400];	/*!< キュー */
	UINT8 m_sReg[0x200];	/*!< レジスタ */

	int Transaction(const void* lpOutput, size_t cbOutput, void* lpInput = NULL, size_t cbInput = 0);
	int GetInfo(UINT8 cParam, Devinfo* pInfo);
	static void TailZeroFill(char* lpBuffer, size_t cbBuffer);
	UINT GetChipAddr(UINT nAddr) const;
};

/**
 * G.I.M.I.Cは有効?
 * @retval true 有効
 * @retval false 無効
 */
inline bool CGimicUSB::IsEnabled() const
{
	return IsOpened();
}

/**
 * @file	sccispfmlight.h
 * @brief	Interface of accessing SPFM Light
 */

#pragma once

#include "sccisoundchip.h"
#include "sccisoundinterface.h"
#include "misc/tty.h"

namespace scci
{

/**
 * @brief The class of SPFM Light
 */
class CSpfmLight : public CSoundInterface
{
public:
	CSpfmLight(CSoundInterfaceManager* pManager, const std::string& deviceName);
	virtual ~CSpfmLight();

	virtual bool Initialize();
	virtual void Deinitialize();
	virtual size_t AddRef();
	virtual size_t Release();
	virtual void Add(const SCCI_SOUND_CHIP_INFO& info);

	virtual bool isSupportLowLevelApi();
	virtual bool setData(const unsigned char* pData, size_t dSendDataLen);
	virtual size_t getData(unsigned char* pData, size_t dGetDataLen);
	virtual bool reset();


protected:
	bool m_bReseted;								/*!< Reset flag */
	CTty m_serial;									/*!< Serial */

	/**
	 * @brief The class of Chip
	 */
	class Chip : public CSoundChip
	{
	public:
		Chip(CSoundInterface* pInterface, const SCCI_SOUND_CHIP_INFO& info);
		virtual bool setRegister(UINT dAddr, UINT dData);
		virtual bool init();
	};

	friend class Chip;
};

}	// namespace scci

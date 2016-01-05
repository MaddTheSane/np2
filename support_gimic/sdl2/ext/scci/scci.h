/**
 * @file	scci.h
 * @brief	Sound Chip common Interface
 */

#pragma once

#include "SCCIDefines.h"

namespace scci
{

class SoundChip;

/**
 * @brief Sound Chip Infomation
 */
struct SCCI_SOUND_CHIP_INFO
{
	char cSoundChipName[64];				/*!< Sound Chip Name */
	SC_CHIP_TYPE iSoundChip;				/*!< Sound Chip ID */
	SC_CHIP_TYPE iCompatibleSoundChip[2];	/*!< Compatible Sound Chip ID */
	UINT dColock;							/*!< Sound Chip clock */
	UINT dCompatibleColock[2];				/*!< Sound Chip clock */
};

/**
 * @brief Sound Interface Manager
 */
class SoundInterfaceManager
{
public:
	/* ---------- HI LEVEL APIs ---------- */

	/**
	 * Gets instance of the sound chip
	 * @param[in] iSoundChipType The type of the chip
	 * @param[in] dClock The clock of the chip
	 * @return The interface
	 */
	virtual SoundChip* getSoundChip(SC_CHIP_TYPE iSoundChipType, UINT dClock) = 0;

	/**
	 * Releases the instance of the sound chip
	 * @param[in] pSoundChip The instance of the sound chip
	 * @retval true If succeeded
	 * @retval false If failed
	 */
	virtual bool releaseSoundChip(SoundChip* pSoundChip) = 0;

	/**
	 * Releases all instances of the sound chip
	 * @retval true If succeeded
	 * @retval false If failed
	 */
	virtual bool releaseAllSoundChip() = 0;

	/**
	 * Resets all interfaces
	 * @retval true If succeeded
	 * @retval false If failed
	 */
	virtual bool reset() = 0;

	/**
	 * Sound Interface instance initialize
	 * @retval true If succeeded
	 * @retval false If failed
	 */
	virtual bool initializeInstance() = 0;

	/**
	 * Sound Interface instance release
	 * @retval true If succeeded
	 * @retval false If failed
	 */
	virtual bool releaseInstance() = 0;
};

/**
 * @brief Sound Interface(LOW level APIs)
 */
class SoundInterface
{
public:
	/**
	 * Sends data to the interface
	 * @param[in] pData The buffer of data
	 * @param[in] dSendDataLen The length of data
	 * @retval true If succeeded
	 * @retval false If failed
	 */
	virtual bool setData(const unsigned char* pData, size_t dSendDataLen) = 0;

	/**
	 * Gets data from the interface
	 * @param[out] pData The buffer of data
	 * @param[in] dGetDataLen The length of data
	 * @return The size of read
	 */
	virtual size_t getData(unsigned char* pData, size_t dGetDataLen) = 0;

	/**
	 * Resets the interface
	 * @retval true If succeeded
	 * @retval false If failed
	 */
	virtual bool reset() = 0;
};

/**
 * @brief Sound Chip
 */
class SoundChip
{
public:
	/**
	 * Gets the informations of the sound chip
	 * @return The pointer of informations
	 */
	virtual const SCCI_SOUND_CHIP_INFO* getSoundChipInfo() = 0;

	/**
	 * Gets sound chip type
	 * @return The type of the chip
	 */
	virtual SC_CHIP_TYPE getSoundChipType() = 0;

	/**
	 * Sets Register data
	 * Writes the register
	 * @param[in] dAddr The address of register
	 * @param[in] dData The data
	 * @retval true If succeeded
	 * @retval false If failed
	 */
	virtual bool setRegister(UINT dAddr, UINT dData) = 0;

	/**
	 * initialize sound chip(clear registers)
	 */
	// virtual bool init() = 0;
};

SoundInterfaceManager* GetSoundInterfaceManager();

}	// namespace scci

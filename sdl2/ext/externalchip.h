/**
 * @file	externalchip.h
 * @brief	Interface of thg external modules
 */

#pragma once

/**
 * @brief The interface of thg external modules
 */
class IExternalChip
{
public:
	/**
	 * ChipType
	 */
	enum ChipType
	{
		kNone				= 0,
		kYM2608,
		kYM3438,
		kYMF288
	};

	/**
	 * MessageType
	 */
	enum
	{
		kMute				= 0,
		kBusy
	};

	/**
	 * Destructor
	 */
	virtual ~IExternalChip() { }

	/**
	 * Get chip type
	 * @return The type of the chip
	 */
	virtual ChipType GetChipType() = 0;

	/**
	 * Reset
	 */
	virtual void Reset() = 0;

	/**
	 * Writes register
	 * @param[in] nAddr The address
	 * @param[in] cData The data
	 */
	virtual void WriteRegister(UINT nAddr, UINT8 cData) = 0;

	/**
	 * Message
	 * @param[in] nMessage The message
	 * @param[in] nParameter The parameter
	 * @return Result
	 */
	virtual INTPTR Message(UINT nMessage, INTPTR nParameter = 0) = 0;
};

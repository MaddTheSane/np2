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
	 * Destructor
	 */
	virtual ~IExternalChip() { }

	/**
	 * Initialize
	 * @retval true Succeeded
	 * @retval false Failed
	 */
	virtual bool Initialize() = 0;

	/**
	 * Deinitialize
	 */
	virtual void Deinitialize() = 0;

	/**
	 * Is device enabled?
	 * @retval true Enabled
	 * @retval false Disabled
	 */
	virtual bool IsEnabled() = 0;

	/**
	 * Is device busy?
	 * @retval true Busy
	 * @retval false Ready
	 */
	virtual bool IsBusy() = 0;

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
	 * Has ADPCM?
	 * @retval true Has
	 * @retval false No exist
	 */
	virtual bool HasADPCM() = 0;
};

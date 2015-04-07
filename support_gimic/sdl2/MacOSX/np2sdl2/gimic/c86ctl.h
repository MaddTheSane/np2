/**
 * @file	c86ctl.h
 * @brief	C86CTL の定義をします
 */

#pragma once

/**
 * Status code
 */
#define C86CTL_ERR_NONE				0		/*!< Succeeded */
#define C86CTL_ERR_UNKNOWN			-1		/*!< Unknown */
#define C86CTL_ERR_INVALID_PARAM	-2		/*!< Invalid parameter */
#define C86CTL_ERR_UNSUPPORTED		-3		/*!< Unspported */
#define C86CTL_ERR_NODEVICE			-1000	/*!< No devices */
#define C86CTL_ERR_NOT_IMPLEMENTED	-9999	/*!< Not implemented */

/**
 * Chip type
 */
enum ChipType
{
	CHIP_UNKNOWN = 0,			/*!< Unknown */
	CHIP_OPNA,					/*!< OPNA */
	CHIP_OPM,					/*!< OPM */
	CHIP_OPN3L,					/*!< OPN3L */
	CHIP_OPL3					/*!< OPL3 */
};

/**
 * @brief Device information structure
 */
struct Devinfo
{
	char Devname[16];		/*!< Device name */
	char Rev;				/*!< Revision */
	char Serial[15];		/*!< Serial */
};

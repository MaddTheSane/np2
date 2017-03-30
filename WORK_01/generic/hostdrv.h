/**
 * @file	hostdrv.h
 * @brief	Interface of host-drive
 */

#pragma once

#if defined(SUPPORT_HOSTDRV)

#include "statsave.h"

enum {
	HDFMODE_READ		= 0x01,
	HDFMODE_WRITE		= 0x02,
	HDFMODE_DELETE		= 0x04
};

/**
 * @brief The handle of file
 */
struct tagHostDrvHandle
{
	INTPTR hdl;					/*!< File handle */
	UINT mode;					/*!< Open mode */
	OEMCHAR path[MAX_PATH];		/*!< Path */
};
typedef struct tagHostDrvHandle _HDRVHANDLE;		/*!< define */
typedef struct tagHostDrvHandle *HDRVHANDLE;		/*!< define */

/**
 * @brief The instance of HostDrv
 */
struct tagHostDrv
{
	struct
	{
		UINT8	is_mount;
		UINT8	drive_no;
		UINT8	dosver_major;
		UINT8	dosver_minor;
		UINT16	sda_off;
		UINT16	sda_seg;
		UINT	flistpos;
	} stat;

	LISTARRAY	fhdl;
	LISTARRAY	flist;
};
typedef struct tagHostDrv HOSTDRV;					/*!< define */

#ifdef __cplusplus
extern "C"
{
#endif	/* __cplusplus */

void hostdrv_initialize(void);
void hostdrv_deinitialize(void);
void hostdrv_reset(void);

void hostdrv_mount(const void *arg1, long arg2);
void hostdrv_unmount(const void *arg1, long arg2);
void hostdrv_intr(const void *arg1, long arg2);

int hostdrv_sfsave(STFLAGH sfh, const SFENTRY *tbl);
int hostdrv_sfload(STFLAGH sfh, const SFENTRY *tbl);

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	/* defined(SUPPORT_HOSTDRV) */

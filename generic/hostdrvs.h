/**
 * @file	hostdrvs.h
 * @brief	Interface of host-drive
 */

#pragma once

#if defined(SUPPORT_HOSTDRV)

#include "hostdrv.h"
#include "dosio.h"

/**
 * @brief DOS ファイル情報
 */
struct tagHostDrvFile
{
	char	fcbname[11];	/*!< FCB 名 */
	UINT8	exist;			/*!< 存在するか? */
	UINT	caps;			/*!< 情報フラグ */
	UINT32	size;			/*!< サイズ */
	UINT32	attr;			/*!< 属性 */
	DOSDATE	date;			/*!< 日付 */
	DOSTIME	time;			/*!< 時間 */
};
typedef struct tagHostDrvFile HDRVFILE;		/*!< 定義 */

/**
 * @brief ファイル リスト情報
 */
struct tagHostDrvList
{
	HDRVFILE file;					/*!< DOS ファイル情報 */
	OEMCHAR szFilename[MAX_PATH];	/*!< ファイル名 */
};
typedef struct tagHostDrvList _HDRVLST;		/*!< 定義 */
typedef struct tagHostDrvList *HDRVLST;		/*!< 定義 */

/**
 * @brief パス情報
 */
struct tagHostDrvPath
{
	HDRVFILE file;				/*!< DOS ファイル情報 */
	OEMCHAR szPath[MAX_PATH];	/*!< パス */
};
typedef struct tagHostDrvPath HDRVPATH;		/*!< 定義 */

LISTARRAY hostdrvs_getpathlist(const OEMCHAR *lpDirectory);
BRESULT hostdrvs_getrealdir(OEMCHAR *lpPath, UINT cchPath, char *lpFcbname, const char *lpDosPath);
BRESULT hostdrvs_getrealpath(HDRVPATH *phdp, const char *lpDosPath);
BRESULT hostdrvs_newrealpath(HDRVPATH *phdp, const char *lpDosPath);
void hostdrvs_fhdlallclose(LISTARRAY fileArray);
HDRVHANDLE hostdrvs_fhdlsea(LISTARRAY fileArray);

#endif	/* defined(SUPPORT_HOSTDRV) */

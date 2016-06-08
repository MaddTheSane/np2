/**
 * @file	hostdrvs.h
 * @brief	Interface of host-drive
 */

#pragma once

#if defined(SUPPORT_HOSTDRV)

#include "hostdrv.h"
#include "dosio.h"

/**
 * @brief DOS �t�@�C�����
 */
struct tagHostDrvFile
{
	char	fcbname[11];	/*!< FCB �� */
	UINT8	exist;			/*!< ���݂��邩? */
	UINT	caps;			/*!< ���t���O */
	UINT32	size;			/*!< �T�C�Y */
	UINT32	attr;			/*!< ���� */
	DOSDATE	date;			/*!< ���t */
	DOSTIME	time;			/*!< ���� */
};
typedef struct tagHostDrvFile HDRVFILE;		/*!< ��` */

/**
 * @brief �t�@�C�� ���X�g���
 */
struct tagHostDrvList
{
	HDRVFILE file;					/*!< DOS �t�@�C����� */
	OEMCHAR szFilename[MAX_PATH];	/*!< �t�@�C���� */
};
typedef struct tagHostDrvList _HDRVLST;		/*!< ��` */
typedef struct tagHostDrvList *HDRVLST;		/*!< ��` */

/**
 * @brief �p�X���
 */
struct tagHostDrvPath
{
	HDRVFILE file;				/*!< DOS �t�@�C����� */
	OEMCHAR szPath[MAX_PATH];	/*!< �p�X */
};
typedef struct tagHostDrvPath HDRVPATH;		/*!< ��` */

LISTARRAY hostdrvs_getpathlist(const OEMCHAR *lpDirectory);
BRESULT hostdrvs_getrealdir(OEMCHAR *lpPath, UINT cchPath, char *lpFcbname, const char *lpDosPath);
BRESULT hostdrvs_getrealpath(HDRVPATH *phdp, const char *lpDosPath);
BRESULT hostdrvs_newrealpath(HDRVPATH *phdp, const char *lpDosPath);
void hostdrvs_fhdlallclose(LISTARRAY fileArray);
HDRVHANDLE hostdrvs_fhdlsea(LISTARRAY fileArray);

#endif	/* defined(SUPPORT_HOSTDRV) */

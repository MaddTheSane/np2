/**
 * @file	hostdrvs.c
 * @brief	Implementation of host-drive
 */

#include "compiler.h"
#include "hostdrvs.h"

#if defined(SUPPORT_HOSTDRV)

#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
#include "oemtext.h"
#endif
#include "pccore.h"

/*! ルート情報 */
static const HDRVFILE hddroot = {{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '}, 0, 0, 0, 0x10, {0}, {0}};

/*! DOSで許可されるキャラクタ */
static const UINT8 s_cDosCharacters[] =
{
	0xfa, 0x23,		/* '&%$#"!  /.-,+*)( */
	0xff, 0x03,		/* 76543210 ?>=<;:98 */
	0xff, 0xff,		/* GFEDCBA@ ONMLKJIH */
	0xff, 0xef,		/* WVUTSRQP _^]\[ZYX */
	0x01, 0x00,		/* gfedcba` onmlkjih */
	0x00, 0x40		/* wvutsrqp ~}|{zyx  */
};

/**
 * パスを FCB に変換
 * @param[out] lpFcbname FCB
 * @param[in] cchFcbname FCB バッファ サイズ
 * @param[in] lpPath パス
 */
static void RealPath2FcbSub(char *lpFcbname, UINT cchFcbname, const char *lpPath)
{
	REG8 c;

	while (cchFcbname)
	{
		c = (UINT8)*lpPath++;
		if (c == 0)
		{
			break;
		}
#if defined(OSLANG_SJIS) || defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
		if ((((c ^ 0x20) - 0xa1) & 0xff) < 0x3c)
		{
			if (lpPath[0] == '\0')
			{
				break;
			}
			if (cchFcbname < 2)
			{
				break;
			}
			lpFcbname[0] = c;
			lpFcbname[1] = *lpPath++;
			lpFcbname += 2;
			cchFcbname -= 2;
		}
		else if (((c - 0x20) & 0xff) < 0x60)
		{
			if (((c - 'a') & 0xff) < 26)
			{
				c -= 0x20;
			}
			if (s_cDosCharacters[(c >> 3) - (0x20 >> 3)] & (1 << (c & 7)))
			{
				*lpFcbname++ = c;
				cchFcbname--;
			}
		}
		else if (((c - 0xa0) & 0xff) < 0x40)
		{
			*lpFcbname++ = c;
			cchFcbname--;
		}
#else
		if (((c - 0x20) & 0xff) < 0x60)
		{
			if (((c - 'a') & 0xff) < 26)
			{
				c -= 0x20;
			}
			if (s_cDosCharacters[(c >> 3) - (0x20 >> 3)] & (1 << (c & 7)))
			{
				*lpFcbname++ = c;
				cchFcbname--;
			}
		}
		else if (c >= 0x80)
		{
			*lpFcbname++ = c;
			cchFcbname--;
		}
#endif
	}
}

/**
 * パスを FCB に変換
 * @param[out] lpFcbname FCB
 * @param[in] fli パス
 * @retval SUCCESS 成功
 * @retval FAILURE 失敗
 */
static BRESULT RealName2Fcb(char *lpFcbname, const FLINFO *fli)
{
	OEMCHAR	*ext;
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
	char sjis[MAX_PATH];
#endif
	OEMCHAR szFilename[MAX_PATH];

	FillMemory(lpFcbname, 11, ' ');

	ext = file_getext(fli->path);
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
	oemtext_oemtosjis(sjis, NELEMENTS(sjis), ext, (UINT)-1);
	RealPath2FcbSub(lpFcbname + 8, 3, sjis);
#else
	RealPath2FcbSub(lpFcbname + 8, 3, ext);
#endif

	file_cpyname(szFilename, fli->path, NELEMENTS(szFilename));
	file_cutext(szFilename);
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
	oemtext_oemtosjis(sjis, NELEMENTS(sjis), szFilename, (UINT)-1);
	RealPath2FcbSub(lpFcbname + 0, 8, sjis);
#else
	RealPath2FcbSub(lpFcbname + 0, 8, szFilename);
#endif
	return SUCCESS;
}

/**
 * FCB 名が一致するか
 * @param[in] vpItem アイテム
 * @param[in] vpArg ユーザ引数
 * @retval TRUE 一致
 * @retval FALSE 不一致
 */
static BOOL IsMatchName(void *vpItem, void *vpArg)
{
	if (!memcmp(((HDRVLST)vpItem)->file.fcbname, vpArg, 11))
	{
		return TRUE;
	}
	return FALSE;
}

/**
 * ディレクトリ名が一致するか
 * @param[in] vpItem アイテム
 * @param[in] vpArg ユーザ引数
 * @retval TRUE 一致
 * @retval FALSE 不一致
 */
static BOOL IsMatchDir(void *vpItem, void *vpArg)
{

	if ((((HDRVLST)vpItem)->file.attr & 0x10) && (!memcmp(((HDRVLST)vpItem)->file.fcbname, vpArg, 11)))
	{
		return TRUE;
	}
	return FALSE;
}

/**
 * ファイル一覧を取得
 * @param[in] lpDirectory ディレクトリ
 * @return ファイル一覧
 */
LISTARRAY hostdrvs_getpathlist(const OEMCHAR *lpDirectory)
{
	FLISTH flh;
	FLINFO fli;
	LISTARRAY ret;
	char fcbname[11];
	HDRVLST hdd;

	flh = file_list1st(lpDirectory, &fli);
	if (flh == FLISTH_INVALID)
	{
		goto hdgpl_err1;
	}
	ret = listarray_new(sizeof(_HDRVLST), 64);
	if (ret == NULL)
	{
		goto hdgpl_err2;
	}
	do
	{
		if ((RealName2Fcb(fcbname, &fli) == SUCCESS) && (fcbname[0] != ' ') && (listarray_enum(ret, IsMatchName, fcbname) == NULL))
		{
			hdd = (HDRVLST)listarray_append(ret, NULL);
			if (hdd == NULL)
			{
				break;
			}
			CopyMemory(hdd->file.fcbname, fcbname, 11);
			hdd->file.exist = 1;
			hdd->file.caps = fli.caps;
			hdd->file.size = fli.size;
			hdd->file.attr = fli.attr;
			hdd->file.date = fli.date;
			hdd->file.time = fli.time;
			file_cpyname(hdd->szFilename, fli.path, NELEMENTS(hdd->szFilename));
		}
	} while (file_listnext(flh, &fli) == SUCCESS);
	if (listarray_getitems(ret) == 0)
	{
		goto hdgpl_err3;
	}
	file_listclose(flh);
	return ret;

hdgpl_err3:
	listarray_destroy(ret);

hdgpl_err2:
	file_listclose(flh);

hdgpl_err1:
	return NULL;
}

/* ---- */

/**
 * DOS 名を FCB に変換
 * @param[out] lpFcbname FCB
 * @param[in] cchFcbname FCB バッファ サイズ
 * @param[in] lpDosPath DOS パス
 * @return 次の DOS パス
 */
static const char *DosPath2FcbSub(char *lpFcbname, UINT cchFcbname, const char *lpDosPath)
{
	char c;

	while (cchFcbname)
	{
		c = lpDosPath[0];
		if ((c == 0) || (c == '.') || (c == '\\'))
		{
			break;
		}
		if ((((c ^ 0x20) - 0xa1) & 0xff) < 0x3c)
		{
			if (lpDosPath[1] == '\0')
			{
				break;
			}
			if (cchFcbname < 2)
			{
				break;
			}
			lpDosPath++;
			lpFcbname[0] = c;
			lpFcbname[1] = *lpDosPath;
			lpFcbname += 2;
			cchFcbname -= 2;
		}
		else
		{
			*lpFcbname++ = c;
			cchFcbname--;
		}
		lpDosPath++;
	}
	return lpDosPath;
}

/**
 * DOS 名を FCB に変換
 * @param[out] lpFcbname FCB
 * @param[in] lpDosPath DOS パス
 * @return 次の DOS パス
 */
static const char *DosPath2Fcb(char *lpFcbname, const char *lpDosPath)
{
	FillMemory(lpFcbname, 11, ' ');
	lpDosPath = DosPath2FcbSub(lpFcbname, 8, lpDosPath);
	if (lpDosPath[0] == '.')
	{
		lpDosPath = DosPath2FcbSub(lpFcbname + 8, 3, lpDosPath + 1);
	}
	return lpDosPath;
}

/**
 * 新規パスを得る
 * @param[out] phdp HostDrv パス
 * @param[in] lpDosPath DOS パス
 * @retval SUCCESS 成功
 * @retval FAILURE 失敗
 */
BRESULT hostdrvs_getrealpath(HDRVPATH *phdp, const char *lpDosPath)
{
	OEMCHAR szPath[MAX_PATH];
	LISTARRAY lst;
	const HDRVFILE *di;
	HDRVLST hdl;
	char fcbname[11];

	file_cpyname(szPath, np2cfg.hdrvroot, NELEMENTS(szPath));
	lst = NULL;
	di = &hddroot;
	while (lpDosPath[0] != '\0')
	{
		if ((lpDosPath[0] != '\\') || (!(di->attr & 0x10)))
		{
			goto hdsgrp_err;
		}
		file_setseparator(szPath, NELEMENTS(szPath));
		lpDosPath++;
		if (lpDosPath[0] == '\0')
		{
			di = &hddroot;
			break;
		}
		lpDosPath = DosPath2Fcb(fcbname, lpDosPath);
		listarray_destroy(lst);
		lst = hostdrvs_getpathlist(szPath);
		hdl = (HDRVLST)listarray_enum(lst, IsMatchName, fcbname);
		if (hdl == NULL)
		{
			goto hdsgrp_err;
		}
		file_catname(szPath, hdl->szFilename, NELEMENTS(szPath));
		di = &hdl->file;
	}
	if (phdp)
	{
		phdp->file = *di;
		file_cpyname(phdp->szPath, szPath, NELEMENTS(phdp->szPath));
	}
	listarray_destroy(lst);
	return SUCCESS;

hdsgrp_err:
	listarray_destroy(lst);
	return FAILURE;
}

/**
 * ディレクトリを得る
 * @param[out] lpPath パス バッファ
 * @param[in] cchPath パス バッファの長さ
 * @param[out] lpFcbname FCB 名
 * @param[in] lpDosPath DOS パス
 * @retval SUCCESS 成功
 * @retval FAILURE 失敗
 */
BRESULT hostdrvs_getrealdir(OEMCHAR *lpPath, UINT cchPath, char *lpFcbname, const char *lpDosPath)
{
	LISTARRAY lst;
	HDRVLST hdl;

	file_cpyname(lpPath, np2cfg.hdrvroot, cchPath);
	if (lpDosPath[0] == '\\')
	{
		file_setseparator(lpPath, cchPath);
		lpDosPath++;
	}
	else if (lpDosPath[0] != '\0')
	{
		goto hdsgrd_err;
	}
	while (TRUE /*CONSTCOND*/)
	{
		lpDosPath = DosPath2Fcb(lpFcbname, lpDosPath);
		if (lpDosPath[0] != '\\')
		{
			break;
		}
		lst = hostdrvs_getpathlist(lpPath);
		hdl = (HDRVLST)listarray_enum(lst, IsMatchDir, lpFcbname);
		if (hdl != NULL)
		{
			file_catname(lpPath, hdl->szFilename, cchPath);
		}
		listarray_destroy(lst);
		if (hdl == NULL)
		{
			goto hdsgrd_err;
		}
		file_setseparator(lpPath, cchPath);
		lpDosPath++;
	}
	if (lpDosPath[0] != '\0')
	{
		goto hdsgrd_err;
	}
	return SUCCESS;

hdsgrd_err:
	return FAILURE;
}

/**
 * 新規パスを得る
 * @param[out] phdp HostDrv パス
 * @param[in] lpDosPath DOS パス
 * @retval SUCCESS 成功
 * @retval FAILURE 失敗
 */
BRESULT hostdrvs_newrealpath(HDRVPATH *phdp, const char *lpDosPath)
{
	OEMCHAR szPath[MAX_PATH];
	char fcbname[11];
	LISTARRAY lst;
	HDRVLST hdl;
	char szDosName[16];
	UINT i;
	char *p;
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
	OEMCHAR oemname[64];
#endif

	if ((hostdrvs_getrealdir(szPath, NELEMENTS(szPath), fcbname, lpDosPath) != SUCCESS) || (fcbname[0] == ' '))
	{
		return FAILURE;
	}
	lst = hostdrvs_getpathlist(szPath);
	hdl = (HDRVLST)listarray_enum(lst, IsMatchName, fcbname);
	if (hdl != NULL)
	{
		file_catname(szPath, hdl->szFilename, NELEMENTS(szPath));
		if (phdp)
		{
			phdp->file = hdl->file;
			file_cpyname(phdp->szPath, szPath, NELEMENTS(phdp->szPath));
		}
	}
	else
	{
		p = szDosName;
		for (i = 0; (i < 8) && (fcbname[i] != ' '); i++)
		{
			*p++ = fcbname[i];
		}
		if (fcbname[8] != ' ')
		{
			*p++ = '.';
			for (i = 8; (i < 11) && (fcbname[i] != ' '); i++)
			{
				*p++ = fcbname[i];
			}
		}
		*p = '\0';
		/* ここで SJIS->OEMコードに未変換！ */
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
		oemtext_sjistooem(oemname, NELEMENTS(oemname), szDosName, (UINT)-1);
		file_catname(szPath, oemname, NELEMENTS(szPath));
#else
		file_catname(szPath, szDosName, NELEMENTS(szPath));
#endif
		if (phdp)
		{
			ZeroMemory(&phdp->file, sizeof(phdp->file));
			CopyMemory(phdp->file.fcbname, fcbname, 11);
			file_cpyname(phdp->szPath, szPath, NELEMENTS(phdp->szPath));
		}
	}
	listarray_destroy(lst);
	return SUCCESS;
}

/* ---- */

/**
 * ファイルハンドルをクローズする
 * @param[in] vpItem アイテム
 * @param[in] vpArg ユーザ引数
 * @retval FALSE 継続
 */
static BOOL CloseFileHandle(void *vpItem, void *vpArg)
{
	INTPTR fh;

	fh = ((HDRVHANDLE)vpItem)->hdl;
	if (fh != (INTPTR)FILEH_INVALID)
	{
		((HDRVHANDLE)vpItem)->hdl = (INTPTR)FILEH_INVALID;
		file_close((FILEH)fh);
	}
	(void)vpArg;
	return FALSE;
}

/**
 * すべてクローズ
 * @param[in] fileArray ファイル リスト ハンドル
 */
void hostdrvs_fhdlallclose(LISTARRAY fileArray)
{
	listarray_enum(fileArray, CloseFileHandle, NULL);
}

/**
 * 空ハンドルを見つけるコールバック
 * @param[in] vpItem アイテム
 * @param[in] vpArg ユーザ引数
 * @retval TRUE 見つかった
 * @retval FALSE 見つからなかった
 */
static BOOL IsHandleInvalid(void *vpItem, void *vpArg)
{
	if (((HDRVHANDLE)vpItem)->hdl == (INTPTR)FILEH_INVALID)
	{
		return TRUE;
	}
	(void)vpArg;
	return FALSE;
}

/**
 * 新しいハンドルを得る
 * @param[in] fileArray ファイル リスト ハンドル
 * @return 新しいハンドル
 */
HDRVHANDLE hostdrvs_fhdlsea(LISTARRAY fileArray)
{
	HDRVHANDLE ret;

	if (fileArray == NULL)
	{
		TRACEOUT(("hostdrvs_fhdlsea hdl == NULL"));
	}
	ret = (HDRVHANDLE)listarray_enum(fileArray, IsHandleInvalid, NULL);
	if (ret == NULL)
	{
		ret = (HDRVHANDLE)listarray_append(fileArray, NULL);
		if (ret != NULL)
		{
			ret->hdl = (INTPTR)FILEH_INVALID;
		}
	}
	return ret;
}

#endif

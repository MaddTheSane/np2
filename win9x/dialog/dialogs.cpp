/**
 * @file	dialogs.cpp
 * @brief	ダイアログ ヘルパーの動作の定義を行います
 */

#include "compiler.h"
#include "dialogs.h"
#include "misc\tstring.h"

static BOOL openFileParam(LPOPENFILENAME lpOFN, PCFSPARAM pcParam,
							LPTSTR pszPath, UINT uSize,
							BOOL (WINAPI * fnAPI)(LPOPENFILENAME lpofn))
{
	if ((lpOFN == NULL) || (pcParam == NULL) ||
		(pszPath == NULL) || (uSize == 0) || (fnAPI == NULL))
	{
		return FALSE;
	}

	std::tstring rTitle(LoadTString(pcParam->lpszTitle));
	std::tstring rFilter(LoadTString(pcParam->lpszFilter));
	std::tstring rDefExt(LoadTString(pcParam->lpszDefExt));

	for (std::tstring::iterator it = rFilter.begin(); it != rFilter.end(); ++it)
	{
#if !defined(_UNICODE)
		if (IsDBCSLeadByte(static_cast<BYTE>(*it)))
		{
			++it;
			if (it == rFilter.end())
			{
				break;
			}
			continue;
		}
#endif	// !defined(_UNICODE)
		if (*it == '|')
		{
			*it = '\0';
		}
	}

	lpOFN->lpstrTitle = rTitle.c_str();
	lpOFN->lpstrFilter = rFilter.c_str();
	lpOFN->lpstrDefExt = rDefExt.c_str();
	lpOFN->nFilterIndex = pcParam->nFilterIndex;
	lpOFN->lpstrFile = pszPath;
	lpOFN->nMaxFile = uSize;

	return (*fnAPI)(lpOFN);
}

BOOL dlgs_openfile(HWND hWnd, PCFSPARAM pcParam, LPTSTR pszPath, UINT uSize, int *pnRO)
{
	OPENFILENAME	ofn;
	BOOL			bResult;

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.Flags = OFN_FILEMUSTEXIST;

	if (pnRO == NULL)
	{
		ofn.Flags |= OFN_HIDEREADONLY;
	}

	bResult = openFileParam(&ofn, pcParam, pszPath, uSize, GetOpenFileName);

	if ((bResult) && (pnRO != NULL))
	{
		*pnRO = ofn.Flags & OFN_READONLY;
	}

	return bResult;
}

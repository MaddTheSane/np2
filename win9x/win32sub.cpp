#include	"compiler.h"


void __msgbox(const char *title, const char *msg) {

#if !defined(_UNICODE)
	const TCHAR *_title = title;
	const TCHAR *_msg = msg;
#else
	TCHAR _title[256];
	TCHAR _msg[2048];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, title, -1,
												_title, NELEMENTS(_title));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, msg, -1,
												_msg, NELEMENTS(_msg));
#endif
	MessageBox(NULL, _msg, _title, MB_OK);
}


// WinAPI‚¾‚Æ Win95‚ÅƒoƒO‚ ‚é‚Ì
int loadstring2(HINSTANCE hInstance, UINT uID,
										LPTSTR lpBuffer, int nBufferMax) {

	HMODULE	hModule;
	HRSRC	hRsrc;
	DWORD	dwResSize;
	HGLOBAL	hGlobal;
	UINT16	*pRes;
	DWORD	dwPos;
	int		nLength;

	hModule = (HMODULE)hInstance;
	hRsrc = FindResource(hModule, MAKEINTRESOURCE((uID >> 4) + 1), RT_STRING);
	if (hRsrc == NULL) {
		return(0);
	}
	dwResSize = SizeofResource(hModule, hRsrc);
	hGlobal = LoadResource(hModule, hRsrc);
	if (hGlobal == NULL) {
		return(0);
	}
	pRes = (UINT16 *)LockResource(hRsrc);
	dwPos = 0;
	uID = uID & 15;
	while((uID) && (dwPos < dwResSize)) {
		dwPos += pRes[dwPos] + 1;
		uID--;
	}
	if (dwPos >= dwResSize) {
		return(0);
	}

	nLength = pRes[dwPos];
	dwPos++;
	nLength = min(nLength, (int)(dwResSize - dwPos));
#if defined(_UNICODE)
	if ((lpBuffer != NULL) && (nBufferMax > 0)) {
		nBufferMax--;
		nLength = min(nLength, nBufferMax);
		if (nLength) {
			CopyMemory(lpBuffer, pRes + dwPos, nLength * sizeof(UINT16));
		}
		lpBuffer[nLength] = '\0';
	}
#else
	if ((lpBuffer != NULL) && (nBufferMax > 0)) {
		nBufferMax--;
		if (nBufferMax == 0) {
			nLength = 0;
		}
	}
	else {
		lpBuffer = NULL;
		nBufferMax = 0;
	}
	nLength = WideCharToMultiByte(CP_ACP, 0, pRes + dwPos, nLength,
											lpBuffer, nBufferMax, NULL, NULL);
	if (lpBuffer) {
		lpBuffer[nLength] = '\0';
	}
#endif
	return(nLength);
}


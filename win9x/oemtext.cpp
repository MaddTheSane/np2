#include	"compiler.h"
#include	"oemtext.h"


#if defined(OSLANG_UTF8)
UINT oemtext_sjistooem(OEMCHAR *dst, UINT dcnt, const char *src, UINT scnt) {

	UINT	leng;
	UINT16	*ucs2;
	UINT	ret;

	(void)scnt;

	leng = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, -1, NULL, 0);
	if (leng == 0) {
		return(0);
	}
	ucs2 = (UINT16 *)_MALLOC(leng * sizeof(UINT16), "");
	if (ucs2 == NULL) {
		return(0);
	}
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, -1, ucs2, leng);
	ret = codecnv_ucs2toutf8(dst, dcnt, ucs2, leng);
	_MFREE(ucs2);
	return(ret);
}

UINT oemtext_oemtosjis(char *dst, UINT dcnt, const OEMCHAR *src, UINT scnt) {

	UINT	leng;
	UINT16	*ucs2;
	UINT	ret;

	(void)scnt;

	leng = codecnv_utf8toucs2(NULL, 0, src, (UINT)-1);
	if (leng == 0) {
		return(0);
	}
	ucs2 = (UINT16 *)_MALLOC(leng * sizeof(UINT16), "");
	if (ucs2 == NULL) {
		return(0);
	}
	codecnv_utf8toucs2(ucs2, leng, src, (UINT)-1);
	ret = WideCharToMultiByte(CP_ACP, 0, ucs2, leng, dst, dcnt, NULL, NULL);
	_MFREE(ucs2);
	return(ret);
}
#elif defined(OSLANG_UCS2)
UINT oemtext_sjistooem(OEMCHAR *dst, UINT dcnt, const char *src, UINT scnt) {

	return(MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, -1, dst, dcnt));
}

UINT oemtext_oemtosjis(char *dst, UINT dcnt, const OEMCHAR *src, UINT scnt) {

	return(WideCharToMultiByte(CP_ACP, 0, src, -1, dst, dcnt, NULL, NULL));
}
#endif


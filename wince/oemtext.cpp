#include	"compiler.h"
#include	"oemtext.h"

#if defined(UNICODE) && defined(OSLANG_UTF8)

#include	"codecnv.h"

UINT oemtext_sjistooem(char *dst, UINT dcnt, const char *src, UINT scnt) {

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

UINT oemtext_oemtosjis(char *dst, UINT dcnt, const char *src, UINT scnt) {

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
#endif


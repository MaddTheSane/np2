
#ifdef __cplusplus
extern "C" {
#endif

#if defined(OSLANG_UTF8)
UINT oemtext_sjis2oem(OEMCHAR *dst, UINT dcnt, const char *src, UINT scnt);
UINT oemtext_oem2sjis(char *dst, UINT dcnt, const OEMCHAR *src, UINT scnt);
#elif defined(OSLANG_UCS2)
#define oemtext_sjis2oem(dst, dcnt, src, scnt) \
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, -1, dst, dcnt)
#define oemtext_oem2sjis(dst, dcnt, src, scnt) \
			WideCharToMultiByte(CP_ACP, 0, src, -1, dst, dcnt, NULL, NULL)
#endif

#ifdef __cplusplus
}
#endif



#ifdef __cplusplus
extern "C" {
#endif

#if defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
UINT oemtext_sjistooem(OEMCHAR *dst, UINT dcnt, const char *src, UINT scnt);
UINT oemtext_oemtosjis(char *dst, UINT dcnt, const OEMCHAR *src, UINT scnt);
#endif

#ifdef __cplusplus
}
#endif


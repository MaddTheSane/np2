
#ifdef __cplusplus
extern "C" {
#endif

UINT oemtext_sjistooem(OEMCHAR *dst, UINT dcnt, const char *src, UINT scnt);
UINT oemtext_oemtosjis(char *dst, UINT dcnt, const OEMCHAR *src, UINT scnt);

#ifdef __cplusplus
}
#endif


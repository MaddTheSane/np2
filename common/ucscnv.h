
#ifdef __cplusplus
extern "C" {
#endif

UINT ucscnv_utf8toucs2(UINT16 *dst, UINT dcnt, const char *src, UINT scnt);
UINT ucscnv_ucs2toutf8(char *dst, UINT dcnt, const UINT16 *src, UINT scnt);

#ifdef __cplusplus
}
#endif


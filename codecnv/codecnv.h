
#if !defined(NP2_CODECNV_H__)
#define	NP2_CODECNV_H__

#ifdef __cplusplus
extern "C" {
#endif

UINT codecnv_sjistoeuc(char *dst, UINT dcnt, const char *src, UINT scnt);
UINT codecnv_sjistoucs2(UINT16 *dst, UINT dcnt, const char *src, UINT scnt);

UINT codecnv_euctosjis(char *dst, UINT dcnt, const char *src, UINT scnt);
UINT codecnv_euctoucs2(UINT16 *dst, UINT dcnt, const char *src, UINT scnt);

UINT codecnv_utf8toucs2(UINT16 *dst, UINT dcnt, const char *src, UINT scnt);

UINT codecnv_ucs2toutf8(char *dst, UINT dcnt, const UINT16 *src, UINT scnt);

#ifdef __cplusplus
}
#endif

#endif	// !defined(NP2_CODECNV_H__)



#ifdef __cplusplus
extern "C" {
#endif

void codecnv_sjis2euc(char *euc, UINT ecnt, const char *sjis, UINT scnt);
void codecnv_sjis2utf(UINT16 *utf, UINT ucnt, const char *sjis, UINT scnt);

#ifdef __cplusplus
}
#endif



// テキストファイルの変換ルール

enum {
	TEXTCNV_DEFAULT	= 0,
	TEXTCNV_SJIS	= 1,
	TEXTCNV_EUC		= 2,
	TEXTCNV_UFT8	= 3,
	TEXTCNV_UCS2	= 4
};

typedef UINT (*TCTOOEM)(OEMCHAR *dst, UINT dcnt, const void *src, UINT scnt);
typedef UINT (*TCFROMOEM)(void *dst, UINT dcnt, const OEMCHAR *src, UINT scnt);

#ifdef __cplusplus
extern "C" {
#endif

BRESULT textcnv_tooem(UINT code, TCTOOEM *tcto);
BRESULT textcnv_fromoem(UINT code, TCFROMOEM *tcfo);

#ifdef __cplusplus
}
#endif


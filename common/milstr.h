
#define	SUPPORT_ANK
#define	SUPPORT_SJIS
#define	SUPPORT_EUC


#ifdef __cplusplus
extern "C" {
#endif

// 大文字小文字を同一視して比較
// ret 0:一致
int milank_cmp(const char *str, const char *cmp);
int milsjis_cmp(const char *str, const char *cmp);
int mileuc_cmp(const char *str, const char *cmp);

// 大文字小文字を 同一視してcmpのヌルまで比較
// ret 0:一致
int milank_memcmp(const char *str, const char *cmp);
int milsjis_memcmp(const char *str, const char *cmp);
int mileuc_memcmp(const char *str, const char *cmp);

// str[pos]が漢字１バイト目かどうか…
int milsjis_kanji1st(const char *str, int pos);
int mileuc_kanji1st(const char *str, int pos);

// str[pos]が漢字２バイト目かどうか…
int milsjis_kanji2nd(const char *str, int pos);
int mileuc_kanji2nd(const char *str, int pos);

// maxlen分だけ文字列をコピー
void milank_ncpy(char *dst, const char *src, int maxlen);
void milsjis_ncpy(char *dst, const char *src, int maxlen);
void mileuc_ncpy(char *dst, const char *src, int maxlen);

// maxlen分だけ文字列をキャット
void milank_ncat(char *dst, const char *src, int maxlen);
void milsjis_ncat(char *dst, const char *src, int maxlen);
void mileuc_ncat(char *dst, const char *src, int maxlen);


// 0~9, A~Z のみを大文字小文字を同一視して比較
// ret 0:一致
int milstr_extendcmp(const char *str, const char *cmp);

// 文字列からARGの取得
int milstr_getarg(char *str, char *arg[], int maxarg);

// HEX2INT
long milstr_solveHEX(const char *str);

// STR2INT
long milstr_solveINT(const char *str);

// STRLIST
const char *milstr_list(const char *lststr, UINT pos);

#ifdef __cplusplus
}
#endif


// ---- macros

#if defined(OSLANG_SJIS)
#define	ISKANJI1ST(c)			((((c ^ 0x20) - 0xa1) & 0xff) < 0x3c)
#define	milstr_cmp(s, c)		milsjis_cmp(s, c)
#define	milstr_memcmp(s, c)		milsjis_memcmp(s, c)
#define	milstr_kanji1st(s, p)	milsjis_kanji1st(s, p)
#define	milstr_kanji2nd(s, p)	milsjis_kanji2nd(s, p)
#define	milstr_ncpy(d, s, l)	milsjis_ncpy(d, s, l)
#define	milstr_ncat(d, s, l)	milsjis_ncat(d, s, l)
#elif defined(OSLANG_EUC)
#define	ISKANJI1ST(c)			(((c - 0xa1) & 0xff) < 0x5d)
#define	milstr_cmp(s, c)		mileuc_cmp(s, c)
#define	milstr_memcmp(s, c)		mileuc_memcmp(s, c)
#define	milstr_kanji1st(s, p)	mileuc_kanji1st(s, p)
#define	milstr_kanji2nd(s, p)	mileuc_kanji2nd(s, p)
#define	milstr_ncpy(d, s, l)	mileuc_ncpy(d, s, l)
#define	milstr_ncat(d, s, l)	mileuc_ncat(d, s, l)
#else
#define	ISKANJI1ST(c)			(0)
#define	milstr_cmp(s, c)		milank_cmp(s, c)
#define	milstr_memcmp(s, c)		milank_memcmp(s, c)
#define	milstr_kanji1st(s, p)	(0)
#define	milstr_kanji2nd(s, p)	(0)
#define	milstr_ncpy(d, s, l)	milank_ncpy(d, s, l)
#define	milstr_ncat(d, s, l)	milank_ncat(d, s, l)
#endif


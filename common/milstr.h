
#ifdef __cplusplus
extern "C" {
#endif

// 大文字小文字を同一視して比較
// ret 0:一致
BOOL milstr_cmp(const char *str, const char *cmp);

// 大文字小文字を 同一視してcmpのヌルまで比較
// ret 0:一致
BOOL milstr_memcmp(const char *str, const char *cmp);

// 0~9, A~Z のみを大文字小文字を同一視して比較
// ret 0:一致
BOOL milstr_extendcmp(const char *str, const char *cmp);

// str[pos]が漢字１バイト目かどうか…
int milstr_kanji1st(const char *str, int pos);

// str[pos]が漢字２バイト目かどうか…
int milstr_kanji2nd(const char *str, int pos);

// maxlen分だけ文字列をコピー
void milstr_ncpy(char *dst, const char *src, int maxlen);

// maxlen分だけ文字列をキャット
void milstr_ncat(char *dst, const char *src, int maxlen);

// 文字列からARGの取得
int milstr_getarg(char *str, char *arg[], int maxarg);

// HEX2INT
long milstr_solveHEX(const char *str);

// STR2INT
long milstr_solveINT(const char *str);

#ifdef __cplusplus
}
#endif



#ifdef __cplusplus
extern "C" {
#endif

// �啶���������𓯈ꎋ���Ĕ�r
// ret 0:��v
int milstr_cmp(const char *str, const char *cmp);

// �啶���������� ���ꎋ����cmp�̃k���܂Ŕ�r
// ret 0:��v
BOOL milstr_memcmp(const char *str, const char *cmp);

// 0~9, A~Z �݂̂�啶���������𓯈ꎋ���Ĕ�r
// ret 0:��v
BOOL milstr_extendcmp(const char *str, const char *cmp);

// str[pos]�������P�o�C�g�ڂ��ǂ����c
int milstr_kanji1st(const char *str, int pos);

// str[pos]�������Q�o�C�g�ڂ��ǂ����c
int milstr_kanji2nd(const char *str, int pos);

// maxlen��������������R�s�[
void milstr_ncpy(char *dst, const char *src, int maxlen);

// maxlen��������������L���b�g
void milstr_ncat(char *dst, const char *src, int maxlen);

// �����񂩂�ARG�̎擾
int milstr_getarg(char *str, char *arg[], int maxarg);

// HEX2INT
long milstr_solveHEX(const char *str);

// STR2INT
long milstr_solveINT(const char *str);

#ifdef __cplusplus
}
#endif


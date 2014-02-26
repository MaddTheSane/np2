
#if !defined(NP2_CODECNV_H__)
#define	NP2_CODECNV_H__

#ifdef __cplusplus
extern "C" {
#endif

UINT codecnv_sjistoeuc(char *dst, UINT dcnt, const char *src, UINT scnt);
UINT codecnv_sjistoucs2(UINT16 *dst, UINT dcnt, const char *src, UINT scnt);
UINT codecnv_sjis2utf(UINT16 *dst, UINT dcnt, const char *src, UINT scnt);

UINT codecnv_euctosjis(char *dst, UINT dcnt, const char *src, UINT scnt);
UINT codecnv_euctoucs2(UINT16 *dst, UINT dcnt, const char *src, UINT scnt);

UINT codecnv_utf8toucs2(UINT16 *dst, UINT dcnt, const char *src, UINT scnt);

UINT codecnv_ucs2toutf8(char *dst, UINT dcnt, const UINT16 *src, UINT scnt);

#ifdef __cplusplus
}
#endif

#endif	// !defined(NP2_CODECNV_H__)




// codecnv�d�l
//
//	(dst != NULL) ���̓e�X�g�̂ݍs�Ȃ��B���̏ꍇdcnt=�����ƈ����B
//
//	scnt == -1 �̏ꍇ�AString Mode�ƂȂ�B
//	�ENULL�܂ŕϊ�����
//	�Edcnt������Ȃ��Ă� dst�͕K��NULL Terminate�ƂȂ�B
//
//	�߂�l: �������݃o�b�t�@�����Ԃ�B(NULL Terminate���܂�)
//
//	codecnv_AtoB(NULL, 0, "ABC\0DEF", -1) == 4
//	codecnv_AtoB(buf, 5, "ABC\0DEF", -1) == 4 / Copy(buf, "ABC\0", 4)
//	codecnv_AtoB(buf, 3, "ABC\0DEF", -1) == 3 / Copy(buf, "AB\0", 3)
//
//
//	scnt != -1 �̏ꍇ�ABinary Mode�ƂȂ�B
//	�Escnt���ϊ����s�Ȃ�
//
//	�߂�l: �������݃o�b�t�@�����Ԃ�B
//
//	codecnv_AtoB(NULL, 0, "ABC\0DEF", 7) == 7
//	codecnv_AtoB(NULL, 0, "ABC\0DEF", 4) == 4
//	codecnv_AtoB(buf, 7, "ABC\0DEF", 5) == 5 / Copy(buf, "ABC\0D", 5)
//	codecnv_AtoB(buf, 3, "ABC\0DEF", 6) == 3 / Copy(buf, "ABC", 3)
//	codecnv_AtoB(buf, 3, "ABC\0DEF", 0) == 0 / Copy(buf, "", 0)



#ifndef PARTSCALL
#define	PARTSCALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

BYTE PARTSCALL AdjustAfterMultiply(BYTE value);
BYTE PARTSCALL AdjustBeforeDivision(BYTE value);
UINT PARTSCALL sjis2jis(UINT sjis);
UINT PARTSCALL jis2sjis(UINT jis);
void PARTSCALL satuation_s16(SINT16 *dst, const SINT32 *src, UINT size);
void PARTSCALL satuation_s16x(SINT16 *dst, const SINT32 *src, UINT size);

#ifdef __cplusplus
}
#endif


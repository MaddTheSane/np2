
#if defined(OSLANG_UTF8)
#include	"codecnv.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

#if defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
UINT oemtext_sjistooem(OEMCHAR *dst, UINT dcnt, const char *src, UINT scnt);
UINT oemtext_oemtosjis(char *dst, UINT dcnt, const OEMCHAR *src, UINT scnt);
#endif

#ifdef __cplusplus
}
#endif


UINT tchartooem(OEMCHAR *dst, UINT dcnt, const TCHAR *src, UINT scnt);
UINT oemtotchar(TCHAR *dst, UINT dcnt, const OEMCHAR *src, UINT scnt);


// ---- strres

#if defined(OSLANG_UTF8)

extern const TCHAR tchar_null[];
extern const TCHAR tchar_d[];
extern const TCHAR tchar_u[];
extern const TCHAR tchar_2x[];
extern const TCHAR tchar_2d[];
extern const TCHAR tchar_4X[];
extern const TCHAR tchar_bmp[];
extern const TCHAR tchar_d88[];
extern const TCHAR tchar_thd[];
extern const TCHAR tchar_hdd[];

#else

#define	tchar_null		str_null
#define	tchar_d			str_d
#define	tchar_u			str_u
#define	tchar_2x		str_2x
#define	tchar_2d		str_2d
#define	tchar_4X		str_4X
#define	tchar_bmp		str_bmp
#define	tchar_d88		str_d88
#define	tchar_thd		str_thd
#define	tchar_hdd		str_hdd

#endif


// ---- milstr

#if defined(OSLANG_UTF8)

long STRCALL miltchar_solveHEX(const TCHAR *str);
long STRCALL miltchar_solveINT(const TCHAR *str);

#else

#define	miltchar_solveHEX(s)	milstr_solveHEX(s)
#define	miltchar_solveINT(s)	milstr_solveINT(s)

#endif


#include	<windows.h>
#include	<tchar.h>
#include	<stdio.h>
#include	<stddef.h>


#define	BYTESEX_LITTLE
#if !defined(OSLANG_ANK) && !defined(OSLANG_SJIS) && !defined(OSLANG_EUC) && !defined(OSLANG_UTF8) && !defined(OSLANG_UCS2) && !defined(OSLANG_UCS4)
#define	OSLANG_SJIS
#endif
#define	OSLINEBREAK_CRLF

typedef	signed int			SINT;
typedef	signed char			SINT8;
typedef	unsigned char		UINT8;
typedef	signed short		SINT16;
typedef	unsigned short		UINT16;
typedef	signed int			SINT32;
typedef	unsigned int		UINT32;


#if !defined(SIZE_VGA)
#define	SIZE_QVGA
#if !defined(SIZE_VGATEST)
#define	RGB16			UINT32
#endif
#endif

#if defined(ADDON_SOFTKBD)
#define	SUPPORT_SOFTKBD		1
#endif


// for RISC test
#define	REG8		UINT
#define	REG16		UINT
#define	LOW12(a)	(((UINT32)((a) << 20)) >> 20)
#define	LOW14(a)	(((UINT32)((a) << 18)) >> 18)
#define	LOW15(a)	(((UINT32)((a) << 17)) >> 17)
#define	LOW16(a)	((UINT16)(a))
#define	HIGH16(a)	(((UINT32)(a)) >> 16)


#define	BRESULT				UINT
#if !defined(OSLANG_UTF8)
#define	OEMCHAR				TCHAR
#define	OEMTEXT(string)		_T(string)
#define	OEMSPRINTF			wsprintf
#define	OEMSTRLEN			lstrlen
#else
#define	OEMCHAR				char
#define	OEMTEXT(string)		(string)
#define	OEMSPRINTF			sprintf
#define	OEMSTRLEN			strlen
#endif

#include	"common.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"rect.h"
#include	"lstarray.h"
#include	"trace.h"


#define	GETTICK()			GetTickCount()
#define	__ASSERT(s)
#if !defined(UNICODE)
#define	SPRINTF				wsprintf
#define	STRLEN				lstrlen
#else
#define	SPRINTF				sprintf
#define	STRLEN				strlen
#endif

#define	VERMOUTH_LIB
#define	SOUND_CRITICAL

#define	SUPPORT_SJIS
#if defined(OSLANG_UTF8)
#define	SUPPORT_UTF8
#endif

#define	SUPPORT_16BPP
#define	MEMOPTIMIZE			2

#define	SOUNDRESERVE		100

#define	SCREEN_BPP			16

#define	SUPPORT_HOSTDRV
#define	SUPPORT_CRT15KHZ
#define	SUPPORT_SWSEEKSND

#define	CPUSTRUC_MEMWAIT


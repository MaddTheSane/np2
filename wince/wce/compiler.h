#include	<windows.h>
#include	<stdio.h>
#include	<stddef.h>

#define	BYTESEX_LITTLE
#define	OSLANG_SJIS
#define	OSLINEBREAK_CRLF


typedef signed char		SINT8;
typedef	signed short	SINT16;
typedef	signed int		SINT32;

#ifndef	ZeroMemory
#define	ZeroMemory(a, b)		memset((a),  0 , (b))
#endif
#ifndef	CopyMemory
#define	CopyMemory(a, b, c)		memcpy((a), (b), (c))
#endif
#ifndef	FillMemory
#define	FillMemory(a, b, c)		memset((a), (c), (b))
#endif


#if !defined(SIZE_VGA)
#define	SIZE_QVGA
#define	RGB16			UINT32
#endif

// for ARM optimize
#define	REG8		UINT
#define REG16		UINT
#define	LOW12(a)	((((UINT)(a)) << 20) >> 20)
#define	LOW14(a)	((((UINT)(a)) << 18) >> 18)
#define	LOW15(a)	((((UINT)(a)) << 17) >> 17)
#define	LOW16(a)	((UINT16)(a))
#define	HIGH16(a)	(((UINT32)(a)) >> 16)


#include	"common.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"rect.h"
#include	"lstarray.h"
#include	"trace.h"


#define	GETTICK()	GetTickCount()
#define	SPRINTF		sprintf
#define	__ASSERT(s)

#if defined(WIN32_PLATFORM_PSPC) && defined(SIZE_QVGA)
#define	MENU_TASKMINIMIZE
#define SUPPORT_SOFTKBD
#endif

#define	VERMOUTH_LIB
#define SOUND_CRITICAL

#define	SUPPORT_SJIS

#define	SUPPORT_16BPP
#define	MEMOPTIMIZE		2

#define	SOUNDRESERVE	100

#define	SCREEN_BPP		16

#if defined(ARM)
#define	OPNGENARM
#endif


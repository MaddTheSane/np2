#include	<windows.h>
#include	<stdio.h>
#include	<stddef.h>

#define	BYTESEX_LITTLE
#define	OSLANG_SJIS
#define	OSLINEBREAK_CRLF


typedef signed char		SINT8;
typedef unsigned char	UINT8;
typedef	signed short	SINT16;
typedef	unsigned short	UINT16;
typedef	signed int		SINT32;
typedef	unsigned int	UINT32;


#if !defined(SIZE_VGA)
#define	SIZE_QVGA
#if !defined(SIZE_VGATEST)
#define	RGB16			UINT32
#endif
#define	SUPPORT_SOFTKBD		1
#endif


// for RISC test
#define	REG8		UINT
#define REG16		UINT
#define	LOW12(a)				(((UINT32)((a) << 20)) >> 20)
#define	LOW14(a)				(((UINT32)((a) << 18)) >> 18)
#define	LOW15(a)				(((UINT32)((a) << 17)) >> 17)
#define	LOW16(a)				((UINT16)(a))
#define	HIGH16(a)				(((UINT32)(a)) >> 16)


#include	"common.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"rect.h"
#include	"lstarray.h"
#include	"trace.h"


#define	GETTICK()	GetTickCount()
#define	SPRINTF		wsprintf
#define	__ASSERT(s)

#define	VERMOUTH_LIB
#define SOUND_CRITICAL

#define	SUPPORT_SJIS

#define	SUPPORT_16BPP
#define	MEMOPTIMIZE		2

#define	SOUNDRESERVE	100

#define	SCREEN_BPP		16

#define	SUPPORT_HOSTDRV
#define	SUPPORT_CRT15KHZ
#define	SUPPORT_SWSEEKSND

#define	CPUSTRUC_MEMWAIT


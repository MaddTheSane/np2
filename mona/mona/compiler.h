#include	<monacapi.h>
#include	<SDL.h>

#define	MONA
#define	BYTESEX_LITTLE
#define	OSLANG_SJIS
#define	OSLINEBREAK_CRLF
#define DISABLE_MATH_H

typedef	signed char		SINT8;
typedef	unsigned char	UINT8;
typedef	signed short	SINT16;
typedef	unsigned short	UINT16;
typedef	signed int		SINT32;
typedef	unsigned int	UINT32;

typedef	int				BOOL;
typedef	signed char		CHAR;
typedef	signed char		TCHAR;
typedef	unsigned char	BYTE;


#ifndef	TRUE
#define	TRUE	1
#endif

#ifndef	FALSE
#define	FALSE	0
#endif

#ifndef	MAX_PATH
#define	MAX_PATH	32
#endif

#ifndef	max
#define	max(a,b)	(((a) > (b)) ? (a) : (b))
#endif
#ifndef	min
#define	min(a,b)	(((a) < (b)) ? (a) : (b))
#endif

#ifndef	ZeroMemory
#define	ZeroMemory(d,n)		memset((d), 0, (n))
#endif
#ifndef	CopyMemory
#define	CopyMemory(d,s,n)	memcpy((d), (s), (n))
#endif
#ifndef	FillMemory
#define	FillMemory(a, b, c)	memset((a), (c), (b))
#endif

#ifndef	roundup
#define	roundup(x, y)	((((x)+((y)-1))/(y))*(y))
#endif

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define	BYTESEX_BIG
#else /* SDL_BYTEORDER == SDL_LIL_ENDIAN */
#define	BYTESEX_LITTLE
#endif	/* SDL_BYTEORDER == SDL_BIG_ENDIAN */

#define	UNUSED(v)	((void)(v))

#ifndef	NELEMENTS
#define	NELEMENTS(a)	((int)(sizeof(a) / sizeof(a[0])))
#endif


#if !defined(SIZE_VGA)
#define	RGB16		UINT32
#define	SIZE_QVGA
#endif


#include	"common.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"rect.h"
#include	"lstarray.h"
#include	"trace.h"


#define	GETTICK()	SDL_GetTicks()
#define	SPRINTF		sprintf
#define	__ASSERT(s)

#define	LABEL				__declspec(naked)
#define RELEASE(x) 			if (x) {(x)->Release(); (x) = NULL;}


#define	DISABLE_SOUND
// #define	VERMOUTH_LIB
// #define	SOUND_CRITICAL
#define	PARTSCALL	__fastcall
#define	CPUCALL		__fastcall
#define	MEMCALL		__fastcall
#define DMACCALL	__fastcall
#define	IOOUTCALL	__fastcall
#define	IOINPCALL	__fastcall
#define SOUNDCALL	__fastcall
#define	VRAMCALL	__fastcall
#define	SCRNCALL	__fastcall

#define	SUPPORT_SJIS

#define	SUPPORT_16BPP
#define	SUPPORT_24BPP
#define	SUPPORT_32BPP

#define	SOUNDRESERVE	100

#define	SUPPORT_CRT15KHZ
#define	SUPPORT_HWSEEKSND


#include <stdio.h>
#include <stddef.h>
#include <SDL2/SDL.h>

#define	BYTESEX_LITTLE
#define	OSLANG_UTF8
#define	OSLINEBREAK_CRLF
#define RESOURCE_US

typedef	signed int			SINT;
typedef	unsigned int		UINT;
typedef	signed char			SINT8;
typedef	unsigned char		UINT8;
typedef	signed short		SINT16;
typedef	unsigned short		UINT16;
typedef	signed int			SINT32;
typedef	unsigned int		UINT32;

#define	BRESULT				UINT
#define	OEMCHAR				char
#define	OEMTEXT(string)		string
#define	OEMSPRINTF			sprintf
#define	OEMSTRLEN			strlen

#define SIZE_VGA
#if !defined(SIZE_VGA)
#define	RGB16		UINT32
#define	SIZE_QVGA
#endif

typedef	int				BOOL;
typedef	signed char		CHAR;
typedef	unsigned char	BYTE;

#ifndef	TRUE
#define	TRUE	1
#endif

#ifndef	FALSE
#define	FALSE	0
#endif

#ifndef	MAX_PATH
#define	MAX_PATH	256
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

#include "common.h"
#include "milstr.h"
#include "_memory.h"
#include "rect.h"
#include "lstarray.h"
#include "trace.h"


#define	GETTICK()			SDL_GetTicks()
#define	__ASSERT(s)
#define	SPRINTF				sprintf
#define	STRLEN				strlen

#define	VERMOUTH_LIB
// #define	SOUND_CRITICAL

#define	SUPPORT_UTF8

#define	SUPPORT_16BPP
#define	MEMOPTIMIZE		2

#define	SOUNDRESERVE	100

#define	SUPPORT_CRT15KHZ
#define	SUPPORT_HOSTDRV
#define	SUPPORT_SWSEEKSND
#define	SUPPORT_SASI
#define	SUPPORT_SCSI

#define SUPPORT_ARC
// #define SUPPORT_ZLIB

#define	SCREEN_BPP		16

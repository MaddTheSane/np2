#include	<windows.h>
#include	<stdio.h>
#include	<stddef.h>

#define	BYTESEX_LITTLE
#define	OSLANG_SJIS
#define	OSLINEBREAK_CRLF


typedef	short			SINT16;
typedef	unsigned short	UINT16;
typedef	int				SINT32;
typedef	unsigned int	UINT32;


#if !defined(SIZE_VGA)
#define	SIZE_QVGA
#define	RGB16			UINT32
#endif


#include	"common.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"rect.h"
#include	"lstarray.h"
#include	"trace.h"


#define	GETTICK()	GetTickCount()
#define	SPRINTF		wsprintf

#define	VERMOUTH_LIB
#define SOUND_CRITICAL

#define	SUPPORT_SJIS

#define	SUPPORT_16BPP
#define	MEMOPTIMIZE		2

#define	SOUNDRESERVE	100

#define	SCREEN_BPP		16


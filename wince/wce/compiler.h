#include	<windows.h>
#include	<stdio.h>
#include	<stddef.h>

#define	BYTESEX_LITTLE
#define	OSLANG_SJIS
#define	OSLINEBREAK_CRLF


typedef	short			SINT16;
typedef	int				SINT32;


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


#include	"common.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"rect.h"
#include	"lstarray.h"
#include	"trace.h"


#define	GETTICK()	GetTickCount()
#define	SPRINTF		sprintf

#if defined(WIN32_PLATFORM_PSPC)
#define	MENU_TASKMINIMIZE
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


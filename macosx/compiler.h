#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<stddef.h>
#ifndef NP2GCC
#include	<MultiProcessing.h>
#endif

#define	MACOS
#define	BYTESEX_BIG
#define	OSLANG_SJIS
#define	OSLINEBREAK_CR


typedef signed int		SINT;
typedef unsigned int	UINT;

typedef signed char		SINT8;
typedef unsigned char	UINT8;

typedef signed short	SINT16;
typedef unsigned short	UINT16;

typedef signed int		SINT32;
typedef unsigned int	UINT32;

typedef Boolean			BOOL;
typedef signed char		TCHAR;
typedef signed char		CHAR;
typedef unsigned char	BYTE;


#define	MAX_PATH	260

#define	ZeroMemory(a, b)		memset((a),  0 , (b))
#define	FillMemory(a, b, c)		memset((a), (c), (b))
#define	CopyMemory(a, b, c)		memcpy((a), (b), (c))

#define	max(a, b)				(((a)>(b))?(a):(b))
#define	min(a, b)				(((a)<(b))?(a):(b))


#include	"common.h"
#include	"macossub.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"rect.h"
#include	"lstarray.h"
#include	"trace.h"

#define	GETTICK()			macos_gettick()
#define	SPRINTF				sprintf
#define	__ASSERT(s)

#define	VERMOUTH_LIB
// #define SOUND_CRITICAL

#define	SUPPORT_SJIS

// #define	SUPPORT_8BPP
#ifdef NP2GCC
#define	SUPPORT_16BPP
#endif
// #define	SUPPORT_24BPP
#define	SUPPORT_32BPP
// #define SUPPORT_NORMALDISP
#define	MEMOPTIMIZE		1

#define	SUPPORT_CRT15KHZ
#define	SUPPORT_S98
#define	SUPPORT_SWSEEKSND
#define	SUPPORT_HOSTDRV
#define	SUPPORT_SASI
#define	SUPPORT_SCSI

#define	SOUNDRESERVE	80

#if defined(CPUCORE_IA32)
typedef signed long long	SINT64;
typedef unsigned long long  UINT64;
#define	INLINE				inline
#define FASTCALL
#define CPUCALL
#define MEMCALL
typedef UINT64  DWORD;
typedef UINT32  WORD;
void msgbox (char* text, char* buf);
#endif

#include	<windows.h>
#include	<stdio.h>
#include	<stddef.h>
#include	<SDL.h>
#include	"memmng.h"

#define	MONA
#define	BYTESEX_LITTLE
#define	OSLANG_SJIS
#define	OSLINEBREAK_CRLF


#ifndef __GNUC__
typedef	signed char		SINT8;
typedef	unsigned char	UINT8;
typedef	signed short	SINT16;
typedef	unsigned short	UINT16;
typedef	signed int		SINT32;
typedef	unsigned int	UINT32;
#else
#include	<stdlib.h>
typedef	signed char		SINT8;
typedef	unsigned char	UINT8;
typedef	signed short	SINT16;
typedef	unsigned short	UINT16;
typedef	signed int		SINT32;
#endif


#include	"common.h"
#include	"milstr.h"
#include	"codecnv.h"
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
#define	SUPPORT_SASI
#define	SUPPORT_SCSI


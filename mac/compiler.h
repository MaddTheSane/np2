//
//  compiler.h
//  Neko Project 2
//
//  Created by C.W. Betts on 6/24/16.
//  Copyright © 2016 C.W. Betts. All rights reserved.
//

#pragma once

#ifdef __OBJC__
#import <Cocoa/Cocoa.h>
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <setjmp.h>
#include <pthread.h>
#include <stdarg.h>

#define	BYTESEX_LITTLE
#define	OSLANG_UTF8
#define	OSLINEBREAK_LF
#define RESOURCE_US
#define SUPPORT_SJIS
#define NP2VER_MACOSX " by Madd the Sane"

typedef	signed int		SINT;
typedef	unsigned int	UINT;
typedef	int8_t			SINT8;
typedef	uint8_t			UINT8;
typedef	int16_t			SINT16;
typedef	uint16_t		UINT16;
typedef	int32_t			SINT32;
typedef	uint32_t		UINT32;
typedef	int64_t			SINT64;
typedef	uint64_t		UINT64;

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

#if !defined(OBJC_BOOL_DEFINED)
typedef	unsigned char	BOOL;
#endif

#ifndef	TRUE
#define	TRUE	1
#endif

#ifndef	FALSE
#define	FALSE	0
#endif

#ifndef	MAX_PATH
#define	MAX_PATH	256
#endif

#ifndef __cplusplus
#ifndef	max
#define	max(a,b)	(((a) > (b)) ? (a) : (b))
#endif
#ifndef	min
#define	min(a,b)	(((a) < (b)) ? (a) : (b))
#endif
#endif	/* __cplusplus */

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
#include "ticks.h"


#define	GETTICK()			getcurrenttime()
#define	__ASSERT(s)
#define	SPRINTF				sprintf
#define	STRLEN				strlen

#define	VERMOUTH_LIB
// #define	SOUND_CRITICAL

#define	SUPPORT_16BPP
#define	MEMOPTIMIZE		0

#define CPUCORE_IA32

#if defined(CPUCORE_IA32)
#define	SUPPORT_CRT31KHZ
#define	SUPPORT_PC9821
#define	IA32_PAGING_EACHSIZE
#endif
#define	SUPPORT_CRT15KHZ
#define	SUPPORT_S98
#define	SUPPORT_SWSEEKSND
#define	SUPPORT_HOSTDRV
#define	SUPPORT_SASI
#define	SUPPORT_SCSI
#define SUPPORT_KEYDISP
#define SUPPORT_SOFTKBD	0

#define	USE_RESUME
#define	SOUNDRESERVE	80

#if defined(OSLANG_SJIS)
#define	SUPPORT_SJIS
#elif defined(OSLANG_UTF8)
#define	SUPPORT_UTF8
#else
#define	SUPPORT_ANK
#endif

#if defined(CPUCORE_IA32)
#define FASTCALL
#define CPUCALL
#define MEMCALL
#define	SUPPORT_PC9821
#define	SUPPORT_CRT31KHZ
#define IA32_PAGING_EACHSIZE
#define	sigjmp_buf				jmp_buf
#define	sigsetjmp(env, mask)	setjmp(env)
#define	siglongjmp(env, val)	longjmp(env, val)
#define	msgbox(title, msg)		TRACEOUT(("%s", title)); \
								TRACEOUT(("%s", msg))
#endif

#define SUPPORT_EXTERNALCHIP

#define SUPPORT_ARC
#define SUPPORT_ZLIB

#define	SCREEN_BPP		16
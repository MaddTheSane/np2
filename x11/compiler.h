#ifndef	NP2_X11_COMPILER_H__
#define	NP2_X11_COMPILER_H__

#ifdef	WORDS_BIGENDIAN
#define	BYTESEX_BIG
#else	/* !WORDS_BIGENDIAN */
#define	BYTESEX_LITTLE
#endif	/* WORDS_BIGENDIAN */

#if defined(USE_NETBSDAUDIO) && !defined(__NetBSD__)
#undef	USE_NETBSDAUDIO
#endif

#if !defined(USE_NETBSDAUDIO) && !defined(USE_OSSAUDIO) && !defined(USE_ESDAUDIO) && !defined(USE_SDLAUDIO) && !defined(USE_SDLMIXER)
#ifndef	NOSOUND
#define	NOSOUND
#endif	/* !NOSOUND */
#else	/* USE_NETBSDAUDIO || USE_OSSAUDIO || USE_ESDAUDIO || USE_SDLAUDIO || USE_SDLMIXER */
#undef	NOSOUND
#endif	/* !USE_NETBSDAUDIO && !USE_OSSAUDIO && !USE_ESDAUDIO && !USE_SDLAUDIO && !USE_SDMIXER */

#if defined(NOSOUND)

#undef	VERMOUTH_LIB
#undef	SOUND_CRITICAL
#undef	SOUNDRESERVE

#else	/* !NOSOUND */

#include <pthread.h>

#define	VERMOUTH_LIB
#undef	SOUND_CRITICAL
#define	SOUNDRESERVE	100

#endif  /* NOSOUND */

#include <sys/param.h>
#include <sys/time.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

#define	X11
#define	OSLANG_EUC
#define	OSLINEBREAK_LF

typedef	signed char		CHAR;
typedef	signed short		SHORT;
typedef	signed int		INT;
typedef	signed long		LONG;

typedef	unsigned char		UCHAR;
typedef	unsigned short		USHORT;
typedef	unsigned int		UINT;
typedef	unsigned long		ULONG;

typedef	signed char		SINT8;
typedef	signed short		SINT16;
typedef	signed int		SINT32;
typedef	signed long long	SINT64;

typedef	unsigned char		UINT8;
typedef	unsigned short		UINT16;
typedef	unsigned int		UINT32;
typedef	unsigned long long	UINT64;

typedef	unsigned char		BYTE;
typedef	unsigned short		WORD;
typedef	unsigned int		DWORD;

typedef	unsigned char		TCHAR;

typedef	int			BOOL;

#ifndef	TRUE
#define	TRUE	1
#endif

#ifndef	FALSE
#define	FALSE	0
#endif

#ifndef	MAX_PATH
#define	MAX_PATH	MAXPATHLEN
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

#ifndef	UNUSED
#define	UNUSED(v)	((void)(v))
#endif

#define	FASTCALL
#define	SOUNDCALL
#define	MEMCALL
#define	CPUCALL

#ifdef	DEBUG
#define	INLINE
#define	__ASSERT(s)	assert(s)
#else
#ifndef	__ASSERT
#define	__ASSERT(s)
#endif
#ifndef	INLINE
#define	INLINE		__inline
#endif
#endif

#ifndef	NELEMENTS
#define	NELEMENTS(a)	((int)(sizeof(a) / sizeof(a[0])))
#endif

UINT32 gettick();
#define	GETTICK()	gettick()
#define	GETRAND()	random()
#define	SPRINTF		sprintf

#if defined(CPUCORE_IA32)
void toolkit_msgbox(const char *title, const char *msg);
#define	msgbox(title, msg)	toolkit_msgbox(title, msg);
#endif

#if defined(i386) || defined(__i386__)
#undef	MEMOPTIMIZE
#define LOADINTELDWORD(a)	(*((UINT32 *)(a)))
#define LOADINTELWORD(a)	(*((UINT16 *)(a)))
#define STOREINTELDWORD(a, b)	*(UINT32 *)(a) = (b)
#define STOREINTELWORD(a, b)	*(UINT16 *)(a) = (b)
#elif defined(arm) || defined (__arm__)
#define	MEMOPTIMIZE	2
#define	REG8		UINT
#define	REG16		UINT
#else
#define	MEMOPTIMIZE	1
#endif

#define	SUPPORT_EUC

#undef	SUPPORT_8BPP
#define	SUPPORT_16BPP
#define	SUPPORT_24BPP
#define	SUPPORT_32BPP
#define	SUPPORT_NORMALDISP

#define	SUPPORT_HOSTDRV

#undef	SUPPORT_SASI
#undef	SUPPORT_SCSI

#if defined(USE_GTK)
#define	SUPPORT_S98
#define	SUPPORT_KEYDISP
#endif

#if defined(USE_SDL)
#define	USE_SYSMENU
#define	SCREEN_BPP	16
#undef	SUPPORT_24BPP
#undef	SUPPORT_32BPP
#endif

/*
 * You could specify a complete path, e.g. "/etc/timidity.cfg", and
 * then specify the library directory in the configuration file.
 */
extern char timidity_cfgfile_path[MAX_PATH];
#define	TIMIDITY_CFGFILE	timidity_cfgfile_path

#include "common.h"
#include "milstr.h"
#include "_memory.h"
#include "rect.h"
#include "lstarray.h"
#include "trace.h"

#endif	/* NP2_X11_COMPILER_H__ */

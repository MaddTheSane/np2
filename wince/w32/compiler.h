#include	<windows.h>
#include	<stdio.h>
#include	<stddef.h>

#define	BYTESEX_LITTLE


typedef	short			SINT16;
typedef	unsigned short	UINT16;
typedef	int				SINT32;
typedef	unsigned int	UINT32;

#define	RGB16		UINT32

#include	"common.h"
#include	"milstr.h"
#include	"_memory.h"
#include	"rect.h"
#include	"lstarray.h"
#include	"trace.h"

#define	GETTICK()	GetTickCount()
#define	GETRAND()	rand()
#define	SPRINTF		wsprintf

#define	SIZE_QVGA

#define	VERMOUTH_LIB
#define SOUND_CRITICAL

#define	SUPPORT_16BPP
#define	MEMOPTIMIZE		2

#define	SOUNDRESERVE	100


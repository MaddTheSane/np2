
enum {
	SUCCESS		= 0,
	FAILURE		= 1
};

#ifndef PI
#define PI 3.14159265357989
#endif

#ifndef LOADINTELDWORD
#define	LOADINTELDWORD(a)		(((UINT32)(a)[0]) |				\
								((UINT32)(a)[1] << 8) |			\
								((UINT32)(a)[2] << 16) |		\
								((UINT32)(a)[3] << 24))
#endif

#ifndef LOADINTELWORD
#define	LOADINTELWORD(a)		(((UINT16)(a)[0]) | ((UINT16)(a)[1] << 8))
#endif

#ifndef STOREINTELDWORD
#define	STOREINTELDWORD(a, b)	*((a)+0) = (BYTE)((b));			\
								*((a)+1) = (BYTE)((b)>>8);		\
								*((a)+2) = (BYTE)((b)>>16);		\
								*((a)+3) = (BYTE)((b)>>24)
#endif

#ifndef STOREINTELWORD
#define	STOREINTELWORD(a, b)	*((a)+0) = (BYTE)((b));			\
								*((a)+1) = (BYTE)((b)>>8)
#endif


// ---- Optimize Macros

#ifndef REG8
#define	REG8		BYTE
#endif
#ifndef REG16
#define	REG16		UINT16
#endif

#ifndef LOW12
#define	LOW12(a)				((a) & 0x0fff)
#endif
#ifndef LOW14
#define	LOW14(a)				((a) & 0x3fff)
#endif
#ifndef LOW15
#define	LOW15(a)				((a) & 0x7fff)
#endif
#ifndef LOW16
#define	LOW16(a)				((UINT16)(a))
#endif
#ifndef HIGH16
#define	HIGH16(a)				(((UINT32)(a)) >> 16)
#endif

#ifndef STRLITERAL
#ifdef UNICODE
#define	STRLITERAL(string)		_T(string)
#else
#define	STRLITERAL(string)		(string)
#endif
#endif

#if !defined(RGB16)
#define	RGB16		UINT16
#endif

#if !defined(RGB32)
#if defined(BYTESEX_LITTLE)
typedef union {
	struct {
		UINT8	b;
		UINT8	g;
		UINT8	r;
		UINT8	e;
	} p;
	UINT32	d;
} RGB32;
#elif defined(BYTESEX_BIG)
typedef union {
	struct {
		UINT8	e;
		UINT8	r;
		UINT8	g;
		UINT8	b;
	} p;
	UINT32	d;
} RGB32;
#endif
#endif


enum {
	FTYPE_NONE = 0,		// �������� or PC
	FTYPE_SMIL,
	FTYPE_TEXT,
	FTYPE_BMP,
	FTYPE_PICT,
	FTYPE_PNG,
	FTYPE_WAV,
	FTYPE_OGG,
	FTYPE_D88,
	FTYPE_FDI,
	FTYPE_BETA,
	FTYPE_THD,
	FTYPE_NHD,
	FTYPE_HDI,
	FTYPE_HDD,
	FTYPE_S98,
	FTYPE_MIMPI,
	FTYPE_USER
};


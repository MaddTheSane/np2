
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
		BYTE	b;
		BYTE	g;
		BYTE	r;
		BYTE	e;
	} p;
	UINT32	d;
} RGB32;
#elif defined(BYTESEX_BIG)
typedef union {
	struct {
		BYTE	e;
		BYTE	r;
		BYTE	g;
		BYTE	b;
	} p;
	UINT32	d;
} RGB32;
#endif
#endif



#define RESIZE_AREAAVG
#define RESIZE_FASTCOPY

enum {
	RSZFN_8BPP		= 0,
	RSZFN_16BPP,
	RSZFN_24BPP,
	RSZFN_16BY24,
	RSZFN_24BY16,
	RSZFNMAX
};

struct _rszhdl;
typedef struct _rszhdl	_RSZHDL;
typedef struct _rszhdl	*RSZHDL;

typedef void (*RSZFN)(RSZHDL hdl, UINT type, BYTE *dst, int dalign,
												const BYTE *src, int salign);

struct _rszhdl {
	RSZFN	func;
	int		width;
	int		height;
};


#ifdef __cplusplus
extern "C" {
#endif

UINT resize_gettype(int dbpp, int sbpp);
RSZHDL resize(int xdst, int ydst, int xsrc, int ysrc);

#ifdef __cplusplus
}
#endif


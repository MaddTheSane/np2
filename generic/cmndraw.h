
#ifndef __CMNDRAW
#define __CMNDRAW


struct _cmnpalfn {
	BYTE	(*get8)(struct _cmnpalfn *fn, UINT num);
	UINT32	(*get32)(struct _cmnpalfn *fn, UINT num);
	UINT16	(*cnv16)(struct _cmnpalfn *fn, RGB32 pal32);
	long	userdata;
};
typedef struct _cmnpalfn	CMNPALFN;

typedef struct {
	UINT8	pal8;
	UINT8	padding;
	UINT16	pal16;
	RGB32	pal32;
} CMNPALS;

typedef union {
	RGB32	pal32;
	UINT16	pal16;
	UINT8	pal8;
} CMNPAL;

typedef struct {
	BYTE	*ptr;
	int		width;
	int		height;
	int		xalign;
	int		yalign;
	int		bpp;
} CMNVRAM;

typedef void (*CMNPALCNV)(CMNPAL *dst, const RGB32 *src, UINT pals, UINT bpp);

#ifdef __cplusplus
extern "C" {
#endif

void cmndraw_getpals(CMNPALFN *fn, CMNPALS *pal, UINT pals);
void cmndraw_makegrad(RGB32 *pal, int pals, RGB32 bg, RGB32 fg);

#if defined(SUPPORT_8BPP)
void cmndraw8_fill(const CMNVRAM *vram, int x, int y,
										int cx, int cy, BYTE fg);
void cmndraw8_setfg(const CMNVRAM *vram, const BYTE *src,
										int x, int y, BYTE fg);
void cmndraw8_setpat(const CMNVRAM *vram, const BYTE *src,
										int x, int y, BYTE bg, BYTE fg);
#endif

#if defined(SUPPORT_16BPP)
void cmndraw16_fill(const CMNVRAM *vram, int x, int y,
										int cx, int cy, UINT16 fg);
void cmndraw16_setfg(const CMNVRAM *vram, const BYTE *src,
										int x, int y, UINT16 fg);
void cmndraw16_setpat(const CMNVRAM *vram, const BYTE *src,
										int x, int y, UINT16 bg, UINT16 fg);
#endif

#if defined(SUPPORT_24BPP)
void cmndraw24_fill(const CMNVRAM *vram, int x, int y,
										int cx, int cy, RGB32 fg);
void cmndraw24_setfg(const CMNVRAM *vram, const BYTE *src,
										int x, int y, RGB32 fg);
void cmndraw24_setpat(const CMNVRAM *vram, const BYTE *src,
										int x, int y, RGB32 bg, RGB32 fg);
#endif

#if defined(SUPPORT_32BPP)
void cmndraw32_fill(const CMNVRAM *vram, int x, int y,
										int cx, int cy, RGB32 fg);
void cmndraw32_setfg(const CMNVRAM *vram, const BYTE *src,
										int x, int y, RGB32 fg);
void cmndraw32_setpat(const CMNVRAM *vram, const BYTE *src,
										int x, int y, RGB32 bg, RGB32 fg);
#endif


// ----

void cmddraw_fill(CMNVRAM *vram, int x, int y, int cx, int cy, CMNPALS *pal);

void cmndraw_fill2(const CMNVRAM *vram, int x, int y,
										int cx, int cy, CMNPAL fg);
void cmndraw_setfg2(const CMNVRAM *vram, const BYTE *src,
										int x, int y, CMNPAL fg);
void cmndraw_setpat2(const CMNVRAM *vram, const BYTE *src,
										int x, int y, CMNPAL bg, CMNPAL fg);
void cmddraw_text8(CMNVRAM *vram, int x, int y, const char *str, CMNPAL fg);

// ----

enum {
	CMNBMP_LEFT		= 0x00,
	CMNBMP_CENTER	= 0x01,
	CMNBMP_RIGHT	= 0x02,
	CMNBMP_TOP		= 0x00,
	CMNBMP_MIDDLE	= 0x04,
	CMNBMP_BOTTOM	= 0x08
};

typedef struct {
	BYTE	*ptr;
	int		width;
	int		height;
	int		align;
	UINT	pals;
	RGB32	paltbl[16];
} CMNBMP;

BOOL cmndraw_bmp4inf(CMNBMP *bmp, const void *ptr);
void cmndraw_bmp16(CMNVRAM *vram, const void *ptr, CMNPALCNV cnv, UINT flag);

#ifdef __cplusplus
}
#endif

#endif


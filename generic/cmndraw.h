
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
} CMNPAL;

typedef struct {
	BYTE	*ptr;
	int		width;
	int		height;
	int		xalign;
	int		yalign;
	int		bpp;
} CMNVRAM;

#ifdef __cplusplus
extern "C" {
#endif

void cmndraw_getpals(CMNPALFN *fn, CMNPAL *pal, UINT pals);
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

void cmddraw_fill(CMNVRAM *vram, int x, int y, int cx, int cy, CMNPAL *pal);

#ifdef __cplusplus
}
#endif

#endif


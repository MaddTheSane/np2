#include	"compiler.h"
// #include	<sys/time.h>
// #include	<signal.h>
// #include	<unistd.h>
#include	"scrnmng.h"
#include	"scrndraw.h"
#include	"vramhdl.h"
#include	"menubase.h"


typedef struct {
	BOOL		enable;
	int			width;
	int			height;
	int			bpp;
	SDL_Surface	*surface;
	VRAMHDL		vram;
} SCRNMNG;

typedef struct {
	int		width;
	int		height;
} SCRNSTAT;

static const char app_name[] = "Neko Project II";

static	SCRNMNG		scrnmng;
static	SCRNSTAT	scrnstat;
static	SCRNSURF	scrnsurf;

typedef struct {
	int		xalign;
	int		yalign;
	int		width;
	int		height;
	int		srcpos;
	int		dstpos;
} DRAWRECT;

static BOOL calcdrawrect(SDL_Surface *surface,
								DRAWRECT *dr, VRAMHDL s, const RECT_T *rt) {

	int		pos;

	dr->xalign = surface->format->BytesPerPixel;
	dr->yalign = surface->pitch;
	dr->srcpos = 0;
	dr->dstpos = 0;
	dr->width = min(scrnmng.width, s->width);
	dr->height = min(scrnmng.height, s->height);
	if (rt) {
		pos = max(rt->left, 0);
		dr->srcpos += pos;
		dr->dstpos += pos * dr->xalign;
		dr->width = min(rt->right, dr->width) - pos;

		pos = max(rt->top, 0);
		dr->srcpos += pos * s->width;
		dr->dstpos += pos * dr->yalign;
		dr->height = min(rt->bottom, dr->height) - pos;
	}
	if ((dr->width <= 0) || (dr->height <= 0)) {
		return(FAILURE);
	}
	return(SUCCESS);
}


void scrnmng_initialize(void) {

	scrnstat.width = 640;
	scrnstat.height = 400;
}

BOOL scrnmng_create(int width, int height) {

	char			s[256];
const SDL_VideoInfo	*vinfo;
	SDL_Surface		*surface;
	SDL_PixelFormat	*fmt;
	BOOL			r;

	if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "Error: SDL_Init: %s\n", SDL_GetError());
		return(FAILURE);
	}
	SDL_WM_SetCaption(app_name, app_name);
	vinfo = SDL_GetVideoInfo();
	if (vinfo == NULL) {
		fprintf(stderr, "Error: SDL_GetVideoInfo: %s\n", SDL_GetError());
		return(FAILURE);
	}
	SDL_VideoDriverName(s, sizeof(s));

	surface = SDL_SetVideoMode(width, height, vinfo->vfmt->BitsPerPixel,
		    SDL_HWSURFACE | SDL_ANYFORMAT | SDL_DOUBLEBUF | SDL_FULLSCREEN);
	if (surface == NULL) {
		fprintf(stderr, "Error: SDL_SetVideoMode: %s\n", SDL_GetError());
		return(FAILURE);
	}

	r = FALSE;
	fmt = surface->format;
#if defined(SUPPORT_8BPP)
	if (fmt->BitsPerPixel == 8) {
		r = TRUE;
	}
#endif
#if defined(SUPPORT_16BPP)
	if ((fmt->BitsPerPixel == 16) && (fmt->Rmask == 0xf800) &&
		(fmt->Gmask == 0x07e0) && (fmt->Bmask == 0x001f)) {
		r = TRUE;
	}
#endif
#if defined(SUPPORT_24BPP)
	if (fmt->BitsPerPixel == 24) {
		r = TRUE;
	}
#endif
#if defined(SUPPORT_32BPP)
	if (fmt->BitsPerPixel == 32) {
		r = TRUE;
	}
#endif
#if defined(SCREEN_BPP)
	if (fmt->BitsPerPixel != SCREEN_BPP) {
		r = FALSE;
	}
#endif
	if (r) {
		scrnmng.enable = TRUE;
		scrnmng.width = width;
		scrnmng.height = height;
		scrnmng.bpp = fmt->BitsPerPixel;
		return(SUCCESS);
	}
	else {
		fprintf(stderr, "Error: Bad screen mode");
		return(FAILURE);
	}
}

void scrnmng_destroy(void) {

	scrnmng.enable = FALSE;
}

RGB16 scrnmng_makepal16(RGB32 pal32) {

	RGB16	ret;

	ret = (pal32.p.r & 0xf8) << 8;
#if defined(SIZE_QVGA)
	ret += (pal32.p.g & 0xfc) << (3 + 16);
#else
	ret += (pal32.p.g & 0xfc) << 3;
#endif
	ret += pal32.p.b >> 3;
	return(ret);
}

void scrnmng_setwidth(int posx, int width) {

	scrnstat.width = width;
}

void scrnmng_setheight(int posy, int height) {

	scrnstat.height = height;
}

const SCRNSURF *scrnmng_surflock(void) {

	SDL_Surface	*surface;

	if (scrnmng.vram == NULL) {
		surface = SDL_GetVideoSurface();
		if (surface == NULL) {
			return(NULL);
		}
		SDL_LockSurface(surface);
		scrnmng.surface = surface;
		scrnsurf.ptr = (BYTE *)surface->pixels;
		scrnsurf.xalign = surface->format->BytesPerPixel;
		scrnsurf.yalign = surface->pitch;
		scrnsurf.bpp = surface->format->BitsPerPixel;
	}
	else {
		scrnsurf.ptr = scrnmng.vram->ptr;
		scrnsurf.xalign = scrnmng.vram->xalign;
		scrnsurf.yalign = scrnmng.vram->yalign;
		scrnsurf.bpp = scrnmng.vram->bpp;
	}
	scrnsurf.width = min(scrnstat.width, 640);
	scrnsurf.height = min(scrnstat.height, 400);
	scrnsurf.extend = 0;
	return(&scrnsurf);
}

static void draw_onmenu(void) {

	RECT_T		rt;
	SDL_Surface	*surface;
	DRAWRECT	dr;
const BYTE		*p;
	BYTE		*q;
const BYTE		*a;
	int			salign;
	int			dalign;
	int			x;

	rt.left = 0;
	rt.top = 0;
	rt.right = min(scrnstat.width, 640);
	rt.bottom = min(scrnstat.height, 400);
#if defined(SIZE_QVGA)
	rt.right >>= 1;
	rt.bottom >>= 1;
#endif

	surface = SDL_GetVideoSurface();
	if (surface == NULL) {
		return;
	}
	SDL_LockSurface(surface);
	if (calcdrawrect(surface, &dr, menuvram, &rt) == SUCCESS) {
		if (scrnmng.bpp == 16) {
			p = scrnmng.vram->ptr + (dr.srcpos * 2);
			q = (BYTE *)surface->pixels + dr.dstpos;
			a = menuvram->alpha + dr.srcpos;
			salign = menuvram->width;
			dalign = dr.yalign - (dr.width * dr.xalign);
			do {
				x = 0;
				do {
					if (a[x] == 0) {
						*(UINT16 *)q = *(UINT16 *)(p + (x * 2));
					}
					q += dr.xalign;
				} while(++x < dr.width);
				p += salign * 2;
				q += dalign;
				a += salign;
			} while(--dr.height);
		}
	}
	SDL_UnlockSurface(surface);
	SDL_Flip(surface);
}

void scrnmng_surfunlock(const SCRNSURF *surf) {

	SDL_Surface	*surface;

	if (surf) {
		if (scrnmng.vram == NULL) {
			if (scrnmng.surface != NULL) {
				surface = scrnmng.surface;
				scrnmng.surface = NULL;
				SDL_UnlockSurface(surface);
				SDL_Flip(surface);
			}
		}
		else {
			if (menuvram) {
				draw_onmenu();
			}
		}
	}
}


// ----

BOOL scrnmng_entermenu(SCRNMENU *smenu) {

	int		bpp;

	if (smenu == NULL) {
		goto smem_err;
	}
	bpp = scrnmng.bpp;
	if (bpp == 32) {
		bpp = 24;
	}
	vram_destroy(scrnmng.vram);
	scrnmng.vram = vram_create(scrnmng.width, scrnmng.height, FALSE, bpp);
	if (scrnmng.vram == NULL) {
		goto smem_err;
	}
	scrndraw_redraw();
	smenu->width = scrnmng.width;
	smenu->height = scrnmng.height;
	smenu->bpp = bpp;
	return(SUCCESS);

smem_err:
	return(FAILURE);
}

void scrnmng_leavemenu(void) {

	VRAM_RELEASE(scrnmng.vram);
}

void scrnmng_menudraw(const RECT_T *rct) {

	SDL_Surface	*surface;
	DRAWRECT	dr;
const BYTE		*p;
const BYTE		*q;
	BYTE		*r;
	BYTE		*a;
	int			salign;
	int			dalign;
	int			x;

	if ((!scrnmng.enable) && (menuvram == NULL)) {
		return;
	}
	surface = SDL_GetVideoSurface();
	if (surface == NULL) {
		return;
	}
	SDL_LockSurface(surface);
	if (calcdrawrect(surface, &dr, menuvram, rct) == SUCCESS) {
		if (scrnmng.bpp == 16) {
			p = scrnmng.vram->ptr + (dr.srcpos * 2);
			q = menuvram->ptr + (dr.srcpos * 2);
			r = (BYTE *)surface->pixels + dr.dstpos;
			a = menuvram->alpha + dr.srcpos;
			salign = menuvram->width;
			dalign = dr.yalign - (dr.width * dr.xalign);
			do {
				x = 0;
				do {
					if (a[x]) {
						if (a[x] & 2) {
							*(UINT16 *)r = *(UINT16 *)(q + (x * 2));
						}
						else {
							a[x] = 0;
							*(UINT16 *)r = *(UINT16 *)(p + (x * 2));
						}
					}
					r += dr.xalign;
				} while(++x < dr.width);
				p += salign * 2;
				q += salign * 2;
				r += dalign;
				a += salign;
			} while(--dr.height);
		}
	}
	SDL_UnlockSurface(surface);
	SDL_Flip(surface);
}


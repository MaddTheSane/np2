/*	$Id: x11_screen.c,v 1.1 2004/02/06 16:52:48 monaka Exp $	*/

/*
 * Copyright (c) 2004 NONAKA Kimihiro
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "compiler.h"

#include "np2.h"
#include "palettes.h"
#include "scrndraw.h"
#include "vramhdl.h"
#include "menubase.h"

#include "drawmng.h"
#include "scrnmng.h"

#include "x11/xnp2.h"

#if defined(MITSHM)
#include <X11/extensions/XShm.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif


typedef struct {
	BYTE		scrnmode;
	int		width;
	int		height;
	int		extend;
	int		clipping;

	PAL16MASK	pal16mask;

	RECT_T		scrn;
	RECT_T		rect;

	int		lpitch;
	int		scrw;
	int		scrh;

	int		shared_pixmap;

	/* toolkit depend */
	int		shared_memory;

	XImage		*surface;
	Pixmap		backsurf;

	unsigned long	pixel[24];	/* pallete */

#if defined(MITSHM)
	XShmSegmentInfo	shminfo;
#endif
} DRAWMNG;

typedef struct {
	int	width;
	int	height;
	int	extend;
} SCRNSTAT;

typedef struct {
	SCRNSURF	ss;

	BYTE		renewal[SURFACE_HEIGHT];
} X11SCRNSURF;

static SCRNMNG scrnmng;
static DRAWMNG drawmng;
static SCRNSTAT scrnstat;
static X11SCRNSURF scrnsurf;

SCRNMNG *scrnmngp = &scrnmng;

static XColor textcolor[] = {
	{ 0, 0x00, 0x00, 0x00 },	/* black */
	{ 0, 0x00, 0x00, 0xff },	/* blue */
	{ 0, 0xff, 0x00, 0x00 },	/* red */
	{ 0, 0xff, 0x00, 0xff },	/* magenda */
	{ 0, 0x00, 0xff, 0x00 },	/* green */
	{ 0, 0x00, 0xff, 0xff },	/* cyan */
	{ 0, 0xff, 0xff, 0x00 },	/* yellow */
	{ 0, 0xff, 0xff, 0xff }		/* white */
};
#define	NTEXTCOLOR	NELEMENTS(textcolor)
#define	NGRPHCOLOR	16


static void
set_window_size(int width, int height)
{

	drawmng.scrw = width + np2oscfg.paddingx * 2;
	drawmng.scrh = height + np2oscfg.paddingy * 2;

	XResizeWindow(display, window, drawmng.scrw, drawmng.scrh);
}

static void
renewal_client_size(void)
{
	DRAWMNG *d = &drawmng;
	int width;
	int height;
	int extend;
	int scrnwidth;
	int scrnheight;

	width = min(scrnstat.width, d->width);
	height = min(scrnstat.height, d->height);
	extend = 0;

#if notyet
	if (d->scrnmode & SCRNMODE_FULLSCREEN) {
	} else
#endif
	{
		if (!(d->scrnmode & SCRNMODE_ROTATE)) {
			if (np2oscfg.paddingx > 0) {
				extend = min(scrnstat.extend, d->extend);
			}
			scrnwidth = width;
			scrnheight = height;
			d->rect.right = width + extend;
			d->rect.bottom = height;
			d->scrn.left = np2oscfg.paddingx - extend;
			d->scrn.top = np2oscfg.paddingy;
		} else {
			if (np2oscfg.paddingy > 0) {
				extend = min(scrnstat.extend, d->extend);
			}
			scrnwidth = height;
			scrnheight = width;
			d->rect.right = height;
			d->rect.bottom = width + extend;
			d->scrn.left = np2oscfg.paddingx;
			d->scrn.top = np2oscfg.paddingy - extend;
		}
		d->scrn.right = np2oscfg.paddingx + scrnwidth;
		d->scrn.bottom = np2oscfg.paddingy + scrnheight;
		set_window_size(scrnwidth, scrnheight);
	}
	scrnsurf.ss.width = width;
	scrnsurf.ss.height = height;
	scrnsurf.ss.extend = extend;
}

void
scrnmng_initialize(void)
{

	scrnstat.width = 640;
	scrnstat.height = 400;
	scrnstat.extend = 1;
	set_window_size(scrnstat.width, scrnstat.height);
}

static void
clear_out_of_rect(const RECT_T *target, const RECT_T *base)
{
	RECT_T rect;

	rect.left = base->left;
	rect.right = base->right;
	rect.top = base->top;
	rect.bottom = target->top;
	if (rect.top < rect.bottom) {
		XFillRectangle(display, window, gc,
		    rect.left, rect.top, rect.right, rect.bottom);
	}
	rect.top = target->bottom;
	rect.bottom = base->bottom;
	if (rect.top < rect.bottom) {
		XFillRectangle(display, window, gc,
		    rect.left, rect.top, rect.right, rect.bottom);
	}

	rect.top = max(base->top, target->top);
	rect.bottom = min(base->bottom, target->bottom);
	if (rect.top < rect.bottom) {
		rect.left = base->left;
		rect.right = target->left;
		if (rect.left < rect.right) {
			XFillRectangle(display, window, gc,
			    rect.left, rect.top, rect.right, rect.bottom);
		}
		rect.left = target->right;
		rect.right = base->right;
		if (rect.left < rect.right) {
			XFillRectangle(display, window, gc,
			    rect.left, rect.top, rect.right, rect.bottom);
		}
	}
}

static void
clear_outscreen(void)
{
	RECT_T target;
	RECT_T base;

	base.left = base.top = 0;
	base.right = drawmng.scrw;
	base.bottom = drawmng.scrh;
	target = drawmng.scrn;
	clear_out_of_rect(&target, &base);
}

static void
palette_init(void)
{
	DRAWMNG *d = &drawmng;
	Visual *visual;
	Colormap cmap;
	XColor xc;
	int i;

	visual = DefaultVisual(display, screen);
	cmap = XDefaultColormap(display, screen);

	if (visual->class == PseudoColor) {
		/* Allocate Text colormap */
		if (XAllocColorCells(display, cmap, True, NULL, 0,
		    d->pixel, NELEMENTS(d->pixel)) == 0) {
			fprintf(stderr, "Can't allocate enough color.\n");
			return;
		}

		for (i = 0; i < NTEXTCOLOR; i++) {
			xc.pixel = d->pixel[i];
			xc.red = textcolor[i].red << 8;
			xc.green = textcolor[i].green << 8;
			xc.blue = textcolor[i].blue << 8;
			xc.flags = DoRed | DoGreen | DoBlue;
			XStoreColor(display, cmap, &xc);
		}
	}
}

static void
palette_set(void)
{
	Colormap cmap;
	XColor xc;
	int i;

	cmap = XDefaultColormap(display, screen);

	for (i = 0; i < NGRPHCOLOR; i++) {
		xc.pixel = drawmng.pixel[NTEXTCOLOR + i];
		xc.red = np2_pal32[NP2PAL_GRPH + i].p.r << 8;
		xc.green = np2_pal32[NP2PAL_GRPH + i].p.g << 8;
		xc.blue = np2_pal32[NP2PAL_GRPH + i].p.b << 8;
		xc.flags = DoRed | DoGreen | DoBlue;
		XStoreColor(display, cmap, &xc);
	}
}

BOOL
scrnmng_create(BYTE mode)
{
	DRAWMNG *d = &drawmng;
	Visual *visual;
	char *mem;
	RECT_T rect;
	int height;
	int depth;
	int bitcolor;
	int bitmap_pad;
	UINT lpitch;
	BYTE bytes_per_pixel;
#if defined(MITSHM)
	int major, minor;
#endif

#if notyet
	if (mode & SCRNMODE_FULLSCREEN) {
		mode &= ~SCRNMODE_ROTATEMASK;
		d->extend = 0;
		return FAILURE;
	} else
#endif
	{
		scrnmng.flag = SCRNFLAG_HAVEEXTEND;

		visual = DefaultVisual(display, screen);
		switch (visual->class) {
		case TrueColor:
		case PseudoColor:
		case DirectColor:
			break;

		default:
			fprintf(stderr, "No support visual class.\n");
			return FAILURE;
		}

		depth = DefaultDepth(display, screen);
		if (depth == 32) {
			bitcolor = 32;
		} else if (depth == 24) {
			XPixmapFormatValues *format;
			int count;
			int i;

			format = XListPixmapFormats(display, &count);
			if (format) {
				for (i = 0; i < count; i++) {
					if (format[i].depth != 24)
						continue;

					if (format[i].bits_per_pixel == 32) {
						bitcolor = 32;
					} else {
						bitcolor = 24;
					}
					break;
				}
				XFree(format);

				if (i == count) {
					fprintf(stderr, "24 bits depth not support?\n");
					return FAILURE;
				}
			} else {
				fprintf(stderr, "Can't get PixmapFormats.\n");
				return FAILURE;
			}
		} else if (depth == 15 || depth == 16) {
			bitcolor = 16;
		} else if (depth == 8) {
			bitcolor = 8;
		} else if (depth < 8) {
			fprintf(stderr, "Too few allocable color.\n");
			return FAILURE;
		} else {
			fprintf(stderr, "No support depth.\n");
			return FAILURE;
		}

		if (bitcolor == 16) {
			drawmng_make16mask(&d->pal16mask, visual->blue_mask, visual->red_mask, visual->green_mask);
		} else if (bitcolor == 8) {
			palette_init();
		}
		d->extend = 1;
		bytes_per_pixel = bitcolor >> 3;

		if (depth <= 8) {
			bitmap_pad = 8;
		} else if (depth <= 16) {
			bitmap_pad = 16;
		} else {
			bitmap_pad = 32;
		}

		if (!(mode & SCRNMODE_ROTATE)) {
			rect.right = 641;
			rect.bottom = 480;
			lpitch = rect.right * bytes_per_pixel;
			if (lpitch % 4) {
				rect.right += (lpitch % 4) / bytes_per_pixel;
				lpitch = rect.right * bytes_per_pixel;
			}
		} else {
			rect.right = 480;
			rect.bottom = 641;
			lpitch = rect.right * bytes_per_pixel;
		}
		height = 480;

#if defined(MITSHM)
		d->shared_pixmap = use_shared_pixmap;

		d->shared_memory = XShmQueryExtension(display);
		if (d->shared_memory && d->shared_pixmap) {
			XShmQueryVersion(display, &major, &minor, &d->shared_pixmap);
		}

restart:
		if (d->shared_memory) {
			d->surface = XShmCreateImage(display, visual,
			    depth, ZPixmap, NULL, &d->shminfo,
			    rect.right, rect.bottom);
			if (drawmng.surface == NULL) {
				fprintf(stderr, "Can't create surface.\n");
				d->shared_memory = 0;
				d->shared_pixmap = 0;
				goto restart;
			}

			d->shminfo.shmid = shmget(IPC_PRIVATE,
			    d->surface->bytes_per_line * d->surface->height,
			    IPC_CREAT | 0777);
			if (d->shminfo.shmid < 0) {
				fprintf(stderr, "shmget: size:%x %s\n",
				    d->surface->bytes_per_line * d->surface->height, strerror(errno));
				XDestroyImage(d->surface);
				d->surface = 0;
				d->shared_memory = 0;
				d->shared_pixmap = 0;
				goto restart;
			}

			d->shminfo.shmaddr = shmat(d->shminfo.shmid, 0, 0);
			if (d->shminfo.shmaddr == NULL) {
				perror("shmat");
				shmctl(d->shminfo.shmid, IPC_RMID, NULL);
				XDestroyImage(d->surface);
				d->surface = 0;
				d->shared_memory = 0;
				d->shared_pixmap = 0;
				goto restart;
			}
			d->surface->data = d->shminfo.shmaddr;

			d->shminfo.readOnly = False;
			if (!XShmAttach(display, &d->shminfo)) {
				fprintf(stderr, "Can't attach shared memory\n");
				shmdt(d->shminfo.shmaddr);
				shmctl(d->shminfo.shmid, IPC_RMID, NULL);
				d->surface->data = 0;
				XDestroyImage(d->surface);
				d->surface = 0;
				d->shared_memory = 0;
				d->shared_pixmap = 0;
				goto restart;
			}

			if (d->shared_pixmap) {
				d->backsurf = XShmCreatePixmap(display, window,
				    d->surface->data, &d->shminfo,
				    d->surface->width, d->surface->height,
				    d->surface->depth);
			}
			if (d->backsurf == None) {
				d->shared_pixmap = 0;
				d->backsurf = XCreatePixmap(display, window,
				    d->surface->width, d->surface->height,
				    d->surface->depth);
			}
			if (d->backsurf == None) {
				fprintf(stderr, "Can't create pixmap\n");
				shmdt(d->shminfo.shmaddr);
				shmctl(d->shminfo.shmid, IPC_RMID, NULL);
				d->surface->data = 0;
				XDestroyImage(d->surface);
				d->surface = 0;
				d->shared_memory = 0;
				d->shared_pixmap = 0;
				goto restart;
			}
		} else
#endif
		{
			d->surface = XCreateImage(display, visual, depth,
			    ZPixmap, 0, NULL, rect.right, rect.bottom,
			    bitmap_pad, 0);
			if (d->surface == NULL) {
				fprintf(stderr, "Can't create surface.\n");
				return FAILURE;
			}

			mem = (char *)malloc(d->surface->width * d->surface->height * d->surface->bits_per_pixel / 8);
			if (mem == NULL) {
				fprintf(stderr, "Can't allocate surface memory.\n");
				XDestroyImage(d->surface);
				d->surface = NULL;
				return FAILURE;
			}
			d->surface->data = mem;

			d->backsurf = XCreatePixmap(display, window,
			    d->surface->width, d->surface->height,
			    d->surface->depth);
			if (drawmng.backsurf == None) {
				fprintf(stderr, "Can't create backsurf.\n");
				XDestroyImage(d->surface);
				drawmng.surface = NULL;
				return FAILURE;
			}
		}
		XFillRectangle(display, d->backsurf, gc, 0, 0,
		    rect.right, rect.bottom);
	}
	scrnmng.bpp = d->surface->bits_per_pixel;
	drawmng.lpitch = d->surface->bytes_per_line;
	scrnsurf.ss.bpp = scrnmng.bpp;
	drawmng.scrnmode = mode;
	drawmng.width = 640;
	drawmng.height = height;
	drawmng.clipping = 0;
	renewal_client_size();

	return SUCCESS;
}

void
scrnmng_destroy(void)
{
	DRAWMNG *d = &drawmng;
	Visual *visual;
	Colormap cmap;

	visual = DefaultVisual(display, screen);
	cmap = XDefaultColormap(display, screen);

	if (visual->class == PseudoColor) {
		XFreeColors(display, cmap, d->pixel, 24, 0);
	}
	if (d->backsurf != None) {
		XFreePixmap(display, d->backsurf);
		d->backsurf = None;
	}
	if (d->shared_memory) {
		XShmDetach(display, &d->shminfo);
		if (d->surface) {
			d->surface = NULL;
		}
		shmdt(d->shminfo.shmaddr);
		shmctl(d->shminfo.shmid, IPC_RMID, NULL);
	}
	if (d->surface) {
		XDestroyImage(d->surface);
		d->surface = NULL;
	}
}

RGB16
scrnmng_makepal16(RGB32 pal32)
{

	return drawmng_makepal16(&drawmng.pal16mask, pal32);
}

void
scrnmng_setwidth(int posx, int width)
{

	UNUSED(posx);

	scrnstat.width = width;
	renewal_client_size();
}

void
scrnmng_setheight(int posy, int height)
{

	UNUSED(posy);

	scrnstat.height = height;
	renewal_client_size();
}

void
scrnmng_setextend(int extend)
{

	scrnstat.extend = extend;
	renewal_client_size();
}

const SCRNSURF *
scrnmng_surflock(void)
{
	DRAWMNG *d = &drawmng;
	int bytes_per_pixel = scrnsurf.ss.bpp >> 3;
	int lpitch = d->lpitch;

	CopyMemory(scrnsurf.renewal, renewal_line, sizeof(scrnsurf.renewal));

	scrnsurf.ss.ptr = (BYTE *)d->surface->data;
	if (!(d->scrnmode & SCRNMODE_ROTATE)) {
		scrnsurf.ss.xalign = bytes_per_pixel;
		scrnsurf.ss.yalign = lpitch;
	} else if (!(drawmng.scrnmode & SCRNMODE_ROTATEDIR)) {
		/* rotate left */
		scrnsurf.ss.ptr += (scrnsurf.ss.width + scrnsurf.ss.extend - 1) * lpitch;
		scrnsurf.ss.xalign = -lpitch;
		scrnsurf.ss.yalign = bytes_per_pixel;
	} else {
		/* rotate right */
		scrnsurf.ss.ptr += (d->rect.right - 1) * bytes_per_pixel;
		scrnsurf.ss.xalign = lpitch;
		scrnsurf.ss.yalign = -bytes_per_pixel;
	}
	return &scrnsurf.ss;
}


#define	DRAW(d, sx, sy, dx, dy, w, h) \
do { \
	if ((d)->shared_memory) { \
		XShmPutImage(display, (d)->backsurf, gc, (d)->surface, \
		    (sx), (sy), (dx), (dy), (w), (h), False); \
	} else { \
		XPutImage(display, (d)->backsurf, gc, (d)->surface, \
		    (sx), (sy), (dx), (dy), (w), (h)); \
	} \
} while (/*CONSTCOND*/ 0)

void
scrnmng_surfunlock(const SCRNSURF *surf)
{
	DRAWMNG *d = &drawmng;

	if (!d->shared_pixmap) {
		X11SCRNSURF *ss = (X11SCRNSURF *)surf;
		BYTE *delta = ss->renewal;
		RECT_T r;
		int h, s;

		r.left = d->scrn.left;
		r.top = d->scrn.top;
		r.right = d->rect.right;
		r.bottom = d->rect.bottom;

		if (!(d->scrnmode & SCRNMODE_ROTATE)) {
			/* normal */
			for (s = h = 0; h < r.bottom; h++) {
				if ((renewal_line[h] != delta[h]) || (s == h))
					continue;

				DRAW(d, 0, s, r.left, r.top+s, r.right, h-s);
				s = h + 1;
			}
			if (s != h) {
				DRAW(d, 0, s, r.left, r.top+s, r.right, h-s);
			}
		} else if (!(d->scrnmode & SCRNMODE_ROTATEDIR)) {
			/* rotate left */
			for (s = h = 0; h < r.right; h++) {
				if ((renewal_line[h] != delta[h]) || (s == h))
					continue;
				DRAW(d, s, 0, r.left+s, r.top, h-s, r.bottom);
				s = h + 1;
			}
			if (s != h) {
				DRAW(d, s, 0, r.left+s, r.top, h-s, r.bottom);
			}
		} else {
			/* rotate right */
			for (s = h = 0; h < r.right; h++) {
				if ((renewal_line[h] != delta[h]) || (s == h))
					continue;
				DRAW(d, r.right-h, 0, d->scrn.right-h, r.top, h-s, r.bottom);
				s = h + 1;
			}
			if (s != h) {
				DRAW(d, r.right-h, 0, d->scrn.right-h, r.top, h-s, r.bottom);
			}
		}
	}

	if (scrnmng.palchanged) {
		scrnmng.palchanged = FALSE;
		palette_set();
	}

#if notyet
	if (drawmng.scrnmode & SCRNMODE_FULLSCREEN) {
	} else
#endif
	{
		if (scrnmng.allflash) {
			scrnmng.allflash = 0;
			if (np2oscfg.paddingx || np2oscfg.paddingy) {
				clear_outscreen();
			}
		}
		XCopyArea(display, d->backsurf, window, gc,
		    0, 0, d->rect.right, d->rect.bottom,
		    d->scrn.left, d->scrn.top);
		XSync(display, False);
	}
}
#undef DRAW

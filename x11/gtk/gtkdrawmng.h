/*
 * Copyright (c) 2003 NONAKA Kimihiro
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

#ifndef	NP2_X11_GTK_GTKDRAWMNG_H__
#define	NP2_X11_GTK_GTKDRAWMNG_H__

#include "compiler.h"

#include "cmndraw.h"

#include "gtk/xnp2.h"

typedef struct {
	RGB32	mask;
	BYTE	r16b;
	BYTE	l16r;
	BYTE	l16g;
} PAL16MASK;

void drawmng_make16mask(PAL16MASK *pal16, UINT32 bmask, UINT32 rmask, UINT32 gmask);
RGB16 drawmng_makepal16(PAL16MASK *pal16, RGB32 pal32);

typedef struct {
	CMNVRAM		vram;

	int		width;
	int		height;
	int		lpitch;

	RECT_T		src;
	POINT_T		dest;

	PAL16MASK	pal16mask;

	GtkWidget	*drawarea;
	GdkImage	*surface;
	GdkPixmap	*backsurf;
	BOOL		shared_pixmap;
} _GTKDRAWMNG_HDL, *GTKDRAWMNG_HDL;

GTKDRAWMNG_HDL gtkdrawmng_create(GtkWidget *parent, int width, int height);
void gtkdrawmng_release(GTKDRAWMNG_HDL hdl);
CMNVRAM *gtkdrawmng_surflock(GTKDRAWMNG_HDL hdl);
void gtkdrawmng_surfunlock(GTKDRAWMNG_HDL hdl);
void gtkdrawmng_blt(GTKDRAWMNG_HDL hdl, RECT_T *sr, POINT_T *dp);
void gtkdrawmng_set_size(GTKDRAWMNG_HDL hdl, int width, int height);

int gtkdrawmng_getbpp(GtkWidget *w, GtkWidget *parent_window);

#endif	/* NP2_X11_GTK_GTKDRAWMNG_H__ */

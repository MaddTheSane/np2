#include	"compiler.h"
#include	"scrnmng.h"
#include	"scrndraw.h"
#include	"sdraw.h"
#include	"palettes.h"


#if defined(SIZE_QVGA) && defined(SUPPORT_16BPP)

// vram off
static void SCRNCALL qvga16p_0(SDRAW sdraw, int maxy) {

	int		xbytes;
	UINT32	palwork;
	UINT16	pal;
	BYTE	*p;
	int		y;
	int		x;

	xbytes = sdraw->xalign * sdraw->width / 2;
	palwork = np2_pal16[NP2PAL_TEXT2];
	pal = (UINT16)(palwork + (palwork >> 16));
	p = sdraw->dst;
	y = sdraw->y;
	do {
		if (*(UINT16 *)(sdraw->dirty + y)) {
			for (x=0; x<sdraw->width; x+=2) {
				*(UINT16 *)p = pal;
				p += sdraw->xalign;
			}
			p -= xbytes;
		}
		p += sdraw->yalign;
		y += 2;
	} while(y < maxy);

	sdraw->dst = p;
	sdraw->y = y;
}

// text or grph 1ƒvƒŒ[ƒ“
static void SCRNCALL qvga16p_1(SDRAW sdraw, int maxy) {

	int		xbytes;
const BYTE	*p;
	BYTE	*q;
	int		y;
	int		x;
	UINT32	work;

	xbytes = sdraw->xalign * sdraw->width / 2;
	p = sdraw->src;
	q = sdraw->dst;
	y = sdraw->y;
	do {
		if (*(UINT16 *)(sdraw->dirty + y)) {
			for (x=0; x<sdraw->width; x+=2) {
				work = np2_pal16[p[x+0] + NP2PAL_GRPH];
				work += np2_pal16[p[x+1] + NP2PAL_GRPH];
				work += np2_pal16[p[x+0+SURFACE_WIDTH] + NP2PAL_GRPH];
				work += np2_pal16[p[x+1+SURFACE_WIDTH] + NP2PAL_GRPH];
				work &= 0x07e0f81f << 2;
				*(UINT16 *)q = (UINT16)((work >> 2) + (work >> 18));
				q += sdraw->xalign;
			}
			q -= xbytes;
		}
		p += SURFACE_WIDTH * 2;
		q += sdraw->yalign;
		y += 2;
	} while(y < maxy);

	sdraw->src = p;
	sdraw->dst = q;
	sdraw->y = y;
}

// text + grph
static void SCRNCALL qvga16p_2(SDRAW sdraw, int maxy) {

	int		xbytes;
const BYTE	*p;
const BYTE	*q;
	BYTE	*r;
	int		y;
	int		x;
	UINT32	work;

	xbytes = sdraw->xalign * sdraw->width / 2;
	p = sdraw->src;
	q = sdraw->src2;
	r = sdraw->dst;
	y = sdraw->y;
	do {
		if (*(UINT16 *)(sdraw->dirty + y)) {
			for (x=0; x<sdraw->width; x+=2) {
				work = np2_pal16[p[x+0] + q[x+0] + NP2PAL_GRPH];
				work += np2_pal16[p[x+1] + q[x+1] + NP2PAL_GRPH];
				work += np2_pal16[p[x+0+SURFACE_WIDTH] +
										q[x+0+SURFACE_WIDTH] + NP2PAL_GRPH];
				work += np2_pal16[p[x+1+SURFACE_WIDTH] + 
										q[x+1+SURFACE_WIDTH] + NP2PAL_GRPH];
				work &= 0x07e0f81f << 2;
				*(UINT16 *)r = (UINT16)((work >> 2) + (work >> 18));
				r += sdraw->xalign;
			}
			r -= xbytes;
		}
		p += SURFACE_WIDTH * 2;
		q += SURFACE_WIDTH * 2;
		r += sdraw->yalign;
		y += 2;
	} while(y < maxy);

	sdraw->src = p;
	sdraw->src2 = q;
	sdraw->dst = r;
	sdraw->y = y;
}

// text + (grph:interleave) - > qvga16p_1

// grph:interleave
static void SCRNCALL qvga16p_gi(SDRAW sdraw, int maxy) {

	int		xbytes;
const BYTE	*p;
	BYTE	*q;
	int		y;
	int		x;
	UINT32	work;

	xbytes = sdraw->xalign * sdraw->width / 2;
	p = sdraw->src;
	q = sdraw->dst;
	y = sdraw->y;
	do {
		if (*(UINT16 *)(sdraw->dirty + y)) {
			for (x=0; x<sdraw->width; x+=2) {
				work = np2_pal16[p[x+0] + NP2PAL_GRPH];
				work += np2_pal16[p[x+1] + NP2PAL_GRPH];
				work &= 0x07e0f81f << 1;
				*(UINT16 *)q = (UINT16)((work >> 1) + (work >> 17));
				q += sdraw->xalign;
			}
			q -= xbytes;
		}
		p += SURFACE_WIDTH * 2;
		q += sdraw->yalign;
		y += 2;
	} while(y < maxy);

	sdraw->src = p;
	sdraw->dst = q;
	sdraw->y = y;
}

// text + grph:interleave
static void SCRNCALL qvga16p_2i(SDRAW sdraw, int maxy) {

	int		xbytes;
const BYTE	*p;
const BYTE	*q;
	BYTE	*r;
	int		y;
	int		x;
	UINT32	work;

	xbytes = sdraw->xalign * sdraw->width / 2;
	p = sdraw->src;
	q = sdraw->src2;
	r = sdraw->dst;
	y = sdraw->y;
	do {
		if (*(UINT16 *)(sdraw->dirty + y)) {
			for (x=0; x<sdraw->width; x+=2) {
				work = np2_pal16[p[x+0] + q[x+0] + NP2PAL_GRPH];
				work += np2_pal16[p[x+1] + q[x+1] + NP2PAL_GRPH];
				if (q[x+0+SURFACE_WIDTH] & 0xf0) {
					work += np2_pal16[(q[x+0+SURFACE_WIDTH] >> 4)
															+ NP2PAL_TEXT];
				}
				else {
					work += np2_pal16[p[x+0] + NP2PAL_GRPH];
				}
				if (q[x+1+SURFACE_WIDTH] & 0xf0) {
					work += np2_pal16[(q[x+1+SURFACE_WIDTH] >> 4)
															+ NP2PAL_TEXT];
				}
				else {
					work += np2_pal16[p[x+1] + NP2PAL_GRPH];
				}
				work &= 0x07e0f81f << 2;
				*(UINT16 *)r = (UINT16)((work >> 2) + (work >> 18));
				r += sdraw->xalign;
			}
			r -= xbytes;
		}
		p += SURFACE_WIDTH * 2;
		q += SURFACE_WIDTH * 2;
		r += sdraw->yalign;
		y += 2;
	} while(y < maxy);

	sdraw->src = p;
	sdraw->src2 = q;
	sdraw->dst = r;
	sdraw->y = y;
}

static const SDRAWFN qvga16p[] = {
		qvga16p_0,		qvga16p_1,		qvga16p_1,		qvga16p_2,
		qvga16p_0,		qvga16p_1,		qvga16p_gi,		qvga16p_2i,
		qvga16p_0,		qvga16p_1,		qvga16p_gi,		qvga16p_2i};


const SDRAWFN *sdraw_getproctbl(const SCRNSURF *surf) {

	if (surf->bpp == 16) {
		return(qvga16p);
	}
	else {
		return(NULL);
	}
}

#endif


#include	"compiler.h"
#include	"scrnmng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"scrndraw.h"
#include	"sdraw.h"
#include	"dispsync.h"
#include	"palettes.h"


	BYTE	renewal_line[SURFACE_HEIGHT];
	BYTE	np2_tram[SURFACE_SIZE];
	BYTE	np2_vram[2][SURFACE_SIZE];


static void updateallline(UINT32 update) {

	UINT	i;

	for (i=0; i<SURFACE_HEIGHT; i+=4) {
		*(UINT32 *)(renewal_line + i) |= update;
	}
}


// ----

void scrndraw_initialize(void) {

	ZeroMemory(np2_tram, sizeof(np2_tram));
	ZeroMemory(np2_vram, sizeof(np2_vram));
	updateallline(0x80808080);
	scrnmng_allflash();
}

void scrndraw_changepalette(void) {

#if defined(SUPPORT_8BPP)
	if (scrnmng_getbpp() == 8) {
		scrnmng_palchanged();
		return;
	}
#endif
	updateallline(0x80808080);
}

static BYTE rasterdraw(SDRAWFN sdrawfn, SDRAW sdraw, int maxy) {

	RGB32		pal[16];
	SINT32		clock;
	PAL1EVENT	*event;
	PAL1EVENT	*eventterm;
	int			nextupdate;
	int			y;

	CopyMemory(pal, palevent.pal, sizeof(pal));
	clock = maxy;
	clock += 2;
	clock += np2cfg.realpal;
	clock -= 32;
	clock += (gdc.m.para[GDC_SYNC + 5] >> 2) & 0x3f;
	clock *= pccore.raster;
	event = palevent.event;
	eventterm = event + palevent.events;
	nextupdate = 0;
	for (y=2; y<maxy; y+=2) {
		if (event >= eventterm) {
			break;
		}
		// ���ٓ��͂������H
		if (clock < event->clock) {
			if (!(np2cfg.LCD_MODE & 1)) {
				pal_makeanalog(pal, 0xffff);
			}
			else {
				pal_makeanalog_lcd(pal, 0xffff);
			}
			if (np2cfg.skipline) {
				np2_pal32[0].d = np2_pal32[NP2PAL_SKIP].d;
#if defined(SUPPORT_16BPP)
				np2_pal16[0] = np2_pal16[NP2PAL_SKIP];
#endif
			}
			(*sdrawfn)(sdraw, y);
			nextupdate = y;
			// ���ٓ���H�ׂ�
			while(clock < event->clock) {
				((BYTE *)pal)[event->color] = event->value;
				event++;
				if (event >= eventterm) {
					break;
				}
			}
		}
		clock -= 2 * pccore.raster;
	}
	if (y < maxy) {
		if (!(np2cfg.LCD_MODE & 1)) {
			pal_makeanalog(pal, 0xffff);
		}
		else {
			pal_makeanalog_lcd(pal, 0xffff);
		}
		if (np2cfg.skipline) {
			np2_pal32[0].d = np2_pal32[NP2PAL_SKIP].d;
#if defined(SUPPORT_16BPP)
			np2_pal16[0] = np2_pal16[NP2PAL_SKIP];
#endif
		}
		(*sdrawfn)(sdraw, maxy);
	}
	if (palevent.vsyncpal) {
		return(2);
	}
	else if (nextupdate) {
		for (y=0; y<nextupdate; y+=2) {
			*(UINT16 *)(renewal_line + y) |= 0x8080;
		}
		return(1);
	}
	else {
		return(0);
	}
}

BYTE scrndraw_draw(BYTE redraw) {

	BYTE		ret;
const SCRNSURF	*surf;
const SDRAWFN	*sdrawfn;
	_SDRAW		sdraw;
	BYTE		bit;
	int			i;

	if (redraw) {
		updateallline(0x80808080);
	}

	ret = 0;
	surf = scrnmng_surflock();
	if (surf == NULL) {
		goto sddr_exit1;
	}
	sdrawfn = sdraw_getproctbl(surf);
	if (sdrawfn == NULL) {
		goto sddr_exit2;
	}

	bit = 0;
	if (gdc.mode1 & 0x80) {						// ver0.28
		if (gdcs.grphdisp & 0x80) {
			bit |= (1 << gdcs.disp);
		}
		if (gdcs.textdisp & 0x80) {
			bit |= 4;
		}
		else if (gdc.mode1 & 2) {
			bit = 0;
		}
	}
	bit |= 0x80;
	for (i=0; i<SURFACE_HEIGHT; i++) {
		sdraw.dirty[i] = renewal_line[i] & bit;
		if (sdraw.dirty[i]) {
			renewal_line[i] &= ~bit;
		}
	}
	if (gdc.mode1 & 0x10) {
		sdrawfn += 4;
		if (np2cfg.skipline) {
			sdrawfn += 4;
		}
	}
	switch(bit & 7) {
		case 1:								// grph1
			sdrawfn += 2;
			sdraw.src = np2_vram[0];
			break;

		case 2:								// grph2
			sdrawfn += 2;
			sdraw.src = np2_vram[1];
			break;

		case 4:								// text
			sdrawfn += 1;
			sdraw.src = np2_tram;
			break;

		case 5:								// text + grph1
			sdrawfn += 3;
			sdraw.src = np2_vram[0];
			sdraw.src2 = np2_tram;
			break;

		case 6:								// text + grph2
			sdrawfn += 3;
			sdraw.src = np2_vram[1];
			sdraw.src2 = np2_tram;
			break;
	}
	sdraw.dst = surf->ptr;
	sdraw.width = surf->width;
	sdraw.xbytes = surf->xalign * surf->width;
	sdraw.y = 0;
	sdraw.xalign = surf->xalign;
	sdraw.yalign = surf->yalign;
	if ((!gdc.analog) || (palevent.events >= PALEVENTMAX)) {
		(*(*sdrawfn))(&sdraw, surf->height);
	}
	else {
		ret = rasterdraw(*sdrawfn, &sdraw, surf->height);
	}

sddr_exit2:
	scrnmng_surfunlock(surf);

sddr_exit1:
	return(ret);
}

void scrndraw_redraw(void) {

	scrnmng_allflash();
	pal_change(1);
	dispsync_renewalmode();
	scrndraw_draw(1);
}


#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"biosmem.h"
#include	"lio.h"
#include	"vram.h"


typedef struct {
	BYTE	mode;
	BYTE	sw;
	BYTE	act;
	BYTE	disp;
} GSCREEN;

typedef struct {
	BYTE	x1[2];
	BYTE	y1[2];
	BYTE	x2[2];
	BYTE	y2[2];
	BYTE	vdraw_bg;
	BYTE	vdraw_ln;
} GVIEW;

typedef struct {
	BYTE	dummy;
	BYTE	bgcolor;
	BYTE	bdcolor;
	BYTE	fgcolor;
	BYTE	palmode;
} GCOLOR1;

typedef struct {
	BYTE	pal;
	BYTE	color1;
	BYTE	color2;
} GCOLOR2;


// ---- INIT

REG8 lio_ginit(LIOWORK lio) {

	UINT	i;

	vramop.operate &= VOP_ACCESSMASK;
	i286_vram_dispatch(vramop.operate);
	bios0x18_42(0x80);
	bios0x18_40();
	iocore_out8(0x006a, 0);
	gdc_paletteinit();

	ZeroMemory(&lio->mem, sizeof(lio->mem));
//	lio->mem.scrnmode = 0;
//	lio->mem.pos = 0;
	lio->mem.plane = 1;
//	lio->mem.bgcolor = 0;
	lio->mem.fgcolor = 7;
	for (i=0; i<8; i++) {
		lio->mem.color[i] = (UINT8)i;
	}
//	STOREINTELWORD(lio->mem.viewx1, 0);
//	STOREINTELWORD(lio->mem.viewy1, 0);
	STOREINTELWORD(lio->mem.viewx2, 639);
	STOREINTELWORD(lio->mem.viewy2, 399);
	lio->palmode = 0;
	i286_memstr_write(CPU_DS, 0x0620, &lio->mem, sizeof(lio->mem));
	i286_membyte_write(CPU_DS, 0x0a08, lio->palmode);
	return(LIO_SUCCESS);
}


// ---- SCREEN

REG8 lio_gscreen(LIOWORK lio) {

	GSCREEN	dat;
	UINT	colorbit;
	UINT8	scrnmode;
	UINT8	mono;
	UINT8	act;
	UINT8	pos;
	UINT8	disp;
	UINT8	plane;
	UINT8	planemax;
	UINT8	mode;

	if (lio->palmode != 2) {
		colorbit = 3;
	}
	else {
		colorbit = 4;
	}
	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	scrnmode = dat.mode;
	if (scrnmode == 0xff) {
		scrnmode = lio->mem.scrnmode;
	}
	else {
		if ((dat.mode >= 2) && (!(mem[MEMB_PRXCRT] & 0x40))) {
			goto gscreen_err5;
		}
	}
	if (scrnmode >= 4) {
		goto gscreen_err5;
	}
	if (dat.sw != 0xff) {
		if (!(dat.sw & 2)) {
			bios0x18_40();
		}
		else {
			bios0x18_41();
		}
	}

	mono = ((scrnmode + 1) >> 1) & 1;
	act = dat.act;
	if (act == 0xff) {
		if (scrnmode != lio->mem.scrnmode) {
			lio->mem.pos = 0;
			lio->mem.access = 0;
		}
	}
	else {
		switch(scrnmode) {
			case 0:
				pos = act & 1;
				act >>= 1;
				break;

			case 1:
				pos = act % (colorbit * 2);
				act = act / (colorbit * 2);
				break;

			case 2:
				pos = act % colorbit;
				act = act / colorbit;
				break;

			case 3:
			default:
				pos = 0;
				break;
		}
		if (act >= 2) {
			goto gscreen_err5;
		}
		lio->mem.pos = pos;
		lio->mem.access = act;
	}
	disp = dat.disp;
	if (disp == 0xff) {
		if (scrnmode != lio->mem.scrnmode) {
			lio->mem.plane = 1;
			lio->mem.disp = 0;
		}
	}
	else {
		plane = disp & ((2 << colorbit) - 1);
		disp >>= (colorbit + 1);
		if (disp >= 2) {
			goto gscreen_err5;
		}
		lio->mem.disp = disp;
		planemax = 1;
		if (mono) {
			planemax <<= colorbit;
		}
		if (!(scrnmode & 2)) {
			planemax <<= 1;
		}
		if ((plane > planemax) &&
			(plane != (1 << colorbit))) {
			goto gscreen_err5;
		}
		lio->mem.plane = plane;
		lio->mem.disp = disp;
	}

	lio->mem.scrnmode = scrnmode;
	switch(scrnmode) {
		case 0:
			mode = (pos)?0x40:0x80;
			break;

		case 1:
			mode = (pos >= colorbit)?0x60:0xa0;
			break;

		case 2:
			mode = 0xe0;
			break;

		case 3:
		default:
			mode = 0xc0;
			break;
	}
	mode |= disp << 4;
	TRACEOUT(("bios1842 - %.2x", mode));
	bios0x18_42(mode);
	iocore_out8(0x00a6, lio->mem.access);
	i286_memstr_write(CPU_DS, 0x0620, &lio->mem, sizeof(lio->mem));
	return(LIO_SUCCESS);

gscreen_err5:
	TRACEOUT(("screen error! %d %d %d %d",
								dat.mode, dat.sw, dat.act, dat.disp));
	return(LIO_ILLEGALFUNC);
}


// ---- VIEW

REG8 lio_gview(LIOWORK lio) {

	GVIEW	dat;
	int		x1;
	int		y1;
	int		x2;
	int		y2;

	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	x1 = (SINT16)LOADINTELWORD(dat.x1);
	y1 = (SINT16)LOADINTELWORD(dat.y1);
	x2 = (SINT16)LOADINTELWORD(dat.x2);
	y2 = (SINT16)LOADINTELWORD(dat.y2);
	if ((x1 >= x2) || (y1 >= y2)) {
		return(LIO_ILLEGALFUNC);
	}
	STOREINTELWORD(lio->mem.viewx1, (UINT16)x1);
	STOREINTELWORD(lio->mem.viewy1, (UINT16)y1);
	STOREINTELWORD(lio->mem.viewx2, (UINT16)x2);
	STOREINTELWORD(lio->mem.viewy2, (UINT16)y2);
	i286_memstr_write(CPU_DS, 0x0620, &lio->mem, sizeof(lio->mem));
	return(LIO_SUCCESS);
}


// ---- COLOR1

REG8 lio_gcolor1(LIOWORK lio) {

	GCOLOR1	dat;

	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	if (dat.bgcolor != 0xff) {
		lio->mem.bgcolor = dat.bgcolor;
	}
	if (dat.fgcolor == 0xff) {
		lio->mem.fgcolor = dat.fgcolor;
	}
	if (dat.palmode != 0xff) {
		if (!(mem[MEMB_PRXCRT] & 1)) {				// 8color lio
			dat.palmode = 0;
		}
		else {
			if (!(mem[MEMB_PRXCRT] & 4)) {			// have e-plane?
				goto gcolor1_err5;
			}
			if (!dat.palmode) {
				iocore_out8(0x006a, 0);
			}
			else {
				iocore_out8(0x006a, 1);
			}
		}
		lio->palmode = dat.palmode;
	}
	i286_memstr_write(CPU_DS, 0x0620, &lio->mem, sizeof(lio->mem));
	i286_membyte_write(CPU_DS, 0x0a08, lio->palmode);
	return(LIO_SUCCESS);

gcolor1_err5:
	return(LIO_ILLEGALFUNC);
}


// ---- COLOR2

REG8 lio_gcolor2(LIOWORK lio) {

	GCOLOR2	dat;

	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	if (dat.pal >= ((lio->palmode == 2)?16:8)) {
		goto gcolor2_err5;
	}
	if (!lio->palmode) {
		dat.color1 &= 7;
		lio->mem.color[dat.pal] = dat.color1;
		gdc_setdegitalpal(dat.pal, dat.color1);
	}
	else {
		gdc_setanalogpal(dat.pal, offsetof(RGB32, p.b),
												(UINT8)(dat.color1 & 0x0f));
		gdc_setanalogpal(dat.pal, offsetof(RGB32, p.r),
												(UINT8)(dat.color1 >> 4));
		gdc_setanalogpal(dat.pal, offsetof(RGB32, p.g),
												(UINT8)(dat.color2 & 0x0f));
	}
	i286_memstr_write(CPU_DS, 0x0620, &lio->mem, sizeof(lio->mem));
	return(LIO_SUCCESS);

gcolor2_err5:
	return(LIO_ILLEGALFUNC);
}


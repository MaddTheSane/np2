#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"biosmem.h"
#include	"lio.h"
#include	"vram.h"


typedef struct {
	BYTE	x1[2];
	BYTE	y1[2];
	BYTE	x2[2];
	BYTE	y2[2];
	BYTE	vdraw_bg;
	BYTE	vdraw_ln;
} GVIEW;

typedef struct {
	UINT8	pal;
	UINT8	color1;
	UINT8	color2;
} GCOLOR2;


static void lio_makescreen(const _LIOWORK *lio) {

	UINT16	pos;

	// GDCバッファを空に
	if (gdc.s.cnt) {
		gdc_work(GDCWORK_SLAVE);
	}
	gdc_forceready(&gdc.s);

	ZeroMemory(&gdc.s.para[GDC_SCROLL], 8);
	if (lio->scrn.lines == 200) {
		gdc.mode1 |= 0x10;
		gdc.s.para[GDC_CSRFORM] = 1;
		pos = lio->scrn.top >> 1;
		STOREINTELWORD(gdc.s.para + GDC_SCROLL + 0, pos);
	}
	else {
		gdc.mode1 &= ~0x10;
		gdc.s.para[GDC_CSRFORM] = 0;
	}
	gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
	gdcs.disp = lio->scrn.disp & 1;
	screenupdate |= 2;
	iocore_out8(0x00a6, lio->scrn.bank);
}


// ---- INIT

REG8 lio_ginit(LIOWORK lio) {

	UINT	i;

	// GDCバッファを空に
	if (gdc.s.cnt) {
		gdc_work(GDCWORK_SLAVE);
	}
	gdc_forceready(&gdc.s);

	ZeroMemory(lio, sizeof(_LIOWORK));
	ZeroMemory(&gdc.s.para[GDC_SCROLL], 8);
	gdc.mode1 |= 0x10;
	gdc.s.para[GDC_CSRFORM] = 1;
	screenupdate |= 2;
	gdcs.access = gdcs.disp = 0;
	vramop.operate &= VOP_ACCESSMASK;
	i286_vram_dispatch(vramop.operate);
	iocore_out8(0x006a, 0);
	gdcs.grphdisp |= GDCSCRN_ENABLE;
	gdc_paletteinit();

	lio->scrn.top = 0;
	lio->scrn.lines = 200;
	lio->scrn.bank = 0;
	lio->scrn.plane = 0x80;

	lio->gcolor1.palmax = 8;
	lio->gcolor1.bgcolor = 0;
	lio->gcolor1.bdcolor = 0;
	lio->gcolor1.fgcolor = 7;
	lio->gcolor1.palmode = 0;

	lio->gview.x1 = 0;
	lio->gview.y1 = 0;
	lio->gview.x2 = 639;
	lio->gview.y2 = 399;

	for (i=0; i<8; i++) {
		lio->degcol[i] = (UINT8)i;
	}
	return(LIO_SUCCESS);
}


// ---- SCREEN

REG8 lio_gscreen(LIOWORK lio) {

	LIOGSCREEN	data;
	LIO_SCRN	scrn;
	BOOL		screenmodechange = FALSE;
	BYTE		bit;
	int			disp;

	i286_memstr_read(CPU_DS, CPU_BX, &data, sizeof(data));
	if (data.mode == 0xff) {
		data.mode = lio->gscreen.mode;
	}
	if (data.mode > 4) {
		goto gscreen_err5;
	}
	else if ((data.mode >= 2) && (!(mem[MEMB_PRXCRT] & 0x40))) {
		goto gscreen_err5;
	}

	if (data.mode != lio->gscreen.mode) {
		screenmodechange = TRUE;
	}
	if (data.sw == 0xff) {
		data.sw = lio->gscreen.mode;
	}
	if (data.act == 0xff) {
		if (screenmodechange) {
			data.act = 0;
		}
		else {
			data.act = lio->gscreen.act;
		}
	}
	if (data.disp == 0xff) {
		if (screenmodechange) {
			data.disp = 1;
		}
		else {
			data.disp = lio->gscreen.disp;
		}
	}

	if (data.mode >= 0x04) {
		return(5);
	}
	if (data.sw >= 0x04) {
		return(5);
	}
	if (lio->gcolor1.palmode != 2) {
		bit = 3;
	}
	else {
		bit = 4;
	}
	switch(data.mode) {
		case 0:
			if (data.act >= 4) {
				return(5);
			}
			scrn.top = (data.act & 1) * 16000;
			scrn.lines = 200;
			scrn.bank = data.act >> 1;
			scrn.plane = 0x80;
			scrn.disp = (data.disp >> (bit + 1)) & 1;
			if (data.disp & ((1 << bit) - 1)) {
				scrn.dbit = (1 << bit) - 1;
				if ((data.disp & ((1 << (bit + 1)) - 1)) >= 3) {
					return(5);
				}
			}
			else {
				scrn.dbit = 0;
			}
			break;

		case 1:
			disp = data.act / bit;
			if (disp >= 4) {
				return(5);
			}
			scrn.top = (disp & 1) * 16000;
			scrn.lines = 200;
			scrn.bank = disp >> 1;
			scrn.plane = data.act % bit;
			scrn.disp = (data.disp >> (bit + 1)) & 1;
			scrn.dbit = data.disp & ((1 << bit) - 1);
			break;

		case 2:
			disp = data.act / bit;
			if (disp >= 2) {
				return(5);
			}
			scrn.top = 0;
			scrn.lines = 400;
			scrn.bank = disp;
			scrn.plane = data.act % bit;
			scrn.disp = (data.disp >> (bit + 1)) & 1;
			scrn.dbit = data.disp & ((1 << bit) - 1);
			if ((scrn.dbit) && (data.disp & (1 << bit))) {
				return(5);
			}
			break;

		case 3:
			if (data.act >= 2) {
				return(5);
			}
			scrn.top = 0;
			scrn.lines = 400;
			scrn.bank = data.act;
			scrn.plane = 0x80;
			scrn.disp = (data.disp >> (bit + 1)) & 1;
			if (data.disp & ((1 << bit) - 1)) {
				scrn.dbit = (1 << bit) - 1;
				if ((data.disp & ((1 << (bit + 1)) - 1)) >= 2) {
					return(5);
				}
			}
			else {
				scrn.dbit = 0;
			}
			break;
	}
	lio->scrn = scrn;
	lio_makescreen(lio);
	return(LIO_SUCCESS);

gscreen_err5:
	return(LIO_ILLEGALFUNC);
}


// ---- VIEW

REG8 lio_gview(LIOWORK lio) {

	GVIEW	dat;
	SINT16	x1;
	SINT16	y1;
	SINT16	x2;
	SINT16	y2;

	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	x1 = (SINT16)LOADINTELWORD(dat.x1);
	y1 = (SINT16)LOADINTELWORD(dat.y1);
	x2 = (SINT16)LOADINTELWORD(dat.x2);
	y2 = (SINT16)LOADINTELWORD(dat.y2);
	if (((dat.x1 >= dat.x2) || (dat.y1 >= dat.y2)) ||
		((dat.vdraw_bg >= lio->gcolor1.palmax) && (dat.vdraw_bg != 0xff)) ||
		((dat.vdraw_ln >= lio->gcolor1.palmax) && (dat.vdraw_ln != 0xff))) {
		return(LIO_ILLEGALFUNC);
	}
	lio->gview.x1 = x1;
	lio->gview.y1 = y1;
	lio->gview.x2 = x2;
	lio->gview.y2 = y2;
	lio->gview.vdraw_bg = dat.vdraw_bg;
	lio->gview.vdraw_ln = dat.vdraw_ln;
	return(LIO_SUCCESS);
}


// ---- COLOR1

REG8 lio_gcolor1(LIOWORK lio) {

	LIOGCOLOR1	dat;

	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	if (dat.bgcolor == 0xff) {
		dat.bgcolor = lio->gcolor1.bgcolor;
	}
	if (dat.bdcolor == 0xff) {
		dat.bdcolor = lio->gcolor1.bdcolor;
	}
	if (dat.fgcolor == 0xff) {
		dat.fgcolor = lio->gcolor1.fgcolor;
	}
	if (dat.palmode == 0xff) {
		dat.palmode = lio->gcolor1.palmode;
	}
	else if (!(mem[MEMB_PRXCRT] & 1)) {				// 16color lio
		dat.palmode = 0;
	}
	else {
		if (!(mem[MEMB_PRXCRT] & 4)) {				// have e-plane?
			goto gcolor1_err5;
		}
		dat.palmax = (dat.palmode == 2)?16:8;
		if (!dat.palmode) {
			iocore_out8(0x006a, 0);
		}
		else {
			iocore_out8(0x006a, 1);
		}
	}
	lio->gcolor1 = dat;
	return(LIO_SUCCESS);

gcolor1_err5:
	return(LIO_ILLEGALFUNC);
}


// ---- COLOR2

REG8 lio_gcolor2(LIOWORK lio) {

	GCOLOR2	dat;

	i286_memstr_read(CPU_DS, CPU_BX, &dat, sizeof(dat));
	if (dat.pal >= lio->gcolor1.palmax) {
		goto gcolor2_err5;
	}
	if (!lio->gcolor1.palmode) {
		dat.color1 &= 7;
		lio->degcol[dat.pal] = dat.color1;
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
	return(LIO_SUCCESS);

gcolor2_err5:
	return(LIO_ILLEGALFUNC);
}


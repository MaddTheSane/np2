#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"lio.h"


static void lio_makescreen(void) {

	UINT16	pos;

	// GDCバッファを空に
	if (gdc.s.cnt) {
		gdc_work(GDCWORK_SLAVE);
	}
	gdc_forceready(&gdc.s);

	ZeroMemory(&gdc.s.para[GDC_SCROLL], 8);
	if (lio.scrn.lines == 200) {
		gdc.mode1 |= 0x10;
		gdc.s.para[GDC_CSRFORM] = 1;
		pos = lio.scrn.top >> 1;
		STOREINTELWORD(gdc.s.para + GDC_SCROLL + 0, pos);
	}
	else {
		gdc.mode1 &= ~0x10;
		gdc.s.para[GDC_CSRFORM] = 0;
	}
	gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
	gdcs.disp = lio.scrn.disp & 1;
	screenupdate |= 2;
	iocore_out8(0x00a6, lio.scrn.bank);
}

BYTE lio_gscreen(void) {

	LIOGSCREEN	data;
	LIO_SCRN		scrn;
	BOOL			screenmodechange = FALSE;
	BYTE			bit;
	int				disp;

	i286_memstr_read(CPU_DS, CPU_BX, &data, sizeof(data));
	if (data.mode == 0xff) {
		data.mode = lio.gscreen.mode;
	}
	else if (data.mode != lio.gscreen.mode) {
		screenmodechange = TRUE;
	}
	if (data.sw == 0xff) {
		data.sw = lio.gscreen.mode;
	}
	if (data.act == 0xff) {
		if (screenmodechange) {
			data.act = 0;
		}
		else {
			data.act = lio.gscreen.act;
		}
	}
	if (data.disp == 0xff) {
		if (screenmodechange) {
			data.disp = 1;
		}
		else {
			data.disp = lio.gscreen.disp;
		}
	}

	if (data.mode >= 0x04) {
		return(5);
	}
	if (data.sw >= 0x04) {
		return(5);
	}
	if (lio.gcolor1.palmode != 2) {
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
	lio.scrn = scrn;
	lio_makeviewmask();
	lio_makescreen();
	return(0);
}


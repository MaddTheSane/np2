#include	"compiler.h"
#include	"i286.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"biosmem.h"
#include	"vram.h"
#include	"lio.h"


	LIO_TABLE	lio;


void lio_init(void) {

	UINT	i;

	mem[0xf9900] = 0x11;
	for (i=0; i<0x11; i++) {
		mem[0xf9904 + i*4] = 0xa0 + i;
		SETBIOSMEM16(0xf9906 + i*4, 0x100 + i*2);
		SETBIOSMEM16(0xf9a00 + i*2, 0xcf90);
	}
	mem[0xf9944] = 0xce;
	ZeroMemory(&lio, sizeof(lio));
}

void bios_lio(BYTE cmd) {

	UINT	i;

//	TRACE_("lio", cmd);

	switch(cmd) {
		case 0x00:			// a0: GINIT
			// GDCバッファを空に
			if (gdc.s.cnt) {
				gdc_work(GDCWORK_SLAVE);
			}
			gdc_forceready(&gdc.s);

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

			lio.scrn.top = 0;
			lio.scrn.lines = 200;
			lio.scrn.bank = 0;
			lio.scrn.plane = 0x80;

			lio.gcolor1.palmax = 8;
			lio.gcolor1.bgcolor = 0;
			lio.gcolor1.bdcolor = 0;
			lio.gcolor1.fgcolor = 7;
			lio.gcolor1.palmode = 0;

			lio.gview.x1 = 0;
			lio.gview.y1 = 0;
			lio.gview.x2 = 639;
			lio.gview.y2 = 399;
			lio_makeviewmask();

			for (i=0; i<8; i++) {
				lio.degcol[i] = i;
			}
			I286_AH = 0;
			break;

		case 0x01:			// a1: GSCREEN
			I286_AH = lio_gscreen();
			break;

		case 0x02:			// a2: GVIEW
			I286_AH = lio_gview();
			break;

		case 0x03:			// a3: GCOLOR1
			I286_AH = lio_gcolor1();
			break;

		case 0x04:			// a4: GCOLOR2
			I286_AH = lio_gcolor2();
			break;

		case 0x05:			// a5: GCLS
			I286_AH = lio_gcls();
			break;

		case 0x06:			// a6: GPSET
			I286_AH = lio_gpset();
			break;

		case 0x07:			// a7: GLINE
			I286_AH = lio_gline();
			break;

		case 0x08:			// a8: GCIRCLE
			break;

		case 0x09:			// a9: GPAINT1
			break;

		case 0x0a:			// aa: GPAINT2
			break;

		case 0x0b:			// ab: GGET
			break;

		case 0x0c:			// ac: GPUT1
			I286_AH = lio_gput1();
			break;

		case 0x0d:			// ad: GPUT2
			break;

		case 0x0e:			// ae: GROLL
			break;

		case 0x0f:			// af: GPOINT2
			break;

		case 0x10:			// ce: GCOPY
			break;
	}
	// TRACEOUT(("lio cmd-%d, %d", cmd, I286_AH));
}


// ----

static const UINT32 planeadrs[4] = {0xa8000, 0xb0000, 0xb8000, 0xe0000};
static const BYTE bit_l[8] = {0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01};
static const BYTE bit_r[8] = {0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe};

static void pixed8(UINT32 vadrs, BYTE bit, BYTE pal) {

	if (lio.scrn.plane & 0x80) {
		if (pal & 1) {
			mem[vadrs+0xa8000] |= bit;
		}
		else {
			mem[vadrs+0xa8000] &= ~bit;
		}
		if (pal & 2) {
			mem[vadrs+0xb0000] |= bit;
		}
		else {
			mem[vadrs+0xb0000] &= ~bit;
		}
		if (pal & 4) {
			mem[vadrs+0xb8000] |= bit;
		}
		else {
			mem[vadrs+0xb8000] &= ~bit;
		}
		if (pal & 8) {
			mem[vadrs+0xe0000] |= bit;
		}
		else {
			mem[vadrs+0xe0000] &= ~bit;
		}
	}
	else {
		vadrs += planeadrs[lio.scrn.plane & 3];
		if (pal) {
			mem[vadrs] |= bit;
		}
		else {
			mem[vadrs] &= ~bit;
		}
	}
}

void lio_pset(SINT16 x, SINT16 y, BYTE pal) {

	UINT32	adrs;
	BYTE	bit;

	if ((lio.x1 > x) || (lio.x2 < x) || (lio.y1 > y) || (lio.y2 < y)) {
		return;
	}
	adrs = (y * 80) + (x >> 3);
	bit = 0x80 >> (x & 7);
	if (lio.scrn.top) {
		adrs += 16000;
	}
	if (!lio.scrn.bank) {
		vramupdate[adrs] |= 1;
		gdcs.grphdisp |= 1;
	}
	else {
		vramupdate[adrs] |= 2;
		gdcs.grphdisp |= 2;
		adrs += 0x200000;
	}
	pixed8(adrs, bit, pal);
}

void lio_line(SINT16 x1, SINT16 x2, SINT16 y, BYTE pal) {

	UINT	adrs;
	UINT32	vadrs;
	BYTE	bit, dbit, sbit;
	SINT16	width;

	if ((lio.y1 > y) || (lio.y2 < y)) {
		return;
	}
	if (lio.x1 > x1) {
		x1 = lio.x1;
	}
	if (lio.x2 < x2) {
		x2 = lio.x2;
	}
	width = x2 - x1 + 1;
	if (width <= 0) {
		return;
	}
	adrs = (y * 80) + (x1 >> 3);
	bit = 0x80 >> (x1 & 7);
	if (lio.scrn.top) {
		adrs += 16000;
	}
	if (!lio.scrn.bank) {
		vadrs = adrs;
		sbit = 1;
		gdcs.grphdisp |= 1;
	}
	else {
		vadrs = adrs + 0x200000;
		sbit = 2;
		gdcs.grphdisp |= 2;
	}

	dbit = 0;
	while(bit && width--) {
		dbit |= bit;
		bit >>= 1;
	}
	pixed8(vadrs++, dbit, pal);
	vramupdate[adrs++] |= sbit;
	while(width >= 8) {
		width -= 8;
		pixed8(vadrs++, 0xff, pal);
		vramupdate[adrs++] |= sbit;
	}
	dbit = 0;
	bit = 0x80;
	while(bit && width--) {
		dbit |= bit;
		bit >>= 1;
	}
	if (dbit) {
		pixed8(vadrs, dbit, pal);
		vramupdate[adrs] |= sbit;
	}
}


#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"bios.h"
#include	"biosmem.h"
#include	"vram.h"
#include	"lio.h"


static	_LIOWORK	liowork;


void lio_initialize(void) {

	UINT	i;

	mem[0xf9900] = 0x11;
	for (i=0; i<0x11; i++) {
		mem[0xf9904 + i*4] = 0xa0 + i;
		mem[0xf9905 + i*4] = 0x00;
		SETBIOSMEM16(0xf9906 + i*4, 0x100 + i*4);
		SETBIOSMEM32(0xf9a00 + i*4, 0xcf90fb90);
	}
	mem[0xf9944] = 0xce;
}

void bios_lio(REG8 cmd) {

	LIOWORK	lio;

	TRACEOUT(("lio command %.2x", cmd));

	lio = &liowork;
	lio->wait = 500;
	switch(cmd) {
		case 0x00:			// a0: GINIT
			CPU_AH = lio_ginit(lio);
			break;

		case 0x01:			// a1: GSCREEN
			CPU_AH = lio_gscreen(lio);
			break;

		case 0x02:			// a2: GVIEW
			CPU_AH = lio_gview(lio);
			break;

		case 0x03:			// a3: GCOLOR1
			CPU_AH = lio_gcolor1(lio);
			break;

		case 0x04:			// a4: GCOLOR2
			CPU_AH = lio_gcolor2(lio);
			break;

		case 0x05:			// a5: GCLS
			CPU_AH = lio_gcls(lio);
			break;

		case 0x06:			// a6: GPSET
			CPU_AH = lio_gpset(lio);
			break;

		case 0x07:			// a7: GLINE
			CPU_AH = lio_gline(lio);
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
			CPU_AH = lio_gput1(lio);
			break;

		case 0x0d:			// ad: GPUT2
			CPU_AH = lio_gput2(lio);
			break;

		case 0x0e:			// ae: GROLL
			break;

		case 0x0f:			// af: GPOINT2
			break;

		case 0x10:			// ce: GCOPY
			break;
	}
	gdcsub_setslavewait(lio->wait);
	TRACEOUT(("lio wait = %d", lio->wait));
}


// ----

const UINT32 lioplaneadrs[4] = {VRAM_B, VRAM_R, VRAM_G, VRAM_E};

void lio_updaterange(LIOWORK lio) {

	lio->range.x1 = max(lio->gview.x1, 0);
	lio->range.y1 = max(lio->gview.y1, 0);
	lio->range.x2 = min(lio->gview.x2, 639);
	lio->range.y2 = min(lio->gview.x2, (lio->scrn.lines - 1));
}


// ----

static const BYTE bit_l[8] = {0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01};
static const BYTE bit_r[8] = {0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe};

static void pixed8(const _LIOWORK *lio, UINT32 vadrs, BYTE bit, REG8 pal) {

	if (lio->scrn.plane & 0x80) {
		if (pal & 1) {
			mem[vadrs + VRAM_B] |= bit;
		}
		else {
			mem[vadrs + VRAM_B] &= ~bit;
		}
		if (pal & 2) {
			mem[vadrs + VRAM_R] |= bit;
		}
		else {
			mem[vadrs + VRAM_R] &= ~bit;
		}
		if (pal & 4) {
			mem[vadrs + VRAM_G] |= bit;
		}
		else {
			mem[vadrs + VRAM_G] &= ~bit;
		}
		if (pal & 8) {
			mem[vadrs + VRAM_E] |= bit;
		}
		else {
			mem[vadrs + VRAM_E] &= ~bit;
		}
	}
	else {
		vadrs += lioplaneadrs[lio->scrn.plane & 3];
		if (pal) {
			mem[vadrs] |= bit;
		}
		else {
			mem[vadrs] &= ~bit;
		}
	}
}

void lio_pset(const _LIOWORK *lio, SINT16 x, SINT16 y, REG8 pal) {

	UINT32	adrs;
	BYTE	bit;

	if ((lio->range.x1 > x) || (lio->range.x2 < x) ||
		(lio->range.y1 > y) || (lio->range.y2 < y)) {
		return;
	}
	adrs = (y * 80) + (x >> 3);
	bit = 0x80 >> (x & 7);
	if (lio->scrn.top) {
		adrs += 16000;
	}
	if (!lio->scrn.bank) {
		vramupdate[adrs] |= 1;
		gdcs.grphdisp |= 1;
	}
	else {
		vramupdate[adrs] |= 2;
		gdcs.grphdisp |= 2;
		adrs += VRAM_STEP;
	}
	pixed8(lio, adrs, bit, pal);
}

void lio_line(const _LIOWORK *lio, SINT16 x1, SINT16 x2, SINT16 y, REG8 pal) {

	UINT	adrs;
	UINT32	vadrs;
	BYTE	bit, dbit, sbit;
	SINT16	width;

	if ((lio->range.y1 > y) || (lio->range.y2 < y)) {
		return;
	}
	if (lio->range.x1 > x1) {
		x1 = lio->range.x1;
	}
	if (lio->range.x2 < x2) {
		x2 = lio->range.x2;
	}
	width = x2 - x1 + 1;
	if (width <= 0) {
		return;
	}
	adrs = (y * 80) + (x1 >> 3);
	bit = 0x80 >> (x1 & 7);
	if (lio->scrn.top) {
		adrs += 16000;
	}
	if (!lio->scrn.bank) {
		vadrs = adrs;
		sbit = 1;
		gdcs.grphdisp |= 1;
	}
	else {
		vadrs = adrs + VRAM_STEP;
		sbit = 2;
		gdcs.grphdisp |= 2;
	}

	dbit = 0;
	while(bit && width--) {
		dbit |= bit;
		bit >>= 1;
	}
	pixed8(lio, vadrs++, dbit, pal);
	vramupdate[adrs++] |= sbit;
	while(width >= 8) {
		width -= 8;
		pixed8(lio, vadrs++, 0xff, pal);
		vramupdate[adrs++] |= sbit;
	}
	dbit = 0;
	bit = 0x80;
	while(bit && width--) {
		dbit |= bit;
		bit >>= 1;
	}
	if (dbit) {
		pixed8(lio, vadrs, dbit, pal);
		vramupdate[adrs] |= sbit;
	}
}


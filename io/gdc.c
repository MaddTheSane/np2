#include	"compiler.h"
#include	"scrnmng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"palettes.h"
#include	"gdc_cmd.tbl"
#include	"timing.h"


#define	SEARCH_SYNC
#define	TURE_SYNC

typedef struct {
	UINT32	clock;
	UINT	minx;
	UINT	maxx;
	UINT	miny;
	UINT	maxy;
} GDCCLK;

static const GDCCLK gdcclk[] = {
			{14318180 / 8, 112 - 8, 112 + 8, 200, 300},
			{21052600 / 8, 106 - 6, 106 + 6, 400, 575},
			{25056815 / 8, 106 - 6, 106 + 6, 400, 575},
			{25175000 / 8, 100 - 4, 100 + 4, 400, 575}};


static const UINT8 defdegpal[4] = {0x04,0x15,0x26,0x37};

static const UINT8 defsyncm15[8] = {0x10,0x4e,0x07,0x25,0x0d,0x0f,0xc8,0x94};
static const UINT8 defsyncs15[8] = {0x06,0x26,0x03,0x11,0x86,0x0f,0xc8,0x94};

static const UINT8 defsyncm24[8] = {0x10,0x4e,0x07,0x25,0x07,0x07,0x90,0x65};
static const UINT8 defsyncs24[8] = {0x06,0x26,0x03,0x11,0x83,0x07,0x90,0x65};


void gdc_setdegitalpal(int color, REG8 value) {

	if (color & 4) {
		color &= 3;
		value &= 0x0f;
		if ((gdc.degpal[color] ^ value) & 0x07) {
			gdcs.palchange = GDCSCRN_REDRAW;
		}
		gdc.degpal[color] &= 0xf0;
		gdc.degpal[color] |= value;
	}
	else {
		color &= 3;
		value <<= 4;
		if ((gdc.degpal[color] ^ value) & 0x70) {
			gdcs.palchange = GDCSCRN_REDRAW;
		}
		gdc.degpal[color] &= 0x0f;
		gdc.degpal[color] |= value;
	}
}

void gdc_setanalogpal(int color, int rgb, REG8 value) {

	UINT8		*ptr;
	PAL1EVENT	*event;

	ptr = ((UINT8 *)(gdc.anapal + color)) + rgb;
	if (((*ptr) ^ value) & 0x0f) {
		gdcs.palchange = GDCSCRN_REDRAW;
		if (palevent.events < PALEVENTMAX) {
			if (!gdc.vsync) {
				event = palevent.event + palevent.events;
				event->clock = nevent.item[NEVENT_FLAMES].clock -
											(CPU_BASECLOCK - CPU_REMCLOCK);
				event->color = (color * sizeof(RGB32)) + rgb;
				event->value = (UINT8)value;
				palevent.events++;
			}
			else {
				palevent.vsyncpal = 1;
			}
		}
	}
	*ptr = value;
}

void gdc_setdegpalpack(int color, REG8 value) {

	if ((gdc.degpal[color] ^ value) & 0x77) {
		gdcs.palchange = GDCSCRN_REDRAW;
	}
	gdc.degpal[color] = (UINT8)value;
}

void gdc_paletteinit(void) {

	int		c;

	CopyMemory(gdc.degpal, defdegpal, 4);
	for (c=0; c<8; c++) {
		gdc.anapal[c+0].p.b = ((c&1)?0x0f:0);
		gdc.anapal[c+8].p.b = ((c&1)?0x0a:0);
		gdc.anapal[c+0].p.r = ((c&2)?0x0f:0);
		gdc.anapal[c+8].p.r = ((c&2)?0x0a:0);
		gdc.anapal[c+0].p.g = ((c&4)?0x0f:0);
		gdc.anapal[c+8].p.g = ((c&4)?0x0a:0);
	}
	gdc.anapal[8].p.b = 0x07;
	gdc.anapal[8].p.r = 0x07;
	gdc.anapal[8].p.g = 0x07;
	gdcs.palchange = GDCSCRN_REDRAW;
}


// --------------------------------------------------------------------------

static void vectdraw(void) {

	UINT32		csrw;
const GDCVECT	*vect;
	REG16		textw;
	REG8		ope;

	csrw = LOADINTELDWORD(gdc.s.para + GDC_CSRW);
	vect = (GDCVECT *)(gdc.s.para + GDC_VECTW);
	textw = LOADINTELWORD(gdc.s.para + GDC_TEXTW);
	ope = gdc.s.para[GDC_WRITE];

	if (vect->ope & 0x08) {
		gdcsub_line(csrw, vect, textw, ope);
	}
	if (vect->ope & 0x10) {		// undocumented
		gdcsub_txt(csrw, vect, textw, ope);
	}
	if (vect->ope & 0x20) {
		gdcsub_circle(csrw, vect, textw, ope);
	}
	if (vect->ope & 0x40) {
		gdcsub_box(csrw, vect, textw, ope);
	}
}

static void textdraw(void) {

	UINT32		csrw;
const GDCVECT	*vect;
	REG16		textw;
	REG8		ope;

	csrw = LOADINTELDWORD(gdc.s.para + GDC_CSRW);
	vect = (GDCVECT *)(gdc.s.para + GDC_VECTW);
	textw = gdc.s.para[GDC_TEXTW + 7];
	textw = (textw << 8) + textw;
	ope = gdc.s.para[GDC_WRITE];

	if (vect->ope & 0x08) {		// undocumented
		gdcsub_line(csrw, vect, textw, ope);
	}
	if (vect->ope & 0x10) {
		gdcsub_text(csrw, vect, gdc.s.para + GDC_TEXTW, ope);
	}
	if (vect->ope & 0x20) {		// undocumented
		gdcsub_circle(csrw, vect, textw, ope);
	}
	if (vect->ope & 0x40) {		// undocumented
		gdcsub_box(csrw, vect, textw, ope);
	}
}

void gdc_work(int id) {

	GDCDATA		item;
	UINT8		*dispflag;
	UINT		i;
	BYTE		data;

	item = (id==GDCWORK_MASTER)?&gdc.m:&gdc.s;
	dispflag = (id==GDCWORK_MASTER)?&gdcs.textdisp:&gdcs.grphdisp;

	for (i=0; i<item->cnt; i++) {
		data = (BYTE)item->fifo[i];
		if (item->fifo[i] & 0xff00) {
			item->cmd = data;										// ver0.29
			item->paracb = 0;
			if ((data & 0x60) == 0x20) {
				item->para[GDC_WRITE] = data;
				switch(data & 0x18) {
					case 0x00:
						item->rcv = 2;
						item->paracb = 1;
						break;

					case 0x10:
					case 0x18:
						item->rcv = 1;
						item->paracb = 1;
						break;

					default:
						item->rcv = 0;
						break;
				}
				item->ptr = GDC_CODE;
				item->snd = 0;
				continue;
			}
			switch(data) {
				case CMD_START_:
				case CMD_START:
				case CMD_SYNC_ON:
					(*dispflag) |= GDCSCRN_ENABLE | GDCSCRN_ALLDRAW2;
					screenupdate |= 2;								// ver0.28
					break;

				case CMD_STOP_:
				case CMD_STOP:
				case CMD_SYNC_OFF:
					(*dispflag) &= (~GDCSCRN_ENABLE);
//					(*dispflag) |= GDCSCRN_ALLDRAW2;
					screenupdate |= 2;								// ver0.28
					break;

				case CMD_VECTE:
					if (id != GDCWORK_MASTER) {
						vectdraw();
					}
					break;

				case CMD_TEXTE:										// ver0.30
					if (id != GDCWORK_MASTER) {
						textdraw();
					}
					break;
			}
			item->ptr = gdc_cmd[data].pos;
			item->rcv = gdc_cmd[data].outdatas;
			item->snd = gdc_cmd[data].indatas;
			if (item->snd) {
				switch(item->ptr) {
					case GDC_CSRR:
						item->para[GDC_CSRR+0] = item->para[GDC_CSRW+0];
						item->para[GDC_CSRR+1] = item->para[GDC_CSRW+1];
						item->para[GDC_CSRR+2] = item->para[GDC_CSRW+2] & 3;
						item->para[GDC_CSRR+3] = 0;
						item->para[GDC_CSRR+4] = 0;
						break;
				}
			}
		}
		else if (item->rcv) {
			if (item->para[item->ptr] != data) {
				item->para[item->ptr] = data;
				(*dispflag) |= gdc_dirtyflag[id][item->ptr];
			}
			(item->ptr)++;
			(item->rcv)--;					// ver0.29
			if ((!(item->rcv)) && (id == GDCWORK_SLAVE) &&
				(((item->cmd) & 0xe4) == 0x20)) {
				gdcsub_write();
				item->paracb = 0;			// ver0.29
			}
		}
	}
	item->cnt = 0;
}

// BIOSとかで弄った時にリセット
void gdc_forceready(GDCDATA item) {

	item->rcv = 0;
	item->snd = 0;
}


void gdc_updateclock(void) {

	UINT		tmp;
	UINT		cr;
	UINT		hfbs;
	UINT		vfbs;
	UINT		lf;
	UINT		x;
	UINT		y;
	UINT		cnt;
const GDCCLK	*clk;
	UINT32		hclock;

	cr = gdc.m.para[GDC_SYNC + 1] + 2;
	tmp = LOADINTELWORD(gdc.m.para + GDC_SYNC + 2);
	hfbs = tmp & 0x1f;									// HS
	hfbs += tmp >> 10;									// HFP
	vfbs = (tmp >> 5) & 0x1f;							// VS
	hfbs += gdc.m.para[GDC_SYNC + 4] & 0x3f;			// HFP
	vfbs += gdc.m.para[GDC_SYNC + 5] & 0x3f;			// VFP
	tmp = LOADINTELWORD(gdc.m.para + GDC_SYNC + 6);
	lf = ((tmp - 1) & 0x3ff) + 1;
	vfbs += tmp >> 10;									// VBP

	hfbs += 3;
	x = cr + hfbs;
	if (!vfbs) {
		vfbs = 1;
	}
	y = lf + vfbs;
//	TRACEOUT(("h %d:%d / v %d:%d", cr, x, lf, y));

	if (!(gdc.crt15khz & 2)) {							// 24.83±300Hz
		clk = gdcclk + 1;
	}
	else {												// 15.98±300Hz
		clk = gdcclk;
	}

	if (x < clk->minx) {
		cr = (clk->minx * cr) / x;
		x = clk->minx;
	}
	else if (x > clk->maxx) {
		cr = (clk->maxx * cr) / x;
		x = clk->maxx;
	}
	if (y < clk->miny) {
		lf = (clk->miny * lf) / y;
		y = clk->miny;
	}
	else if (y > clk->maxy) {
		lf = (clk->maxy * lf) / y;
		y = clk->maxy;
	}
	hclock = clk->clock / x;
	cnt = (pccore.baseclock * y) / hclock;
	cnt *= pccore.multiple;
	gdc.rasterclock = cnt / y;
	gdc.hsyncclock = (gdc.rasterclock * cr) / x;
	gdc.dispclock = gdc.rasterclock * lf;
	gdc.vsyncclock = cnt - gdc.dispclock;
	timing_setrate(y, hclock);
}

void gdc_restorekacmode(void) {

	BYTE	bit;

	bit = (((!np2cfg.uPD72020) && (gdc.mode1 & 0x20))?0x00:0xff);
	if (gdc.bitac != bit) {
		gdc.bitac = bit;
		gdcs.textdisp |= GDCSCRN_ALLDRAW2;
	}
}


// ---- I/O

static void IOOUTCALL gdc_o60(UINT port, REG8 dat) {

	if (gdc.m.cnt < GDCCMD_MAX) {
		gdc.m.fifo[gdc.m.cnt++] = dat;
	}
	(void)port;
}

static void IOOUTCALL gdc_o62(UINT port, REG8 dat) {

	if (gdc.m.cnt < GDCCMD_MAX) {
		gdc.m.fifo[gdc.m.cnt++] = 0x100 | dat;
	}
	gdc_work(GDCWORK_MASTER);
	(void)port;
}

static void IOOUTCALL gdc_o64(UINT port, REG8 dat) {

	gdc.vsyncint = 1;
	(void)port;
	(void)dat;
}

static void IOOUTCALL gdc_o68(UINT port, REG8 dat) {

	REG8	bit;

	if (!(dat & 0xf0)) {									// ver0.28
		bit = 1 << ((dat >> 1) & 7);
		if (dat & 1) {
			gdc.mode1 |= bit;
		}
		else {
			gdc.mode1 &= ~bit;
		}
		if (bit & (0x01 | 0x04 | 0x10)) {
			gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
		}
		else if (bit == 0x02) {								// ver0.28
			gdcs.grphdisp |= GDCSCRN_ALLDRAW2;
			gdcs.palchange = GDCSCRN_REDRAW;
		}
		else if (bit == 0x08) {
			gdcs.textdisp |= GDCSCRN_ALLDRAW2;
		}
		else if (bit == 0x20) {
			gdc_restorekacmode();
		}
		else if (bit == 0x80) {
			screenupdate |= 2;
		}
		gdcs.msw_accessable = gdc.mode1 & 0x40;
	}
	(void)port;
}

static void IOOUTCALL gdc_o6a(UINT port, REG8 dat) {

	REG8	bit;

	if (!(dat & 0xf8)) {
		bit = (dat >> 1) & 3;
		dat &= 1;
		if (!(((gdc.mode2 >> bit) ^ dat) & 1)) {
			return;
		}
		gdc.mode2 ^= (1 << bit);
		switch(bit) {
			case 0:
				if (gdc.display & 2) {
					gdc.analog = dat;
					gdcs.palchange = GDCSCRN_REDRAW;
					vramop.operate &= VOP_ANALOGMASK;
					vramop.operate |= dat << 4;
					i286_vram_dispatch(vramop.operate);
				}
				break;

			case 2:
				if ((gdc.mode2 & 0x08) && (grcg.chip == 3)) {
					vramop.operate &= VOP_EGCMASK;
					vramop.operate |= dat << 1;
					i286_vram_dispatch(vramop.operate);
				}
				break;
		}
		gdcs.mode2 = gdc.mode2;
	}
	else {
		switch(dat) {
			case 0x40:
			case 0x80:					// EPSON?
				gdc.display &= ~1;
				gdcs.textdisp |= GDCSCRN_EXT;
				break;

			case 0x41:
			case 0x81:					// EPSON?
				gdc.display |= 1;
				gdcs.textdisp |= GDCSCRN_EXT;
				break;

			case 0x82:
				gdc.clock &= ~1;
				gdcs.grphdisp |= GDCSCRN_EXT;
				break;

			case 0x83:
				gdc.clock |= 1;
				gdcs.grphdisp |= GDCSCRN_EXT;
				break;

			case 0x84:
				gdc.clock &= ~2;
				gdcs.grphdisp |= GDCSCRN_EXT;
				break;

			case 0x85:
				gdc.clock |= 2;
				gdcs.grphdisp |= GDCSCRN_EXT;
				break;
		}
	}
	(void)port;
}

static void IOOUTCALL gdc_o6e(UINT port, REG8 dat) {

	switch(dat) {
		case 0:
			gdc.crt15khz &= ~1;
			gdcs.textdisp |= GDCSCRN_ALLDRAW2;
			break;

		case 1:
			gdc.crt15khz |= 1;
			gdcs.textdisp |= GDCSCRN_ALLDRAW2;
			break;
	}
	(void)port;
}

static REG8 IOINPCALL gdc_i60(UINT port) {

	REG8	ret;
	SINT32	remain;

	ret = 0x80 | gdc.vsync;		// | m_drawing;
	remain = nevent_getremain(NEVENT_FLAMES);
	if (remain >= 0) {
		if ((remain % gdc.rasterclock) < gdc.hsyncclock) {
			ret |= 0x40;
		}
	}
	if (gdc.m.snd) {
		ret |= 0x01;
	}
	if (gdc.m.cnt >= GDCCMD_MAX) {
		ret |= 0x02;
	}
	if (!gdc.m.cnt) {
		ret |= 0x04;
	}
	else {
		gdc_work(GDCWORK_MASTER);
	}
#ifdef SEARCH_SYNC
	if ((CPU_INPADRS) && (CPU_REMCLOCK >= 5)) {
		UINT16 jadr = 0xfa74;
		UINT16 memv;
		memv = i286_memoryread_w(CPU_INPADRS);
		while((memv == 0x00eb) || (memv == 0x5fe6)) {
			jadr -= 0x200;
			CPU_INPADRS += 2;
			memv = i286_memoryread_w(CPU_INPADRS);
		}
		if ((memv == 0x20a8) || (memv == 0x2024)) {
			memv = i286_memoryread_w(CPU_INPADRS + 2);
			if (memv == jadr) {					// je
				if (!gdc.vsync) {
					CPU_REMCLOCK = -1;
				}
			}
			else if (memv == (jadr + 1)) {		// jne
				if (gdc.vsync) {
					CPU_REMCLOCK = -1;
				}
			}
		}
	}
#endif
#ifdef TURE_SYNC				// クロックイベントの誤差修正
	if (nevent.item[NEVENT_FLAMES].clock < (CPU_BASECLOCK - CPU_REMCLOCK)) {
		ret ^= 0x20;
	}
#endif
	(void)port;
	return(ret);
}

static REG8 IOINPCALL gdc_i62(UINT port) {

	if (gdc.m.snd) {
		gdc.m.snd--;
		return(gdc.m.para[gdc.m.ptr++]);
	}
	(void)port;
	return(0xff);
}

static REG8 IOINPCALL gdc_i68(UINT port) {

	(void)port;
	return(gdc.mode1);
}

static REG8 IOINPCALL gdc_i6a(UINT port) {

	(void)port;
	return(gdc.mode2);
}


static void IOOUTCALL gdc_oa0(UINT port, REG8 dat) {

	if (gdc.s.cnt < GDCCMD_MAX) {
		gdc.s.fifo[gdc.s.cnt++] = dat;
	}
//	TRACEOUT(("GDC-B %.2x", dat));
	if (gdc.s.paracb) {						// ver0.29
		gdc_work(GDCWORK_SLAVE);
	}
	(void)port;
}

static void IOOUTCALL gdc_oa2(UINT port, REG8 dat) {

	if (gdc.s.cnt < GDCCMD_MAX) {
		gdc.s.fifo[gdc.s.cnt++] = 0x100 | dat;
	}
//	TRACEOUT(("GDC-A %.2x", dat));
	gdc_work(GDCWORK_SLAVE);
	(void)port;
}

static void IOOUTCALL gdc_oa4(UINT port, REG8 dat) {

	if ((gdcs.disp ^ dat) & 1) {
		gdcs.disp = dat & 1;
		screenupdate |= 2;
	}
	(void)port;
}

static void IOOUTCALL gdc_oa6(UINT port, REG8 dat) {

	if ((gdcs.access ^ dat) & 1) {
		gdcs.access = dat & 1;
		vramop.operate &= VOP_ACCESSMASK;
		vramop.operate |= gdcs.access;
		i286_vram_dispatch(vramop.operate);
	}
	(void)port;
}

static void IOOUTCALL gdc_oa8(UINT port, REG8 dat) {

	if (gdc.analog) {
		gdc.palnum = dat & 0x0f;
	}
	else {
		gdc_setdegpalpack(3, dat);
	}
	(void)port;
}

static void IOOUTCALL gdc_oaa(UINT port, REG8 dat) {

	if (gdc.analog) {
		gdc_setanalogpal(gdc.palnum, offsetof(RGB32, p.g), dat);
	}
	else {
		gdc_setdegpalpack(1, dat);
	}
	(void)port;
}

static void IOOUTCALL gdc_oac(UINT port, REG8 dat) {

	if (gdc.analog) {
		gdc_setanalogpal(gdc.palnum, offsetof(RGB32, p.r), dat);
	}
	else {
		gdc_setdegpalpack(2, dat);
	}
	(void)port;
}

static void IOOUTCALL gdc_oae(UINT port, REG8 dat) {

	if (gdc.analog) {
		gdc_setanalogpal(gdc.palnum, offsetof(RGB32, p.b), dat);
	}
	else {
		gdc_setdegpalpack(0, dat);
	}
	(void)port;
}

static REG8 IOINPCALL gdc_ia0(UINT port) {

	REG8	ret;
	SINT32	remain;

	ret = 0x80 | gdc.vsync | gdc.s_drawing;
	remain = nevent_getremain(NEVENT_FLAMES);
	if (remain >= 0) {
		if ((remain % gdc.rasterclock) < (gdc.hsyncclock)) {
			ret |= 0x40;
		}
	}
	if (gdc.s.snd) {
		ret |= 0x01;
	}
	if (gdc.s.cnt >= GDCCMD_MAX) {
		ret |= 0x02;
	}
	if (!gdc.s.cnt) {
		ret |= 0x04;
	}
	else {
		gdc_work(GDCWORK_SLAVE);
		TRACEOUT(("gdc.s.cnt=%d", gdc.s.cnt));
	}
#ifdef SEARCH_SYNC
	if ((CPU_INPADRS) && (CPU_REMCLOCK >= 5)) {
		UINT16 jadr = 0xfa74;
		UINT16 memv;
		memv = i286_memoryread_w(CPU_INPADRS);
		while((memv == 0x00eb) || (memv == 0x5fe6)) {
			jadr -= 0x200;
			CPU_INPADRS += 2;
			memv = i286_memoryread_w(CPU_INPADRS);
		}
		if ((memv == 0x20a8) || (memv == 0x2024)) {
			memv = i286_memoryread_w(CPU_INPADRS + 2);
			if (memv == jadr) {					// je
				if (!gdc.vsync) {
					CPU_REMCLOCK = -1;
				}
			}
			else if (memv == (jadr + 1)) {		// jne
				if (gdc.vsync) {
					CPU_REMCLOCK = -1;
				}
			}
		}
	}
#endif
#ifdef TURE_SYNC				// クロックイベントの誤差修正
	if (nevent.item[NEVENT_FLAMES].clock < (CPU_BASECLOCK - CPU_REMCLOCK)) {
		ret ^= 0x20;
	}
#endif
	(void)port;
	return(ret);
}

static REG8 IOINPCALL gdc_ia2(UINT port) {

	if (gdc.s.snd) {
		gdc.s.snd--;
		return(gdc.s.para[gdc.s.ptr++]);
	}
	(void)port;
	return(0xff);
}

static REG8 IOINPCALL gdc_ia4(UINT port) {

	(void)port;
	return(gdcs.disp);
}

static REG8 IOINPCALL gdc_ia6(UINT port) {

	(void)port;
	return(gdcs.access);
}


// ---- I/F

static const IOOUT gdco60[8] = {
					gdc_o60,	gdc_o62,	gdc_o64,	NULL,
					gdc_o68,	gdc_o6a,	NULL,		gdc_o6e};

static const IOOUT gdcoa0[8] = {
					gdc_oa0,	gdc_oa2,	gdc_oa4,	gdc_oa6,
					gdc_oa8,	gdc_oaa,	gdc_oac,	gdc_oae};

static const IOINP gdci60[8] = {
					gdc_i60,	gdc_i62,	NULL,		NULL,
					gdc_i68,	gdc_i6a,	NULL,		NULL};

static const IOINP gdcia0[8] = {
					gdc_ia0,	gdc_ia2,	gdc_ia4,	gdc_ia6,
					NULL,		NULL,		NULL,		NULL};

void gdc_reset(void) {

	ZeroMemory(&gdc, sizeof(gdc));
	ZeroMemory(&gdcs, sizeof(gdcs));

	if (!(np2cfg.dipsw[0] & 0x01)) {
		gdc.mode1 = 0x98;
		gdc.m.para[GDC_CSRFORM + 0] = 0x0f;
		gdc.m.para[GDC_CSRFORM + 1] = 0xc0;
		gdc.m.para[GDC_CSRFORM + 2] = 0x7b;
		CopyMemory(gdc.m.para + GDC_SYNC, defsyncm24, 8);
		gdc.s.para[GDC_CSRFORM + 0] = 1;
		CopyMemory(gdc.s.para + GDC_SYNC, defsyncs24, 8);
	}
	else {
		gdc.crt15khz = 3;
		gdc.mode1 = 0x80;
		gdc.m.para[GDC_CSRFORM + 0] = 0x07;
		gdc.m.para[GDC_CSRFORM + 1] = 0xc0;
		gdc.m.para[GDC_CSRFORM + 2] = 0x3b;
		CopyMemory(gdc.m.para + GDC_SYNC, defsyncm15, 8);
		CopyMemory(gdc.s.para + GDC_SYNC, defsyncs15, 8);
	}

	gdc.clock = 0;
	gdc.m.para[GDC_PITCH] = 80;
	gdc.s.para[GDC_PITCH] = 40;

	gdc_paletteinit();

	gdcs.textdisp = GDCSCRN_ENABLE | GDCSCRN_ALLDRAW2 | GDCSCRN_EXT;
	gdcs.grphdisp = GDCSCRN_ALLDRAW2 | GDCSCRN_EXT;
	if (np2cfg.color16 & 1) {
		gdc.s.para[GDC_SYNC] = 0x16;
		gdc.display = 2;
	}
	gdc.bitac = 0xff;

#if 0	// bind で計算される筈
	gdc.rasterclock = pccore.realclock / 24816;
	gdc.hsyncclock = (gdc.rasterclock * 4) / 5;
	gdc.dispclock = pccore.realclock * 50 / 3102;
	gdc.vsyncclock = pccore.realclock * 5 / 3102;
#endif
}

void gdc_bind(void) {

	gdc_updateclock();
	iocore_attachsysoutex(0x0060, 0x0cf1, gdco60, 8);
	iocore_attachsysinpex(0x0060, 0x0cf1, gdci60, 8);
	iocore_attachsysoutex(0x00a0, 0x0cf1, gdcoa0, 8);
	iocore_attachsysinpex(0x00a0, 0x0cf1, gdcia0, 8);
}


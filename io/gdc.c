#include	"compiler.h"
#include	"scrnmng.h"
#include	"cpucore.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"palettes.h"
#include	"gdc_cmd.tbl"


#define	SEARHC_SYNC
#define	TURE_SYNC


static const UINT8 defdegpal[4] = {0x04,0x15,0x26,0x37};
static const UINT8 defsync[8] = {0x10,0x4e,0x07,0x25,0x07,0x07,0x90,0x65};


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

	UINT32	csrw;
	UINT16	textw;

	csrw = LOADINTELDWORD(gdc.s.para + GDC_CSRW);
	textw = LOADINTELWORD(gdc.s.para + GDC_TEXTW);

	if (gdc.s.para[GDC_VECTW] & 0x8) {
		gdcsub_line(csrw, (GDCVECT *)(gdc.s.para + GDC_VECTW),
											textw, gdc.s.para[GDC_WRITE]);
	}
	if (gdc.s.para[GDC_VECTW] & 0x20) {
		gdcsub_circle(csrw, (GDCVECT *)(gdc.s.para + GDC_VECTW),
											textw, gdc.s.para[GDC_WRITE]);
	}
	if (gdc.s.para[GDC_VECTW] & 0x40) {
		gdcsub_box(csrw, (GDCVECT *)(gdc.s.para + GDC_VECTW),
											textw, gdc.s.para[GDC_WRITE]);
	}
}

static void textdraw(void) {									// ver0.30

	UINT32	csrw;
	UINT16	textw;

	if (gdc.s.para[GDC_VECTW] & 0x10) {
		csrw = LOADINTELDWORD(gdc.s.para + GDC_CSRW);
		textw = LOADINTELWORD(gdc.s.para + GDC_TEXTW);
		gdcsub_text(csrw, (GDCVECT *)(gdc.s.para + GDC_VECTW),
											textw, gdc.s.para[GDC_WRITE]);
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
				gdc.display &= ~1;
				gdcs.textdisp |= GDCSCRN_EXT;
				break;

			case 0x41:
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

static REG8 IOINPCALL gdc_i60(UINT port) {

	REG8	ret;
	SINT32	remain;

	ret = 0x80 | gdc.vsync;		// | m_drawing;
	remain = nevent_getremain(NEVENT_FLAMES);
	if (remain >= 0) {
		if ((remain % pc.raster) < pc.hsync) {
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
#ifdef SEARHC_SYNC
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
		if ((remain % pc.raster) < (pc.hsync)) {
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
	}
#ifdef SEARHC_SYNC
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
					gdc_o68,	gdc_o6a,	NULL,		NULL};

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

	gdc.mode1 = 0x98;

	gdc.m.para[GDC_CSRFORM + 0] = 0x0f;
	gdc.m.para[GDC_CSRFORM + 1] = 0xc0;
	gdc.m.para[GDC_CSRFORM + 2] = 0x7b;
	CopyMemory(gdc.m.para + GDC_SYNC, defsync, 8);
	CopyMemory(gdc.s.para + GDC_SYNC, defsync, 8);

	gdc.s.para[GDC_CSRFORM] = 1;
	gdc.clock = 0;
	gdc.m.para[GDC_PITCH] = 80;					// ver0.26
	gdc.s.para[GDC_PITCH] = 40;

	gdc_paletteinit();

	gdcs.textdisp = GDCSCRN_ENABLE | GDCSCRN_ALLDRAW2 | GDCSCRN_EXT;
	gdcs.grphdisp = GDCSCRN_ALLDRAW2 | GDCSCRN_EXT;
	gdc.display = (np2cfg.color16 & 1) << 1;
	gdc.bitac = 0xff;
}

void gdc_bind(void) {

	iocore_attachsysoutex(0x0060, 0x0cf1, gdco60, 8);
	iocore_attachsysinpex(0x0060, 0x0cf1, gdci60, 8);
	iocore_attachsysoutex(0x00a0, 0x0cf1, gdcoa0, 8);
	iocore_attachsysinpex(0x00a0, 0x0cf1, gdcia0, 8);
}


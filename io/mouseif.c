#include	"compiler.h"
#include	"mousemng.h"
#include	"pccore.h"
#include	"iocore.h"


// マウス ver0.28
// 一部のゲームでマウスデータを切り捨てるので正常な動かなくなる事がある
// それを救う為に 均等に移動データが伝わるようにしなければならない


void mouseif_sync(void) {

	// 前回の分を補正
	mouseif.x += mouseif.rx;
	mouseif.y += mouseif.ry;

	// 今回の移動量を取得
	mouseif.b = mousemng_getstat(&mouseif.sx, &mouseif.sy, 1);
	mouseif.rx = mouseif.sx;
	mouseif.ry = mouseif.sy;

	mouseif.lastc = nevent.clock + nevent.baseclock
											- nevent.remainclock;
}

static void calc_mousexy(void) {

	UINT32	clock;
	SINT32	diff;

	clock = nevent.clock + nevent.baseclock - nevent.remainclock;
	diff = clock - mouseif.lastc;
	if (diff >= 2000) {
		SINT16 dx;
		SINT16 dy;
		mouseif.rapid ^= 0xa0;
		diff /= 1000;
		dx = (SINT16)(mouseif.sx * diff / pc.frame1000);
		if (dx >= 0) {											// 正
			if (dx > mouseif.rx) {
				dx = mouseif.rx;
			}
		}
		else {													// 負
			if (dx < mouseif.rx) {
				dx = mouseif.rx;
			}
		}
		mouseif.x += dx;
		mouseif.rx -= dx;
		dy = (SINT16)(mouseif.sy * diff / pc.frame1000);
		if (dy >= 0) {											// 正
			if (dy > mouseif.ry) {
				dy = mouseif.ry;
			}
		}
		else {													// 負
			if (dy < mouseif.ry) {
				dy = mouseif.ry;
			}
		}
		mouseif.y += dy;
		mouseif.ry -= dy;
		mouseif.lastc += diff * 1000;
	}
}

void mouseint(NEVENTITEM item) {

	if (item->flag & NEVENT_SETEVENT) {
		if (!(mouseif.portc & 0x10)) {
			pic_setirq(0x0d);
			nevent_set(NEVENT_MOUSE, pc.mouseclock << mouseif.timing,
												mouseint, NEVENT_RELATIVE);
		}
	}
}

static void setportc(BYTE value) {

	if ((value & 0x80) && (!(mouseif.portc & 0x80))) {
		calc_mousexy();
		mouseif.latch_x = mouseif.x;
		mouseif.x = 0;
		mouseif.latch_y = mouseif.y;
		mouseif.y = 0;
		if (mouseif.latch_x > 127) {
			mouseif.latch_x = 127;
		}
		else if (mouseif.latch_x < -128) {
			mouseif.latch_x = -128;
		}
		if (mouseif.latch_y > 127) {
			mouseif.latch_y = 127;
		}
		else if (mouseif.latch_y < -128) {
			mouseif.latch_y = -128;
		}
	}
	if ((value ^ mouseif.portc) & 0x10) {
		if (!(value & 0x10)) {
			if (!nevent_iswork(NEVENT_MOUSE)) {
				nevent_set(NEVENT_MOUSE, pc.mouseclock << mouseif.timing,
												mouseint, NEVENT_ABSOLUTE);
			}
		}
	}
	mouseif.portc = value;
}


// ---- I/O

static void IOOUTCALL mouseif_o7fdd(UINT port, BYTE dat) {

	setportc(dat);
	(void)port;
}

static void IOOUTCALL mouseif_o7fdf(UINT port, BYTE dat) {

	if (dat & 0xf0) {
		mouseif.mode = dat;
		if (dat == 0x93) {
			setportc(0);
		}
	}
	else {
		if (dat & 1) {
			setportc((BYTE)(mouseif.portc | (1 << (dat >> 1))));
		}
		else {
			setportc((BYTE)(mouseif.portc & (~(1 << (dat >> 1)))));
		}
	}
	(void)port;
}

static BYTE IOINPCALL mouseif_i7fd9(UINT port) {

	SINT16	x;
	SINT16	y;
	BYTE	ret;
	BYTE	portc;

	calc_mousexy();
	ret = mouseif.b & 0xf0;
	if (np2cfg.MOUSERAPID) {
		ret |= mouseif.rapid;
	}
	ret |= 0x40;
	portc = mouseif.portc;
	if (portc & 0x80) {
		x = mouseif.latch_x;
		y = mouseif.latch_y;
	}
	else {
		x = mouseif.x;
		y = mouseif.y;
	}
	if (portc & 0x40) {
		x = y;
	}
	if (!(portc & 0x20)) {
		ret |= x & 0x0f;
	}
	else {
		ret |= (x >> 4) & 0x0f;
	}
	(void)port;
	return(ret);
}

static BYTE IOINPCALL mouseif_i7fdb(UINT port) {

	(void)port;
	return(0x40);
}

static BYTE IOINPCALL mouseif_i7fdd(UINT port) {

	BYTE	ret;
	BYTE	mode;

	ret = mouseif.portc;
	mode = mouseif.mode;
	if (mode & 8) {
		ret &= 0x1f;
	}
	if (mode & 1) {
		ret &= 0xf0;
		ret |= 0x08;
		ret |= (np2cfg.dipsw[2] >> 5) & 0x04;
		ret |= ((~np2cfg.dipsw[0]) >> 4) & 0x03;
	}
	(void)port;
	return(ret);
}


static void IOOUTCALL mouseif_obfdb(UINT port, BYTE dat) {

	mouseif.timing = dat & 3;
	(void)port;
}


// ---- I/F

void mouseif_reset(void) {

	ZeroMemory(&mouseif, sizeof(mouseif));
	mouseif.mode = 0x93;
	mouseif.portc = 0x10;											// ver0.28
}

void mouseif_bind(void) {

	iocore_attachout(0x7fdd, mouseif_o7fdd);
	iocore_attachout(0x7fdf, mouseif_o7fdf);
	iocore_attachinp(0x7fd9, mouseif_i7fd9);
	iocore_attachinp(0x7fdb, mouseif_i7fdb);
	iocore_attachinp(0x7fdd, mouseif_i7fdd);
	iocore_attachout(0xbfdb, mouseif_obfdb);
}


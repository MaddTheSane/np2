#include	"compiler.h"
#include	"mousemng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"keystat.h"


// マウス ver0.28
// 一部のゲームでマウスデータを切り捨てるので正常な動かなくなる事がある
// それを救う為に 均等に移動データが伝わるようにしなければならない


void mouseif_sync(void) {

	// 前回の分を補正
	mouseif.x += mouseif.rx;
	mouseif.y += mouseif.ry;

	// 今回の移動量を取得
	mouseif.b = mousemng_getstat(&mouseif.sx, &mouseif.sy, 1);
	if (np2cfg.KEY_MODE == 3) {
		mouseif.b &= keystat_getmouse(&mouseif.sx, &mouseif.sy);
	}
	mouseif.rx = mouseif.sx;
	mouseif.ry = mouseif.sy;

	mouseif.lastc = CPU_CLOCK + CPU_BASECLOCK + CPU_REMCLOCK;
}

static void calc_mousexy(void) {

	UINT32	clock;
	SINT32	diff;

	clock = CPU_CLOCK + CPU_BASECLOCK + CPU_REMCLOCK;
	diff = clock - mouseif.lastc;
	if (diff >= 2000) {
		SINT32 dx;
		SINT32 dy;
		mouseif.rapid ^= 0xa0;
		diff /= 1000;
		dx = mouseif.sx;
		if (dx > 0) {
			dx = dx * diff / mouseif.moveclock;
			if (dx > mouseif.rx) {
				dx = mouseif.rx;
			}
		}
		else if (dx < 0) {
			dx *= -1;
			dx = dx * diff / mouseif.moveclock;
			dx *= -1;
			if (dx < mouseif.rx) {
				dx = mouseif.rx;
			}
		}
		mouseif.x += dx;
		mouseif.rx -= dx;

		dy = mouseif.sy;
		if (dy > 0) {
			dy = dy * diff / mouseif.moveclock;
			if (dy > mouseif.ry) {
				dy = mouseif.ry;
			}
		}
		else if (dy < 0) {
			dy *= -1;
			dy = dy * diff / mouseif.moveclock;
			dy *= -1;
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
			TRACEOUT(("mouse int"));
			nevent_set(NEVENT_MOUSE, mouseif.intrclock << mouseif.timing,
												mouseint, NEVENT_RELATIVE);
		}
	}
}

static void setportc(REG8 value) {

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
				nevent_set(NEVENT_MOUSE, mouseif.intrclock << mouseif.timing,
												mouseint, NEVENT_ABSOLUTE);
			}
		}
	}
	mouseif.portc = (UINT8)value;
}


// ---- I/O

static void IOOUTCALL mouseif_o7fdd(UINT port, REG8 dat) {

	setportc(dat);
	(void)port;
}

static void IOOUTCALL mouseif_o7fdf(UINT port, REG8 dat) {

	if (!(dat & 0xf0)) {
		if (dat & 1) {
			setportc((REG8)(mouseif.portc | (1 << (dat >> 1))));
		}
		else {
			setportc((REG8)(mouseif.portc & (~(1 << (dat >> 1)))));
		}
	}
	else if (dat & 0x80) {
		mouseif.mode = (UINT8)dat;
		setportc(0);
	}
	(void)port;
}

static REG8 IOINPCALL mouseif_i7fd9(UINT port) {

	SINT16	x;
	SINT16	y;
	REG8	ret;
	REG8	portc;

	calc_mousexy();
	ret = mouseif.b;
	if (np2cfg.MOUSERAPID) {
		ret |= mouseif.rapid;
	}
	ret &= 0xf0;
	ret |= 0x50;
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
	TRACEOUT(("mouse read %x", ret));
	return(ret);
}

static REG8 IOINPCALL mouseif_i7fdb(UINT port) {

	(void)port;
	return(0x40);
}

static REG8 IOINPCALL mouseif_i7fdd(UINT port) {

	REG8	ret;
	REG8	mode;

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


static void IOOUTCALL mouseif_obfdb(UINT port, REG8 dat) {

	mouseif.timing = dat & 3;
	(void)port;
}


// ---- I/F

void mouseif_reset(void) {

	ZeroMemory(&mouseif, sizeof(mouseif));
	mouseif.mode = 0x93;
	mouseif.portc = 0x10;
	mouseif.intrclock = pccore.realclock / 120;
	mouseif.moveclock = pccore.realclock / 56400;
}

void mouseif_bind(void) {

	iocore_attachout(0x7fdd, mouseif_o7fdd);
	iocore_attachout(0x7fdf, mouseif_o7fdf);
	iocore_attachinp(0x7fd9, mouseif_i7fd9);
	iocore_attachinp(0x7fdb, mouseif_i7fdb);
	iocore_attachinp(0x7fdd, mouseif_i7fdd);
	iocore_attachout(0xbfdb, mouseif_obfdb);
}


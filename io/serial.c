#include	"compiler.h"
#include	"dosio.h"
#include	"commng.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"


static const BYTE joykeytable[12] = {
				0x2a,	0x34,
				0x29,	0x1c,
				0x3c,	0x48,
				0x3b,	0x46,
				0x3d,	0x4b,
				0x3a,	0x43};

static const BYTE kbexflag[0x80] = {
		//	 ESC,  ÇP,  ÇQ,  ÇR,  ÇS,  ÇT,  ÇU,  ÇV		; 00h
			   0,   0,   0,   0,   0,   0,   0,   0,
		//	  ÇW,  ÇX,  ÇO,  Å|,  ÅO,  Åè,  BS, TAB		; 08h
			   0,   0,   0,   0,   0,   0,   0,   0,
		//	  Çp,  Çv,  Çd,  Çq,  Çs,  Çx,  Çt,  Çh		; 10h
			   0,   0,   0,   0,   0,   0,   0,   0,
		//	  Çn,  Ço,  Åó,  Åm, Ret,  Ç`,  Çr,  Çc		; 18h
			   0,   0,   0,   0,   1,   0,   0,   0,
		//	  Çe,  Çf,  Çg,  Çi,  Çj,  Çk,  ÅG,  ÅF		; 20h
			   0,   0,   0,   0,   0,   0,   0,   0,
		//    Ån,  Çy,  Çw,  Çb,  Çu,  Ça,  Çm,  Çl		; 28h
			   0,   1,   1,   0,   0,   0,   0,   0,
		//    ÅC,  ÅD,  Å^,  ÅQ, SPC,XFER,RLUP,RLDN		; 30h
			   0,   0,   0,   0,   1,   0,   0,   0,
		//	 INS, DEL,  Å™,  Å©,  Å®,  Å´,HMCR,HELP		; 38h
			   2,   0,   1,   1,   1,   1,   0,   0,
		//	<Å|>,<Å^>,<ÇV>,<ÇW>,<ÇX>,<Åñ>,<ÇS>,<ÇT>		; 40h
			   0,   0,   0,   1,   0,   0,   1,   0,
		//	<ÇU>,<Å{>,<ÇP>,<ÇQ>,<ÇR>,<ÅÅ>,<ÇO>,<ÅC>		; 48h
			   1,   0,   0,   1,   0,   0,   0,   0,
		//	<ÅD>,NFER,vf.1,vf.2,vf.3,vf.4,vf.5,   		; 50h
			   0,   0,   2,   2,   2,   2,   2,   0,
		//	    ,    ,    ,    ,    ,    ,HOME,   		; 58h
			   0,   0,   0,   0,   0,   0,   0,   0,
		//	STOP,COPY, f.1, f.2, f.3, f.4, f.5, f.6		; 60h
			   0,   0,   2,   2,   2,   2,   2,   2,
		//	 f.7, f.8, f.9, f10,    ,    ,    ,   		; 68h
			   2,   2,   2,   2,   0,   0,   0,   0,
		//	 SFT,CAPS,KANA,GRPH,CTRL,    ,    ,   		; 70h
			   2,   2,   2,   2,   2,   0,   0,   0,
		//	    ,    ,    ,    ,    ,    ,    ,   		; 78h
			   0,   0,   0,   0,   0,   0,   0,   0};


static void keyb_int(BOOL absolute) {

	if (keyb.buffers) {
		if (!((pic.pi[0].irr | pic.pi[0].isr) & PIC_KEYBOARD)) {
			keyb.status |= 2;
			keyb.data = keyb.buf[keyb.pos];
			keyb.pos = (keyb.pos + 1) & KB_BUFMASK;
			keyb.buffers--;
			pic_setirq(1);
		}
		nevent_set(NEVENT_KEYBOARD, pc.keyboardclock,
											keyb_callback, absolute);
	}
	else {
		keyb.status &= ~2;
	}
}

void keyb_callback(NEVENTITEM item) {

	if (item->flag & NEVENT_SETEVENT) {
		keyb_int(NEVENT_RELATIVE);
	}
}

static void keyb_out(BYTE data) {

	if (keyb.buffers < KB_BUF) {
		keyb.buf[(keyb.pos + keyb.buffers) & KB_BUFMASK] = data;
		keyb.buffers++;
		if (!nevent_iswork(NEVENT_KEYBOARD)) {
			keyb_int(NEVENT_ABSOLUTE);
		}
	}
	else {
		keyb.status |= 0x10;
	}
}


// ----

	BYTE	keystat[0x80];

void keystat_reset(void) {

	ZeroMemory(keystat, sizeof(keystat));
}

void keystat_senddata(BYTE data) {

	BYTE	key = data & 0x7f;
	BOOL	keynochange = FALSE;

	if ((key == 0x71) || (key == 0x72)) {
		if (data & 0x80) {
			return;
		}
		data = key | (keystat[key] & 0x80);
		keystat[key] ^= 0x80;
	}
	else {
		if ((np2cfg.XSHIFT) &&
			((key == 0x70) && (np2cfg.XSHIFT & 1)) ||
			((key == 0x74) && (np2cfg.XSHIFT & 2)) ||
			((key == 0x73) && (np2cfg.XSHIFT & 4))) {
			if (data & 0x80) {
				return;
			}
			data = key | (keystat[key] & 0x80);
			keystat[key] ^= 0x80;
		}
		else {
			if (!((keystat[key] ^ data) & 0x80)) {
				keystat[key] ^= 0x80;
			}
			else {
				keynochange = TRUE;
				if (kbexflag[key] & 2) {			// ÉLÅ[ÉäÉsÅ[Égñ≥Çµ
					return;
				}
			}
		}
	}
	if ((!np2cfg.KEY_MODE) || (!(kbexflag[key] & 1))) {
		if (keynochange) {
			if (data & 0x80) {						// ver0.30
				return;
			}
			keyb_out((BYTE)(data ^ 0x80));
		}
		keyb_out(data);
	}
}

void keystat_resetcopyhelp(void) {

	BYTE	i;

	for (i=0x60; i<0x62; i++) {
		if (keystat[i] & 0x80) {
			keystat[i] &= 0x7f;
			keyb_out((BYTE)(i | 0x80));
		}
	}
}

void keystat_forcerelease(BYTE value) {

	if (keystat[value & 0x7f] & 0x80) {
		keystat[value & 0x7f] &= ~0x80;
		keyb_out((BYTE)(value | 0x80));
	}
}

void keystat_resetjoykey(void) {

	int		i;
	BYTE	key;

	for (i=0; i<12; i++) {
		key = joykeytable[i];
		if (keystat[key] & 0x80) {
			keystat[key] &= 0x7f;
			keyb_out((BYTE)(key | 0x80));
		}
	}
}

BYTE keystat_getjoy(void) {

	BYTE	ret = 0xff;
	BYTE	*p = (BYTE *)joykeytable;
	BYTE	bit;

	for (bit=0x20; bit; bit>>=1, p+=2) {
		if ((keystat[*p] & 0x80) || (keystat[*(p+1)] & 0x80)) {
			ret &= ~bit;
		}
	}
	return(ret);
}


// ----

static void IOOUTCALL keyb_o41(UINT port, BYTE dat) {

	keyb.mode = dat;
	(void)port;
}

static void IOOUTCALL keyb_o43(UINT port, BYTE dat) {

	if ((!(dat & 0x08)) && (keyb.cmd & 0x08)) {
		keyboard_resetsignal();
	}
	if (dat & 0x10) {
		keyb.status &= ~(0x38);
	}
	keyb.cmd = dat;
	(void)port;
}

static BYTE IOINPCALL keyb_i41(UINT port) {

	(void)port;
	return(keyb.data);
}

static BYTE IOINPCALL keyb_i43(UINT port) {

	(void)port;
	return(keyb.status);
}


// ----

static const IOOUT keybo41[2] = {
					keyb_o41,	keyb_o43};

static const IOINP keybi41[2] = {
					keyb_i41,	keyb_i43};


void keyboard_reset(void) {

	ZeroMemory(&keyb, sizeof(keyb));
	keyb.data = 0xff;
	keyb.mode = 0x5e;
}

void keyboard_bind(void) {

	iocore_attachsysoutex(0x0041, 0x0cf1, keybo41, 2);
	iocore_attachsysinpex(0x0041, 0x0cf1, keybi41, 2);
}

void keyboard_resetsignal(void) {									// ver0.29

	int		i;

	keyboard_reset();
	for (i=0; i<0x80; i++) {
		if (keystat[i]) {
			keyb_out((BYTE)i);
		}
	}
}


// -----------------------------------------------------------------------


	COMMNG	cm_rs232c;

void rs232c_construct(void) {

	cm_rs232c = NULL;
}

void rs232c_destruct(void) {

	commng_destroy(cm_rs232c);
	cm_rs232c = NULL;
}

void rs232c_open(void) {

	if (cm_rs232c == NULL) {
		cm_rs232c = commng_create(COMCREATE_SERIAL);
	}
}

void rs232c_callback(void) {

	BOOL	interrupt;

	if ((cm_rs232c) && (cm_rs232c->read(cm_rs232c, &rs232c.data))) {
		rs232c.result |= 2;
		if (sysport.c & 1) {
			interrupt = TRUE;
		}
	}
	else {
		rs232c.result &= (BYTE)~2;
	}
	if (sysport.c & 4) {
		if (rs232c.send) {
			rs232c.send = 0;
			interrupt = TRUE;
		}
	}
	if (interrupt) {
		pic_setirq(4);
	}
}

BYTE rs232c_stat(void) {

	if (cm_rs232c == NULL) {
		cm_rs232c = commng_create(COMCREATE_SERIAL);
	}
	return(cm_rs232c->getstat(cm_rs232c));
}

void rs232c_midipanic(void) {

	if (cm_rs232c) {
		cm_rs232c->msg(cm_rs232c, COMMSG_MIDIRESET, 0);
	}
}


// ----

static void IOOUTCALL rs232c_o30(UINT port, BYTE dat) {

	if (cm_rs232c) {
		cm_rs232c->write(cm_rs232c, dat);
	}
	if (sysport.c & 4) {
		rs232c.send = 0;
		pic_setirq(4);
	}
	else {
		rs232c.send = 1;
	}
	(void)port;
}

static void IOOUTCALL rs232c_o32(UINT port, BYTE dat) {

	if (!(dat & 0xfd)) {
		rs232c.dummyinst++;
	}
	else {
		if ((rs232c.dummyinst >= 3) && (dat == 0x40)) {
			rs232c.pos = 0;
		}
		rs232c.dummyinst = 0;
	}
	switch(rs232c.pos) {
		case 0x00:			// reset
			rs232c.pos++;
			break;

		case 0x01:			// mode
			if (!(dat & 0x03)) {
				rs232c.mul = 10 * 16;
			}
			else {
				rs232c.mul = ((dat >> 1) & 6) + 10;
				if (dat & 0x10) {
					rs232c.mul += 2;
				}
				switch(dat & 0xc0) {
					case 0x80:
						rs232c.mul += 3;
						break;
					case 0xc0:
						rs232c.mul += 4;
						break;
					default:
						rs232c.mul += 2;
						break;
				}
				switch(dat & 0x03) {
					case 0x01:
						rs232c.mul >>= 1;
						break;
					case 0x03:
						rs232c.mul *= 32;
						break;
					default:
						rs232c.mul *= 8;
						break;
				}
			}
			rs232c.pos++;
			break;

		case 0x02:			// cmd
			rs232c.pos++;
			break;
	}
	(void)port;
}

static BYTE IOINPCALL rs232c_i30(UINT port) {

	(void)port;
	return(rs232c.data);
}

static BYTE IOINPCALL rs232c_i32(UINT port) {

	if (!(rs232c_stat() & 0x20)) {
		return(rs232c.result | 0x80);
	}
	else {
		(void)port;
		return(rs232c.result);
	}
}


// ----

static const IOOUT rs232co30[2] = {
					rs232c_o30,	rs232c_o32};

static const IOINP rs232ci30[2] = {
					rs232c_i30,	rs232c_i32};

void rs232c_reset(void) {

	commng_destroy(cm_rs232c);
	cm_rs232c = NULL;
	rs232c.result = 0x05;
	rs232c.data = 0xff;
	rs232c.send = 1;
	rs232c.pos = 0;
	rs232c.dummyinst = 0;
	rs232c.mul = 10 * 16;
}

void rs232c_bind(void) {

	iocore_attachsysoutex(0x0030, 0x0cf1, rs232co30, 2);
	iocore_attachsysinpex(0x0030, 0x0cf1, rs232ci30, 2);
}


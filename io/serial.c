#include	"compiler.h"
#include	"commng.h"
#include	"pccore.h"
#include	"iocore.h"


static const UINT8 joykeytable[12] = {
				0x2a,	0x34,
				0x29,	0x1c,
				0x3c,	0x48,
				0x3b,	0x46,
				0x3d,	0x4b,
				0x3a,	0x43};

static const UINT8 kbexflag[0x80] = {
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
		nevent_set(NEVENT_KEYBOARD, pccore.keyboardclock,
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

static void keyb_out(REG8 data) {

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

	UINT8	keystat[0x80];

void keystat_reset(void) {

	ZeroMemory(keystat, sizeof(keystat));
}


void keystat_senddata(REG8 data) {

	REG8	key;
	BOOL	keynochange;

	key = data & 0x7f;
	keynochange = FALSE;

	// CTRL:ÉJÉi 0x71,0x72 bit7==0Ç≈ÉgÉOÉãèàóù (ïWèÄèàóù)
	if ((key == 0x71) || (key == 0x72)) {
		if (data & 0x80) {
			return;
		}
		data = key | (keystat[key] & 0x80);
		keystat[key] ^= 0x80;
	}
	else {
		if ((np2cfg.XSHIFT) &&
			(((key == 0x70) && (np2cfg.XSHIFT & 1)) ||
			((key == 0x74) && (np2cfg.XSHIFT & 2)) ||
			((key == 0x73) && (np2cfg.XSHIFT & 4)))) {
			if (data & 0x80) {
				return;
			}
			data = key | (keystat[key] & 0x80);
			keystat[key] ^= 0x80;
		}
		else {
			// CTRL:ÉJÉi 0x79,0x7a bit7ÇÇªÇÃÇ‹Ç‹í ím
			// (ÉnÅ[ÉhÉEÉFÉAÇ≈ÉÅÉJÉjÉJÉãèàóùÇµÇƒÇÈèÍçá)
			if ((key == 0x79) || (key == 0x7a)) {
				key -= 0x08;
				data -= 0x08;
			}
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
			keyb_out((REG8)(data ^ 0x80));
		}
		keyb_out(data);
	}
}

void keystat_resetcopyhelp(void) {

	REG8	i;

	for (i=0x60; i<0x62; i++) {
		if (keystat[i] & 0x80) {
			keystat[i] &= 0x7f;
			keyb_out((REG8)(i | 0x80));
		}
	}
}

void keystat_allrelease(void) {

	REG8	i;

	for (i=0; i<0x80; i++) {
		if (keystat[i] & 0x80) {
			keystat[i] &= ~0x80;
			keyb_out((REG8)(i | 0x80));
		}
	}
}

void keystat_forcerelease(REG8 value) {

	if (keystat[value & 0x7f] & 0x80) {
		keystat[value & 0x7f] &= ~0x80;
		keyb_out((REG8)(value | 0x80));
	}
}

void keystat_resetjoykey(void) {

	int		i;
	REG8	key;

	for (i=0; i<12; i++) {
		key = joykeytable[i];
		if (keystat[key] & 0x80) {
			keystat[key] &= 0x7f;
			keyb_out((REG8)(key | 0x80));
		}
	}
}


// ----

typedef struct {
	UINT8	joysync;
	UINT8	joylast;
	UINT8	mouselast;
	UINT8	padding;
	UINT8	d_up;
	UINT8	d_dn;
	UINT8	d_lt;
	UINT8	d_rt;
} KEYEXT;

static	KEYEXT	keyext;
static const UINT8 mousedelta[] = {1, 1, 1, 1,
									2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 4};
#define	MOUSESTEPMAX ((sizeof(mousedelta) / sizeof(UINT8)) - 1)

void keyext_flash(void) {

	keyext.joysync = 0;
}

BYTE keyext_getjoy(void) {

	BYTE	flg;
const BYTE	*p;
	BYTE	bit;

	if (!keyext.joysync) {
		keyext.joysync = 1;
		flg = 0xff;
		p = joykeytable;
		for (bit=0x20; bit; bit>>=1) {
			if ((keystat[p[0]] & 0x80) || (keystat[p[1]] & 0x80)) {
				flg ^= bit;
			}
			p += 2;
		}
		keyext.joylast = flg;
	}
	return(keyext.joylast);
}

BYTE keyext_getmouse(SINT16 *x, SINT16 *y) {

	BYTE	btn;
	BYTE	acc;
	SINT16	tmp;
	BYTE	ret;

	btn = keyext_getjoy();
	acc = btn | keyext.mouselast;
	keyext.mouselast = btn;
	tmp = 0;
	if (!(btn & 1)) {
		tmp -= mousedelta[keyext.d_up];
	}
	if (!(acc & 1)) {
		if (keyext.d_up < MOUSESTEPMAX) {
			keyext.d_up++;
		}
	}
	else {
		keyext.d_up = 0;
	}
	if (!(btn & 2)) {
		tmp += mousedelta[keyext.d_dn];
	}
	if (!(acc & 2)) {
		if (keyext.d_dn < MOUSESTEPMAX) {
			keyext.d_dn++;
		}
	}
	else {
		keyext.d_dn = 0;
	}
	*y += tmp;

	tmp = 0;
	if (!(btn & 4)) {
		tmp -= mousedelta[keyext.d_lt];
	}
	if (!(acc & 4)) {
		if (keyext.d_lt < MOUSESTEPMAX) {
			keyext.d_lt++;
		}
	}
	else {
		keyext.d_lt = 0;
	}
	if (!(btn & 8)) {
		tmp += mousedelta[keyext.d_rt];
	}
	if (!(acc & 8)) {
		if (keyext.d_rt < MOUSESTEPMAX) {
			keyext.d_rt++;
		}
	}
	else {
		keyext.d_rt = 0;
	}
	*x += tmp;

	ret = 0x5f;
	ret += (btn & 0x10) << 3;
	ret += (btn & 0x20);
	return(ret);
}


// ----

static void IOOUTCALL keyb_o41(UINT port, REG8 dat) {

	keyb.mode = dat;
	(void)port;
}

static void IOOUTCALL keyb_o43(UINT port, REG8 dat) {

	if ((!(dat & 0x08)) && (keyb.cmd & 0x08)) {
		keyboard_resetsignal();
	}
	if (dat & 0x10) {
		keyb.status &= ~(0x38);
	}
	keyb.cmd = dat;
	(void)port;
}

static REG8 IOINPCALL keyb_i41(UINT port) {

	(void)port;
	return(keyb.data);
}

static REG8 IOINPCALL keyb_i43(UINT port) {

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
			keyb_out((REG8)i);
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

	interrupt = FALSE;
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

static void IOOUTCALL rs232c_o30(UINT port, REG8 dat) {

	if (cm_rs232c) {
		cm_rs232c->write(cm_rs232c, (UINT8)dat);
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

static void IOOUTCALL rs232c_o32(UINT port, REG8 dat) {

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

static REG8 IOINPCALL rs232c_i30(UINT port) {

	(void)port;
	return(rs232c.data);
}

static REG8 IOINPCALL rs232c_i32(UINT port) {

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


#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"keystat.h"


static	UINT8	keystat[0x80];


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


// ----

void keystat_reset(void) {

	ZeroMemory(keystat, sizeof(keystat));
}

void keystat_senddata(REG8 data) {

	REG8		key;
	BOOL		keynochange;
const _NKEYM	*user;
	UINT		i;

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
	else if ((key == 0x76) || (key == 0x77)) {		// user key
		user = np2cfg.userkey + (key - 0x76);
		for (i=0; i<user->keys; i++) {
			key = user->key[i] & 0x7f;
			if (!((keystat[key] ^ data) & 0x80)) {
				keystat[key] ^= 0x80;
				keyboard_send((REG8)(key | (data & 0x80)));
			}
		}
		return;
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
			keyboard_send((REG8)(data ^ 0x80));
		}
		keyboard_send(data);
	}
}

void keystat_forcerelease(REG8 value) {

	REG8		key;
const _NKEYM	*user;
	UINT		i;

	key = value & 0x7f;
	if ((key != 0x76) && (key != 0x77)) {
		if (keystat[key] & 0x80) {
			keystat[key] &= ~0x80;
			keyboard_send((REG8)(key | 0x80));
		}
	}
	else {
		user = np2cfg.userkey + (key - 0x76);
		for (i=0; i<user->keys; i++) {
			key = user->key[i] & 0x7f;
			if (keystat[key] & 0x80) {
				keystat[key] &= ~0x80;
				keyboard_send((REG8)(key | 0x80));
			}
		}
	}
}

void keystat_resetcopyhelp(void) {

	REG8	i;

	for (i=0x60; i<0x62; i++) {
		if (keystat[i] & 0x80) {
			keystat[i] &= 0x7f;
			keyboard_send((REG8)(i | 0x80));
		}
	}
}

void keystat_allrelease(void) {

	REG8	i;

	for (i=0; i<0x80; i++) {
		if (keystat[i] & 0x80) {
			keystat[i] &= ~0x80;
			keyboard_send((REG8)(i | 0x80));
		}
	}
}

void keystat_resetjoykey(void) {

	int		i;
	REG8	key;

	for (i=0; i<12; i++) {
		key = joykeytable[i];
		if (keystat[key] & 0x80) {
			keystat[key] &= 0x7f;
			keyboard_send((REG8)(key | 0x80));
		}
	}
}

void keystat_resendstat(void) {

	int		i;

	for (i=0; i<0x80; i++) {
		if (keystat[i]) {
			keyboard_send((REG8)i);
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

void keystat_sync(void) {

	keyext.joysync = 0;
}

REG8 keystat_getjoy(void) {

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

REG8 keystat_getmouse(SINT16 *x, SINT16 *y) {

	REG8	btn;
	BYTE	acc;
	SINT16	tmp;
	BYTE	ret;

	btn = keystat_getjoy();
	acc = btn | keyext.mouselast;
	keyext.mouselast = (UINT8)btn;
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


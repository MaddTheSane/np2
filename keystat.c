#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"keystat.h"
#include	"keystat.tbl"


typedef struct {
	UINT8	refer[0x80];
	UINT8	joysync;
	UINT8	joylast;
	UINT8	mouselast;
	UINT8	padding;
	UINT8	d_up;
	UINT8	d_dn;
	UINT8	d_lt;
	UINT8	d_rt;
} KEYSTAT;

static	KEYSTAT		keystat;


// ----

void keystat_reset(void) {

	ZeroMemory(keystat.refer, sizeof(keystat.refer));
}

void keystat_senddata(REG8 data) {

	REG8		key;
	BOOL		keynochange;
const _NKEYM	*user;
	UINT		i;

	key = data & 0x7f;
	keynochange = FALSE;

	// CTRL:カナ 0x71,0x72 bit7==0でトグル処理 (標準処理)
	if ((key == 0x71) || (key == 0x72)) {
		if (data & 0x80) {
			return;
		}
		data = key | (keystat.refer[key] & 0x80);
		keystat.refer[key] ^= 0x80;
	}
	else if ((key == 0x76) || (key == 0x77)) {		// user key
		user = np2cfg.userkey + (key - 0x76);
		for (i=0; i<user->keys; i++) {
			key = user->key[i] & 0x7f;
			if (!((keystat.refer[key] ^ data) & 0x80)) {
				keystat.refer[key] ^= 0x80;
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
			data = key | (keystat.refer[key] & 0x80);
			keystat.refer[key] ^= 0x80;
		}
		else {
			// CTRL:カナ 0x79,0x7a bit7をそのまま通知
			// (ハードウェアでメカニカル処理してる場合)
			if ((key == 0x79) || (key == 0x7a)) {
				key -= 0x08;
				data -= 0x08;
			}
			if (!((keystat.refer[key] ^ data) & 0x80)) {
				keystat.refer[key] ^= 0x80;
			}
			else {
				keynochange = TRUE;
				if (kbexflag[key] & 2) {			// キーリピート無し
					return;
				}
			}
		}
	}
	if ((!np2cfg.KEY_MODE) || (!(kbexflag[key] & 1))) {
		if (keynochange) {
			if (data & 0x80) {
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
		if (keystat.refer[key] & 0x80) {
			keystat.refer[key] &= ~0x80;
			keyboard_send((REG8)(key | 0x80));
		}
	}
	else {
		user = np2cfg.userkey + (key - 0x76);
		for (i=0; i<user->keys; i++) {
			key = user->key[i] & 0x7f;
			if (keystat.refer[key] & 0x80) {
				keystat.refer[key] &= ~0x80;
				keyboard_send((REG8)(key | 0x80));
			}
		}
	}
}

void keystat_allrelease(void) {

	REG8	i;

	for (i=0; i<0x80; i++) {
		if (keystat.refer[i] & 0x80) {
			keystat.refer[i] &= ~0x80;
			keyboard_send((REG8)(i | 0x80));
		}
	}
}

void keystat_resetjoykey(void) {

	int		i;
	REG8	key;

	for (i=0; i<12; i++) {
		key = joykeytable[i];
		if (keystat.refer[key] & 0x80) {
			keystat.refer[key] &= 0x7f;
			keyboard_send((REG8)(key | 0x80));
		}
	}
}

void keystat_resendstat(void) {

	int		i;

	for (i=0; i<0x80; i++) {
		if (keystat.refer[i]) {
			keyboard_send((REG8)i);
		}
	}
}


// ----

void keystat_sync(void) {

	keystat.joysync = 0;
}

REG8 keystat_getjoy(void) {

	UINT8	flg;
const BYTE	*p;
	UINT8	bit;

	if (!keystat.joysync) {
		keystat.joysync = 1;
		flg = 0xff;
		p = joykeytable;
		for (bit=0x20; bit; bit>>=1) {
			if ((keystat.refer[p[0]] & 0x80) ||
				(keystat.refer[p[1]] & 0x80)) {
				flg ^= bit;
			}
			p += 2;
		}
		keystat.joylast = flg;
	}
	return(keystat.joylast);
}

REG8 keystat_getmouse(SINT16 *x, SINT16 *y) {

	REG8	btn;
	UINT8	acc;
	SINT16	tmp;
	REG8	ret;

	btn = keystat_getjoy();
	acc = btn | keystat.mouselast;
	keystat.mouselast = (UINT8)btn;
	tmp = 0;
	if (!(btn & 1)) {
		tmp -= mousedelta[keystat.d_up];
	}
	if (!(acc & 1)) {
		if (keystat.d_up < MOUSESTEPMAX) {
			keystat.d_up++;
		}
	}
	else {
		keystat.d_up = 0;
	}
	if (!(btn & 2)) {
		tmp += mousedelta[keystat.d_dn];
	}
	if (!(acc & 2)) {
		if (keystat.d_dn < MOUSESTEPMAX) {
			keystat.d_dn++;
		}
	}
	else {
		keystat.d_dn = 0;
	}
	*y += tmp;

	tmp = 0;
	if (!(btn & 4)) {
		tmp -= mousedelta[keystat.d_lt];
	}
	if (!(acc & 4)) {
		if (keystat.d_lt < MOUSESTEPMAX) {
			keystat.d_lt++;
		}
	}
	else {
		keystat.d_lt = 0;
	}
	if (!(btn & 8)) {
		tmp += mousedelta[keystat.d_rt];
	}
	if (!(acc & 8)) {
		if (keystat.d_rt < MOUSESTEPMAX) {
			keystat.d_rt++;
		}
	}
	else {
		keystat.d_rt = 0;
	}
	*x += tmp;

	ret = 0x5f;
	ret += (btn & 0x10) << 3;
	ret += (btn & 0x20);
	return(ret);
}


#include	"compiler.h"
#include	"dosio.h"
#include	"textfile.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"keystat.h"
#include	"keystat.tbl"


typedef struct {
	UINT8	ref[NKEY_SYSTEM];
	UINT8	extkey;
	UINT8	mouselast;
	UINT8	padding;
	UINT8	d_up;
	UINT8	d_dn;
	UINT8	d_lt;
	UINT8	d_rt;
} KEYSTAT;


		NKEYTBL		nkeytbl;
static	KEYSTAT		keystat;


void keystat_initialize(void) {

	char	path[MAX_PATH];

	ZeroMemory(&keystat, sizeof(keystat));
	FillMemory(keystat.ref, sizeof(keystat.ref), NKEYREF_NC);
	keystat_tblreset();
	getbiospath(path, "key.txt", sizeof(path));
	keystat_tblload(path);
}

void keystat_tblreset(void) {

	UINT	i;

	ZeroMemory(&nkeytbl, sizeof(nkeytbl));
	for (i=0; i<0x80; i++) {
		nkeytbl.key[i].keys = 1;
		nkeytbl.key[i].key[0] = (UINT8)i;
	}
	for (i=0; i<0x10; i++) {
		nkeytbl.key[i+0x80].keys = 1;
		nkeytbl.key[i+0x80].key[0] = (UINT8)(i + 0xf0);
	}
}

void keystat_tblset(REG8 ref, const UINT8 *key, UINT cnt) {

	NKEYM	*nkey;

	if ((ref >= NKEY_USER) && (ref < (NKEY_USER + NKEY_USERKEYS))) {
		nkey = (NKEYM *)(nkeytbl.user + (ref - NKEY_USER));
		cnt = min(cnt, 15);
	}
	else if (ref < NKEY_SYSTEM) {
		nkey = (NKEYM *)(nkeytbl.key + ref);
		cnt = min(cnt, 3);
	}
	else {
		return;
	}
	nkey->keys = (UINT8)cnt;
	if (cnt) {
		CopyMemory(nkey->key, key, cnt);
	}
}


// ---- config...

static const char str_userkey1[] = "userkey1";
static const char str_userkey2[] = "userkey2";

static REG8 searchkeynum(const char *str, BOOL user) {

const KEYNAME	*n;
const KEYNAME	*nterm;

	n = keyname;
	nterm = keyname + (sizeof(keyname) / sizeof(KEYNAME));
	while(n < nterm) {
		if (!milstr_cmp(n->str, str)) {
			return(n->num);
		}
		n++;
	}
	if (user) {
		if (!milstr_cmp(str_userkey1, str)) {
			return(NKEY_USER + 0);
		}
		if (!milstr_cmp(str_userkey2, str)) {
			return(NKEY_USER + 1);
		}
	}
	return(0xff);
}

void keystat_tblload(const char *filename) {

	TEXTFILEH	tfh;
	char		work[256];
	char		*p;
	char		*q;
	char		*r;
	UINT8		ref;
	UINT8		key[15];
	UINT		cnt;

	tfh = textfile_open(filename, 0x800);
	if (tfh == NULL) {
		goto kstbl_err;
	}
	while(textfile_read(tfh, work, sizeof(work)) == SUCCESS) {
		p = milstr_nextword(work);
		q = milstr_chr(p, '\t');
		if (q == NULL) {
			q = milstr_chr(p, '=');
		}
		if (q == NULL) {
			continue;
		}
		*q++ = '\0';
		r = milstr_chr(p, ' ');
		if (r != NULL) {
			*r = '\0';
		}
		ref = searchkeynum(p, TRUE);
		if (ref == 0xff) {
			continue;
		}
		cnt = 0;
		while((q) && (cnt < 16)) {
			p = milstr_nextword(q);
			q = milstr_chr(p, ' ');
			if (q != NULL) {
				*q++ = '\0';
			}
			key[cnt] = searchkeynum(p, FALSE);
			if (key[cnt] != 0xff) {
				cnt++;
			}
		}
		keystat_tblset(ref, key, cnt);
	}
	textfile_close(tfh);

kstbl_err:
	return;
}


// ----

void keystat_down(const UINT8 *key, REG8 keys, REG8 ref) {

	UINT8	keydata;
	UINT8	keycode;
	REG8	data;

	while(keys--) {
		keydata = *key++;
		keycode = (keydata & 0x7f);
		if (keycode < 0x70) {
			if ((keystat.ref[keycode] == NKEYREF_NC) ||
				(!(kbexflag[keycode] & KBEX_NONREP))) {
				keyboard_send(keycode);
			}
			keystat.ref[keycode] = ref;
		}
		else {
			if ((np2cfg.XSHIFT) &&
				(((keycode == 0x70) && (np2cfg.XSHIFT & 1)) ||
				((keycode == 0x74) && (np2cfg.XSHIFT & 2)) ||
				((keycode == 0x73) && (np2cfg.XSHIFT & 4)))) {
				keydata |= 0x80;
			}
			if (!(keydata & 0x80)) {			// �V�t�g
				if ((keystat.ref[keycode + 0x00] == NKEYREF_NC) &&
					(keystat.ref[keycode + 0x10] == NKEYREF_NC)) {
					keyboard_send(keycode);
				}
				if (keystat.ref[keycode] > ref) {
					keystat.ref[keycode] = ref;
				}
			}
			else {								// �V�t�g���J�j�J������
				if (keystat.ref[keycode + 0x10] == NKEYREF_NC) {
					keystat.ref[keycode + 0x10] = ref;
					data = keycode;
				}
				else {
					keystat.ref[keycode + 0x10] = NKEYREF_NC;
					data = (REG8)(keycode + 0x80);
				}
				if (keystat.ref[keycode] == NKEYREF_NC) {
					keyboard_send(data);
				}
			}
		}
	}
}

void keystat_up(const UINT8 *key, REG8 keys, REG8 ref) {

	UINT8	keydata;
	UINT8	keycode;

	while(keys--) {
		keydata = *key++;
		keycode = (keydata & 0x7f);
		if (keycode < 0x70) {
			if (keystat.ref[keycode] == ref) {
				keystat.ref[keycode] = NKEYREF_NC;
				keyboard_send((REG8)(keycode + 0x80));
			}
		}
		else {
			if ((np2cfg.XSHIFT) &&
				(((keycode == 0x70) && (np2cfg.XSHIFT & 1)) ||
				((keycode == 0x74) && (np2cfg.XSHIFT & 2)) ||
				((keycode == 0x73) && (np2cfg.XSHIFT & 4)))) {
				keydata |= 0x80;
			}
			if (!(keydata & 0x80)) {			// �V�t�g
				if (keystat.ref[keycode] == ref) {
					keystat.ref[keycode] = NKEYREF_NC;
					if (keystat.ref[keycode + 0x10] == NKEYREF_NC) {
						keyboard_send((REG8)(keycode + 0x80));
					}
				}
			}
		}
	}
}

void keystat_resendstat(void) {

	REG8	i;

	for (i=0; i<0x70; i++) {
		if (keystat.ref[i] != NKEYREF_NC) {
			keyboard_send(i);
		}
	}
	for (i=0; i<0x10; i++) {
		if ((keystat.ref[0x70 + i] != NKEYREF_NC) ||
			(keystat.ref[0x80 + i] != NKEYREF_NC)) {
			keyboard_send((REG8)(i + 0x70));
		}
	}
}


// ----

void keystat_keydown(REG8 ref) {

	UINT8	shift;
const NKEYM	*nkey;

	if ((ref >= NKEY_USER) && (ref < (NKEY_USER + NKEY_USERKEYS))) {
		nkey = (NKEYM *)(nkeytbl.user + (ref - NKEY_USER));
		keystat_down(nkey->key, nkey->keys, NKEYREF_USER);
	}
	else if (ref < NKEY_SYSTEM) {
		if (np2cfg.KEY_MODE) {
			shift = kbexflag[ref];
			if (shift & KBEX_JOYKEY) {
				keystat.extkey |= (1 << (shift & 7));
				return;
			}
		}
		nkey = (NKEYM *)(nkeytbl.key + ref);
		keystat_down(nkey->key, nkey->keys, ref);
	}
}

void keystat_keyup(REG8 ref) {

	UINT8	shift;
const NKEYM	*nkey;

	if ((ref >= NKEY_USER) && (ref < (NKEY_USER + NKEY_USERKEYS))) {
		nkey = (NKEYM *)(nkeytbl.user + (ref - NKEY_USER));
		keystat_up(nkey->key, nkey->keys, NKEYREF_USER);
	}
	else if (ref < NKEY_SYSTEM) {
		if (np2cfg.KEY_MODE) {
			shift = kbexflag[ref];
			if (shift & KBEX_JOYKEY) {
				keystat.extkey &= ~(1 << (shift & 7));
				return;
			}
		}
		nkey = (NKEYM *)(nkeytbl.key + ref);
		keystat_up(nkey->key, nkey->keys, ref);
	}
}

void keystat_releaseref(REG8 ref) {

	REG8	i;

	for (i=0; i<0x70; i++) {
		if (keystat.ref[i] == ref) {
			keystat.ref[i] = NKEYREF_NC;
			keyboard_send((REG8)(i + 0x80));
		}
	}
	for (i=0; i<0x10; i++) {
		if (keystat.ref[0x70 + i] == ref) {
			keystat.ref[0x70 + i] = NKEYREF_NC;
			if (keystat.ref[0x80 + i] == NKEYREF_NC) {
				keyboard_send((REG8)(0xf0 + i));
			}
		}
		if (keystat.ref[0x80 + i] == ref) {
			keystat.ref[0x80 + i] = NKEYREF_NC;
			if (keystat.ref[0x70 + i] == NKEYREF_NC) {
				keyboard_send((REG8)(0xf0 + i));
			}
		}
	}
}

void keystat_resetjoykey(void) {

	REG8	i;

	keystat.extkey = 0;
	for (i=0; i<0x80; i++) {
		if (kbexflag[i] & KBEX_JOYKEY) {
			keystat_releaseref(i);
		}
	}
}


void keystat_releasekey(REG8 key) {

	key &= 0x7f;
	if (key < 0x70) {
		if (keystat.ref[key] != NKEYREF_NC) {
			keystat.ref[key] = NKEYREF_NC;
			keyboard_send((REG8)(key + 0x80));
		}
	}
	else {
		if ((keystat.ref[key + 0x00] != NKEYREF_NC) ||
			(keystat.ref[key + 0x10] != NKEYREF_NC)) {
			keystat.ref[key + 0x00] = NKEYREF_NC;
			keystat.ref[key + 0x10] = NKEYREF_NC;
			keyboard_send((REG8)(key + 0x80));
		}
	}
}

void keystat_allrelease(void) {

	REG8	i;

	for (i=0; i<0x80; i++) {
		keystat_releasekey(i);
	}
}


REG8 keystat_getjoy(void) {

	return(~keystat.extkey);
}

REG8 keystat_getmouse(SINT16 *x, SINT16 *y) {

	REG8	btn;
	UINT8	acc;
	SINT16	tmp;
	REG8	ret;

	btn = ~keystat.extkey;
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


// ----

// �L�[�R�[�h�ύX

static REG8 cnvnewcode(REG8 oldcode) {

	switch(oldcode) {
		case 0x71:				// �ʏ�caps
			return(0x81);

		case 0x72:				// �ʏ�J�i
			return(0x82);

		case 0x79:				// ���J�j�J�����b�Ncaps
			return(0x71);

		case 0x7a:				// ���J�j�J�����b�Ncaps
			return(0x72);

		case 0x76:
			return(0x90);		// NKEY_USER + 0

		case 0x77:
			return(0x91);		// NKEY_USER + 1

		default:
			return(oldcode);
	}
}

void keystat_senddata(REG8 data) {

	REG8	keycode;

	keycode = cnvnewcode((REG8)(data & 0x7f));
	if (!(data & 0x80)) {
		keystat_keydown(keycode);
	}
	else {
		keystat_keyup(keycode);
	}
}

void keystat_forcerelease(REG8 data) {

	REG8	keycode;

	keycode = cnvnewcode((REG8)(data & 0x7f));
	keystat_releasekey(keycode);
}


#include	"compiler.h"
#include	"np2.h"
#include	"dosio.h"
#include	"mackbd.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"


#define		NC		0xff


#if TARGET_API_MAC_CARBON

typedef struct {
	UINT32	tick;
	BYTE	keymap[16];
	BOOL	active;
	UINT32	repbase;
	UINT32	reptick;
	BYTE	repkey;
} MACKBD;

static	MACKBD		mackbd;


static const BYTE keymac[128] = {
			//	  �`,  �r,  �c,  �e,  �g,  �f,  �y,  �w		; 0x00
				0x1d,0x1e,0x1f,0x20,0x22,0x21,0x29,0x2a,
			//	  �b,  �u,    ,  �a,  �p,  �v,  �d,  �q		; 0x08
				0x2b,0x2c,  NC,0x2d,0x10,0x11,0x12,0x13,
			//	  �x,  �s,  �P,  �Q,  �R,  �S,  �U,  �T		; 0x10
				0x15,0x14,0x01,0x02,0x03,0x04,0x06,0x05,
			//	  �O,  �X,  �V,  �|,  �W,  �O,  �m,  �n 	; 0x18
				0x0c,0x09,0x07,0x0b,0x08,0x0a,0x1b,0x18,
			//	  �t,  ��,  �h,  �o, ret,  �k,  �i,  �F		; 0x20
				0x16,0x1a,0x17,0x19,0x1c,0x25,0x23,0x27,
			//	  �j,  �G,  �n,  �C,  �^,  �m,  �l,  �D		; 0x28
				0x24,0x26,0x28,0x30,0x32,0x2e,0x2f,0x31,
			//	 TAB, SPC,    ,  BS,    , ESC,    , apl		; 0x30
				0x0f,0x34,  NC,0x0e,  NC,0x00,  NC,  NC,
			//	 sft, cps, alt, ctl,    ,    ,    ,    		; 0x38
				0x70,0x79,0x73,0x74,  NC,  NC,  NC,  NC,
			//	    , [.],    , [*],    ,    , [+],    		; 0x40
				  NC,0x50,  NC,0x45,  NC,  NC,0x49,  NC,
			//	    ,    ,    ,    , ret,    , [-], clr		; 0x48
				  NC,  NC,  NC,  NC,0x1c,  NC,0x40,0x3e,
			//	    , [=], [0], [1], [2], [3], [4], [5]		; 0x50
				  NC,0x4d,0x4e,0x4a,0x4b,0x4c,0x46,0x47,
			//	 [6], [7],    , [8], [9],  ��,  �Q, [,]		; 0x58
				0x48,0x42,  NC,0x43,0x44,0x0d,0x33,0x4f,
			//	  F5,  F6,  F7,  F3,  F8,  F9,    , F11		; 0x60
				0x66,0x67,0x68,0x64,0x69,0x6a,  NC,0x72,
			//	    , F13,    , F14,    , F10,    , F12		; 0x68
				  NC,  NC,  NC,  NC,  NC,0x6b,  NC,  NC,
			//	    , F15, hlp, hom,  ru, del,  F4, end		; 0x70
				  NC,  NC,0x3f,0x3e,0x37,0x39,0x65,0x3f,
			//	  F2,  rd,  F1,  ��,  ��,  ��,  ��,    		; 0x78
				0x63,0x36,0x62,0x3b,0x3c,0x3d,0x3a,  NC};

static const BYTE repkey[16] = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
		0xff, 0xff, 0xc1, 0xff, 0xc0, 0x0f, 0x07, 0x07};


void mackbd_initialize(void) {

	mackbd.tick = GETTICK();
	ZeroMemory(&mackbd.keymap, sizeof(mackbd.keymap));
	mackbd.active = TRUE;
	mackbd.repkey = NC;
}

void mackbd_callback(void) {

	UINT32	tick;
	BYTE	key[16];
	UINT	i;
	BYTE	update;
	UINT	j;
	BYTE	keycode;

	if (!mackbd.active) {
		return;
	}

	tick = GETTICK();
	if (mackbd.tick == tick) {
		return;
	}
	mackbd.tick = tick;
#if TARGET_API_MAC_CARBON
	GetKeys((long *)key);
#else
	GetKeys((unsigned long *)key);
#endif
	for (i=0; i<16; i++) {
		update = mackbd.keymap[i] ^ key[i];
		if (update) {
			mackbd.keymap[i] = key[i];
			for (j=0; j<8; j++) {
				if (update & (1 << j)) {
					keycode = keymac[i * 8 + j];
					if (keycode != NC) {
						if (key[i] & (1 << j)) {
							keystat_senddata(keycode);
							if ((repkey[keycode >> 3] << (keycode & 7))
																	& 0x80) {
								mackbd.repkey = keycode;
								mackbd.repbase = tick;
								mackbd.reptick = 500;
							}
						}
						else {
							if (mackbd.repkey == keycode) {
								mackbd.repkey = NC;
							}
							keystat_senddata(keycode + 0x80);
						}
					}
				}
			}
		}
	}

	// �L�[���s�[�g����
	if (mackbd.repkey != NC) {
		if ((tick - mackbd.repbase) >= mackbd.reptick) {
			keystat_senddata(mackbd.repkey);		// keystat����break����
			mackbd.repbase = tick;
			mackbd.reptick = 40;
		}
	}
}

BOOL mackbd_keydown(int keycode) {

	if (keycode == 0x6f) {
		if (np2oscfg.F12COPY == 1) {
			keystat_senddata(0x61);
			return(TRUE);
		}
		else if (np2oscfg.F12COPY == 2) {
			keystat_senddata(0x60);
			return(TRUE);
        }
	}
	return(FALSE);
}

BOOL mackbd_keyup(int keycode) {

	if (keycode == 0x6f) {
		if (np2oscfg.F12COPY == 1) {
			keystat_senddata(0x61 | 0x80);
			return(TRUE);
		}
		else if (np2oscfg.F12COPY == 2) {
			keystat_senddata(0x60 | 0x80);
			return(TRUE);
		}
	}
	return(FALSE);
}

void mackbd_activate(BOOL active) {

	if (mackbd.active != active) {
		mackbd.active = active;
		if (!active) {
			ZeroMemory(&mackbd.keymap, sizeof(mackbd.keymap));
			keystat_allrelease();
		}
	}
}

#else

enum {
	kMac_kana		= 0x37,
	kMac_shift		= 0x38,
	kMac_caps		= 0x39,
	kMac_alt		= 0x3a,
	kMac_ctrl		= 0x3b
};

#define	kMac_Basebit(a)		(1 << ((a) & 15))

typedef struct {
	UINT16	bit;
	BYTE	code;
} KEYSEA;

static const KEYSEA keysea[] = {
					{kMac_Basebit(kMac_shift),	0x70},
					{kMac_Basebit(kMac_caps),	0x79},
//					{kMac_Basebit(kMac_kana),	0x72},
					{kMac_Basebit(kMac_alt),	0x73},
					{kMac_Basebit(kMac_ctrl),	0x74}};

static const BYTE keymac[128] = {
			//	  �`,  �r,  �c,  �e,  �g,  �f,  �y,  �w		; 0x00
				0x1d,0x1e,0x1f,0x20,0x22,0x21,0x29,0x2a,
			//	  �b,  �u,    ,  �a,  �p,  �v,  �d,  �q		; 0x08
				0x2b,0x2c,  NC,0x2d,0x10,0x11,0x12,0x13,
			//	  �x,  �s,  �P,  �Q,  �R,  �S,  �U,  �T		; 0x10
				0x15,0x14,0x01,0x02,0x03,0x04,0x06,0x05,
			//	  �O,  �X,  �V,  �|,  �W,  �O,  �m,  �n 	; 0x18
				0x0c,0x09,0x07,0x0b,0x08,0x0a,0x1b,0x18,
			//	  �t,  ��,  �h,  �o, ret,  �k,  �i,  �F		; 0x20
				0x16,0x1a,0x17,0x19,0x1c,0x25,0x23,0x27,
			//	  �j,  �G,  �n,  �C,  �^,  �m,  �l,  �D		; 0x28
				0x24,0x26,0x28,0x30,0x32,0x2e,0x2f,0x31,
			//	 TAB, SPC,    ,  BS,    , ESC,    , apl		; 0x30
				0x0f,0x34,  NC,0x0e,  NC,0x00,  NC,  NC,
			//	 sft, cps, alt, ctl,    ,    ,    ,    		; 0x38
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    , [.],    , [*],    ,    , [+],    		; 0x40
				  NC,0x50,  NC,0x45,  NC,  NC,0x49,  NC,
			//	    ,    ,    ,    , ret,    , [-], clr		; 0x48
				  NC,  NC,  NC,  NC,0x1c,  NC,0x40,0x3e,
			//	    , [=], [0], [1], [2], [3], [4], [5]		; 0x50
				  NC,0x4d,0x4e,0x4a,0x4b,0x4c,0x46,0x47,
			//	 [6], [7],    , [8], [9],  ��,  �Q, [,]		; 0x58
				0x48,0x42,  NC,0x43,0x44,0x0d,0x33,0x4f,
			//	  F5,  F6,  F7,  F3,  F8,  F9,    , F11		; 0x60
				0x66,0x67,0x68,0x64,0x69,0x6a,  NC,0x72,
			//	    , F13,    , F14,    , F10,    , F12		; 0x68
				  NC,  NC,  NC,  NC,  NC,0x6b,  NC,  NC,
			//	    , F15, hlp, hom,  ru, del,  F4, end		; 0x70
				  NC,  NC,0x3f,0x3e,0x37,0x39,0x65,0x3f,
			//	  F2,  rd,  F1,  ��,  ��,  ��,  ��,    		; 0x78
				0x63,0x36,0x62,0x3b,0x3c,0x3d,0x3a,  NC};

typedef struct {
	UINT32	tick;
	UINT16	shift;
	BOOL	active;
} MACKBD;

static	MACKBD		mackbd;

void mackbd_initialize(void) {

	ZeroMemory(&mackbd, sizeof(mackbd));
	mackbd.active = TRUE;
}

void mackbd_callback(void) {

	UINT32	tick;
	BYTE	key[16];
	UINT16	shift;
	UINT16	shiftchg;
	UINT	i;

	if (!mackbd.active) {
		return;
	}

	tick = GETTICK();
	if (mackbd.tick != tick) {
		mackbd.tick = tick;
		GetKeys((unsigned long *)key);
		shift = ((UINT16)key[7] << 8) + key[6];
		shiftchg = mackbd.shift ^ shift;
		mackbd.shift = shift;
		for (i=0; i<(sizeof(keysea) / sizeof(KEYSEA)); i++) {
			if (shiftchg & keysea[i].bit) {
				if (shift & keysea[i].bit) {
					keystat_senddata(keysea[i].code);
				}
				else {
					keystat_senddata(keysea[i].code | 0x80);
				}
			}
		}
	}
}

BOOL mackbd_keydown(int keycode) {

	BYTE	data;

	data = NC;
	if (keycode == 0x6f) {
		if (np2oscfg.F12COPY == 1) {
			data = 0x61;
		}
		else if (np2oscfg.F12COPY == 2) {
			data = 0x60;
		}
	}
	else if (keycode < 0x80) {
		data = keymac[keycode];
	}
	if (data != NC) {
		keystat_senddata(data);
		return(TRUE);
	}
	else {
		return(FALSE);
	}
}

BOOL mackbd_keyup(int keycode) {

	BYTE	data;

	data = NC;
	if (keycode == 0x6f) {
		if (np2oscfg.F12COPY == 1) {
			data = 0x61;
		}
		else if (np2oscfg.F12COPY == 2) {
			data = 0x60;
		}
	}
	else if (keycode < 0x80) {
		data = keymac[keycode];
	}
	if (data != NC) {
		keystat_senddata(data | 0x80);
		return(TRUE);
	}
	else {
		return(FALSE);
	}
}

void mackbd_activate(BOOL active) {

	if (mackbd.active != active) {
		mackbd.active = active;
		if (!active) {
			keystat_allrelease();
		}
	}
}

#endif


#if 0
// �ь�L�[����
static const BYTE keymac2[128] = {
			//	  �`,  �r,  �c,  �e,  �g,  �f,  �y,  �w		; 0x00
				  NC,  NC,  NC,  NC,0x3f,  NC,  NC,  NC,
			//	  �b,  �u,    ,  �a,  �p,  �v,  �d,  �q		; 0x08
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	  �x,  �s,  �P,  �Q,  �R,  �S,  �U,  �T		; 0x10
				  NC,  NC,0x62,0x63, 0x64,0x65,0x67,0x66,
			//	  �O,  �X,  �V,  �|,  �W,  �O,  �m,  �n 	; 0x18
				  NC,0x6a,0x68,  NC,0x69,0x6b,  NC,  NC,
			//	  �t,  ��,  �h,  �o, ret,  �k,  �i,  �F		; 0x20
				  NC,  NC,  NC,  NC,  NC,0x3e,  NC,  NC,
			//	  �j,  �G,  �n,  �C,  �^,  �m,  �l,  �D		; 0x28
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	 TAB, SPC,    ,  BS,    , ESC,    , apl		; 0x30
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	 sft, cps, alt, ctl,    ,    ,    ,    		; 0x38
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    , [.],    , [*],    ,    , [+],    		; 0x40
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    , ret,    , [-], clr		; 0x48
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    , [=], [0], [1], [2], [3], [4], [5]		; 0x50
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	 [6], [7],    , [8], [9],  ��,  �Q, [,]		; 0x58
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	  F5,  F6,  F7,  F3,  F8,  F9,    , F11		; 0x60
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    , F13,    , F14,    , F10,    , F12		; 0x68
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    , F15, hlp, hom,  ru, del,  F4, end		; 0x70
				  NC,  NC,  NC,  NC,  NC,0x38,  NC,  NC,
			//	  F2,  rd,  F1,  ��,  ��,  ��,  ��,    		; 0x78
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC};
#endif

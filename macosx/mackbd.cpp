#include	"compiler.h"
#include	"np2.h"
#include	"dosio.h"
#include	"mackbd.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"


#define		NC		0xff

static const BYTE keymac[128] = {
			//	  Ç`,  Çr,  Çc,  Çe,  Çg,  Çf,  Çy,  Çw		; 0x00
				0x1d,0x1e,0x1f,0x20,0x22,0x21,0x29,0x2a,
			//	  Çb,  Çu,    ,  Ça,  Çp,  Çv,  Çd,  Çq		; 0x08
				0x2b,0x2c,  NC,0x2d,0x10,0x11,0x12,0x13,
			//	  Çx,  Çs,  ÇP,  ÇQ,  ÇR,  ÇS,  ÇU,  ÇT		; 0x10
				0x15,0x14,0x01,0x02,0x03,0x04,0x06,0x05,
			//	  ÅO,  ÇX,  ÇV,  Å|,  ÇW,  ÇO,  Åm,  Çn 	; 0x18
				0x0c,0x09,0x07,0x0b,0x08,0x0a,0x1b,0x18,
			//	  Çt,  Åó,  Çh,  Ço, ret,  Çk,  Çi,  ÅF		; 0x20
				0x16,0x1a,0x17,0x19,0x1c,0x25,0x23,0x27,
			//	  Çj,  ÅG,  Ån,  ÅC,  Å^,  Çm,  Çl,  ÅD		; 0x28
				0x24,0x26,0x28,0x30,0x32,0x2e,0x2f,0x31,
			//	 TAB, SPC,    ,  BS,    , ESC,    , apl		; 0x30
				0x0f,0x34,  NC,0x0e,  NC,0x00,  NC,  NC,
			//	 sft, ctl, alt, cps,    ,    ,    ,    		; 0x38
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    , [.],    , [*],    ,    , [+],    		; 0x40
				  NC,0x50,  NC,0x45,  NC,  NC,0x49,  NC,
			//	    ,    ,    ,    , ret,    , [-], clr		; 0x48
				  NC,  NC,  NC,  NC,0x1c,  NC,0x40,0x3e,
			//	    , [=], [0], [1], [2], [3], [4], [5]		; 0x50
				  NC,0x4d,0x4e,0x4a,0x4b,0x4c,0x46,0x47,
			//	 [6], [7],    , [8], [9],  Åè,  ÅQ, [,]		; 0x58
				0x48,0x42,  NC,0x43,0x44,0x0d,0x33,0x4f,
			//	  F5,  F6,  F7,  F3,  F8,  F9,    , F11		; 0x60
				0x66,0x67,0x68,0x64,0x69,0x6a,  NC,0x72,
			//	    , F13,    , F14,    , F10,    , F12		; 0x68
				  NC,  NC,  NC,  NC,  NC,0x6b,  NC,0x80,
			//	    , F15, hlp, hom,  ru, del,  F4, end		; 0x70
				  NC,  NC,0x3f,0x3e,0x37,0x39,0x65,0x3f,
			//	  F2,  rd,  F1,  Å©,  Å®,  Å´,  Å™,    		; 0x78
				0x63,0x36,0x62,0x3b,0x3c,0x3d,0x3a,  NC};


// ó—åÁÉLÅ[âüâ∫
static const BYTE keymac2[128] = {
			//	  Ç`,  Çr,  Çc,  Çe,  Çg,  Çf,  Çy,  Çw		; 0x00
				  NC,  NC,  NC,  NC,0x3f,  NC,  NC,  NC,
			//	  Çb,  Çu,    ,  Ça,  Çp,  Çv,  Çd,  Çq		; 0x08
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	  Çx,  Çs,  ÇP,  ÇQ,  ÇR,  ÇS,  ÇU,  ÇT		; 0x10
				  NC,  NC,0x62,0x63, 0x64,0x65,0x67,0x66,
			//	  ÅO,  ÇX,  ÇV,  Å|,  ÇW,  ÇO,  Åm,  Çn 	; 0x18
				  NC,0x6a,0x68,  NC,0x69,0x6b,  NC,  NC,
			//	  Çt,  Åó,  Çh,  Ço, ret,  Çk,  Çi,  ÅF		; 0x20
				  NC,  NC,  NC,  NC,  NC,0x3e,  NC,  NC,
			//	  Çj,  ÅG,  Ån,  ÅC,  Å^,  Çm,  Çl,  ÅD		; 0x28
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	 TAB, SPC,    ,  BS,    , ESC,    , apl		; 0x30
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	 sft, ctl, alt, cps,    ,    ,    ,    		; 0x38
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    , [.],    , [*],    ,    , [+],    		; 0x40
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    ,    ,    ,    , ret,    , [-], clr		; 0x48
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    , [=], [0], [1], [2], [3], [4], [5]		; 0x50
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	 [6], [7],    , [8], [9],  Åè,  ÅQ, [,]		; 0x58
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	  F5,  F6,  F7,  F3,  F8,  F9,    , F11		; 0x60
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    , F13,    , F14,    , F10,    , F12		; 0x68
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC,
			//	    , F15, hlp, hom,  ru, del,  F4, end		; 0x70
				  NC,  NC,  NC,  NC,  NC,0x38,  NC,  NC,
			//	  F2,  rd,  F1,  Å©,  Å®,  Å´,  Å™,    		; 0x78
				  NC,  NC,  NC,  NC,  NC,  NC,  NC,  NC};


enum {
	kMac_base		= 0x01,

	kMac_kana		= 0x37,
	kMac_shift		= 0x38,
	kMac_caps		= 0x39,
	kMac_alt		= 0x3a,
	kMac_ctrl		= 0x3b
};

#define	kMac_Basebit(a)		(1 << ((a) & 15))

static	UINT16	shiftback = 0;
static	UINT32	shiftchktick = 0;

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


void mackbd_init(void) {

	shiftback = 0;
	shiftchktick = 0;
}

void mackbd_callback(void) {

	UINT32	tick;
	BYTE	mackeymap[16];
	UINT16	tmp;
	UINT16	keyxor;
	int		i;

	tick = GETTICK();
	if (shiftchktick != tick) {
		shiftchktick = tick;

#if TARGET_API_MAC_CARBON
		GetKeys((long *)mackeymap);
#else
		GetKeys((unsigned long *)mackeymap);
#endif
		tmp = (mackeymap[7] << 8) + mackeymap[6];
		keyxor = shiftback ^ tmp;
		shiftback = tmp;
		for (i=0; i<(sizeof(keysea) / sizeof(KEYSEA)); i++) {
			if (keyxor & keysea[i].bit) {
				if (shiftback & keysea[i].bit) {
					keystat_senddata(keysea[i].code);
				}
				else {
					keystat_senddata(keysea[i].code | 0x80);
				}
			}
		}
	}
}

BOOL mackbd_down(int cmd, int keycode) {

	BYTE	data;

	if (keycode >= 0x80) {
		return(FALSE);
	}
	if (!cmd) {
		data = keymac[keycode];
	}
	else {
		data = keymac2[keycode];
	}
	if (data != NC) {
		if (data == 0x80) {
			if (np2oscfg.F12COPY == 1) {
				data = 0x61;
			}
			else {
				data = 0x60;
			}
		}
		keystat_senddata(data);
		return(TRUE);
	}
	return(FALSE);
}

void mackbd_up(int keycode) {

	BYTE	data, data2;

	if (keycode >= 0x80) {
		return;
	}
	data = keymac[keycode];
	if (data != NC) {
		if (data == 0x80) {
			if (np2oscfg.F12COPY == 1) {
				data = 0x61;
			}
			else {
				data = 0x60;
			}
		}
		keystat_senddata(data | 0x80);
	}
	data2 = keymac2[keycode];
	if (data2 != NC) {
		keystat_senddata(data2 | 0x80);
	}
}


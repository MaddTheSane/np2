#include	"compiler.h"
#include	"np2.h"
#include	"softkbd.h"
#include	"pccore.h"
#include	"iocore.h"


typedef struct {
	UINT8	left;
	UINT8	keys;
	UINT8	left1;
	UINT8	left2;
	UINT8	base;
	UINT8	right;
} FULLKEY;

typedef struct {
	FULLKEY		fk[4];
	UINT8		cur0[5];
	UINT8		cur1[5];
	UINT8		ten[20];
} KEYTBL;

static const KEYTBL keytbl = {
		// fullkey 90
		   {{12, 13, 0x00, 0x00, 0x01, 0x0e},
			{16, 12, 0x0f, 0x0f, 0x10, 0x1c},
			{20, 12, 0x74, 0x71, 0x1d, 0x1c},
			{24, 11, 0x70, 0x70, 0x29, 0x70}},

		// 247
		   {0x38, 0x36, 0x3a, 0x3b, 0x3d},
		   {0x39, 0x37, 0x3a, 0x3c, 0x3d},

		// 270
		   {0x3e, 0x3f, 0x40, 0x41,
			0x42, 0x43, 0x44, 0x45,
			0x46, 0x47, 0x48, 0x49,
			0x4a, 0x4b, 0x4c, 0x4d,
			0x4e, 0x4f, 0x50, 0x1c}};

#define	NC		0xff

static	UINT8	softkey = NC;

static UINT getsoftkbd(int x, int y) {

	UINT		posy;
const FULLKEY	*fk;

	x -= 40;
	posy = (y - 200) >> 3;
	if ((posy >= 5) || (x < 0)) {
		return(NC);
	}
	if (x < 0) {
		return(NC);
	}
	if (x < (54 - 40)) {			// stop/copy
		if ((x < 10) && (posy < 2)) {
			return(0x60 + posy);
		}
		return(NC);
	}
	x -= (54 - 40);
	if (x < (90 - 54)) {			// function
		if (x < 15) {
			return(0x62 + posy);
		}
		if ((x >= 17) && (x < 32)) {
			return(0x67 + posy);
		}
		return(NC);
	}
	x -= (90 - 54);
	if (x < (247 - 90)) {			// fullkey
		if (posy < 4) {
			if (x < 153) {
				fk = keytbl.fk + posy;
				x -= fk->left;
				if (x < -10) {
					return(fk->left1);
				}
				else if (x < 0) {
					return(fk->left2);
				}
				x = (x * (((1 << 12) / 10) + 1)) >> 12;
				if (x < (int)fk->keys) {
					return(fk->base + x);
				}
				return(fk->right);
			}
		}
		else {
			x -= 19;
			if (x < 0) {
				return(NC);
			}
			x -= 10;
			if (x < 0) {
				return(0x72);
			}
			x -= 10;
			if (x < 0) {
				return(0x73);
			}
			x -= 15;
			if (x < 0) {
				return(0x51);
			}
			x -= 66;
			if (x < 0) {
				return(0x34);
			}
			if (x < 15) {
				return(0x35);
			}
		}
		return(NC);
	}
	x -= (247 - 90);
	if (x < (270 - 247)) {			// cur
		if (x < 10) {
			return(keytbl.cur0[posy]);
		}
		else if (x < 20) {
			return(keytbl.cur1[posy]);
		}
		return(NC);
	}
	x -= (270 - 247);
	if (x < (310 - 270)) {			// ten key
		x = (x * (((1 << 12) / 10) + 1)) >> 12;
		return(keytbl.ten[x + posy * 4]);
	}
	return(NC);
}

void softkbd_up(void) {

	if (softkey != NC) {
		keystat_senddata(softkey | 0x80);
		softkey = NC;
	}
}

void softkbd_down(int x, int y) {

	UINT8	key;

	softkbd_up();
	key = getsoftkbd(x, y);
	if (key != NC) {
		keystat_senddata((UINT8)key);
		softkey = key;
	}
}




#if 0
//	keycpy(out, kbd1, 69, 32, 10, 1, "ÉJÉi", FONT_CTRL);
//	keycpy(out, kbd1, 79, 32, 10, 3, "GRPH", FONT_CTRL);
//	keycpy(out, kbd1, 89, 32, 15, 3, "NFER", FONT_CTRL);
//	keycpy(out, kbd0, 104, 32, 66, 3, NULL, FONT_NULL);
//	keycpy(out, kbd1, 170, 32, 15, 2, "XFER", FONT_CTRL);
#endif




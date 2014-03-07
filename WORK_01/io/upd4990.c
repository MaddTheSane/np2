#include	"compiler.h"
#include	"timemng.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"calendar.h"


// ---- I/O

static void IOOUTCALL upd4990_o20(UINT port, REG8 dat) {

	REG8	mod;
	REG8	cmd;

	mod = dat ^ uPD4990.last;
	uPD4990.last = (UINT8)dat;

	if (dat & 0x08) {										// STB
		if (mod & 0x08) {
			cmd = uPD4990.parallel;
			if (cmd == 7) {
				cmd = uPD4990.serial & 0x0f;
			}
			switch(cmd) {
				case 0x00:			// register hold
					uPD4990.regsft = 0;
					break;

				case 0x01:			// register shift
					uPD4990.regsft = 1;
					uPD4990.pos = (UPD4990_REGLEN * 8) - 1;
					uPD4990.cdat = uPD4990.reg[UPD4990_REGLEN - 1] & 1;
					break;

				case 0x02:			// time set	/ counter hold
					uPD4990.regsft = 0;
					break;

				case 0x03:			// time read
					uPD4990.regsft = 0;
					ZeroMemory(uPD4990.reg, sizeof(uPD4990.reg));
					calendar_get(uPD4990.reg + UPD4990_REGLEN - 6);
					uPD4990.cdat = uPD4990.reg[UPD4990_REGLEN - 1] & 1;
					// uPD4990 Happy!! :)
					uPD4990.reg[UPD4990_REGLEN - 7] = 0x01;
					break;
#if 0
				case 0x04:			// TP=64Hz
				case 0x05:			// TP=256Hz
				case 0x06:			// TP=2048Hz
				case 0x07:			// TP=4096Hz
				case 0x08:			// TP=1sec interrupt
				case 0x09:			// TP=10sec interrupt
				case 0x0a:			// TP=30sec interrupt
				case 0x0b:			// TP=60sec interrupt
				case 0x0c:			// interrupt reset
				case 0x0d:			// interrupt start
				case 0x0e:			// interrupt stop
				case 0x0f:			// test..
					break;
#endif
			}
		}
	}
	else if (dat & 0x10) {								// CLK
		if (mod & 0x10) {
			if (uPD4990.parallel == 7) {
				uPD4990.serial >>= 1;
			}
			if ((uPD4990.regsft) && (uPD4990.pos)) {
				uPD4990.pos--;
			}
			uPD4990.cdat = (uPD4990.reg[uPD4990.pos / 8] >>
												((~uPD4990.pos) & 7)) & 1;
		}
	}
	else {													// DATA
		uPD4990.parallel = dat & 7;
		if (uPD4990.parallel == 7) {
			uPD4990.serial &= 0x0f;
			uPD4990.serial |= (dat >> 1) & 0x10;
		}
		if (dat & 0x20) {
			uPD4990.reg[uPD4990.pos / 8] |= (0x80 >> (uPD4990.pos & 7));
		}
		else {
			uPD4990.reg[uPD4990.pos / 8] &= ~(0x80 >> (uPD4990.pos & 7));
		}
	}
	(void)port;
}


// ---- I/F

static const IOOUT updo20[1] = {upd4990_o20};

void uPD4990_reset(void) {

	ZeroMemory(&uPD4990, sizeof(uPD4990));
}

void uPD4990_bind(void) {

	iocore_attachsysoutex(0x0020, 0x0cf1, updo20, 1);
}


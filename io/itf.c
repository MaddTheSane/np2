#include	"compiler.h"
#include	"i286.h"
#include	"pccore.h"
#include	"iocore.h"


// ---- I/O

static void IOOUTCALL itf_o043d(UINT port, BYTE dat) {

	switch(dat) {
		case 0x10:
			itf.bank = 1;
			break;

		case 0x12:
			itf.bank = 0;
			break;
	}
	(void)port;
}


// ---- I/F

void itf_reset(void) {

	itf.bank = 1;
}

void itf_bind(void) {

	iocore_attachout(0x043d, itf_o043d);
}


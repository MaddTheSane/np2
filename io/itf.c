#include	"compiler.h"
#include	"i286.h"
#include	"pccore.h"
#include	"iocore.h"


// ---- I/O

static void IOOUTCALL itf_o043d(UINT port, BYTE dat) {

	switch(dat) {
		case 0x10:
			i286core.s.itfbank = 1;
			break;

		case 0x12:
			i286core.s.itfbank = 0;
			break;
	}
	(void)port;
}


// ---- I/F

void itf_bind(void) {

	iocore_attachout(0x043d, itf_o043d);
}


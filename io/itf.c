#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"


// ---- I/O

static void IOOUTCALL itf_o043d(UINT port, REG8 dat) {

	switch(dat) {
		case 0x10:
			CPU_ITFBANK = 1;
			break;

		case 0x12:
			CPU_ITFBANK = 0;
			break;
	}
	(void)port;
}


// ---- I/F

void itf_bind(void) {

	iocore_attachout(0x043d, itf_o043d);
}


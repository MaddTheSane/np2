#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"


// EPSON専用ポート 0c00～

// ---- I/O

static REG8 IOINPCALL epcpuio_ic03(UINT port) {

	(void)port;
	return(epsonio.cpumode);
}


// ---- I/F

void epsonio_reset(void) {

	epsonio.cpumode = 'R';
}

void epsonio_bind(void) {

	iocore_attachinp(0x0c03, epcpuio_ic03);
}


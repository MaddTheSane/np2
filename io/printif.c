#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"


// ---- I/O

static REG8 IOINPCALL prt_i42(UINT port) {

	REG8	ret;

	ret = 0x84;
	if (pc.cpumode & CPUMODE_8MHz) {
		ret |= 0x20;
	}
	if (np2cfg.dipsw[0] & 4) {
		ret |= 0x10;
	}
	if (np2cfg.dipsw[0] & 0x80) {
		ret |= 0x08;
	}
#ifndef EPSON_286
	if (CPU_TYPE & CPUTYPE_V30) {
		ret |= 0x02;
	}
#else
	if (np2cfg.dipsw[2] & 0x80) {
		ret |= 0x02;
	}
#endif
	(void)port;
	return(ret);
}


// ---- I/F

static const IOINP prti40[4] = {
					NULL,		prt_i42,	NULL,		NULL};

void printif_reset(void) {
}

void printif_bind(void) {

	iocore_attachsysinpex(0x0040, 0x0cf1, prti40, 4);
}


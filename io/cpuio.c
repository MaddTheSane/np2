#include	"compiler.h"
#include	"i286.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"


// ---- I/O

static void IOOUTCALL cpuio_of0(UINT port, BYTE dat) {

	cpuio.reset_req = 1;
	extmem.adrsmask = 0x0fffff;
	i286_interrupt(0x02);
	nevent_forceexit();
	(void)port;
	(void)dat;
}

static void IOOUTCALL cpuio_of2(UINT port, BYTE dat) {

	extmem.adrsmask = 0x1fffff;
	(void)port;
	(void)dat;
}

static BYTE IOINPCALL cpuio_if0(UINT port) {

	BYTE	ret;

	if (!(usesound & 0x80)) {
		ret = 0x00;
	}
	else {				// for AMD-98
		ret = 0x18;		// 0x14?
	}
	(void)port;
	return(ret);
}

static BYTE IOINPCALL cpuio_if2(UINT port) {

	BYTE	ret;

	ret = 0xfe;
	if (extmem.adrsmask != 0x1fffff) {
		ret++;
	}
	(void)port;
	return(ret);
}


#ifdef CPU386											// define‚ð•Ï‚¦‚Ä‚Ë
static void IOOUTCALL cpuio_of6(UINT port, BYTE dat) {

	switch(dat) {
		case 0x02:
			extmem.adrsmask = 0x1fffff;
			break;

		case 0x03:
			extmem.adrsmask = 0x0fffff;
			break;
	}
	(void)port;
}

static BYTE IOINPCALL cpuio_if6(UINT port) {

	BYTE	ret;

	ret = 0x00;
	if (extmem.adrsmask != 0x1fffff) {
		ret |= 0x01;
	}
	if (nmi.enable) {
		ret |= 0x02;
	}
	(void)port;
	return(ret);
}
#endif


// ---- I/F

#ifndef CPU386											// define‚ð•Ï‚¦‚Ä‚Ë
static const IOOUT cpuioof0[8] = {
					cpuio_of0,	cpuio_of2,	NULL,		NULL,
					NULL,		NULL,		NULL,		NULL};

static const IOINP cpuioif0[8] = {
					cpuio_if0,	cpuio_if2,	NULL,		NULL,
					NULL,		NULL,		NULL,		NULL};
#else
static const IOOUT cpuioof0[8] = {
					cpuio_of0,	cpuio_of2,	NULL,		cpuio_of6,
					NULL,		NULL,		NULL,		NULL};

static const IOINP cpuioif0[8] = {
					cpuio_if0,	cpuio_if2,	NULL,		cpuio_if6,
					NULL,		NULL,		NULL,		NULL};
#endif

void cpuio_reset(void) {

	ZeroMemory(&cpuio, sizeof(cpuio));
}

void cpuio_bind(void) {

	iocore_attachsysoutex(0x00f0, 0x0cf1, cpuioof0, 8);
	iocore_attachsysinpex(0x00f0, 0x0cf1, cpuioif0, 8);
}


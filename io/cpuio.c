#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"

// Œã‚Å’²®
#if defined(CPU386)
DWORD cpumem_addrmask = 0xfffff;
#endif


// ---- I/O

static void IOOUTCALL cpuio_of0(UINT port, REG8 dat) {

	CPU_ADRSMASK = 0x0fffff;
#if defined(CPU386)
	cpumem_addrmask = 0xfffff;
#endif
	CPU_RESETREQ = 1;
	CPU_INTERRUPT(0x02);
	nevent_forceexit();
	(void)port;
	(void)dat;
}

static void IOOUTCALL cpuio_of2(UINT port, REG8 dat) {

	CPU_ADRSMASK = 0x1fffff;
#if defined(CPU386)
	cpumem_addrmask = 0xffffffff;
#endif
	(void)port;
	(void)dat;
}

static REG8 IOINPCALL cpuio_if0(UINT port) {

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

static REG8 IOINPCALL cpuio_if2(UINT port) {

	REG8	ret;

#if 1
	ret = 0xff;
	ret -= (REG8)((CPU_ADRSMASK >> 20) & 1);
#else
	ret = 0xfe;
	if (CPU_ADRSMASK != 0x1fffff) {
		ret++;
	}
#endif
	(void)port;
	return(ret);
}


#ifdef CPU386											// define‚ð•Ï‚¦‚Ä‚Ë
static void IOOUTCALL cpuio_of6(UINT port, REG8 dat) {

	switch(dat) {
		case 0x02:
			CPU_ADRSMASK = 0x1fffff;
#if defined(CPU_386)
			cpumem_addrmask = 0xffffffff;
#endif
			break;

		case 0x03:
			CPU_ADRSMASK = 0x0fffff;
#if defined(CPU_386)
			cpumem_addrmask = 0xfffff;
#endif
			break;
	}
	(void)port;
}

static REG8 IOINPCALL cpuio_if6(UINT port) {

	REG8	ret;

	ret = 0x00;
	if (CPU_ADRSMASK != 0x1fffff) {
		ret |= 0x01;
	}
	if (nmiio.enable) {
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

void cpuio_bind(void) {

	iocore_attachsysoutex(0x00f0, 0x0cf1, cpuioof0, 8);
	iocore_attachsysinpex(0x00f0, 0x0cf1, cpuioif0, 8);
}


#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"fmboard.h"

// Œã‚Å’²®
#if defined(CPUCORE_IA32)
DWORD cpumem_addrmask = 0xfffff;
#endif


// ---- I/O

static void IOOUTCALL cpuio_of0(UINT port, REG8 dat) {

	CPU_ADRSMASK = 0x0fffff;
#if defined(CPUCORE_IA32)
	cpumem_addrmask = 0xfffff;
#endif
	CPU_RESETREQ = 1;
//	CPU_INTERRUPT(0x02);
	nevent_forceexit();
	(void)port;
	(void)dat;
}

static void IOOUTCALL cpuio_of2(UINT port, REG8 dat) {

	CPU_ADRSMASK = 0xffffffff;
#if defined(CPUCORE_IA32)
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

	ret = 0xff;
	ret -= (REG8)((CPU_ADRSMASK >> 20) & 1);
	(void)port;
	return(ret);
}


#if defined(CPUCORE_IA32)
static void IOOUTCALL cpuio_of6(UINT port, REG8 dat) {

	switch(dat) {
		case 0x02:
			CPU_ADRSMASK = 0xffffffff;
#if defined(CPUCORE_IA32)
			cpumem_addrmask = 0xffffffff;
#endif
			break;

		case 0x03:
			CPU_ADRSMASK = 0x0fffff;
#if defined(CPUCORE_IA32)
			cpumem_addrmask = 0xfffff;
#endif
			break;
	}
	(void)port;
}

static REG8 IOINPCALL cpuio_if6(UINT port) {

	REG8	ret;

	ret = 0x00;
	if (!(CPU_ADRSMASK & (1 << 20))) {
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

#if !defined(CPUCORE_IA32)
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


#include	"compiler.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"


void extmemmng_clear(void) {

	if (CPU_EXTMEM) {
		_MFREE(CPU_EXTMEM);
		CPU_EXTMEM = NULL;
		CPU_EXTMEMSIZE = 0;
	}
}

BOOL extmemmng_realloc(UINT megabytes) {

	megabytes <<= 20;
	if (megabytes != CPU_EXTMEMSIZE) {
		extmemmng_clear();
		if (megabytes) {
			CPU_EXTMEM = (BYTE *)_MALLOC(megabytes + 16, "EXTMEM");
			if (CPU_EXTMEM == NULL) {
				return(FAILURE);
			}
		}
		CPU_EXTMEMSIZE = megabytes;
	}
	return(SUCCESS);
}


// ---- I/O

static void IOOUTCALL emm_o08e1(UINT port, REG8 dat) {

	UINT	pos;

	if (!(CPU_TYPE & CPUTYPE_V30)) {
		pos = (port >> 1) & 3;
		if (!extmem.target) {							// ver0.28
			extmem.page[pos] = 0xffffffff;
			extmem.pageptr[pos] = mem + 0xc0000 + (pos << 14);
		}
		else if (extmem.target < extmem.maxmem) {		// ver0.28
			dat &= 0xfc;
			extmem.page[pos] = ((extmem.target - 1) << 8) + dat;
			extmem.pageptr[pos] = CPU_EXTMEM + (extmem.page[pos] << 12);
		}
	}
}

static void IOOUTCALL emm_o08e9(UINT port, REG8 dat) {

	if (!(CPU_TYPE & CPUTYPE_V30)) {
		extmem.target = dat & 0x0f;
	}
	(void)port;
}

static REG8 IOINPCALL emm_i08e9(UINT port) {

	if ((!(CPU_TYPE & CPUTYPE_V30)) &&
		(extmem.target) && (extmem.target < extmem.maxmem)) {
		return(0);
	}
	else {
		return(0xff);
	}
	(void)port;
}


// ---- I/F

BOOL extmem_init(BYTE usemem) {

	UINT	i;

	if (usemem > 13) {
		usemem = 13;
	}
	extmem.target = 0;
	extmem.maxmem = 0;
	for (i=0; i<4; i++) {
		extmem.page[i] = 0xffffffff;
		extmem.pageptr[i] = mem + 0xc0000 + (i << 14);
	}
	if (extmemmng_realloc(usemem)) {
		return(FAILURE);
	}
	extmem.maxmem = usemem + 1;
#if defined(CPUCORE_IA32)
	init_cpumem(usemem);
#endif
	return(SUCCESS);
}

void extmem_reset(void) {
}

void extmem_bind(void) {

#if !defined(CPUCORE_IA32)
	if (!(np2cfg.dipsw[2] & 0x80)) {
		iocore_attachout(0x08e1, emm_o08e1);
		iocore_attachout(0x08e3, emm_o08e1);
		iocore_attachout(0x08e5, emm_o08e1);
		iocore_attachout(0x08e7, emm_o08e1);
		iocore_attachout(0x08e9, emm_o08e9);
		iocore_attachinp(0x08e9, emm_i08e9);
	}
#endif
}


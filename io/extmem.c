#include	"compiler.h"
#include	"i286.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"


	BYTE	*extmemmng_ptr;
	UINT32	extmemmng_size;


void extmemmng_clear(void) {

	if (extmemmng_ptr) {
		free(extmemmng_ptr);
		extmemmng_ptr = NULL;
		extmemmng_size = 0;
	}
}

BOOL extmemmng_realloc(UINT megabytes) {

	megabytes <<= 20;
	if (megabytes != extmemmng_size) {
		extmemmng_clear();
		if (megabytes) {
			extmemmng_ptr = (BYTE *)malloc(megabytes + 16);
			if (extmemmng_ptr == NULL) {
				return(FAILURE);
			}
		}
		extmemmng_size = megabytes;
	}
	return(SUCCESS);
}


// ---- I/O

static void IOOUTCALL emm_o08e1(UINT port, BYTE dat) {

	UINT	pos;

	if (!(CPUTYPE & CPUTYPE_V30)) {
		pos = (port >> 1) & 3;
		if (!extmem.target) {							// ver0.28
			extmem.page[pos] = 0xffffffff;
			extmem.pageptr[pos] = mem + 0xc0000 + (pos << 14);
		}
		else if (extmem.target < extmem.maxmem) {		// ver0.28
			dat &= 0xfc;
			extmem.page[pos] = ((extmem.target-1) << 8) + dat;
			extmem.pageptr[pos] = extmemmng_ptr +
									(extmem.page[pos] << 12);
		}
	}
}

static void IOOUTCALL emm_o08e9(UINT port, BYTE dat) {

	if (!(CPUTYPE & CPUTYPE_V30)) {
		extmem.target = dat & 0x0f;
	}
	(void)port;
}

static BYTE IOINPCALL emm_i08e9(UINT port) {

	if ((!(CPUTYPE & CPUTYPE_V30)) &&
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

	if (usemem > 13) {											// ver0.28
		usemem = 13;
	}
	extmem.target = 0;
	extmem.maxmem = 0;
	for (i=0; i<4; i++) {
		extmem.page[i] = 0xffffffff;
		extmem.pageptr[i] = mem + 0xc0000 + (i << 14);			// ver0.28
	}
	if (extmemmng_realloc(usemem)) {
		return(FAILURE);
	}
	extmem.maxmem = usemem + 1;
	return(SUCCESS);
}

void extmem_reset(void) {
}

void extmem_bind(void) {

	iocore_attachout(0x08e1, emm_o08e1);
	iocore_attachout(0x08e3, emm_o08e1);
	iocore_attachout(0x08e5, emm_o08e1);
	iocore_attachout(0x08e7, emm_o08e1);
	iocore_attachout(0x08e9, emm_o08e9);
	iocore_attachinp(0x08e9, emm_i08e9);
}


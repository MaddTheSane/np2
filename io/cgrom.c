#include	"compiler.h"
#include	"dosio.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"memory.h"
#include	"font.h"


static void cgwindowset(CGROM cr) {

	int		code;

	cgwindow.low = 0x7fff0;
	cgwindow.writable &= ~1;
	if (grcg.chip >= 2) {
		if (!(cr->code & 0xff00)) {
			cgwindow.high = 0x80000 + (cr->code << 4);
		}
		else {
			code = cr->code & 0x007f;
			cgwindow.high = (cr->code & 0x7f7f) << 4;
			if ((code >= 0x56) && (code < 0x58)) {
				cgwindow.writable |= 1;
				cgwindow.high += cr->lr;
			}
			else if (((code >= 0x0c) && (code < 0x10)) ||
				((code >= 0x58) && (code < 0x60))) {
				cgwindow.high += cr->lr;
			}
			else if ((code < 0x08) || (code >= 0x10)) {
				cgwindow.low = cgwindow.high;
				cgwindow.high += 0x800;
			}
		}
	}
	else {
		cgwindow.high = cgwindow.low;
	}
}


// ---- I/O

// write charactor code low
static void IOOUTCALL cgrom_oa1(UINT port, BYTE dat) {

	CGROM	cr;

	cr = &cgrom;
	cr->code = (dat << 8) | (cr->code & 0xff);
	cgwindowset(cr);
	(void)port;
}

// write charactor code high
static void IOOUTCALL cgrom_oa3(UINT port, BYTE dat) {

	CGROM	cr;

	cr = &cgrom;
	cr->code = (cr->code & 0xff00) | dat;
	cgwindowset(cr);
	(void)port;
}

// write charactor line
static void IOOUTCALL cgrom_oa5(UINT port, BYTE dat) {

	CGROM	cr;

	cr = &cgrom;
	cr->line = dat & 0x0f;
	cr->lr = ((~dat) & 0x20) << 6;
	cgwindowset(cr);
	(void)port;
}

// CG write pattern
static void IOOUTCALL cgrom_oa9(UINT port, BYTE dat) {

	CGROM	cr;

	cr = &cgrom;
	if ((cr->code & 0x007e) == 0x0056) {
		font[((cr->code & 0x7f7f) << 4) +
							cr->lr + cr->line] = dat;
		cgwindow.writable |= 0x80;
	}
	(void)port;
}

static BYTE IOINPCALL cgrom_ia9(UINT port) {

	CGROM	cr;
	BYTE	*ptr;

	cr = &cgrom;
	ptr = font;
	if (cr->code & 0xff00) {
		ptr += (cr->code & 0x7f7f) << 4;
		ptr += cr->lr;
	}
	else {
		ptr += 0x80000;
		ptr += cr->code << 4;
	}
	(void)port;
	return(ptr[cr->line]);
}


// ---- I/F

static const IOOUT cgromoa1[8] = {
					cgrom_oa1,	cgrom_oa3,	cgrom_oa5,	NULL,
					cgrom_oa9,	NULL,		NULL,		NULL};

static const IOINP cgromia1[8] = {
					NULL,		NULL,		NULL,		NULL,
					cgrom_ia9,	NULL,		NULL,		NULL};

void cgrom_reset(void) {

	CGWINDOW	cgw;

	cgw = &cgwindow;
	ZeroMemory(cgw, sizeof(cgrom));
	cgw->low = 0x7fff0;
	cgw->high = 0x7fff0;
	cgw->writable = 0;
}

void cgrom_bind(void) {

	iocore_attachsysoutex(0x00a1, 0x0cf1, cgromoa1, 8);
	iocore_attachsysinpex(0x00a1, 0x0cf1, cgromia1, 8);
}


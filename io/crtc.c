#include	"compiler.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"vram.h"
#include	"maketext.h"


// ---- I/O

static void IOOUTCALL crtc_o70(UINT port, BYTE dat) {

	port = (port & 0x0e) >> 1;
	dat &= 0x1f;
	if (crtc.b[port] != dat) {
		crtc.b[port] = dat;
		gdcs.textdisp |= GDCSCRN_ALLDRAW;
	}
}

static void IOOUTCALL crtc_o7c(UINT port, BYTE dat) {

	if (grcg.chip) {
		grcg.modereg = dat;
		grcg.counter = 0;
		vramop.operate &= VOP_GRCGMASK;
		vramop.operate |= ((dat & 0xc0) >> 4);
		if (grcg.chip >= 2) {
			grcg.gdcwithgrcg = (dat >> 4) & 0x0c;
		}
		i286_vram_dispatch(vramop.operate);
	}
	(void)port;
}

static void IOOUTCALL crtc_o7e(UINT port, BYTE dat) {

	int		cnt;

	cnt = grcg.counter;
	grcg.tile[cnt].b[0] = dat;
	grcg.tile[cnt].b[1] = dat;
	grcg.counter = (cnt + 1) & 3;
	(void)port;
}

static BYTE IOINPCALL crtc_i7c(UINT port) {

	(void)port;
	return(grcg.modereg);
}


// ---- I/F

static const IOOUT crtco70[8] = {
				crtc_o70,	crtc_o70,	crtc_o70,	crtc_o70,
				crtc_o70,	crtc_o70,	crtc_o7c,	crtc_o7e};

static const IOINP crtci70[8] = {
				NULL,		NULL,		NULL,		NULL,
				NULL,		NULL,		crtc_i7c,	NULL};

void crtc_reset(void) {

	ZeroMemory(&grcg, sizeof(grcg));
	ZeroMemory(&crtc, sizeof(crtc));
	crtc.reg.pl = 0x00;
	crtc.reg.bl = 0x0f;
	crtc.reg.cl = 0x10;
	grcg.chip = np2cfg.grcg & 3;			// GRCGìÆçÏÇÃÉRÉsÅ[
}

void crtc_bind(void) {

	iocore_attachsysoutex(0x0070, 0x0cf1, crtco70, 8);
	iocore_attachsysinpex(0x0070, 0x0cf1, crtci70, 8);
}


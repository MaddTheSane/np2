#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"cs4231.h"


void DMACCALL dma_dummyout(BYTE data) {

	(void)data;
}

BYTE DMACCALL dma_dummyin(void) {

	return(0xff);
}

BYTE DMACCALL dma_dummyproc(BYTE func) {

	(void)func;
	return(0);
}


// ----

void dmac_check(void) {

	BOOL	workchg;
	DMACH	ch;
	BYTE	bit;

	workchg = FALSE;
	ch = dmac.dmach;
	bit = 1;
	do {
		if ((!(dmac.mask & bit)) && (ch->ready)) {
			if (!(dmac.work & bit)) {
				dmac.work |= bit;
				if (ch->extproc(DMAEXT_START)) {
					dmac.stat &= ~bit;						// ver0.27
					dmac.working |= bit;
					workchg = TRUE;
				}
			}
		}
		else {
			if (dmac.work & bit) {
				dmac.work &= ~bit;
				dmac.working &= ~bit;
				ch->extproc(DMAEXT_BREAK);
				workchg = TRUE;
			}
		}
		bit <<= 1;
		ch++;
	} while(bit & 0x0f);
	if (workchg) {
		nevent_forceexit();
	}
}


// ---- I/O

static void IOOUTCALL dmac_o01(UINT port, BYTE dat) {

	DMACH	dmach;
	int		lh;

	dmach = dmac.dmach + ((port >> 2) & 3);
	lh = dmac.lh;
	dmac.lh = lh ^ 1;
	dmach->adrs.b[lh + DMA32_LOW] = dat;
	dmach->adrsorg.b[lh] = dat;
}

static void IOOUTCALL dmac_o03(UINT port, BYTE dat) {

	int		ch;
	DMACH	dmach;
	int		lh;

	ch = (port >> 2) & 3;
	dmach = dmac.dmach + ch;
	lh = dmac.lh;
	dmac.lh = lh ^ 1;
	dmach->leng.b[lh] = dat;
	dmach->lengorg.b[lh] = dat;
	dmac.stat &= ~(1 << ch);
}

static void IOOUTCALL dmac_o13(UINT port, BYTE dat) {

	dmac.dmach[dat & 3].sreq = dat;
	(void)port;
}

static void IOOUTCALL dmac_o15(UINT port, BYTE dat) {

	if (dat & 4) {
		dmac.mask |= (1 << (dat & 3));
	}
	else {
		dmac.mask &= ~(1 << (dat & 3));
	}
	dmac_check();
	(void)port;
}

static void IOOUTCALL dmac_o17(UINT port, BYTE dat) {

	dmac.dmach[dat & 3].mode = dat;
	(void)port;
}

static void IOOUTCALL dmac_o19(UINT port, BYTE dat) {

	dmac.lh = DMA16_LOW;
	(void)port;
	(void)dat;
}

static void IOOUTCALL dmac_o1b(UINT port, BYTE dat) {

	dmac.mask = 0x0f;
	(void)port;
	(void)dat;
}

static void IOOUTCALL dmac_o1f(UINT port, BYTE dat) {

	dmac.mask = dat;
	dmac_check();
	(void)port;
}

static void IOOUTCALL dmac_o21(UINT port, BYTE dat) {

	DMACH	dmach;

	dmach = dmac.dmach + (((port >> 1) + 1) & 3);
	dmach->adrs.b[DMA32_HIGH + DMA16_LOW] = dat;
}

static BYTE IOINPCALL dmac_i01(UINT port) {

	DMACH	dmach;
	int		lh;

	dmach = dmac.dmach + ((port >> 2) & 3);
	lh = dmac.lh;
	dmac.lh = lh ^ 1;
	return(dmach->leng.b[lh]);
}

static BYTE IOINPCALL dmac_i03(UINT port) {

	DMACH	dmach;
	int		lh;

	dmach = dmac.dmach + ((port >> 2) & 3);
	lh = dmac.lh;
	dmac.lh = lh ^ 1;
	return(dmach->adrs.b[lh + DMA32_LOW]);
}

static BYTE IOINPCALL dmac_i11(UINT port) {

	(void)port;
	return(dmac.stat);												// ToDo!!
}


// ---- I/F

static const IOOUT dmaco00[16] = {
					dmac_o01,	dmac_o03,	dmac_o01,	dmac_o03,
					dmac_o01,	dmac_o03,	dmac_o01,	dmac_o03,
					NULL,		dmac_o13,	dmac_o15,	dmac_o17,
					dmac_o19,	dmac_o1b,	NULL,		dmac_o1f};

static const IOINP dmaci00[16] = {
					dmac_i01,	dmac_i03,	dmac_i01,	dmac_i03,
					dmac_i01,	dmac_i03,	dmac_i01,	dmac_i03,
					dmac_i11,	NULL,		NULL,		NULL,
					NULL,		NULL,		NULL,		NULL};

static const IOOUT dmaco21[4] = {
					dmac_o21,	dmac_o21,	dmac_o21,	dmac_o21};

void dmac_reset(void) {

	int		i;

	ZeroMemory(&dmac, sizeof(dmac));
	dmac.lh = DMA16_LOW;
	dmac.mask = 0xf;
	for (i=0; i<4; i++) {
		dmac.dmach[i].outproc = dma_dummyout;
		dmac.dmach[i].inproc = dma_dummyin;
		dmac.dmach[i].extproc = dma_dummyproc;
	}
	dmac.dmach[0].extproc = cs4231dmafunc;
	dmac.dmach[DMA_2HD].inproc = fdc_DataRegRead;
	dmac.dmach[DMA_2HD].outproc = fdc_DataRegWrite;
	dmac.dmach[DMA_2HD].extproc = fdc_dmafunc;
	dmac.dmach[DMA_2DD].inproc = fdc_DataRegRead;
	dmac.dmach[DMA_2DD].outproc = fdc_DataRegWrite;
	dmac.dmach[DMA_2DD].extproc = fdc_dmafunc;

//	TRACEOUT(("sizeof(_DMACH) = %d", sizeof(_DMACH)));
}

void dmac_bind(void) {

	iocore_attachsysoutex(0x0001, 0x0ce1, dmaco00, 16);
	iocore_attachsysinpex(0x0001, 0x0ce1, dmaci00, 16);
	iocore_attachsysoutex(0x0021, 0x0cf1, dmaco21, 4);
}


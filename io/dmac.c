#include	"compiler.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"sound.h"
#include	"cs4231.h"


void DMACCALL dma_dummyout(REG8 data) {

	(void)data;
}

REG8 DMACCALL dma_dummyin(void) {

	return(0xff);
}

REG8 DMACCALL dma_dummyproc(REG8 func) {

	(void)func;
	return(0);
}

static const DMAPROC dmaproc[] = {
		{dma_dummyout,		dma_dummyin,		dma_dummyproc},		// NONE
		{fdc_DataRegWrite,	fdc_DataRegRead,	fdc_dmafunc},		// 2HD
		{fdc_DataRegWrite,	fdc_DataRegRead,	fdc_dmafunc},		// 2DD
		{dma_dummyout,		dma_dummyin,		dma_dummyproc},		// SASI
		{dma_dummyout,		dma_dummyin,		dma_dummyproc},		// SCSI
		{dma_dummyout,		dma_dummyin,		cs4231dmafunc},		// CS4231
};


// ----

void dmac_check(void) {

	BOOL	workchg;
	DMACH	ch;
	REG8	bit;

	workchg = FALSE;
	ch = dmac.dmach;
	bit = 1;
	do {
		if ((!(dmac.mask & bit)) && (ch->ready)) {
			if (!(dmac.work & bit)) {
				dmac.work |= bit;
				if (ch->proc.extproc(DMAEXT_START)) {
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
				ch->proc.extproc(DMAEXT_BREAK);
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

static void IOOUTCALL dmac_o01(UINT port, REG8 dat) {

	DMACH	dmach;
	int		lh;

	dmach = dmac.dmach + ((port >> 2) & 3);
	lh = dmac.lh;
	dmac.lh = lh ^ 1;
	dmach->adrs.b[lh + DMA32_LOW] = dat;
	dmach->adrsorg.b[lh] = dat;
}

static void IOOUTCALL dmac_o03(UINT port, REG8 dat) {

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

static void IOOUTCALL dmac_o13(UINT port, REG8 dat) {

	dmac.dmach[dat & 3].sreq = dat;
	(void)port;
}

static void IOOUTCALL dmac_o15(UINT port, REG8 dat) {

	if (dat & 4) {
		dmac.mask |= (1 << (dat & 3));
	}
	else {
		dmac.mask &= ~(1 << (dat & 3));
	}
	dmac_check();
	(void)port;
}

static void IOOUTCALL dmac_o17(UINT port, REG8 dat) {

	dmac.dmach[dat & 3].mode = dat;
	(void)port;
}

static void IOOUTCALL dmac_o19(UINT port, REG8 dat) {

	dmac.lh = DMA16_LOW;
	(void)port;
	(void)dat;
}

static void IOOUTCALL dmac_o1b(UINT port, REG8 dat) {

	dmac.mask = 0x0f;
	(void)port;
	(void)dat;
}

static void IOOUTCALL dmac_o1f(UINT port, REG8 dat) {

	dmac.mask = dat;
	dmac_check();
	(void)port;
}

static void IOOUTCALL dmac_o21(UINT port, REG8 dat) {

	DMACH	dmach;

	dmach = dmac.dmach + (((port >> 1) + 1) & 3);
	dmach->adrs.b[DMA32_HIGH + DMA16_LOW] = dat & 0x0f;
}

static REG8 IOINPCALL dmac_i01(UINT port) {

	DMACH	dmach;
	int		lh;

	dmach = dmac.dmach + ((port >> 2) & 3);
	lh = dmac.lh;
	dmac.lh = lh ^ 1;
	return(dmach->adrs.b[lh + DMA32_LOW]);
}

static REG8 IOINPCALL dmac_i03(UINT port) {

	DMACH	dmach;
	int		lh;

	dmach = dmac.dmach + ((port >> 2) & 3);
	lh = dmac.lh;
	dmac.lh = lh ^ 1;
	return(dmach->leng.b[lh]);
}

static REG8 IOINPCALL dmac_i11(UINT port) {

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

	ZeroMemory(&dmac, sizeof(dmac));
	dmac.lh = DMA16_LOW;
	dmac.mask = 0xf;
	dmac_procset();
//	TRACEOUT(("sizeof(_DMACH) = %d", sizeof(_DMACH)));
}

void dmac_bind(void) {

	iocore_attachsysoutex(0x0001, 0x0ce1, dmaco00, 16);
	iocore_attachsysinpex(0x0001, 0x0ce1, dmaci00, 16);
	iocore_attachsysoutex(0x0021, 0x0cf1, dmaco21, 4);
}


// ----

static void dmacset(REG8 channel) {

	DMADEV		*dev;
	DMADEV		*devterm;
	UINT		dmadev;

	dev = dmac.device;
	devterm = dev + dmac.devices;
	dmadev = DMADEV_NONE;
	while(dev < devterm) {
		if (dev->channel == channel) {
			dmadev = dev->device;
		}
		dev++;
	}
	if (dmadev >= sizeof(dmaproc) / sizeof(DMAPROC)) {
		dmadev = 0;
	}
	dmac.dmach[channel].proc = dmaproc[dmadev];
}

void dmac_procset(void) {

	REG8	i;

	for (i=0; i<4; i++) {
		dmacset(i);
	}
}

void dmac_attach(REG8 device, REG8 channel) {

	dmac_detach(device);

	if (dmac.devices < (sizeof(dmac.device) / sizeof(DMADEV))) {
		dmac.device[dmac.devices].device = device;
		dmac.device[dmac.devices].channel = channel;
		dmac.devices++;
		dmacset(channel);
	}
}

void dmac_detach(REG8 device) {

	DMADEV	*dev;
	DMADEV	*devterm;
	REG8	ch;

	dev = dmac.device;
	devterm = dev + dmac.devices;
	while(dev < devterm) {
		if (dev->device == device) {
			break;
		}
		dev++;
	}
	if (dev < devterm) {
		ch = dev->channel;
		dev++;
		while(dev < devterm) {
			*(dev - 1) = *dev;
			dev++;
		}
		dmac.devices--;
		dmacset(ch);
	}
}


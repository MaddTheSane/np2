#include	"compiler.h"

// ‚±‚êAscsicmd‚Æ‚Ç‚¤“‡‚·‚é‚Ì‚æH

#if defined(SUPPORT_IDEIO)

#include	"dosio.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"ideio.h"
#include	"atapicmd.h"
#include	"sxsi.h"


// INQUIRY
static const UINT8 cdrom_inquiry[] = {
	0x05,	// CD-ROM
	0x80,	// bit7: Removable Medium Bit, other: Reserved
	0x00,	// version [7-6: ISO, ECMA: 5-3, 2-0: ANSI(00)]
	0x21,	// 7-4: ATAPI version, 3-0: Response Data Format
	0x1f,	// Additional length
	0x00,0x00,0x00,	// Reserved
	'N', 'E', 'C', ' ', ' ', ' ', ' ', ' ',	// Vendor ID
	'C', 'D', '-', 'R', 'O', 'M', ' ', 'D',	// Product ID
	'R', 'I', 'V', 'E', ' ', ' ', ' ', ' ',	// Product ID
	'1', '.', '0', ' '	// Product Revision Level
};

static void senddata(IDEDRV drv, UINT size, UINT limit) {

	size = min(size, limit);
	drv->sc = IDEINTR_IO;
	drv->cy = size;
	drv->status &= ~IDESTAT_BSY;
	drv->status |= IDESTAT_DRQ;
	drv->error = 0;
	drv->bufdir = IDEDIR_IN;
	drv->bufpos = 0;
	drv->bufsize = size;

	ideio.bank[0] = ideio.bank[1] | 0x80;			// ????
	pic_setirq(IDE_IRQ);
}


// ----- ATAPI packet command

void atapicmd_a0(IDEDRV drv) {

	UINT8	cmd;
	UINT	leng;

	cmd = drv->buf[0];
	switch(cmd) {
		case 0x12:		// inquiry
			TRACEOUT(("atapicmd: inquiry"));
			leng = drv->buf[4];
			CopyMemory(drv->buf, cdrom_inquiry, sizeof(cdrom_inquiry));
			senddata(drv, sizeof(cdrom_inquiry), leng);
			break;

//		case 0x5a:		// mode sense(10)
//			break;

		default:
			TRACEOUT(("atapicmd: unknown command = %.2x", cmd));
//			sendabort(drv);
			break;
	}
}

#endif	/* SUPPORT_IDEIO */


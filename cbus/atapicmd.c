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


static const UINT8 cdrom_inquiry[] = {
				0x05,0x80,0x00,0x21,0x1f,0x00,0x00,0x00,
				'N', 'E', 'C', 0x20,0x20,0x20,0x20,0x20,
				'C', 'D', '-', 'R', 'O', 'M', ' ', 'D',
				'R', 'I', 'V', 'E', 0x20,0x20,0x20,0x20,
				'1', '.', '0', ' '};

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


void atapicmd_a0(IDEDRV drv) {

	UINT8	cmd;
	UINT	leng;

	cmd = drv->buf[0];
	switch(cmd) {
		case 0x12:		// inquiry
			leng = drv->buf[4];
			CopyMemory(drv->buf, cdrom_inquiry, sizeof(cdrom_inquiry));
			senddata(drv, sizeof(cdrom_inquiry), leng);
			break;

		case 0x5a:		// mode sense(10)
			break;
	}
}

#endif


#include	"compiler.h"
#include	"memory.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"dmap.h"


void dmap_i286(void) {

	DMACH	ch;
	int		bit;

	if (dmac.working) {
		ch = dmac.dmach;
		bit = 1;
		do {
			if (dmac.working & bit) {
				// DMA working !
				if (!ch->leng.w) {
					dmac.stat |= bit;
					dmac.working &= ~bit;
					ch->extproc(DMAEXT_END);
				}
				ch->leng.w--;

				switch(ch->mode & 0x0c) {
					case 0x00:		// verifty
						ch->inproc();
						break;

					case 0x04:		// port->mem
						i286_memorywrite(ch->adrs.d, ch->inproc());
						break;

					default:
						ch->outproc(i286_memoryread(ch->adrs.d));
						break;
				}
				ch->adrs.d += (((ch->mode) & 0x20)?-1:1);
			}
			ch++;
			bit <<= 1;
		} while(bit & 0x0f);
	}
}

